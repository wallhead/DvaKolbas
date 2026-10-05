#include "RendererBackendPolicy.h"
#include "RendererGpuPolicy.h"
#include "RendererStartupValidation.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>

static_assert(DLSS == 0 && DLAA == 3 && FSR == 4,
    "Persisted upscaler selectors are part of the configuration ABI");

static void Require(bool ok, const char* why)
{
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}

static void StartupNeuralPassLimit()
{
    for (const bool canonical : {false, true}) {
        CSimpleIniA startup;
        const char* section = canonical ? "NeuralRendering" : "SourceDLSSG";
        const char* enabled = canonical ? "Enabled" : "NeuralRenderingEnabled";
        const char* passes = canonical ? "PassCount" : "NRPasses";
        startup.SetBoolValue(section, enabled, true);
        startup.SetLongValue(section, passes, 2);
        auto validate = [&] { return TheosRenderPipeline::ValidateRendererConfiguration(startup, true, true); };
        Require(!validate(), "legacy startup permits two enabled NR passes");
        for (const long count : {3L, 5L}) {
            startup.SetLongValue(section, passes, count);
            Require(!validate(), "legacy startup keeps rendering with a saved community pass count");
            Require(startup.GetLongValue(section, passes, 0) == count,
                "legacy startup validation preserves the saved pass count");
        }
        startup.SetLongValue(section, passes, 3);
        startup.SetBoolValue(section, enabled, false);
        Require(!validate(), "disabled legacy NR keeps an inactive three-pass preference");
        startup.SetBoolValue(section, enabled, true);
        startup.SetBoolValue("NeuralRendering", "CommunityRuntime", true);
        Require(!validate(), "community startup accepts three enabled passes");
        startup.SetLongValue("Settings", "UpscaleType", FSR);
        startup.SetLongValue("FrameGeneration", "Backend", 0);
        startup.SetBoolValue("FrameGeneration", "Enabled", false);
#if !defined(TRP_NO_NEURAL_RENDERING)
        Require(!validate(), "community three-pass startup works with ordinary FSR");
#endif
        Require(TheosRenderPipeline::ValidateRendererConfiguration(startup, false, true),
            "community three-pass startup still obeys FSR build availability");
    }
    CSimpleIniA mixed;
    mixed.LoadData("[SourceDLSSG]\nNeuralRenderingEnabled=true\nNRPasses=3\n[NeuralRendering]\nEnabled=true\nPassCount=2\n");
    Require(!TheosRenderPipeline::ValidateRendererConfiguration(mixed, true),
        "canonical two-pass value overrides an unsupported stale legacy count");
    mixed.SetLongValue("NeuralRendering", "PassCount", 3);
    mixed.SetBoolValue("NeuralRendering", "Enabled", false);
    Require(!TheosRenderPipeline::ValidateRendererConfiguration(mixed, true),
        "canonical disabled value overrides an enabled stale legacy request");
}

int main()
{
    using namespace TheosRenderPipeline;
    for (const long mode : {0L, 3L, 4L}) {
        CSimpleIniA amd;
        amd.SetLongValue("Settings", "UpscaleType", mode);
        amd.SetLongValue("FrameGeneration", "Backend", 1);
        amd.SetBoolValue("FrameGeneration", "Enabled", true);
        amd.SetBoolValue("NeuralRendering", "Enabled", true);
        amd.SetBoolValue("DynamicResolution", "Enabled", true);
        amd.SetBoolValue("DynamicResolution", "Oscillate", true);
        amd.SetBoolValue("HDROutput", "Enabled", true);
        amd.SetValue("FSR", "SourceColorEncoding", "Gamma22");
        Require(ValidateRendererStartup(amd, 0x1002, true, true).empty(), "AMD normalizes saved incompatible choices before startup validation");
        const IniLayout::ReadView read(amd);
        Require(read.GetLongValue("Settings", "UpscaleType", -1) == FSR, "AMD forces FSR for all saved upscalers");
        Require(read.GetLongValue("Experimental", "FrameGenerationBackend", -1) == 2, "AMD uses available FSR FG presenter");
        Require(!read.GetBoolValue("FrameGeneration", "Enabled", true), "AMD does not activate a saved NVIDIA FG request");
        Require(!read.GetBoolValue("SourceDLSSG", "NeuralRenderingEnabled", true), "AMD disables saved NR");
        Require(!read.GetBoolValue("DynamicResolution", "Enabled", true) && !read.GetBoolValue("DynamicResolution", "Oscillate", true), "AMD reload clears unsupported dynamic resolution requests");
        Require(!read.GetBoolValue("HDROutput", "Enabled", true), "AMD FSR route disables NVIDIA HDR output");
        Require(std::string_view(read.GetValue("FSR", "SourceColorEncoding", "")) == "Gamma22", "GPU policy preserves explicit color encoding");
        Require(!ValidateRendererConfiguration(amd, true, true), "normalized AMD startup passes ordinary configuration validation");
    }
    CSimpleIniA fsr;
    fsr.SetLongValue("Settings", "UpscaleType", FSR);
    fsr.SetLongValue("FrameGeneration", "Backend", 2);
    fsr.SetBoolValue("FrameGeneration", "Enabled", true);
    ApplyRendererGpuPolicy(fsr, 0x1002, true);
    Require(fsr.GetBoolValue("FrameGeneration", "Enabled", false), "AMD keeps an existing FSR FG request");
    ApplyRendererGpuPolicy(fsr, 0x1002, false);
    Require(fsr.GetLongValue("FrameGeneration", "Backend", -1) == 0 && !fsr.GetBoolValue("FrameGeneration", "Enabled", true), "AMD without FG build uses ordinary FSR");
    CSimpleIniA nvidia;
    nvidia.SetLongValue("Settings", "UpscaleType", DLAA);
    ApplyRendererGpuPolicy(nvidia, 0x10de, true);
    Require(nvidia.GetLongValue("Settings", "UpscaleType", -1) == DLAA, "NVIDIA settings remain unchanged");
    CSimpleIniA incompatible;
    incompatible.SetLongValue("Settings", "UpscaleType", FSR);
    incompatible.SetLongValue("FrameGeneration", "Backend", 1);
    incompatible.SetValue("FSR", "SourceColorEncoding", "Gamma22");
    Require(!ValidateRendererStartup(incompatible, 0x10de, true, true).empty(), "NVIDIA still rejects incompatible saved FSR/NVIDIA presenter");
    Require(ValidateRendererStartup(incompatible, 0x1002, true, true).empty(), "AMD normalizes same incompatible saved presenter before validation");
    incompatible.SetValue("FSR", "SourceColorEncoding", "Unknown");
    const auto encodingError = ValidateRendererStartup(incompatible, 0x1002, true, true);
    Require(!encodingError.empty(), "AMD startup never guesses unknown FSR source encoding");
    Require(encodingError.find("[FSR]") != std::string::npos && encodingError.find("RaZkolbaS.ini") != std::string::npos,
        "first-launch encoding error identifies the INI and section to edit");
    incompatible.SetValue("FSR", "SourceColorEncoding", "Gamma22");
    Require(!ValidateRendererStartup(incompatible, 0x1002, false, true).empty(), "AMD rejects build without FSR instead of using NVIDIA");
    for (const auto mode : {Upscaling::BackendKind::Dlss, Upscaling::BackendKind::Dlaa, Upscaling::BackendKind::Fsr}) {
        Upscaling::BackendConfiguration amd;
        amd.adapterVendorId = 0x1002;
        amd.backend = mode;
        amd.generationBackend = 1;
        Require(!ResolveBackend(amd, true, true).valid, "AMD rejects NVIDIA presentation even with FSR");
        amd.generationBackend = 2;
        Require(ResolveBackend(amd, true, true).valid == (mode == Upscaling::BackendKind::Fsr), "AMD accepts only FSR with FSR FG");
        amd.backend = Upscaling::BackendKind::Fsr;
        amd.neuralRendering = amd.communityNeural = true;
        Require(!ResolveBackend(amd, true, true).valid, "AMD rejects NR independently of provider normalization");
        amd.neuralRendering = amd.communityNeural = false;
        amd.generationBackend = 0;
        amd.generationEnabled = false;
        amd.providerPolicy = Upscaling::ProviderPolicy::Compatible;
        Require(!ResolveBackend(amd, true, true).valid, "AMD rejects unqualified provider selection instead of silently overwriting it");
    }
    StartupNeuralPassLimit();
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
    ini.Delete("FrameGeneration", "Enabled");
    Require(!validate(), "missing interpolation preference defaults off for ordinary FSR");
    ini.SetBoolValue("FrameGeneration", "Enabled", true);
    Require(validate(), "explicit interpolation on remains invalid for ordinary FSR");
    ini.SetBoolValue("FrameGeneration", "Enabled", false);
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
