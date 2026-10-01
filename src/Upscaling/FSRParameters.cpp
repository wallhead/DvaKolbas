#include "FSRParameters.h"
#include <dx12/ffx_api_dx12.h>
#include <wrl/client.h>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <DirectXMath.h>
#include <cstring>

namespace TheosRenderPipeline::Upscaling
{
    static bool ValidTexture(ID3D12Resource* resource, Extent size, DXGI_FORMAT format, bool output=false)
    {
        if (!resource) return false;
        const auto desc=resource->GetDesc();
        return desc.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && desc.Width==size.width && desc.Height==size.height &&
            desc.DepthOrArraySize==1 && desc.MipLevels==1 && desc.SampleDesc.Count==1 && desc.SampleDesc.Quality==0 &&
            desc.Format==format && !(desc.Flags&D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) &&
            (!output || (desc.Flags&D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));
    }
    Result<ffxDispatchDescUpscale> BuildFsrDispatch(const GpuFrameResources& gpu, const UpscaleFrame& frame, const FsrContextLimits& limits)
    {
        auto invalid=[](const char* message)->Result<ffxDispatchDescUpscale> { return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,message}); };
        const auto finite=[](float x){return std::isfinite(x);};
        const auto& camera=frame.camera;
        if (frame.backend!=BackendKind::Fsr || !limits.render.width || !limits.render.height || !limits.output.width || !limits.output.height ||
            frame.render!=limits.render || frame.display!=limits.output || frame.subrect!=frame.render)
            return invalid("FSR requires provider-sized fixed render/output extents and a full prepared subrect");
        if (!finite(frame.deltaMilliseconds) || frame.deltaMilliseconds<=0 || !finite(frame.preExposure) || frame.preExposure<=0 ||
            !finite(frame.jitterX) || !finite(frame.jitterY) || std::abs(frame.jitterX)>1 || std::abs(frame.jitterY)>1 ||
            !finite(frame.sharpness) || frame.sharpness<0 || frame.sharpness>1)
            return invalid("FSR time must be positive milliseconds; jitter, exposure and sharpening must be finite and valid");
        if (!camera.identity || !std::ranges::all_of(camera.view,finite) || !std::ranges::all_of(camera.projection,finite) ||
            !std::ranges::all_of(camera.position,finite) || !finite(camera.nearDistance) || camera.nearDistance<=0 ||
            (!camera.depthInfinite && (!finite(camera.farDistance) || camera.farDistance<=camera.nearDistance)) ||
            (camera.depthInfinite && !(std::isinf(camera.farDistance) && camera.farDistance>0)) ||
            !finite(camera.verticalFovRadians) || camera.verticalFovRadians<=0 || camera.verticalFovRadians>=std::numbers::pi_v<float> ||
            !finite(camera.worldUnitsToMeters) || camera.worldUnitsToMeters<=0)
            return invalid("FSR camera measurements are invalid");
        if (camera.depthInverted!=limits.input.depthInverted || camera.depthInfinite!=limits.input.depthInfinite ||
            !frame.motionConvention.currentToPrevious || frame.motionConvention.includesJitter!=limits.input.motionIncludesJitter ||
            !finite(frame.motionConvention.scaleX) || !finite(frame.motionConvention.scaleY) ||
            frame.motionConvention.scaleX==0 || frame.motionConvention.scaleY==0 ||
            !frame.colorIsLinear || !limits.input.colorIsLinear)
            return invalid("FSR input depth/motion/color conventions do not match the context; prepare explicit linear SDR guides");
        DirectX::XMFLOAT4X4 view,projection;
        std::memcpy(&view,camera.view.data(),sizeof(view));std::memcpy(&projection,camera.projection.data(),sizeof(projection));
        const auto determinant=DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&view)));
        if (!finite(determinant) || std::abs(determinant)<1e-12f || projection._11<=0 || projection._22<=0 ||
            std::abs(projection._34)<0.5f || std::abs(projection._44)>0.001f ||
            std::abs(2.0f*std::atan(1.0f/projection._22)-camera.verticalFovRadians)>0.001f)
            return invalid("FSR camera basis/projection must describe the supplied physical FOV");
        if (frame.colorFormat!=limits.colorFormat || frame.depthFormat!=limits.depthFormat || frame.motionFormat!=limits.motionFormat ||
            !ValidTexture(gpu.color,frame.render,limits.colorFormat) || !ValidTexture(gpu.depth,frame.render,limits.depthFormat) ||
            !ValidTexture(gpu.motion,frame.render,limits.motionFormat) || !ValidTexture(gpu.output,frame.display,limits.colorFormat,true))
            return invalid("FSR resource formats, dimensions, samples or output UAV flags are invalid");
        if ((gpu.exposure && !ValidTexture(gpu.exposure,{1,1},DXGI_FORMAT_R32_FLOAT)) ||
            (gpu.reactive && !ValidTexture(gpu.reactive,frame.render,DXGI_FORMAT_R8_UNORM)) ||
            (gpu.transparencyComposition && !ValidTexture(gpu.transparencyComposition,frame.render,DXGI_FORMAT_R8_UNORM)))
            return invalid("FSR optional exposure/mask resources are invalid");
        Microsoft::WRL::ComPtr<IUnknown> deviceIdentity;
        auto resources=gpu.All();
        for (std::size_t i=0;i<resources.size();++i) if (resources[i]) {
            Microsoft::WRL::ComPtr<IUnknown> identity;
            if (FAILED(resources[i]->GetDevice(IID_PPV_ARGS(&identity)))) return invalid("FSR resource device unavailable");
            if (deviceIdentity && deviceIdentity.Get()!=identity.Get()) return invalid("FSR resources belong to different devices");
            deviceIdentity=identity;
            for(std::size_t j=0;j<i;++j) if(resources[j]==resources[i]) return invalid("FSR input/output resources must not alias");
        }
        ffxDispatchDescUpscale result{}; result.header.type=FFX_API_DISPATCH_DESC_TYPE_UPSCALE;
        result.color=ffxApiGetResourceDX12(gpu.color); result.depth=ffxApiGetResourceDX12(gpu.depth);
        result.motionVectors=ffxApiGetResourceDX12(gpu.motion); result.output=ffxApiGetResourceDX12(gpu.output,FFX_API_RESOURCE_STATE_UNORDERED_ACCESS);
        result.exposure=ffxApiGetResourceDX12(gpu.exposure); result.reactive=ffxApiGetResourceDX12(gpu.reactive);
        result.transparencyAndComposition=ffxApiGetResourceDX12(gpu.transparencyComposition);
        result.jitterOffset={frame.jitterX,frame.jitterY}; result.motionVectorScale={frame.motionConvention.scaleX,frame.motionConvention.scaleY};
        result.renderSize={frame.render.width,frame.render.height}; result.upscaleSize={frame.display.width,frame.display.height};
        result.enableSharpening=frame.sharpness>0; result.sharpness=frame.sharpness;
        result.frameTimeDelta=frame.deltaMilliseconds; result.preExposure=frame.preExposure; result.reset=frame.reset || camera.reset;
        result.cameraNear=camera.nearDistance; result.cameraFar=camera.farDistance;
        result.cameraFovAngleVertical=camera.verticalFovRadians; result.viewSpaceToMetersFactor=camera.worldUnitsToMeters;
        return result;
    }
}
