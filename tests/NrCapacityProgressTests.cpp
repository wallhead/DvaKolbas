#include "NeuralRendering/PreparedBeforeUpscale.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <thread>
#include <chrono>
#include <array>
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
void Need(bool value,const char* text){if(!value){std::printf("FAIL %s\n",text);ExitProcess(1);}}
void Gpu(HRESULT hr){Need(SUCCEEDED(hr),"GPU operation");}
template<class T>auto Take(T result){if(!result){std::puts(result.error().message.c_str());ExitProcess(1);}return std::move(*result);}
void Done(Result<void> result){if(!result){std::puts(result.error().message.c_str());ExitProcess(1);}}
template<class Adapter>void Exercise(ID3D11Device* device,ID3D11DeviceContext* context,const StageContract& c,std::shared_ptr<RuntimeOwner> owner){
    constexpr bool prepared=std::is_same_v<Adapter,PreparedBeforeUpscale>;
    Adapter a;Done(a.Initialize(owner,device,c));
    std::array<BeforeInput,4> inputs;std::array<ComPtr<ID3D12Fence>,3> gates,readers;std::array<ComPtr<ID3D12CommandQueue>,3> queues;
    SettingsSnapshot settings;settings.enabled=true;settings.revision=1;settings.tuning.localToneStrength=0;
    for(unsigned i=0;i<4;++i){auto& p=inputs[i];p.context=context;p.epoch=p.guideEpoch=1;p.sourceId=p.guideSourceId=i+1;p.previousSourceId=i;p.presentationTime=(i+1)/60.;p.colorExtent=p.guideExtent={320,180};p.colorDomain=ColorDomain::Linear;p.motionScaleX=320;p.motionScaleY=180;
        const auto make=[&](DXGI_FORMAT f){D3D11_TEXTURE2D_DESC d{};d.Width=320;d.Height=180;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=f;d.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;ComPtr<ID3D11Texture2D> t;Gpu(device->CreateTexture2D(&d,nullptr,&t));return t;};
        p.color=make(prepared?DXGI_FORMAT_R8G8B8A8_UNORM:DXGI_FORMAT_R16G16B16A16_FLOAT);p.depth=make(DXGI_FORMAT_R32_FLOAT);p.motion=make(DXGI_FORMAT_R16G16_FLOAT);
        std::vector<uint16_t> linear(320*180*4,0x3800),motion(320*180*2);std::vector<uint32_t> gamma(320*180,0xff804020);std::vector<float> depth(320*180,.5f);
        context->UpdateSubresource(p.color.Get(),0,nullptr,prepared?static_cast<void*>(gamma.data()):static_cast<void*>(linear.data()),320*(prepared?4:8),0);context->UpdateSubresource(p.depth.Get(),0,nullptr,depth.data(),320*4,0);context->UpdateSubresource(p.motion.Get(),0,nullptr,motion.data(),320*4,0);
    }
    const auto evaluate=[&](unsigned i){if constexpr(prepared)return a.Evaluate(inputs[i],TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,settings);else return a.Evaluate(inputs[i],settings);};
    D3D12_COMMAND_QUEUE_DESC q{};
    for(unsigned i=0;i<3;++i){Gpu(c.device->CreateCommandQueue(&q,IID_PPV_ARGS(&queues[i])));Gpu(c.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gates[i])));Gpu(c.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&readers[i])));Gpu(queues[i]->Wait(gates[i].Get(),1));Gpu(queues[i]->Signal(readers[i].Get(),1));auto r=Take(evaluate(i));Done(a.TrackReader(r.delivery,readers[i].Get(),1));}
    // Complete the real NR/copy/encode work; only the three independent readers remain.
    ComPtr<ID3D11Device5> d5;ComPtr<ID3D11DeviceContext4> c4;ComPtr<ID3D11Fence> done11;Gpu(device->QueryInterface(IID_PPV_ARGS(&d5)));Gpu(context->QueryInterface(IID_PPV_ARGS(&c4)));Gpu(d5->CreateFence(0,D3D11_FENCE_FLAG_NONE,IID_PPV_ARGS(&done11)));Gpu(c4->Signal(done11.Get(),1));context->Flush();HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Need(event!=nullptr,"event");Gpu(done11->SetEventOnCompletion(1,event));Need(WaitForSingleObject(event,5000)==WAIT_OBJECT_0,"actual NR delivery complete");CloseHandle(event);
    Need(Take(a.CollectCompleted())==0&&a.Diagnostics().pendingTickets==3,"independent readers retain three slots");
    auto release=std::thread([gates]{std::this_thread::sleep_for(std::chrono::milliseconds(250));Gpu(gates[1]->Signal(1));std::this_thread::sleep_for(std::chrono::milliseconds(1350));Gpu(gates[0]->Signal(1));Gpu(gates[2]->Signal(1));});
    auto start=std::chrono::steady_clock::now();auto fourth=evaluate(3);double elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
    std::printf("CAPACITY %s elapsedMs=%.3f oldestCompleted=%llu\n",prepared?"prepared":"before",elapsed,gates[0]->GetCompletedValue());Need(bool(fourth)&&elapsed>=100&&elapsed<900&&gates[0]->GetCompletedValue()==0,"LaterReaderCompletionWakesCapacityBeforeOldest");
    release.join();Done(a.Retire());Done(owner->Retire());
}
}
int wmain(int argc,wchar_t** argv){if(argc!=4)return 77;if(NrRuntimeResearch::GameRunningOrUnknown())return 1;setvbuf(stdout,nullptr,_IONBF,0);
    ComPtr<ID3D11Device> d;ComPtr<ID3D11DeviceContext> context;Gpu(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&d,nullptr,&context));ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;Gpu(d.As(&dxgi));Gpu(dxgi->GetAdapter(&adapter));DXGI_ADAPTER_DESC desc{};Gpu(adapter->GetDesc(&desc));if(desc.VendorId!=0x10de||desc.DeviceId!=0x2702)return 77;
    StageContract c;c.colorExtent=c.guideExtent={320,180};c.adapterLuid={desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart};Gpu(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&c.device)));D3D12_COMMAND_QUEUE_DESC q{};Gpu(c.device->CreateCommandQueue(&q,IID_PPV_ARGS(&c.queue)));
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-capacity-cache"),true});Done(owner->Open(RuntimeCatalog()[1],c.device.Get(),{desc.VendorId,desc.DeviceId,desc.SubSysId,c.adapterLuid,false}));
    if(std::wstring_view(argv[3])==L"prepared")Exercise<PreparedBeforeUpscale>(d.Get(),context.Get(),c,owner);else Exercise<BeforeUpscale>(d.Get(),context.Get(),c,owner);std::puts("PASS independent later-reader capacity progress");
}
