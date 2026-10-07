#include "FrameGen/SourceFrameGeneration.h"
#include "RendererBackendPolicy.h"
#include "OverlayLayout.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

static void Check(bool value, const char* message)
{
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

int main(int argc, char** argv)
{
    using namespace TheosRenderPipeline;
    CSimpleIniA obsolete;
    obsolete.LoadData("[Settings]\nQualityLevel=4\nDLSSPreset=5\nConfigVersion=1\n[SourceDLSSG]\nNeuralRenderingEnabled=true\nNRStyle=7\nGeneratedFrames=5\n[Experimental]\nFrameGenerationBackend=2\nNeuralRenderingRuntimePath=Old/NR.dll\n[NeuralRendering]\nDriverCore=Old/core.dll\n");
    const auto ignored = SourceDLSSG::LoadPreferences(obsolete);
    Check(!ignored.neuralEnabled && ignored.neuralTuning.style == 0 && ignored.generation.generatedFrames == 1,
        "obsolete NR and FG keys must not configure the current renderer");
    RuntimePathSettings obsoletePaths;
    obsoletePaths.Load(obsolete);
    Check(obsoletePaths.neural.empty(), "obsolete runtime path must not be loaded");
    IniLayout::PrepareForUpdate(obsolete);
    IniLayout::StoreCanonical(obsolete);
    Check(!obsolete.GetValue("Settings", "ConfigVersion", nullptr) &&
        !obsolete.GetValue("NR PASS 1", "Style", nullptr),
        "save must not migrate obsolete keys or write a layout version");
    CSimpleIniA ini;
    Check(ini.LoadData(R"ini(
[Settings]
UpscaleType=4
[FrameGeneration]
Backend=2
Enabled=false
GeneratedFrames=3
UIRecomposition=false
[NeuralRendering]
Enabled=true
BeforeUpscaling=false
StableColors=false
PassCount=3
CommunityRuntime=true
[NR PASS 1]
Style=1
Intensity=0.4
Tone=0.7
Structure=0.8
AutoSkin=true
Preset=1
InputScale=0.75
[NR PASS 2]
UseSameSettings=false
Style=2
Intensity=0.3
InputScale=0.5
[NR PASS 3]
UseSameSettings=false
Style=3
Intensity=0.2
InputScale=0.25
[Runtime]
StreamlineDirectory=Custom/SL
NRRuntimePath=Custom/NR.dll
NRDriverCore=Custom/core.dll
[Menu]
WindowX=123
WindowWidth=987
[FSR]
ProviderPolicy=Analytical
[SourceDLSSG]
NeuralRenderingEnabled=false
NRStyle=0
NRLocalTone=0.1
[Experimental]
FrameGenerationBackend=1
)ini") >= 0, "parse new-format fixture");
    const auto nr = SourceDLSSG::LoadPreferences(ini);
    Check(nr.neuralPasses == 3, "canonical community preference retains three requested passes");
    Check(nr.neuralEnabled && !nr.neuralBeforeUpscaling,
        "canonical NR switches override stale legacy values");
    Check(nr.neuralTuning.style == 1 && nr.neuralTuning.intensity == .4f &&
        nr.neuralTuning.localToneStrength == .7f && nr.neuralTuning.localStructureStrength == .8f &&
        nr.neuralTuning.useAutoSkinMask, "read named first-pass tuning");
    Check(nr.neuralReconstruction.preset == 1 && nr.neuralReconstruction.inputScale == .75f,
        "first-pass resource choices survive the new layout");
    Check(!nr.neuralSecondPass.linked && nr.neuralSecondPass.tuning.style == 2 &&
        nr.neuralSecondPass.tuning.intensity == .3f && nr.neuralSecondPass.inputScale == .5f,
        "second-pass overrides remain independent");
    Check(!nr.neuralThirdPass.linked && nr.neuralThirdPass.tuning.style == 3 &&
        nr.neuralThirdPass.tuning.intensity == .2f && nr.neuralThirdPass.inputScale == .25f,
        "third-pass overrides remain independent of the first two passes");
    Check(nr.generation.generatedFrames == 3 && !nr.uiRecomposition,
        "FG preferences are independent of NR");
    auto& owner = *SourceFrameGeneration::GetSingleton();
    owner.LoadStartupPreferences(ini);
    Check(owner.settings.generationBackend == 2 && !owner.RuntimeInterpolationRequested(),
        "new presenter and explicit FG-off are respected before startup");
    Check(owner.settings.sourceDLSSGStreamlineDirectory == "Custom/SL" &&
        owner.settings.neuralRenderingRuntimePath == "Custom/NR.dll" &&
        owner.settings.neuralStartup.driverCore == std::filesystem::path("Custom/core.dll"),
        "startup finds separated runtime paths");
    Check(ValidateRendererConfiguration(ini, true, true) == nullptr,
        "startup validation uses the same canonical settings as the renderer");
    const auto layout = Overlay::LoadLayout(ini);
    Check(layout.x == 123 && layout.width == 987, "menu restores its new section");
    // Exercise the actual menu-save sequence, including an on-disk startup edit.
    ini.SetValue("Runtime", "NRRuntimePath", "Edited/NR.dll");
    ini.SetValue("Experimental", "KeepUnknown", "user-value");
    ini.SetValue("SourceDLSSG", "FutureSetting", "42");
    ini.SetBoolValue("SourceDLSSG", "NRStableColors", true);
    IniLayout::PrepareForUpdate(ini);
    owner.StoreRuntimePaths(ini);
    auto changed = nr;
    changed.neuralTuning.style = 0;
    changed.neuralTuning.localToneStrength = .9f;
    changed.neuralThirdPass.tuning.style = 4;
    SourceDLSSG::StorePreferences(ini, changed);
    owner.StoreInterpolationPreference(ini);
    Overlay::StoreLayout(ini, layout);
    IniLayout::StoreCanonical(ini);
    Check(!ini.GetValue("Settings", "ConfigVersion", nullptr), "current save does not write ConfigVersion");
    Check(!ini.GetValue("SourceDLSSG", "NRStyle", nullptr) &&
        !ini.GetValue("Experimental", "FrameGenerationBackend", nullptr),
        "save removes migrated keys so edits cannot conflict");
    Check(!ini.GetValue("SourceDLSSG", "NRStableColors", nullptr) &&
        !ini.GetValue("NeuralRendering", "StableColors", nullptr), "save removes both retired color settings");
    Check(ini.GetLongValue("NR PASS 3", "Style", -1) == 4 &&
        !ini.GetValue("SourceDLSSG", "NRPass3Style", nullptr), "save writes third-pass canonical section");
    Check(ini.GetLongValue("NR PASS 1", "Style", -1) == 0 &&
        std::abs(ini.GetDoubleValue("NR PASS 1", "Tone", -1) - .9) < .000001,
        "applied values replace old canonical values");
    Check(std::string(ini.GetValue("Runtime", "NRRuntimePath", "")) == "Edited/NR.dll",
        "save preserves startup path edited on disk since launch");
    Check(std::string(ini.GetValue("Experimental", "KeepUnknown", "")) == "user-value" &&
        ini.GetLongValue("SourceDLSSG", "FutureSetting", 0) == 42,
        "migration preserves unknown keys and their sections");
    std::string saved;
    Check(ini.Save(saved) >= 0, "serialize migrated settings");
    CSimpleIniA restart;
    Check(restart.LoadData(saved.c_str()) >= 0 && SourceDLSSG::LoadPreferences(restart) == changed,
        "save and restart retain all NR/FG preferences");
    // A missing pass-2 option inherits pass 1, while an explicit false wins.
    CSimpleIniA partial;
    partial.LoadData("[NR PASS 1]\nInputScale=0.5\nIntensity=0.6\n[NR PASS 2]\nUseSameSettings=false\n");
    const auto inherited = SourceDLSSG::LoadPreferences(partial);
    Check(!inherited.neuralSecondPass.linked && inherited.neuralSecondPass.inputScale == .5f &&
        inherited.neuralSecondPass.tuning.intensity == .6f, "partial new INIs retain inheritance");
    Check(inherited.neuralThirdPass.linked && inherited.neuralThirdPass.inputScale == .5f &&
        inherited.neuralThirdPass.tuning.intensity == .6f, "missing pass three follows first-pass defaults");
    partial.SetBoolValue("SourceDLSSG", "NRStableColors", false);
    partial.SetBoolValue("NeuralRendering", "StableColors", true);
    Check(SourceDLSSG::LoadPreferences(partial) == inherited, "retired color options cannot alter loaded preferences");
    IniLayout::StoreCanonical(partial);
    Check(!partial.GetValue("SourceDLSSG", "NRStableColors", nullptr) &&
        !partial.GetValue("NeuralRendering", "StableColors", nullptr), "migration removes retired keys without a preference writer");
    // Never substitute a legacy nonempty path for an explicitly empty new path.
    partial.SetValue("Runtime", "NRRuntimePath", "");
    partial.SetValue("Experimental", "NeuralRenderingRuntimePath", "Old/NR.dll");
    RuntimePathSettings paths;
    paths.Load(partial);
    Check(paths.neural.empty(), "explicit empty new runtime path overrides legacy path");
    for (int index = 1; index < argc; ++index) {
        CSimpleIniA current;
        Check(current.LoadFile(argv[index]) >= 0, "load actual INI fixture");
        const auto before = SourceDLSSG::LoadPreferences(current);
        owner.LoadStartupPreferences(current);
        const auto backend = owner.settings.generationBackend;
        const auto runtimePaths = owner.settings.configuredRuntimePaths;
        const auto startup = owner.settings.neuralStartup;
        const auto menu = Overlay::LoadLayout(current);
        IniLayout::PrepareForUpdate(current);
        IniLayout::StoreCanonical(current);
        std::string migrated;
        Check(current.Save(migrated) >= 0, "serialize actual migrated fixture");
        CSimpleIniA reloaded;
        Check(reloaded.LoadData(migrated.c_str()) >= 0, "reload actual migrated fixture");
        Check(SourceDLSSG::LoadPreferences(reloaded) == before, "actual NR/FG values unchanged by migration");
        owner.LoadStartupPreferences(reloaded);
        Check(owner.settings.generationBackend == backend &&
            owner.settings.configuredRuntimePaths.streamline == runtimePaths.streamline &&
            owner.settings.configuredRuntimePaths.neural == runtimePaths.neural &&
            owner.settings.neuralStartup.driverCore == startup.driverCore &&
            owner.settings.neuralStartup.runtimeRoot == startup.runtimeRoot &&
            owner.settings.neuralStartup.sourceEncoding == startup.sourceEncoding &&
            owner.settings.neuralStartup.sdrBytesTrial == startup.sdrBytesTrial,
            "actual startup choices and driver paths unchanged by migration");
        Check(Overlay::LoadLayout(reloaded).x == menu.x && Overlay::LoadLayout(reloaded).width == menu.width,
            "actual menu geometry survives migration");
    }
    std::puts("PASS: new INI loading, precedence, startup policy, runtime paths and independent passes");
}
