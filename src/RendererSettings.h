#pragma once

#include "TextureProviderBridge.h"
#include "UpscaleType.h"
#include "NeuralRenderingMode.h"
#include "NeuralRendering/BeforeSettings.h"
#include "FrameGen/SourceDLSSGSettings.h"
#include "WeatherAppearance.h"
#include "Upscaling/FSRSettings.h"
#include <array>
#include <cmath>

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
    bool generationEnabled{true}, dynamicResolution{};
    long generationBackend{1};
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
    // Menu-only memory: mode changes are reversible until Apply/Discard.
    struct ModePreferences {
        bool captured{}, generationEnabled{}, neuralEnabled{}, hdrEnabled{}, dynamicResolution{};
        long generationBackend{};
    } nvidiaMode, fsrMode;
};

inline void SetRendererUpscaleMode(RendererSettingsDraft& draft, int mode)
{
    if (mode == draft.upscaleType) return;
    auto& previous = draft.upscaleType == FSR ? draft.fsrMode : draft.nvidiaMode;
    previous = {true, draft.generationEnabled, draft.sourceDLSSG.neuralEnabled,
                draft.sourceDLSSG.hdrOutput.enabled, draft.dynamicResolution, draft.generationBackend};
    draft.upscaleType = mode;
    const auto& next = mode == FSR ? draft.fsrMode : draft.nvidiaMode;
    if (next.captured) {
        draft.generationBackend = next.generationBackend;
        draft.generationEnabled = next.generationEnabled;
        draft.sourceDLSSG.neuralEnabled = next.neuralEnabled;
        draft.sourceDLSSG.hdrOutput.enabled = next.hdrEnabled;
        draft.dynamicResolution = next.dynamicResolution;
    } else if (mode == FSR) {
        draft.generationBackend = 0;
        draft.generationEnabled = false;
        draft.sourceDLSSG.neuralEnabled = false;
        draft.sourceDLSSG.hdrOutput.enabled = false;
        draft.dynamicResolution = false;
    } else {
        draft.generationBackend = 1;
    }
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
    count += draft.generationEnabled != current.generationEnabled;
    count += draft.generationBackend != current.generationBackend;
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

struct RendererSettingsCapabilities
{
    bool sourceHost{}, neuralRuntime{}, dedicatedUI{}, externalWorld{};
    bool neuralOperational{true};
    bool fsrBuilt{};
    bool fsrFgBuilt{};
    bool fsrFgPresenter{};
    bool communityNeural{};
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
    generation.settings.enabled=draft.generationEnabled;
    if(draft.generationBackend==actualBackend) generation.RequestRuntimeInterpolation(draft.generationEnabled);
}

inline bool CanEditNeuralEnabled(bool enabled, bool available) { return enabled || available; }

inline bool SameNeuralPreferences(const SourceDLSSG::Preferences& a, const SourceDLSSG::Preferences& b)
{
    return a.neuralEnabled == b.neuralEnabled && a.neuralBeforeUpscaling == b.neuralBeforeUpscaling &&
        a.neuralPasses == b.neuralPasses && a.neuralCombat == b.neuralCombat && a.neuralTuning == b.neuralTuning &&
        a.neuralReconstruction == b.neuralReconstruction && a.neuralSecondPass == b.neuralSecondPass;
}

inline const char* NeuralSettingsUnavailable(int mode, RendererSettingsCapabilities capabilities)
{
    if (!capabilities.neuralRuntime) {
        return "NR runtime DLL not found. Install nvngx_dlssnr.dll at the configured path and restart Skyrim.";
    }
    if (!capabilities.neuralOperational) { return "NR is unavailable after a runtime failure; turn NR off or restart Skyrim."; }
    if (!SupportsNeuralRenderingMode(mode, capabilities.externalWorld, capabilities.communityNeural) ||
        (!capabilities.externalWorld && !capabilities.dedicatedUI)) {
        return "NR requires dedicated UI composition. Turn NR off to apply other changes, or set NativeUICompositionMode=0 in the INI and restart Skyrim.";
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
    if (capabilities.fsrFgPresenter && !draft.nativeUI) {
        return "Native UI must stay enabled while the AMD FSR presenter is active. Restart with the new presenter before disabling it.";
    }
    if (draft.upscaleType != DLSS && draft.upscaleType != DLAA && draft.upscaleType != FSR)
    {
        return "Choose DLSS, DLAA or FSR.";
    }
    if (draft.upscaleType == FSR) {
        if (!capabilities.fsrBuilt) return "FSR is not included in this build.";
        if (!Upscaling::ValidFsrSettings(draft.fsr)) return "FSR quality/provider/sharpness is invalid.";
        if (!Upscaling::IsKnownColorEncoding(draft.fsr.sourceColorEncoding)) return "FSR requires an explicit source color encoding: Linear, Gamma22 or SRGB. Check the source producer before choosing.";
        if(draft.generationBackend==0){if(draft.generationEnabled)return "Ordinary FSR presentation requires frame generation off.";}
        else if(draft.generationBackend==2){
            if(!capabilities.fsrFgBuilt)return "FSR frame generation is not included in this build.";
            if(draft.fsr.providerPolicy!=Upscaling::ProviderPolicy::Analytical)return "FSR frame generation requires the analytical provider.";
            if(!capabilities.dedicatedUI || !draft.nativeUI || capabilities.externalWorld)return "FSR frame generation requires TRP's dedicated native UI and source ownership.";
        } else return "FSR requires ordinary presentation (backend 0) or FSR frame generation (backend 2).";
        if (draft.sourceDLSSG.neuralEnabled && !capabilities.communityNeural) return "Neural Rendering is unavailable with FSR.";
        if (draft.sourceDLSSG.hdrOutput.enabled) return "HDR output is unavailable with FSR.";
        if (draft.dynamicResolution) return "Dynamic resolution is unavailable with FSR.";
    } else if (draft.generationBackend!=1) return "DLSS/DLAA require the NVIDIA presentation backend; choose backend 1.";
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
