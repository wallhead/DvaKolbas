#include "NeuralRendering/PostUpscale.h"
#include "FrameGen/FSRHostPresentation.h"
#include "Upscaling/FSRFrameAdapter.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include "../fsr-fg/PresentationObserver.h"
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3d11sdklayers.h>
#include <d3d12sdklayers.h>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>
#include <unordered_map>
#include <unordered_set>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
namespace NR=TheosRenderPipeline::NeuralRendering;
namespace {
unsigned failures{};
std::atomic<unsigned> sdkMessages{};
void Check(bool value,const char* reason){if(!value){++failures;std::fprintf(stderr,"FAIL %s\n",reason);}}
void Gpu(HRESULT value){if(FAILED(value))throw value;}
template<class T> T Value(Result<T> value){if(!value)throw std::runtime_error(value.error().message);return std::move(*value);}
void Accepted(Result<void> value){if(!value)throw std::runtime_error(value.error().message);}
template<class T> T NeuralValue(NR::Result<T> value){if(!value)throw std::runtime_error(value.error().message);return std::move(*value);}
void NeuralAccepted(NR::Result<void> value){if(!value)throw std::runtime_error(value.error().message);}
void SdkMessage(uint32_t,const wchar_t* text){++sdkMessages;if(text)std::fwprintf(stderr,L"SDK: %ls\n",text);}
struct Window{HWND value{};~Window(){if(value)DestroyWindow(value);}};

// A CPU-signalled gate delays real GPU work. Completion is only signalled by
// the actual game queue after FG Prepare/publication, never acknowledged by CPU.
// The independent watchdog guarantees the test cannot strand a graphics queue.
class PendingWork {
public:
    PendingWork(ID3D12Device* device,ID3D12CommandQueue* queue):queue_(queue){
        Gpu(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate_)));
        Gpu(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&done_)));
        start_=CreateEventW(nullptr,TRUE,FALSE,nullptr);if(!start_)throw HRESULT_FROM_WIN32(GetLastError());
        try{release_=std::jthread([this]{
            fallback_=WaitForSingleObject(start_,2000)!=WAIT_OBJECT_0;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));gate_->Signal(1);
        });}catch(...){CloseHandle(start_);throw;}
    }
    ~PendingWork(){
        // On an earlier exception, still queue the real completion marker and
        // release the gate before any owner attempts its retirement.
        if(armed_&&!marked_)(void)queue_->Signal(done_.Get(),1);
        gate_->Signal(1);SetEvent(start_);if(release_.joinable())release_.join();CloseHandle(start_);
    }
    void Arm(){Gpu(queue_->Wait(gate_.Get(),1));armed_=true;}
    void Mark(){Gpu(queue_->Signal(done_.Get(),1));marked_=true;}
    void Release(){SetEvent(start_);}
    bool Pending()const{return done_->GetCompletedValue()<1;}
    bool Completed()const{return done_->GetCompletedValue()==1;}
    bool Fallback()const{return fallback_.load();}
private:
    ComPtr<ID3D12CommandQueue> queue_;
    ComPtr<ID3D12Fence> gate_,done_;HANDLE start_{};
    bool armed_{},marked_{};std::atomic<bool> fallback_{};std::jthread release_;
};
struct World {
    UINT width{},height{};ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    ComPtr<ID3D11Texture2D> input,color,depth,motion,ui,reference;
    World(ID3D11Device* d,ID3D11DeviceContext* c,Extent extent):width(extent.width),height(extent.height),device(d),context(c){
        auto make=[&](DXGI_FORMAT format,UINT bind){D3D11_TEXTURE2D_DESC shape{};shape.Width=width;shape.Height=height;
            shape.MipLevels=shape.ArraySize=shape.SampleDesc.Count=1;shape.Format=format;shape.BindFlags=bind;
            ComPtr<ID3D11Texture2D> t;Gpu(device->CreateTexture2D(&shape,nullptr,&t));return t;};
        input=make(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
        color=make(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
        ui=make(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
        reference=make(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
        depth=make(DXGI_FORMAT_R32_TYPELESS,D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE);
        motion=make(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
        D3D11_DEPTH_STENCIL_VIEW_DESC desc{};desc.Format=DXGI_FORMAT_D32_FLOAT;desc.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2D;
        ComPtr<ID3D11DepthStencilView> view;Gpu(device->CreateDepthStencilView(depth.Get(),&desc,&view));context->ClearDepthStencilView(view.Get(),D3D11_CLEAR_DEPTH,.5f,0);
        std::vector<unsigned short> velocity(size_t(width)*height*2);context->UpdateSubresource(motion.Get(),0,nullptr,velocity.data(),width*4,0);
        auto hud=Hud();context->UpdateSubresource(ui.Get(),0,nullptr,hud.data(),width*4,0);
    }
    std::vector<unsigned char> Hud()const{
        std::vector<unsigned char> bytes(size_t(width)*height*4);
        for(size_t i=0;i<size_t(width)*height;++i){const auto x=i%width;bytes[i*4]=x<20?255:0;
            bytes[i*4+1]=x>=20&&x<40?128:0;bytes[i*4+3]=x<20?255:x<40?128:0;}return bytes;
    }
    std::vector<unsigned char> Read(ID3D11Texture2D* texture)const{
        D3D11_TEXTURE2D_DESC shape{};texture->GetDesc(&shape);const unsigned bpp=shape.Format==DXGI_FORMAT_R16G16B16A16_FLOAT?8:4;
        shape.Usage=D3D11_USAGE_STAGING;shape.BindFlags=shape.MiscFlags=0;shape.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
        ComPtr<ID3D11Texture2D> staging;Gpu(device->CreateTexture2D(&shape,nullptr,&staging));context->CopyResource(staging.Get(),texture);
        D3D11_MAPPED_SUBRESOURCE mapped{};Gpu(context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped));
        std::vector<unsigned char> bytes(size_t(width)*height*bpp);
        for(UINT y=0;y<height;++y)std::memcpy(bytes.data()+size_t(y)*width*bpp,static_cast<const unsigned char*>(mapped.pData)+y*mapped.RowPitch,width*bpp);
        context->Unmap(staging.Get(),0);return bytes;
    }
};
struct Stats {
    uint64_t pixels{},alpha{},changed{},bypass{},bypassExpected{},realRgb{},realRgbExpected{},ui{},uiExpected{};
    unsigned sources{},nr{},callbacks{},generated{},real{},changing{},suspensions{},resizes{},pending{},hud{},srUntouched{},historyResets{},reentries{};
};
}
int wmain(int argc,wchar_t** argv){try{
    std::setvbuf(stdout,nullptr,_IONBF,0);
    if(argc!=5)return 1;const bool observed=std::wstring_view(argv[4])==L"observer";
    if(!observed&&std::wstring_view(argv[4])!=L"automatic")return 1;
    if(NrRuntimeResearch::GameRunningOrUnknown()){std::puts("REFUSED: Skyrim running or process inventory unavailable");return 1;}
    ComPtr<ID3D12Debug> debug;const bool debug12=SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));if(debug12)debug->EnableDebugLayer();
    ComPtr<IDXGIFactory6> factory;Gpu(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 desc{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> candidate;const auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));
        if(hr==DXGI_ERROR_NOT_FOUND)break;Gpu(hr);Gpu(candidate->GetDesc1(&desc));if(desc.VendorId==0x10de&&desc.DeviceId==0x2702){adapter=candidate;break;}}
    if(!adapter){std::puts("REFUSED: qualified RTX40 adapter unavailable");return 1;}
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    auto hr=D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,D3D11_CREATE_DEVICE_DEBUG,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context);
    if(hr==DXGI_ERROR_SDK_COMPONENT_MISSING)hr=D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context);Gpu(hr);
    ComPtr<ID3D11InfoQueue> diagnostics11;device.As(&diagnostics11);
    WNDCLASSW wc{};wc.hInstance=GetModuleHandleW(nullptr);wc.lpfnWndProc=DefWindowProcW;wc.lpszClassName=L"TRPNrFsrLifecycleProbe";
    if(!RegisterClassW(&wc))throw std::runtime_error("window class");Window window;
    window.value=CreateWindowExW(0,wc.lpszClassName,L"NR FSR lifecycle",WS_OVERLAPPEDWINDOW,0,0,500,320,nullptr,nullptr,wc.hInstance,nullptr);
    if(!window.value)throw std::runtime_error("fixture HWND");
    auto sr=std::make_shared<FsrHostResources>(std::filesystem::absolute(argv[3]));
    FsrHostPresentation host;FsrSettings settings;settings.quality=Quality::NativeAA;settings.sourceColorEncoding=ColorEncoding::Gamma22;
    DXGI_SWAP_CHAIN_DESC swap{};swap.OutputWindow=window.value;swap.Windowed=TRUE;swap.BufferDesc.Width=320;swap.BufferDesc.Height=180;
    swap.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;swap.SampleDesc.Count=1;swap.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;swap.BufferCount=1;
    Check(Value(host.Create(factory.Get(),device.Get(),sr,swap,settings))==Extent{320,180},"HostNativeSizing");Gpu(host.StartupPresent(0,0));Accepted(sr->CompleteStartup());Gpu(host.SwapChain()->GetDesc(&swap));
    const auto bridge=sr->Bridge();const auto heldDevice=bridge->Device12();const auto heldQueue=bridge->Queue();const auto heldChain=host.SwapChain();
    ComPtr<ID3D12InfoQueue> diagnostics12;heldDevice->QueryInterface(IID_PPV_ARGS(&diagnostics12));
    std::printf("GRAPHICS_DEBUG d3d11=%u d3d12=%u\n",bool(diagnostics11),bool(diagnostics12));
    auto runtime=sr->Runtime();ffxConfigureDescGlobalDebug1 dbg{{FFX_API_CONFIGURE_DESC_TYPE_GLOBALDEBUG1,nullptr},SdkMessage,FFX_API_CONFIGURE_GLOBALDEBUG_LEVEL_ERRORS|FFX_API_CONFIGURE_GLOBALDEBUG_LEVEL_WARNINGS};
    for(auto module:{L"amd_fidelityfx_upscaler_dx12.dll",L"amd_fidelityfx_framegeneration_dx12.dll"}){
        auto configure=reinterpret_cast<PfnFfxConfigure>(GetProcAddress(GetModuleHandleW(module),"ffxConfigure"));
        if(!configure||configure(nullptr,&dbg.header)!=FFX_API_RETURN_OK)throw std::runtime_error("SDK diagnostics unavailable");}
    auto& functions=const_cast<FsrFunctions&>(runtime->Functions());FgObservation::originalConfigure=functions.Configure;functions.Configure=FgObservation::Configure;
    struct Restore{FsrFunctions& functions;~Restore(){functions.Configure=FgObservation::originalConfigure;FgObservation::activeCapture=nullptr;}} restore{functions};
    auto owner=std::make_shared<NR::RuntimeOwner>(NR::RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-fsr-lifecycle-cache"),true});
    const NR::AdapterLuid luid{desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart};
    NeuralAccepted(owner->Open(NR::RuntimeCatalog()[1],heldDevice,{desc.VendorId,desc.DeviceId,desc.SubSysId,luid,false}));
    Stats stats;const std::array<Extent,3> extents{{{320,180},{384,216},{320,180}}};
    for(unsigned cycle=0;cycle<extents.size();++cycle){const auto extent=extents[cycle];
        World world(device.Get(),context.Get(),extent);NR::PostUpscale post;
        NR::StageContract contract;contract.device=heldDevice;contract.queue=heldQueue;contract.adapterLuid=luid;
        contract.colorExtent=contract.guideExtent={extent.width,extent.height};NeuralAccepted(post.Initialize(owner,device.Get(),contract));
        auto reconstruction=std::make_unique<FsrFrameAdapter>(*sr->Upscaler(),bridge,sr->Resources(),sr->Color11(),sr->Depth11(),sr->Motion11(),sr->Output11(),sr->HandoffEncoding());
        std::unique_ptr<FgObservation::Capture> capture;
        if(observed){capture=std::make_unique<FgObservation::Capture>(heldDevice,136,extent.width,extent.height,true);FgObservation::activeCapture=capture.get();}
        // Any exceptional path must retire SDK users before deleting callback
        // state. On uncertainty, quarantine the capture as the host does.
        struct CaptureLifetime{FsrHostPresentation& host;std::unique_ptr<FgObservation::Capture>& capture;bool retired{};
            ~CaptureLifetime(){if(!retired){try{if(!host.Retire())(void)capture.release();}catch(...){(void)capture.release();}}FgObservation::activeCapture=nullptr;}}
            lifetime{host,capture};
        std::unordered_map<uint64_t,std::vector<unsigned char>> expectedRows;
        std::unordered_set<uint64_t> submitted,eligible,requiredReal,requiredGenerated;
        FsrColorConverter encoder;UpscaleFrame real;real.backend=BackendKind::Fsr;real.render=real.subrect=real.display=extent;
        real.color=real.input=world.input.Get();real.output=world.color.Get();real.depth=world.depth.Get();real.motion=world.motion.Get();
        real.depthFormat=DXGI_FORMAT_R32_FLOAT;real.motionFormat=DXGI_FORMAT_R16G16_FLOAT;real.deltaMilliseconds=1000.f/60;
        real.motionConvention={float(extent.width),float(extent.height),true,false};real.sourceEpoch=cycle+1;
        real.camera.identity=1;real.camera.worldUnitsToMeters=1;real.camera.nearDistance=.1f;real.camera.farDistance=100;real.camera.verticalFovRadians=1;
        DirectX::XMFLOAT4X4 view,projection;DirectX::XMStoreFloat4x4(&view,DirectX::XMMatrixIdentity());
        DirectX::XMStoreFloat4x4(&projection,DirectX::XMMatrixPerspectiveFovLH(1,float(extent.width)/extent.height,.1f,100));
        std::memcpy(real.camera.view.data(),&view,64);std::memcpy(real.camera.projection.data(),&projection,64);
        NR::SettingsSnapshot tuning;tuning.placement=NR::Placement::After;tuning.revision=1;
        std::unique_ptr<PendingWork> pending;
        const auto retirePending=[&](bool suspend){
            Check(pending&&pending->Pending(),"ActualFgWorkPendingAtLifecycleBoundary");if(pending)pending->Release();
            const auto start=std::chrono::steady_clock::now();
            if(suspend)Accepted(host.Suspend());else Accepted(host.BeforeResize());
            const auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
            Check(elapsed>=100,"LifecycleWaitsForGenuinelyPendingFgConsumer");
            Check(pending&&pending->Completed(),"FgQueueCompletionObservedBeforeResourceRetirement");
            Check(!host.FeatureReady()&&host.Suspended(),"SourceAdmissionStoppedAtRetiredBoundary");
            std::printf("PENDING_BOUNDARY epoch=%u operation=%s waitMs=%lld markerComplete=%u watchdogFallback=%u\n",cycle+1,suspend?"suspend":"resize",static_cast<long long>(elapsed),pending&&pending->Completed(),pending&&pending->Fallback());
            if(suspend){Accepted(host.Resume());++stats.suspensions;}
            Check(pending&&!pending->Fallback(),"PresentReturnsBeforeIndependentGateRelease");++stats.pending;pending.reset();
        };
        for(unsigned local=0;local<64;++local){
            if(local==56)retirePending(true);
            Gpu(host.WaitBeforeProducer());real.sourceId=++stats.sources;real.reset=local==0||local==40||local==41||local==56;
            real.camera.reset=local==48;auto jitter=Value(sr->Upscaler()->QueryJitter(real.sourceId));real.jitterX=jitter[0];real.jitterY=jitter[1];
            std::vector<unsigned char> bytes(size_t(extent.width)*extent.height*4);
            for(size_t i=0;i<bytes.size()/4;++i){bytes[i*4]=((i/8+real.sourceId)%2)?48:208;bytes[i*4+1]=(i+real.sourceId)%256;
                bytes[i*4+2]=(i/extent.width+real.sourceId)%256;bytes[i*4+3]=(i+real.sourceId)%256;}
            context->UpdateSubresource(world.input.Get(),0,nullptr,bytes.data(),extent.width*4,0);
            Check(Value(reconstruction->Evaluate(real))==UpscaleOutcome::Temporal,"ActualTemporalSrBeforeNr");
            auto original=world.Read(world.color.Get());auto retained=world.Read(sr->Output11());
            NR::PostSrInput input;auto& native=input.resources;native.context=context;native.color=world.color;native.depth=world.depth;native.motion=world.motion;
            native.epoch=native.guideEpoch=cycle+1;native.sourceId=native.guideSourceId=real.sourceId;native.previousSourceId=real.sourceId-1;
            native.presentationTime=double(real.sourceId)/60.;native.colorExtent=native.guideExtent={extent.width,extent.height};
            native.motionScaleX=float(extent.width);native.motionScaleY=float(extent.height);native.colorDomain=NR::ColorDomain::SdrBytes;native.reset=real.reset||real.camera.reset;
            auto& meta=input.source;meta.backend=BackendKind::Fsr;meta.outcome=UpscaleOutcome::Temporal;
            meta.epoch=meta.guideEpoch=native.epoch;meta.sourceId=meta.guideSourceId=native.sourceId;meta.previousSourceId=native.previousSourceId;
            meta.sourceTime=meta.guideTime=native.presentationTime;meta.render=meta.display=meta.color=meta.guides={extent.width,extent.height};
            meta.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;meta.depthFormat=DXGI_FORMAT_R32_FLOAT;meta.motionFormat=DXGI_FORMAT_R16G16_FLOAT;meta.guideOrigin=NR::GuideOrigin::RealSource;
            meta.colorDomain=NR::ColorDomain::SdrBytes;meta.encoding=ColorEncoding::Gamma22;meta.motion=real.motionConvention;
            tuning.enabled=local!=40;tuning.revision=local<40?1:local==40?2:3;
            auto output=NeuralValue(post.Evaluate(input,tuning));Check(output.evaluated==tuning.enabled,"ExactlyOneNrPassPerEnabledRealSource");
            NeuralAccepted(post.WaitDelivery(output));stats.nr+=output.evaluated;stats.historyResets+=output.effectiveReset;
            auto enhanced=world.Read(world.color.Get());const auto count=original.size()/4;stats.pixels+=count;
            for(size_t i=0;i<count;++i){stats.alpha+=original[i*4+3]==enhanced[i*4+3];const bool same=std::memcmp(original.data()+i*4,enhanced.data()+i*4,3)==0;
                stats.changed+=!same;if(!tuning.enabled){stats.bypassExpected+=1;stats.bypass+=same&&original[i*4+3]==enhanced[i*4+3];}}
            stats.hud+=world.Read(world.ui.Get())==world.Hud();stats.srUntouched+=world.Read(sr->Output11())==retained;
            Gpu(encoder.Convert(context.Get(),world.color.Get(),world.reference.Get(),ColorEncoding::Gamma22,ColorEncoding::SRGB));
            if(observed){auto reference=world.Read(world.reference.Get());auto& rows=expectedRows[real.sourceId];rows.resize(extent.width*2*4);
                std::memcpy(rows.data(),reference.data(),extent.width*4);std::memcpy(rows.data()+extent.width*4,reference.data()+(extent.height/2)*extent.width*4,extent.width*4);}
            context->CopyResource(host.SceneTarget11(),world.color.Get());real.reset|=output.effectiveReset;
            const bool requested=local<20||local>=24,menu=local>=32&&local<36;
            if(local==55||local==63){pending=std::make_unique<PendingWork>(heldDevice,heldQueue);pending->Arm();}
            Gpu(host.Present(real,UpscaleOutcome::Temporal,world.ui.Get(),nullptr,true,menu,requested,0,0));
            const auto status=host.Status();Check(status.submitted&&status.sourceId==real.sourceId,"SourceSubmissionIdentity");
            Check(status.callback.invocations==(status.decision.generate?1u:0u),"OneSdkGenerationCallbackOnlyOnEligibleSource");
            if(!requested||menu||real.reset||real.camera.reset)Check(!status.decision.generate&&status.callback.invocations==0,"IndependentSuppressionSchedule");
            if(local==1||local==24||local==36||local==42||local==49||local==57)Check(status.decision.generate&&status.decision.reset&&status.callback.invocations==1,"EachResetAndRecoveryActuallyGenerates");
            stats.callbacks+=status.callback.invocations;stats.reentries+=status.decision.generate&&status.decision.reset;
            submitted.insert(real.sourceId);if(status.decision.generate)eligible.insert(real.sourceId);
            // Suspend/resize may detach the public Present observer before the
            // two deliberately queued boundary sources reach it. Every other
            // source, including off/menu/bypass/reset/recovery, is mandatory.
            if(local!=55&&local!=63){requiredReal.insert(real.sourceId);if(status.decision.generate)requiredGenerated.insert(real.sourceId);}
            if(pending&&(local==55||local==63)){pending->Mark();Check(pending->Pending(),"ActuallySubmittedFgQueueFenceStillIncomplete");}
        }
        NeuralAccepted(post.Retire());retirePending(false);lifetime.retired=true;FgObservation::activeCapture=nullptr;
        if(capture){Check(!capture->Failed(),"CaptureRemainsValidThroughLifecycleRetirement");
            std::unordered_set<uint64_t> realSeen,generatedSeen;uint64_t previous{};unsigned generatedThisCycle{},changedThisCycle{};
            for(unsigned index=0;index<capture->Count();++index){const auto& sample=capture->Samples()[index];
                Check(sample.recorded&&submitted.contains(sample.source),"CapturedSourceBelongsToCurrentEpoch");
                Check((sample.generated?generatedSeen:realSeen).insert(sample.source).second,"OneObservationPerImageKindAndSource");
                if(sample.generated)Check(eligible.contains(sample.source),"GeneratedImageBelongsToAnEligibleSource");
                D3D12_RANGE range{0,capture->RowPitch()*4};void* mapped{};Gpu(sample.readback->Map(0,&range,&mapped));const auto* data=static_cast<const unsigned char*>(mapped);
                uint64_t hash=1469598103934665603ull;
                for(unsigned row=0;row<2;++row){auto* composed=data+row*capture->RowPitch();auto* raw=data+(row+2)*capture->RowPitch();
                    for(UINT x=0;x<extent.width;++x){const unsigned a=x<20?255:x<40?128:0;
                        for(unsigned channel=0;channel<3;++channel){const unsigned hud=(x<20&&channel==0)?255:(x>=20&&x<40&&channel==1)?128:0;
                            stats.ui+=std::abs(int(composed[x*4+channel])-int(std::lround(hud+raw[x*4+channel]*(255-a)/255.)))<=1;
                            if(!sample.generated)stats.realRgb+=raw[x*4+channel]==expectedRows.at(sample.source)[(row*extent.width+x)*4+channel];
                            if(row==1&&x>=40)hash=(hash^raw[x*4+channel])*1099511628211ull;}
                        stats.ui+=composed[x*4+3]==255;}}
                D3D12_RANGE noWrite{};sample.readback->Unmap(0,&noWrite);stats.uiExpected+=uint64_t(extent.width)*2*4;
                if(sample.generated){++stats.generated;++generatedThisCycle;if(previous&&previous!=hash){++stats.changing;++changedThisCycle;}previous=hash;}
                else{++stats.real;stats.realRgbExpected+=uint64_t(extent.width)*2*3;}
            }
            for(auto source:requiredReal)Check(realSeen.contains(source),"EveryNonBoundaryRealSourceIsCaptured");
            for(auto source:requiredGenerated)Check(generatedSeen.contains(source),"EveryNonBoundaryEligibleGeneratedSourceIsCaptured");
            Check(generatedThisCycle>=2&&changedThisCycle>=1,"ActualChangingGeneratedImagesAtEveryResizedExtent");
        }
        reconstruction.reset();Check(owner->Ready()&&sr->Bridge().get()==bridge.get()&&bridge->Device12()==heldDevice&&bridge->Queue()==heldQueue,"RuntimeAndDeviceRetainedAcrossSizeRetirement");
        if(cycle+1<extents.size()){swap.BufferDesc.Width=extents[cycle+1].width;swap.BufferDesc.Height=extents[cycle+1].height;
            const auto resized=Value(host.Resize(swap));Gpu(resized.result);Check(resized.render==extents[cycle+1]&&host.SwapChain()==heldChain,"ActualSameSwapchainResizeNativeAa");
            Accepted(sr->CompleteStartup());++stats.resizes;}
    }
    Accepted(host.Retire());NeuralAccepted(owner->Retire());
    Check(stats.sources==192&&stats.nr==189,"ExactlyOneNrPerEnabledSourceAcrossThreeEpochs");
    Check(stats.alpha==stats.pixels&&stats.changed>57600&&stats.bypass==stats.bypassExpected,"AlphaAndOffBypassAcrossChangedExtents");
    Check(stats.hud==192&&stats.srUntouched==192,"UiAndTemporalSrHistoryNeverReceiveNrFeedback");
    Check(stats.suspensions==3&&stats.resizes==2&&stats.pending==6&&stats.reentries==18,"AllPendingLifecycleAndResetTransitionsObserved");
    if(observed)Check(stats.realRgb==stats.realRgbExpected&&stats.ui==stats.uiExpected,"EnhancedRealRgbAndIndependentUiBlendAtEveryExtent");
    Check(sdkMessages.load()==0,"SdkWarningAndErrorCountZero");
    unsigned graphicsErrors{};
    if(diagnostics11)for(UINT64 i=0;i<diagnostics11->GetNumStoredMessagesAllowedByRetrievalFilter();++i){
        SIZE_T size{};Gpu(diagnostics11->GetMessage(i,nullptr,&size));std::vector<unsigned char> storage(size);
        auto* message=reinterpret_cast<D3D11_MESSAGE*>(storage.data());Gpu(diagnostics11->GetMessage(i,message,&size));
        if(message->Severity<=D3D11_MESSAGE_SEVERITY_ERROR){++graphicsErrors;std::fprintf(stderr,"D3D11: %s\n",message->pDescription);}}
    if(diagnostics12)for(UINT64 i=0;i<diagnostics12->GetNumStoredMessagesAllowedByRetrievalFilter();++i){
        SIZE_T size{};Gpu(diagnostics12->GetMessage(i,nullptr,&size));std::vector<unsigned char> storage(size);
        auto* message=reinterpret_cast<D3D12_MESSAGE*>(storage.data());Gpu(diagnostics12->GetMessage(i,message,&size));
        if(message->Severity<=D3D12_MESSAGE_SEVERITY_ERROR){++graphicsErrors;std::fprintf(stderr,"D3D12: %s\n",message->pDescription);}}
    if(diagnostics11||diagnostics12)Check(graphicsErrors==0,"AvailableGraphicsDebugQueuesHaveNoErrors");
    else std::puts("SKIPPED: Graphics Tools debug queues unavailable");
    std::printf("FSR_NR_LIFECYCLE mode=%s sources=%u nr=%u callbacks=%u generatedReadbacks=%u changingGenerated=%u realReadbacks=%u alphaExact=%llu pixelExpected=%llu bypassExact=%llu realRgbExact=%llu realRgbExpected=%llu uiVerified=%llu uiExpected=%llu suspensions=%u resizes=%u pendingBoundaries=%u reentries=%u sdkMessages=%u\n",
        observed?"observer":"automatic",stats.sources,stats.nr,stats.callbacks,stats.generated,stats.changing,stats.real,
        (unsigned long long)stats.alpha,(unsigned long long)stats.pixels,(unsigned long long)stats.bypass,(unsigned long long)stats.realRgb,(unsigned long long)stats.realRgbExpected,
        (unsigned long long)stats.ui,(unsigned long long)stats.uiExpected,stats.suspensions,stats.resizes,stats.pending,stats.reentries,sdkMessages.load());
    std::puts("SCOPE genuine pending FG queue + SDK reader retirement, same-chain native-AA resize; callback-row pixels only, no physical cadence/automatic UI/performance claim");
    return failures?1:0;
}catch(HRESULT error){std::printf("GPU failure 0x%08x\n",unsigned(error));return 1;}catch(const std::exception& error){std::printf("FAIL %s\n",error.what());return 1;}}
