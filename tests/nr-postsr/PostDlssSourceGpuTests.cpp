#include "DLSSBackend.h"
#include "FrameGen/SourceNvidiaFrameEvaluator.h"
#include "FrameGen/SourceDLSSGSession.h"
#include "FrameGen/SourceDLSSGInterop.h"
#include "NeuralRendering/PostUpscale.h"
#include "NeuralRendering/RuntimeFileLease.h"
#include "PluginPaths.h"
#include "GpuProbeLifetime.h"
#include "TaggedSourceReader.h"
#include "VendorPresentation.h"
#include "QueuedPresentationGate.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <d3d11sdklayers.h>
#include <cstdio>
#include <cstring>
#include <vector>
using namespace TheosRenderPipeline;
using Microsoft::WRL::ComPtr;
namespace NR=NeuralRendering;
namespace SL=SourceDLSSG;
namespace {
unsigned failures{};
enum class ReaderFault {None,MissingFence,StateError,Callback};
bool wrongRealSource{},waitForFocus{};unsigned framesPerExtent=64;std::shared_ptr<VendorPresentation> vendor;
bool vendorLifecycle{},omitLifecycleGates{};unsigned vendorGates{},vendorPending{},vendorReuse{},vendorReentry{};
bool scriptedReentry{},reuseRetiredPreparation{};unsigned sourceReentries{};
bool LifecycleGate(unsigned local){return local==63||local==95||local==127||local==223;}
bool readerMode{},omitReaderWait{};ReaderFault readerFault{};unsigned readerBoundaries{},readerReuses{},readerDrainedEpochs{},quarantines{};uint64_t readerExact{},readerExpected{};
void Check(bool ok,const char* why){if(!ok){++failures;std::fprintf(stderr,"FAIL %s\n",why);}}
void Gpu(HRESULT hr){if(FAILED(hr))throw hr;}
template<class T>T Neural(NR::Result<T> value){if(!value)throw std::runtime_error(value.error().message);return std::move(*value);}
void Neural(NR::Result<void> value){if(!value)throw std::runtime_error(value.error().message);}
struct Surface {
    ComPtr<ID3D11Texture2D> texture;
    Surface(ID3D11Device* device,UINT width,UINT height,DXGI_FORMAT format,UINT bind){
        D3D11_TEXTURE2D_DESC d{};d.Width=width;d.Height=height;d.Format=format;d.MipLevels=d.ArraySize=d.SampleDesc.Count=1;d.BindFlags=bind;
        Gpu(device->CreateTexture2D(&d,nullptr,&texture));
    }
};
std::vector<unsigned char> Read(ID3D11DeviceContext* context,ID3D11Texture2D* texture){
    ComPtr<ID3D11Device> device;context->GetDevice(&device);D3D11_TEXTURE2D_DESC d{};texture->GetDesc(&d);
    const auto width=d.Width,height=d.Height;d.Usage=D3D11_USAGE_STAGING;d.CPUAccessFlags=D3D11_CPU_ACCESS_READ;d.BindFlags=d.MiscFlags=0;
    ComPtr<ID3D11Texture2D> staging;Gpu(device->CreateTexture2D(&d,nullptr,&staging));context->CopyResource(staging.Get(),texture);
    D3D11_MAPPED_SUBRESOURCE mapped{};Gpu(context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped));std::vector<unsigned char> result(size_t(width)*height*4);
    for(UINT y=0;y<height;++y)std::memcpy(result.data()+size_t(y)*width*4,static_cast<const unsigned char*>(mapped.pData)+size_t(y)*mapped.RowPitch,width*4);
    context->Unmap(staging.Get(),0);return result;
}
// The Session is production code. Its public API sink observes actual tagged
// shared resources. Default/reader cases are scripted; vendor cases forward
// every public API call to pinned Streamline and execute actual proxy Presents.
struct Token:sl::FrameToken{unsigned index{};operator uint32_t()const override{return index;}};
struct TagSink {
    Token token;SL::Interop* bridge{};std::vector<unsigned char> expected,realExpected;
    Graphics::SharedTexture presentInput;
    ComPtr<ID3D12Resource> capture;UINT width{},height{},pitch{};unsigned tags{},clears{},fgOn{},fgOff{};uint64_t exact{},expectedBytes{},realExact{},realBytes{};
    std::shared_ptr<TaggedSourceReader> reader;bool faultArmed{},observedFault{};unsigned tokens{},waitCalls{};
    std::shared_ptr<QueuedPresentationGate> presentationGate;
    std::vector<ComPtr<ID3D12Fence>>* retainedVendorFences{};
    ComPtr<ID3D12Fence> pendingVendorFence;uint64_t pendingVendorValue{};bool vendorGateArmed{},vendorReuseExpected{};
    void Allocate(ID3D12Device* device,UINT w,UINT h){width=w;height=h;pitch=(w*4+255)&~255;
        D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_READBACK;D3D12_RESOURCE_DESC d{};d.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;
        d.Width=uint64_t(pitch)*h;d.Height=1;d.DepthOrArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        capture.Reset();Gpu(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&capture)));
    }
    void Capture(ID3D12GraphicsCommandList* list,ID3D12Resource* resource){
        D3D12_RESOURCE_BARRIER barrier{};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;barrier.Transition={resource,D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_SOURCE};list->ResourceBarrier(1,&barrier);
        D3D12_TEXTURE_COPY_LOCATION from{};from.pResource=resource;from.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        D3D12_TEXTURE_COPY_LOCATION to{};to.pResource=capture.Get();to.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;to.PlacedFootprint.Footprint={DXGI_FORMAT_R8G8B8A8_UNORM,width,height,1,pitch};
        list->CopyTextureRegion(&to,0,0,0,&from,nullptr);std::swap(barrier.Transition.StateBefore,barrier.Transition.StateAfter);list->ResourceBarrier(1,&barrier);
    }
    void Verify(bool real=false){Gpu(bridge->Drain());D3D12_RANGE range{0,size_t(pitch)*height};void* data{};Gpu(capture->Map(0,&range,&data));
        const auto& reference=real?realExpected:expected;auto& matched=real?realExact:exact;
        for(UINT y=0;y<height;++y)for(UINT x=0;x<width*4;++x)matched+=static_cast<unsigned char*>(data)[size_t(y)*pitch+x]==reference[size_t(y)*width*4+x];
        (real?realBytes:expectedBytes)+=reference.size();D3D12_RANGE noWrite{};capture->Unmap(0,&noWrite);
    }
};
TagSink* sink{};
sl::Result TokenFn(sl::FrameToken*& token,const uint32_t* index){++sink->tokens;if(vendor)return vendor->api.newFrameToken(token,index);sink->token.index=*index;token=&sink->token;return sl::Result::eOk;}
sl::Result ConstantsFn(const sl::Constants& c,const sl::FrameToken& token,const sl::ViewportHandle& viewport){Check(SL::Session::ValidConstants(c),"ActualSourceConstantsValid");return vendor?vendor->api.setConstants(c,token,viewport):sl::Result::eOk;}
sl::Result TagsFn(const sl::ViewportHandle& viewport,const sl::ResourceTag* tags,uint32_t count,sl::CommandBuffer* command){
    if(!command){++sink->clears;for(unsigned i=0;i<count;++i)Check(!tags[i].resource,"RetiredTagsExplicitlyCleared");return vendor?vendor->api.setTag(viewport,tags,count,command):sl::Result::eOk;}
    const sl::ResourceTag* color{};for(unsigned i=0;i<count;++i)if(tags[i].type==sl::kBufferTypeHUDLessColor)color=&tags[i];
    Check(color&&color->resource&&color->lifecycle==sl::ResourceLifecycle::eValidUntilPresent,"ActualHudlessTagHasPresentLifetime");
    if(!color||!color->resource)return sl::Result::eErrorInvalidParameter;
    auto* resource=static_cast<ID3D12Resource*>(color->resource->native);const auto d=resource->GetDesc();
    Check(d.Width==sink->width&&d.Height==sink->height&&d.Format==DXGI_FORMAT_R8G8B8A8_UNORM,"TagResourceMatchesActualDisplayAllocation");
    auto* list=reinterpret_cast<ID3D12GraphicsCommandList*>(command);
    sink->Capture(list,resource);++sink->tags;return vendor?vendor->api.setTag(viewport,tags,count,command):sl::Result::eOk;
}
sl::Result OptionsFn(const sl::ViewportHandle& viewport,const sl::DLSSGOptions& o){if(SL::GenerationEnabled(o.mode))++sink->fgOn;else ++sink->fgOff;if(vendor){auto options=o;options.onErrorCallback=VendorPresentation::ApiError;return vendor->api.setOptions(viewport,options);}return sl::Result::eOk;}
sl::Result StateFn(const sl::ViewportHandle& viewport,sl::DLSSGState& s,const sl::DLSSGOptions* options){
    if(vendor){const auto result=vendor->api.getState(viewport,s,options);
        if(sink->vendorGateArmed){sink->vendorGateArmed=false;
            const bool valid=(result==sl::Result::eOk||result==sl::Result::eWarnOutOfVRAM)&&s.inputsProcessingCompletionFence&&s.lastPresentInputsProcessingCompletionFenceValue;
            auto* fence=valid?static_cast<ID3D12Fence*>(s.inputsProcessingCompletionFence):nullptr;
            const auto complete=fence?fence->GetCompletedValue():0;
            const bool pending=valid&&complete!=UINT64_MAX&&complete<s.lastPresentInputsProcessingCompletionFenceValue;
            Check(pending,"ActualVendorInputFenceGenuinelyPendingAtHeldPresent");
            std::printf("VENDOR_PENDING gate=%u pending=%u complete=%llu value=%llu actual=%u\n",vendorGates,pending,(unsigned long long)complete,(unsigned long long)s.lastPresentInputsProcessingCompletionFenceValue,s.numFramesActuallyPresented);
            if(pending){++vendorPending;sink->pendingVendorFence=fence;sink->pendingVendorValue=s.lastPresentInputsProcessingCompletionFenceValue;
                sink->vendorReuseExpected=true;sink->retainedVendorFences->push_back(fence);}
        }return result;}
    if(sink->faultArmed&&readerFault==ReaderFault::StateError)return sl::Result::eErrorInvalidParameter;
    s={};s.status=sl::DLSSGStatus::eOk;s.numFramesToGenerateMax=1;s.minWidthOrHeight=1;
    if(sink->reader){s.inputsProcessingCompletionFence=sink->faultArmed&&readerFault==ReaderFault::MissingFence?nullptr:sink->reader->Fence();s.lastPresentInputsProcessingCompletionFenceValue=sink->reader->Value();}
    return sl::Result::eOk;
}
sl::Result ReflexFn(const sl::ReflexOptions& options){return vendor?vendor->api.setReflexOptions(options):sl::Result::eOk;}
sl::Result SleepFn(const sl::FrameToken& token){return vendor?vendor->api.reflexSleep(token):sl::Result::eOk;}
sl::Result MarkerFn(sl::PCLMarker marker,const sl::FrameToken& token){return vendor?vendor->api.marker(marker,token):sl::Result::eOk;}
bool WaitFn(void* opaque,void* fence,uint64_t value){auto& observer=*static_cast<TagSink*>(opaque);++observer.waitCalls;if(observer.faultArmed&&readerFault==ReaderFault::Callback)return false;
    // Explicit test-only negative control; never used by a passing CTest case.
    if(omitReaderWait&&value)return true;
    return SUCCEEDED(observer.bridge->WaitForInputReaders(static_cast<ID3D12Fence*>(fence),value));}
sl::Constants Constants(UINT width,UINT height,bool reset){sl::Constants c{};
    for(auto* matrix:{&c.cameraViewToClip,&c.clipToCameraView,&c.clipToPrevClip,&c.prevClipToClip})for(unsigned i=0;i<4;++i)matrix->row[i]={i==0?1.f:0.f,i==1?1.f:0.f,i==2?1.f:0.f,i==3?1.f:0.f};
    c.cameraPinholeOffset={0,0};c.jitterOffset={0,0};c.mvecScale={1,1};c.cameraPos={0,0,0};c.cameraUp={0,1,0};c.cameraRight={1,0,0};c.cameraFwd={0,0,1};
    c.cameraNear=.1f;c.cameraFar=100;c.cameraFOV=1;c.cameraAspectRatio=float(width)/height;c.depthInverted=sl::eFalse;c.cameraMotionIncluded=sl::eTrue;
    c.motionVectors3D=sl::eFalse;c.reset=reset?sl::eTrue:sl::eFalse;c.orthographicProjection=c.motionVectorsDilated=c.motionVectorsJittered=sl::eFalse;return c;
}
struct Stats{unsigned sources{},nr{},hud{},frozen{},sr{},post{},prepared{},camera{},bypassFrames{},fgOffNr{};uint64_t alpha{},pixels{},changed{},bypass{};};
struct SubmittedResources {
    std::vector<ComPtr<ID3D11Texture2D>> textures11;
    std::vector<ComPtr<ID3D12Resource>> textures12;
    ComPtr<ID3D12Device> device;ComPtr<ID3D12CommandQueue> queue;
    std::shared_ptr<NR::RuntimeOwner> owner;
    std::shared_ptr<VendorPresentation> vendor;
    std::shared_ptr<TaggedSourceReader> reader;std::shared_ptr<int> lifetimeToken;
    std::shared_ptr<QueuedPresentationGate> presentationGate;std::vector<ComPtr<ID3D12Fence>> vendorFences;
};
struct Operations {
    ID3D11DeviceContext* context;std::unique_ptr<NR::PostUpscale>& post;NR::SettingsSnapshot& settings;SL::Interop& bridge;SL::Session& session;TagSink& observer;Stats& stats;
    Graphics::SharedTexture colorTag,motionTag,depthTag,uiTag;UINT width,height;uint64_t epoch{},source{};bool fg{};
    std::vector<unsigned char> original,enhanced,frozen,expectedUi;
    void CopyInput(ID3D11DeviceContext* c,const SourceNvidiaFrameInputs& f){c->CopyResource(f.input,f.color);frozen=Read(c,f.input);}
    bool EvaluateNeuralBeforeDLSS(SourceNvidiaFrameInputs&){return true;}
    void RenderReShade(const SourceNvidiaFrameInputs&,bool){}
    bool EvaluateDLSS(const SourceNvidiaFrameInputs& f){return DLSSBackend::GetSingleton()->Evaluate(f.input,f.motion,f.depth,f.output,width,height,0,0,0,float(width),float(height),f.reset);}
    void UpscaleSucceeded(){++stats.sr;}
    bool EvaluateNeuralAfterDLSS(SourceNvidiaFrameInputs& f,Upscaling::UpscaleOutcome outcome){
        ++stats.post;original=Read(context,f.output);NR::PostSrInput input;auto& r=input.resources;
        r.context=context;r.color=f.output;r.depth=f.depth;r.motion=f.motion;r.epoch=r.guideEpoch=epoch;r.sourceId=r.guideSourceId=source;r.previousSourceId=source-1;
        r.presentationTime=double(source)/60.;r.colorExtent=r.guideExtent={width,height};r.colorDomain=NR::ColorDomain::SdrBytes;r.motionScaleX=float(width);r.motionScaleY=float(height);r.reset=f.reset;
        auto& m=input.source;m.backend=Upscaling::BackendKind::Dlaa;m.outcome=outcome;m.epoch=m.guideEpoch=epoch;m.sourceId=m.guideSourceId=source;m.previousSourceId=source-1;
        m.sourceTime=m.guideTime=r.presentationTime;m.render=m.display=m.color=m.guides={width,height};m.colorDomain=NR::ColorDomain::SdrBytes;m.encoding=Upscaling::ColorEncoding::Gamma22;
        m.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;m.depthFormat=DXGI_FORMAT_R32_FLOAT;m.motionFormat=DXGI_FORMAT_R16G16_FLOAT;m.guideOrigin=NR::GuideOrigin::RealSource;m.motion={float(width),float(height),true,false};
        auto result=Neural(post->Evaluate(input,settings));Check(result.evaluated==settings.enabled,"OneNrAfterActualDlaaSource");Neural(post->WaitDelivery(result));f.reset|=result.effectiveReset;
        enhanced=Read(context,f.output);stats.nr+=result.evaluated;stats.fgOffNr+=result.evaluated&&!fg;stats.frozen+=Read(context,f.input)==frozen;
        stats.pixels+=original.size()/4;for(size_t i=0;i<original.size()/4;++i){stats.alpha+=original[i*4+3]==enhanced[i*4+3];stats.changed+=std::memcmp(&original[i*4],&enhanced[i*4],3)!=0;}
        if(!settings.enabled){Check(enhanced==original,"NrOffLeavesActualDlaaOutputUnchanged");stats.bypass+=enhanced.size();++stats.bypassFrames;}return true;
    }
    bool CaptureCamera(const SourceNvidiaFrameGuides&){++stats.camera;return true;}
    bool Prepare(const SourceNvidiaFrameGuides& f){
        ++stats.prepared;Check(f.hudLessColor!=f.uiColorAndAlpha,"NativeUiSeparatedFromEnhancedSource");observer.expected=enhanced;
        stats.hud+=Read(context,f.uiColorAndAlpha)==expectedUi;
        Gpu(bridge.CopyInput(f.hudLessColor,colorTag));Gpu(bridge.CopyInput(f.motion,motionTag));Gpu(bridge.CopyInput(f.depth,depthTag));Gpu(bridge.CopyInput(f.uiColorAndAlpha,uiTag));
        if(vendor){
            // Synthetic native UI composition: full real source and separate
            // premultiplied UI tag both derive from the same known layers.
            auto composed=enhanced;for(size_t i=0;i<composed.size()/4;++i){const unsigned alpha=expectedUi[i*4+3];
                for(unsigned c=0;c<3;++c)composed[i*4+c]=static_cast<unsigned char>(expectedUi[i*4+c]+(unsigned(enhanced[i*4+c])*(255-alpha)+127)/255);
                composed[i*4+3]=255;}
            observer.realExpected=std::move(composed);context->UpdateSubresource(observer.presentInput.texture11.Get(),0,nullptr,observer.realExpected.data(),width*4,0);
        }
        Gpu(bridge.SignalD3D11(SL::Work::FrameGeneration));ID3D12GraphicsCommandList* list{};Gpu(bridge.Begin(SL::Work::FrameGeneration,&list));
        auto tag=[](const Graphics::SharedTexture& pair){SL::TaggedTexture t;t.resource=sl::Resource(sl::ResourceType::eTex2d,pair.texture12.Get(),D3D12_RESOURCE_STATE_COMMON);t.resource.width=pair.desc.Width;t.resource.height=pair.desc.Height;t.extent={0,0,pair.desc.Width,pair.desc.Height};return t;};
        SL::FrameGuides guides;guides.motion=tag(motionTag);guides.depth=tag(depthTag);guides.ui=tag(uiTag);guides.hudless=tag(colorTag);guides.displayWidth=width;guides.displayHeight=height;
        Check(session.Prepare(Constants(width,height,f.reset),guides,list),"ProductionSessionAcceptsCompletedSourceTags");Gpu(bridge.Submit(SL::Work::FrameGeneration));observer.Verify();return true;
    }
    void PublishGeneration(bool ready){
        Check(ready&&session.CompleteInputWrites()&&session.BeforePresent(fg&&!(vendor&&vendor->forceOff)),"ProductionSessionPrePresentLifecycle");
        const bool armed=readerMode&&(source%64==32||source%64==0);
        if(armed){
            observer.reader->Arm(colorTag.texture12.Get(),enhanced);Check(observer.reader->Pending(),"EnhancedSourceGpuReaderActuallyPendingBeforeStateQuery");++readerBoundaries;
            observer.faultArmed=readerFault!=ReaderFault::None&&source==framesPerExtent*2;
        }
        bool presented=true;
        if(vendor){
            Gpu(bridge.SignalD3D11(SL::Work::SwapChain));ID3D12GraphicsCommandList* list{};Gpu(bridge.Begin(SL::Work::SwapChain,&list));
            ComPtr<ID3D12Resource> destination;Gpu(vendor->chain->GetBuffer(vendor->chain->GetCurrentBackBufferIndex(),IID_PPV_ARGS(&destination)));
            Gpu(bridge.RecordCopy(list,wrongRealSource?colorTag.texture12.Get():observer.presentInput.texture12.Get(),destination.Get()));observer.Capture(list,destination.Get());Gpu(bridge.Submit(SL::Work::SwapChain));observer.Verify(true);
            if(vendorLifecycle&&!omitLifecycleGates&&LifecycleGate(unsigned((source-1)%framesPerExtent))){
                observer.presentationGate->Arm(bridge.Queue());observer.vendorGateArmed=true;++vendorGates;}
            Gpu(vendor->Present());
        }
        const bool accepted=session.AfterPresent(presented);
        if(vendor){const unsigned local=unsigned((source-1)%framesPerExtent);vendor->Sample(session.Snapshot(),source>framesPerExtent?3:(local<16?0:local<framesPerExtent-16?1:2));
            if(!accepted){const auto& state=session.Snapshot();std::printf("VENDOR_SESSION_FAILURE operation=%s failure=%u result=%u frame=%u\n",state.operation,unsigned(state.failure),unsigned(state.result),state.frameIndex);throw std::runtime_error("Actual vendor session failed");}
        }
        if(observer.faultArmed){
            Check(!accepted&&session.Snapshot().stage==SL::SessionStage::Faulted,"ReaderFailureStopsSessionBeforeAnyNextProducer");
            const auto expected=readerFault==ReaderFault::StateError?SL::SessionFailure::Streamline:SL::SessionFailure::InputRetirement;
            Check(session.Snapshot().failure==expected,"ReaderFailureKeepsOriginalReason");
            const auto tokens=observer.tokens,waits=observer.waitCalls;
            Check(!session.Prepare(Constants(width,height,false),{},nullptr)&&!session.BeforePresent(true)&&!session.ResumeAfterResize(),"FaultedSessionRejectsReusePresentAndResume");
            Check(observer.tokens==tokens&&observer.waitCalls==waits,"FaultedSessionDoesNotRetryApiOrMintProducerToken");observer.observedFault=true;
        }else{Check(accepted,"ProductionSessionSourceLifecycleWithFgOffOn");}
        if(armed)Check(observer.reader->Pending(),"ReaderStillPendingWhenSessionReturnsToCaller");
    }
};
}
int wmain(int argc,wchar_t** argv){try{
    std::setvbuf(stdout,nullptr,_IONBF,0);if(argc<3||argc>5||NrRuntimeResearch::GameRunningOrUnknown()){std::puts("REFUSED: arguments/game inventory");return 1;}
    if(argc==5){const std::wstring_view mode=argv[3];
        vendorLifecycle=mode==L"--vendor-lifecycle"||mode==L"--vendor-lifecycle-visible"||mode==L"--vendor-lifecycle-omit-gates"||mode==L"--vendor-lifecycle-omit-gates-visible";
        if(!vendorLifecycle&&mode!=L"--vendor"&&mode!=L"--vendor-force-off"&&mode!=L"--vendor-wrong-real-source"&&mode!=L"--vendor-visible")return 1;
        omitLifecycleGates=mode==L"--vendor-lifecycle-omit-gates"||mode==L"--vendor-lifecycle-omit-gates-visible";
        waitForFocus=mode==L"--vendor-visible"||mode==L"--vendor-lifecycle-visible"||mode==L"--vendor-lifecycle-omit-gates-visible";
        framesPerExtent=240;vendor=std::make_shared<VendorPresentation>();vendor->forceOff=mode==L"--vendor-force-off";wrongRealSource=mode==L"--vendor-wrong-real-source";}
    if(argc==4){const std::wstring_view mode=argv[3];readerMode=true;
        if(mode==L"--missing-reader-fence")readerFault=ReaderFault::MissingFence;else if(mode==L"--reader-state-error")readerFault=ReaderFault::StateError;
        else if(mode==L"--reader-callback-error")readerFault=ReaderFault::Callback;else if(mode==L"--omit-reader-wait")omitReaderWait=true;
        else if(mode==L"--reentry"||mode==L"--reentry-reuse-retired"){scriptedReentry=true;readerMode=false;reuseRetiredPreparation=mode==L"--reentry-reuse-retired";}
        else if(mode!=L"--readers")return 1;}
    const NR::RuntimeProfile srPin{"dlss-sr-probe","nvngx_dlss.dll","c85f971ce023c9f3492fc7455f0b01a24ba18ea39636407a846902c4360b0b7e",58956400};
    const auto srPath=PluginPaths::Directory()/L"TheosRenderPipeline"/L"nvngx_dlss.dll";
    auto srLease=Neural(NR::RuntimeFileLease::Open(srPath,srPin));
    ComPtr<IDXGIFactory6> factory;Gpu(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 desc{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> candidate;auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));if(hr==DXGI_ERROR_NOT_FOUND)break;Gpu(hr);Gpu(candidate->GetDesc1(&desc));if(desc.VendorId==0x10de&&desc.DeviceId==0x2702){adapter=candidate;break;}}
    if(!adapter)throw std::runtime_error("Qualified RTX40 unavailable");
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    auto deviceResult=D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,D3D11_CREATE_DEVICE_DEBUG,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context);
    if(deviceResult==DXGI_ERROR_SDK_COMPONENT_MISSING)deviceResult=D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context);Gpu(deviceResult);
    ComPtr<ID3D11InfoQueue> diagnostics;device.As(&diagnostics);std::printf("GRAPHICS_DEBUG d3d11=%u\n",bool(diagnostics));
    NR::StageContract contract;contract.adapterLuid={desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart};Gpu(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&contract.device)));
    if(vendor)vendor->Load(std::filesystem::absolute(argv[4]),contract.device.Get());
    D3D12_COMMAND_QUEUE_DESC q{};Gpu(contract.device->CreateCommandQueue(&q,IID_PPV_ARGS(&contract.queue)));SL::Interop bridge;Gpu(bridge.Initialize(device.Get(),contract.device.Get(),contract.queue.Get()));
    auto owner=std::make_shared<NR::RuntimeOwner>(NR::RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-dlaa-source-cache"),true});
    Neural(owner->Open(NR::RuntimeCatalog()[1],contract.device.Get(),{desc.VendorId,desc.DeviceId,desc.SubSysId,contract.adapterLuid,false}));
    TagSink observer;observer.bridge=&bridge;sink=&observer;SL::Session session;SL::SessionAPI api;api.newFrameToken=TokenFn;api.setConstants=ConstantsFn;api.setTag=TagsFn;
    api.setReflexOptions=ReflexFn;api.reflexSleep=SleepFn;api.marker=MarkerFn;api.setOptions=OptionsFn;api.getState=StateFn;api.waitForInputReaders=WaitFn;api.context=&observer;if(vendor){api.getReflexState=vendor->api.getReflexState;vendor->Create(factory.Get(),contract.queue.Get(),1280,720);}
    if(vendor){try{std::puts("FOREGROUND_REQUIRED click the isolated probe window; keep it foreground through both size epochs");vendor->RequireForeground(waitForFocus);}catch(const VendorPresentation::ForegroundUnavailable&){vendor->Close();throw;}session.RequestUIRecomposition(true);}
    Check(session.Start(api,vendor?1:0),"SessionStartsWithActualInteropReaderBridge");auto* dlss=DLSSBackend::GetSingleton();dlss->SetupDevice(device.Get(),context.Get());Stats stats;
    if(vendor){auto warmResources=std::make_unique<SubmittedResources>();warmResources->device=contract.device;warmResources->queue=contract.queue;warmResources->vendor=vendor;
        ComPtr<ID3D12DescriptorHeap> heap;D3D12_DESCRIPTOR_HEAP_DESC hd{};hd.Type=D3D12_DESCRIPTOR_HEAP_TYPE_RTV;hd.NumDescriptors=2;Gpu(contract.device->CreateDescriptorHeap(&hd,IID_PPV_ARGS(&heap)));
        const auto stride=contract.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        for(unsigned i=0;i<2;++i){ComPtr<ID3D12Resource> back;Gpu(vendor->chain->GetBuffer(i,IID_PPV_ARGS(&back)));warmResources->textures12.push_back(back);auto handle=heap->GetCPUDescriptorHandleForHeapStart();handle.ptr+=i*stride;contract.device->CreateRenderTargetView(back.Get(),nullptr,handle);}
        // Descriptor heap must survive uncertain queued ClearRenderTargetView.
        struct WarmCapsule{std::unique_ptr<SubmittedResources> sources;ComPtr<ID3D12DescriptorHeap> heap;};
        auto capsule=std::make_unique<WarmCapsule>();capsule->sources=std::move(warmResources);capsule->heap=heap;
        GpuProbeLifetime<WarmCapsule> warmLifetime(std::move(capsule),[&]{return SUCCEEDED(bridge.Drain());});
        for(unsigned warmup=0;warmup<600;++warmup){
        // Real generation-off Presents, matching the production host warm-up.
        ID3D12GraphicsCommandList* list{};Gpu(bridge.Begin(SL::Work::SwapChain,&list));
        ComPtr<ID3D12Resource> back;Gpu(vendor->chain->GetBuffer(vendor->chain->GetCurrentBackBufferIndex(),IID_PPV_ARGS(&back)));
        auto handle=heap->GetCPUDescriptorHandleForHeapStart();handle.ptr+=vendor->chain->GetCurrentBackBufferIndex()*stride;
        D3D12_RESOURCE_BARRIER b{};b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;b.Transition={back.Get(),D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_RENDER_TARGET};list->ResourceBarrier(1,&b);
        const float clear[]{.1f,.1f,.1f,1};list->ClearRenderTargetView(handle,clear,0,nullptr);std::swap(b.Transition.StateBefore,b.Transition.StateAfter);list->ResourceBarrier(1,&b);
        Gpu(bridge.Submit(SL::Work::SwapChain));if(!session.BeforePresent(false))throw std::runtime_error("Warm-up BeforePresent failed");Gpu(vendor->Present());if(!session.AfterPresent(true))throw std::runtime_error("Warm-up AfterPresent failed");Gpu(bridge.Drain());
    }if(!warmLifetime.Retire())throw std::runtime_error("Warm-up resources quarantined");std::puts("VENDOR_WARMUP actualPresents=600 generation=off");}
    for(unsigned cycle=0;cycle<2;++cycle){const UINT width=vendor?(cycle?1344:1280):(cycle?384:320),height=vendor?(cycle?756:720):(cycle?216:180);contract.colorExtent=contract.guideExtent={width,height};
        auto post=std::make_unique<NR::PostUpscale>();Neural(post->Initialize(owner,device.Get(),contract));observer.Allocate(contract.device.Get(),width,height);
        if(vendor&&cycle)vendor->Resize(width,height);
        if(readerMode)observer.reader=std::make_shared<TaggedSourceReader>(contract.device.Get(),width,height);
        Surface world(device.Get(),width,height,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE),
            input(device.Get(),width,height,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE),
            output(device.Get(),width,height,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE),
            motion(device.Get(),width,height,DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE),depth(device.Get(),width,height,DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE),
            ui(device.Get(),width,height,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE);
        std::vector<unsigned short> velocity(size_t(width)*height*2);context->UpdateSubresource(motion.texture.Get(),0,nullptr,velocity.data(),width*4,0);
        std::vector<float> z(size_t(width)*height,.5f);context->UpdateSubresource(depth.texture.Get(),0,nullptr,z.data(),width*4,0);
        std::vector<unsigned char> hud(size_t(width)*height*4);for(size_t i=0;i<hud.size()/4;++i){hud[i*4]=(i%width)<20?255:0;hud[i*4+3]=(i%width)<20?255:0;}context->UpdateSubresource(ui.texture.Get(),0,nullptr,hud.data(),width*4,0);
        if(!dlss->InitUpscale(width,height,width,height,DXGI_FORMAT_R8G8B8A8_UNORM,false,true,0,5))throw std::runtime_error("Actual production DLAA creation failed");
        Check(srLease.Matches(PluginPaths::ModulePath(GetModuleHandleW(L"nvngx_dlss.dll"))),"LoadedDlaaRuntimeMatchesHeldQualifiedFile");
        NR::SettingsSnapshot settings;settings.placement=NR::Placement::After;settings.stableColors=false;
        Operations ops{context.Get(),post,settings,bridge,session,observer,stats,{},{},{},{},width,height,cycle+1};
        ops.expectedUi=hud;
        for(auto* pair:{&ops.colorTag,&ops.uiTag,&ops.depthTag,&ops.motionTag}){D3D11_TEXTURE2D_DESC d{};d.Width=width;d.Height=height;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=pair==&ops.motionTag?DXGI_FORMAT_R16G16_FLOAT:pair==&ops.depthTag?DXGI_FORMAT_R32_FLOAT:DXGI_FORMAT_R8G8B8A8_UNORM;d.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;if(pair==&ops.depthTag)d.BindFlags|=D3D11_BIND_UNORDERED_ACCESS;const auto hr=bridge.CreateSharedTexture(d,*pair);if(FAILED(hr))std::printf("SHARED_ALLOCATION format=%u bind=%u result=0x%08x\n",unsigned(d.Format),d.BindFlags,unsigned(hr));Gpu(hr);}
        if(vendor){D3D11_TEXTURE2D_DESC d{};d.Width=width;d.Height=height;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;Gpu(bridge.CreateSharedTexture(d,observer.presentInput));}
        auto resources=std::make_unique<SubmittedResources>();resources->vendor=vendor;
        if(vendor){resources->textures11.push_back(observer.presentInput.texture11);resources->textures12.push_back(observer.presentInput.texture12);
            for(unsigned index=0;index<2;++index){ComPtr<ID3D12Resource> back;Gpu(vendor->chain->GetBuffer(index,IID_PPV_ARGS(&back)));resources->textures12.push_back(back);}}resources->device=contract.device;resources->queue=contract.queue;resources->owner=owner;
        for(auto* surface:{&world,&input,&output,&motion,&depth,&ui})resources->textures11.push_back(surface->texture);
        for(auto* pair:{&ops.colorTag,&ops.uiTag,&ops.depthTag,&ops.motionTag}){resources->textures11.push_back(pair->texture11);resources->textures12.push_back(pair->texture12);}
        resources->textures12.push_back(observer.capture);
        resources->reader=observer.reader;resources->lifetimeToken=std::make_shared<int>(1);std::weak_ptr<int> resourceLifetime=resources->lifetimeToken;unsigned retirementCalls{};
        if(vendorLifecycle){observer.presentationGate=std::make_shared<QueuedPresentationGate>(contract.device.Get());
            resources->presentationGate=observer.presentationGate;observer.retainedVendorFences=&resources->vendorFences;}
        // Declared after ops: retire/quarantine all submitted external resources
        // before ops, surfaces or readback storage unwind on any failure.
        GpuProbeLifetime<SubmittedResources> lifetime(std::move(resources),[&]{
            ++retirementCalls;
            if(session.Snapshot().stage!=SL::SessionStage::Stopped&&!session.Stop())return false;
            (void)bridge.DiscardUnsubmitted(SL::Work::FrameGeneration);
            if(FAILED(bridge.SignalD3D11(SL::Work::FrameGeneration))||FAILED(bridge.Drain()))return false;
            if(observer.reader)observer.reader->Verify();
            if(!post->Retire())return false;dlss->ReleaseFeature();return true;
        });
        for(unsigned local=0;local<framesPerExtent;++local){const auto sourceStart=std::chrono::steady_clock::now();ops.source=++stats.sources;ops.fg=local>=16&&local<framesPerExtent-16;
            const auto previousSettings=settings;settings.enabled=local!=40&&!(vendorLifecycle&&local==128);
            if(vendorLifecycle){settings.tuning.localToneStrength=local>=64?.65f:1.f;settings.tuning.style=local>=96?1:0;}
            const bool settingsChanged=settings.enabled!=previousSettings.enabled||settings.tuning!=previousSettings.tuning;
            settings.revision+=unsigned(settingsChanged);
            const bool vendorReuseExpected=vendorLifecycle&&observer.vendorReuseExpected;
            if(vendorReuseExpected)Check(observer.pendingVendorFence->GetCompletedValue()<observer.pendingVendorValue,"ActualVendorReaderStillPendingBeforeSourceMutationOrTeardown");
            const bool reentryBoundary=(vendorLifecycle&&local==224)||(scriptedReentry&&local==48);
            if(reentryBoundary){
                if(!session.Stop())throw std::runtime_error("Source session stop failed before feature reentry; retirement unconfirmed");
                Gpu(bridge.SignalD3D11(SL::Work::FrameGeneration));Gpu(bridge.Drain());
                if(vendorReuseExpected){Check(observer.pendingVendorFence->GetCompletedValue()>=observer.pendingVendorValue&&std::chrono::steady_clock::now()-sourceStart>=std::chrono::milliseconds(100),"ActualVendorInputReaderRetiresBeforeFeatureRelease");++vendorReentry;observer.vendorReuseExpected=false;}
                Neural(post->Retire());dlss->ReleaseFeature();
                if(!dlss->InitUpscale(width,height,width,height,DXGI_FORMAT_R8G8B8A8_UNORM,false,true,0,5))throw std::runtime_error("Reentry DLAA creation failed");
                // Preparation instances initialize once, even after successful retirement.
                if(!reuseRetiredPreparation)post=std::make_unique<NR::PostUpscale>();
                Neural(post->Initialize(owner,device.Get(),contract));Check(session.ResumeAfterResize(),"SourceSessionResumesAfterFeatureReentry");++sourceReentries;
            }
            std::vector<unsigned char> bytes(size_t(width)*height*4);for(size_t i=0;i<bytes.size()/4;++i){bytes[i*4]=((i/8+ops.source)%2)?48:208;bytes[i*4+1]=(i+ops.source)%256;bytes[i*4+2]=(i/width+ops.source)%256;bytes[i*4+3]=255;}
            const bool reuse=readerMode&&local==32;const auto producerStart=std::chrono::steady_clock::now();if(reuse)Check(observer.reader->Pending(),"PriorEnhancedTagReaderPendingBeforeProducerReuse");context->UpdateSubresource(world.texture.Get(),0,nullptr,bytes.data(),width*4,0);
            SourceNvidiaFrameInputs frame;frame.color=world.texture.Get();frame.input=input.texture.Get();frame.output=frame.hudLessColor=output.texture.Get();frame.depth=depth.texture.Get();frame.motion=motion.texture.Get();frame.uiColorAndAlpha=ui.texture.Get();
            frame.renderWidth=frame.outputWidth=width;frame.renderHeight=frame.outputHeight=height;frame.motionScaleX=float(width);frame.motionScaleY=float(height);frame.reset=local==0||settingsChanged||reentryBoundary;frame.jitterEnabled=true;
            const auto result=SourceNvidiaFrameEvaluator::Evaluate(context.Get(),frame,ops);Check(result.upscaled&&result.cameraValid&&result.prepared,"ActualDlaaSourceFullyPrepared");
            if(vendorReuseExpected&&local!=224){Check(observer.pendingVendorFence->GetCompletedValue()>=observer.pendingVendorValue&&std::chrono::steady_clock::now()-sourceStart>=std::chrono::milliseconds(100),"NewSourceSettingsWaitForGenuineVendorInputReader");++vendorReuse;observer.vendorReuseExpected=false;}
            if(vendor){const auto& snapshot=session.Snapshot();std::printf("VENDOR_FRAME source=%llu configured=%u actual=%u status=%u fenceValue=%llu reset=%u\n",(unsigned long long)ops.source,SL::GenerationEnabled(snapshot.options.mode),snapshot.state.numFramesActuallyPresented,unsigned(snapshot.state.status),(unsigned long long)snapshot.state.lastPresentInputsProcessingCompletionFenceValue,frame.reset);std::this_thread::sleep_until(sourceStart+std::chrono::milliseconds(33));}
            if(reuse){Check(observer.reader->Completed()&&std::chrono::steady_clock::now()-producerStart>=std::chrono::milliseconds(100),"NextSourceGpuWritesWaitForGenuineEnhancedTagReader");observer.reader->Verify();++readerReuses;}
        }
        if(readerMode)Check(observer.reader->Pending(),"EnhancedReaderGenuinelyPendingAtRetireBoundary");
        const auto retirementStart=std::chrono::steady_clock::now();const bool retired=lifetime.Retire();
        if(observer.observedFault){
            Check(!retired&&!resourceLifetime.expired(),"ReaderFailureRetainsEverySubmittedResourceCapsule");
            Check(!lifetime.Retire()&&retirementCalls==1,"FailedRetirementIsTerminalWithoutRetry");
            observer.reader->Verify();Gpu(bridge.Drain());++quarantines;
        }else{
            if(!retired){const auto& state=session.Snapshot();std::printf("RETIREMENT_FAILURE operation=%s failure=%u result=%u\n",state.operation,unsigned(state.failure),unsigned(state.result));throw std::runtime_error("Source retirement unconfirmed; external resources quarantined");}
            Check(resourceLifetime.expired(),"ConfirmedRetirementReleasesSubmissionCapsule");
            if(readerMode)Check(std::chrono::steady_clock::now()-retirementStart>=std::chrono::milliseconds(100),"ConfirmedRetirementWaitsForGenuineTaggedReader");
        }
        if(readerMode){Check(observer.reader->Completed()&&observer.reader->Captures()==2,"EveryEpochOldEnhancedTagCapturedBeforeResourceRebuild");readerExact+=observer.reader->Exact();readerExpected+=observer.reader->Expected();++readerDrainedEpochs;}
        if(vendor){observer.presentInput={};observer.retainedVendorFences=nullptr;Gpu(bridge.Drain());}
        if(cycle==0)Check(session.ResumeAfterResize(),"SameSessionResumesWithMonotonicTokenIdentity");
    }
    if(!observer.observedFault)Neural(owner->Retire());Gpu(bridge.Drain());
    if(vendor){Check(VendorPresentation::apiErrors==0,"NoAsynchronousVendorPresentationApiErrors");
        Check(vendor->presents==600+framesPerExtent*2&&vendor->doubleSamples>=48&&vendor->phaseDoubles[1]>=24&&vendor->phaseDoubles[3]>=24,"ActualVendorPresentsReportGeneratedOutputBeforeAndAfterResize");
        Check(observer.realExact==observer.realBytes&&observer.realBytes==observer.expectedBytes,"ActualNativeBackbufferMatchesEnhancedRealSourceAndNativeUi");
        Check(vendor->offSamples==64&&vendor->onSamples==framesPerExtent*2-64&&!session.Snapshot().optionsWarnings,"ActualVendorOffOnOffOnSourceSchedule");
        std::printf("NVIDIA_VENDOR actualPresents=%u warmup=600 sources=%u on=%u off=%u doubles=%u firstOn=%u resumedOn=%u readerFenceSamples=%u realExactBytes=%llu expectedRealBytes=%llu status=%u failures=%u\n",vendor->presents,stats.sources,vendor->onSamples,vendor->offSamples,vendor->doubleSamples,vendor->phaseDoubles[1],vendor->phaseDoubles[3],vendor->readerSamples,(unsigned long long)observer.realExact,(unsigned long long)observer.realBytes,unsigned(session.Snapshot().state.status),failures);vendor->Close();}
    if(readerMode){Check(readerBoundaries==4&&readerReuses==2&&readerDrainedEpochs==2,"EveryEnhancedSourceReaderBoundaryIsGenuinelyPending");Check(readerExact==readerExpected&&readerExpected==1124352,"OldEnhancedTaggedBytesSurviveReuseAndRetirement");Check(quarantines==unsigned(readerFault!=ReaderFault::None),"OnlyUnconfirmedRetirementQuarantinesSubmissionOwners");}
    const unsigned expectedBypass=vendorLifecycle?4:2;
    Check(stats.sources==framesPerExtent*2&&stats.sr==stats.sources&&stats.nr==stats.sources-expectedBypass&&stats.post==stats.sources&&stats.prepared==stats.sources&&stats.camera==stats.sources,"ActualDlaaNrCountsWithoutDuplicateLegacyPass");
    Check(stats.alpha==stats.pixels&&stats.changed>57600&&stats.frozen==stats.sources&&stats.hud==stats.sources&&stats.bypassFrames==expectedBypass,"SourceAlphaFrozenInputUiAndOffBypass");
    Check(observer.tags==stats.sources&&observer.exact==observer.expectedBytes&&stats.fgOffNr==64,"EveryActualTaggedByteMatchesEnhancedOutputIncludingFgOff");
    // Stop's Off is identical to the upcoming first Off Present, so production
    // Session deduplicates that pending option rather than submitting it twice.
    Check(vendor||(observer.fgOn==64&&observer.fgOff==65-unsigned(observer.observedFault)),"ExplicitSessionFgOffOnOffOptionsSchedule");
    if(diagnostics){unsigned errors{};for(UINT64 i=0;i<diagnostics->GetNumStoredMessagesAllowedByRetrievalFilter();++i){SIZE_T bytes{};Gpu(diagnostics->GetMessage(i,nullptr,&bytes));std::vector<unsigned char> storage(bytes);auto* message=reinterpret_cast<D3D11_MESSAGE*>(storage.data());Gpu(diagnostics->GetMessage(i,message,&bytes));if(message->Severity<=D3D11_MESSAGE_SEVERITY_ERROR){++errors;std::fprintf(stderr,"D3D11: %s\n",message->pDescription);}}Check(errors==0,"AvailableD3d11DebugQueueNoErrors");}
    else std::puts("SKIPPED: D3D11 Graphics Tools debug queue unavailable");
    std::printf("NVIDIA_SOURCE sources=%u dlaa=%u nr=%u tags=%u exactTagBytes=%llu expectedTagBytes=%llu alphaExact=%llu expectedPixels=%llu frozen=%u hud=%u nrWhileFgOff=%u configuredOn=%u configuredOff=%u\n",stats.sources,stats.sr,stats.nr,observer.tags,(unsigned long long)observer.exact,(unsigned long long)observer.expectedBytes,(unsigned long long)stats.alpha,(unsigned long long)stats.pixels,stats.frozen,stats.hud,stats.fgOffNr,observer.fgOn,observer.fgOff);
    if(readerMode)std::printf("NVIDIA_READERS boundaries=%u reuse=%u readerDrainedEpochs=%u exactBytes=%llu expectedBytes=%llu fault=%u omittedWait=%u quarantined=%u failures=%u\n",readerBoundaries,readerReuses,readerDrainedEpochs,(unsigned long long)readerExact,(unsigned long long)readerExpected,unsigned(readerFault),omitReaderWait,quarantines,failures);
    if(vendorLifecycle){Check(vendorGates==8&&vendorPending==8&&vendorReuse==6&&vendorReentry==2,"AllActualVendorPendingSettingsAndFeatureReentryBoundariesObserved");
        std::printf("NVIDIA_VENDOR_LIFECYCLE gates=%u pending=%u reuse=%u reentry=%u omittedGates=%u failures=%u\n",vendorGates,vendorPending,vendorReuse,vendorReentry,omitLifecycleGates,failures);}
    if(scriptedReentry||vendorLifecycle){Check(sourceReentries==2,"FeatureReentryCreatesFreshNrPreparationPerEpoch");std::printf("NVIDIA_SOURCE_REENTRY count=%u reusedRetired=%u failures=%u\n",sourceReentries,reuseRetiredPreparation,failures);}
    if(vendor)std::puts("SCOPE actual DLAA/NR enhanced tags and real public vendor proxy Presents/counters; synthetic native UI composition; no generated pixel readback/cadence/full SDK shutdown claim");else std::puts("SCOPE actual production DLAA+NR+Interop+Session tags; scripted public Streamline API sink, independent real GPU readers, no vendor FG output/cadence/UI composition claim");return failures?1:0;
}catch(const VendorPresentation::ForegroundUnavailable& e){std::printf("NOT_QUALIFIED foreground precondition: %s\n",e.what());return 77;}catch(HRESULT hr){std::printf("GPU failure 0x%08x\n",unsigned(hr));return 1;}catch(const std::exception& e){std::printf("FAIL %s\n",e.what());return 1;}}
