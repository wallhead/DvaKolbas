#include "NeuralRendering/Amd/FormatProbe.h"
#include "TestSupport.h"
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <bit>
#include <random>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using AmdNrTest::Require;
using Microsoft::WRL::ComPtr;
namespace TheosRenderPipeline::NeuralRendering::Amd {
struct FormatProbeTestAccess {
    static void FailRetirement(FormatProbe& probe);
    static std::size_t RetainedContexts();
    static std::size_t RetainedJobs();
};
}
int main(int argc,char**) {
    bool validated=argc==1;
    ComPtr<ID3D12Debug> debug;
    if(validated && FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {std::puts("UNAVAILABLE: D3D12 debug layer (install Windows Graphics Tools)");return 77;}
    if(debug) debug->EnableDebugLayer();
    ComPtr<IDXGIFactory4> factory;Require(SUCCEEDED(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory))),"factory");
    ComPtr<IDXGIAdapter> adapter;Require(SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter))),"WARP");
    ComPtr<ID3D12Device> device;Require(SUCCEEDED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&device))),"device");
    D3D12_COMMAND_QUEUE_DESC desc{};desc.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
    ComPtr<ID3D12CommandQueue> queue;Require(SUCCEEDED(device->CreateCommandQueue(&desc,IID_PPV_ARGS(&queue))),"queue");
    ComPtr<ID3D12InfoQueue> info;if(validated) Require(SUCCEEDED(device.As(&info)),"validation messages");
    auto probe=FormatProbe::Create(device.Get(),queue.Get());Require(bool(probe),"probe creation");
    auto empty=probe->Submit(FormatOperation::E4m3ToHalf,{});Require(bool(empty) && probe->Readback(*empty,std::chrono::milliseconds(0))->empty(),"empty job complete");
    Require(!probe->Submit(static_cast<FormatOperation>(99),{}),"invalid operation");
    std::vector<std::uint32_t> excessive(65535*64+1);Require(!probe->Submit(FormatOperation::E4m3ToHalf,excessive),"dispatch count rejected");
    std::vector<std::uint32_t> half(65536),fp8(256),random(50003);
    for(unsigned i=0;i<half.size();++i) half[i]=0xffff0000|i;
    for(unsigned i=0;i<fp8.size();++i) fp8[i]=0xffff0000|i;
    std::mt19937 generator(12345);for(auto& v:random) v=generator();
    random.insert(random.end(),{0,0x80000000,0x33000000,0x33800000,0x387fc000,0x38800000,0x477fe000,0x477ff000,0x7f800001,0xff800001,0x7fc12345,
        1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,0x1a996262,0x9a996262,0x3a800000,0x3a800001,0x43e00000,0xc3e00000,0x7f800000,0xff800000});
    for(auto [op,input]:{std::pair{FormatOperation::HalfToFloat32,&half},std::pair{FormatOperation::E4m3ToHalf,&fp8},std::pair{FormatOperation::Float32ToHalf,&random},std::pair{FormatOperation::Float32ToE4m3,&random}}) {
        std::vector<std::uint32_t> expected(input->size());Require(bool(ConvertFormatWords(op,*input,expected)),"CPU reference");
        auto job=probe->Submit(op,*input);Require(bool(job),"submit");
        auto got=probe->Readback(*job,std::chrono::seconds(10));Require(bool(got) && *got==expected,"GPU format storage bits");
        Require(probe->Readback(*job,std::chrono::seconds(1)).value()==expected,"repeated readback");
    }
    for(unsigned count:{0u,1u,63u,64u,65u}) {
        const auto input=std::span<const std::uint32_t>(random).first(count);
        std::vector<std::uint32_t> expected(count);Require(bool(ConvertFormatWords(FormatOperation::Float32ToE4m3,input,expected)),"FP8 dispatch tail CPU reference");
        auto tail=probe->Submit(FormatOperation::Float32ToE4m3,input);Require(bool(tail),"FP8 dispatch tail submission");
        Require(probe->Readback(*tail,std::chrono::seconds(10)).value()==expected,"FP8 dispatch tail output length/bits");
    }
    // Keep jobs alive across another submission and release an unconsumed job.
    const std::uint32_t word=0x38;
    {auto job=probe->Submit(FormatOperation::E4m3ToHalf,std::span(&word,1));Require(bool(job),"unconsumed job submission");}
    auto job=probe->Submit(FormatOperation::E4m3ToHalf,std::span(&word,1));Require(bool(job),"repeated submission");
    Require(probe->Readback(*job,std::chrono::seconds(10)).value()==std::vector<std::uint32_t>{0x3c00},"tail count");
    Require(bool(probe->Drain(std::chrono::seconds(10))),"drain");
    ComPtr<IDXGIAdapter1> hardware;
    for(UINT i=0;SUCCEEDED(factory->EnumAdapters1(i,&hardware));++i) {
        DXGI_ADAPTER_DESC1 d{};hardware->GetDesc1(&d);if(d.Flags&DXGI_ADAPTER_FLAG_SOFTWARE){hardware.Reset();continue;}
        ComPtr<ID3D12Device> other;
        if(SUCCEEDED(D3D12CreateDevice(hardware.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&other)))) {Require(!FormatProbe::Create(other.Get(),queue.Get()),"wrong device queue rejected");break;}
        hardware.Reset();
    }
    ComPtr<ID3D12Fence> gate;Require(SUCCEEDED(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate))),"gate fence");
    Require(SUCCEEDED(queue->Wait(gate.Get(),1)),"gate queue");
    auto delayed=probe->Submit(FormatOperation::E4m3ToHalf,std::span(&word,1));Require(bool(delayed),"delayed submit");
    auto timeout=probe->Readback(*delayed,std::chrono::milliseconds(0));
    Require(!timeout && timeout.error()==ProbeError::Timeout,"timeout reported without freeing job");
    Require(SUCCEEDED(gate->Signal(1)),"release gate");
    Require(probe->Readback(*delayed,std::chrono::seconds(10)).value()==std::vector<std::uint32_t>{0x3c00},"timed out job remains readable");
    auto foreign=FormatProbe::Create(device.Get(),queue.Get());Require(bool(foreign) && !foreign->Readback(*delayed,std::chrono::milliseconds(0)),"foreign job rejected");
    desc.Type=D3D12_COMMAND_LIST_TYPE_COMPUTE;ComPtr<ID3D12CommandQueue> compute;
    Require(SUCCEEDED(device->CreateCommandQueue(&desc,IID_PPV_ARGS(&compute))),"compute queue");
    Require(!FormatProbe::Create(device.Get(),compute.Get()),"non-DIRECT queue rejected");
    auto retained=FormatProbeTestAccess::RetainedContexts();
    {auto fault=FormatProbe::Create(device.Get(),queue.Get());Require(bool(fault),"fault probe");
        auto completed=fault->Submit(FormatOperation::E4m3ToHalf,std::span(&word,1));
        Require(bool(completed) && bool(fault->Readback(*completed,std::chrono::seconds(10))),"fault work really retired");
        FormatProbeTestAccess::FailRetirement(*fault);
        Require(!fault->Drain(std::chrono::milliseconds(0)),"injected fence failure");}
    Require(FormatProbeTestAccess::RetainedContexts()==retained+1,"failed shutdown retains complete context");
    Require(FormatProbeTestAccess::RetainedJobs()==1,"retained context owns submitted buffers and commands");
    if(info) for(UINT64 i=0;i<info->GetNumStoredMessagesAllowedByRetrievalFilter();++i) {
        SIZE_T size{};info->GetMessage(i,nullptr,&size);std::vector<std::byte> storage(size);
        auto message=reinterpret_cast<D3D12_MESSAGE*>(storage.data());Require(SUCCEEDED(info->GetMessage(i,message,&size)),"message retrieval");
        if(message->Severity<=D3D12_MESSAGE_SEVERITY_WARNING) {std::fprintf(stderr,"D3D12: %s\n",message->pDescription);Require(false,"validation warning/error");}
    }
    std::puts(validated?"PASS: WARP GPU storage bits and D3D12 validation":"PASS: WARP GPU storage bits (debug validation not requested)");
}
