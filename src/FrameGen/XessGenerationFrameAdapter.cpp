#include "XessGenerationFrameAdapter.h"
#include "Upscaling/CameraDepthPolicy.h"
#include <algorithm>
#include <cmath>
#include <numbers>
namespace TheosRenderPipeline
{
    Upscaling::Result<XessGenerationFrame> AdaptXessGenerationFrame(const Upscaling::UpscaleFrame& frame, std::uint32_t sdkId)
    {
        using namespace Upscaling;
        const auto invalid=[](const char* message)->Result<XessGenerationFrame> {
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,message});
        };
        const auto valid=[](Extent extent) { return extent.width && extent.height; };
        const auto finite=[](float value) { return std::isfinite(value); };
        if (!frame.sourceId || !frame.sourceEpoch || !frame.output || !frame.depth || !frame.motion ||
            !valid(frame.render) || !valid(frame.display) || frame.subrect!=frame.render ||
            frame.render.width>frame.display.width || frame.render.height>frame.display.height)
            return invalid("Intel FG requires completed output, source identity and fixed full render/display regions");
        if (frame.backend!=BackendKind::Dlss && frame.backend!=BackendKind::Dlaa && frame.backend!=BackendKind::Fsr && frame.backend!=BackendKind::Xess)
            return invalid("Intel FG source upscaler is unsupported");
        if (!valid(frame.depthExtent) || frame.depthExtent!=frame.motionExtent ||
            (frame.motionExtent!=frame.render && frame.motionExtent!=frame.display))
            return invalid("Intel FG depth/motion valid regions must agree and match render or display size");
        const bool highResolution=frame.motionExtent!=frame.render;
        if (highResolution!=frame.motionDilated)
            return invalid("Intel FG requires undilated low-resolution or explicitly dilated high-resolution guides");
        if (frame.depthFormat!=DXGI_FORMAT_R32_FLOAT || frame.motionFormat!=DXGI_FORMAT_R16G16_FLOAT)
            return invalid("Intel FG prepared guides must be R32_FLOAT depth and signed RG16_FLOAT motion");
        if (!finite(frame.motionConvention.scaleX) || !finite(frame.motionConvention.scaleY) ||
            frame.motionConvention.scaleX==0 || frame.motionConvention.scaleY==0 ||
            !finite(frame.jitterX) || !finite(frame.jitterY) || std::abs(frame.jitterX)>.5f || std::abs(frame.jitterY)>.5f ||
            !finite(frame.deltaMilliseconds) || frame.deltaMilliseconds<0)
            return invalid("Intel FG requires finite motion scales, half-pixel jitter and nonnegative measured milliseconds");
        const auto& camera=frame.camera;
        if (!camera.identity || !std::ranges::all_of(camera.view,finite) || !std::ranges::all_of(camera.projection,finite) ||
            !std::ranges::all_of(camera.position,finite) ||
            !ValidCameraDepthRange(camera.nearDistance,camera.farDistance,camera.depthInfinite) ||
            !finite(camera.verticalFovRadians) || camera.verticalFovRadians<=0 || camera.verticalFovRadians>=std::numbers::pi_v<float> ||
            !finite(camera.worldUnitsToMeters) || camera.worldUnitsToMeters<=0)
            return invalid("Intel FG camera measurements are invalid");
        const auto& v=camera.view;
        const auto determinant=v[0]*(v[5]*v[10]-v[6]*v[9])-v[1]*(v[4]*v[10]-v[6]*v[8])+v[2]*(v[4]*v[9]-v[5]*v[8]);
        const auto& p=camera.projection;
        if (!finite(determinant) || std::abs(determinant)<1e-12f || p[0]<=0 || p[5]<=0 || std::abs(p[11])<.5f ||
            std::abs(p[15])>.001f || std::abs(p[8])>1e-7f || std::abs(p[9])>1e-7f ||
            std::abs(2*std::atan(1.f/p[5])-camera.verticalFovRadians)>.001f)
            return invalid("Intel FG requires a nondegenerate row-major camera and qualified unjittered centered perspective projection");
        XessGenerationFrame result;
        result.render=frame.render;result.display=frame.display;result.depth=frame.depthExtent;result.motion=frame.motionExtent;
        result.sdkId=sdkId;result.sourceId=frame.sourceId;result.sourceEpoch=frame.sourceEpoch;
        std::copy(camera.view.begin(),camera.view.end(),result.constants.viewMatrix);
        std::copy(camera.projection.begin(),camera.projection.end(),result.constants.projectionMatrix);
        result.constants.jitterOffsetX=frame.jitterX;result.constants.jitterOffsetY=frame.jitterY;
        const float direction=frame.motionConvention.currentToPrevious?1.f:-1.f;
        result.constants.motionVectorScaleX=direction*frame.motionConvention.scaleX;
        result.constants.motionVectorScaleY=direction*frame.motionConvention.scaleY;
        result.constants.resetHistory=frame.reset || camera.reset;
        result.constants.frameRenderTime=frame.deltaMilliseconds;
        if (camera.depthInverted) result.initFlags|=XEFG_SWAPCHAIN_INIT_FLAG_INVERTED_DEPTH;
        if (highResolution) result.initFlags|=XEFG_SWAPCHAIN_INIT_FLAG_HIGH_RES_MV;
        if (frame.motionConvention.includesJitter) result.initFlags|=XEFG_SWAPCHAIN_INIT_FLAG_JITTERED_MV;
        if (frame.motionConvention.usesNdc) result.initFlags|=XEFG_SWAPCHAIN_INIT_FLAG_USE_NDC_VELOCITY;
        return result;
    }
}
