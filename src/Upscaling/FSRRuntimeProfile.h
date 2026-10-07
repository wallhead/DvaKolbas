#pragma once
#include "UpscalerBackend.h"
#include <ffx_upscale.h>

namespace TheosRenderPipeline::Upscaling
{
    enum class FsrRuntimeProfile { Official, Int8 };

    // Runtime choice is immutable for this process. Auto deliberately retains
    // its official-runtime fallback; the older INT8 analytical path is not qualified.
    inline FsrRuntimeProfile SelectFsrRuntimeProfile(ProviderPolicy policy, std::uint32_t vendor)
    {
        return policy == ProviderPolicy::MachineLearning && vendor == 0x10de ?
            FsrRuntimeProfile::Int8 : FsrRuntimeProfile::Official;
    }
    inline uint32_t FsrUpscaleApiVersion(FsrRuntimeProfile profile)
    {
        return profile == FsrRuntimeProfile::Int8 ? FFX_UPSCALER_MAKE_VERSION(4,0,3) : FFX_UPSCALER_VERSION;
    }
    inline bool FsrMlProfileAdmits(FsrRuntimeProfile profile, std::uint32_t vendor, const ProviderInfo& provider)
    {
        return profile == FsrRuntimeProfile::Int8 ? vendor == 0x10de && provider.name == "4.0.2b" :
            (!vendor || vendor == 0x1002);
    }
}
