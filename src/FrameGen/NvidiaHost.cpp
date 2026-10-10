#include "NvidiaHost.h"
#include "DLSSBackend.h"
#include "RenderPipeline.h"
#include "NativeInput.h"
#include "SourceDLSSGBackend.h"
#include "SourceDLSSGCamera.h"
#include "SourceFrameGeneration.h"
#include "PerformanceTuning.h"
#include "CommunityShaderIntegration.h"
#include "SourceGenerationPolicy.h"
#include <PCH.h>

bool NvidiaHost::EvaluateFrame(IDXGISwapChain* a_swapChain, bool a_nativeUIHandoff)
{
    if(XessActive())return EvaluateXessFrame(a_swapChain,a_nativeUIHandoff);
    if(FsrActive())return EvaluateFsrFrame(a_swapChain,a_nativeUIHandoff);
    if (FAILED(FailureResult()) || !proxyActive_ || a_swapChain != outerSwapChain_ || !splitSourceDLSSActive_ || !upscalerReady_ || !gameTargets_.GameFacing() ||
        !gameTargets_.UpscaleInput() || !gameTargets_.UpscaleOutput() || !context_ || presentation_.Buffers().empty())
    {
        return false;
    }
#if defined(TRP_ENABLE_FSR_FG)
    if(FsrFgActive()) {
        if(UpdateFsrSuspension()!=S_OK)return false;
        const auto waited=fsrPresentation_->WaitBeforeProducer();
        if(FAILED(waited)){FailLifecycle(waited,"DLSS FSR producer ownership");return false;}
        fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();
    }
#endif
    if(XessFgActive()) {
        if(UpdateXessSuspension()!=S_OK || FAILED(WaitXessProducer()))return false;
#if defined(TRP_ENABLE_XESS_FG)
        fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();
#endif
    }
    auto* upscaler = RenderPipeline::GetSingleton();
    if(!NativeGenerationActive()) {
    if (!upscaler->mMotionVectors.mImage || !upscaler->mDepthBuffer.mImage)
    {
        SetRuntimeEnabled(false);
        status_ = "NVIDIA source upscaler waiting for motion and depth";
        return false;
    }
    D3D11_TEXTURE2D_DESC motionDesc{};
    D3D11_TEXTURE2D_DESC depthDesc{};
    upscaler->mMotionVectors.mImage->GetDesc(&motionDesc);
    upscaler->mDepthBuffer.mImage->GetDesc(&depthDesc);
    if (motionDesc.Width != renderWidth_ || motionDesc.Height != renderHeight_ || depthDesc.Width != renderWidth_ ||
        depthDesc.Height != renderHeight_)
    {
        SetRuntimeEnabled(false);
        status_ = "NVIDIA source upscaler rejected stale guide extents";
        if (!evaluationFailureLogged_)
        {
            evaluationFailureLogged_ = true;
            logger::error("[NvidiaHost] guide extent mismatch motion={}x{} "
                          "depth={}x{} expected={}x{}",
                          motionDesc.Width, motionDesc.Height, depthDesc.Width, depthDesc.Height, renderWidth_, renderHeight_);
        }
        return false;
    }
    }
    if (!PresentationBackendReadyForEvaluation())
    {
        SetRuntimeEnabled(false);
        status_ = "NVIDIA Streamline presentation backend is not ready";
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGISwapChain3> indexedSwapChain;
    if (FAILED(innerSwapChain_->QueryInterface(IID_PPV_ARGS(&indexedSwapChain))))
    {
        if (!evaluationFailureLogged_)
        {
            evaluationFailureLogged_ = true;
            logger::error("[NvidiaHost] source bridge could not query IDXGISwapChain3");
        }
        return false;
    }
    const auto currentIndex = presentation_.BufferIndex(indexedSwapChain->GetCurrentBackBufferIndex());
    if (currentIndex >= presentation_.Buffers().size())
    {
        if (!evaluationFailureLogged_)
        {
            evaluationFailureLogged_ = true;
            logger::error("[NvidiaHost] current buffer index {} exceeds cached count {}", currentIndex, presentation_.Buffers().size());
        }
        return false;
    }

    bool resetHistory = resetNextEvaluation_ || upscaler->mPendingHistoryResets > 0;
    // Consume the source reset before evaluating. Source recovery may arm it
    // again; FG skips already invalidate their own FsrGenerationHistory.
    resetNextEvaluation_ = false;
    sourceRecoveryActive_ = false;

    if (!EvaluateSourceNvidiaFrame(a_nativeUIHandoff, resetHistory))
    {
        resetNextEvaluation_ = true;
        return false;
    }

    {
        ScopedD3D11PerformanceStage timer{context_.Get(), PerformanceTuning::D3D11Stage::kPresentationCopy};
        context_->CopyResource(presentation_.Buffers()[currentIndex].Get(), gameTargets_.UpscaleOutput());
    }
    if (!resetNextEvaluation_ && upscaler->mPendingHistoryResets > 0)
    {
        --upscaler->mPendingHistoryResets;
    }
    ++evaluationCount_;
    evaluationFailureLogged_ = false;
    if (StartupConfigured() && !sourceRecoveryActive_)
    {
        status_ = std::format("RaZkolbaS source DLSS + {}; camera/input valid={} warm-up={}",
            FsrFgActive()?"FSR FG":"Streamline DLSS-G", !splitSourceRuntimeFailureLogged_, warmupPresentsRemaining_);
    }
    if (evaluationCount_ == 1 || (PerformanceTuning::GetSingleton()->settings.diagnostics.frameDetails &&
        (evaluationCount_ <= 3 || evaluationCount_ % 600 == 0)))
    {
        if (StartupConfigured())
        {
            logger::info("[SourceDLSSG] {}", status_);
        }
        logger::info("[NvidiaHost] evaluation={} sourceOwner={} entry={} upscale={} "
                     "generation={} render={}x{} output={}x{} slot={} reset={}",
                     evaluationCount_, "RaZkolbaS-DLSS", "SourceNvidiaFrameEvaluator", upscaleEvaluationCount_, evaluationCount_,
                     renderWidth_, renderHeight_, outputWidth_, outputHeight_, currentIndex, resetHistory);
    }
    return true;
}

float NvidiaHost::OptimalMipmapBias() const
{
    if (!UpscalerReady())
    {
        return 0.0f;
    }
    if (FsrActive() || XessActive()) {
        return outputWidth_ ? std::log2(static_cast<float>(renderWidth_) / outputWidth_) - 1.0f : 0.0f;
    }
    return DLSSBackend::GetSingleton()->GetOptimalMipLodBias();
}

bool NvidiaHost::PrepareCommunityFrameForPresent()
{
    if (!proxyActive_ || !upscalerReady_ || FAILED(FailureResult()) || !gameTargets_.GameFacing()) { return false; }
    Microsoft::WRL::ComPtr<IDXGISwapChain3> indexed;
    if (FAILED(innerSwapChain_->QueryInterface(IID_PPV_ARGS(&indexed)))) { return false; }
    const auto index = presentation_.BufferIndex(indexed->GetCurrentBackBufferIndex());
    if (index >= presentation_.Buffers().size()) { return false; }
    if(FsrActive()) {
        context_->CopyResource(presentation_.Buffers()[index].Get(),gameTargets_.GameFacing());
        SetRuntimeEnabled(false);
        status_="Community Shaders owns upscaling; ordinary presentation, FSR not active";
        return true;
    }
    D3D11_TEXTURE2D_DESC desc{}; gameTargets_.GameFacing()->GetDesc(&desc);
    const bool prepared = communityFrame_.Prepare(desc);
    auto* pipeline = RenderPipeline::GetSingleton();
    SetRuntimeEnabled(TheosRenderPipeline::SourceGenerationEnabled(warmupPresentsRemaining_,
        SourceFrameGeneration::GetSingleton()->RuntimeInterpolationRequested(), prepared,
        pipeline->FrameGenerationTransitionBlocked()));
    // Every real presentation copies CS's completed image, including menus and
    // frames where camera/guide inputs are unavailable for generation.
    context_->CopyResource(presentation_.Buffers()[index].Get(), gameTargets_.GameFacing());
    if (prepared) {
        renderWidth_ = communityFrame_.RenderExtent().width;
        renderHeight_ = communityFrame_.RenderExtent().height;
        ++evaluationCount_;
        if (pipeline->mPendingHistoryResets > 0) { --pipeline->mPendingHistoryResets; }
    }
    status_ = communityFrame_.Status();
    if (presentCount_ < 3 || presentCount_ % 600 == 0) {
        logger::info("[CS Adapter] present={} prepared={} evaluations={} render={}x{} output={}x{} {}",
            presentCount_, prepared, evaluationCount_, renderWidth_, renderHeight_, outputWidth_, outputHeight_, status_);
    }
    return prepared;
}

HRESULT NvidiaHost::GetGameFacingBuffer(IDXGISwapChain* a_swapChain, UINT, REFIID a_iid, void** a_surface)
{
    if (!a_surface)
    {
        return E_POINTER;
    }
    *a_surface = nullptr;
    if (FAILED(FailureResult())) { return FailureResult(); }
    if (!proxyActive_ || a_swapChain != outerSwapChain_ || !gameTargets_.GameFacing())
    {
        return DXGI_ERROR_INVALID_CALL;
    }
    // The outer wrapper returns one stable texture for every game-facing
    // GetBuffer request. Skyrim keeps buffer 0 for the lifetime of its original
    // discard-model renderer, while the inner flip-model chain rotates normally.
    return gameTargets_.GameFacing()->QueryInterface(a_iid, a_surface);
}

void NvidiaHost::AdjustLegacyDescForCaller(DXGI_SWAP_CHAIN_DESC* a_desc, const void* a_returnAddress) const
{
    if (TheosRenderPipeline::CommunityShaders::Active() || !proxyActive_ || !a_desc || !a_returnAddress || !renderWidth_ || !renderHeight_)
    {
        return;
    }

    HMODULE callerModule = nullptr;
    if (!::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                              reinterpret_cast<LPCWSTR>(a_returnAddress), &callerModule))
    {
        return;
    }
    if (callerModule == ::GetModuleHandleW(L"d3d11.dll"))
    {
        a_desc->BufferDesc.Width = renderWidth_;
        a_desc->BufferDesc.Height = renderHeight_;
    }
}

bool NvidiaHost::PrepareNativeUITarget(UINT index)
{
    const auto result = presentation_.Select(device_.Get(), index);
    if (FAILED(result))
    {
        logger::warn("[CoreHost] native presentation/depth target unavailable "
                     "result=0x{:08X}",
                     static_cast<std::uint32_t>(result));
    }
    return SUCCEEDED(result);
}

bool NvidiaHost::CreateNativeUIExtractionResources(const D3D11_TEXTURE2D_DESC& desc)
{
    nativeUIExtractionFailureLogged_ = false;
    return nativeUI_.Initialize(device_.Get(), context_.Get(), gameTargets_.UpscaleOutput(), desc,
                                SourceFrameGeneration::GetSingleton()->settings.nativeUICompositionMode == 0);
}

bool NvidiaHost::ExtractNativeUIColorAndAlpha() { return nativeUI_.Extract(context_.Get(), presentation_.Texture()); }

void NvidiaHost::EndNativeUIPass() { nativeUIPass_.End(context_.Get()); }

bool NvidiaHost::FinishNativeUIPassForPresent()
{
    // The source path already ended redirection and ran any needed fallback
    // before drawing the overlay. A UI-only ENB handoff is valid on this route.
    if (StartupConfigured() && nativeUIPass_.Frame().PresentPrepared())
    {
        return FinishSourceFrameForPresent();
    }
    if (!nativeUIPass_.HasEarlyEvaluation())
    {

        return false;
    }
    nativeUIPass_.FinishForPresent(context_.Get(),
                                   [&]
                                   {
                                       const bool attemptedDedicatedUI = nativeUI_.Dedicated();
                                       const bool uiReady = !nativeUI_.Available() || (attemptedDedicatedUI ? CaptureAndComposeDedicatedNativeUI()
                                                                                                            : ExtractNativeUIColorAndAlpha());
                                       if (!uiReady)
                                       {
                                           if (StartupConfigured())
                                           {
                                               SetRuntimeEnabled(false);
                                           }
                                           nativeUI_.Invalidate();
                                           // A failed isolated path must return future UI draws to the composed
                                           // presentation row. Otherwise the game remains visible but all UI is
                                           // stranded in the private target.
                                           if (!nativeUIExtractionFailureLogged_)
                                           {
                                               nativeUIExtractionFailureLogged_ = true;
                                               logger::warn("[NvidiaHost] native UI {} failed; reverting to "
                                                            "HUD-less-only tagging",
                                                            attemptedDedicatedUI ? "texture composition" : "extraction");
                                           }
                                       }
                                   });
    static std::uint32_t logged = 0;
    if (logged < 3)
    {
        ++logged;
        logger::info("[NvidiaHost] Present consumed early native-UI evaluation; "
                     "duplicate evaluation skipped");
    }
    return true;
}

bool NvidiaHost::PresentationBackendReadyForEvaluation()
{
#if defined(TRP_ENABLE_FSR_FG)
    if (FsrFgActive()) return fsrPresentation_ && fsrPresentation_->SwapChain();
#endif
#if defined(TRP_ENABLE_XESS_FG)
    if(XessFgActive())return xessPresentation_ && xessPresentation_->SwapChain();
#endif
    return OrdinarySourceActive() ? ordinaryPresentation_.Ready() : TheosRenderPipeline::SourceDLSSG::Backend::Get().Ready();
}

HRESULT NvidiaHost::QueryFsrProducerDevice(REFIID iid,void** output) const
{
    if (!output)return E_POINTER;*output=nullptr;
    return NativeGenerationActive() && device_?device_->QueryInterface(iid,output):E_NOINTERFACE;
}
