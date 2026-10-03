#include "RendererBackendPolicy.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>

static_assert(DLSS == 0 && DLAA == 3 && FSR == 4,
    "Persisted upscaler selectors are part of the configuration ABI");

static void Require(bool ok, const char* why)
{
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}

int main()
{
    // Rejecting a supported ordinary FSR request because it lacks NVIDIA
    // ownership is the production bug this test catches.
    CSimpleIniA ini;
    auto validate = [&](bool built = true) { return TheosRenderPipeline::ValidateRendererConfiguration(ini, built); };
    Require(!validate(), "legacy defaults remain valid");
    for (long mode : {0L, 3L}) {
        ini.SetLongValue("Settings", "UpscaleType", mode);
        Require(!validate(), "legacy DLSS/DLAA remain valid");
    }
    for (long mode : {1L, 2L, -1L, 5L}) {
        ini.SetLongValue("Settings", "UpscaleType", mode);
        Require(validate(), "unknown/obsolete modes remain rejected");
    }
    ini.SetLongValue("Settings", "UpscaleType", 4);
    ini.SetBoolValue("FrameGeneration", "Enabled", false);
    ini.SetLongValue("Experimental", "FrameGenerationBackend", 0);
    ini.SetBoolValue("Experimental", "SourceDLSSGBackend", false);
    Require(!validate(), "ordinary FSR needs no NVIDIA owner");
    Require(validate(false), "unfinished/disabled FSR cannot become active");
    ini.SetBoolValue("Experimental", "SourceDLSSGBackend", true);
    Require(!validate(), "unused NVIDIA selector cannot force FSR ownership");
    for (auto key : {"EnableX3Presentation", "EnableXessCapabilityProbe", "EnableNeuralRenderingCapabilityProbe", "PureDarkFullDelegation"}) {
        ini.SetBoolValue("Experimental", key, true);
        Require(validate(), "obsolete experiments stay rejected for FSR");
        ini.SetBoolValue("Experimental", key, false);
    }
    for (auto section : {"SourceDLSSG", "HDROutput", "DynamicResolution"}) {
        const char* key = std::string_view(section) == "SourceDLSSG" ? "NeuralRenderingEnabled" : "Enabled";
        ini.SetBoolValue(section, key, true);
        Require(validate(), "unsupported FSR NR/HDR/dynamic resolution rejected");
        ini.SetBoolValue(section, key, false);
    }
    using namespace TheosRenderPipeline::Upscaling;
    BackendConfiguration config;
    config.backend = BackendKind::Fsr; config.generationEnabled = false; config.generationBackend = 0;
    const auto ordinary = TheosRenderPipeline::ResolveBackend(config, true);
    Require(ordinary.valid && ordinary.presentation == PresentationKind::Ordinary && !ordinary.generationEnabled,
        "FSR selection uses ordinary presentation with no generation");
    Require(!TheosRenderPipeline::ResolveBackend(config, false).valid, "FSR-off build rejects selection");
    for (unsigned mask = 1; mask < 16; ++mask) {
        config.generationEnabled = mask & 1; config.neuralRendering = mask & 2;
        config.hdr = mask & 4; config.dynamicResolution = mask & 8;
        const auto result = TheosRenderPipeline::ResolveBackend(config, true);
        Require(!result.valid && !result.diagnostic.empty(), "unsupported combinations have a reason");
    }
    for (long backend : {-1L, 1L, 2L}) {
        config = {}; config.backend = BackendKind::Fsr; config.generationEnabled = false; config.generationBackend = backend;
        Require(!TheosRenderPipeline::ResolveBackend(config, true).valid, "FSR rejects other presenter owners");
    }
    config={};config.backend=BackendKind::Fsr;config.generationBackend=2;config.neuralRendering=true;config.communityNeural=true;
    Require(TheosRenderPipeline::ResolveBackend(config,true,true).valid,"community NR can precede FSR FG");
    config.communityNeural=false;
    Require(!TheosRenderPipeline::ResolveBackend(config,true,true).valid,"legacy FSR NR remains unavailable");
    ini.SetBoolValue("SourceDLSSG","NeuralRenderingEnabled",true);
    ini.SetBoolValue("NeuralRendering","CommunityRuntime",true);
#if !defined(TRP_NO_NEURAL_RENDERING)
    Require(!validate(),"community FSR NR survives startup configuration validation");
#else
    Require(validate(),"NR-disabled builds reject saved FSR NR requests");
#endif
    std::puts("PASS: backend selection and legacy configuration");
}
