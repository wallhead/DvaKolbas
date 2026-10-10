#pragma once
#include "Upscaling/UpscalerBackend.h"
#include <xess_fg/xefg_swapchain.h>
namespace TheosRenderPipeline
{
    struct XessGenerationFrame
    {
        xefg_swapchain_frame_constant_data_t constants{};
        Upscaling::Extent render{}, display{}, depth{}, motion{};
        std::uint32_t initFlags{}, sdkId{};
        std::uint64_t sourceId{}, sourceEpoch{};
    };
    Upscaling::Result<XessGenerationFrame> AdaptXessGenerationFrame(const Upscaling::UpscaleFrame&, std::uint32_t sdkId);
}
