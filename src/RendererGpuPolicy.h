#pragma once

#include "IniLayout.h"
#include "UpscaleType.h"
#include <cstdint>

namespace TheosRenderPipeline
{
inline bool IsAmdRenderer(std::uint32_t vendor) { return vendor == 0x1002; }

// This restricts providers, independently of NR's model/device-ID catalog.
inline bool FsrOnlyRendererSelectionAllowed(int mode, long presenter, bool neural)
{
    return (mode == FSR && (presenter == 0 || presenter == 2 || presenter == 3) || mode == Xess && (presenter == 0 || presenter == 2 || presenter == 3)) && !neural;
}

// Normalize only the in-memory startup view. Saving defaults remains explicit.
// Keep an existing FSR FG request; never translate NVIDIA FG into enabled FSR FG.
template<class Ini> void ApplyRendererGpuPolicy(Ini& ini, std::uint32_t vendor, bool fsrFgBuilt, bool fsrOnlyRenderer = false)
{
    if (!IsAmdRenderer(vendor) && !fsrOnlyRenderer) return;
    const IniLayout::ReadView read(ini);
    if(read.GetLongValue("Settings","UpscaleType",DLSS)==Xess) return; // Explicit cross-vendor SR; validated separately, never normalized to FSR.
    const bool fsr = read.GetLongValue("Settings", "UpscaleType", DLSS) == FSR;
    const long backend = read.GetLongValue("Experimental", "FrameGenerationBackend", 1);
    const bool intel=backend==3;
    const bool enabled = (intel || fsr && backend == 2 && fsrFgBuilt) && read.GetBoolValue("FrameGeneration", "Enabled", false);
    const long presenter = intel ? 3 : fsr && backend == 0 ? 0 : fsrFgBuilt ? 2 : 0;
    ini.SetLongValue("Settings", "UpscaleType", FSR);
    ini.SetBoolValue("Settings", "EnableUpscaler", true);
    ini.SetBoolValue("Settings", "NativeUI", true);
    ini.SetLongValue("FrameGeneration", "Backend", presenter);
    ini.SetLongValue("Experimental", "FrameGenerationBackend", presenter);
    ini.SetBoolValue("FrameGeneration", "Enabled", enabled);
    ini.SetLongValue("FrameGeneration", "UICompositionMode", 0);
    ini.SetLongValue("Experimental", "NativeUICompositionMode", 0);
    ini.SetBoolValue("NeuralRendering", "Enabled", false);
    ini.SetBoolValue("SourceDLSSG", "NeuralRenderingEnabled", false);
    ini.SetBoolValue("NeuralRendering", "CommunityRuntime", false);
    if(!intel) {
        ini.SetBoolValue("HDROutput", "Enabled", false);
        ini.SetBoolValue("DynamicResolution", "Enabled", false);
        ini.SetBoolValue("DynamicResolution", "Oscillate", false);
    }
    if (ini.GetLongValue("FrameGeneration", "BackendPreference", 0) == 1)
        ini.SetLongValue("FrameGeneration", "BackendPreference", 0);
    if (vendor == 0x10de) {
        // Non-RTX NVIDIA must not enter either ML or NVIDIA-only probing.
        ini.SetValue("FSR", "ProviderPolicy", "Analytical");
        ini.SetValue("FrameGeneration", "FsrProviderPolicy", "Analytical");
        ini.SetBoolValue("Experimental", "SourceDLSSGMFGUnlock", false);
        ini.SetBoolValue("Compatibility", "NvidiaMFGUnlock", false);
    }
}
}
