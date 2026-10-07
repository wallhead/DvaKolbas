#include "PublicIni.h"
#include "IniLayout.h"
#include "FrameGen/SourceFrameGeneration.h"
#include "Upscaling/FSRSettings.h"
#include "SettingsFile.h"
#include "RendererGpuPolicy.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>

void Check(bool value, const char* message)
{
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
void IndependentGenerationPreference()
{
    using namespace TheosRenderPipeline;
    for (const char* upscaler : {"DLSS", "FSR"}) {
        for (const char* preference : {"Auto", "NVIDIA", "FSR"}) {
            CSimpleIniA configured;
            configured.SetValue("Upscaling", "Upscaler", upscaler);
            configured.SetValue("FrameGeneration", "Backend", preference);
            Check(PublicIni::Decode(configured).empty(), "independent FG preference decodes");
            const long expected = std::string_view(preference) == "FSR" ? 2 :
                std::string_view(preference) == "NVIDIA" ? 1 : std::string_view(upscaler) == "FSR" ? 2 : 1;
            Check(configured.GetLongValue("FrameGeneration", "Backend", -1) == expected,
                "explicit FG choice overrides automatic upscaler selection");
            auto* owner = SourceFrameGeneration::GetSingleton();
            owner->LoadStartupPreferences(configured);
            owner->StoreInterpolationPreference(configured);
            Check(owner->settings.generationBackend == expected,
                "startup runtime readers retain independent presentation selection");
            Check(PublicIni::Encode(configured).empty() &&
                std::string_view(configured.GetValue("FrameGeneration", "Backend", "")) == preference,
                "saving preserves configured Auto or explicit backend rather than its effective value");
            Check(PublicIni::Decode(configured).empty() &&
                configured.GetLongValue("FrameGeneration", "Backend", -1) == expected,
                "independent backend survives restart");
        }
    }
    CSimpleIniA invalid;
    invalid.SetValue("Upscaling", "Upscaler", "DLSS");
    invalid.SetValue("FrameGeneration", "Backend", "Ordinary");
    Check(!PublicIni::Decode(invalid).empty(), "backend zero has no public normal choice");
}
int main(int argc, char** argv)
{
    using namespace TheosRenderPipeline;
    IndependentGenerationPreference();
    CSimpleIniA ini;
    ini.LoadData(R"ini(
[Upscaling]
Upscaler=DLSS
MipLodBias=Auto
[DLSS]
Quality=Native
; Keep the model preset guidance.
Preset=K
Sharpness=0.6
[FSR]
Quality=Native
Provider=FSR4
SourceColorEncoding=Gamma22
Sharpness=0.4
[FrameGeneration]
Enabled=false
FsrProvider=FSR4
NvidiaGeneratedFrames=2
[Latency]
Reflex=Boost
[Interface]
UIComposition=Dedicated
[NeuralRendering]
Enabled=true
Placement=After
[NeuralRendering Advanced]
Runtime=Community
SourceColorEncoding=Gamma22
SdrBytesTrial=true
[Hotkeys]
ToggleOverlay=End
[User Extras]
; Preserve this note.
MySetting=custom
)ini");
    Check(PublicIni::Decode(ini).empty(), "named settings decode successfully");
    Check(ini.GetLongValue("Settings", "UpscaleType", -1) == 3,
        "DLSS Native selects the internal DLAA path without a public DLAA mode");
    Check(ini.GetLongValue("DLSS", "Preset", -1) == 11 &&
        ini.GetLongValue("FrameGeneration", "Backend", -1) == 1,
        "named model preset and derived NVIDIA FG presenter reach the existing runtime");
    const auto fsr = Upscaling::ReadFsrSettings(ini);
    Check(fsr && fsr->quality == Upscaling::Quality::NativeAA &&
        fsr->providerPolicy == Upscaling::ProviderPolicy::MachineLearning &&
        fsr->generationProviderPolicy == Upscaling::ProviderPolicy::MachineLearning,
        "FSR Native and explicit ML choices remain distinct from Auto");
    auto* owner = SourceFrameGeneration::GetSingleton();
    owner->LoadStartupPreferences(ini);
    Check(owner->settings.generationBackend == 1 && !owner->RuntimeInterpolationRequested() &&
        owner->settings.neuralStartup.community && owner->settings.neuralStartup.sdrBytesTrial &&
        !owner->settings.sourceDLSSG.neuralBeforeUpscaling &&
        owner->settings.sourceDLSSG.generation.generatedFrames == 2,
        "existing NR and FG readers consume decoded named settings");
    Check(ini.GetLongValue("Hotkeys", "ToggleOverlay", -1) == 0x23,
        "named End key selects the existing menu hotkey");
    IniLayout::PrepareForUpdate(ini);
    owner->StoreInterpolationPreference(ini);
    SourceDLSSG::StorePreferences(ini, owner->settings.sourceDLSSG);
    IniLayout::StoreCanonical(ini);
    Check(PublicIni::Encode(ini).empty(), "runtime values encode back to the public layout");
    Check(!ini.GetValue("Settings", "UpscaleType", nullptr) &&
        std::string_view(ini.GetValue("FrameGeneration", "Backend", "")) == "Auto" &&
        std::string(ini.GetValue("DLSS", "Preset", "")) == "K" &&
        std::string(ini.GetValue("DLSS", "Quality", "")) == "Native" &&
        std::string(ini.GetValue("FrameGeneration", "FsrProvider", "")) == "FSR4",
        "Save persists configured Auto, not a derived backend or layout version");
    std::string saved;
    Check(ini.Save(saved) >= 0 && saved.find("Preserve this note") != std::string::npos,
        "unknown user setting comments survive save");
    Check(saved.find("Keep the model preset guidance") != std::string::npos,
        "known setting guidance also survives decoder and menu save");
    CSimpleIniA restart;
    restart.LoadData(saved.c_str());
    Check(PublicIni::Decode(restart).empty() && restart.GetLongValue("Settings", "UpscaleType", -1) == 3,
        "save and restart keep Native DLSS selected");
    restart.SetValue("Settings", "UpscaleType", "4");
    restart.SetValue("FrameGeneration", "Backend", "2");
    Check(PublicIni::Encode(restart).empty(), "switching to FSR encodes the requested upscaler");
    Check(std::string(restart.GetValue("DLSS", "Quality", "")) == "Native" &&
        PublicIni::Decode(restart).empty() && restart.GetLongValue("FrameGeneration", "Backend", -1) == 2,
        "FSR switch retains inactive NVIDIA Native and derives its own ready FG presenter");
    restart.SetBoolValue("FrameGeneration", "Enabled", true);
    ApplyRendererGpuPolicy(restart, 0x1002, true);
    const auto amdFsr = Upscaling::ReadFsrSettings(restart);
    Check(amdFsr && amdFsr->providerPolicy == Upscaling::ProviderPolicy::MachineLearning &&
        amdFsr->generationProviderPolicy == Upscaling::ProviderPolicy::MachineLearning &&
        restart.GetBoolValue("FrameGeneration", "Enabled", false) &&
        restart.GetLongValue("FrameGeneration", "Backend", -1) == 2 &&
        !restart.GetBoolValue("NeuralRendering", "Enabled", true),
        "AMD policy follows named decoding without weakening explicit FSR4 SR/FG requests");
    restart.SetLongValue("Settings", "UpscaleType", 0);
    restart.SetBoolValue("Settings", "DLSSNativeScale", false);
    restart.SetLongValue("DLSS", "QualityLevel", 2);
    Check(PublicIni::Encode(restart).empty() &&
        std::string(restart.GetValue("DLSS", "Quality", "")) == "Quality",
        "DLSS Quality must not accidentally encode as Native because both use internal quality index two");

    CSimpleIniA invalid;
    invalid.LoadData("[Upscaling]\nUpscaler=DLAA\n");
    Check(!PublicIni::Decode(invalid).empty(), "public DLAA is rejected; Native is the quality choice");
    invalid.Reset(); invalid.LoadData("[Upscaling]\nUpscaler=FSR\n[FSR]\nProvider=MachineLearning\n");
    Check(!PublicIni::Decode(invalid).empty(), "internal provider enum names are not old-layout aliases");
    invalid.Reset(); invalid.LoadData("[Settings]\nUpscaleType=3\n");
    Check(!PublicIni::Decode(invalid).empty(), "old layout cannot silently start with renderer defaults");
    for (const auto* bad : {"[Upscaling]\nUpscaler=FSR\nMipLodBias=banana\n",
        "[Upscaling]\nUpscaler=FSR\n[FrameGeneration]\nEnabled=maybe\n",
        "[Upscaling]\nUpscaler=FSR\n[FSR]\nSharpness=nan\n",
        "[Upscaling]\nUpscaler=FSR\n[Hotkeys]\nToggleOverlay=0\n",
        "[Upscaling]\nUpscaler=FSR\n[Upscaling Advanced]\nFsrOrdinaryPresenter=true\n"}) {
        invalid.Reset(); invalid.LoadData(bad);
        Check(!PublicIni::Decode(invalid).empty(), "invalid scalar/hotkey or incompatible ordinary FG is rejected");
    }
    for (int i = 1; i < argc; ++i) {
        CSimpleIniA file;
        const auto [result, error] = SettingsFile::LoadRenderer(file, std::filesystem::path(argv[i]).c_str());
        Check(result >= 0 && error.empty(), "production file loader accepts the generated public template");
        Check(file.GetLongValue("Settings", "UpscaleType", -1) >= 0,
            "production file loader decodes named choices before runtime readers use them");
    }
    std::puts("PASS: named settings, Native DLSS/FSR, explicit ML, derived FG, runtime readers and save/restart");
}
