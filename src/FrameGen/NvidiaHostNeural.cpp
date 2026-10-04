#include <PCH.h>
#include "NvidiaHost.h"
#include "SourceFrameGeneration.h"
#include "SourceDLSSGBackend.h"
#include "GameCameraMeasurements.h"
#include "../RenderPipeline.h"
#include "CommunityShaderIntegration.h"
#include "NeuralRendering/BeforeSettings.h"
#include "NeuralRendering/MotionDiagnosticStats.h"
#include "PerformanceTuning.h"
#include "PluginPaths.h"
#include <chrono>
#include <cstring>
#include <vector>

using namespace TheosRenderPipeline;
namespace NR=TheosRenderPipeline::NeuralRendering;
namespace {
using Microsoft::WRL::ComPtr;

struct NrMotionDiagnostic {
    unsigned eligibleFrames{}, samples{};
    bool disabled{};
    void Reset() { eligibleFrames=0; samples=0; disabled=false; }
    bool Due() { return !disabled && samples<16 && (++eligibleFrames==1 || eligibleFrames%120==0); }
    void Failure(const char* step,HRESULT hr) {
        logger::warn("[NR color probe] disabled step={} hr=0x{:08X}; rendering continues",step,static_cast<uint32_t>(hr));
        disabled=true;
    }
    bool CopyToStaging(ID3D11Device* device,ID3D11DeviceContext* context,ID3D11Texture2D* source,
        DXGI_FORMAT expected,ComPtr<ID3D11Texture2D>& staging) {
        if(!device||!context||!source){Failure("missing readback resource",E_INVALIDARG);return false;}
        D3D11_TEXTURE2D_DESC desc{};source->GetDesc(&desc);
        if(desc.Format!=expected || desc.SampleDesc.Count!=1 || desc.MipLevels!=1 || desc.ArraySize!=1){
            Failure("unexpected texture format/shape",E_INVALIDARG);return false;
        }
        desc.Usage=D3D11_USAGE_STAGING;desc.BindFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;desc.MiscFlags=0;
        const auto hr=device->CreateTexture2D(&desc,nullptr,&staging);
        if(FAILED(hr)){Failure("create staging",hr);return false;}
        context->CopyResource(staging.Get(),source);
        return true;
    }
    bool ReadRows(ID3D11DeviceContext* context,ID3D11Texture2D* staging,
        UINT width,UINT height,UINT bytesPerPixel,std::vector<std::uint8_t>& rows) {
        D3D11_MAPPED_SUBRESOURCE mapped{};
        const auto hr=context->Map(staging,0,D3D11_MAP_READ,0,&mapped);
        if(FAILED(hr)){Failure("map staging",hr);return false;}
        const auto rowBytes=std::size_t(width)*bytesPerPixel;
        if(mapped.RowPitch<rowBytes){context->Unmap(staging,0);Failure("short staging pitch",E_INVALIDARG);return false;}
        rows.resize(rowBytes*height);
        for(UINT y=0;y<height;++y)
            std::memcpy(rows.data()+std::size_t(y)*rowBytes,
                static_cast<const std::uint8_t*>(mapped.pData)+std::size_t(y)*mapped.RowPitch,rowBytes);
        context->Unmap(staging,0);
        return true;
    }
    struct Capture {
        std::vector<std::uint8_t> color;
        std::optional<NR::MotionStats> motion;
        DXGI_FORMAT depthFormat{};
        bool ready{};
    };
    Capture Before(ID3D11Device* device,ID3D11DeviceContext* context,ID3D11Texture2D* color,
        ID3D11Texture2D* depth,ID3D11Texture2D* motion,UINT width,UINT height) {
        Capture capture;
        if(!depth||width>4096||height>4096||!width||!height){Failure("invalid diagnostic extent/depth",E_INVALIDARG);return capture;}
        D3D11_TEXTURE2D_DESC depthDesc{};depth->GetDesc(&depthDesc);capture.depthFormat=depthDesc.Format;
        ComPtr<ID3D11Texture2D> colorCopy,motionCopy;
        if(!CopyToStaging(device,context,color,DXGI_FORMAT_R8G8B8A8_UNORM,colorCopy) ||
           !CopyToStaging(device,context,motion,DXGI_FORMAT_R16G16_FLOAT,motionCopy))return capture;
        std::vector<std::uint8_t> rawMotion;
        if(!ReadRows(context,colorCopy.Get(),width,height,4,capture.color) ||
           !ReadRows(context,motionCopy.Get(),width,height,4,rawMotion))return capture;
        capture.motion=NR::SummarizeMotionRg16(rawMotion.data(),std::size_t(width)*4,width,height,width,height);
        capture.ready=bool(capture.motion);
        return capture;
    }
    void After(ID3D11Device* device,ID3D11DeviceContext* context,ID3D11Texture2D* color,
        UINT width,UINT height,uint64_t sourceId,uint64_t revision,bool reset,const Capture& before) {
        if(!before.ready)return;
        ComPtr<ID3D11Texture2D> colorCopy;
        if(!CopyToStaging(device,context,color,DXGI_FORMAT_R8G8B8A8_UNORM,colorCopy))return;
        std::vector<std::uint8_t> after;
        if(!ReadRows(context,colorCopy.Get(),width,height,4,after))return;
        const auto rgb=NR::CompareRgba8(before.color.data(),after.data(),std::size_t(width)*4,std::size_t(width)*4,width,height);
        if(!rgb){Failure("color summary",E_INVALIDARG);return;}
        ++samples;
        logger::info("[NR color probe] sample={}/16 source={} revision={} reset={} extent={}x{} depthFormat={} rgbIn=({:.2f},{:.2f},{:.2f}) rgbOut=({:.2f},{:.2f},{:.2f}) rgbSigned=({:+.2f},{:+.2f},{:+.2f}) rgbAbs=({:.2f},{:.2f},{:.2f}) motionPxMean=({:+.2f},{:+.2f}) motionPxAbs={:.2f} motionSamples={} motionZero={} motionInvalid={} motionOutsideUv={}",
            samples,sourceId,revision,reset,width,height,static_cast<unsigned>(before.depthFormat),
            rgb->before[0],rgb->before[1],rgb->before[2],rgb->after[0],rgb->after[1],rgb->after[2],
            rgb->signedChange[0],rgb->signedChange[1],rgb->signedChange[2],
            rgb->absoluteChange[0],rgb->absoluteChange[1],rgb->absoluteChange[2],
            before.motion->meanXpixels,before.motion->meanYpixels,before.motion->meanMagnitudePixels,before.motion->samples,
            before.motion->zeroVectors,before.motion->nonFinite,before.motion->outsideUv);
    }
};
NrMotionDiagnostic nrMotionDiagnostic;
}
void NvidiaHost::InspectCommunityNeural()
{
    if (!SourceFrameGeneration::GetSingleton()->settings.neuralStartup.community || communityNeural_) return;
    communityNeural_=std::make_unique<NR::BeforeHost>();
    const auto& startup=SourceFrameGeneration::GetSingleton()->settings.neuralStartup;
    const auto cache=PluginPaths::Directory()/"TheosRenderPipeline"/"NR"/"cache";
    ID3D12Device* presenter{};
#if defined(TRP_ENABLE_FSR)
    if(fsrResources_ && fsrResources_->Bridge())presenter=fsrResources_->Bridge()->Device12();
#endif
    if(!presenter)presenter=SourceDLSSG::Backend::Get().Transport().Device12();
    // Never create another injector device proxy while the presenter is live.
    const auto inspected=presenter?communityNeural_->Inspect(device_.Get(),startup,cache,presenter):
        NR::Result<void>{std::unexpected(NR::Error{NR::ErrorKind::Unsupported,0,"NR waiting for presenter D3D12 device"})};
    communityLastStatus_=inspected?communityNeural_->Status():inspected.error().message;
    logger::info("[Community NR startup] available={} profile={} encoding={} sdrBytesTrial={} root={} core={} presenterDevice={} status={}",
        bool(inspected),communityNeural_->ProfileId(),Upscaling::ColorEncodingName(startup.sourceEncoding),startup.sdrBytesTrial,
        startup.runtimeRoot.string(),startup.driverCore.string(),fmt::ptr(presenter),communityLastStatus_);
}
bool NvidiaHost::EvaluateCommunityNeuralBefore(ID3D11Texture2D* color,ID3D11Texture2D* depth,
    ID3D11Texture2D* motion,UINT width,UINT height,uint64_t sourceId,bool& reset,bool eligible,NR::PreparedFsrInput* linearOutput)
{
    const auto& startup=SourceFrameGeneration::GetSingleton()->settings.neuralStartup;
    if (!startup.community) return true;
    InspectCommunityNeural();
    const auto& p=SourceFrameGeneration::GetSingleton()->settings.sourceDLSSG;
    NR::SettingsSnapshot snapshot;
    snapshot.enabled=p.neuralEnabled;snapshot.placement=p.neuralBeforeUpscaling?NR::Placement::Before:NR::Placement::After;
    snapshot.stableColors=p.neuralStableColors;
    snapshot.tuning=p.neuralTuning;snapshot.reconstruction=p.neuralReconstruction;
    const bool changed=!communitySnapshotValid_ || snapshot.enabled!=communitySnapshot_.enabled ||
        snapshot.placement!=communitySnapshot_.placement || snapshot.stableColors!=communitySnapshot_.stableColors || snapshot.tuning!=communitySnapshot_.tuning ||
        snapshot.reconstruction!=communitySnapshot_.reconstruction;
    snapshot.revision=communitySnapshot_.revision+(changed?1:0);
    communitySnapshot_=snapshot;communitySnapshotValid_=true;
    if(changed)logger::info("[Community NR settings] revision={} stableColors={} tone={} structure={} style={}",
        snapshot.revision,snapshot.stableColors,snapshot.tuning.localToneStrength,snapshot.tuning.localStructureStrength,snapshot.tuning.style);
    NR::BeforeInput input;input.context=context_;input.color=color;input.depth=depth;input.motion=motion;
    input.epoch=communityEpoch_;input.sourceId=input.guideSourceId=sourceId;
    input.guideEpoch=input.epoch;input.previousSourceId=sourceId?sourceId-1:0;
    input.colorExtent=input.guideExtent={width,height};input.reset=reset || changed;
    input.presentationTime=std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    input.motionScaleX=float(width);input.motionScaleY=float(height);
    auto* pipeline=RenderPipeline::GetSingleton();
    const char* unavailable=NR::NativeBeforeUnavailable(p);
    if (CommunityShaders::Active() || !nativeUI_.Dedicated() || !pipeline->mNativeUI) eligible=false;
    auto camera=eligible && snapshot.enabled && communityNeural_->Available() && !unavailable?
        CaptureGameCameraMeasurements(pipeline->mGraphicsState,{width,height},pipeline->mEnableJitter,reset):
        Upscaling::Result<Upscaling::CameraMeasurements>{std::unexpected(Upscaling::RuntimeError{Upscaling::ErrorKind::InvalidInput,0,"NR waiting for world camera"})};
    if (camera) {
        const auto decision=communityCameraHistory_.Accept(sourceId,*camera,{width,height});
        eligible=decision.valid;
        input.depthInverted=camera->depthInverted;
        input.reset|=decision.reset;
    } else eligible=false;
    if (!eligible || unavailable) communityCameraHistory_.Invalidate();
    // Reset retained temporal state on menus, invalid camera, or an unavailable
    // request; the user's authoritative enabled/placement choices stay intact.
    if (!eligible || unavailable) snapshot.enabled=false;
    const bool probe=eligible && snapshot.enabled && communityNeural_->Available() &&
        PerformanceTuning::GetSingleton()->settings.diagnostics.frameDetails && nrMotionDiagnostic.Due();
    const auto probeBefore=probe?nrMotionDiagnostic.Before(device_.Get(),context_.Get(),color,depth,motion,width,height):
        NrMotionDiagnostic::Capture{};
    const bool previouslyActive=communityNeural_->Active();
    const auto result=communityNeural_->Evaluate(input,snapshot,probe?nullptr:linearOutput);
    if(FsrActive() && result){
        const auto route=communityFsrRouteDiagnostics_.Observe(result->evaluated,linearOutput&&linearOutput->Valid(),pipeline->mReShadeBeforeUpscaling,probe);
        if(route){
            std::string_view modelHash="unavailable";
            for(const auto& profile:NR::RuntimeCatalog())if(profile.id==communityNeural_->ProfileId()){modelHash=profile.sha256;break;}
            logger::info("[NR->FSR route] source={} route={} ReShadeBefore={} diagnosticCapture={} profile={} modelSha256={} work={}x{} status={}",
                sourceId,NR::FsrNrRouteName(route->route),route->reshadeBefore,probe,communityNeural_->ProfileId(),modelHash,width,height,communityNeural_->Status());
        }
    }
    if(probe && result && result->evaluated)
        nrMotionDiagnostic.After(device_.Get(),context_.Get(),color,width,height,sourceId,snapshot.revision,
            result->effectiveReset,probeBefore);
    std::string status=communityNeural_->Status();
    if (p.neuralEnabled && communityNeural_->Available() && !communityNeural_->Terminal()) {
        if (unavailable) status=unavailable;
        else if (!eligible) status="NR paused; waiting for a valid TRP world frame";
    }
    if (status!=communityLastStatus_ || (result && result->evaluated && communityNeural_->Recorded()%600==0)) {
        logger::info("[Community NR frame] source={} requested={} evaluated={} profile={} revision={} reset={} frames={} resets={} status={}",
            sourceId,p.neuralEnabled,result && result->evaluated,communityNeural_->ProfileId(),snapshot.revision,
            result && result->effectiveReset,communityNeural_->Recorded(),communityNeural_->Resets(),status);
        communityLastStatus_=status;
    }
    if (!result || !result->evaluated) communityCameraHistory_.Invalidate();
    if (!result) {
        logger::error("[Community NR failure] source={} profile={} kind={} native=0x{:08X} extent={}x{} revision={} deviceRemoved=0x{:08X} reason={}",
            sourceId,communityNeural_->ProfileId(),static_cast<unsigned>(result.error().kind),static_cast<uint32_t>(result.error().nativeCode),
            width,height,snapshot.revision,static_cast<uint32_t>(device_->GetDeviceRemovedReason()),result.error().message);
        status_=result.error().message;FailLifecycle(E_FAIL,"community NR source evaluation");return false;}
    reset=NR::SourceResetAfterNr(reset,result->evaluated,previouslyActive,result->effectiveReset);
    return true;
}
bool NvidiaHost::RetireCommunityNeural()
{
    if (!communityNeural_) return true;
    const auto retired=communityNeural_->Retire();
    if (!retired) {status_=retired.error().message;FailLifecycle(E_FAIL,"community NR retirement");return false;}
    communityNeural_.reset();communitySnapshotValid_=false;communityCameraHistory_.Invalidate();nrMotionDiagnostic.Reset();
    ++communityEpoch_;communityLastStatus_.clear();communityFsrRouteDiagnostics_.Reset();return true;
}
