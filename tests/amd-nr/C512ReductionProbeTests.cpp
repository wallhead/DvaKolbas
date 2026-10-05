#include "NeuralRendering/Amd/C512ReductionProbe.h"
#include "NeuralRendering/Amd/C512Reduction.h"
#include "TestSupport.h"
#include "../../tools/AmdNrDevices.h"
#include <d3d12sdklayers.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <array>
#include <random>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using AmdNrTest::Require;
using AmdNrTools::ComPtr;
namespace {
struct GpuContext {
    ComPtr<ID3D12Device> device;ComPtr<ID3D12CommandQueue> queue;ComPtr<ID3D12InfoQueue> info;
    GpuContext(bool warp,LUID luid,bool debug=false) {
        auto adapter=AmdNrTools::SelectAdapter(warp,luid);AmdNrTools::Describe(adapter.Get());
        AmdNrTools::CheckDeviceApi(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&device)));
        D3D12_COMMAND_QUEUE_DESC desc{};AmdNrTools::CheckDeviceApi(device->CreateCommandQueue(&desc,IID_PPV_ARGS(&queue)));
        if(debug) AmdNrTools::CheckDeviceApi(device.As(&info));
    }
    void CheckMessages() {
        if(info) for(UINT64 i=0;i<info->GetNumStoredMessagesAllowedByRetrievalFilter();++i) {
            SIZE_T size{};info->GetMessage(i,nullptr,&size);std::vector<std::byte> bytes(size);
            auto message=reinterpret_cast<D3D12_MESSAGE*>(bytes.data());AmdNrTools::CheckDeviceApi(info->GetMessage(i,message,&size));
            if(message->Severity<=D3D12_MESSAGE_SEVERITY_WARNING) {std::fprintf(stderr,"D3D12: %s\n",message->pDescription);Require(false,"validation warning/error");}
        }
    }
};
std::vector<std::uint8_t> Cpu(const C512TensorLayout& src,const C512TensorLayout& dst,
    std::span<const std::uint16_t> input,std::span<const std::uint8_t> initial) {
    std::vector<std::uint8_t> result(initial.begin(),initial.end());Require(bool(ReduceC512Half2x2(src,dst,input,result)),"CPU reduction");return result;
}
void GpuTests(bool warp,LUID luid,bool debug) {
    GpuContext context(warp,luid,debug);auto probe=C512ReductionProbe::Create(context.device.Get(),context.queue.Get());Require(bool(probe),"reduction probe");
    Require(!C512ReductionProbe::Create(nullptr,context.queue.Get()) && !C512ReductionProbe::Create(context.device.Get(),nullptr),"null device/queue");
    D3D12_COMMAND_QUEUE_DESC desc{};desc.Type=D3D12_COMMAND_LIST_TYPE_COMPUTE;ComPtr<ID3D12CommandQueue> compute;
    AmdNrTools::CheckDeviceApi(context.device->CreateCommandQueue(&desc,IID_PPV_ARGS(&compute)));
    Require(!C512ReductionProbe::Create(context.device.Get(),compute.Get()),"compute queue rejected");
    auto factory=AmdNrTools::Factory();
    for(UINT i=0;;++i) {
        ComPtr<IDXGIAdapter1> adapter;if(factory->EnumAdapters1(i,&adapter)==DXGI_ERROR_NOT_FOUND) break;
        DXGI_ADAPTER_DESC1 d{};adapter->GetDesc1(&d);if(d.Flags&DXGI_ADAPTER_FLAG_SOFTWARE) continue;
        ComPtr<ID3D12Device> other;
        if(SUCCEEDED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&other))) && other.Get()!=context.device.Get()) {
            Require(!C512ReductionProbe::Create(other.Get(),context.queue.Get()),"foreign queue device");break;
        }
    }
    struct Pair {Extent src,dst;};
    constexpr Pair cases[]{{{4,4},{4,4}},{{8,16},{4,8}},{{60,36},{32,20}},{{16,12},{4,4}},{{64,64},{32,32}}};
    std::mt19937 random(20261005);
    for(auto dims:cases) {
        const auto src=MakeC512TensorLayout(dims.src).value(),dst=MakeC512TensorLayout(dims.dst).value();
        std::vector<std::uint16_t> input(src.ByteCount());std::vector<std::uint8_t> initial(dst.ByteCount());
        for(auto& v:input) v=std::uint16_t(random());for(auto& v:initial) v=std::uint8_t(random());
        const auto expected=Cpu(src,dst,input,initial);
        auto job=probe->Submit(src,dst,input,initial);Require(bool(job),"GPU reduction submit");
        std::fill(input.begin(),input.end(),0);std::fill(initial.begin(),initial.end(),0);
        const auto output=probe->Readback(*job,std::chrono::seconds(30));
        Require(bool(output) && *output==expected && output->size()==dst.ByteCount(),"all packed bytes and snapshots");
        Require(probe->Readback(*job,std::chrono::seconds(1)).value()==expected,"repeat readback");
        const auto width=std::min(dims.src.width/2,dims.dst.width),height=std::min(dims.src.height/2,dims.dst.height);
        const auto padding=(std::size_t(dims.dst.width)*dims.dst.height-std::size_t(width)*height)*512;
        if(dims.src==Extent{60,36}) Require(padding==51200,"exact padding count");
    }
    const auto l=MakeC512TensorLayout({4,4}).value();std::vector<std::uint16_t> input(l.ByteCount(),0x8000);
    std::vector<std::uint8_t> initial(l.ByteCount(),0x69);const auto expected=Cpu(l,l,input,initial);
    auto bad=probe->Submit(l,l,std::span(input).first(input.size()-1),initial);
    Require(!bad && bad.error()==ProbeError::Count,"short half span");
    Require(!probe->Submit(l,l,input,std::span(initial).first(initial.size()-1)),"short initial output");
    Require(!probe->Submit(MakeC512TensorLayout({68,64}).value(),l,input,initial),"source cap");
    Require(!probe->Submit(l,MakeC512TensorLayout({36,32}).value(),input,initial),"destination cap");
    {auto dropped=probe->Submit(l,l,input,initial);Require(bool(dropped),"dropped job");}
    Require(bool(probe->Drain(std::chrono::seconds(30))),"dropped resources retained");
    ComPtr<ID3D12Fence> gate;AmdNrTools::CheckDeviceApi(context.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)));
    AmdNrTools::CheckDeviceApi(context.queue->Wait(gate.Get(),1));
    auto job=probe->Submit(l,l,input,initial);Require(bool(job),"gated job");auto moved=std::move(*probe);
    Require(!probe->Submit(l,l,input,initial) && !probe->Readback(*job,std::chrono::milliseconds(0)),"moved-from context");
    auto timeout=moved.Readback(*job,std::chrono::milliseconds(0));AmdNrTools::CheckDeviceApi(gate->Signal(1));
    Require(!timeout && timeout.error()==ProbeError::Timeout,"gated timeout retains job");
    Require(moved.Readback(*job,std::chrono::seconds(30)).value()==expected,"moved pending job remains readable");
    auto foreign=C512ReductionProbe::Create(context.device.Get(),context.queue.Get());
    const auto invalid=foreign->Readback(*job,std::chrono::milliseconds(0));Require(!invalid && invalid.error()==ProbeError::InvalidJob,"foreign job");
    Require(bool(moved.Drain(std::chrono::seconds(30))),"gate drain");context.CheckMessages();
    std::puts("PASS: GPU C512 half reduction, packed bytes/padding, caps and lifecycle");
}
void FileOracle(bool cpu,bool warp,LUID luid,const wchar_t* sourcePath,const wchar_t* outputPath) {
    std::ifstream file(std::filesystem::path(sourcePath),std::ios::binary);std::array<std::uint8_t,16> header{};
    if(!file.read(reinterpret_cast<char*>(header.data()),header.size())) throw std::runtime_error("short header");
    auto get=[&](unsigned at){return std::uint32_t(header[at])|std::uint32_t(header[at+1])<<8|std::uint32_t(header[at+2])<<16|std::uint32_t(header[at+3])<<24;};
    auto src=MakeC512TensorLayout({get(0),get(4)}),dst=MakeC512TensorLayout({get(8),get(12)});
    if(!src || !dst || src->ByteCount()>C512ReductionMaxSourceValues || dst->ByteCount()>C512ReductionMaxDestinationBytes) throw std::runtime_error("extent/cap");
    std::vector<std::uint16_t> input(src->ByteCount());std::vector<std::uint8_t> initial(dst->ByteCount());
    if(!file.read(reinterpret_cast<char*>(input.data()),input.size()*2) || !file.read(reinterpret_cast<char*>(initial.data()),initial.size()) ||
        file.peek()!=std::char_traits<char>::eof()) throw std::runtime_error("fixture length");
    std::vector<std::uint8_t> output;
    if(cpu) output=Cpu(*src,*dst,input,initial);
    else {GpuContext context(warp,luid);auto probe=C512ReductionProbe::Create(context.device.Get(),context.queue.Get());
        if(!probe) throw std::runtime_error("probe");auto job=probe->Submit(*src,*dst,input,initial);
        if(!job) throw std::runtime_error("submit");output=probe->Readback(*job,std::chrono::seconds(30)).value();}
    std::ofstream result(std::filesystem::path(outputPath),std::ios::binary|std::ios::trunc);
    result.write(reinterpret_cast<const char*>(output.data()),output.size());result.close();if(!result) throw std::runtime_error("output write");
}
}
int wmain(int argc,wchar_t** argv) try {
    if(argc>=2 && std::wstring_view(argv[1])==L"--oracle") {
        if(argc==5 && std::wstring_view(argv[2])==L"--cpu") FileOracle(true,false,{},argv[3],argv[4]);
        else if(argc==5 && std::wstring_view(argv[2])==L"--warp") FileOracle(false,true,{},argv[3],argv[4]);
        else if(argc==6 && std::wstring_view(argv[2])==L"--adapter-luid") FileOracle(false,false,AmdNrTools::ParseLuid(argv[3]),argv[4],argv[5]);
        else throw std::runtime_error("oracle arguments");return 0;
    }
    const bool warp=argc>=2 && std::wstring_view(argv[1])==L"--warp",debug=argc==3 && std::wstring_view(argv[2])==L"--debug";
    if(!(warp && (argc==2 || debug)) && !(argc==3 && std::wstring_view(argv[1])==L"--adapter-luid")) throw std::runtime_error("arguments");
    ComPtr<ID3D12Debug> layer;
    if(debug && FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&layer)))) {std::puts("UNAVAILABLE: D3D12 debug layer");return 77;}
    if(layer) layer->EnableDebugLayer();GpuTests(warp,warp?LUID{}:AmdNrTools::ParseLuid(argv[2]),debug);return 0;
} catch(const std::exception& e) {std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}
