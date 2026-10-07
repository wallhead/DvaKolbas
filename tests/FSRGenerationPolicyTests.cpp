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
    Require(Resolve(config, true, true).valid, "analytical FG is independent of the Auto SR provider");
    config.providerPolicy=ProviderPolicy::MachineLearning;
    Require(Resolve(config,true,true).valid,"analytical FG is independent of the ML SR provider");
    config.providerPolicy = ProviderPolicy::Analytical;
    for (auto backend : {BackendKind::Dlss, BackendKind::Dlaa, BackendKind::External}) {
        config.backend = backend;
        Require(Resolve(config, true, true).valid == (backend != BackendKind::External),
            "DLSS/DLAA permit FSR FG; unowned external sources remain rejected");
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
static void PrepareHistory(FsrGenerationHistory& history)
{
    for (uint64_t id = 1; id <= 15; ++id) {
        const auto decision = history.Decide(Frame(id), UpscaleOutcome::Temporal, true, false, true);
        Require(decision.admit, "valid sequential source admitted");
        Require(decision.prepare && decision.generate, "valid source requests generation without rate warmup");
        Require(decision.reset == (id == 1), "only initial preparation resets after acknowledged success");
        history.AcknowledgePrepared(id);
    }
}
static void CheckHistory()
{
    {
        FsrGenerationHistory history;
        auto first = history.Decide(Frame(1, 1000.0f / 30.0f), UpscaleOutcome::Temporal, true, false, true);
        Require(first.admit && first.prepare && first.generate && first.reset,
            "NoSourceRateGate: valid 30 FPS source immediately requests reset preparation");
    }
    {
        FsrGenerationHistory history; PrepareHistory(history);
        const auto stable = history.Decide(Frame(16), UpscaleOutcome::Temporal, true, false, true);
        Require(stable.prepare && stable.generate && !stable.reset, "successful preparation acknowledges reset history");
        auto repeated = history.Decide(Frame(16), UpscaleOutcome::Temporal, true, false, true);
        Require(!repeated.admit && !repeated.prepare, "DuplicateOrSkippedIdResets: duplicate rejected before Configure");
        auto older = history.Decide(Frame(15), UpscaleOutcome::Temporal, true, false, true);
        Require(!older.admit, "out-of-order source never admitted");
        auto gap = history.Decide(Frame(20), UpscaleOutcome::Temporal, true, false, true);
        Require(gap.admit && !gap.prepare && !gap.generate && gap.reset, "forward gap consumes one disabled source and resets reentry");
        const auto reentry = history.Decide(Frame(21), UpscaleOutcome::Temporal, true, false, true);
        Require(reentry.prepare && reentry.generate && reentry.reset, "sequential source after gap immediately prepares with reset");
    }
    for (auto outcome : {UpscaleOutcome::SpatialRecovery, UpscaleOutcome::SkippedInvalidInput, UpscaleOutcome::Fatal}) {
        FsrGenerationHistory history; PrepareHistory(history);
        auto decision = history.Decide(Frame(16), outcome, true, false, true);
        Require(decision.admit && !decision.prepare && decision.reset, "SpatialOrMenuSuppresses: non-temporal source");
        const auto reentry = history.Decide(Frame(17), UpscaleOutcome::Temporal, true, false, true);
        Require(reentry.prepare && reentry.generate && reentry.reset, "temporal recovery immediately prepares with reset");
    }
    {
        FsrGenerationHistory history; PrepareHistory(history);
        Require(!history.Decide(Frame(16), UpscaleOutcome::Temporal, true, true, true).prepare, "SpatialOrMenuSuppresses: menu");
        Require(!history.Decide(Frame(17), UpscaleOutcome::Temporal, false, false, true).prepare, "UiIdentityIsNotCompletion: unfinished UI");
        Require(!history.Decide(Frame(18), UpscaleOutcome::Temporal, true, false, false).generate, "runtime request off");
        const auto reentry = history.Decide(Frame(19), UpscaleOutcome::Temporal, true, false, true);
        Require(reentry.prepare && reentry.generate && reentry.reset, "request reentry immediately prepares with reset");
    }
    for (auto delta : {0.0f, -1.0f, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN(), 100.0f}) {
        FsrGenerationHistory history; PrepareHistory(history);
        auto decision = history.Decide(Frame(16, delta), UpscaleOutcome::Temporal, true, false, true);
        Require(!decision.prepare && decision.reset, "InvalidDeltaOrCameraRejected: invalid time or inclusive 100ms stall");
        const auto reentry = history.Decide(Frame(17), UpscaleOutcome::Temporal, true, false, true);
        Require(reentry.prepare && reentry.generate && reentry.reset, "StallResetsReentry: valid source resumes with reset");
    }
    {
        FsrGenerationHistory history; PrepareHistory(history);
        auto camera = Frame(16); camera.camera.view[0] = std::numeric_limits<float>::quiet_NaN();
        Require(!history.Decide(camera, UpscaleOutcome::Temporal, true, false, true).prepare, "InvalidDeltaOrCameraRejected: camera NaN");
        auto missing = Frame(17); missing.depth = nullptr;
        Require(!history.Decide(missing, UpscaleOutcome::Temporal, true, false, true).prepare, "missing depth suppresses");
        auto cut = Frame(18); cut.camera.reset = true;
        Require(!history.Decide(cut, UpscaleOutcome::Temporal, true, false, true).prepare, "camera cut rearms reset");
        const auto reentry = history.Decide(Frame(19), UpscaleOutcome::Temporal, true, false, true);
        Require(reentry.prepare && reentry.generate && reentry.reset, "valid source after camera cut immediately prepares with reset");
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
        const auto invalidated = history.Decide(Frame(19), UpscaleOutcome::Temporal, true, false, true);
        Require(invalidated.prepare && invalidated.generate && invalidated.reset, "invalidation clears pending acknowledgment and rearms reset");
    }
    for (auto fps : {20.0f, 30.0f, 45.0f, 59.0f, 60.0f, 65.9f, 66.0f, 90.0f}) {
        FsrGenerationHistory history;
        for (uint64_t id = 1; id <= 30; ++id) {
            const auto decision = history.Decide(Frame(id, 1000.0f / fps), UpscaleOutcome::Temporal, true, false, true);
            Require(decision.admit && decision.prepare && decision.generate && decision.reason.empty(), "valid low/high source rates never suppress generation");
            Require(decision.reset == (id == 1), "rate alone never rearms reset");
            history.AcknowledgePrepared(id);
        }
    }
    {
        FsrGenerationHistory history;
        for (uint64_t id = 1; id <= 40; ++id) {
            const auto decision = history.Decide(Frame(id, id % 2 ? 1000.0f / 30.0f : 1000.0f / 90.0f), UpscaleOutcome::Temporal, true, false, true);
            Require(decision.prepare && decision.generate && decision.reset == (id == 1), "fluctuating valid rate neither suppresses nor resets");
            history.AcknowledgePrepared(id);
        }
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
    decision.reason="Source stalled";
    Require(DescribeFsrGenerationStatus(true,true,true,false,decision,0).text.find("Source stalled")!=std::string::npos,"stall suppression remains explicit");
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
    Require(false, "source history is not implemented");
#endif
    std::puts("PASS: backend capability, source identity, safety suppression, preparation acknowledgment and unrestricted source rate");
}
