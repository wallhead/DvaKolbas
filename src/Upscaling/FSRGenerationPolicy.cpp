#include "FSRGenerationPolicy.h"
#include "CameraDepthPolicy.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace TheosRenderPipeline::Upscaling
{
    namespace
    {
        bool ValidCamera(const CameraMeasurements& camera)
        {
            const auto finite = [](float value) { return std::isfinite(value); };
            if (!camera.identity || !std::ranges::all_of(camera.view, finite) || !std::ranges::all_of(camera.projection, finite) ||
                !std::ranges::all_of(camera.position, finite) ||
                !ValidCameraDepthRange(camera.nearDistance, camera.farDistance, camera.depthInfinite) ||
                !finite(camera.worldUnitsToMeters) || camera.worldUnitsToMeters <= 0 ||
                !finite(camera.verticalFovRadians) || camera.verticalFovRadians <= 0 || camera.verticalFovRadians >= std::numbers::pi_v<float>) return false;
            const auto& v = camera.view;
            const auto determinant = v[0]*(v[5]*v[10]-v[6]*v[9]) - v[1]*(v[4]*v[10]-v[6]*v[8]) + v[2]*(v[4]*v[9]-v[5]*v[8]);
            const auto& p = camera.projection;
            return std::isfinite(determinant) && std::abs(determinant) >= 1e-12f && p[0] > 0 && p[5] > 0 &&
                std::abs(p[11]) >= .5f && std::abs(p[15]) <= .001f &&
                std::abs(2.0f * std::atan(1.0f / p[5]) - camera.verticalFovRadians) <= .001f;
        }
    }

    void FsrGenerationHistory::ArmReset()
    {
        resetArmed_ = true; pendingPrepare_ = 0;
    }
    void FsrGenerationHistory::Invalidate() { ArmReset(); }
    void FsrGenerationHistory::AcknowledgePrepared(uint64_t sourceId)
    {
        if (pendingPrepare_ && pendingPrepare_ == sourceId && lastSource_ == sourceId) {
            resetArmed_ = false; pendingPrepare_ = 0;
        }
    }

    FsrGenerationDecision FsrGenerationHistory::Decide(const UpscaleFrame& frame, UpscaleOutcome outcome, bool uiComplete, bool menu, bool requested)
    {
        if (!frame.sourceId || (lastSource_ && frame.sourceId <= lastSource_)) {
            ArmReset(); return {false, false, true, false, "Duplicate or invalid source ID"};
        }
        const auto gap = lastSource_ && frame.sourceId - lastSource_ != 1;
        const auto changed = lastSource_ && (lastCamera_ != frame.camera.identity || lastRender_ != frame.render || lastDisplay_ != frame.display);
        lastSource_ = frame.sourceId; lastCamera_ = frame.camera.identity;
        lastRender_ = frame.render; lastDisplay_ = frame.display; pendingPrepare_ = 0;
        const auto suppress = [this](std::string_view reason) {
            ArmReset(); return FsrGenerationDecision{false, false, true, true, reason};
        };
        if (gap) return suppress("Source ID discontinuity");
        if (!requested) return suppress("Generation not requested");
        if (menu) return suppress("Menu or loading");
        if (outcome != UpscaleOutcome::Temporal) return suppress("Non-temporal source");
        if (!uiComplete) return suppress("UI not completed");
        if (!std::isfinite(frame.deltaMilliseconds) || frame.deltaMilliseconds <= 0) return suppress("Invalid source time");
        if (frame.deltaMilliseconds >= 100) return suppress("Source stalled");
        if ((frame.backend != BackendKind::Fsr && frame.backend != BackendKind::Dlss && frame.backend != BackendKind::Dlaa) ||
            !frame.depth || !frame.motion || !frame.render.width || !frame.render.height ||
            !frame.display.width || !frame.display.height || frame.render != frame.subrect ||
            frame.render.width > frame.display.width || frame.render.height > frame.display.height ||
            !frame.motionConvention.currentToPrevious || !std::isfinite(frame.motionConvention.scaleX) ||
            !std::isfinite(frame.motionConvention.scaleY) || frame.motionConvention.scaleX == 0 || frame.motionConvention.scaleY == 0)
            return suppress("Invalid temporal guides");
        if (!ValidCamera(frame.camera)) return suppress("Invalid camera");
        if (changed || frame.reset || frame.camera.reset) return suppress("Camera or extent reset");

        pendingPrepare_ = frame.sourceId;
        return {true, true, resetArmed_, true, {}};
    }
}
