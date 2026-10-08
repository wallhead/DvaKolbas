#include "NvidiaHost.h"
#include <PCH.h>
#include "../DLSSBackend.h"
#include "../RenderPipeline.h"
#include "SourceFrameGeneration.h"
#include "SourceDLSSGBackend.h"
#include "SourceDLSSGCamera.h"
#include "GameCameraMeasurements.h"
#include "SourceNvidiaFrameEvaluator.h"
#include "SourceGenerationPolicy.h"
#include "PerformanceTuning.h"
#include "NeuralCombatMode.h"
#include "CommunityShaderIntegration.h"
#include "WeatherAppearanceRuntime.h"

struct NvidiaHost::SourceNvidiaEvaluationOperations
{
    NvidiaHost& host;
    RenderPipeline& upscaler;
    TheosRenderPipeline::SourceDLSSG::NeuralOptions neuralOptions;
    sl::Constants constants{};

    bool NeuralEligible(const TheosRenderPipeline::SourceNvidiaFrameGuides& frame) const
    {
        auto* ui = RE::UI::GetSingleton();
        return host.nativeUI_.Dedicated() && frame.uiColorAndAlpha && frame.hudLessColor && ui &&
            !ui->IsMenuOpen(RE::MainMenu::MENU_NAME) && !ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME);
    }

    bool EvaluateNeuralBeforeDLSS(TheosRenderPipeline::SourceNvidiaFrameInputs& frame)
    {
#if !defined(TRP_NO_NEURAL_RENDERING)
        if (SourceFrameGeneration::GetSingleton()->settings.neuralStartup.community) {
            auto* ui=RE::UI::GetSingleton();
            const bool world=ui && !ui->IsMenuOpen(RE::MainMenu::MENU_NAME) && !ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME) &&
                !host.loadingScreenRoute_.Active(host.presentCount_);
            return host.EvaluateCommunityNeuralBefore(frame.input,frame.depth,frame.motion,frame.renderWidth,frame.renderHeight,
                frame.sourceSnapshot.sourceId,frame.reset,TheosRenderPipeline::NeuralRendering::SourceWorldEligible(world,
                    frame.uiColorAndAlpha && frame.hudLessColor,host.nativeUI_.Dedicated(),TheosRenderPipeline::CommunityShaders::Active()),nullptr,nullptr,TheosRenderPipeline::Upscaling::UpscaleOutcome::Temporal,host.FsrFgActive()?&frame.sourceSnapshot.camera:nullptr);
        }
        if(host.FsrFgActive())return true; // Legacy NR belongs to the NVIDIA presenter.
        auto& backend = TheosRenderPipeline::SourceDLSSG::Backend::Get();
        auto& options = neuralOptions;
        sl::Constants preview{};
        const bool eligible = NeuralEligible(frame);
        TheosRenderPipeline::NeuralRendering::ApplyCombatMode(options, eligible);
        const bool cameraValid = options.enabled && options.beforeUpscaling && eligible &&
            TheosRenderPipeline::SourceDLSSG::CaptureCameraConstants(upscaler.mGraphicsState,
                frame.renderWidth, frame.renderHeight, frame.jitterX, frame.jitterY,
                frame.reset, frame.jitterEnabled, preview, false);
        return backend.EvaluateNeuralBeforeUpscaling(options, cameraValid ? &preview : nullptr,
            eligible, frame.input, frame.motion, frame.depth, {frame.renderWidth, frame.renderHeight}, frame.reset);
#else
        (void)frame;
        return true;
#endif
    }

    void CopyInput(ID3D11DeviceContext* context, const TheosRenderPipeline::SourceNvidiaFrameInputs& frame)
    {
        ScopedD3D11PerformanceStage timer{context, PerformanceTuning::D3D11Stage::kInputColorCopy};
        context->CopyResource(frame.input, frame.color);
    }

    bool EvaluateDLSS(const TheosRenderPipeline::SourceNvidiaFrameInputs& frame)
    {
        const bool spatial = host.loadingScreenRoute_.Active(host.presentCount_);
        bool evaluated{};
        if (spatial) {
            host.loadingScreenResult_ = host.loadingScreenUpscaler_.Evaluate(host.context_.Get(), frame.input, frame.output);
            evaluated = SUCCEEDED(host.loadingScreenResult_);
            if (evaluated) {
                host.loadingScreenRoute_.SpatialSucceeded();
                if (!host.loadingScreenLogged_) {
                    logger::info("[LoadingScreen] spatial scaling active; native UI composition unchanged");
                    host.loadingScreenLogged_ = true;
                }
            }
        } else {
            evaluated = DLSSBackend::GetSingleton()->Evaluate(frame.input, frame.motion, frame.depth,
                frame.output, static_cast<int>(frame.renderWidth), static_cast<int>(frame.renderHeight),
                frame.sharpness, frame.jitterX, frame.jitterY, frame.motionScaleX, frame.motionScaleY,
                frame.reset);
            if (evaluated) {
                if (host.loadingScreenLogged_) {
                    logger::info("[LoadingScreen] resumed DLSS with temporal history reset");
                    host.loadingScreenLogged_ = false;
                }
                host.loadingScreenRoute_.TemporalSucceeded();
            }
        }
        return evaluated;
    }
    bool EvaluateNeuralAfterDLSS(TheosRenderPipeline::SourceNvidiaFrameInputs& frame,TheosRenderPipeline::Upscaling::UpscaleOutcome outcome)
    {
#if !defined(TRP_NO_NEURAL_RENDERING)
        using namespace TheosRenderPipeline;
        if(SourceFrameGeneration::GetSingleton()->settings.neuralStartup.community){
            Upscaling::UpscaleFrame completed=frame.sourceSnapshot;
            completed.backend=upscaler.mUpscaleType==DLAA?Upscaling::BackendKind::Dlaa:Upscaling::BackendKind::Dlss;
            completed.output=frame.output;completed.depth=frame.depth;completed.motion=frame.motion;
            completed.render={frame.renderWidth,frame.renderHeight};completed.display={frame.outputWidth,frame.outputHeight};
            completed.sourceId=frame.sourceSnapshot.sourceId;completed.sourceEpoch=host.communityEpoch_;
            completed.jitterX=frame.jitterX;completed.jitterY=frame.jitterY;
            completed.motionConvention={frame.motionScaleX,frame.motionScaleY,true,false};completed.reset=frame.reset;
            const bool ok=host.EvaluateCommunityNeuralAfter(completed,outcome,NeuralEligible(frame) && !host.loadingScreenRoute_.Active(host.presentCount_));
            frame.reset|=completed.reset;return ok;
        }
#else
        (void)frame;(void)outcome;
#endif
        return true;
    }
    void UpscaleSucceeded() { ++host.upscaleEvaluationCount_; }
    void RenderReShade(const TheosRenderPipeline::SourceNvidiaFrameInputs& frame, bool before)
    {
        auto& effects = TheosRenderPipeline::ReShadeIntegration::Get();
        effects.SetBeforeUpscaling(upscaler.mReShadeBeforeUpscaling);
        auto* ui = RE::UI::GetSingleton();
        const bool world = ui && !ui->IsMenuOpen(RE::MainMenu::MENU_NAME) && !ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME);
        const auto hr = effects.Render(before ? frame.input : frame.output, world ? frame.depth : nullptr,
            before ? TheosRenderPipeline::FrameExtent{frame.renderWidth, frame.renderHeight} :
                     TheosRenderPipeline::FrameExtent{frame.outputWidth, frame.outputHeight},
            {frame.renderWidth, frame.renderHeight}, before);
        if (FAILED(hr) && (effects.Snapshot().failures <= 3 || host.presentCount_ % 600 == 0)) {
            logger::warn("[ReShade] {} (0x{:08X})", effects.Status(), static_cast<unsigned>(hr));
        }
    }
    TheosRenderPipeline::Upscaling::GenerationPreparationStatus PrepareGeneration(
        const TheosRenderPipeline::SourceNvidiaFrameInputs& frame,const TheosRenderPipeline::Upscaling::UpscaleFrame& completed)
    {
        using namespace TheosRenderPipeline;
        if(host.FsrFgActive()) {
            auto prepared=host.PrepareExternalGeneration(completed,host.loadingScreenRoute_.Active(host.presentCount_)?
                Upscaling::UpscaleOutcome::SpatialRecovery:Upscaling::UpscaleOutcome::Temporal);
            return prepared?Upscaling::GenerationPreparationStatus::Succeeded:Upscaling::GenerationPreparationStatus::Failed;
        }
        const auto result=SourceNvidiaFramePreparation::PrepareCompletedFrame(frame,*this);
        return result.prepared?Upscaling::GenerationPreparationStatus::Succeeded:Upscaling::GenerationPreparationStatus::Failed;
    }
    bool ExternalGuideRecoveryEnabled() const { return host.FsrFgActive(); }
    bool RecoverInvalidSourceGuides(const TheosRenderPipeline::SourceNvidiaFrameInputs& frame)
    {
        // The common admission gate already froze this real source into input.
        // No NR/vendor temporal call may consume missing/stale guides.
        const auto scaled=host.loadingScreenUpscaler_.Evaluate(host.context_.Get(),frame.input,frame.output);
        if(FAILED(scaled)){host.FailLifecycle(scaled,"DLSS spatial guide recovery");return false;}
        auto recovery=frame.sourceSnapshot;recovery.output=frame.output;recovery.depth=recovery.motion=nullptr;recovery.reset=true;
        auto prepared=host.PrepareExternalGeneration(recovery,TheosRenderPipeline::Upscaling::UpscaleOutcome::SpatialRecovery);
        host.resetNextEvaluation_=true;
        host.sourceRecoveryActive_=true;
        host.status_="DLSS guides unavailable; spatial real frame, NR and FSR FG suppressed";
        return bool(prepared);
    }
    bool CaptureCamera(const TheosRenderPipeline::SourceNvidiaFrameGuides& frame)
    {
        return TheosRenderPipeline::SourceDLSSG::CaptureCameraConstants(upscaler.mGraphicsState,
            frame.renderWidth, frame.renderHeight, frame.jitterX, frame.jitterY,
            frame.reset, frame.jitterEnabled, constants);
    }
    bool Prepare(const TheosRenderPipeline::SourceNvidiaFrameGuides& frame)
    {
        ScopedD3D11PerformanceStage timer{host.context_.Get(), PerformanceTuning::D3D11Stage::kFrameGenInputs};
        return TheosRenderPipeline::SourceDLSSG::Backend::Get().Prepare(constants, frame.motion, frame.depth,
            frame.uiColorAndAlpha, frame.hudLessColor, {frame.renderWidth, frame.renderHeight},
            frame.outputWidth, frame.outputHeight, NeuralEligible(frame));
    }
    void PublishGeneration(bool prepared)
    {
        host.SetRuntimeEnabled(TheosRenderPipeline::SourceGenerationEnabled(host.warmupPresentsRemaining_,
            SourceFrameGeneration::GetSingleton()->RuntimeInterpolationRequested(), prepared,
            upscaler.FrameGenerationTransitionBlocked()));
    }
};

bool NvidiaHost::EvaluateSourceNvidiaFrame(bool nativeUIHandoff, bool resetHistory)
{
    auto& upscaler = *RenderPipeline::GetSingleton();
    const float jitterEnabled = upscaler.mEnableJitter ? 1.0f : 0.0f;
    TheosRenderPipeline::SourceNvidiaFrameInputs frame{};
    frame.color = gameTargets_.GameFacing();
    frame.input = gameTargets_.UpscaleInput();
    frame.output = gameTargets_.UpscaleOutput();
    frame.motion = upscaler.mMotionVectors.mImage;
    frame.depth = upscaler.mDepthBuffer.mImage;
    // Stable tag identity is published now and filled after native UI drawing,
    // before Streamline consumes it at Present. Startup foreground is separate.
    frame.uiColorAndAlpha = nativeUIHandoff && nativeUI_.Available() ? nativeUI_.TaggedTexture() : nullptr;
    frame.hudLessColor = nativeUIHandoff ? gameTargets_.UpscaleOutput() : nullptr;
    frame.renderWidth = renderWidth_;
    frame.renderHeight = renderHeight_;
    frame.outputWidth = outputWidth_;
    frame.outputHeight = outputHeight_;
    TheosRenderPipeline::SourceDLSSG::NeuralOptions neuralOptions;
    if(!FsrFgActive())neuralOptions=TheosRenderPipeline::SourceDLSSG::Backend::Get().NeuralConfiguration();
    else {
        const auto& preferences=SourceFrameGeneration::GetSingleton()->settings.sourceDLSSG;
        neuralOptions.enabled=preferences.neuralEnabled;neuralOptions.tuning=preferences.neuralTuning;
        neuralOptions.beforeUpscaling=preferences.neuralBeforeUpscaling;neuralOptions.passes=preferences.neuralPasses;
        neuralOptions.secondPass=preferences.neuralSecondPass;neuralOptions.combat=preferences.neuralCombat;
    }
    bool sharpening = upscaler.mSharpening;
    float sharpness = upscaler.mSharpness;
    TheosRenderPipeline::Appearance::Runtime::Get().Apply(neuralOptions, sharpening, sharpness);
    frame.sharpness = sharpening ? sharpness : 0.0f;
    frame.jitterX = upscaler.mJitterOffsets[0] * jitterEnabled;
    frame.jitterY = upscaler.mJitterOffsets[1] * jitterEnabled;
    frame.motionScaleX = static_cast<float>(renderWidth_);
    frame.motionScaleY = static_cast<float>(renderHeight_);
    frame.reset = resetHistory || loadingScreenRoute_.NeedsTemporalReset();
    frame.jitterEnabled = upscaler.mEnableJitter;
    auto& snapshot=frame.sourceSnapshot;
    snapshot.backend=upscaler.mUpscaleType==DLAA?TheosRenderPipeline::Upscaling::BackendKind::Dlaa:TheosRenderPipeline::Upscaling::BackendKind::Dlss;
    snapshot.sourceId=FsrFgActive()?presentCount_+1:upscaler.mRenderedFrameCount;
#if !defined(TRP_NO_NEURAL_RENDERING)
    snapshot.sourceEpoch=communityEpoch_;
#endif
    snapshot.deltaMilliseconds=upscaler.mSourceDeltaMilliseconds;
    snapshot.render=snapshot.subrect={renderWidth_,renderHeight_};snapshot.display={outputWidth_,outputHeight_};
    snapshot.jitterX=frame.jitterX;snapshot.jitterY=frame.jitterY;snapshot.reset=frame.reset;
    snapshot.depthFormat=DXGI_FORMAT_R32_FLOAT;snapshot.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
    snapshot.motionConvention={frame.motionScaleX,frame.motionScaleY,true,false};
    auto* ui=RE::UI::GetSingleton();
    const bool world=ui && !ui->IsMenuOpen(RE::MainMenu::MENU_NAME) && !ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME);
    if(FsrFgActive() && world) {
        auto measured=TheosRenderPipeline::CaptureGameCameraMeasurements(upscaler.mGraphicsState,snapshot.render,frame.jitterEnabled,frame.reset);
        if(measured)snapshot.camera=*measured;
    }
    SourceNvidiaEvaluationOperations operations{*this, upscaler, std::move(neuralOptions)};
    loadingScreenResult_ = S_OK;
    const auto result = TheosRenderPipeline::SourceNvidiaFrameEvaluator::Evaluate(context_.Get(), frame, operations);
    if (!result.upscaled) {
#if defined(TRP_ENABLE_FSR_FG)
        if(FsrFgActive() && SUCCEEDED(loadingScreenUpscaler_.Evaluate(context_.Get(),frame.input,frame.output))) {
            auto recovery=frame.sourceSnapshot;recovery.output=frame.output;recovery.depth=frame.depth;recovery.motion=frame.motion;
            recovery.reset=true;
            if(PrepareExternalGeneration(recovery,TheosRenderPipeline::Upscaling::UpscaleOutcome::SpatialRecovery)) {
                resetNextEvaluation_=sourceRecoveryActive_=true;++sourceRecoveryFailures_;
                status_=std::format("DLSS/NR source recovery for {} frames; last NGX result=0x{:08X}; spatial image, FSR FG inactive",
                    sourceRecoveryFailures_,DLSSBackend::GetSingleton()->LastEvalResult());
                if(sourceRecoveryFailures_==1)logger::warn("[Source recovery] {}",status_);
                else if(sourceRecoveryFailures_%120==0)logger::error("[Source recovery] {}",status_);
                return true;
            }
        }
#endif
        SetRuntimeEnabled(false);
        const auto& backend = TheosRenderPipeline::SourceDLSSG::Backend::Get();
        if (FAILED(loadingScreenResult_)) {
            status_ = std::format("Loading-screen scaling failed (0x{:08X}); generation held off",
                static_cast<std::uint32_t>(loadingScreenResult_));
        } else {
            status_ = !FsrFgActive() && !backend.Ready() ? backend.Status() :
                std::format("RaZkolbaS direct DLSS evaluation failed (0x{:08X}); generation held off",
                    DLSSBackend::GetSingleton()->LastEvalResult());
        }
        if (!splitSourceRuntimeFailureLogged_) {
            splitSourceRuntimeFailureLogged_ = true;
            logger::error("[NvidiaHost] {}", status_);
        }
        return false;
    }
    if(!sourceRecoveryActive_ && sourceRecoveryFailures_) {
        logger::info("[Source recovery] temporal DLSS source resumed after {} recovery frames",sourceRecoveryFailures_);
        sourceRecoveryFailures_=0;
    }
    if (!FsrFgActive() && !result.prepared && !splitSourceRuntimeFailureLogged_) {
        logger::warn("[SourceDLSSG] generation held off cameraValid={} backend={}", result.cameraValid,
            TheosRenderPipeline::SourceDLSSG::Backend::Get().Status());
    }
    splitSourceRuntimeFailureLogged_ = !result.prepared;
    return true;
}

TheosRenderPipeline::Upscaling::Result<void> NvidiaHost::PrepareExternalGeneration(
    const TheosRenderPipeline::Upscaling::UpscaleFrame& frame,TheosRenderPipeline::Upscaling::UpscaleOutcome outcome)
{
    using namespace TheosRenderPipeline::Upscaling;
#if defined(TRP_ENABLE_FSR_FG)
    if(!FsrFgActive() || !fsrResources_ || !fsrPresentation_)return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_UNEXPECTED,"External FSR owner unavailable"});
    fsrGenerationFrame_=frame;fsrGenerationOutcome_=outcome;
    fsrSourceRenderedCount_=RenderPipeline::GetSingleton()->mRenderedFrameCount;
    auto* ui=RE::UI::GetSingleton();
    fsrMenu_=!ui || ui->IsMenuOpen(RE::MainMenu::MENU_NAME) || ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME) ||
        loadingScreenRoute_.Active(presentCount_) || RenderPipeline::GetSingleton()->FrameGenerationTransitionBlocked();
    const FsrInputPolicy input{frame.camera.depthInverted,frame.camera.depthInfinite,frame.motionConvention.includesJitter,true};
    const bool valid=outcome==UpscaleOutcome::Temporal && frame.camera.identity && !fsrMenu_;
    if(valid) {
        if(fsrResources_->GenerationInputPolicy()!=input) {
            const auto old=fsrResources_->GenerationInputPolicy();
            logger::info("[FSR FG] guide convention changed inverted={}->{} infinite={}->{} jitter={}->{}; retire/recreate FG feature at Present",
                old.depthInverted,input.depthInverted,old.depthInfinite,input.depthInfinite,old.motionIncludesJitter,input.motionIncludesJitter);
        }
        auto configured=fsrResources_->EnsureInputPolicy(input);if(!configured)return configured;
        ++fsrGuideCaptureCount_;
    }else fsrGenerationOutcome_=UpscaleOutcome::SkippedInvalidInput;
    fsrSourcePending_=true;
    return {};
#else
    (void)frame;(void)outcome;return std::unexpected(RuntimeError{ErrorKind::NoProvider,E_NOTIMPL,"FSR FG is not included"});
#endif
}
