#pragma once
#include <array>
#include <string>
#include <string_view>
#include <utility>

// Public INI layout; legacy names remain an internal serialization bridge.
namespace TheosRenderPipeline::IniLayout
{
struct Key { const char* legacySection; const char* legacyKey; const char* section; const char* key; };
inline constexpr std::array keys{
    Key{"Settings", "QualityLevel", "DLSS", "QualityLevel"},
    Key{"Settings", "DLSSPreset", "DLSS", "Preset"},
    Key{"Experimental", "FrameGenerationBackend", "FrameGeneration", "Backend"},
    Key{"Experimental", "SourceDLSSGMFGUnlock", "Compatibility", "NvidiaMFGUnlock"},
    Key{"Experimental", "NativeUICompositionMode", "FrameGeneration", "UICompositionMode"},
    Key{"SourceDLSSG", "GeneratedFrames", "FrameGeneration", "GeneratedFrames"},
    Key{"SourceDLSSG", "DynamicMFG", "FrameGeneration", "DynamicMFG"},
    Key{"SourceDLSSG", "DynamicTargetFPS", "FrameGeneration", "DynamicTargetFPS"},
    Key{"SourceDLSSG", "UIRecomposition", "FrameGeneration", "UIRecomposition"},
    Key{"SourceDLSSG", "OutputFPSLimit", "FrameGeneration", "OutputFPSLimit"},
    Key{"SourceDLSSG", "ReflexMode", "FrameGeneration", "ReflexMode"},
    Key{"SourceDLSSG", "NeuralRenderingEnabled", "NeuralRendering", "Enabled"},
    Key{"SourceDLSSG", "NRBeforeUpscaling", "NeuralRendering", "BeforeUpscaling"},
    Key{"SourceDLSSG", "NRPasses", "NeuralRendering", "PassCount"},
    Key{"SourceDLSSG", "NROnePassInCombat", "NeuralRendering", "OnePassInCombat"},
    Key{"SourceDLSSG", "NROnePassWeaponsDrawn", "NeuralRendering", "OnePassWeaponsDrawn"},
    Key{"SourceDLSSG", "NRPassRecoverySeconds", "NeuralRendering", "PassRecoverySeconds"},
    Key{"SourceDLSSG", "NRStyle", "NR PASS 1", "Style"},
    Key{"SourceDLSSG", "NRIntensity", "NR PASS 1", "Intensity"},
    Key{"SourceDLSSG", "NRLocalTone", "NR PASS 1", "Tone"},
    Key{"SourceDLSSG", "NRLocalStructure", "NR PASS 1", "Structure"},
    Key{"SourceDLSSG", "NRSkinStructure", "NR PASS 1", "SkinStructure"},
    Key{"SourceDLSSG", "NRAutoSkinMask", "NR PASS 1", "AutoSkin"},
    Key{"SourceDLSSG", "NRUICorrection", "NR PASS 1", "UICorrection"},
    Key{"SourceDLSSG", "NRPreset", "NR PASS 1", "Preset"},
    Key{"SourceDLSSG", "NRResolveMethod", "NR PASS 1", "ResolveMethod"},
    Key{"SourceDLSSG", "NRInputScale", "NR PASS 1", "InputScale"},
    Key{"SourceDLSSG", "NRTransferStrength", "NR PASS 1", "TransferStrength"},
    Key{"SourceDLSSG", "NRColourStrength", "NR PASS 1", "ColourStrength"},
    Key{"SourceDLSSG", "NRMaxRatio", "NR PASS 1", "MaxRatio"},
    Key{"SourceDLSSG", "NRWhitePoint", "NR PASS 1", "WhitePoint"},
    Key{"SourceDLSSG", "NRColorIsHDR", "NR PASS 1", "ColorIsHDR"},
    Key{"SourceDLSSG", "NRPeripheralCompression", "NR PASS 1", "PeripheralCompression"},
    Key{"SourceDLSSG", "NRFusedPreparation", "NR PASS 1", "FusedPreparation"},
    Key{"SourceDLSSG", "NRPass2UseSameSettings", "NR PASS 2", "UseSameSettings"},
    Key{"SourceDLSSG", "NRPass2InputScale", "NR PASS 2", "InputScale"},
    Key{"SourceDLSSG", "NRPass2Preset", "NR PASS 2", "Preset"},
    Key{"SourceDLSSG", "NRPass2Style", "NR PASS 2", "Style"},
    Key{"SourceDLSSG", "NRPass2Intensity", "NR PASS 2", "Intensity"},
    Key{"SourceDLSSG", "NRPass2LocalTone", "NR PASS 2", "Tone"},
    Key{"SourceDLSSG", "NRPass2LocalStructure", "NR PASS 2", "Structure"},
    Key{"SourceDLSSG", "NRPass2SkinStructure", "NR PASS 2", "SkinStructure"},
    Key{"SourceDLSSG", "NRPass2AutoSkinMask", "NR PASS 2", "AutoSkin"},
    Key{"SourceDLSSG", "NRPass2UICorrection", "NR PASS 2", "UICorrection"},
    Key{"SourceDLSSG", "NRPass3UseSameSettings", "NR PASS 3", "UseSameSettings"},
    Key{"SourceDLSSG", "NRPass3InputScale", "NR PASS 3", "InputScale"},
    Key{"SourceDLSSG", "NRPass3Preset", "NR PASS 3", "Preset"},
    Key{"SourceDLSSG", "NRPass3Style", "NR PASS 3", "Style"},
    Key{"SourceDLSSG", "NRPass3Intensity", "NR PASS 3", "Intensity"},
    Key{"SourceDLSSG", "NRPass3LocalTone", "NR PASS 3", "Tone"},
    Key{"SourceDLSSG", "NRPass3LocalStructure", "NR PASS 3", "Structure"},
    Key{"SourceDLSSG", "NRPass3SkinStructure", "NR PASS 3", "SkinStructure"},
    Key{"SourceDLSSG", "NRPass3AutoSkinMask", "NR PASS 3", "AutoSkin"},
    Key{"SourceDLSSG", "NRPass3UICorrection", "NR PASS 3", "UICorrection"},
    Key{"Experimental", "SourceDLSSGStreamlineDirectory", "Runtime", "StreamlineDirectory"},
    Key{"Experimental", "NeuralRenderingRuntimePath", "Runtime", "NRRuntimePath"},
    Key{"NeuralRendering", "RuntimeRoot", "Runtime", "NRRuntimeRoot"},
    Key{"NeuralRendering", "DriverCore", "Runtime", "NRDriverCore"},
    Key{"Overlay", "WindowX", "Menu", "WindowX"},
    Key{"Overlay", "WindowY", "Menu", "WindowY"},
    Key{"Overlay", "WindowWidth", "Menu", "WindowWidth"},
    Key{"Overlay", "WindowHeight", "Menu", "WindowHeight"},
    Key{"Overlay", "LeftColumnFraction", "Menu", "LeftColumnFraction"},
};

template<class Ini> class ReadView
{
public:
    explicit ReadView(const Ini& source) : source_(source) {}
    const char* GetValue(const char* section, const char* key, const char* fallback = nullptr) const
    { const auto selected = Select(section, key); return source_.GetValue(selected.first, selected.second, fallback); }
    bool GetBoolValue(const char* section, const char* key, bool fallback = false) const
    { const auto selected = Select(section, key); return source_.GetBoolValue(selected.first, selected.second, fallback); }
    long GetLongValue(const char* section, const char* key, long fallback = 0) const
    { const auto selected = Select(section, key); return source_.GetLongValue(selected.first, selected.second, fallback); }
    double GetDoubleValue(const char* section, const char* key, double fallback = 0) const
    { const auto selected = Select(section, key); return source_.GetDoubleValue(selected.first, selected.second, fallback); }
private:
    std::pair<const char*, const char*> Select(const char* section, const char* key) const
    {
        for (const auto& entry : keys) {
            if (std::string_view(section) == entry.legacySection && std::string_view(key) == entry.legacyKey)
                return {entry.section, entry.key};
        }
        return {section, key};
    }
    const Ini& source_;
};

// Populate internal writer slots only from the current public layout. Never
// promote obsolete external keys, including when their replacements are absent.
template<class Ini> void PrepareForUpdate(Ini& ini)
{
    ini.Delete("SourceDLSSG", "NRStableColors");
    ini.Delete("NeuralRendering", "StableColors");
    for (const auto& entry : keys) {
        ini.Delete(entry.legacySection, entry.legacyKey);
        if (const auto value = ini.GetValue(entry.section, entry.key, nullptr)) {
            const std::string copy(value);
            ini.SetValue(entry.legacySection, entry.legacyKey, copy.c_str());
        }
    }
}

// Call only after PrepareForUpdate and all writers: newly applied values must
// replace old canonical values. Unknown keys and sections are preserved.
template<class Ini> void StoreCanonical(Ini& ini)
{
    ini.Delete("SourceDLSSG", "NRStableColors");
    ini.Delete("NeuralRendering", "StableColors");
    for (const auto& entry : keys) {
        if (const auto value = ini.GetValue(entry.legacySection, entry.legacyKey, nullptr)) {
            const std::string copy(value);
            ini.SetValue(entry.section, entry.key, copy.c_str());
            ini.Delete(entry.legacySection, entry.legacyKey);
        }
    }
    for (const auto section : {"SourceDLSSG", "Experimental", "Overlay"}) {
        if (ini.GetSectionSize(section) == 0) ini.Delete(section, nullptr);
    }
    ini.Delete("Settings", "ConfigVersion");
}
}
