#include <PCH.h>
#include "NvidiaHost.h"
#include "RenderPipeline.h"
#include "SourceFrameEvaluator.h"
#include "GameCameraMeasurements.h"
#include "CommunityShaderIntegration.h"
#include "PerformanceTuning.h"
#include "SourceInternalScope.h"

#if defined(TRP_ENABLE_XESS)
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
struct NvidiaHost::SourceXessEvaluationOperations
{
    NvidiaHost& host;
    RenderPipeline& pipeline;
    bool menu{};
    std::optional<RuntimeError> error;

    void CopyInput(ID3D11DeviceContext* context,const UpscaleFrame& frame){context->CopyResource(frame.input,frame.color);}
    bool EvaluateOptionalPreUpscale(UpscaleFrame&){return true;} // NR qualification follows separately.
    bool EvaluateOptionalPostUpscale(UpscaleFrame&,UpscaleOutcome){return true;}
    void UpscaleSucceeded(){++host.upscaleEvaluationCount_;}
    GenerationPreparationStatus PrepareGeneration(const UpscaleFrame&){return GenerationPreparationStatus::NotRequested;}
    void RenderReShade(const UpscaleFrame& frame,bool before)
    {
        auto& effects=ReShadeIntegration::Get();effects.SetBeforeUpscaling(pipeline.mReShadeBeforeUpscaling);
        const auto result=effects.Render(before?frame.input:frame.output,menu?nullptr:frame.depth,
            before?FrameExtent{frame.render.width,frame.render.height}:FrameExtent{frame.display.width,frame.display.height},
            {frame.render.width,frame.render.height},before);
        if(FAILED(result) && effects.Snapshot().failures<=3)logger::warn("[XeSS/ReShade] {}",effects.Status());
    }
    Result<UpscaleOutcome> Spatial(const UpscaleFrame& frame,const std::string& reason)
    {
        host.xessRecoveryReason_=reason;
        const auto result=host.loadingScreenUpscaler_.Evaluate(host.context_.Get(),frame.input,frame.output);
        if(FAILED(result)){error=RuntimeError{ErrorKind::DispatchFailure,result,"XeSS spatial recovery copy failed"};return std::unexpected(*error);}
        return UpscaleOutcome::SpatialRecovery;
    }
    Result<UpscaleOutcome> EvaluateUpscaler(UpscaleFrame& frame)
    {
        if(host.xessRecovery_)return Spatial(frame,host.xessRecoveryReason_);
        if(menu || !frame.depth || !frame.motion)return Spatial(frame,menu?"main/loading menu":"waiting for original game depth/motion guides");
        const auto camera=CaptureGameCameraMeasurements(pipeline.mGraphicsState,frame.render,pipeline.mEnableJitter,frame.reset);
        if(!camera)return Spatial(frame,camera.error().message);
        frame.camera=*camera;
        const auto prepared=host.xessResources_->PrepareInput(frame.input,frame.depth,frame.motion);
        if(!prepared)return Spatial(frame,prepared.error().message);
        const auto bridge=host.xessResources_->Bridge();ID3D12GraphicsCommandList* list{};
        auto hr=bridge->SignalProducer();
        if(SUCCEEDED(hr))hr=bridge->Begin(&list);
        if(FAILED(hr)){error=RuntimeError{ErrorKind::DispatchFailure,hr,"XeSS producer/command-list admission failed"};return std::unexpected(*error);}
        const auto dispatched=host.xessResources_->Upscaler()->Dispatch(list,host.xessResources_->Resources(),frame);
        if(!dispatched) {
            // Failed vendor recording is never submitted. Retirement may refuse;
            // in that case the host retains everything and reports fatal recovery.
            hr=bridge->DiscardRecording();
            const auto retired=SUCCEEDED(hr)?host.xessResources_->Retire():Result<void>{std::unexpected(RuntimeError{ErrorKind::RetirementFailure,hr,"XeSS failed recording could not be discarded"})};
            if(!retired){error=retired.error();return std::unexpected(*error);}
            host.xessEncode_={};host.xessRecovery_=true;
            logger::error("[XeSS recovery] requested=XeSS effective=spatial; context retired; NR/FG off; restart required: {}",dispatched.error().message);
            return Spatial(frame,dispatched.error().message+"; restart required");
        }
        hr=bridge->Submit();
        if(SUCCEEDED(hr))hr=bridge->WaitConsumer();
        if(FAILED(hr)){error=RuntimeError{ErrorKind::RetirementFailure,hr,"XeSS accepted work could not establish consumer ownership; retain resources"};return std::unexpected(*error);}
        hr=host.xessEncode_.Convert(host.context_.Get(),host.xessResources_->Output11(),frame.output,ColorEncoding::Linear,
            host.sourceUpscalerSettings_.Effective().xess.sourceEncoding);
        if(FAILED(hr)){error=RuntimeError{ErrorKind::DispatchFailure,hr,"XeSS reconstructed output encoding failed"};return std::unexpected(*error);}
        return UpscaleOutcome::Temporal;
    }
};
#endif

bool NvidiaHost::QueryXessJitter(std::uint64_t sourceId,float& x,float& y)
{
    x=y=0;
#if defined(TRP_ENABLE_XESS)
    if(XessActive() && !xessRecovery_ && xessResources_ && xessResources_->Upscaler()) {
        const auto jitter=xessResources_->Upscaler()->QueryJitter(sourceId);
        if(jitter){x=(*jitter)[0];y=(*jitter)[1];return true;}
    }
#else
    (void)sourceId;
#endif
    return false;
}

bool NvidiaHost::EvaluateXessFrame(IDXGISwapChain* swapChain,bool nativeUIHandoff)
{
    (void)nativeUIHandoff;
#if defined(TRP_ENABLE_XESS)
    if(FAILED(FailureResult()) || !proxyActive_ || swapChain!=outerSwapChain_ || !upscalerReady_ ||
        !xessResources_ || !context_ || !gameTargets_.GameFacing() || !gameTargets_.UpscaleInput() ||
        !gameTargets_.UpscaleOutput() || !PresentationBackendReadyForEvaluation())return false;
    SourceInternalScope internal(sourceUIInternal_);
    Microsoft::WRL::ComPtr<IDXGISwapChain3> chain;
    if(FAILED(innerSwapChain_->QueryInterface(IID_PPV_ARGS(&chain))))return false;
    const auto index=presentation_.BufferIndex(chain->GetCurrentBackBufferIndex());if(index>=presentation_.Buffers().size())return false;
    auto& pipeline=*RenderPipeline::GetSingleton();UpscaleFrame frame{};
    frame.backend=BackendKind::Xess;frame.color=gameTargets_.GameFacing();frame.input=gameTargets_.UpscaleInput();frame.output=gameTargets_.UpscaleOutput();
    frame.depth=pipeline.mDepthBuffer.mImage;frame.motion=pipeline.mMotionVectors.mImage;
    frame.render=frame.subrect={renderWidth_,renderHeight_};frame.display={outputWidth_,outputHeight_};
    frame.sourceId=pipeline.mRenderedFrameCount+1;frame.sourceEpoch=xessEpoch_;
    frame.deltaMilliseconds=pipeline.mSourceDeltaMilliseconds;frame.jitterX=pipeline.mJitterOffsets[0];frame.jitterY=pipeline.mJitterOffsets[1];
    frame.motionConvention={float(renderWidth_),float(renderHeight_),true,false};
    frame.colorIsLinear=true;frame.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
    frame.reset=resetNextEvaluation_ || pipeline.mPendingHistoryResets>0 || loadingScreenRoute_.NeedsTemporalReset();
    auto* ui=RE::UI::GetSingleton();const bool menu=ui && (ui->IsMenuOpen(RE::MainMenu::MENU_NAME) || ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME));
    SourceXessEvaluationOperations operations{*this,pipeline,menu || loadingScreenRoute_.Active(presentCount_)};
    const auto result=SourceFrameEvaluator::Evaluate(context_.Get(),frame,operations);
    if(result.outcome!=UpscaleOutcome::Temporal && result.outcome!=UpscaleOutcome::SpatialRecovery) {
        if(operations.error){status_=operations.error->message;logger::error("[XeSS frame] native=0x{:08X} {}",std::uint32_t(operations.error->nativeResult),status_);}
        FailLifecycle(E_FAIL,"XeSS frame delivery");return false;
    }
    context_->CopyResource(presentation_.Buffers()[index].Get(),frame.output);
    const bool temporal=result.outcome==UpscaleOutcome::Temporal;
    const bool changed=evaluationCount_==0 || temporal!=lastXessTemporal_;
    lastXessTemporal_=temporal;resetNextEvaluation_=!temporal;
    if(temporal){pipeline.mPendingHistoryResets=0;loadingScreenRoute_.TemporalSucceeded();status_="XeSS active | ordinary presentation | NR/FG off";}
    else {loadingScreenRoute_.SpatialSucceeded();status_="XeSS requested; spatial recovery: "+xessRecoveryReason_;}
    if(changed || (PerformanceTuning::GetSingleton()->settings.diagnostics.frameDetails && evaluationCount_%600==0))
        logger::info("[XeSS state] source={} epoch={} route={} render={}x{} display={}x{} jitter=({},{}) motionScale=({},{}) guide=original-engine-output reset={} reason={}",
            frame.sourceId,frame.sourceEpoch,temporal?"temporal":"spatial",renderWidth_,renderHeight_,outputWidth_,outputHeight_,
            frame.jitterX,frame.jitterY,frame.motionConvention.scaleX,frame.motionConvention.scaleY,frame.reset,status_);
    ++evaluationCount_;return true;
#else
    (void)swapChain;return false;
#endif
}

bool NvidiaHost::XessTemporalActive()const
{
#if defined(TRP_ENABLE_XESS)
    return XessActive() && lastXessTemporal_ && !xessRecovery_ && !FAILED(FailureResult());
#else
    return false;
#endif
}

TheosRenderPipeline::SettingsActionStatus NvidiaHost::XessStatus()const
{
    using namespace TheosRenderPipeline;
#if defined(TRP_ENABLE_XESS)
    if(!xessStartupFallbackReason_.empty() && FsrActive())return {"XeSS unavailable; active FSR startup fallback: "+xessStartupFallbackReason_,SettingsStatusKind::Pending};
    if(FAILED(FailureResult()))return {status_,SettingsStatusKind::Error};
    if(sourceUpscalerSettings_.NeedsRestart())return {status_+"; requested startup settings awaiting Save and restart.",SettingsStatusKind::Pending};
    if(XessActive())return {status_,lastXessTemporal_?SettingsStatusKind::Success:SettingsStatusKind::Pending};
    return {"XeSS awaits Save and restart.",SettingsStatusKind::Neutral};
#else
    return {"XeSS is unavailable in this build.",SettingsStatusKind::Error};
#endif
}
