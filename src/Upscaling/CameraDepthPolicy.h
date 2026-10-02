#pragma once
#include <cmath>
namespace TheosRenderPipeline::Upscaling
{
    inline bool ValidCameraDepthRange(float nearDistance,float farDistance,bool infinite)
    {
        return std::isfinite(nearDistance) && nearDistance>0 &&
            (infinite ? std::isinf(farDistance) && farDistance>0 : std::isfinite(farDistance) && farDistance>nearDistance);
    }
}
