#pragma once
#include "IniLayout.h"
#include "RendererGpuPolicy.h"

#include "NvidiaBaselinePolicy.h"
#include "Upscaling/UpscalerBackend.h"
#include <string_view>

namespace TheosRenderPipeline
{
    inline constexpr bool XessFgBuilt =
#if defined(TRP_ENABLE_XESS_FG)
        true;
#else
        false;
#endif
    inline constexpr bool XessBuilt =
#if defined(TRP_ENABLE_XESS)
        true;
#else
        false;
#endif
    inline Upscaling::BackendDecision ResolveBackend(const Upscaling::BackendConfiguration& config, bool fsrBuilt, bool fsrFgBuilt = false,bool xessFgBuilt = XessFgBuilt)
    {
        using namespace Upscaling;
        BackendDecision decision{config.backend, PresentationKind::Nvidia, false, config.generationEnabled, {}};
        if ((IsAmdRenderer(config.adapterVendorId) || config.fsrOnlyRenderer) &&
            !FsrOnlyRendererSelectionAllowed(config.backend == BackendKind::Xess ? ::Xess : config.backend == BackendKind::Fsr ? ::FSR : ::DLSS,
                config.generationBackend, config.neuralRendering)) {
            decision.diagnostic = "This GPU supports only FSR upscaling and optional FSR frame generation; DLSS, DLAA and NR are unavailable.";
        }
        else if (!config.enabled) { decision.diagnostic = "This renderer requires an enabled temporal upscaler."; }
        else if(config.generationBackend==3) {
            decision.presentation=PresentationKind::Xess;
            if(!xessFgBuilt)decision.diagnostic="XeSS frame generation support is unavailable in this build.";
            else if(config.backend==BackendKind::External)decision.diagnostic="XeSS FG requires the mod's source ownership; external/Community Shaders presentation is not qualified.";
            else if(config.backend==BackendKind::Fsr && !fsrBuilt)decision.diagnostic="FSR upscaling support is unavailable in this build.";
            else if(config.backend==BackendKind::Xess && !XessBuilt)decision.diagnostic="XeSS upscaling support is unavailable in this build.";
            else if(config.backend==BackendKind::Fsr && !ValidProviderPolicy(config.providerPolicy))decision.diagnostic="Invalid FSR provider policy.";
            else if(config.neuralRendering && config.adapterVendorId!=0x10de)decision.diagnostic="NR requires a supported NVIDIA RTX render adapter.";
            else if(config.neuralRendering && !config.communityNeural)decision.diagnostic="XeSS FG requires the community NR runtime.";
            else if(config.hdr)decision.diagnostic="XeSS FG requires SDR output. Disable the mod's HDR output and restart.";
            else if(config.dynamicResolution)decision.diagnostic="XeSS FG requires fixed render dimensions. Disable dynamic resolution and restart.";
            else decision.valid=true;
        }
        else if(config.backend==BackendKind::Xess) {
            decision.presentation=config.generationBackend==2?PresentationKind::Fsr:PresentationKind::Ordinary;
            if(!XessBuilt)decision.diagnostic="XeSS support is unavailable in this build.";
            else if(config.generationBackend==2 && !fsrFgBuilt)decision.diagnostic="XeSS with FSR FG requires the built FSR presentation transport.";
            else if(config.generationBackend!=0 && config.generationBackend!=2)decision.diagnostic="XeSS NVIDIA FG integration is pending; select FSR or Auto with FG off.";
            else if(config.generationBackend==0 && config.generationEnabled)decision.diagnostic="XeSS Auto currently uses ordinary presentation. Select Backend=FSR, save and restart to use FG.";
            else if(config.neuralRendering && config.adapterVendorId!=0x10de)decision.diagnostic="XeSS NR requires a supported NVIDIA RTX render adapter.";
            else if(config.neuralRendering && !config.communityNeural)decision.diagnostic="XeSS requires the community NR runtime.";
            else if(config.hdr || config.dynamicResolution)decision.diagnostic="XeSS SR currently requires fixed dimensions and SDR output.";
            else decision.valid=true;
        }
        else if (config.backend == BackendKind::Dlss || config.backend == BackendKind::Dlaa) {
            if (config.generationBackend == 2) {
                decision.presentation = PresentationKind::Fsr;
                if (!fsrFgBuilt) decision.diagnostic = "FSR frame generation support is unavailable in this build.";
                else if (config.hdr) decision.diagnostic = "FSR frame generation requires SDR output. Disable HDR and restart.";
                else if (config.dynamicResolution) decision.diagnostic = "FSR frame generation requires fixed render dimensions. Disable dynamic resolution and restart.";
                else if (config.neuralRendering && !config.communityNeural) decision.diagnostic = "FSR presentation requires the community NR runtime.";
                else decision.valid = true;
            }
            else if (config.generationBackend != 1) { decision.diagnostic = "DLSS/DLAA requires NVIDIA or FSR presentation."; }
            else { decision.valid = true; }
        } else if (config.backend == BackendKind::Fsr) {
            decision.presentation = PresentationKind::Ordinary;
            if (config.generationBackend == 2) decision.presentation = PresentationKind::Fsr;
            if (!fsrBuilt) { decision.diagnostic = "FSR support is unavailable in this build."; }
            else if (config.generationBackend == 1) {
                decision.diagnostic = "[FrameGeneration] Backend=NVIDIA requires [Upscaling] Upscaler=DLSS; use Auto or FSR.";
            } else if (config.generationBackend != 0 && config.generationBackend != 2) {
                decision.diagnostic = "FSR requires its normal presenter, or the diagnostic [Upscaling Advanced] FsrOrdinaryPresenter=true with FG disabled.";
            } else if (config.generationBackend == 0 && config.generationEnabled) {
                decision.diagnostic = "Ordinary FSR presentation requires frame generation off.";
            } else if (config.generationBackend == 2 && !fsrFgBuilt) {
                decision.diagnostic = "FSR frame generation support is unavailable in this build.";
            } else if (!ValidProviderPolicy(config.providerPolicy)) {
                decision.diagnostic = "Invalid FSR provider policy.";
            } else if (config.neuralRendering && !config.communityNeural) { decision.diagnostic = "Neural Rendering is unavailable with FSR."; }
            else if (config.hdr) { decision.diagnostic = "HDR output is not validated with FSR."; }
            else if (config.dynamicResolution) { decision.diagnostic = "FSR currently requires fixed render dimensions."; }
            else { decision.valid = true; }
        } else { decision.diagnostic = "External rendering ownership must be supplied by the frame producer."; }
        return decision;
    }

    template<class Ini> const char* ValidateRendererConfiguration(const Ini& source, bool fsrBuilt, bool fsrFgBuilt = false)
    {
        const TheosRenderPipeline::IniLayout::ReadView ini(source);
        // Pass count is a saved preference shared with the community runtime.
        // Legacy execution caps it at two without erasing the third-pass setup.
        const auto mode = ini.GetLongValue("Settings", "UpscaleType", DLSS);
        const auto presenter = ini.GetLongValue("Experimental", "FrameGenerationBackend", 1);
        if(presenter==3) {
            if(!XessFgBuilt)return "XeSS frame generation support is unavailable in this build.";
            if(mode!=DLSS && mode!=DLAA && mode!=FSR && mode!=Xess)return "XeSS FG requires DLSS, FSR or XeSS upscaling with the mod's source ownership.";
            if(mode==FSR && !fsrBuilt)return "FSR upscaling support is unavailable in this build.";
            if(mode==Xess && !XessBuilt)return "XeSS upscaling support is unavailable in this build.";
            if(!ini.GetBoolValue("Settings","NativeUI",true) || ini.GetLongValue("Experimental","NativeUICompositionMode",0)!=0)return "XeSS FG requires [Interface] NativeUI=true and UIComposition=Dedicated.";
            if(ini.GetBoolValue("HDROutput","Enabled",false))return "XeSS FG requires SDR output. Disable the mod's HDR output and restart.";
            if(ini.GetBoolValue("DynamicResolution","Enabled",false) || ini.GetBoolValue("DynamicResolution","Oscillate",false))return "XeSS FG requires fixed render dimensions. Disable dynamic resolution and restart.";
            if(!ini.GetBoolValue("Settings","EnableJitter",true))return "XeSS FG requires camera jitter enabled.";
            if(ini.GetBoolValue("SourceDLSSG","NeuralRenderingEnabled",false) && ini.GetBoolValue("NeuralRendering","LegacyRuntimeDiagnostic",false))return "XeSS FG requires the community NR runtime.";
            return ValidateLegacyRendererExperiments(ini);
        }
        if(mode==Xess) {
            if(!XessBuilt)return "XeSS support is unavailable in this build.";
            if(presenter==2) {
                if(!fsrFgBuilt)return "XeSS with FSR FG requires the built FSR presentation transport.";
                if(!ini.GetBoolValue("Settings","NativeUI",true) || ini.GetLongValue("Experimental","NativeUICompositionMode",0)!=0)
                    return "XeSS with FSR FG requires [Interface] NativeUI=true and UIComposition=Dedicated.";
            } else if(presenter!=0)return "XeSS NVIDIA FG integration is pending; select FSR or Auto with FG off.";
            else if(ini.GetBoolValue("FrameGeneration","Enabled",false))return "XeSS Auto currently uses ordinary presentation. Select Backend=FSR, save and restart to use FG.";
            if(ini.GetBoolValue("SourceDLSSG","NeuralRenderingEnabled",false) && ini.GetBoolValue("NeuralRendering","LegacyRuntimeDiagnostic",false))return "XeSS requires the community NR runtime.";
            if(ini.GetBoolValue("HDROutput","Enabled",false) || ini.GetBoolValue("DynamicResolution","Enabled",false) || ini.GetBoolValue("DynamicResolution","Oscillate",false))return "XeSS SR requires fixed dimensions and SDR output.";
            if(!ini.GetBoolValue("Settings","EnableJitter",true))return "XeSS requires camera jitter enabled.";
            return ValidateLegacyRendererExperiments(ini);
        }
        if (mode != FSR && presenter != 2) { return ValidateNvidiaBaseline(ini); }
        if (mode != FSR && mode != DLSS && mode != DLAA) return "Choose DLSS or FSR upscaling.";
        if (mode == FSR && !fsrBuilt) { return "FSR support is unavailable in this build."; }
        if (!ini.GetBoolValue("Settings", "EnableUpscaler", true)) { return "FSR cannot start with upscaling disabled. Remove the obsolete [Settings] EnableUpscaler key."; }
        if (ini.GetBoolValue("Experimental", "PureDarkFullDelegation", false)) { return "Full renderer delegation is unavailable."; }
        if (presenter == 0) {
            if (ini.GetBoolValue("FrameGeneration", "Enabled", false)) { return "Ordinary FSR requires frame generation off."; }
        } else if (presenter == 2) {
            if (!fsrFgBuilt) { return "FSR frame generation support is unavailable in this build."; }
            if (!ini.GetBoolValue("Settings", "NativeUI", true) ||
                ini.GetLongValue("Experimental", "NativeUICompositionMode", 0) != 0) {
                return "FSR frame generation requires [Interface] NativeUI=true and UIComposition=Dedicated.";
            }
        } else if(mode==FSR && presenter==1) { return "[FrameGeneration] Backend=NVIDIA requires [Upscaling] Upscaler=DLSS; use Auto or FSR."; }
        else { return "FSR requires its normal presenter, or the diagnostic [Upscaling Advanced] FsrOrdinaryPresenter=true with FG disabled."; }
        if (ini.GetBoolValue("SourceDLSSG", "NeuralRenderingEnabled", false) &&
#if !defined(TRP_NO_NEURAL_RENDERING)
            !ini.GetBoolValue("NeuralRendering", "CommunityRuntime",
                !ini.GetBoolValue("NeuralRendering", "LegacyRuntimeDiagnostic", false)) &&
#endif
            true) { return "Neural Rendering is unavailable with FSR."; }
        if (ini.GetBoolValue("HDROutput", "Enabled", false)) { return "HDR output is not validated with FSR."; }
        if (ini.GetBoolValue("DynamicResolution", "Enabled", false) || ini.GetBoolValue("DynamicResolution", "Oscillate", false)) {
            return "FSR currently requires fixed render dimensions.";
        }
        return ValidateLegacyRendererExperiments(ini);
    }
}
