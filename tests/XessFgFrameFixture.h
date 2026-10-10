#pragma once
#include "Upscaling/UpscalerBackend.h"
#include <cmath>
inline TheosRenderPipeline::Upscaling::UpscaleFrame XessFgFrame()
{
    using namespace TheosRenderPipeline::Upscaling;
    UpscaleFrame frame;
    // Pure contract tests use non-dereferenced sentinels. Transport tests must
    // independently validate actual resources, formats, devices and extents.
    frame.output=frame.depth=frame.motion=reinterpret_cast<ID3D11Texture2D*>(1);
    frame.render=frame.subrect=frame.depthExtent=frame.motionExtent={1600,900};
    frame.display={2560,1440};frame.sourceId=1;frame.sourceEpoch=7;
    frame.motionConvention={1600,900,true,false};frame.deltaMilliseconds=1000.f/13.f;
    frame.camera.view={1,0,0,0,0,1,0,0,0,0,1,0,3,4,5,1};
    frame.camera.projection={.5625f,0,0,0,0,1,0,0,0,0,1.01f,1,0,0,-.101f,0};
    frame.camera.identity=41;frame.camera.nearDistance=.1f;frame.camera.farDistance=10;
    frame.camera.verticalFovRadians=2*std::atan(1.f);
    frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
    return frame;
}
