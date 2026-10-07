#include <PCH.h>
#include "NvidiaHost.h"
#include "../RenderPipeline.h"
#include "GameCameraMeasurements.h"
#include "SourceFrameEvaluator.h"
#include "PerformanceTuning.h"
#include "SourceFrameGeneration.h"
#include "CommunityShaderIntegration.h"
#if defined(TRP_ENABLE_FSR_FG)
#include "Upscaling/FSRGenerationStatus.h"
#endif
#include <optional>
#include <utility>
TheosRenderPipeline::SettingsActionStatus NvidiaHost::FsrStatus() const
{
    using namespace TheosRenderPipeline;using namespace Upscaling;
    BackendConfiguration requested;
    const auto& creation=sourceUpscalerSettings_.Requested();
    requested.backend=creation.mode==FSR?BackendKind::Fsr:creation.mode==DLAA?BackendKind::Dlaa:BackendKind::Dlss;
    auto active=backendDecision_;active.valid=false;
    const ProviderInfo* provider=nullptr;const RuntimeError* error=nullptr;
#if defined(TRP_ENABLE_FSR)
    if(FsrActive()) {
        if(CommunityShaders::Active())return {"Community Shaders owns upscaling; TRP FSR and frame generation are inactive.",SettingsStatusKind::Neutral};
        active.valid=UpscalerReady() && lastFsrTemporal_;
        active.diagnostic=evaluationCount_ ? "FSR spatial recovery; temporal history remains pending." : "FSR requested; waiting for first temporal frame.";
        if(fsrResources_ && fsrResources_->FeatureReady())provider=&fsrResources_->Provider();
        if(fsrFrame_)error=fsrFrame_->LastError();
        if(FAILED(FailureResult()))return {status_,SettingsStatusKind::Error};
    }
#endif
    return DescribeFsrStatus(requested,active,provider,sourceUpscalerSettings_.NeedsRestart(),error);
}
#if defined(TRP_ENABLE_FSR)
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
struct NvidiaHost::SourceFsrEvaluationOperations
{
    NvidiaHost& host;RenderPipeline& pipeline;bool spatial{},nativeUIHandoff{};
    std::optional<RuntimeError> error;
    std::string recoveryReason;
#if !defined(TRP_NO_NEURAL_RENDERING)
    NeuralRendering::PreparedFsrInput preparedNr;
#endif
    void CopyInput(ID3D11DeviceContext* context,const UpscaleFrame& frame){context->CopyResource(frame.input,frame.color);}
    bool EvaluateOptionalPreUpscale(UpscaleFrame& frame){
#if !defined(TRP_NO_NEURAL_RENDERING)
        frame.sourceEpoch=host.communityEpoch_;
        return host.EvaluateCommunityNeuralBefore(frame.input,frame.depth,frame.motion,frame.render.width,frame.render.height,
            frame.sourceId,frame.reset,NeuralRendering::SourceWorldEligible(!spatial,nativeUIHandoff,host.nativeUI_.Dedicated(),CommunityShaders::Active()),
            pipeline.mReShadeBeforeUpscaling?nullptr:&preparedNr);
#else
        (void)frame;return true;
#endif
    }
    void RenderReShade(const UpscaleFrame& frame,bool before)
    {
        auto& effects=ReShadeIntegration::Get();effects.SetBeforeUpscaling(pipeline.mReShadeBeforeUpscaling);
        auto* ui=RE::UI::GetSingleton();bool world=ui && !ui->IsMenuOpen(RE::MainMenu::MENU_NAME) && !ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME);
        auto hr=effects.Render(before?frame.input:frame.output,world?frame.depth:nullptr,
            before?FrameExtent{frame.render.width,frame.render.height}:FrameExtent{frame.display.width,frame.display.height},
            {frame.render.width,frame.render.height},before);
        if(FAILED(hr) && effects.Snapshot().failures<=3)logger::warn("[FSR/ReShade] {}",effects.Status());
    }
    Result<UpscaleOutcome> EvaluateUpscaler(UpscaleFrame frame)
    {
        frame.sharpness=host.sourceUpscalerSettings_.Effective().fsr.sharpness;
        auto makeAdapter=[&]{
            if(!host.fsrFrame_)host.fsrFrame_=std::make_unique<FsrFrameAdapter>(*host.fsrResources_->Upscaler(),host.fsrResources_->Bridge(),host.fsrResources_->Resources(),
                host.fsrResources_->Color11(),host.fsrResources_->Depth11(),host.fsrResources_->Motion11(),host.fsrResources_->Output11(),host.fsrResources_->HandoffEncoding());
        };
        makeAdapter();
        if(spatial) { recoveryReason="main/loading menu or loading-screen presentation";host.loadingScreenRoute_.SpatialSucceeded();return host.fsrFrame_->Spatial(frame); }
        auto camera=CaptureGameCameraMeasurements(pipeline.mGraphicsState,frame.render,pipeline.mEnableJitter,frame.reset);
        if(!camera || !frame.depth || !frame.motion) {
#if !defined(TRP_NO_NEURAL_RENDERING)
            if(preparedNr.Valid()){error=RuntimeError{ErrorKind::InvalidInput,0,"FSR prepared NR input lost its temporal camera/guides"};return std::unexpected(*error);}
#endif
            host.status_=camera?"FSR requested; spatial recovery while guides are unavailable":camera.error().message;
            recoveryReason=host.status_;
            return host.fsrFrame_->Spatial(frame);
        }
        const FsrInputPolicy policy{camera->depthInverted,camera->depthInfinite,false,true};
        auto configured=host.fsrResources_->EnsureInputPolicy(policy);
        if(!configured){error=configured.error();return std::unexpected(configured.error());}
        frame.camera=*camera;
#if defined(TRP_ENABLE_FSR_FG)
        if(host.FsrFgActive())host.fsrGenerationFrame_=frame;
#endif
        if(PerformanceTuning::GetSingleton()->settings.diagnostics.frameDetails &&
            (host.evaluationCount_<3 || host.evaluationCount_%600==0)) {
            D3D11_TEXTURE2D_DESC motion{},depth{};frame.motion->GetDesc(&motion);frame.depth->GetDesc(&depth);
            logger::info("[FSR frame] source={} deltaMs={} extent={}x{} jitter=({},{}) camera={} near={} far={} fov={} depthInverted={} unitsToMeters={} motionFormat={} depthFormat={} motionScale=({},{}) sharpness={}",
                frame.sourceId,frame.deltaMilliseconds,frame.render.width,frame.render.height,frame.jitterX,frame.jitterY,frame.camera.identity,
                frame.camera.nearDistance,frame.camera.farDistance,frame.camera.verticalFovRadians,frame.camera.depthInverted,frame.camera.worldUnitsToMeters,
                static_cast<unsigned>(motion.Format),static_cast<unsigned>(depth.Format),frame.motionConvention.scaleX,frame.motionConvention.scaleY,frame.sharpness);
        }
        // Before-upscale ReShade edits the game-format image, so it retains the
        // encoded P3 route. Direct linear handoff preserves after-upscale effects.
#if !defined(TRP_NO_NEURAL_RENDERING)
        const bool direct=preparedNr.Valid();
        auto result=direct?host.fsrFrame_->EvaluatePrepared(frame,preparedNr):host.fsrFrame_->Evaluate(frame);
#else
        auto result=host.fsrFrame_->Evaluate(frame);
#endif
        if(!result)error=result.error();
        if(result && *result==UpscaleOutcome::SkippedInvalidInput) {
#if !defined(TRP_NO_NEURAL_RENDERING)
            if(direct){error=RuntimeError{ErrorKind::InvalidInput,0,"FSR rejected an owned prepared NR input"};return std::unexpected(*error);}
#endif
            const auto* reason=host.fsrFrame_->LastError();
            host.status_=reason?reason->message:"FSR requested; source parameters rejected, spatial recovery";
            recoveryReason=host.status_;
            return host.fsrFrame_->Spatial(frame);
        }
        return result;
    }
    bool EvaluateOptionalPostUpscale(UpscaleFrame& frame,UpscaleOutcome outcome){
#if !defined(TRP_NO_NEURAL_RENDERING)
        const bool ok=host.EvaluateCommunityNeuralAfter(frame,outcome,NeuralRendering::SourceWorldEligible(!spatial,
            nativeUIHandoff,host.nativeUI_.Dedicated(),CommunityShaders::Active()));
#if defined(TRP_ENABLE_FSR_FG)
        if(host.FsrFgActive())host.fsrGenerationFrame_.reset|=frame.reset;
#endif
        return ok;
#else
        (void)frame;(void)outcome;return true;
#endif
    }
    void UpscaleSucceeded(){++host.upscaleEvaluationCount_;}
    GenerationPreparationStatus PrepareGeneration(const UpscaleFrame&){return GenerationPreparationStatus::NotRequested;}
};
#endif

bool NvidiaHost::QueryFsrJitter(std::uint64_t sourceId,float& x,float& y)
{
#if defined(TRP_ENABLE_FSR)
    if(FsrActive() && fsrResources_ && fsrResources_->FeatureReady()) {
        auto jitter=fsrResources_->Upscaler()->QueryJitter(FsrFgActive()?presentCount_+1:sourceId);
        if(jitter){x=(*jitter)[0];y=(*jitter)[1];return true;}
        if(fsrFrame_)fsrFrame_->InvalidateHistory();
    }
#else
    (void)sourceId;
#endif
    x=y=0;return false;
}

bool NvidiaHost::EvaluateFsrFrame(IDXGISwapChain* swapChain,bool nativeUIHandoff)
{
    (void)nativeUIHandoff;
#if defined(TRP_ENABLE_FSR)
    if(FsrPresentSuspended() && UpdateFsrSuspension()!=S_OK)return false;
    if(FAILED(FailureResult()) || FsrPresentSuspended() || !proxyActive_ || swapChain!=outerSwapChain_ || !upscalerReady_ || !fsrResources_ || !context_ ||
        !gameTargets_.GameFacing() || !gameTargets_.UpscaleInput() || !gameTargets_.UpscaleOutput() || !PresentationBackendReadyForEvaluation())return false;
    struct InternalScope {
        bool& flag;bool previous;
        explicit InternalScope(bool& value):flag(value),previous(std::exchange(value,true)){}
        ~InternalScope(){flag=previous;}
    } internal(sourceUIInternal_);
    Microsoft::WRL::ComPtr<IDXGISwapChain3> indexed;
    if(FAILED(innerSwapChain_->QueryInterface(IID_PPV_ARGS(&indexed))))return false;
    const auto index=presentation_.BufferIndex(indexed->GetCurrentBackBufferIndex());if(index>=presentation_.Buffers().size())return false;
    auto& pipeline=*RenderPipeline::GetSingleton();UpscaleFrame frame;
    frame.backend=BackendKind::Fsr;frame.color=gameTargets_.GameFacing();frame.input=gameTargets_.UpscaleInput();frame.output=gameTargets_.UpscaleOutput();
    frame.depth=pipeline.mDepthBuffer.mImage;frame.motion=pipeline.mMotionVectors.mImage;
    frame.render=frame.subrect={renderWidth_,renderHeight_};frame.display={outputWidth_,outputHeight_};
    frame.sourceId=FsrFgActive()?presentCount_+1:pipeline.mRenderedFrameCount;frame.deltaMilliseconds=pipeline.mSourceDeltaMilliseconds;
    frame.jitterX=pipeline.mJitterOffsets[0];frame.jitterY=pipeline.mJitterOffsets[1];
    // Skyrim's unjittered previous/current projection motion is current-to-
    // previous UV displacement. The signed RG16_FLOAT producer is copied raw;
    // no UNORM decode or vendor constant conversion is introduced here.
    frame.motionConvention={float(renderWidth_),float(renderHeight_),true,false};
    frame.reset=resetNextEvaluation_ || pipeline.mPendingHistoryResets>0 || loadingScreenRoute_.NeedsTemporalReset();
    const bool frameDetails=PerformanceTuning::GetSingleton()->settings.diagnostics.frameDetails;
    const auto logHandoff=[&] {
        auto* bridgeContext=fsrResources_->Bridge()->Context11();
        Microsoft::WRL::ComPtr<ID3D11Device> bridgeDevice;bridgeContext->GetDevice(&bridgeDevice);
        for(auto target:{frame.input,frame.output}) {
            D3D11_TEXTURE2D_DESC desc{};target->GetDesc(&desc);
            Microsoft::WRL::ComPtr<ID3D11Device> targetDevice;target->GetDevice(&targetDevice);
            logger::info("[FSR handoff] source={} deltaMs={} role={} extent={}x{} expected={}x{} format={} bind=0x{:X} mips={} array={} samples={} context={} contextType={} contextDevice={} targetDevice={} sameDevice={}",
                frame.sourceId,frame.deltaMilliseconds,target==frame.input?"input":"output",desc.Width,desc.Height,
                target==frame.input?frame.render.width:frame.display.width,target==frame.input?frame.render.height:frame.display.height,
                static_cast<unsigned>(desc.Format),desc.BindFlags,desc.MipLevels,desc.ArraySize,desc.SampleDesc.Count,
                static_cast<void*>(bridgeContext),static_cast<unsigned>(bridgeContext->GetType()),static_cast<void*>(bridgeDevice.Get()),
                static_cast<void*>(targetDevice.Get()),D3D11FrameCopy::SameObject(bridgeDevice.Get(),targetDevice.Get()));
        }
    };
    const bool handoffLogged=evaluationCount_==0 || (frameDetails && (evaluationCount_<3 || evaluationCount_%600==0));
    if(handoffLogged)logHandoff();
    auto* ui=RE::UI::GetSingleton();const bool menu=ui && (ui->IsMenuOpen(RE::MainMenu::MENU_NAME) || ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME));
    SourceFsrEvaluationOperations operations{*this,pipeline,menu || loadingScreenRoute_.Active(presentCount_),nativeUIHandoff};
#if defined(TRP_ENABLE_FSR_FG)
    if(FsrFgActive()) {
        auto waited=fsrPresentation_->WaitBeforeProducer();
        if(FAILED(waited)){FailLifecycle(waited,"AMD guide producer ownership");return false;}
        fsrGenerationFrame_=frame;fsrSourceRenderedCount_=pipeline.mRenderedFrameCount;
        fsrSourcePending_=false;fsrUiComplete_=false;fsrForeground_.Reset();
        fsrMenu_=operations.spatial || pipeline.FrameGenerationTransitionBlocked();
    }
#endif
    auto result=SourceFrameEvaluator::Evaluate(context_.Get(),frame,operations);
    if(result.outcome!=UpscaleOutcome::Temporal && result.outcome!=UpscaleOutcome::SpatialRecovery) {
        if(!handoffLogged)logHandoff();
        const auto* error=operations.error?&*operations.error:fsrFrame_?fsrFrame_->LastError():nullptr;
        if(error) {
            status_=error->message;
            logger::error("[FSR frame delivery] outcome={} error=0x{:08X} {}",static_cast<unsigned>(result.outcome),
                static_cast<std::uint32_t>(error->nativeResult),status_);
        }
        FailLifecycle(E_FAIL,"FSR frame delivery");return false;
    }
#if defined(TRP_ENABLE_FSR_FG)
    if(FsrFgActive()) {
        fsrGenerationOutcome_=result.outcome;
        if(result.outcome==UpscaleOutcome::Temporal)++fsrGuideCaptureCount_;
        fsrGenerationFrame_.depthFormat=DXGI_FORMAT_R32_FLOAT;fsrGenerationFrame_.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
        fsrGenerationFrame_.colorIsLinear=true;
        fsrGenerationFrame_.reset |= fsrFrame_->LastTemporalReset();fsrSourcePending_=true;
    }
#endif
    context_->CopyResource(presentation_.Buffers()[index].Get(),gameTargets_.UpscaleOutput());
    const bool stateChanged=evaluationCount_==0 || lastFsrTemporal_!=(result.outcome==UpscaleOutcome::Temporal);
    lastFsrTemporal_=result.outcome==UpscaleOutcome::Temporal;
    if(lastFsrTemporal_) {
        resetNextEvaluation_=false;pipeline.mPendingHistoryResets=0;loadingScreenRoute_.TemporalSucceeded();
        status_=std::format("FSR active: {} on {} presentation",fsrResources_->Provider().name,FsrFgActive()?"AMD":"ordinary");
    } else {
        resetNextEvaluation_=true;
        status_="FSR requested; spatial recovery active, frame generation off";
        if(fsrFrame_ && fsrFrame_->LastError())status_+=std::format(" ({})",fsrFrame_->LastError()->message);
    }
    // Menu changes can alternate spatial/temporal routes frequently. Retain a
    // bounded normal-mode history; opt-in frame diagnostics keep all changes.
    if(stateChanged && (fsrTransitionLogs_<16 || frameDetails)) {
        logger::info("[FSR state] source={} route={} provider={} menu={} reset={} deltaMs={} reason={}",
            frame.sourceId,lastFsrTemporal_?"temporal":"spatial recovery",fsrResources_->Provider().name,
            menu,frame.reset,frame.deltaMilliseconds,operations.recoveryReason.empty()?status_:operations.recoveryReason);
        if(++fsrTransitionLogs_==16 && !frameDetails)logger::info("[FSR state] transition log budget reached; enable Debug/LogFrameDiagnostics for further transitions; failures remain logged");
    }
    ++evaluationCount_;return true;
#else
    (void)swapChain;return false;
#endif
}

HRESULT NvidiaHost::PresentFsrSource(UINT interval,UINT flags)
{
#if defined(TRP_ENABLE_FSR_FG)
    if(FAILED(FailureResult()))return FailureResult();
    if(!FsrFgActive() || !fsrPresentation_)return E_UNEXPECTED;
    if((flags&DXGI_PRESENT_TEST) || !UpscalerReady())return fsrPresentation_->StartupPresent(interval,flags);
    if(!fsrSourcePending_ || !fsrUiComplete_ || !nativeUI_.Dedicated())
        return FailLifecycle(DXGI_ERROR_INVALID_CALL,"AMD completed source/UI boundary");
    auto before=fsrPresentation_->Status();
    auto result=fsrPresentation_->Present(fsrGenerationFrame_,fsrGenerationOutcome_,nativeUI_.TaggedTexture(),
        fsrForeground_.Get(),fsrUiComplete_,fsrMenu_,SourceFrameGeneration::GetSingleton()->RuntimeInterpolationRequested(),interval,flags);
    fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();
    auto status=fsrPresentation_->Status();frameGenerationEnabled_=SUCCEEDED(result) && status.decision.generate;
    const bool changed=before.decision.reason!=status.decision.reason || before.decision.generate!=status.decision.generate;
    if(FAILED(result) || presentCount_<3 || (changed && fsrGenerationTransitionLogs_++<24) ||
        (PerformanceTuning::GetSingleton()->settings.diagnostics.frameDetails && presentCount_%600==0)) {
        logger::info("[FSR FG] source={} presentCount={} renderedAtCapture={} renderedNow={} camera={} guideCapture={} temporalGuides={} configuredId={} preparedId={} requested={} prepare={} generate={} reason={} callbackCount={} callbackResult={} submitted={} apiResult=0x{:08X}",
            status.sourceId,presentCount_,fsrSourceRenderedCount_,RenderPipeline::GetSingleton()->mRenderedFrameCount,
            fsrGenerationFrame_.camera.identity,fsrGuideCaptureCount_,fsrGenerationOutcome_==UpscaleOutcome::Temporal,
            status.callback.configuredId,status.callback.preparedId,SourceFrameGeneration::GetSingleton()->RuntimeInterpolationRequested(),status.decision.prepare,
            status.decision.generate,status.decision.reason,status.callback.invocations,status.callback.result,
            status.submitted,static_cast<std::uint32_t>(result));
    }
    if(FAILED(result))return FailLifecycle(result,"AMD source Present");
    return result;
#else
    (void)interval;(void)flags;return E_NOTIMPL;
#endif
}

TheosRenderPipeline::SettingsActionStatus NvidiaHost::FsrFgStatus() const
{
#if defined(TRP_ENABLE_FSR_FG)
    auto state=fsrPresentation_?fsrPresentation_->Status():TheosRenderPipeline::FsrPresentationStatus{};
    return TheosRenderPipeline::Upscaling::DescribeFsrGenerationStatus(FsrFgActive(),
        SourceFrameGeneration::GetSingleton()->RuntimeInterpolationRequested(),state.submitted,
        FAILED(FailureResult()) || FAILED(state.result),state.decision,state.callback.invocations);
#else
    return {"FSR frame generation is unavailable in this build.",TheosRenderPipeline::SettingsStatusKind::Neutral};
#endif
}
