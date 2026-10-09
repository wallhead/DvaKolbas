#include <PCH.h>
#include "RendererSettingsController.h"
#include "RendererSettingsEdits.h"
#include "RenderPipeline.h"
#include "FrameGen/SourceFrameGeneration.h"
#include "FrameGen/NvidiaHost.h"
#include "FrameGen/SourceDLSSGBackend.h"
#include "PerformanceTuning.h"
#include "CommunityShaderIntegration.h"
#include "WeatherAppearanceRuntime.h"

namespace TheosRenderPipeline
{
RendererSettingsController RendererSettingsController::Current()
{
    return {*RenderPipeline::GetSingleton(), *SourceFrameGeneration::GetSingleton(), *NvidiaHost::GetSingleton(),
            *PerformanceTuning::GetSingleton(), *TextureProviderBridge::GetSingleton()};
}

RendererSettingsDraft RendererSettingsController::Capture([[maybe_unused]] bool nrRuntimePresent, bool readTextures) const
{
    RendererSettingsDraft settingsDraft;
    settingsDraft.valid = true;
    settingsDraft.upscaleType = upscaler_.mUpscaleType;
    settingsDraft.nvidiaMode.nativeScale = upscaler_.mDlssNativeScale;
    settingsDraft.fsr = upscaler_.mFsrSettings;
    settingsDraft.xess = upscaler_.mXessSettings;
    settingsDraft.generationEnabled = frameGen_.settings.enabled;
    settingsDraft.generationBackend = frameGen_.settings.generationBackend;
    settingsDraft.generationBackendPreference = frameGen_.settings.generationBackendPreference;
    settingsDraft.dynamicResolution = upscaler_.mDynamicResolutionRequested;
    settingsDraft.qualityLevel = upscaler_.mQualityLevel;
    settingsDraft.dlssPreset = upscaler_.mDLSSPreset;
    settingsDraft.autoExposure = upscaler_.mAutoExposure;
    settingsDraft.sharpening = upscaler_.mSharpening;
    settingsDraft.sharpness = upscaler_.mSharpness;
    settingsDraft.enableJitter = upscaler_.mEnableJitter;
    settingsDraft.nativeUI = upscaler_.mNativeUI;
    settingsDraft.reShadeBeforeUpscaling = upscaler_.mReShadeBeforeUpscaling;
    settingsDraft.requestLoadingArtwork = upscaler_.mRequestLoadingArtwork.load(std::memory_order_relaxed);
    if (host_.StartupConfigured())
    {
        const auto& requested = host_.SourceUpscalerSettings().Requested();
        settingsDraft.upscaleType = requested.mode;
        settingsDraft.fsr = requested.fsr;
        settingsDraft.xess = requested.xess;
        settingsDraft.qualityLevel = requested.quality;
        settingsDraft.dlssPreset = requested.preset;
        settingsDraft.sharpening = requested.sharpening;
        settingsDraft.autoExposure = requested.autoExposure;
    }
    settingsDraft.lateOverlayBridge = upscaler_.mWheelerLateOverlayBridge;
    const auto& performanceSettings = performance_.settings;
    settingsDraft.enableGPUTimings = performanceSettings.enableGPUTimings;
    settingsDraft.enableFrameTrace = performanceSettings.enableFrameTrace;
    settingsDraft.directRCASOutput = performanceSettings.directRCASOutput;
    settingsDraft.directDLSSOutput = performanceSettings.directDLSSOutput;
    settingsDraft.sourceDLSSG = frameGen_.settings.sourceDLSSG;
    settingsDraft.appearance = Appearance::Runtime::Get().Configuration();
#if !defined(TRP_NO_NEURAL_RENDERING)
    // A saved NR request must not strand unrelated settings behind disabled controls.
    if (!frameGen_.settings.neuralStartup.community) {
        if (host_.StartupConfigured() && !host_.OrdinarySourceActive() && !host_.FsrFgActive() && settingsDraft.upscaleType == FSR)
            settingsDraft.sourceDLSSG.neuralEnabled = SourceDLSSG::Backend::Get().NeuralConfiguration().enabled;
        settingsDraft.sourceDLSSG.neuralEnabled &= nrRuntimePresent;
    }
#endif
    settingsDraft.textureProviderConnected = readTextures && textures_.Read(settingsDraft.textureProviderSettings);
    return settingsDraft;
}

int RendererSettingsController::CountChanges(const RendererSettingsDraft& draft, bool nrRuntimePresent) const
{
    if (!draft.valid)
    {
        return 0;
    }
    auto current = Capture(nrRuntimePresent, draft.textureProviderConnected);
    if (host_.StartupConfigured())
    {
        const auto& requested = host_.SourceUpscalerSettings().Requested();
        current.upscaleType = requested.mode;
        current.qualityLevel = requested.quality;
        current.dlssPreset = requested.preset;
        current.autoExposure = requested.autoExposure;
        current.sharpening = requested.sharpening;
    }
    return CountRendererSettingsChanges(draft, current);
}

RendererSettingsResult RendererSettingsController::Apply(const RendererSettingsDraft& settingsDraft,
                                                         bool a_saveAsDefault, const Overlay::Layout* layout)
{
    return ApplyImpl(settingsDraft,a_saveAsDefault,layout,false);
}

RendererSettingsResult RendererSettingsController::ApplyLiveEdits(const RendererSettingsDraft& before,
                                                                 const RendererSettingsDraft& after)
{
    if(!before.valid || !after.valid)
        return RejectSettingsAction(false,"Settings draft is unavailable; reopen the menu.",
            [](const std::string& message){logger::error("{}",message);});
    auto current=Capture(true,after.textureProviderConnected);
    if(host_.StartupConfigured()){
        const auto requestedSharpness=current.fsr.sharpness;
        const auto& effective=host_.SourceUpscalerSettings().Effective();
        current.upscaleType=effective.mode;current.qualityLevel=effective.quality;current.fsr=effective.fsr;current.xess=effective.xess;
        // FSR sharpness remains a requested preference on a NVIDIA session.
        // Use effective allocation fields without losing that pending live value.
        current.fsr.sharpness=requestedSharpness;
    }
    current.generationBackend=host_.FsrFgActive()?2:host_.OrdinarySourceActive()?0:1;
    current.generationEnabled=frameGen_.RuntimeInterpolationRequested();
    if (host_.StartupConfigured() && !host_.OrdinarySourceActive() && !host_.FsrFgActive() && !frameGen_.settings.neuralStartup.community)
        current.sourceDLSSG.neuralEnabled = SourceDLSSG::Backend::Get().NeuralConfiguration().enabled;
    const auto live=ProjectRendererLiveEdits(before,after,current);
    if(CountRendererSettingsChanges(live,current)==0 && live.sharpness==current.sharpness)
        return {"Startup choices pending; Save as default and restart to activate them.",false,false};
    auto result=ApplyImpl(live,false,nullptr,true);
    if(result.applied&&!result.error)result.message="Live edit applied; startup selections require Save and restart.";
    return result;
}

RendererSettingsResult RendererSettingsController::ApplyImpl(const RendererSettingsDraft& settingsDraft,
    bool a_saveAsDefault,const Overlay::Layout* layout,bool liveOnly)
{
    if (!settingsDraft.valid)
    {
        return RejectSettingsAction(a_saveAsDefault, "Settings draft is unavailable; reopen the menu.",
            [](const std::string& message) { logger::error("{}", message); });
    }
    std::string actionMessage;
    bool actionMessageIsError = false;
    const bool sourceUpscaler = host_.StartupConfigured();
    RendererSettingsCapabilities capabilities{sourceUpscaler, false, host_.DedicatedUITextureMode(), CommunityShaders::Active()};
    capabilities.adapterVendorId = upscaler_.mAdapterVendorId;
    capabilities.fsrOnlyRenderer = upscaler_.mFsrOnlyRenderer;
#if !defined(TRP_NO_NEURAL_RENDERING)
    if (frameGen_.settings.neuralStartup.community) {
        capabilities.communityNeural=true;
        capabilities.neuralRuntime=host_.CommunityNeuralAvailable();
        capabilities.neuralOperational=!host_.CommunityNeuralTerminal();
    } else if (!host_.OrdinarySourceActive() && !host_.FsrFgActive()) {
    capabilities.neuralRuntime =
        TheosRenderPipeline::SourceDLSSG::NeuralRuntimePresent(frameGen_.settings.neuralRenderingRuntimePath);
    capabilities.neuralOperational = SourceDLSSG::Backend::Get().Ready() && !SourceDLSSG::Backend::Get().NeuralState().failed;
    }
#endif
#if defined(TRP_ENABLE_FSR)
    capabilities.fsrBuilt = true;
#endif
#if defined(TRP_ENABLE_FSR_FG)
    capabilities.fsrFgBuilt = true;
    capabilities.fsrFgPresenter = host_.FsrFgActive();
#endif
    const auto current = Capture(capabilities.neuralRuntime, false);
    if (const char* error = ValidateRendererSettings(settingsDraft, capabilities, &current))
    {
        return RejectSettingsAction(a_saveAsDefault, error,
            [](const std::string& message) { logger::error("{}", message); });
    }
    if (settingsDraft.textureProviderConnected &&
        !textures_.Apply(settingsDraft.textureProviderSettings, a_saveAsDefault))
    {
        return RejectSettingsAction(a_saveAsDefault, textures_.Status(),
            [](const std::string& message) { logger::error("{}", message); });
    }
    upscaler_.mUpscaleType = settingsDraft.upscaleType;
    upscaler_.mDlssNativeScale = settingsDraft.upscaleType == DLAA ||
        ((settingsDraft.upscaleType == FSR || settingsDraft.upscaleType == Xess) && settingsDraft.nvidiaMode.nativeScale);
    upscaler_.mFsrSettings = settingsDraft.fsr;
    upscaler_.mXessSettings = settingsDraft.xess;
    const long actualBackend=host_.FsrFgActive()?2:host_.OrdinarySourceActive()?0:1;
    if(!liveOnly)ApplyRendererGeneration(settingsDraft,frameGen_,actualBackend);
    else if(actualBackend!=0 && settingsDraft.generationEnabled!=frameGen_.RuntimeInterpolationRequested()){
        const auto pendingBackend=frameGen_.settings.generationBackend;
        const bool pendingEnabled=frameGen_.settings.enabled;
        frameGen_.RequestRuntimeInterpolation(settingsDraft.generationEnabled);
        if(pendingBackend!=actualBackend)frameGen_.settings.enabled=pendingEnabled;
    }
    upscaler_.mQualityLevel = std::clamp(settingsDraft.qualityLevel, 0, 4);
    upscaler_.mDLSSPreset = settingsDraft.dlssPreset;
    upscaler_.mAutoExposure = settingsDraft.autoExposure;
    upscaler_.mSharpening = settingsDraft.sharpening;
    upscaler_.mSharpness = std::clamp(settingsDraft.sharpness, 0.0f, 1.0f);
    upscaler_.mEnableJitter = settingsDraft.enableJitter;
    upscaler_.mNativeUI = settingsDraft.nativeUI;
    upscaler_.mReShadeBeforeUpscaling = settingsDraft.reShadeBeforeUpscaling;
    upscaler_.mRequestLoadingArtwork.store(settingsDraft.requestLoadingArtwork, std::memory_order_relaxed);
    upscaler_.mWheelerLateOverlayBridge = settingsDraft.lateOverlayBridge;
    // The host stages allocation changes and applies live changes after a completed Present.
    auto sourceRequest=TheosRenderPipeline::Upscaler::Creation{settingsDraft.upscaleType,settingsDraft.qualityLevel,
        settingsDraft.dlssPreset,settingsDraft.sharpening,settingsDraft.autoExposure,settingsDraft.fsr,settingsDraft.xess};
    if(liveOnly && host_.StartupConfigured()){
        const auto& pending=host_.SourceUpscalerSettings().Requested();
        sourceRequest.mode=pending.mode;sourceRequest.quality=pending.quality;sourceRequest.xess=pending.xess;
        sourceRequest.fsr.quality=pending.fsr.quality;sourceRequest.fsr.providerPolicy=pending.fsr.providerPolicy;
        sourceRequest.fsr.sourceColorEncoding=pending.fsr.sourceColorEncoding;
        sourceRequest.fsr.generationProviderPolicy=pending.fsr.generationProviderPolicy;
    }
    host_.RequestSourceUpscalerSettings(sourceRequest);
    auto performanceSettings = performance_.settings;
    performanceSettings.enableGPUTimings = settingsDraft.enableGPUTimings;
    performanceSettings.enableFrameTrace = settingsDraft.enableFrameTrace;
    performanceSettings.directRCASOutput = settingsDraft.directRCASOutput;
    performanceSettings.directDLSSOutput = settingsDraft.directDLSSOutput;
    performance_.ApplySettings(performanceSettings);
    const auto runtimePreferences = TheosRenderPipeline::SourceDLSSG::SanitizePreferences(settingsDraft.sourceDLSSG);
    const bool legacyNeuralOwner = host_.StartupConfigured() && !host_.OrdinarySourceActive() && !host_.FsrFgActive() && !frameGen_.settings.neuralStartup.community;
    const bool fsrPending = legacyNeuralOwner && host_.SourceUpscalerSettings().Requested().mode == FSR;
    const bool startupNeuralEnabled = frameGen_.settings.sourceDLSSG.neuralEnabled;
    frameGen_.settings.sourceDLSSG = runtimePreferences;
    if (liveOnly && fsrPending) frameGen_.settings.sourceDLSSG.neuralEnabled = startupNeuralEnabled;
    if (host_.StartupConfigured() && !host_.OrdinarySourceActive() && !host_.FsrFgActive())
    {
        auto& source = TheosRenderPipeline::SourceDLSSG::Backend::Get();
        source.ConfigureReflex(static_cast<sl::ReflexMode>(frameGen_.settings.sourceDLSSG.reflexMode));
        source.ConfigureUIRecomposition(frameGen_.settings.sourceDLSSG.uiRecomposition);
        source.ConfigureOutputFPSLimit(frameGen_.settings.sourceDLSSG.outputFPSLimit);
        source.ConfigureGeneration(frameGen_.settings.sourceDLSSG.generation);
        // Enabled is kept for saving; the backend applies it at the next swapchain creation.
        source.ConfigureHDROutput(frameGen_.settings.sourceDLSSG.hdrOutput);
        // Saving another provider stages its NR defaults without touching the
        // current legacy pass. Live edits still address the actual NVIDIA owner.
        if (!(legacyNeuralOwner && settingsDraft.upscaleType == FSR)) {
            TheosRenderPipeline::SourceDLSSG::NeuralOptions options;
            options.enabled = !frameGen_.settings.neuralStartup.community && runtimePreferences.neuralEnabled &&
                NeuralSettingsUnavailable(host_.SourceUpscalerSettings().Effective().mode, capabilities) == nullptr;
            options.runtimePath = frameGen_.settings.neuralRenderingRuntimePath;
            options.tuning = runtimePreferences.neuralTuning;
            options.reconstruction = runtimePreferences.neuralReconstruction;
            options.secondPass = runtimePreferences.neuralSecondPass;
            options.beforeUpscaling = runtimePreferences.neuralBeforeUpscaling;
            options.passes = runtimePreferences.neuralPasses;
            options.combat = runtimePreferences.neuralCombat;
            source.ConfigureNeuralRendering(std::move(options));
        }
    }
    Appearance::Runtime::Get().Configure(settingsDraft.appearance);
    if (a_saveAsDefault)
    {
        const bool saved = upscaler_.SaveINI(layout);
        actionMessage =
            saved ? "Startup defaults saved." : "Could not write RaZkolbaS.ini; settings remain active for this session.";
        actionMessageIsError = !saved;
    }
    else
    {
        actionMessage = "Session settings applied.";
        actionMessageIsError = false;
    }
    logger::info("[Overlay] source settings applied save={} mode={} quality={} preset={} requestedMode={} requestedQuality={} requestedPreset={}", a_saveAsDefault,
                 upscaler_.mUpscaleType, upscaler_.mQualityLevel, upscaler_.mDLSSPreset,
                 settingsDraft.upscaleType, settingsDraft.qualityLevel, settingsDraft.dlssPreset);
    logger::info("[LoadingArtwork] request setting={} save={}", settingsDraft.requestLoadingArtwork, a_saveAsDefault);
    if (sourceUpscaler && !actionMessageIsError)
    {
        const auto& configuration = host_.SourceUpscalerSettings();
        if (configuration.Failed())
        {
            actionMessage = "Source DLSS configuration failed; restart required.";
            actionMessageIsError = true;
        }
        else if (configuration.NeedsRestart() || settingsDraft.generationBackend!=actualBackend)
        {
            actionMessage =
                a_saveAsDefault
                    ? "Presenter/mode/quality saved for restart; live feature changes apply after this frame."
                    : "Presenter/mode/quality staged for restart; Save as default to keep them. Live feature changes are queued.";
        }
        else if (configuration.NeedsLiveChange())
        {
            actionMessage = a_saveAsDefault ? "Settings saved; feature update queued after this frame."
                                            : "Feature update queued after this frame; defaults unchanged.";
        }
        else
        {
            actionMessage = a_saveAsDefault ? "Settings saved." : "Session settings applied.";
        }
    }
    if (!actionMessageIsError && settingsDraft.sourceDLSSG.neuralEnabled &&
        NeuralSettingsUnavailable(settingsDraft.upscaleType, capabilities)) {
        actionMessage += " NR remains unavailable; its saved request is unchanged.";
    }
    return {std::move(actionMessage), actionMessageIsError, true};
}

RendererSettingsResult RendererSettingsController::SetNeuralRenderingEnabled(bool enabled)
{
    if (enabled && (upscaler_.mFsrOnlyRenderer || IsAmdRenderer(upscaler_.mAdapterVendorId)))
        return {"Neural Rendering requires a supported NVIDIA RTX GPU; use FSR and optional FSR frame generation.", true};
#if defined(TRP_NO_NEURAL_RENDERING)
    (void)enabled;
    return {"Neural Rendering is not included in this build.", true};
#else
    if (frameGen_.settings.neuralStartup.community) {
        if (enabled) {
            if (!host_.StartupConfigured() || !host_.CommunityNeuralAvailable() || host_.CommunityNeuralTerminal())
                return {"Community NR is unavailable; check its status and restart after correcting the runtime.",true};
            if (CommunityShaders::Active() || !host_.DedicatedUITextureMode() || !upscaler_.mNativeUI)
                return {"Community NR requires TRP world ownership and native UI.",true};
            if (const auto error=NeuralRendering::NativeBeforeUnavailable(frameGen_.settings.sourceDLSSG)) return {error,true};
            const auto request=Capture(true,false);
            if(const auto error=NeuralRendering::NativeAfterUnavailable(frameGen_.settings.sourceDLSSG,request.upscaleType,
                request.upscaleType==Xess?request.xess.quality:request.fsr.quality,request.dynamicResolution))return {error,true};
        }
        frameGen_.settings.sourceDLSSG.neuralEnabled=enabled;
        return {enabled?"NR requested for the next world frame.":"NR disabled.",false,true};
    }
    if (host_.FsrActive()) return {"Neural Rendering is unavailable with FSR.",true};
    std::string actionMessage;
    bool actionMessageIsError = false;

    if (host_.StartupConfigured())
    {
        auto& backend = TheosRenderPipeline::SourceDLSSG::Backend::Get();
        auto options = backend.NeuralConfiguration();
        options.runtimePath = frameGen_.settings.neuralRenderingRuntimePath;
        options.enabled = enabled;
        if (enabled && !TheosRenderPipeline::SourceDLSSG::NeuralRuntimePresent(options.runtimePath))
        {
            return {"NR runtime DLL not found. Install nvngx_dlssnr.dll at the configured path and restart Skyrim.",
                    true};
        }
        if (options.enabled && (!backend.Ready() ||
            !SupportsNeuralRenderingMode(host_.SourceUpscalerSettings().Effective().mode, CommunityShaders::Active()) ||
            (!CommunityShaders::Active() && !host_.DedicatedUITextureMode())))
        {
            actionMessage = "Source NR requires DLSS or DLAA, dedicated UI Texture mode, and a configured NR runtime.";
            actionMessageIsError = true;
            return {actionMessage, actionMessageIsError};
        }
        if (host_.SourceUpscalerSettings().Requested().mode != FSR && host_.SourceUpscalerSettings().Requested().mode != Xess)
            frameGen_.settings.sourceDLSSG.neuralEnabled = options.enabled;
        backend.ConfigureNeuralRendering(std::move(options));
        actionMessage =
            !enabled ? "Neural Rendering disabled." : "Source NR requested for the next eligible world frame.";
        actionMessageIsError = false;
        return {actionMessage, actionMessageIsError, true};
    }
    actionMessage = "Source NVIDIA host is unavailable.";
    actionMessageIsError = true;
    return {actionMessage, actionMessageIsError};
#endif
}
} // namespace TheosRenderPipeline
