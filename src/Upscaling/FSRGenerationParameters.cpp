#include "FSRGenerationParameters.h"
#include "CameraDepthPolicy.h"
#include <dx12/ffx_api_dx12.h>
#include <wrl/client.h>
#include <algorithm>
#include <cmath>
#include <numbers>
namespace TheosRenderPipeline::Upscaling
{
    Result<ffxDispatchDescFrameGenerationPrepareV2> BuildFsrGenerationPrepare(
        ID3D12GraphicsCommandList* list, const UpscaleFrame& frame, const FsrGenerationResources& resources,
        const FsrGenerationLimits& limits, bool reset)
    {
        const auto invalid = [](const char* reason)->Result<ffxDispatchDescFrameGenerationPrepareV2> {
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput, 0, reason});
        };
        const auto finite = [](float value) { return std::isfinite(value); };
        if (!list || list->GetType() != D3D12_COMMAND_LIST_TYPE_DIRECT || !frame.sourceId || frame.backend != BackendKind::Fsr ||
            frame.render != limits.render || frame.subrect != frame.render || frame.display != limits.display ||
            !limits.render.width || !limits.render.height || !limits.display.width || !limits.display.height ||
            limits.render.width > limits.display.width || limits.render.height > limits.display.height ||
            limits.format != DXGI_FORMAT_R8G8B8A8_UNORM || !limits.input.colorIsLinear || resources.sceneEncoding != ColorEncoding::SRGB)
            return invalid("FG requires matching fixed SDR extents, a direct command list and explicitly converted sRGB publication");
        if (!finite(frame.deltaMilliseconds) || frame.deltaMilliseconds <= 0 || frame.deltaMilliseconds >= 100 ||
            !finite(frame.jitterX) || !finite(frame.jitterY) || std::abs(frame.jitterX) > 1 || std::abs(frame.jitterY) > 1)
            return invalid("FG source time must be positive milliseconds below the stall limit, with finite bounded jitter");
        if (!frame.motionConvention.currentToPrevious || frame.motionConvention.includesJitter != limits.input.motionIncludesJitter ||
            !finite(frame.motionConvention.scaleX) || !finite(frame.motionConvention.scaleY) ||
            frame.motionConvention.scaleX == 0 || frame.motionConvention.scaleY == 0 ||
            frame.camera.depthInverted != limits.input.depthInverted || frame.camera.depthInfinite != limits.input.depthInfinite)
            return invalid("FG motion/depth conventions differ from the SR context");
        const auto& camera = frame.camera;
        if (!camera.identity || !std::ranges::all_of(camera.view, finite) || !std::ranges::all_of(camera.projection, finite) ||
            !std::ranges::all_of(camera.position, finite) || !ValidCameraDepthRange(camera.nearDistance, camera.farDistance, camera.depthInfinite) ||
            !finite(camera.verticalFovRadians) || camera.verticalFovRadians <= 0 || camera.verticalFovRadians >= std::numbers::pi_v<float> ||
            !finite(camera.worldUnitsToMeters) || camera.worldUnitsToMeters <= 0)
            return invalid("FG camera measurements are invalid");
        const auto& p = camera.projection;
        if (p[0] <= 0 || p[5] <= 0 || std::abs(p[11]) < .5f || std::abs(p[15]) > .001f ||
            std::abs(2.0f*std::atan(1.0f/p[5])-camera.verticalFovRadians) > .001f)
            return invalid("FG projection must describe the supplied perspective FOV");
        std::array<std::array<float, 3>, 3> axes{};
        for (unsigned axis = 0; axis < 3; ++axis) {
            float lengthSquared{};
            for (unsigned component = 0; component < 3; ++component) {
                axes[axis][component] = camera.view[component*4+axis];
                lengthSquared += axes[axis][component]*axes[axis][component];
            }
            if (!finite(lengthSquared) || lengthSquared < 1e-12f) return invalid("FG camera basis is degenerate");
            const auto length = std::sqrt(lengthSquared);
            for (auto& component : axes[axis]) component /= length;
        }
        for (unsigned a = 0; a < 3; ++a) for (unsigned b = 0; b < a; ++b) {
            float dot{}; for (unsigned component = 0; component < 3; ++component) dot += axes[a][component]*axes[b][component];
            if (std::abs(dot) > .001f) return invalid("FG camera basis must be orthogonal");
        }
        Microsoft::WRL::ComPtr<IUnknown> device;
        if (FAILED(list->GetDevice(IID_PPV_ARGS(&device)))) return invalid("FG command-list device unavailable");
        const std::array<ID3D12Resource*,4> inputs{resources.scene, resources.depth, resources.motion, resources.ui};
        const std::array<Extent,4> sizes{limits.display, limits.render, limits.render, limits.display};
        const std::array<DXGI_FORMAT,4> formats{limits.format, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R16G16_FLOAT, limits.format};
        for (size_t i = 0; i < inputs.size(); ++i) {
            if (!inputs[i]) return invalid("FG requires all scene, guides and completed publication UI resources");
            const auto desc = inputs[i]->GetDesc();
            if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || desc.Width != sizes[i].width || desc.Height != sizes[i].height ||
                desc.Format != formats[i] || desc.DepthOrArraySize != 1 || desc.MipLevels != 1 || desc.SampleDesc.Count != 1 ||
                desc.SampleDesc.Quality != 0 || (desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE))
                return invalid("FG resource dimensions/formats/samples are invalid");
            Microsoft::WRL::ComPtr<IUnknown> actual;
            if (FAILED(inputs[i]->GetDevice(IID_PPV_ARGS(&actual))) || actual.Get() != device.Get())
                return invalid("FG command list and resources must belong to the identical D3D12 device");
            for (size_t j = 0; j < i; ++j) if (inputs[j] == inputs[i]) return invalid("FG scene, guides and UI cannot alias");
        }
        if (frame.depthFormat != DXGI_FORMAT_R32_FLOAT || frame.motionFormat != DXGI_FORMAT_R16G16_FLOAT)
            return invalid("FG prepared guide formats must match SR");
        ffxDispatchDescFrameGenerationPrepareV2 result{};
        result.header.type = FFX_API_DISPATCH_DESC_TYPE_FRAMEGENERATION_PREPARE_V2;
        result.commandList = list; result.frameID = frame.sourceId;
        result.renderSize = {frame.render.width, frame.render.height};
        result.frameTimeDelta = frame.deltaMilliseconds; result.reset = reset || frame.reset || camera.reset;
        result.jitterOffset = {frame.jitterX, frame.jitterY};
        result.motionVectorScale = {frame.motionConvention.scaleX, frame.motionConvention.scaleY};
        result.depth = ffxApiGetResourceDX12(resources.depth, FFX_API_RESOURCE_STATE_COMPUTE_READ);
        result.motionVectors = ffxApiGetResourceDX12(resources.motion, FFX_API_RESOURCE_STATE_COMPUTE_READ);
        result.cameraNear = camera.nearDistance; result.cameraFar = camera.farDistance;
        result.cameraFovAngleVertical = camera.verticalFovRadians; result.viewSpaceToMetersFactor = camera.worldUnitsToMeters;
        std::copy(camera.position.begin(), camera.position.end(), result.cameraPosition);
        std::copy(axes[0].begin(), axes[0].end(), result.cameraRight);
        std::copy(axes[1].begin(), axes[1].end(), result.cameraUp);
        std::copy(axes[2].begin(), axes[2].end(), result.cameraForward);
        return result;
    }
}
