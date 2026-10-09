#include "Upscaling/FSRGenerationPolicy.h"
#include "FrameGen/SourceGenerationPolicy.h"
#include <cstdio>
#include <cstdlib>

using namespace TheosRenderPipeline::Upscaling;

static void Require(bool value, const char* reason)
{
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", reason); std::exit(1); }
}

static UpscaleFrame Frame(std::uint64_t id, std::uint64_t epoch = 7)
{
    UpscaleFrame frame;
    frame.backend = BackendKind::Xess;
    frame.sourceId = id; frame.sourceEpoch = epoch;
    frame.render = frame.subrect = {1114, 627}; frame.display = {2560, 1440};
    frame.deltaMilliseconds = 1000.0f / 13.0f;
    frame.depth = reinterpret_cast<ID3D11Texture2D*>(1);
    frame.motion = reinterpret_cast<ID3D11Texture2D*>(2);
    frame.motionConvention = {1114, 627, true, false};
    frame.camera.identity = 7; frame.camera.nearDistance = .1f; frame.camera.farDistance = 1000;
    frame.camera.verticalFovRadians = 1.04719755f;
    frame.camera.view = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    frame.camera.projection = {1,0,0,0, 0,1.7320508f,0,0, 0,0,1.0001f,1, 0,0,-.10001f,0};
    return frame;
}

int main()
{
    FsrGenerationHistory history;
    auto decision = history.Decide(Frame(1), UpscaleOutcome::Temporal, true, false, true);
    Require(decision.prepare && decision.generate && decision.reset, "completed XeSS source eligible without FPS gate");
    history.AcknowledgePrepared(1);
    decision = history.Decide(Frame(2), UpscaleOutcome::Temporal, true, false, true);
    Require(decision.prepare && decision.generate && !decision.reset, "sequential XeSS source retains history");
    history.AcknowledgePrepared(2);
    Require(TheosRenderPipeline::SourceGenerationEnabled(0, true, decision.generate), "completed source can prepare NVIDIA FG");
    Require(!TheosRenderPipeline::SourceGenerationEnabled(0, true, decision.generate, true), "NVIDIA transition blocks generation");
    decision = history.Decide(Frame(2), UpscaleOutcome::Temporal, true, false, true);
    Require(!decision.admit && !decision.generate, "duplicate XeSS source never generates");
    for (auto outcome : {UpscaleOutcome::SpatialRecovery, UpscaleOutcome::SkippedInvalidInput, UpscaleOutcome::Fatal}) {
        FsrGenerationHistory recovery;
        decision = recovery.Decide(Frame(1), outcome, true, false, true);
        Require(!decision.prepare && !decision.generate, "recovery source never prepares FG");
    }
    for (unsigned condition = 0; condition < 3; ++condition) {
        FsrGenerationHistory blocked;
        decision = blocked.Decide(Frame(1), UpscaleOutcome::Temporal, condition != 0, condition == 1, condition != 2);
        Require(!decision.prepare && !decision.generate, "incomplete HUD, menu or FG off suppresses generation");
    }
    decision = history.Decide(Frame(3, 8), UpscaleOutcome::Temporal, true, false, true);
    Require(!decision.prepare && !decision.generate && decision.reset, "new source epoch cannot use old history");
    decision = history.Decide(Frame(4, 8), UpscaleOutcome::Temporal, true, false, true);
    Require(decision.prepare && decision.generate && decision.reset, "new epoch starts with reset preparation");
    history.AcknowledgePrepared(4);
    decision = history.Decide(Frame(5, 8), UpscaleOutcome::Temporal, true, false, false);
    Require(!decision.generate, "live FG off");
    decision = history.Decide(Frame(6, 8), UpscaleOutcome::Temporal, true, false, true);
    Require(decision.prepare && decision.generate && decision.reset, "live FG on starts clean history");
    auto invalid = Frame(7, 8); invalid.motion = nullptr;
    decision = history.Decide(invalid, UpscaleOutcome::Temporal, true, false, true);
    Require(!decision.generate && !decision.prepare, "missing temporal guides cannot prepare FG");
    return 0;
}
