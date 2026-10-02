#include "RendererBackendPolicy.h"
#if __has_include("Upscaling/FSRGenerationPolicy.h")
#include "Upscaling/FSRGenerationPolicy.h"
#define HAS_FG_POLICY
#endif
#include <cstdio>
#include <cstdlib>
#include <limits>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
static void Require(bool value, const char* why)
{ if (!value) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); } }
template<class Configuration> static BackendDecision Resolve(const Configuration& config, bool built, bool fgBuilt)
{
    if constexpr (requires { ResolveBackend(config, built, fgBuilt); }) return ResolveBackend(config, built, fgBuilt);
    else return ResolveBackend(config, built);
}
static void CheckBackend()
{
    BackendConfiguration config;
    Require(Resolve(config, false, false).valid, "LegacyGenerationValuesPreserved: NVIDIA backend 1");
    config.generationEnabled = false;
    Require(Resolve(config, false, false).valid && Resolve(config, false, false).presentation == PresentationKind::Nvidia, "NVIDIA off retains its presenter");
    config.backend = BackendKind::Fsr; config.generationBackend = 0;
    Require(Resolve(config, true, false).valid && Resolve(config, true, false).presentation == PresentationKind::Ordinary, "LegacyGenerationValuesPreserved: SR backend 0");
    config.generationBackend = 2; config.generationEnabled = true;
    const auto enabled = Resolve(config, true, true);
    Require(enabled.valid && enabled.generationEnabled && unsigned(enabled.presentation) == 2, "FsrFgRequiresBuild: matching FG capability accepted");
    Require(!Resolve(config, true, false).valid && !Resolve(config, false, true).valid, "FsrFgRequiresBuild: both build capabilities required");
    config.generationEnabled = false;
    Require(Resolve(config, true, true).valid && unsigned(Resolve(config, true, true).presentation) == 2, "FG off retains AMD presenter for live reentry");
    for (auto selector : {-1l, 1l, 3l, 99l}) { config.generationBackend = selector; Require(!Resolve(config, true, true).valid, "unknown or NVIDIA selector rejected for FSR"); }
    config.generationBackend = 0; config.generationEnabled = true;
    Require(!Resolve(config, true, true).valid, "ordinary SR cannot generate");
    config.generationBackend = 2;
    for (auto flag : {&BackendConfiguration::neuralRendering, &BackendConfiguration::hdr, &BackendConfiguration::dynamicResolution}) {
        config.*flag = true; Require(!Resolve(config, true, true).valid, "UnsupportedCombinationRejected: NR/HDR/dynamic resolution"); config.*flag = false;
    }
    config.providerPolicy = ProviderPolicy::Compatible;
    Require(!Resolve(config, true, true).valid, "FG initially requires analytical SR"); config.providerPolicy = ProviderPolicy::Analytical;
    for (auto backend : {BackendKind::Dlss, BackendKind::Dlaa, BackendKind::External}) {
        config.backend = backend; Require(!Resolve(config, true, true).valid, "UnsupportedCombinationRejected: other upscalers plus FSR FG");
    }
}
#ifdef HAS_FG_POLICY
static UpscaleFrame Frame(uint64_t id, float delta = 1000.0f / 90.0f)
{
    UpscaleFrame frame; frame.backend = BackendKind::Fsr; frame.sourceId = id; frame.deltaMilliseconds = delta;
    frame.render = frame.subrect = {1280, 720}; frame.display = {1920, 1080};
    frame.depth = reinterpret_cast<ID3D11Texture2D*>(1); frame.motion = reinterpret_cast<ID3D11Texture2D*>(2);
    frame.camera.identity = 7; frame.camera.nearDistance = .1f; frame.camera.farDistance = 1000;
    frame.camera.verticalFovRadians = 1.04719755f;
    frame.camera.view = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    frame.camera.projection = {1,0,0,0, 0,1.7320508f,0,0, 0,0,1.0001f,1, 0,0,-.10001f,0};
    frame.motionConvention = {1280, 720, true, false};
    return frame;
}
static void Warm(FsrGenerationHistory& history)
{
    for (uint64_t id = 1; id <= 15; ++id) {
        const auto decision = history.Decide(Frame(id), UpscaleOutcome::Temporal, true, false, true);
        Require(decision.admit, "valid sequential source admitted");
        Require(decision.prepare == (id == 15) && decision.generate == (id == 15), "ColdRateWindowSuppresses: 8 deltas plus 8 high means, first eligible source 15");
        if (id == 15) { Require(decision.reset, "first eligible preparation resets"); history.AcknowledgePrepared(id); }
    }
}
static void CheckHistory()
{
    {
        FsrGenerationHistory history; Warm(history);
        const auto stable = history.Decide(Frame(16), UpscaleOutcome::Temporal, true, false, true);
        Require(stable.prepare && stable.generate && !stable.reset, "successful preparation acknowledges reset history");
        auto repeated = history.Decide(Frame(16), UpscaleOutcome::Temporal, true, false, true);
        Require(!repeated.admit && !repeated.prepare, "DuplicateOrSkippedIdResets: duplicate rejected before Configure");
        auto older = history.Decide(Frame(15), UpscaleOutcome::Temporal, true, false, true);
        Require(!older.admit, "out-of-order source never admitted");
        auto gap = history.Decide(Frame(20), UpscaleOutcome::Temporal, true, false, true);
        Require(gap.admit && !gap.prepare && !gap.generate && gap.reset, "forward gap consumes one disabled source and resets reentry");
    }
    for (auto outcome : {UpscaleOutcome::SpatialRecovery, UpscaleOutcome::SkippedInvalidInput, UpscaleOutcome::Fatal}) {
        FsrGenerationHistory history; Warm(history);
        auto decision = history.Decide(Frame(16), outcome, true, false, true);
        Require(decision.admit && !decision.prepare && decision.reset, "SpatialOrMenuSuppresses: non-temporal source");
    }
    {
        FsrGenerationHistory history; Warm(history);
        Require(!history.Decide(Frame(16), UpscaleOutcome::Temporal, true, true, true).prepare, "SpatialOrMenuSuppresses: menu");
        Require(!history.Decide(Frame(17), UpscaleOutcome::Temporal, false, false, true).prepare, "UiIdentityIsNotCompletion: unfinished UI");
        Require(!history.Decide(Frame(18), UpscaleOutcome::Temporal, true, false, false).generate, "runtime request off");
        Require(!history.Decide(Frame(19), UpscaleOutcome::Temporal, true, false, true).prepare, "request reentry warms before Prepare");
    }
    for (auto delta : {0.0f, -1.0f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN(), 100.0f}) {
        FsrGenerationHistory history; Warm(history);
        auto decision = history.Decide(Frame(16, delta), UpscaleOutcome::Temporal, true, false, true);
        Require(!decision.prepare && decision.reset, "InvalidDeltaOrCameraRejected: invalid time or inclusive 100ms stall");
        Require(!history.Decide(Frame(17), UpscaleOutcome::Temporal, true, false, true).prepare, "StallResetsReentry: restart window");
    }
    {
        FsrGenerationHistory history; Warm(history);
        auto camera = Frame(16); camera.camera.view[0] = std::numeric_limits<float>::quiet_NaN();
        Require(!history.Decide(camera, UpscaleOutcome::Temporal, true, false, true).prepare, "InvalidDeltaOrCameraRejected: camera NaN");
        auto missing = Frame(17); missing.depth = nullptr;
        Require(!history.Decide(missing, UpscaleOutcome::Temporal, true, false, true).prepare, "missing depth suppresses");
        auto cut = Frame(18); cut.camera.reset = true;
        Require(!history.Decide(cut, UpscaleOutcome::Temporal, true, false, true).prepare, "camera cut resets window");
    }
    {
        FsrGenerationHistory history;
        for (uint64_t id = 1; id <= 15; ++id) history.Decide(Frame(id), UpscaleOutcome::Temporal, true, false, true);
        Require(history.Decide(Frame(16), UpscaleOutcome::Temporal, true, false, true).reset, "FailedPrepareDoesNotAcknowledge: no success acknowledgment");
        history.AcknowledgePrepared(15);
        Require(history.Decide(Frame(17), UpscaleOutcome::Temporal, true, false, true).reset, "stale success cannot acknowledge current source");
        history.AcknowledgePrepared(17);
        Require(!history.Decide(Frame(18), UpscaleOutcome::Temporal, true, false, true).reset, "current success acknowledges history");
        history.Invalidate(); history.AcknowledgePrepared(18);
        Require(!history.Decide(Frame(19), UpscaleOutcome::Temporal, true, false, true).prepare, "invalidation clears pending acknowledgment");
    }
    {
        FsrGenerationHistory history; Warm(history);
        for (uint64_t id = 16; id <= 24; ++id)
            Require(history.Decide(Frame(id, 1000.0f/59.0f), UpscaleOutcome::Temporal, true, false, true).generate, "LowRateHysteresis: retain until third low rolling mean");
        Require(!history.Decide(Frame(25, 1000.0f/59.0f), UpscaleOutcome::Temporal, true, false, true).generate, "third low mean suppresses");
        for (uint64_t id = 26; id <= 37; ++id)
            Require(!history.Decide(Frame(id, 1000.0f/70.0f), UpscaleOutcome::Temporal, true, false, true).generate, "resume requires eight successive >=66FPS means");
        auto reentry = history.Decide(Frame(38, 1000.0f/70.0f), UpscaleOutcome::Temporal, true, false, true);
        Require(reentry.generate && reentry.prepare && reentry.reset, "LowRateHysteresis: resume with reset at source 38");
    }
    {
        FsrGenerationHistory history; Warm(history);
        for (uint64_t id = 16; id <= 40; ++id)
            Require(history.Decide(Frame(id, 1000.0f/60.0f), UpscaleOutcome::Temporal, true, false, true).generate, "exact 60FPS floor does not suppress active generation");
    }
    {
        FsrGenerationHistory below;
        for (uint64_t id = 1; id <= 30; ++id)
            Require(!below.Decide(Frame(id, 1000.0f/65.9f), UpscaleOutcome::Temporal, true, false, true).generate, "below 66FPS cannot exit initial warmup");
        FsrGenerationHistory boundary;
        for (uint64_t id = 1; id <= 15; ++id)
            Require(boundary.Decide(Frame(id, 1000.0f/66.0f), UpscaleOutcome::Temporal, true, false, true).generate == (id == 15), "exact 66FPS resume boundary");
    }
}
#endif
#include "Upscaling/FSRGenerationStatus.h"
void CheckRuntimeStatus()
{
    using namespace TheosRenderPipeline::Upscaling;
    FsrGenerationDecision decision{};
    Require(DescribeFsrGenerationStatus(false,false,false,false,decision,0).kind==TheosRenderPipeline::SettingsStatusKind::Neutral,"ordinary presenter unavailable independently of SR");
    Require(DescribeFsrGenerationStatus(true,false,false,false,decision,0).text.find("off")!=std::string::npos,"FG off on AMD owner");
    Require(DescribeFsrGenerationStatus(true,true,false,false,decision,0).text.find("requested")!=std::string::npos,"requested before first submitted source");
    decision.reason="Source rate warmup";
    Require(DescribeFsrGenerationStatus(true,true,true,false,decision,0).text.find("warming")!=std::string::npos,"warmup distinct from request");
    decision.reason="Source rate suppressed";
    Require(DescribeFsrGenerationStatus(true,true,true,false,decision,0).text.find("rate suppressed")!=std::string::npos,"rate suppression explicit");
    decision.generate=true;decision.reason={};
    Require(DescribeFsrGenerationStatus(true,true,true,false,decision,0).kind!=TheosRenderPipeline::SettingsStatusKind::Success,"request alone cannot claim active generation");
    Require(DescribeFsrGenerationStatus(true,true,true,false,decision,1).kind==TheosRenderPipeline::SettingsStatusKind::Success,"observed callback active independently of scanout");
    Require(DescribeFsrGenerationStatus(true,true,true,true,decision,1).kind==TheosRenderPipeline::SettingsStatusKind::Error,"failure overrides previous callback");
}
int main()
{
    CheckBackend();
    CheckRuntimeStatus();
#ifdef HAS_FG_POLICY
    CheckHistory();
#else
    Require(false, "source/rate history is not implemented");
#endif
    std::puts("PASS: backend capability, source identity, suppression, preparation acknowledgment and rate hysteresis");
}
