#include "FrameGen/SourceFrameEvaluator.h"
#include <wrl/client.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
static void Require(bool ok, const char* why)
{ if (!ok) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); } }

struct Operations
{
    std::vector<std::string> events;
    bool optionalOK{true}, postOK{true}, upscaleOK{true}, effectsBefore{};
    unsigned effects{};
    UpscaleOutcome outcome{UpscaleOutcome::Temporal};
    GenerationPreparationStatus preparation{GenerationPreparationStatus::NotRequested};
    void CopyInput(ID3D11DeviceContext* context, const UpscaleFrame&)
    {
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> target;
        context->OMGetRenderTargets(1, &target, nullptr);
        Require(!target, "copy follows output unbinding"); events.push_back("copy");
    }
    bool EvaluateOptionalPreUpscale(UpscaleFrame&) { events.push_back("optional"); return optionalOK; }
    void RenderReShade(const UpscaleFrame&, bool before)
    { if (before == effectsBefore) { ++effects; events.push_back(before ? "effects-before" : "effects-after"); } }
    Result<UpscaleOutcome> EvaluateUpscaler(const UpscaleFrame&)
    {
        events.push_back("upscale");
        if (!upscaleOK) { return std::unexpected(RuntimeError{ErrorKind::DispatchFailure, -1, "test dispatch failed"}); }
        return outcome;
    }
    bool EvaluateOptionalPostUpscale(UpscaleFrame&, UpscaleOutcome outcome) { events.push_back("post"); Require(outcome==UpscaleOutcome::Temporal || outcome==UpscaleOutcome::SpatialRecovery,"post stage sees producer outcome"); return postOK; }
    void UpscaleSucceeded() { events.push_back("succeeded"); }
    GenerationPreparationStatus PrepareGeneration(const UpscaleFrame&) { events.push_back("prepare"); return preparation; }
};

struct CameraSnapshotOperations : Operations {
    Result<UpscaleOutcome> EvaluateUpscaler(UpscaleFrame& frame) { frame.camera.identity=42;return Operations::EvaluateUpscaler(frame); }
    bool EvaluateOptionalPostUpscale(UpscaleFrame& frame,UpscaleOutcome outcome) {
        Require(frame.camera.identity==42,"FSR After NR receives the measured source camera");return Operations::EvaluateOptionalPostUpscale(frame,outcome);
    }
    GenerationPreparationStatus PrepareGeneration(const UpscaleFrame& frame) {
        Require(frame.camera.identity==42,"FSR FG shares After NR's measured camera");return Operations::PrepareGeneration(frame);
    }
};

int main()
{
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Require(SUCCEEDED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &device, nullptr, &context)), "WARP context");
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = desc.SampleDesc.Count = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
    Require(SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &texture)) &&
        SUCCEEDED(device->CreateRenderTargetView(texture.Get(), nullptr, &rtv)), "bound source target");

    UpscaleFrame frame{}; frame.backend = BackendKind::Fsr;
    for(auto backend:{BackendKind::Fsr,BackendKind::Xess})for (bool before : {false, true}) {
        frame.backend=backend;
        ID3D11RenderTargetView* target = rtv.Get(); context->OMSetRenderTargets(1, &target, nullptr);
        Operations ops; ops.effectsBefore = before;
        const auto result = SourceFrameEvaluator::Evaluate(context.Get(), frame, ops);
        Require(result.outcome == UpscaleOutcome::Temporal && result.preparation == GenerationPreparationStatus::NotRequested,
            "NoGenerationIsNotFailure: ordinary SR keeps its completed temporal output");
        Require(ops.events == (before ? std::vector<std::string>{"copy", "optional", "effects-before", "upscale", "succeeded", "post", "prepare"} :
            std::vector<std::string>{"copy", "optional", "upscale", "succeeded", "effects-after", "post", "prepare"}), "one upscale/effects in source order");
        Require(ops.effects == 1, "one selected ReShade stage");
    }
    Operations external; frame.backend = BackendKind::External;
    SourceFrameEvaluator::Evaluate(nullptr, frame, external);
    Require(external.events.empty(), "external CS ownership runs no TRP temporal operation");
    frame.backend = BackendKind::Fsr;
    Operations failed; failed.upscaleOK = false;
    const auto failure = SourceFrameEvaluator::Evaluate(context.Get(), frame, failed);
    Require(failure.outcome == UpscaleOutcome::Fatal && failed.events == std::vector<std::string>{"copy", "optional", "upscale"},
        "failed temporal work does not acknowledge history or prepare generation");
    Operations optional; optional.optionalOK = false;
    SourceFrameEvaluator::Evaluate(context.Get(), frame, optional);
    Require(optional.events == std::vector<std::string>{"copy", "optional"}, "failed pre-upscale stops all consumers");
    Operations generation; generation.preparation = GenerationPreparationStatus::Failed;
    const auto realFrame = SourceFrameEvaluator::Evaluate(context.Get(), frame, generation);
    Require(realFrame.outcome == UpscaleOutcome::Temporal && realFrame.preparation == GenerationPreparationStatus::Failed,
        "failed generation preparation retains usable real frame");
    Operations recovery; recovery.outcome = UpscaleOutcome::SpatialRecovery;
    const auto recovered = SourceFrameEvaluator::Evaluate(context.Get(), frame, recovery);
    Require(recovered.outcome == UpscaleOutcome::SpatialRecovery && recovered.preparation == GenerationPreparationStatus::NotRequested,
        "spatial recovery never claims temporal generation readiness");
    Require(recovery.events == std::vector<std::string>{"copy", "optional", "upscale", "effects-after", "post"},
        "spatial output completes effects without preparing generation");
    Operations postFailed; postFailed.postOK=false;
    const auto unavailable=SourceFrameEvaluator::Evaluate(context.Get(),frame,postFailed);
    Require(unavailable.outcome==UpscaleOutcome::Fatal && postFailed.events.back()=="post",
        "post delivery failure stops generation and presentation");
    Operations fgOff;
    SourceFrameEvaluator::Evaluate(context.Get(),frame,fgOff);
    Require(fgOff.events[fgOff.events.size()-2]=="post","post stage runs even with FG off");
    CameraSnapshotOperations camera;SourceFrameEvaluator::Evaluate(context.Get(),frame,camera);
    std::puts("PASS: common source-frame ordering, ownership, and independent generation outcomes");
}
