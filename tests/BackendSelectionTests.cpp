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
    Upscaling::BackendConfiguration nonRtx;
    nonRtx.adapterVendorId=0x10de;nonRtx.fsrOnlyRenderer=true;
    Require(!ResolveBackend(nonRtx,true,true).valid,"non-RTX cannot reenter NVIDIA presentation through typed settings");
    for (auto kind : {Upscaling::BackendKind::Dlss, Upscaling::BackendKind::Dlaa}) {
        Upscaling::BackendConfiguration mixed;
        mixed.backend = kind; mixed.generationBackend = 2; mixed.adapterVendorId = 0x10de;
        auto decision = ResolveBackend(mixed, false, true);
        Require(decision.valid && decision.presentation == Upscaling::PresentationKind::Fsr,
            "DLSS/DLAA select FSR FG without requiring an FSR SR build");
        mixed.hdr = true;
        Require(!ResolveBackend(mixed, true, true).valid, "FSR presentation rejects HDR even with DLSS SR");
        mixed.hdr = false; mixed.dynamicResolution = true;
        Require(!ResolveBackend(mixed, true, true).valid, "FSR presentation requires fixed DLSS sizing");
        mixed.dynamicResolution = false;
        Require(!ResolveBackend(mixed, true, false).valid, "mixed route requires an actual FSR FG build");
    }
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
    for(const char* provider : {"Compatible","MachineLearning"}) {
        fsr.SetValue("FSR","ProviderPolicy",provider);
        ApplyRendererGpuPolicy(fsr,0x1002,true);
        Require(std::string_view(fsr.GetValue("FSR","ProviderPolicy",""))==provider,"AMD startup preserves explicit FSR4/Auto choice");
    }
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
        Require(ResolveBackend(amd, true, true).valid, "AMD admits Auto; actual device/provider checks happen before publication");
    }
    CSimpleIniA gtx;
    gtx.SetLongValue("Settings","UpscaleType",DLAA);
    gtx.SetLongValue("FrameGeneration","Backend",1);
    gtx.SetBoolValue("FrameGeneration","Enabled",true);
    gtx.SetBoolValue("NeuralRendering","Enabled",true);
    gtx.SetValue("FSR","SourceColorEncoding","Gamma22");
    Require(ValidateRendererStartup(gtx,0x10de,true,true,true).empty(),"GTX fallback startup validation succeeds");
    const IniLayout::ReadView gtxView(gtx);
    Require(gtxView.GetLongValue("Settings","UpscaleType",-1)==FSR &&
        gtxView.GetLongValue("Experimental","FrameGenerationBackend",-1)==2 &&
        !gtxView.GetBoolValue("FrameGeneration","Enabled",true) &&
        !gtxView.GetBoolValue("SourceDLSSG","NeuralRenderingEnabled",true),
        "GTX1070 fallback replaces DLAA/NVIDIA presentation and clears incompatible NR/FG requests");
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
    ini.SetLongValue("FrameGeneration", "Backend", 0);
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
    ini.SetBoolValue("NeuralRendering", "CommunityRuntime", false); // Explicit Legacy diagnostic implementation.
    for (auto section : {"NeuralRendering", "HDROutput", "DynamicResolution"}) {
        const char* key = "Enabled";
        ini.SetBoolValue(section, key, true);
        Require(validate(), "unsupported FSR NR/HDR/dynamic resolution rejected");
        ini.SetBoolValue(section, key, false);
    }
    ini.Delete("NeuralRendering", "CommunityRuntime");
    ini.SetBoolValue("NeuralRendering", "Enabled", true);
    Require(!validate(), "automatic bundled NR is admitted with FSR before runtime/model checks");
    ini.SetBoolValue("NeuralRendering", "Enabled", false);
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
    ini.SetBoolValue("NeuralRendering","Enabled",true);
    ini.SetBoolValue("NeuralRendering","CommunityRuntime",true);
#if !defined(TRP_NO_NEURAL_RENDERING)
    Require(!validate(),"community FSR NR survives startup configuration validation");
#else
    Require(validate(),"NR-disabled builds reject saved FSR NR requests");
#endif
    std::puts("PASS: backend selection and legacy configuration");
}
