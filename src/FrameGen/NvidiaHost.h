#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include "GameFacingTargets.h"
#include "FrameTelemetry.h"
#if !defined(TRP_NO_NEURAL_RENDERING)
#include "NeuralRendering/BeforeHost.h"
#include "NeuralRendering/SourcePolicy.h"
#include "NeuralRendering/FsrRouteDiagnostics.h"
#endif
#include "CommunityShaderAdapter.h"
#include "NativeUIAttachments.h"
#include "NativeUIComposition.h"
#include "NativeUIContexts.h"
#include "NativeUIPass.h"
#include "NvidiaUpscalerConfiguration.h"
#include "PresentationTargets.h"
#include "SourceFrameCoordinator.h"
#include "SourceHostBoundary.h"
#include "StartupOverlayPass.h"
#include "InventoryPreviewDraw.h"
#include "LoadingScreenState.h"
#include "LoadingScreenUpscaler.h"
#include "LoadingFadeIn.h"
#include "OrdinaryPresentation.h"
#include "UpscaleType.h"
#include "Upscaling/UpscalerBackend.h"
#include "Upscaling/FSRAvailability.h"
#if defined(TRP_ENABLE_XESS)
#include "Upscaling/XessHostResources.h"
#include "Upscaling/SdrColorConversion.h"
#endif
#if defined(TRP_ENABLE_FSR)
#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRFrameAdapter.h"
#endif
#if defined(TRP_ENABLE_FSR_FG)
#include "FSRHostPresentation.h"
#endif
#include "PresentationFade.h"
#include "ReShadeIntegration.h"
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <sl_dlss_g.h>
#include <string>
#include <vector>

class NvidiaHost
{
  public:
    static NvidiaHost* GetSingleton()
    {
        static NvidiaHost singleton;
        return &singleton;
    }

    HRESULT CreateSwapChain(IDXGIFactory* a_factory, ID3D11Device* a_device, DXGI_SWAP_CHAIN_DESC* a_desc, IDXGISwapChain** a_swapChain,
        TheosRenderPipeline::OriginalCreateSwapChain original = nullptr);
    bool CompleteStartupAfterDeviceCreation();
    TheosRenderPipeline::CommunityShaderAdapter& CommunityFrame() { return communityFrame_; }
    bool PrepareCommunityFrameForPresent();
    bool EvaluateFrame(IDXGISwapChain* a_swapChain, bool a_nativeUIHandoff = false);
    bool FinishNativeUIPassForPresent();
    void OnBackgroundReady(TheosRenderPipeline::BackgroundBoundary boundary, bool mainOrLoading);
    bool PrepareSourceFrameForPresent(IDXGISwapChain* swapChain);
    bool QueryStartupOverlay() const;
    bool BeginStartupOverlay();
    void EndStartupOverlay();
    TheosRenderPipeline::StartupOverlayPass& StartupOverlay() { return startupOverlay_; }
    TheosRenderPipeline::InventoryPreviewDraw& PreviewDraw() { return previewDraw_; }
    bool NativePresentReady() const { return StartupConfigured() && nativeUIPass_.Frame().PresentPrepared() && nativeUIPass_.HasEarlyEvaluation(); }
    bool NativeUIDrawnThisFrame() const { return nativeUIPass_.Frame().UIDrawn(); }
    const TheosRenderPipeline::NativeUIFrame& NativeFrame() const { return nativeUIPass_.Frame(); }
    const TheosRenderPipeline::NativeUIAttachments& UIAttachments() const { return nativeUIAttachments_; }
    bool SourceContext(ID3D11DeviceContext* context) const { return StartupConfigured() && nativeUIContexts_.Contains(context_.Get(), context); }
    void RegisterSourceGameContext(ID3D11DeviceContext* context);
    void LogSourceContextHook(unsigned hook, ID3D11DeviceContext* context) const;
    void LogNativeUIState(const char* stage, bool mainOrLoading, bool force=false);
    bool TakeNativeUITraceSlot();
    HRESULT GetGameFacingBuffer(IDXGISwapChain* a_swapChain, UINT a_buffer, REFIID a_iid, void** a_surface);
    void AdjustLegacyDescForCaller(DXGI_SWAP_CHAIN_DESC* a_desc, const void* a_returnAddress) const;
    HRESULT BeforeResizeBuffers(IDXGISwapChain* a_swapChain);
    HRESULT AfterResizeBuffers(IDXGISwapChain* a_swapChain, HRESULT a_result);
    HRESULT FailureResult() const { return lifecycleFailure_.Result(); }
    HRESULT FailLifecycle(HRESULT result, const char* operation);
    void OnPresentCompleted(HRESULT a_result);
    void OnGameFacingSwapChainDestroyed(IDXGISwapChain* a_swapChain);

    bool ProxyActive() const { return proxyActive_; }
    bool FrameGenerationEnabled() const { return frameGenerationEnabled_; }
    bool UpscalerReady() const { return upscalerReady_ && SUCCEEDED(FailureResult()); }
    bool SplitSourceDLSSActive() const { return splitSourceDLSSActive_; }
    bool FsrActive() const { return StartupConfigured() && sourceUpscalerSettings_.Startup().mode==FSR; }
    bool XessActive() const { return StartupConfigured() && sourceUpscalerSettings_.Startup().mode==Xess; }
    bool XessTemporalActive()const;
    bool OrdinarySourceActive()const { return (FsrActive() || XessActive()) && !FsrFgActive(); }
    TheosRenderPipeline::Telemetry::OutputCounter OrdinaryOutputCounter() const
    {
        return TheosRenderPipeline::Telemetry::ReadDxgiOutputCounter(innerSwapChain_,
            reinterpret_cast<std::uintptr_t>(innerSwapChain_), presentCount_,
            OrdinarySourceActive() && proxyActive_ && ordinaryPresentation_.Ready() &&
                SUCCEEDED(FailureResult()) && lastPresentResult_ == S_OK);
    }
    bool QueryXessJitter(std::uint64_t,float&,float&);
    TheosRenderPipeline::Telemetry::OutputCounter FsrOutputCounter()const;
    TheosRenderPipeline::SettingsActionStatus XessStatus()const;
    bool XessSharpeningAvailable()const;
    float XessAppliedSharpness()const;
    std::string XessSharpeningStatus()const;
    bool FsrTemporalActive() const { return FsrActive() && upscalerReady_ && lastFsrTemporal_ && SUCCEEDED(FailureResult()); }
    bool FsrFgActive() const { return StartupConfigured() && backendDecision_.presentation == TheosRenderPipeline::Upscaling::PresentationKind::Fsr; }
    bool FsrPresentSuspended()const;
    HRESULT UpdateFsrSuspension();
    HRESULT PresentFsrSource(UINT interval, UINT flags);
    HRESULT QueryFsrProducerDevice(REFIID iid, void** output) const;
    HRESULT ResizeFsrSwapChain(class GameSwapChain&, UINT count, UINT width, UINT height, DXGI_FORMAT format, UINT flags,
        const UINT* masks = nullptr, IUnknown* const* queues = nullptr);
    bool QueryFsrJitter(std::uint64_t sourceId,float& x,float& y);
    TheosRenderPipeline::SettingsActionStatus FsrStatus() const;
    TheosRenderPipeline::SettingsActionStatus FsrFgStatus() const;
    TheosRenderPipeline::Upscaling::FsrMlAvailability FsrMlChoices() const;
    bool StartupConfigured() const { return sourceUpscalerSettings_.Initialized(); }
    const TheosRenderPipeline::Upscaler::Configuration& SourceUpscalerSettings() const { return sourceUpscalerSettings_; }
    void RequestSourceUpscalerSettings(TheosRenderPipeline::Upscaler::Creation request);
    void SourceUpscalerSettingsSaved() { sourceUpscalerSettings_.Saved(); }
    void AdoptEffectiveSourceUpscalerSettings() const;
    UINT OutputWidth() const { return outputWidth_; }
    UINT OutputHeight() const { return outputHeight_; }
    UINT RenderWidth() const { return renderWidth_; }
    UINT RenderHeight() const { return renderHeight_; }
    HWND GameWindow() const { return outputWindow_; }
    ID3D11Texture2D* GameFacingTexture() const { return gameTargets_.GameFacing(); }
    ID3D11Texture2D* NativePresentationTexture() const { return presentation_.Texture(); }
    ID3D11RenderTargetView* NativePresentationRTV() const { return presentation_.RTV(); }
    ID3D11Texture2D* NativeUIRenderTexture() const { return nativeUI_.Dedicated() ? nativeUI_.RenderTexture() : presentation_.Texture(); }
    ID3D11RenderTargetView* NativeUIRenderRTV() const { return nativeUI_.Dedicated() ? nativeUI_.RenderRTV() : presentation_.RTV(); }
    ID3D11DepthStencilView* NativeUIDepthDSV() const { return presentation_.DepthDSV(); }
    bool DedicatedUITextureMode() const { return nativeUI_.Dedicated(); }
    bool NativeUIPassActive() const { return nativeUIPass_.Active(); }
    bool NativeUIInternalBind() const { return nativeUIPass_.InternalBind() || sourceUIInternal_ || startupOverlay_.Internal() || previewDraw_.Internal() || TheosRenderPipeline::ReShadeIntegration::Get().Internal(); }
    ID3D11DepthStencilView* NativeUIBackgroundDepth() const { return nativeUIPass_.BackgroundDepth(); }
    bool NativeUITargetBound() const { return nativeUIPass_.TargetBound(); }
    void SetNativeUITargetBound(bool a_bound) { nativeUIPass_.SetTargetBound(a_bound); }
    float OptimalMipmapBias() const;
    std::uint64_t EvaluationCount() const { return evaluationCount_; }
    std::int32_t WarmupPresentsRemaining() const { return warmupPresentsRemaining_; }
    std::uint64_t PresentCount() const { return presentCount_; }
    std::uint64_t FailedPresentCount() const { return failedPresentCount_; }
    HRESULT LastPresentResult() const { return lastPresentResult_; }
    std::uint64_t RuntimeStateObservationCount() const { return runtimeStateObservationCount_; }
    std::uint32_t RuntimeDLSSGStatus() const { return runtimeDLSSGStatus_; }
    std::uint32_t RuntimeFramesActuallyPresented() const { return runtimeFramesActuallyPresented_; }
    std::uint32_t RuntimeMinWidthOrHeight() const { return runtimeMinWidthOrHeight_; }
    std::uint32_t RuntimeMaxGeneratedFrames() const { return runtimeMaxGeneratedFrames_; }
    const std::string& Status() const { return status_; }
    bool SourceRecoveryActive() const { return sourceRecoveryActive_; }
#if !defined(TRP_NO_NEURAL_RENDERING)
    bool CommunityNeuralAvailable() const {return communityNeural_ && communityNeural_->Available();}
    bool CommunityNeuralTerminal() const {return communityNeural_ && communityNeural_->Terminal();}
    bool CommunityNeuralActive() const {return communityNeural_ && communityNeural_->Active();}
    uint64_t CommunityNeuralRecorded() const {return communityNeural_?communityNeural_->Recorded():0;}
    const std::string& CommunityNeuralStatus() const {return communityLastStatus_;}
#endif


  private:
    NvidiaHost() = default;
    struct LifecycleOperations;
#if !defined(TRP_NO_NEURAL_RENDERING)
    void InspectCommunityNeural();
    bool RetireCommunityNeural();
    bool EvaluateCommunityNeuralBefore(ID3D11Texture2D*,ID3D11Texture2D*,ID3D11Texture2D*,UINT,UINT,uint64_t,bool&,bool,TheosRenderPipeline::NeuralRendering::PreparedFsrInput* linearOutput=nullptr,const TheosRenderPipeline::Upscaling::UpscaleFrame* post=nullptr,TheosRenderPipeline::Upscaling::UpscaleOutcome outcome=TheosRenderPipeline::Upscaling::UpscaleOutcome::Temporal,const TheosRenderPipeline::Upscaling::CameraMeasurements* camera=nullptr);
    bool EvaluateCommunityNeuralAfter(TheosRenderPipeline::Upscaling::UpscaleFrame&,TheosRenderPipeline::Upscaling::UpscaleOutcome,bool);
    std::unique_ptr<TheosRenderPipeline::NeuralRendering::BeforeHost> communityNeural_;
    TheosRenderPipeline::NeuralRendering::SettingsSnapshot communitySnapshot_;
    uint64_t communityEpoch_{1};
    TheosRenderPipeline::NeuralRendering::SourceCameraHistory communityCameraHistory_;
    bool communitySnapshotValid_{};
    std::string communityLastStatus_;
    TheosRenderPipeline::NeuralRendering::FsrRouteDiagnostics communityFsrRouteDiagnostics_;
#endif

    void ResetSessionAfterRetirement();

    bool CreateGameFacingResources(IDXGISwapChain* a_swapChain);
    bool InitializeSourceUpscaler(const D3D11_TEXTURE2D_DESC& a_outputDesc);
    bool CreateNativeUIExtractionResources(const D3D11_TEXTURE2D_DESC& a_outputDesc);
    bool ExtractNativeUIColorAndAlpha();
    bool CaptureAndComposeDedicatedNativeUI();
    bool PresentationBackendReadyForEvaluation();
    void UpdateRuntimeDLSSGState(std::uint32_t a_status, std::uint32_t a_framesActuallyPresented, std::uint32_t a_minWidthOrHeight,
                                 std::uint32_t a_maxGeneratedFrames);
    bool PrepareNativeUITarget(UINT a_bufferIndex);
    bool PrepareSourceNativeUITargets();
    struct SourceFrameOperations;
    struct SourceNvidiaEvaluationOperations;
    bool EvaluateSourceNvidiaFrame(bool nativeUIHandoff, bool resetHistory);
    TheosRenderPipeline::Upscaling::Result<void> PrepareExternalGeneration(const TheosRenderPipeline::Upscaling::UpscaleFrame&, TheosRenderPipeline::Upscaling::UpscaleOutcome);
    TheosRenderPipeline::Upscaling::Result<void> QuiesceActivePresentation();
    TheosRenderPipeline::Upscaling::Result<void> ResumeActivePresentation();
    bool EvaluateFsrFrame(IDXGISwapChain*,bool nativeUIHandoff);
    struct SourceFsrEvaluationOperations;
    bool EvaluateXessFrame(IDXGISwapChain*,bool);
    struct SourceXessEvaluationOperations;
    bool FinishSourceFrameForPresent();
    void ApplyLoadingFade(bool composed);
    void EndNativeUIPass();
    void ReleaseSourceUpscaler(bool retainFsrDevice=false);
    void ArmFrameGenerationWarmup();
    void SetRuntimeEnabled(bool a_enabled);
    void ApplySourceUpscalerSettingsAfterPresent();
    TheosRenderPipeline::Upscaler::Configuration sourceUpscalerSettings_;
    TheosRenderPipeline::Upscaling::BackendDecision backendDecision_;
    TheosRenderPipeline::OrdinaryPresentation ordinaryPresentation_;
#if defined(TRP_ENABLE_XESS)
    std::unique_ptr<TheosRenderPipeline::Upscaling::XessHostResources> xessResources_;
    TheosRenderPipeline::Upscaling::SdrColorConverter xessEncode_;
    bool xessRecovery_{},lastXessTemporal_{};
    unsigned xessDeferredReported_{};
    std::string xessRecoveryReason_;
    std::string xessStartupFallbackReason_;
    std::uint64_t xessEpoch_{1};
    // UpscaleOutput holds the completed HUD-less image until a new evaluation
    // writes it. Reuse is invalidated on spatial recovery, reset and retirement.
    TheosRenderPipeline::Upscaling::UpscaleFrame xessCompletedFrame_{};
    bool xessHasCompleted_{};
    std::uint64_t xessDuplicateCount_{},xessForeignThreadCount_{},xessRepeatedCount_{};
#endif
#if defined(TRP_ENABLE_FSR)
    std::shared_ptr<TheosRenderPipeline::Upscaling::FsrHostResources> fsrResources_;
    std::unique_ptr<TheosRenderPipeline::Upscaling::FsrFrameAdapter> fsrFrame_;
    bool lastFsrTemporal_{};
    // Set only after ordinary resize has retired the sized resources. Keep it
    // through reconstruction failures until complete startup or full teardown.
    bool fsrSizingRetainedForResize_{};
    unsigned fsrTransitionLogs_{};
#endif
#if defined(TRP_ENABLE_FSR_FG)
    HRESULT CreateFsrPresenter(IDXGIFactory*, ID3D11Device*, const DXGI_SWAP_CHAIN_DESC&, IDXGISwapChain**);
    std::unique_ptr<TheosRenderPipeline::FsrHostPresentation> fsrPresentation_;
    std::uint64_t fsrSourceRenderedCount_{},fsrGuideCaptureCount_{};
    Microsoft::WRL::ComPtr<IDXGIFactory> fsrFactory_;
    DXGI_SWAP_CHAIN_DESC fsrDescriptor_{};
    UINT fsrGameBufferCount_{};
    TheosRenderPipeline::Upscaling::UpscaleFrame fsrGenerationFrame_{};
    TheosRenderPipeline::Upscaling::UpscaleOutcome fsrGenerationOutcome_{TheosRenderPipeline::Upscaling::UpscaleOutcome::SkippedInvalidInput};
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fsrForeground_;
    bool fsrSourcePending_{}, fsrUiComplete_{}, fsrMenu_{};
    unsigned fsrGenerationTransitionLogs_{};
#endif
    TheosRenderPipeline::SourceHostFailure lifecycleFailure_;

    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    TheosRenderPipeline::NativeUIContexts nativeUIContexts_;
    TheosRenderPipeline::GameFacingTargets gameTargets_;
    TheosRenderPipeline::CommunityShaderAdapter communityFrame_;
    TheosRenderPipeline::NativeUIComposition nativeUI_;
    TheosRenderPipeline::PresentationTargets presentation_;
    TheosRenderPipeline::NativeUIPass nativeUIPass_;
    TheosRenderPipeline::SourceFrameCoordinator sourceFrameCoordinator_{nativeUIPass_};
    TheosRenderPipeline::StartupOverlayPass startupOverlay_;
    TheosRenderPipeline::InventoryPreviewDraw previewDraw_;
    TheosRenderPipeline::LoadingScreenUpscaler loadingScreenUpscaler_;
    TheosRenderPipeline::LoadingScreenRoute loadingScreenRoute_;
    TheosRenderPipeline::LoadingFadeIn loadingFade_;
    TheosRenderPipeline::PresentationFade presentationFade_;
    HRESULT loadingScreenResult_{S_OK};
    bool loadingScreenLogged_{};
    std::uint64_t startupWorldFrame_{~std::uint64_t{}};
    DWORD sourceRenderThread_{};
    TheosRenderPipeline::NativeUIAttachments nativeUIAttachments_;
    bool sourceUIInternal_{};
    std::uint32_t sourceUIBufferIndex_{};
    IDXGISwapChain* innerSwapChain_{nullptr};
    IDXGISwapChain* outerSwapChain_{nullptr};
    HWND outputWindow_{nullptr};
    UINT outputWidth_{0};
    UINT outputHeight_{0};
    UINT renderWidth_{0};
    UINT renderHeight_{0};
    bool proxyActive_{false};
    bool sourceUpscalerInitializationPending_{false};
    // Retained feature ownership can outlive a terminal failure. Runtime callers
    // must use UpscalerReady(), which also checks the latched lifecycle error.
    bool upscalerReady_{false};
    bool splitSourceDLSSActive_{false};
    bool splitSourceRuntimeFailureLogged_{false};
    bool frameGenerationStateKnown_{false};
    bool frameGenerationEnabled_{false};
    bool resetNextEvaluation_{true};
    bool sourceRecoveryActive_{};
    std::uint64_t sourceRecoveryFailures_{};
    bool evaluationFailureLogged_{false};
    bool nativeUIExtractionFailureLogged_{false};
    std::int32_t warmupPresentsRemaining_{0};
    std::uint64_t upscaleEvaluationCount_{0};
    std::uint64_t evaluationCount_{0};
    std::uint64_t presentCount_{0};
    std::uint64_t failedPresentCount_{0};
    HRESULT lastPresentResult_{S_OK};
    std::uint64_t runtimeStateObservationCount_{0};
    std::uint32_t runtimeDLSSGStatus_{0};
    std::uint32_t runtimeFramesActuallyPresented_{0};
    std::uint32_t runtimeMinWidthOrHeight_{0};
    std::uint32_t runtimeMaxGeneratedFrames_{0};
    std::string status_{"not requested"};
};
