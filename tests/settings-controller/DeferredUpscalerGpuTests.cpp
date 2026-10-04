#include "PCH.h"
#include "FrameGen/NvidiaHost.h"
#include "DLSSBackend.h"
#include "../nr-postsr/GpuProbeLifetime.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <thread>
#include <chrono>
#include <stdexcept>
using Microsoft::WRL::ComPtr;
using namespace TheosRenderPipeline;
void Need(HRESULT h){if(FAILED(h))throw std::runtime_error("D3D call failed");}
void Require(bool v,const char* message){if(!v)throw std::runtime_error(message);}
struct PendingCopy {
    ComPtr<ID3D12Device> device;ComPtr<ID3D12CommandQueue> queue;
    ComPtr<ID3D12CommandAllocator> allocator;ComPtr<ID3D12GraphicsCommandList> list;
    ComPtr<ID3D12Resource> source,readback;ComPtr<ID3D12Fence> gate,done;
    HANDLE event{CreateEventW(nullptr,FALSE,FALSE,nullptr)};
    ~PendingCopy(){if(event)CloseHandle(event);}
    bool Wait(){if(done->GetCompletedValue()==UINT64_MAX)return false;if(done->GetCompletedValue()>=1)return true;
        if(FAILED(done->SetEventOnCompletion(1,event)))return false;
        return WaitForSingleObject(event,5000)==WAIT_OBJECT_0&&done->GetCompletedValue()!=UINT64_MAX&&done->GetCompletedValue()>=1;}
};
int main(int argc,char** argv){try{
    if(argc>2||(argc==2&&std::string_view(argv[1])!="--omit-wait"))return 1;
    if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
    const bool omit=argc==2&&std::string_view(argv[1])=="--omit-wait";
    auto resources=std::make_unique<PendingCopy>();auto* r=resources.get();Require(r->event!=nullptr,"completion event exists");
    ComPtr<IDXGIFactory6> factory;Need(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));
    ComPtr<IDXGIAdapter1> adapter;
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> candidate;const auto h=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));if(h==DXGI_ERROR_NOT_FOUND)break;Need(h);
        DXGI_ADAPTER_DESC1 desc{};Need(candidate->GetDesc1(&desc));if(!(desc.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)){adapter=candidate;break;}}
    if(!adapter)return 77;
    Need(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&r->device)));
    D3D12_COMMAND_QUEUE_DESC q{};Need(r->device->CreateCommandQueue(&q,IID_PPV_ARGS(&r->queue)));
    Need(r->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&r->allocator)));
    Need(r->device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,r->allocator.Get(),nullptr,IID_PPV_ARGS(&r->list)));
    Need(r->device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&r->gate)));
    Need(r->device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&r->done)));
    D3D12_RESOURCE_DESC d{};d.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;d.Width=4096;d.Height=d.DepthOrArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_UPLOAD;
    Need(r->device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_GENERIC_READ,nullptr,IID_PPV_ARGS(&r->source)));
    heap.Type=D3D12_HEAP_TYPE_READBACK;
    Need(r->device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&r->readback)));
    unsigned char* mapped{};D3D12_RANGE empty{};Need(r->source->Map(0,&empty,reinterpret_cast<void**>(&mapped)));
    for(unsigned i=0;i<4096;++i)mapped[i]=static_cast<unsigned char>(i*7+11);r->source->Unmap(0,nullptr);
    r->list->CopyBufferRegion(r->readback.Get(),0,r->source.Get(),0,4096);Need(r->list->Close());
    // CPU release is running before any GPU queue can wait on it.
    std::jthread release([gate=r->gate]{std::this_thread::sleep_for(std::chrono::milliseconds(500));gate->Signal(1);});
    GpuProbeLifetime<PendingCopy> lifetime(std::move(resources),[r]{return r->Wait();});
    Need(r->queue->Wait(r->gate.Get(),1));ID3D12CommandList* commands[]{r->list.Get()};r->queue->ExecuteCommandLists(1,commands);Need(r->queue->Signal(r->done.Get(),1));
    Require(r->done->GetCompletedValue()==0,"actual old-source reader is pending before feature request");
    NvidiaHost host;auto& backend=SourceDLSSG::Backend::Get();backend.events.clear();unsigned exact{},created{};
    backend.retire=[r,omit]{return omit||r->Wait();};
    DLSSBackend::GetSingleton()->create=[r,&exact,&created]{++created;
        if(r->done->GetCompletedValue()!=1)return false;
        void* data{};D3D12_RANGE range{0,4096};if(FAILED(r->readback->Map(0,&range,&data)))return false;
        const auto* bytes=static_cast<const unsigned char*>(data);for(unsigned i=0;i<4096;++i)exact+=bytes[i]==static_cast<unsigned char>(i*7+11);D3D12_RANGE written{};r->readback->Unmap(0,&written);return exact==4096;
    };
    struct ResetCallbacks {~ResetCallbacks(){SourceDLSSG::Backend::Get().retire=[] {return true;};DLSSBackend::GetSingleton()->create=[] {return true;};}} reset;
    auto request=host.configuration.Requested();request.preset=12;host.RequestSourceUpscalerSettings(request);
    Require(r->done->GetCompletedValue()==0&&created==0,"request does not retire or recreate pending source");
    host.nativeUIPass_.active=true;host.ApplySourceUpscalerSettingsAfterPresent();
    Require(r->done->GetCompletedValue()==0&&backend.events.empty(),"UI-pass deferral leaves real old source pending");
    host.nativeUIPass_.active=false;const auto begin=std::chrono::steady_clock::now();host.ApplySourceUpscalerSettingsAfterPresent();
    Require(!host.configuration.Failed()&&host.configuration.Effective().preset==12&&exact==4096,"actual deferred feature creation follows completion and exact old-source pixels");
    Require(backend.events==std::vector<std::string>{"retire","create","resume"}&&created==1,"pending deferred path has one ordered recreation");
    Require(std::chrono::steady_clock::now()-begin>=std::chrono::milliseconds(100)&&r->done->GetCompletedValue()==1,"deferred apply actually waits for old GPU source");
    Require(lifetime.Retire(),"all submitted probe resources retire safely");
    std::cout<<"DEFERRED_GPU pending=1 exactBytes="<<exact<<" creates="<<created<<" retired=1 vendorBoundary=facade\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
