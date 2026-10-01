#pragma once

#include "Upscaling/UpscalerBackend.h"
#include <d3d11.h>

namespace TheosRenderPipeline
{
    struct SourceFrameResult
    {
        Upscaling::UpscaleOutcome outcome{Upscaling::UpscaleOutcome::Fatal};
        Upscaling::GenerationPreparationStatus preparation{Upscaling::GenerationPreparationStatus::NotRequested};
    };

    class SourceFrameEvaluator
    {
    public:
        template<class Operations>
        static SourceFrameResult Evaluate(ID3D11DeviceContext* context,
            Upscaling::UpscaleFrame frame, Operations& operations)
        {
            using namespace Upscaling;
            if (frame.backend == BackendKind::External) { return {UpscaleOutcome::SkippedInvalidInput, GenerationPreparationStatus::NotRequested}; }
            if (!context) { return {}; }
            context->OMSetRenderTargets(0, nullptr, nullptr);
            operations.CopyInput(context, frame);
            if (!operations.EvaluateOptionalPreUpscale(frame)) { return {}; }
            operations.RenderReShade(frame, true);
            const auto upscale = operations.EvaluateUpscaler(frame);
            if (!upscale) { return {}; }
            if (*upscale != UpscaleOutcome::Temporal && *upscale != UpscaleOutcome::SpatialRecovery) {
                return {*upscale, GenerationPreparationStatus::NotRequested};
            }
            // Only accepted temporal work may acknowledge temporal history.
            if (*upscale == UpscaleOutcome::Temporal) { operations.UpscaleSucceeded(); }
            operations.RenderReShade(frame, false);
            // A real reconstructed frame is usable even when generation is off
            // or preparation fails. Spatial recovery must never arm generation.
            return {*upscale, *upscale == UpscaleOutcome::Temporal ? operations.PrepareGeneration(frame) :
                GenerationPreparationStatus::NotRequested};
        }
    };
}
