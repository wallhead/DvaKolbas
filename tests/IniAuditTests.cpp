#include "PublicIni.h"
#include "OverlayLayout.h"
#include "Upscaling/FSRAvailability.h"
#include <SimpleIni.h>
#include <cstdio>
#include <tuple>

int main()
{
    using namespace TheosRenderPipeline;
    int failures{};
    const auto check = [&](bool ok, const char* description) {
        if (!ok) { std::fprintf(stderr, "FAIL: %s\n", description); ++failures; }
    };
    for (const auto [section, key] : {std::pair{"NR PASS 1", "Style"}, {"NR PASS 2", "Style"},
        {"NR PASS 3", "Style"}, {"NeuralRendering", "PassCount"}, {"FrameGeneration", "NvidiaGeneratedFrames"},
        {"FrameGeneration", "DynamicTargetFPS"}, {"Latency", "OutputFPSLimit"},
        {"NR PASS 1 Advanced", "Preset"}, {"NR PASS 2 Advanced", "Preset"}, {"NR PASS 3 Advanced", "Preset"},
        {"Debug", "PerformanceLogIntervalSeconds"}, {"Appearance", "WeatherCount"}}) {
        for (const auto* bad : {"1.0", "1.5", "1e0", "2147483648"}) {
            CSimpleIniA ini; ini.SetValue("Upscaling", "Upscaler", "DLSS"); ini.SetValue(section, key, bad);
            const auto error = PublicIni::Decode(ini);
            check(!error.empty() && error.find(key) != std::string::npos &&
                error.find("whole number") != std::string::npos, "integer errors identify the public key and required type");
        }
    }
    for (const auto [section, key, bad] : {std::tuple{"FSR", "SourceColorEncoding", "Gama22"},
        {"NeuralRendering Advanced", "SourceColorEncoding", "Guess"},
        {"NeuralRendering Advanced", "Profile", "rtx41"}}) {
        CSimpleIniA ini; ini.SetValue("Upscaling", "Upscaler", "DLSS"); ini.SetValue(section, key, bad);
        const auto error = PublicIni::Decode(ini);
        check(!error.empty() && error.find(key) != std::string::npos && error.find("Use ") != std::string::npos,
            "fixed-choice errors identify the public key and valid choices even for inactive features");
    }
    for (const auto [section, key] : {std::pair{"Settings", "EnableUpscaler"},
        {"Experimental", "PureDarkFullDelegation"}, {"Experimental", "SourceDLSSGBackend"}, {"Debug", "AutoABTest"}}) {
        CSimpleIniA ini; ini.SetValue("Upscaling", "Upscaler", "DLSS"); ini.SetValue(section, key, "false");
        const auto error = PublicIni::Decode(ini);
        check(!error.empty() && error.find(key) != std::string::npos && error.find("Remove") != std::string::npos,
            "obsolete release controls give removal guidance rather than an unexplained renderer rejection");
    }
    for (const char* name : {"End", "Insert", "Home", "PageUp", "PageDown", "Delete", "Tab", "F1", "F12"}) {
        CSimpleIniA ini; ini.SetValue("Upscaling", "Upscaler", "DLSS"); ini.SetValue("Hotkeys", "ToggleOverlay", name);
        check(PublicIni::Decode(ini).empty() && PublicIni::Encode(ini).empty() &&
            std::string(ini.GetValue("Hotkeys", "ToggleOverlay", "")) == name, "named hotkeys survive Save unchanged");
    }
    CSimpleIniA fractional;
    fractional.LoadData("[Upscaling]\nUpscaler=DLSS\n[Menu]\nWindowWidth=640.5\n[NR PASS 1]\nStyle=1 \n");
    check(PublicIni::Decode(fractional).empty() && Overlay::LoadLayout(fractional).width == 640.5f &&
        fractional.GetLongValue("NR PASS 1", "Style", -1) == 1, "menu geometry remains floating point and trailing spaces remain valid");
    check(std::string_view(Upscaling::kFsrSrRecovery).find("[FSR] Provider=FSR3") != std::string_view::npos &&
        std::string_view(Upscaling::kFsrFgRecovery).find("[FrameGeneration] FsrProvider=FSR3") != std::string_view::npos,
        "FSR recovery instructions name editable public settings");
    std::printf("INI audit regressions: %d failures\n", failures);
    return failures ? 1 : 0;
}
