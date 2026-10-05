#pragma once

#include "IniLayout.h"
#include "UpscaleType.h"
#include <cstdint>

namespace TheosRenderPipeline
{
inline bool IsAmdRenderer(std::uint32_t vendor) { return vendor == 0x1002; }

// This restricts providers, independently of NR's model/device-ID catalog.
inline bool AmdRendererSelectionAllowed(int mode, long presenter, bool neural)
{
    return mode == FSR && (presenter == 0 || presenter == 2) && !neural;
}

// Normalize only the in-memory startup view. Saving defaults remains explicit.
// Keep an existing FSR FG request; never translate NVIDIA FG into enabled FSR FG.
template<class Ini> void ApplyRendererGpuPolicy(Ini& ini, std::uint32_t vendor, bool fsrFgBuilt)
{
    if (!IsAmdRenderer(vendor)) return;
    const IniLayout::ReadView read(ini);
    const bool fsr = read.GetLongValue("Settings", "UpscaleType", DLSS) == FSR;
    const long backend = read.GetLongValue("Experimental", "FrameGenerationBackend", 1);
    const bool enabled = fsr && backend == 2 && fsrFgBuilt && read.GetBoolValue("FrameGeneration", "Enabled", true);
    const long presenter = fsr && backend == 0 ? 0 : fsrFgBuilt ? 2 : 0;
    ini.SetLongValue("Settings", "UpscaleType", FSR);
    ini.SetBoolValue("Settings", "EnableUpscaler", true);
    ini.SetBoolValue("Settings", "NativeUI", true);
    ini.SetValue("FSR", "ProviderPolicy", "Analytical");
    ini.SetLongValue("FrameGeneration", "Backend", presenter);
    ini.SetLongValue("Experimental", "FrameGenerationBackend", presenter);
    ini.SetBoolValue("FrameGeneration", "Enabled", enabled);
    ini.SetLongValue("FrameGeneration", "UICompositionMode", 0);
    ini.SetLongValue("Experimental", "NativeUICompositionMode", 0);
    ini.SetBoolValue("NeuralRendering", "Enabled", false);
    ini.SetBoolValue("SourceDLSSG", "NeuralRenderingEnabled", false);
    ini.SetBoolValue("NeuralRendering", "CommunityRuntime", false);
    ini.SetBoolValue("HDROutput", "Enabled", false);
    ini.SetBoolValue("DynamicResolution", "Enabled", false);
    ini.SetBoolValue("DynamicResolution", "Oscillate", false);
}
}
