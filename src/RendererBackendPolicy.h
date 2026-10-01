#pragma once

#include "NvidiaBaselinePolicy.h"
#include "Upscaling/UpscalerBackend.h"

namespace TheosRenderPipeline
{
    inline Upscaling::BackendDecision ResolveBackend(const Upscaling::BackendConfiguration& config, bool fsrBuilt)
    {
        using namespace Upscaling;
        BackendDecision decision{config.backend, PresentationKind::Nvidia, false, config.generationEnabled, {}};
        if (!config.enabled) { decision.diagnostic = "This renderer requires an enabled temporal upscaler."; }
        else if (config.backend == BackendKind::Dlss || config.backend == BackendKind::Dlaa) {
            if (config.generationBackend != 1) { decision.diagnostic = "DLSS/DLAA requires the NVIDIA presentation backend."; }
            else { decision.valid = true; }
        } else if (config.backend == BackendKind::Fsr) {
            decision.presentation = PresentationKind::Ordinary;
            if (!fsrBuilt) { decision.diagnostic = "FSR support is unavailable in this build."; }
            else if (config.generationEnabled || config.generationBackend != 0) {
                decision.diagnostic = "FSR currently requires frame generation off and ordinary presentation (backend 0).";
            } else if (config.neuralRendering) { decision.diagnostic = "Neural Rendering is unavailable with FSR."; }
            else if (config.hdr) { decision.diagnostic = "HDR output is not validated with FSR."; }
            else if (config.dynamicResolution) { decision.diagnostic = "FSR currently requires fixed render dimensions."; }
            else { decision.valid = true; }
        } else { decision.diagnostic = "External rendering ownership must be supplied by the frame producer."; }
        return decision;
    }

    template<class Ini> const char* ValidateRendererConfiguration(const Ini& ini, bool fsrBuilt)
    {
        const auto mode = ini.GetLongValue("Settings", "UpscaleType", DLSS);
        if (mode != FSR) { return ValidateNvidiaBaseline(ini); }
        if (!fsrBuilt) { return "FSR support is unavailable in this build."; }
        if (!ini.GetBoolValue("Settings", "EnableUpscaler", true)) { return "FSR requires EnableUpscaler=true."; }
        if (ini.GetBoolValue("Experimental", "PureDarkFullDelegation", false)) { return "Full renderer delegation is unavailable."; }
        if (ini.GetBoolValue("FrameGeneration", "Enabled", true) ||
            ini.GetLongValue("Experimental", "FrameGenerationBackend", 1) != 0) {
            return "FSR requires frame generation off and ordinary presentation (backend 0).";
        }
        if (ini.GetBoolValue("SourceDLSSG", "NeuralRenderingEnabled", false)) { return "Neural Rendering is unavailable with FSR."; }
        if (ini.GetBoolValue("HDROutput", "Enabled", false)) { return "HDR output is not validated with FSR."; }
        if (ini.GetBoolValue("DynamicResolution", "Enabled", false) || ini.GetBoolValue("DynamicResolution", "Oscillate", false)) {
            return "FSR currently requires fixed render dimensions.";
        }
        return ValidateLegacyRendererExperiments(ini);
    }
}
