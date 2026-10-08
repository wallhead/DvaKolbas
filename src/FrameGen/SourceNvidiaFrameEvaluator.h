#pragma once

#include "SourceNvidiaFramePreparation.h"
#include "SourceFrameEvaluator.h"
#include "D3D11FrameCopy.h"

namespace TheosRenderPipeline
{
    // Snapshot used by TRP's reconstruction stage. The host retains resource
    // ownership; per-frame NR reset changes stay local to the evaluation copy.
    struct SourceNvidiaFrameInputs : SourceNvidiaFrameGuides
    {
        ID3D11Texture2D* color{};
        ID3D11Texture2D* input{};
        ID3D11Texture2D* output{};
        float sharpness{}, motionScaleX{}, motionScaleY{};
        Upscaling::UpscaleFrame sourceSnapshot{};
    };

    struct SourceNvidiaFrameResult
    {
        bool upscaled{}, cameraValid{}, prepared{};
    };

    class SourceNvidiaFrameEvaluator
    {
        template<class Operations> struct Adapter
        {
            SourceNvidiaFrameInputs frame;
            Operations& operations;
            bool cameraValid{};
            void CopyInput(ID3D11DeviceContext* context, const Upscaling::UpscaleFrame&)
            { operations.CopyInput(context, frame); }
            bool EvaluateOptionalPreUpscale(Upscaling::UpscaleFrame&)
            { return operations.EvaluateNeuralBeforeDLSS(frame); }
            void RenderReShade(const Upscaling::UpscaleFrame&, bool before)
            { operations.RenderReShade(frame, before); }
            Upscaling::Result<Upscaling::UpscaleOutcome> EvaluateUpscaler(const Upscaling::UpscaleFrame&)
            {
                if (operations.EvaluateDLSS(frame)) { return Upscaling::UpscaleOutcome::Temporal; }
                return std::unexpected(Upscaling::RuntimeError{Upscaling::ErrorKind::DispatchFailure, 0, "DLSS evaluation failed"});
            }
            bool EvaluateOptionalPostUpscale(Upscaling::UpscaleFrame&, Upscaling::UpscaleOutcome outcome)
            { return operations.EvaluateNeuralAfterDLSS(frame, outcome); }
            void UpscaleSucceeded() { operations.UpscaleSucceeded(); }
            Upscaling::GenerationPreparationStatus PrepareGeneration(const Upscaling::UpscaleFrame& completed)
            {
                if constexpr (requires { operations.PrepareGeneration(frame, completed); }) {
                    auto snapshot = frame.sourceSnapshot;
                    snapshot.output = frame.output; snapshot.depth = frame.depth; snapshot.motion = frame.motion;
                    snapshot.render = {frame.renderWidth, frame.renderHeight};
                    snapshot.display = {frame.outputWidth, frame.outputHeight}; snapshot.reset |= frame.reset;
                    const auto status = operations.PrepareGeneration(frame, snapshot);
                    cameraValid = status == Upscaling::GenerationPreparationStatus::Succeeded;
                    return status;
                } else {
                    // NVIDIA preparation also serves after-upscale NR with FG off.
                    const auto result = SourceNvidiaFramePreparation::PrepareCompletedFrame(frame, operations);
                    cameraValid = result.cameraValid;
                    return result.prepared ? Upscaling::GenerationPreparationStatus::Succeeded : Upscaling::GenerationPreparationStatus::Failed;
                }
            }
        };
    public:
        template<class Operations>
        static SourceNvidiaFrameResult Evaluate(ID3D11DeviceContext* context,
            SourceNvidiaFrameInputs frame, Operations& operations)
        {
            if constexpr (requires { operations.ExternalGuideRecoveryEnabled(); operations.RecoverInvalidSourceGuides(frame); }) {
                const auto matches=[&](ID3D11Texture2D* guide,DXGI_FORMAT format){
                    if(!guide)return false;D3D11_TEXTURE2D_DESC desc{};guide->GetDesc(&desc);
                    Microsoft::WRL::ComPtr<ID3D11Device> expected,actual;context->GetDevice(&expected);guide->GetDevice(&actual);
                    return desc.Width==frame.renderWidth && desc.Height==frame.renderHeight && desc.Format==format && desc.SampleDesc.Count==1 &&
                        D3D11FrameCopy::SameObject(expected.Get(),actual.Get());
                };
                if(context && operations.ExternalGuideRecoveryEnabled() && (!matches(frame.depth,DXGI_FORMAT_R32_FLOAT) || !matches(frame.motion,DXGI_FORMAT_R16G16_FLOAT))) {
                    context->OMSetRenderTargets(0,nullptr,nullptr);operations.CopyInput(context,frame);
                    const auto recovered=operations.RecoverInvalidSourceGuides(frame);
                    return {recovered,false,recovered};
                }
            }
            Adapter<Operations> adapter{frame, operations};
            Upscaling::UpscaleFrame common = frame.sourceSnapshot;
            common.color = frame.color; common.input = frame.input; common.output = frame.output;
            common.depth = frame.depth; common.motion = frame.motion;
            common.render = {frame.renderWidth, frame.renderHeight};
            common.display = {frame.outputWidth, frame.outputHeight};
            common.jitterX = frame.jitterX; common.jitterY = frame.jitterY;
            common.reset = frame.reset; common.sharpness = frame.sharpness;
            const auto result = SourceFrameEvaluator::Evaluate(context, common, adapter);
            return {result.outcome == Upscaling::UpscaleOutcome::Temporal, adapter.cameraValid,
                result.preparation == Upscaling::GenerationPreparationStatus::Succeeded};
        }
    };
}
