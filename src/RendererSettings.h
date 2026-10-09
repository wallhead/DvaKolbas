#pragma once

#include "TextureProviderBridge.h"
#include "PublicIniSchema.h"
#include "UpscaleType.h"
#include "RendererGpuPolicy.h"
#include "NeuralRenderingMode.h"
#include "NeuralRendering/BeforeSettings.h"
#include "FrameGen/SourceDLSSGSettings.h"
#include "FrameGen/GenerationBackendPreference.h"
#include "WeatherAppearance.h"
#include "Upscaling/FSRSettings.h"
#include "Upscaling/XessSettings.h"
#include <array>
#include <cmath>
#include <utility>
#include <string>

namespace TheosRenderPipeline
{
struct RendererSettingsDraft
{
    bool valid{false};
    int upscaleType{DLSS};
    int qualityLevel{2};
    int dlssPreset{11};
    bool autoExposure{true};
    bool sharpening{true};
    float sharpness{0.672f};
    Upscaling::FsrSettings fsr;
    Upscaling::XessSettings xess;
    bool generationEnabled{true}, dynamicResolution{};
    long generationBackend{1};
    GenerationBackendPreference generationBackendPreference{};
    bool enableJitter{true};
    bool nativeUI{true};
    bool requestLoadingArtwork{true};
    bool reShadeBeforeUpscaling{false};
    bool lateOverlayBridge{true};
    bool enableGPUTimings{true};
    bool enableFrameTrace{false};
    bool directRCASOutput{false};
    bool directDLSSOutput{false};
    TheosRenderPipeline::SourceDLSSG::Preferences sourceDLSSG;
    Appearance::Settings appearance;
    bool textureProviderConnected{false};
    TextureProviderBridge::Settings textureProviderSettings{};
    // Menu-only memory: retain each provider's choices across automatic applies.
    struct ModePreferences {
        bool captured{}, generationEnabled{}, neuralEnabled{}, hdrEnabled{}, dynamicResolution{};
        long generationBackend{};
        bool nativeScale{};
    } nvidiaMode, fsrMode, xessMode;
};

inline void SetRendererUpscaleMode(RendererSettingsDraft& draft, int mode)
{
    if (mode == draft.upscaleType) return;
    auto& previous = draft.upscaleType == Xess ? draft.xessMode : draft.upscaleType == FSR ? draft.fsrMode : draft.nvidiaMode;
    previous = {true, draft.generationEnabled, draft.sourceDLSSG.neuralEnabled,
                draft.sourceDLSSG.hdrOutput.enabled, draft.dynamicResolution, draft.generationBackend,
                draft.upscaleType == DLAA};
    draft.upscaleType = mode;
    const auto& next = mode == Xess ? draft.xessMode : mode == FSR ? draft.fsrMode : draft.nvidiaMode;
    if (next.captured) {
        draft.generationBackend = next.generationBackend;
        draft.generationEnabled = next.generationEnabled;
        draft.sourceDLSSG.neuralEnabled = next.neuralEnabled;
        draft.sourceDLSSG.hdrOutput.enabled = next.hdrEnabled;
        draft.dynamicResolution = next.dynamicResolution;
    } else if (mode == FSR) {
        draft.generationBackend = 2;
        draft.generationEnabled = false;
        draft.sourceDLSSG.neuralEnabled = false;
        draft.sourceDLSSG.hdrOutput.enabled = false;
        draft.dynamicResolution = false;
    } else {
        draft.generationBackend = 1;
    }
    if(mode==Xess){draft.generationBackend=0;draft.generationEnabled=false;draft.sourceDLSSG.neuralEnabled=false;draft.sourceDLSSG.hdrOutput.enabled=false;draft.dynamicResolution=false;draft.enableJitter=true;}
    if(mode==FSR && draft.generationBackendPreference==GenerationBackendPreference::Nvidia)
        draft.generationBackendPreference=GenerationBackendPreference::Auto;
    if (draft.generationBackend != 0)
    {
        draft.generationBackend = ResolveGenerationBackend(draft.generationBackendPreference,
            mode == FSR ? Upscaling::BackendKind::Fsr : Upscaling::BackendKind::Dlss);
    }
}

inline void SetRendererUpscaleProvider(RendererSettingsDraft& draft, bool fsr)
{
    if ((draft.upscaleType == FSR) == fsr) return;
    SetRendererUpscaleMode(draft, fsr ? FSR : draft.nvidiaMode.nativeScale ? DLAA : DLSS);
}

// -1 is the menu's Native choice. Persist the existing DLAA ID for INI compatibility.
inline void SetNvidiaRenderScale(RendererSettingsDraft& draft, int quality)
{
    SetRendererUpscaleMode(draft, quality == -1 ? DLAA : DLSS);
    if (quality >= 0 && quality <= 4) draft.qualityLevel = quality;
}

inline void RefreshAppliedRendererSettingsDraft(RendererSettingsDraft& draft, RendererSettingsDraft current)
{
    current.nvidiaMode = draft.nvidiaMode;
    current.fsrMode = draft.fsrMode;
    current.xessMode = draft.xessMode;
    draft = std::move(current);
}

inline int CountRendererSettingsChanges(const RendererSettingsDraft& draft, const RendererSettingsDraft& current)
{
    if (!draft.valid)
    {
        return 0;
    }
    constexpr std::array flags{&RendererSettingsDraft::autoExposure,     &RendererSettingsDraft::sharpening,
                               &RendererSettingsDraft::enableJitter,    &RendererSettingsDraft::nativeUI,
                               &RendererSettingsDraft::lateOverlayBridge, &RendererSettingsDraft::enableGPUTimings,
                               &RendererSettingsDraft::enableFrameTrace, &RendererSettingsDraft::directRCASOutput,
                               &RendererSettingsDraft::directDLSSOutput, &RendererSettingsDraft::requestLoadingArtwork,
                               &RendererSettingsDraft::reShadeBeforeUpscaling};
    constexpr std::array choices{&RendererSettingsDraft::upscaleType, &RendererSettingsDraft::qualityLevel,
                                 &RendererSettingsDraft::dlssPreset};
    int count = 0;
    for (auto field : flags)
    {
        count += draft.*field != current.*field;
    }
    for (auto field : choices)
    {
        count += draft.*field != current.*field;
    }
    count += std::abs(draft.sharpness - current.sharpness) > 0.0001f;
    count += draft.fsr != current.fsr;
    count += draft.xess != current.xess;
    count += draft.generationEnabled != current.generationEnabled;
    count += draft.generationBackend != current.generationBackend;
    count += draft.generationBackendPreference != current.generationBackendPreference;
    count += draft.dynamicResolution != current.dynamicResolution;
    count += draft.sourceDLSSG != current.sourceDLSSG;
    count += draft.appearance != current.appearance;
    if (draft.textureProviderConnected && current.textureProviderConnected)
    {
        count += draft.textureProviderSettings.enabled != current.textureProviderSettings.enabled;
        for (std::size_t i = 0; i < current.textureProviderSettings.maxSize.size(); ++i)
        {
            count += draft.textureProviderSettings.maxSize[i] != current.textureProviderSettings.maxSize[i];
        }
    }
    return count;
}

template<class Apply>
inline bool ApplyRendererSettingsEdits(const RendererSettingsDraft& before,
                                      const RendererSettingsDraft& after, Apply&& apply)
{
    // Compare this UI frame's edits, not differences from active settings. A rejected
    // or restart-only request must not be submitted again on passive menu frames.
    if (!before.valid || !after.valid ||
        (CountRendererSettingsChanges(after, before) == 0 && after.sharpness == before.sharpness)) return false;
    apply();
    return true;
}

struct RendererSettingsCapabilities
{
    bool sourceHost{}, neuralRuntime{}, dedicatedUI{}, externalWorld{};
    bool neuralOperational{true};
    bool fsrBuilt{};
    bool fsrFgBuilt{};
    bool fsrFgPresenter{};
    bool communityNeural{};
    std::uint32_t adapterVendorId{};
    bool fsrOnlyRenderer{};
};

template<class Generation>
inline void SetLiveGenerationRequest(RendererSettingsDraft& draft, Generation& generation, bool enabled, long actualBackend)
{
    // A live checkbox addresses the current owner; an ordinary next-launch
    // presenter must still retain a valid off preference.
    draft.generationEnabled=enabled && draft.generationBackend!=0;
    if(actualBackend==1 && draft.nvidiaMode.captured) draft.nvidiaMode.generationEnabled=enabled;
    if(actualBackend==2 && draft.fsrMode.captured && draft.fsrMode.generationBackend==2)
        draft.fsrMode.generationEnabled=enabled;
    generation.RequestRuntimeInterpolation(enabled);
    if(generation.settings.generationBackend==0) generation.settings.enabled=false;
}
template<class Generation>
inline void ApplyRendererGeneration(const RendererSettingsDraft& draft, Generation& generation, long actualBackend)
{
    generation.settings.generationBackend=draft.generationBackend;
    generation.settings.generationBackendPreference=draft.generationBackendPreference;
    generation.settings.enabled=draft.generationEnabled;
    if(draft.generationBackend==actualBackend) generation.RequestRuntimeInterpolation(draft.generationEnabled);
}

inline bool CanEditNeuralEnabled(bool enabled, bool available) { return enabled || available; }

inline bool SameNeuralPreferences(const SourceDLSSG::Preferences& a, const SourceDLSSG::Preferences& b)
{
    return a.neuralEnabled == b.neuralEnabled && a.neuralBeforeUpscaling == b.neuralBeforeUpscaling &&
        a.neuralPasses == b.neuralPasses && a.neuralCombat == b.neuralCombat && a.neuralTuning == b.neuralTuning &&
        a.neuralReconstruction == b.neuralReconstruction && a.neuralSecondPass == b.neuralSecondPass &&
        a.neuralThirdPass == b.neuralThirdPass;
}

inline const char* NeuralSettingsUnavailable(int mode, RendererSettingsCapabilities capabilities)
{
    if (!capabilities.neuralRuntime) {
        return "NR runtime DLL not found. Install nvngx_dlssnr.dll at the configured path and restart Skyrim.";
    }
    if (!capabilities.neuralOperational) { return "NR is unavailable after a runtime failure; turn NR off or restart Skyrim."; }
    if (!SupportsNeuralRenderingMode(mode, capabilities.externalWorld, capabilities.communityNeural) ||
        (!capabilities.externalWorld && !capabilities.dedicatedUI)) {
        return "NR requires dedicated UI composition. Turn NR off to apply other changes, or set [Interface] UIComposition=Dedicated in the INI and restart Skyrim.";
    }
    return nullptr;
}

inline const char* ValidateRendererSettings(const RendererSettingsDraft& draft,
                                            RendererSettingsCapabilities capabilities,
                                            const RendererSettingsDraft* current = nullptr)
{
    if (!capabilities.sourceHost)
    {
        return "Presentation host is unavailable; settings were not applied.";
    }
    if ((IsAmdRenderer(capabilities.adapterVendorId) || capabilities.fsrOnlyRenderer) &&
        !FsrOnlyRendererSelectionAllowed(draft.upscaleType, draft.generationBackend, draft.sourceDLSSG.neuralEnabled)) {
        return "This GPU supports only FSR upscaling and optional FSR frame generation; DLSS, DLAA and NR are unavailable.";
    }
    if (draft.upscaleType==FSR && capabilities.fsrOnlyRenderer && capabilities.adapterVendorId==0x10de &&
        (draft.fsr.providerPolicy!=Upscaling::ProviderPolicy::Analytical ||
         draft.fsr.generationProviderPolicy!=Upscaling::ProviderPolicy::Analytical))
        return "Non-RTX NVIDIA uses FSR3 upscaling and FSR3 frame generation; ML providers are unavailable.";
    if (capabilities.fsrFgPresenter && !draft.nativeUI) {
        return "Native UI must stay enabled while the FSR presenter is active. Restart with the new presenter before disabling it.";
    }
    if (draft.upscaleType != DLSS && draft.upscaleType != DLAA && draft.upscaleType != FSR && draft.upscaleType != Xess)
    {
        return "Choose DLSS or FSR; DLSS Quality=Native selects DLAA.";
    }
    if(draft.upscaleType==Xess) {
        if(!XessBuilt)return "XeSS is not included in this build.";
        if(!Upscaling::ValidXessSettings(draft.xess))return "XeSS quality/source encoding is invalid.";
        if(draft.generationBackend!=0 || draft.generationEnabled || draft.sourceDLSSG.neuralEnabled)return "This XeSS SR trial requires NR and frame generation off.";
        if(draft.dynamicResolution || draft.sourceDLSSG.hdrOutput.enabled)return "XeSS SR currently requires fixed dimensions and SDR output.";
        if(!draft.enableJitter)return "XeSS requires camera jitter enabled.";
    } else if (draft.upscaleType == FSR) {
        if (!capabilities.fsrBuilt) return "FSR is not included in this build.";
        if (!Upscaling::ValidFsrSettings(draft.fsr)) return "FSR quality/provider/sharpness is invalid.";
        if (!Upscaling::IsKnownColorEncoding(draft.fsr.sourceColorEncoding)) return "FSR requires an explicit source color encoding: Linear, Gamma22 or SRGB. Check the source producer before choosing.";
        if(draft.generationBackend==0){if(draft.generationEnabled)return "Ordinary FSR presentation requires frame generation off.";}
        else if(draft.generationBackend==2){
            if(!capabilities.fsrFgBuilt)return "FSR frame generation is not included in this build.";
            if(!capabilities.dedicatedUI || !draft.nativeUI || capabilities.externalWorld)return "FSR frame generation requires TRP's dedicated native UI and source ownership.";
        } else if(draft.generationBackend==1) return "[FrameGeneration] Backend=NVIDIA requires [Upscaling] Upscaler=DLSS; use Auto or FSR.";
        else return "FSR requires its normal presenter, or the diagnostic [Upscaling Advanced] FsrOrdinaryPresenter=true with FG disabled.";
        if (draft.sourceDLSSG.neuralEnabled && !capabilities.communityNeural) return "Neural Rendering is unavailable with FSR.";
        if (draft.sourceDLSSG.hdrOutput.enabled) return "HDR output is unavailable with FSR.";
        if (draft.dynamicResolution) return "Dynamic resolution is unavailable with FSR.";
    } else if (draft.generationBackend==2) {
        if(!Upscaling::IsKnownColorEncoding(draft.fsr.sourceColorEncoding)) {
            static const std::string encodingMessage=std::string("FSR FG requires explicit SDR source encoding in ")+
                PublicIni::Reference("FSR","SourceColorEncoding")+" (Linear, Gamma22 or SRGB).";
            return encodingMessage.c_str();
        }
        if (!capabilities.fsrFgBuilt) return "FSR frame generation is not included in this build.";
        if (!capabilities.dedicatedUI || !draft.nativeUI || capabilities.externalWorld)
            return "FSR frame generation requires dedicated native UI and source ownership.";
        if (draft.sourceDLSSG.hdrOutput.enabled) return "FSR frame generation requires SDR output; disable HDR and restart.";
        if (draft.dynamicResolution) return "FSR frame generation requires fixed render dimensions; disable dynamic resolution and restart.";
        if (draft.sourceDLSSG.neuralEnabled && !capabilities.communityNeural)
            return "FSR presentation requires the community NR runtime.";
    } else if (draft.generationBackend!=1) return "DLSS/DLAA require NVIDIA or FSR presentation.";
    if (!Appearance::ValidHours(draft.appearance.hours)) {
        return "Preset times must increase from Night to Dusk and stay between 0 and 24 hours.";
    }
    if (!TheosRenderPipeline::SourceDLSSG::ValidGenerationRequest(draft.sourceDLSSG.generation))
    {
        return "Dynamic target output FPS must be 0 or between 61 and 1000.";
    }
#if !defined(TRP_NO_NEURAL_RENDERING)
    if (draft.sourceDLSSG.neuralEnabled)
    {
        if (draft.sourceDLSSG.neuralPasses < 1 || draft.sourceDLSSG.neuralPasses > 3) {
            return "Choose one, two or three NR passes. The legacy runtime executes at most two.";
        }
        if (capabilities.communityNeural) {
            if (capabilities.externalWorld || !draft.nativeUI) return "Community NR requires TRP world ownership and native UI.";
            if (const auto error=NeuralRendering::NativeBeforeUnavailable(draft.sourceDLSSG)) return error;
            if (const auto error=NeuralRendering::NativeAfterUnavailable(draft.sourceDLSSG,draft.upscaleType,draft.fsr.quality,draft.dynamicResolution)) return error;
        }
        if (const auto error = NeuralSettingsUnavailable(draft.upscaleType, capabilities)) {
            // Preserve an unchanged startup request when saving unrelated edits.
            // Apply separately gates execution, so this cannot restart failed NR.
            if (!current || !SameNeuralPreferences(draft.sourceDLSSG, current->sourceDLSSG)) { return error; }
        }
    }
#endif
    return nullptr;
}
} // namespace TheosRenderPipeline
