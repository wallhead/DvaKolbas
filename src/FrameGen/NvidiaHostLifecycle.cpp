#include "DLSSBackend.h"
#include "RenderPipeline.h"
#include "NativeInput.h"
#include "NvidiaHost.h"
#include "CommunityShaderIntegration.h"
#include "LoadingArtwork.h"
#include "SourceDLSSGBackend.h"
#include "SourceDLSSGCamera.h"
#include "SourceFrameGeneration.h"
#include <PCH.h>

#include "SourceHostLifecycle.h"

struct NvidiaHost::LifecycleOperations
{
    NvidiaHost& host;
    IDXGISwapChain* swapChain{};
    bool resizing{};
    bool RebuildGameFacing() { return host.CreateGameFacingResources(swapChain); }
    bool RebuildUpscaler() { return host.CompleteStartupAfterDeviceCreation(); }
    void RequestHistoryReset() { host.resetNextEvaluation_ = true; }
    HRESULT Fail(HRESULT result, const char* operation) { return host.FailLifecycle(result, operation); }
    void DisableGeneration() { host.SetRuntimeEnabled(false); }
    bool Retire()
    {
#if !defined(TRP_NO_NEURAL_RENDERING)
        if (!host.RetireCommunityNeural()) return false;
#endif
#if defined(TRP_ENABLE_FSR_FG)
        if (host.FsrFgActive()) {
            auto retired=host.fsrPresentation_->Retire();
            if (!retired) {host.status_=retired.error().message;host.FailLifecycle(E_FAIL,"AMD presentation retirement");return false;}
#if defined(TRP_ENABLE_XESS)
            if(host.xessResources_) {
                const auto sourceRetired=host.xessResources_->Retire();
                if(!sourceRetired){host.status_=sourceRetired.error().message;host.FailLifecycle(E_FAIL,"XeSS resource retirement");return false;}
            }
#endif
            return true;
        }
#endif
#if defined(TRP_ENABLE_XESS_FG)
        if(host.XessFgActive()) {
            host.StopXessEngineObserver();
            auto retired=host.xessPresentation_->Retire();
            if(!retired){host.status_=retired.error().message;host.FailLifecycle(E_FAIL,"Intel presentation retirement");return false;}
            host.ReleaseSourceUpscaler();return SUCCEEDED(host.FailureResult());
        }
#endif
        if (!host.OrdinarySourceActive()) { return TheosRenderPipeline::SourceDLSSG::Backend::Get().Quiesce(); }
        const auto result = host.ordinaryPresentation_.Retire();
        if (FAILED(result)) { host.FailLifecycle(result, "FSR presentation retirement"); return false; }
#if defined(TRP_ENABLE_FSR)
        if (host.fsrResources_) {
            const auto retired = resizing ? host.fsrResources_->ReleaseSizedAfterRetirement() : host.fsrResources_->Retire();
            if (!retired) { host.status_ = retired.error().message; host.FailLifecycle(E_FAIL, "FSR resource retirement"); return false; }
            if (resizing) { host.fsrSizingRetainedForResize_ = true; }
        }
#endif
 #if defined(TRP_ENABLE_XESS)
        if(host.xessResources_) {
            const auto retired=host.xessResources_->Retire();
            if(!retired){host.status_=retired.error().message;host.FailLifecycle(E_FAIL,"XeSS resource retirement");return false;}
        }
 #endif
        return true;
    }
    void EndUI() { host.EndNativeUIPass(); }
    void ClearAndFlush()
    {
        host.context_->ClearState();
        host.context_->Flush();
    }
    void ReleaseGameFacing() { host.gameTargets_.ResetGameFacingAfterRetirement(); }
    void ReleaseUpscaler() { host.ReleaseSourceUpscaler(resizing && host.FsrActive()); }
    void ReleasePresentation() { host.presentation_.ResetAfterRetirement(); }
    void UnpublishInput() { TheosRenderPipeline::NativeInput::Publish(0, 0); }
    void DetachFailedHost()
    {
        host.proxyActive_ = false;
        host.outerSwapChain_ = nullptr;
    }
    void BeginDestruction()
    {
        host.proxyActive_ = false;
        host.sourceUpscalerInitializationPending_ = false;
    }
    void DetachSwapchains()
    {
        host.outerSwapChain_ = nullptr;
        host.innerSwapChain_ = nullptr;
    }
    void ResetSession() { host.ResetSessionAfterRetirement(); }
};

HRESULT NvidiaHost::FailLifecycle(HRESULT result, const char* operation)
{
    if (FAILED(FailureResult())) { return FailureResult(); }
    const auto failure = lifecycleFailure_.Fail(result);
    const auto detail = status_;
    sourceUpscalerInitializationPending_ = false;
    SetRuntimeEnabled(false);
    TheosRenderPipeline::NativeInput::Publish(0, 0);
    status_ = std::format("{} failed (0x{:08X}); rendering stopped, restart required. {}",
        operation, static_cast<std::uint32_t>(failure), detail);
    logger::critical("[NvidiaHost] {}", status_);
    logger::error("[Renderer failure] operation={} backend={} present={} evaluations={} upscales={} render={}x{} output={}x{} deviceRemovedReason=0x{:08X}",
        operation, FsrActive()?"FSR":"NVIDIA", presentCount_, evaluationCount_, upscaleEvaluationCount_,
        renderWidth_, renderHeight_, outputWidth_, outputHeight_, static_cast<std::uint32_t>(device_?device_->GetDeviceRemovedReason():S_OK));
    // The failure latch above guarantees one snapshot, even with frame logs off.
    LogNativeUIState("failure", false, true);
    // Keep ownership until the normal teardown proves retirement. In particular,
    // do not clear the feature-exists flags used by ReleaseSourceUpscaler here.
    return failure;
}

HRESULT NvidiaHost::BeforeResizeBuffers(IDXGISwapChain* a_swapChain)
{
    if (FAILED(FailureResult())) { return FailureResult(); }
    if (!proxyActive_ || a_swapChain != innerSwapChain_)
    {
        return S_OK;
    }
    LifecycleOperations operations{*this, nullptr, true};
#if defined(TRP_ENABLE_XESS)
    // Refuse before retiring NR/presentation or releasing any game resource.
    // The caller may retry on the creation thread; do not invoke the SDK here.
    if(xessResources_ && xessResources_->Upscaler() && !xessResources_->Upscaler()->OnOwnerThread()) {
        if(!(xessDeferredReported_ & 4u)) {
            xessDeferredReported_ |= 4u;
            logger::warn("[XeSS resize] deferred before retirement: ownerThread={} currentThread={}; retry ResizeBuffers on the owner thread",
                xessResources_->Upscaler()->OwnerThread(),GetCurrentThreadId());
        }
        return DXGI_ERROR_WAS_STILL_DRAWING;
    }
#endif
    if (!TheosRenderPipeline::SourceHostLifecycle::BeforeResize(operations)) { return DXGI_ERROR_WAS_STILL_DRAWING; }
    return OrdinarySourceActive() ? ordinaryPresentation_.BeforeResize() : S_OK;
}

HRESULT NvidiaHost::AfterResizeBuffers(IDXGISwapChain* a_swapChain, HRESULT a_result)
{
    if (FAILED(FailureResult())) { return FailureResult(); }
    if (!proxyActive_ || a_swapChain != innerSwapChain_)
    {
        return a_result;
    }
    LifecycleOperations operations{*this, a_swapChain};
    if (OrdinarySourceActive()) { a_result = ordinaryPresentation_.AfterResize(a_result); }
    return TheosRenderPipeline::SourceHostLifecycle::AfterResize(operations, a_result);
}

void NvidiaHost::OnGameFacingSwapChainDestroyed(IDXGISwapChain* a_swapChain)
{
    if (a_swapChain != outerSwapChain_)
    {
        return;
    }
    LifecycleOperations operations{*this};
    TheosRenderPipeline::SourceHostLifecycle::Destroy(operations);
}

void NvidiaHost::ResetSessionAfterRetirement()
{
#if defined(TRP_ENABLE_FSR)
#if defined(TRP_ENABLE_FSR_FG)
    fsrPresentation_.reset();fsrFactory_.Reset();fsrForeground_.Reset();fsrSourcePending_=fsrUiComplete_=false;
    fsrSourceRenderedCount_=fsrGuideCaptureCount_=0;
#endif
    fsrFrame_.reset();
    fsrResources_.reset();
    fsrSizingRetainedForResize_=false;
    lastFsrTemporal_=false;
#endif
#if defined(TRP_ENABLE_XESS_FG)
    StopXessEngineObserver();xessPresentation_.reset();xessPresentationScene_={};xessTimingBound_=false;
    xessEngineSource_=xessEngineTrace_=0;xessPresentTrace_=0;
#endif
    ordinaryPresentation_.ResetAfterRetirement();
    outputWindow_ = nullptr;
    outputWidth_ = 0;
    outputHeight_ = 0;
    renderWidth_ = 0;
    renderHeight_ = 0;
    runtimeStateObservationCount_ = 0;
    runtimeDLSSGStatus_ = 0;
    runtimeFramesActuallyPresented_ = 0;
    runtimeMinWidthOrHeight_ = 0;
    runtimeMaxGeneratedFrames_ = 0;
    presentCount_ = 0;
    failedPresentCount_ = 0;
    lastPresentResult_ = S_OK;
    warmupPresentsRemaining_ = 0;
    nativeUIContexts_.ResetAfterRetirement();
    context_.Reset();
    device_.Reset();
}

bool NvidiaHost::FsrPresentSuspended()const
{
#if defined(TRP_ENABLE_FSR_FG)
    return fsrPresentation_ && fsrPresentation_->Suspended();
#else
    return false;
#endif
}
HRESULT NvidiaHost::UpdateFsrSuspension()
{
#if defined(TRP_ENABLE_FSR_FG)
    if(FAILED(FailureResult()))return FailureResult();
    if(!FsrFgActive() || !FsrPresentSuspended())return S_OK;
    RECT client{};
    if(!GetClientRect(outputWindow_,&client) || client.right<=client.left || client.bottom<=client.top)return DXGI_STATUS_OCCLUDED;
    auto resumed=fsrPresentation_->Resume();
    if(!resumed){status_=resumed.error().message;return FailLifecycle(E_FAIL,"AMD client restoration");}
    // Resume the retained game buffers. DXGI may scale them to a restored
    // client; an explicit ResizeBuffers still owns any resolution change.
    resetNextEvaluation_=true;nativeUIPass_.ResetEvaluation();
    logger::info("[FSR resize] client restored; retained AMD chain/game buffers resumed with temporal reset");
#endif
    return S_OK;
}

void NvidiaHost::ReleaseSourceUpscaler(bool retainFsrDevice)
{
#if !defined(TRP_NO_NEURAL_RENDERING)
    if (!RetireCommunityNeural()) return;
#endif
#if defined(TRP_ENABLE_XESS)
    if(xessResources_) {
        const auto retired=xessResources_->Retire();
        if(!retired){status_=retired.error().message;FailLifecycle(E_FAIL,"XeSS feature release");return;}
        xessResources_.reset();xessEncode_={};lastXessTemporal_=false;xessRecovery_=false;xessRecoveryReason_.clear();xessDeferredReported_=0;
        xessHasCompleted_=false;xessCompletedFrame_={};
    }
#endif
#if defined(TRP_ENABLE_FSR)
    if ((FsrActive() || FsrFgActive()) && fsrResources_) {
        if(!retainFsrDevice){
            const auto retired = fsrResources_->Retire();
            if (!retired) { status_ = retired.error().message; FailLifecycle(E_FAIL, "FSR feature release"); return; }
        }
        fsrFrame_.reset();
        lastFsrTemporal_=false;
    }
#endif
    TheosRenderPipeline::ReShadeIntegration::Get().ResetAfterRetirement();
    communityFrame_.ResetAfterRetirement();
    EndNativeUIPass();
    startupOverlay_.ResetAfterRetirement();
    previewDraw_.ResetAfterRetirement();
    TheosRenderPipeline::LoadingArtwork::ResetAfterRetirement();
    loadingScreenUpscaler_.ResetAfterRetirement();
    loadingScreenRoute_.ResetAfterRetirement();
    loadingFade_.Reset();
    presentationFade_.ResetAfterRetirement();
    loadingScreenResult_ = S_OK;
    loadingScreenLogged_ = false;
    startupWorldFrame_ = ~std::uint64_t{};
    sourceRenderThread_ = 0;
    nativeUIPass_.ResetEvaluation();
    nativeUIAttachments_.ResetAfterRetirement();
    nativeUI_.ResetAfterRetirement();
    gameTargets_.ResetUpscalerAfterRetirement();
    if (upscalerReady_ && splitSourceDLSSActive_)
    {
        DLSSBackend::GetSingleton()->ReleaseFeature();
    }
    upscalerReady_ = false;
    splitSourceDLSSActive_ = false;
}

void NvidiaHost::OnPresentCompleted(HRESULT a_result)
{
    if (!proxyActive_ || FAILED(FailureResult()))
    {
        return;
    }

    ++presentCount_;
    TheosRenderPipeline::ReShadeIntegration::Get().PresentCompleted();
    if (TheosRenderPipeline::CommunityShaders::Active()) { communityFrame_.PresentCompleted(SUCCEEDED(a_result)); }
    const auto previousResult = lastPresentResult_;
    lastPresentResult_ = a_result;
    if (FAILED(a_result))
    {
        ++failedPresentCount_;
        if (failedPresentCount_ <= 3 || a_result != previousResult)
        {
            logger::error("[NvidiaHost] outer Present failed result=0x{:08X} "
                          "failures={} presents={}",
                          static_cast<std::uint32_t>(a_result), failedPresentCount_, presentCount_);
            if ((a_result == DXGI_ERROR_DEVICE_REMOVED || a_result == DXGI_ERROR_DEVICE_RESET ||
                    a_result == DXGI_ERROR_DEVICE_HUNG) && device_)
            {
                logger::error("[NvidiaHost] D3D11 device removed reason=0x{:08X}",
                              static_cast<std::uint32_t>(device_->GetDeviceRemovedReason()));
            }
        }
    }
    else if (FAILED(previousResult))
    {
        logger::info("[NvidiaHost] outer Present recovered result=0x{:08X} "
                     "failures={} presents={}",
                     static_cast<std::uint32_t>(a_result), failedPresentCount_, presentCount_);
    }

    // Consume the session snapshot after Present; querying Streamline again
    // here would consume its output-count delta a second time.
    if (!OrdinarySourceActive() && !NativeGenerationActive()) {
        const auto& state = TheosRenderPipeline::SourceDLSSG::Backend::Get().Snapshot().state;
        UpdateRuntimeDLSSGState(static_cast<std::uint32_t>(state.status), state.numFramesActuallyPresented, state.minWidthOrHeight,
                                state.numFramesToGenerateMax);
    }

    if (StartupConfigured() && SUCCEEDED(a_result) && !TheosRenderPipeline::CommunityShaders::Active())
    {
        ApplySourceUpscalerSettingsAfterPresent();
        if (FAILED(FailureResult())) { return; }
    }
    if (warmupPresentsRemaining_ <= 0)
    {
        return;
    }

    // Advance host warm-up after every real outer Present, including failed
    // DXGI calls. Do not enable generation at this boundary:
    // the next valid evaluation must still prove complete color, depth, motion,
    // and presentation inputs before RaZkolbaS crosses the runtime boundary.
    --warmupPresentsRemaining_;
    if (warmupPresentsRemaining_ == 0)
    {
        logger::info("[NvidiaHost] 600-Present host warm-up complete "
                     "result=0x{:08X}; waiting for next valid frame inputs",
                     static_cast<std::uint32_t>(a_result));
        status_ = "NVIDIA host warm-up complete; waiting for valid frame inputs";
    }
    else if (warmupPresentsRemaining_ == 599 || warmupPresentsRemaining_ % 120 == 0)
    {
        logger::info("[NvidiaHost] host warm-up presents remaining={} lastResult=0x{:08X}", warmupPresentsRemaining_,
                     static_cast<std::uint32_t>(a_result));
    }
}

void NvidiaHost::ArmFrameGenerationWarmup()
{
    static constexpr std::int32_t kHostWarmupPresents = 600;
    warmupPresentsRemaining_ = kHostWarmupPresents;
    SetRuntimeEnabled(false);
    status_ = std::format("NVIDIA source ready; host warm-up {} Presents remaining", warmupPresentsRemaining_);
    logger::info("[NvidiaHost] armed host warm-up presents={}", warmupPresentsRemaining_);
}

void NvidiaHost::SetRuntimeEnabled(bool a_enabled)
{
    if (NativeGenerationActive()) { frameGenerationStateKnown_=true;frameGenerationEnabled_=false;return; }
    if (OrdinarySourceActive()) {
        if (!frameGenerationStateKnown_ || frameGenerationEnabled_) { resetNextEvaluation_ = true; }
        frameGenerationStateKnown_ = true;
        frameGenerationEnabled_ = false;
        return;
    }
    if (frameGenerationStateKnown_ && frameGenerationEnabled_ == a_enabled)
    {
        return;
    }
    TheosRenderPipeline::SourceDLSSG::Backend::Get().SetEnabled(a_enabled);
    frameGenerationStateKnown_ = true;
    frameGenerationEnabled_ = a_enabled;
    if (!a_enabled)
    {
        resetNextEvaluation_ = true;
    }
    logger::info("[NvidiaHost] runtime generation {}", a_enabled ? "enabled" : "disabled");
}

void NvidiaHost::UpdateRuntimeDLSSGState(std::uint32_t a_status, std::uint32_t a_framesActuallyPresented, std::uint32_t a_minWidthOrHeight,
                                         std::uint32_t a_maxGeneratedFrames)
{
    const bool changed = runtimeDLSSGStatus_ != a_status || runtimeFramesActuallyPresented_ != a_framesActuallyPresented ||
                         runtimeMinWidthOrHeight_ != a_minWidthOrHeight || runtimeMaxGeneratedFrames_ != a_maxGeneratedFrames;
    runtimeDLSSGStatus_ = a_status;
    runtimeFramesActuallyPresented_ = a_framesActuallyPresented;
    runtimeMinWidthOrHeight_ = a_minWidthOrHeight;
    runtimeMaxGeneratedFrames_ = a_maxGeneratedFrames;
    ++runtimeStateObservationCount_;

    const auto observationCount = RuntimeStateObservationCount();
    if (observationCount <= 3 || changed || observationCount % 600 == 0)
    {
        logger::info("[NvidiaHost] runtime DLSS-G state source=source-session observation={} "
                     "status={} actuallyPresented={} maxGenerated={} minDimension={}",
                     observationCount, a_status, a_framesActuallyPresented, a_maxGeneratedFrames, a_minWidthOrHeight);
    }
}

TheosRenderPipeline::Upscaling::Result<void> NvidiaHost::QuiesceActivePresentation()
{
#if defined(TRP_ENABLE_XESS_FG)
    if(XessFgActive())return xessPresentation_->Suspend();
#endif
#if defined(TRP_ENABLE_FSR_FG)
    if(FsrFgActive()) {
        if(!fsrPresentation_)return std::unexpected(TheosRenderPipeline::Upscaling::RuntimeError{TheosRenderPipeline::Upscaling::ErrorKind::RetirementFailure,E_UNEXPECTED,"FSR presenter unavailable"});
        return fsrPresentation_->Suspend();
    }
#endif
    if(TheosRenderPipeline::SourceDLSSG::Backend::Get().Quiesce())return {};
    return std::unexpected(TheosRenderPipeline::Upscaling::RuntimeError{TheosRenderPipeline::Upscaling::ErrorKind::RetirementFailure,E_FAIL,"NVIDIA presenter retirement failed"});
}
TheosRenderPipeline::Upscaling::Result<void> NvidiaHost::ResumeActivePresentation()
{
#if defined(TRP_ENABLE_XESS_FG)
    if(XessFgActive()) {
        auto result=xessPresentation_->Resume();
        if(result){resetNextEvaluation_=true;fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();}
        return result;
    }
#endif
#if defined(TRP_ENABLE_FSR_FG)
    if(FsrFgActive()) {
        auto resumed=fsrPresentation_->Resume();
        if(resumed){resetNextEvaluation_=true;fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();}
        return resumed;
    }
#endif
    if(TheosRenderPipeline::SourceDLSSG::Backend::Get().ResumeAfterResize())return {};
    return std::unexpected(TheosRenderPipeline::Upscaling::RuntimeError{TheosRenderPipeline::Upscaling::ErrorKind::RetirementFailure,E_FAIL,"NVIDIA presenter restoration failed"});
}
