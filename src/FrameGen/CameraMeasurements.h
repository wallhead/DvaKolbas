#pragma once
#include "Upscaling/UpscalerBackend.h"
#include <DirectXMath.h>
namespace TheosRenderPipeline
{
    Upscaling::Result<Upscaling::CameraMeasurements> MeasureCamera(const DirectX::XMFLOAT4X4& projection,
        const DirectX::XMFLOAT4X4& view, const std::array<float,3>& position, float nearPlane,float farPlane,
        std::uint64_t identity,bool reset,float worldUnitsToMeters=1.0f);
}
