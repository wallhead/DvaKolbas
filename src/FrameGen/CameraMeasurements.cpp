#include "CameraMeasurements.h"
#include "Upscaling/CameraDepthPolicy.h"
#include <algorithm>
#include <cmath>
#include <cstring>
namespace TheosRenderPipeline
{
    Upscaling::Result<Upscaling::CameraMeasurements> MeasureCamera(const DirectX::XMFLOAT4X4& projection,
        const DirectX::XMFLOAT4X4& view,const std::array<float,3>& position,float nearPlane,float farPlane,
        std::uint64_t identity,bool reset,float worldUnitsToMeters)
    {
        auto invalid=[](){return std::unexpected(Upscaling::RuntimeError{Upscaling::ErrorKind::InvalidInput,0,"Invalid camera measurements"});};
        auto finiteMatrix=[](const DirectX::XMFLOAT4X4& matrix){for(auto& row:matrix.m)for(float v:row)if(!std::isfinite(v))return false;return true;};
        const bool infinite=std::isinf(farPlane) && farPlane>0;
        if(!identity || !finiteMatrix(projection) || !finiteMatrix(view) ||
            !std::ranges::all_of(position,[](float v){return std::isfinite(v);}) ||
            std::abs(projection._34)<0.5f || std::abs(projection._44)>0.001f || projection._11<=0 || projection._22<=0 ||
            !Upscaling::ValidCameraDepthRange(nearPlane,farPlane,infinite) ||
            !std::isfinite(worldUnitsToMeters) || worldUnitsToMeters<=0) return invalid();
        const float direction=projection._34>0?1.0f:-1.0f;
        auto projectedDepth=[&](double distance){const double z=direction*distance;return (z*projection._33+projection._43)/(z*projection._34+projection._44);};
        // Evaluate the projection's limit explicitly: multiplying infinity by
        // zero would turn a valid reversed infinite projection into NaN.
        const double nearDepth=projectedDepth(nearPlane),farDepth=infinite?double(projection._33)/projection._34:projectedDepth(farPlane);
        if(!std::isfinite(nearDepth) || !std::isfinite(farDepth) || nearDepth==farDepth) return invalid();
        if(infinite && (std::abs(nearDepth-(nearDepth>farDepth?1.0:0.0))>0.000001 ||
            std::abs(farDepth-(nearDepth>farDepth?0.0:1.0))>0.000001))return invalid();
        auto absoluteView=view;
        absoluteView._41=-(position[0]*view._11+position[1]*view._21+position[2]*view._31);
        absoluteView._42=-(position[0]*view._12+position[1]*view._22+position[2]*view._32);
        absoluteView._43=-(position[0]*view._13+position[1]*view._23+position[2]*view._33);
        absoluteView._14=absoluteView._24=absoluteView._34=0; absoluteView._44=1;
        Upscaling::CameraMeasurements result;
        std::memcpy(result.view.data(),&absoluteView,sizeof(absoluteView));
        std::memcpy(result.projection.data(),&projection,sizeof(projection));
        result.position=position; result.nearDistance=nearPlane; result.farDistance=farPlane;
        result.verticalFovRadians=2.0f*std::atan(1.0f/projection._22); result.worldUnitsToMeters=worldUnitsToMeters;
        result.identity=identity; result.reset=reset; result.depthInverted=nearDepth>farDepth;result.depthInfinite=infinite;
        return result;
    }
}
