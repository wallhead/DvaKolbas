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
    CSimpleIniA xess;
    xess.SetValue("Upscaling", "Upscaler", "XeSS");
    xess.SetValue("FrameGeneration", "Backend", "NVIDIA");
    xess.SetBoolValue("FrameGeneration", "Enabled", true);
    Check(PublicIni::Decode(xess).empty() && xess.GetLongValue("FrameGeneration", "Backend", -1)==0 &&
        xess.GetLongValue("FrameGeneration", "BackendPreference", -1)==0 && !xess.GetBoolValue("FrameGeneration", "Enabled", true),
        "XeSS startup normalizes unavailable NVIDIA backend to Auto SR-only, matching menu");
    Check(PublicIni::Encode(xess).empty() && std::string_view(xess.GetValue("FrameGeneration", "Backend", ""))=="Auto",
        "normalized XeSS choice survives Save and restart");
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
void AutomaticNeuralRuntime()
{
    using namespace TheosRenderPipeline;
    for (const auto obsoleteChoice : {"Legacy", "Community"}) {
        CSimpleIniA ini;
        ini.SetValue("Upscaling", "Upscaler", "DLSS");
        ini.SetValue("NeuralRendering Advanced", "Runtime", obsoleteChoice);
        const auto notice = SettingsFile::LegacyRuntimeNotice(ini);
        Check(std::string_view(obsoleteChoice) != "Legacy" || notice.find("[Debug] NRLegacyRuntime") != std::string::npos,
            "retired Legacy preference produces an actionable notice before decoding");
        Check(PublicIni::Decode(ini).empty(), "obsolete NR selector cannot block named startup");
        auto* owner = SourceFrameGeneration::GetSingleton();
        owner->LoadStartupPreferences(ini);
        Check(owner->settings.neuralStartup.community && owner->settings.neuralStartup.profile == "Auto",
            "bundled NR automatically uses the rendering GPU catalog even with an old Legacy selector");
        ApplyRendererGpuPolicy(ini, 0x1002, true);
        owner->LoadStartupPreferences(ini);
        Check(!owner->settings.neuralStartup.community && !owner->settings.sourceDLSSG.neuralEnabled,
            "AMD has neither NR runtime nor NR enabled");
        Check(PublicIni::Encode(ini).empty() && !ini.GetValue("NeuralRendering Advanced", "Runtime", nullptr) &&
            !ini.GetValue("NeuralRendering", "CommunityRuntime", nullptr),
            "saving does not export a Legacy choice from the effective AMD policy");
        Check(PublicIni::Decode(ini).empty(), "automatic NR configuration survives saving");
        owner->LoadStartupPreferences(ini);
        Check(owner->settings.neuralStartup.community,
            "saved AMD policy cannot select Legacy on the next NVIDIA launch");
    }
    CSimpleIniA diagnostic;
    diagnostic.SetValue("Upscaling", "Upscaler", "DLSS");
    diagnostic.SetBoolValue("Debug", "NRLegacyRuntime", true);
    Check(PublicIni::Decode(diagnostic).empty(), "explicit Legacy diagnostic selector decodes");
    auto* owner = SourceFrameGeneration::GetSingleton();
    owner->LoadStartupPreferences(diagnostic);
    Check(!owner->settings.neuralStartup.community, "Legacy is available only through its explicit diagnostic switch");
    Check(PublicIni::Encode(diagnostic).empty() && diagnostic.GetBoolValue("Debug", "NRLegacyRuntime", false),
        "manual diagnostic override is retained separately from effective adapter policy");
}
int main(int argc, char** argv)
{
    using namespace TheosRenderPipeline;
    const auto absentPath=std::filesystem::temp_directory_path()/L"razkolbas-no-such-startup-ini-fixture.ini";
    Check(!std::filesystem::exists(absentPath),"missing INI fixture is absent");
    CSimpleIniA missing;std::string missingNotice="stale";
    const auto [missingResult,missingError]=SettingsFile::LoadRenderer(missing,absentPath.c_str(),&missingNotice);
    Check(missingResult<0 && missingError.find("matching INI")!=std::string::npos && missingNotice.empty(),
        "unreadable INI gives actionable failure without stale migration notice");
    const auto legacyPath=std::filesystem::temp_directory_path()/L"razkolbas-legacy-notice-fixture.ini";
    Check(!std::filesystem::exists(legacyPath),"legacy notice fixture is absent");
    CSimpleIniA previous;previous.SetValue("Upscaling","Upscaler","DLSS");previous.SetValue("NeuralRendering Advanced","Runtime","Legacy");
    Check(previous.SaveFile(legacyPath.c_str())>=0,"legacy notice fixture saved");
    CSimpleIniA loaded;std::string notice;
    const auto [legacyResult,legacyError]=SettingsFile::LoadRenderer(loaded,legacyPath.c_str(),&notice);
    std::filesystem::remove(legacyPath);
    Check(legacyResult>=0 && legacyError.empty() && notice.find("[Debug] NRLegacyRuntime")!=std::string::npos &&
        !loaded.GetValue("NeuralRendering Advanced","Runtime",nullptr),"file loader reports Legacy before decoder removes it");
    CSimpleIniA nvidiaXeSS;nvidiaXeSS.SetValue("Upscaling","Upscaler","XeSS");
    nvidiaXeSS.SetValue("FrameGeneration","Backend","NVIDIA");nvidiaXeSS.SetBoolValue("FrameGeneration","Enabled",true);
    Check(nvidiaXeSS.SaveFile(legacyPath.c_str())>=0,"XeSS notice fixture saved");
    CSimpleIniA normalized;const auto [normalizedResult,normalizedError]=SettingsFile::LoadRenderer(normalized,legacyPath.c_str(),&notice);
    std::filesystem::remove(legacyPath);
    Check(normalizedResult>=0 && normalizedError.empty() && notice.find("Backend=NVIDIA")!=std::string::npos &&
        notice.find("SR-only")!=std::string::npos && normalized.GetLongValue("FrameGeneration","Backend",-1)==0,
        "startup file loader logs actionable XeSS NVIDIA normalization before decoding");
    IndependentGenerationPreference();
    for(const auto* backend : {"Auto","FSR"}) {
        CSimpleIniA requested;requested.SetValue("Upscaling","Upscaler","XeSS");
        requested.SetValue("FrameGeneration","Backend",backend);requested.SetBoolValue("FrameGeneration","Enabled",true);
        Check(requested.SaveFile(legacyPath.c_str())>=0,"XeSS FG notice fixture saved");
        CSimpleIniA readBack;const auto [status,error]=SettingsFile::LoadRenderer(readBack,legacyPath.c_str(),&notice);
        std::filesystem::remove(legacyPath);
        Check(status>=0 && error.empty(),"XeSS FG notice fixture decodes");
        if(std::string_view(backend)=="Auto")
            Check(!readBack.GetBoolValue("FrameGeneration","Enabled",true) && notice.find("Backend=FSR")!=std::string::npos,
                "XeSS Auto FG-off normalization reports the requested FG change");
        else Check(readBack.GetBoolValue("FrameGeneration","Enabled",false) && notice.empty(),
            "explicit XeSS FSR FG request is retained without a normalization notice");
    }
    AutomaticNeuralRuntime();
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
        "[Upscaling]\nUpscaler=FSR\n[FrameGeneration]\nEnabled=true\n[Upscaling Advanced]\nFsrOrdinaryPresenter=true\n"}) {
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
