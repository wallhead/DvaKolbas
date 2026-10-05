#pragma once
#include "IniLayout.h"
#include "RendererGpuPolicy.h"

#include "NvidiaBaselinePolicy.h"
#include "Upscaling/UpscalerBackend.h"
#include <string_view>

namespace TheosRenderPipeline
{
    inline Upscaling::BackendDecision ResolveBackend(const Upscaling::BackendConfiguration& config, bool fsrBuilt, bool fsrFgBuilt = false)
    {
        using namespace Upscaling;
        BackendDecision decision{config.backend, PresentationKind::Nvidia, false, config.generationEnabled, {}};
        if (IsAmdRenderer(config.adapterVendorId) &&
            !AmdRendererSelectionAllowed(config.backend == BackendKind::Fsr ? ::FSR : ::DLSS,
                config.generationBackend, config.neuralRendering)) {
            decision.diagnostic = "AMD supports only FSR upscaling and optional FSR frame generation; DLSS, DLAA and NR are unavailable.";
        }
        else if (IsAmdRenderer(config.adapterVendorId) && config.providerPolicy != ProviderPolicy::Analytical) {
            decision.diagnostic = "AMD currently uses the Analytical FSR provider; Compatible/ML is not validated.";
        }
        else if (!config.enabled) { decision.diagnostic = "This renderer requires an enabled temporal upscaler."; }
        else if (config.backend == BackendKind::Dlss || config.backend == BackendKind::Dlaa) {
            if (config.generationBackend != 1) { decision.diagnostic = "DLSS/DLAA requires the NVIDIA presentation backend."; }
            else { decision.valid = true; }
        } else if (config.backend == BackendKind::Fsr) {
            decision.presentation = PresentationKind::Ordinary;
            if (config.generationBackend == 2) decision.presentation = PresentationKind::Fsr;
            if (!fsrBuilt) { decision.diagnostic = "FSR support is unavailable in this build."; }
            else if (config.generationBackend != 0 && config.generationBackend != 2) {
                decision.diagnostic = "FSR requires ordinary presentation (backend 0) or the FSR presenter (backend 2).";
            } else if (config.generationBackend == 0 && config.generationEnabled) {
                decision.diagnostic = "Ordinary FSR presentation requires frame generation off.";
            } else if (config.generationBackend == 2 && !fsrFgBuilt) {
                decision.diagnostic = "FSR frame generation support is unavailable in this build.";
            } else if (config.generationBackend == 2 && config.providerPolicy != ProviderPolicy::Analytical) {
                decision.diagnostic = "FSR frame generation initially requires the analytical SR provider.";
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
        if (mode != FSR) { return ValidateNvidiaBaseline(ini); }
        if (!fsrBuilt) { return "FSR support is unavailable in this build."; }
        if (!ini.GetBoolValue("Settings", "EnableUpscaler", true)) { return "FSR requires EnableUpscaler=true."; }
        if (ini.GetBoolValue("Experimental", "PureDarkFullDelegation", false)) { return "Full renderer delegation is unavailable."; }
        const auto presenter = ini.GetLongValue("Experimental", "FrameGenerationBackend", 1);
        if (presenter == 0) {
            if (ini.GetBoolValue("FrameGeneration", "Enabled", false)) { return "Ordinary FSR requires frame generation off."; }
        } else if (presenter == 2) {
            if (!fsrFgBuilt) { return "FSR frame generation support is unavailable in this build."; }
            if (std::string_view(ini.GetValue("FSR", "ProviderPolicy", "Analytical")) != "Analytical") {
                return "FSR frame generation requires the analytical provider.";
            }
            if (!ini.GetBoolValue("Settings", "NativeUI", true) ||
                ini.GetLongValue("Experimental", "NativeUICompositionMode", 0) != 0) {
                return "FSR frame generation requires NativeUI=true and dedicated NativeUICompositionMode=0.";
            }
        } else { return "FSR requires ordinary presentation (backend 0) or FSR frame generation (backend 2)."; }
        if (ini.GetBoolValue("SourceDLSSG", "NeuralRenderingEnabled", false) &&
#if !defined(TRP_NO_NEURAL_RENDERING)
            !ini.GetBoolValue("NeuralRendering", "CommunityRuntime", false) &&
#endif
            true) { return "Neural Rendering is unavailable with FSR."; }
        if (ini.GetBoolValue("HDROutput", "Enabled", false)) { return "HDR output is not validated with FSR."; }
        if (ini.GetBoolValue("DynamicResolution", "Enabled", false) || ini.GetBoolValue("DynamicResolution", "Oscillate", false)) {
            return "FSR currently requires fixed render dimensions.";
        }
        return ValidateLegacyRendererExperiments(ini);
    }
}
