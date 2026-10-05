#pragma once

#include "RendererBackendPolicy.h"
#include "Upscaling/FSRSettings.h"
#include <string>

namespace TheosRenderPipeline
{
// Provider validation needs the actual renderer adapter. Keep encoding checks
// explicit even when a saved NVIDIA selection is normalized for AMD.
template<class Ini> std::string ValidateRendererStartup(Ini& ini, std::uint32_t vendor, bool fsrBuilt, bool fsrFgBuilt)
{
    ApplyRendererGpuPolicy(ini, vendor, fsrFgBuilt);
    if (const auto* error = ValidateRendererConfiguration(ini, fsrBuilt, fsrFgBuilt)) return error;
    if (ini.GetLongValue("Settings", "UpscaleType", DLSS) == FSR) {
        const auto fsr = Upscaling::ReadFsrSettings(ini);
        if (!fsr) return fsr.error().message;
        if (!Upscaling::IsKnownColorEncoding(fsr->sourceColorEncoding))
            return "FSR requires an explicit SourceColorEncoding: Linear, Gamma22 or SRGB. Missing/Unknown encoding is not guessed.";
    }
    return {};
}
}
