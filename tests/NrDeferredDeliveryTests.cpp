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
[[noreturn]] void Fail(const char* name){std::fprintf(stderr,"FAIL %s\n",name);ExitProcess(1);}
void Need(bool value,const char* name){if(!value)Fail(name);}
void Gpu(HRESULT hr,const char* name){if(FAILED(hr)){std::fprintf(stderr,"HRESULT=%08lx\n",hr);Fail(name);}}
void Done(Result<void> result,const char* name){if(!result){std::fprintf(stderr,"%s\n",result.error().message.c_str());Fail(name);}}
RuntimeExports::EvaluateFn vendor;
ComPtr<ID3D11DeviceContext4> context4;
ComPtr<ID3D11Fence> encoderGate11;
bool injectGate=true;
uint32_t __cdecl EvaluateWithDelayedEncoder(ID3D12GraphicsCommandList* list,void* feature,NVSDK_NGX_Parameter* parameters,void* callback){auto result=vendor(list,feature,parameters,callback);if(result==1&&injectGate){injectGate=false;Gpu(context4->Wait(encoderGate11.Get(),1),"queue encoder gate after actual input production");}return result;}
template<class T>Result<uint32_t> Collect(T& prepared){if constexpr(requires{prepared.CollectCompleted();})return prepared.CollectCompleted();else return uint32_t{0};}
template<class T>Result<void> WaitDelivery(T& prepared,const BeforeResult& result){if constexpr(requires{prepared.WaitDelivery(result);})return prepared.WaitDelivery(result);else return {};}
struct Image {BeforeInput input;ComPtr<ID3D11Texture2D> readback;std::vector<unsigned char> pixels;};
double Milliseconds(std::chrono::steady_clock::time_point start){return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();}
}
int wmain(int argc,wchar_t** argv){
    if(argc!=4&&argc!=5)return 77;if(NrRuntimeResearch::GameRunningOrUnknown())return 1;setvbuf(stdout,nullptr,_IONBF,0);
    ComPtr<ID3D11Device> device11;ComPtr<ID3D11DeviceContext> context;
    Gpu(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device11,nullptr,&context),"D3D11");Gpu(context.As(&context4),"context4");ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;Gpu(device11.As(&dxgi),"DXGI");Gpu(dxgi->GetAdapter(&adapter),"adapter");DXGI_ADAPTER_DESC desc{};Gpu(adapter->GetDesc(&desc),"description");if(desc.VendorId!=0x10de||desc.DeviceId!=0x2702)return 77;
    StageContract contract;contract.colorExtent=contract.guideExtent={320,180};contract.adapterLuid={desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart};Gpu(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&contract.device)),"D3D12");D3D12_COMMAND_QUEUE_DESC queue{};Gpu(contract.device->CreateCommandQueue(&queue,IID_PPV_ARGS(&contract.queue)),"queue");
    if(argc==5){ComPtr<ID3D12Fence> probe;ComPtr<ID3D12Device> native;ComPtr<IUnknown> wrapper,anchor;Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&probe)),"identity fence");Gpu(probe->GetDevice(IID_PPV_ARGS(&native)),"native device");Gpu(contract.device.As(&wrapper),"wrapper identity");Gpu(native.As(&anchor),"native device identity");Need(wrapper.Get()!=anchor.Get(),"ActualReShadeWrappedDeviceRequired");}
    ComPtr<ID3D12Fence> encoderGate;Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_SHARED,IID_PPV_ARGS(&encoderGate)),"encoder gate");HANDLE shared{};Gpu(contract.device->CreateSharedHandle(encoderGate.Get(),nullptr,GENERIC_ALL,nullptr,&shared),"gate handle");ComPtr<ID3D11Device5> device5;Gpu(device11.As(&device5),"device5");auto opened=device5->OpenSharedFence(shared,IID_PPV_ARGS(&encoderGate11));CloseHandle(shared);Gpu(opened,"shared encoder gate");
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-deferred-cache"),true});Done(owner->Open(RuntimeCatalog()[1],contract.device.Get(),{desc.VendorId,desc.DeviceId,desc.SubSysId,contract.adapterLuid,false}),"runtime");vendor=owner->Exports().evaluate;const_cast<RuntimeExports&>(owner->Exports()).evaluate=&EvaluateWithDelayedEncoder;
    PreparedBeforeUpscale prepared;Done(prepared.Initialize(owner,device11.Get(),contract),"prepared initialization");
    std::array<Image,4> images;
    for(unsigned frame=0;frame<4;++frame){auto& image=images[frame];auto make=[&](DXGI_FORMAT format,UINT bind,D3D11_USAGE usage=D3D11_USAGE_DEFAULT){D3D11_TEXTURE2D_DESC t{};t.Width=320;t.Height=180;t.ArraySize=t.MipLevels=t.SampleDesc.Count=1;t.Format=format;t.BindFlags=bind;t.Usage=usage;t.CPUAccessFlags=usage==D3D11_USAGE_STAGING?D3D11_CPU_ACCESS_READ:0;ComPtr<ID3D11Texture2D> texture;Gpu(device11->CreateTexture2D(&t,nullptr,&texture),"texture");return texture;};
        auto& p=image.input;p.context=context;p.color=make(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);p.depth=make(DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE);p.motion=make(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);image.readback=make(DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_USAGE_STAGING);image.pixels.resize(320*180*4);for(unsigned y=0;y<180;++y)for(unsigned x=0;x<320;++x){auto i=(y*320+x)*4;image.pixels[i]=((x/8+y/8+frame)&1)?48:208;image.pixels[i+1]=(x+frame)%256;image.pixels[i+2]=(y+frame)%256;image.pixels[i+3]=(x+y+frame)%256;}std::vector<float> depth(320*180,.5f);std::vector<uint16_t> motion(320*180*2);context->UpdateSubresource(p.color.Get(),0,nullptr,image.pixels.data(),320*4,0);context->UpdateSubresource(p.depth.Get(),0,nullptr,depth.data(),320*4,0);context->UpdateSubresource(p.motion.Get(),0,nullptr,motion.data(),320*4,0);p.epoch=p.guideEpoch=1;p.sourceId=p.guideSourceId=frame+1;p.previousSourceId=frame;p.presentationTime=double(frame+1)/60;p.colorExtent=p.guideExtent={320,180};p.motionScaleX=p.motionScaleY=1;p.reset=frame==0;}
    context->Flush();SettingsSnapshot settings;settings.enabled=true;settings.revision=1;settings.tuning.localToneStrength=0;
    const bool timeout=std::wstring_view(argv[3])==L"timeout";
    auto release=std::thread([encoderGate,timeout]{std::this_thread::sleep_for(std::chrono::milliseconds(timeout?22000:750));Gpu(encoderGate->Signal(1),"external encoder gate release");});
    const auto start=std::chrono::steady_clock::now();auto first=prepared.Evaluate(images[0].input,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,settings);Need(bool(first),"first evaluated");Need(Milliseconds(start)<350&&encoderGate->GetCompletedValue()==0,"EvaluateReturnsBeforeGatedDeliveryCompletes");
    ComPtr<ID3D12Fence> nrDone;Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&nrDone)),"observe actual NR completion");Gpu(contract.queue->Signal(nrDone.Get(),1),"actual NR observation signal");auto event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Need(event!=nullptr,"event");Gpu(nrDone->SetEventOnCompletion(1,event),"NR event");Need(WaitForSingleObject(event,500)==WAIT_OBJECT_0&&nrDone->GetCompletedValue()==1,"actual NR completes while encoder gated");CloseHandle(event);
    auto collected=Collect(prepared);Need(collected&&*collected==0&&prepared.Diagnostics().pendingTickets==1,"PreparedEncodeReaderPreventsSlotReuse");
    ComPtr<ID3D12CommandQueue> otherQueue;Gpu(contract.device->CreateCommandQueue(&queue,IID_PPV_ARGS(&otherQueue)),"other reader queue");ComPtr<ID3D12Fence> otherDone;Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&otherDone)),"other reader fence");Done(prepared.TrackReader(first->delivery,otherDone.Get(),1),"register unrelated reader");Gpu(otherQueue->Signal(otherDone.Get(),99),"higher other queue signal");event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Need(event!=nullptr,"other event");Gpu(otherDone->SetEventOnCompletion(99,event),"other reader event");Need(WaitForSingleObject(event,500)==WAIT_OBJECT_0,"other queue completed");CloseHandle(event);collected=Collect(prepared);Need(collected&&*collected==0&&prepared.Diagnostics().pendingTickets==1,"HigherOtherQueueSignalCannotRetireReader");
    const bool retire=std::wstring_view(argv[3])==L"retire";
    const bool style=std::wstring_view(argv[3])==L"style";
    if(style){
        settings.tuning.style=1;++settings.revision;
        const auto begun=std::chrono::steady_clock::now();
        auto changed=prepared.Evaluate(images[1].input,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,settings);
        Need(bool(changed),"LiveStyleChangeEvaluatesAfterRetirement");
        Need(Milliseconds(begun)>250&&encoderGate->GetCompletedValue()==1,"LiveStyleChangeWaitsForActualEncoderReader");
        Need(changed->effectiveReset,"LiveStyleChangeResetsHistoryWithoutCallerReset");
        Done(prepared.Retire(),"style transition final retirement");
    }
    else if(retire){auto begun=std::chrono::steady_clock::now();Done(prepared.Retire(),"resize/retirement drains pending delivery");Need(Milliseconds(begun)>250,"ResizeRetainsPendingDelivery");}
    else{
        std::array<BeforeResult,4> results;results[0]=*first;
        for(unsigned i=1;i<3;++i){auto result=prepared.Evaluate(images[i].input,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,settings);Need(bool(result),"next queued source");results[i]=*result;}
        Need(prepared.Diagnostics().pendingTickets==3,"ThreePendingPreparedImagesRetained");collected=Collect(prepared);Need(collected&&*collected==0,"NoPendingEncoderSlotReused");auto fourthStart=std::chrono::steady_clock::now();auto fourth=prepared.Evaluate(images[3].input,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,settings);
        if(timeout){Need(!fourth&&Milliseconds(fourthStart)>=19000&&Milliseconds(fourthStart)<21500,"PressureDeadlineIsBounded");Need(prepared.Diagnostics().terminal&&prepared.Diagnostics().pendingTickets==3&&!prepared.Retire()&&!owner->Retire(),"TimeoutRetainsOwnersAndReportsTerminal");release.join();std::puts("PASS terminal deadline retains all pending owners");return 0;}
        Need(bool(fourth)&&Milliseconds(fourthStart)>250,"CapacityPressureWaitsForRealReader");results[3]=*fourth;
        Done(WaitDelivery(prepared,results[3]),"explicit source delivery wait");
        for(auto& image:images){context->CopyResource(image.readback.Get(),image.input.color.Get());D3D11_MAPPED_SUBRESOURCE mapped{};Gpu(context->Map(image.readback.Get(),0,D3D11_MAP_READ,0,&mapped),"later source readback");for(unsigned y=0;y<180;++y){auto* row=static_cast<const unsigned char*>(mapped.pData)+y*mapped.RowPitch;for(unsigned x=0;x<320;++x)Need(row[x*4+3]==image.pixels[(y*320+x)*4+3],"LaterSourceWorkFollowsQueuedNr_Alpha");}context->Unmap(image.readback.Get(),0);}
        auto off=settings;off.enabled=false;off.revision=2;Need(bool(prepared.Evaluate({},TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,off)),"off drains pending work");Done(prepared.Retire(),"final retirement");
    }
    release.join();Done(owner->Retire(),"runtime retirement");std::puts("PASS deferred delivery, actual NR/encoder ownership, bounded pressure/lifecycle and alpha");return 0;
}
