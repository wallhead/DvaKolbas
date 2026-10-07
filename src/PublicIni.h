#pragma once
#include "PublicIniSchema.h"
#include "FrameGen/GenerationBackendPreference.h"
#include <charconv>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
namespace TheosRenderPipeline::PublicIni
{
inline bool Number(std::string_view value)
{
    double parsed{};
    const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
    return result.ec == std::errc{} && result.ptr == value.data() + value.size() && std::isfinite(parsed);
}
inline bool Integer(std::string_view value)
{
    std::int32_t parsed{};
    const auto result = std::from_chars(value.data(), value.data() + value.size(), parsed);
    return result.ec == std::errc{} && result.ptr == value.data() + value.size();
}
inline constexpr std::array<std::pair<const char*, long>, 7> NamedHotkeys{{{"End", 0x23L}, {"Insert", 0x2DL},
    {"Home", 0x24L}, {"PageUp", 0x21L}, {"PageDown", 0x22L}, {"Delete", 0x2EL}, {"Tab", 0x09L}}};
inline std::optional<long> Hotkey(std::string_view name)
{
    for (const auto [label, value] : NamedHotkeys) {
        if (name == label) return value;
    }
    long value{};
    if (name.starts_with('F')) {
        name.remove_prefix(1);
        const auto parsed = std::from_chars(name.data(), name.data() + name.size(), value);
        if (parsed.ec == std::errc{} && parsed.ptr == name.data() + name.size() && value >= 1 && value <= 12)
            return 0x70 + value - 1;
        return std::nullopt;
    }
    int base = 10;
    if (name.starts_with("0x") || name.starts_with("0X")) { name.remove_prefix(2); base = 16; }
    const auto parsed = std::from_chars(name.data(), name.data() + name.size(), value, base);
    return parsed.ec == std::errc{} && parsed.ptr == name.data() + name.size() && value > 0 && value <= 255 ?
        std::optional(value) : std::nullopt;
}
inline std::string HotkeyName(long value)
{
    for (const auto [label, code] : NamedHotkeys) if (value == code) return label;
    if (value >= 0x70 && value <= 0x7B) return "F" + std::to_string(value - 0x70 + 1);
    char text[16]{};
    const auto encoded = std::to_chars(text, text + sizeof(text), value, 16);
    return "0x" + std::string(text, encoded.ptr);
}
inline std::string Invalid(const Field& field)
{
    std::string message = "[" + std::string(field.section) + "] " + field.key + " has an invalid value.";
    if (std::string_view(field.type) == "Integer")
        message += " Use a whole number from -2147483648 to 2147483647; decimals and exponents are not supported.";
    if (!field.values.empty()) {
        message += " Use ";
        for (std::size_t i = 0; i < field.values.size(); ++i) {
            if (i) message += ", ";
            message += field.values[i].publicValue;
        }
        message += ".";
    }
    return message;
}
template<class Ini> void RemoveRetired(Ini& ini)
{
    ini.Delete("", "ConfigVersion");
    ini.Delete("Settings", "ConfigVersion");
    ini.Delete("SourceDLSSG", "NRStableColors");
    ini.Delete("NeuralRendering", "StableColors");
}
template<class Ini> std::string Decode(Ini& ini)
{
    if (!ini.GetValue("Upscaling", "Upscaler", nullptr))
        return "RaZkolbaS.ini uses an obsolete or incomplete layout. Install the matching named INI or run the offline converter. Set [Upscaling] Upscaler=DLSS or FSR; choose Native in the provider's Quality setting.";
    for (const auto& input : RetiredInputs) {
        if (ini.GetValue(input.section, input.key, nullptr))
            return "[" + std::string(input.section) + "] " + input.key +
                " is obsolete. Remove this key and configure the named Upscaling, NeuralRendering and FrameGeneration sections.";
    }
    // Snapshot before changing same-named keys such as DLSS/Preset and FSR/Quality.
    std::vector<std::optional<std::string>> decoded;
    decoded.reserve(Fields.size());
    bool automaticBias{};
    for (const auto& field : Fields) {
        const auto* raw = ini.GetValue(field.section, field.key, field.inherit ? nullptr : field.defaultValue);
        if (!raw) { decoded.emplace_back(std::nullopt); continue; }
        std::string value(raw);
        if (std::string_view(field.codec) == "Hotkey") {
            const auto key = Hotkey(value);
            if (!key) return Invalid(field);
            value = std::to_string(*key);
        } else if (std::string_view(field.codec) == "MipLodBias") {
            automaticBias = value == "Auto";
            if (!automaticBias && !Number(value)) return Invalid(field);
            value = automaticBias ? "true" : "false";
        } else if (!field.values.empty()) {
            bool found{};
            for (const auto& entry : field.values) {
                if (value == entry.publicValue) { value = entry.internalValue; found = true; break; }
            }
            if (!found) return Invalid(field);
        } else if (std::string_view(field.type) == "Number" && !Number(value)) return Invalid(field);
        else if (std::string_view(field.type) == "Integer" && !Integer(value)) return Invalid(field);
        else if (std::string_view(field.type) == "Bool" && value != "true" && value != "false") return Invalid(field);
        decoded.emplace_back(std::move(value));
    }
    const std::string bias = ini.GetValue("Upscaling", "MipLodBias", "Auto");
    const bool native = std::string_view(ini.GetValue("DLSS", "Quality", "Native")) == "Native";
    for (const auto& field : Fields) {
        if (std::string_view(field.internalSection) != field.section || std::string_view(field.internalKey) != field.key)
            ini.Delete(field.internalSection, field.internalKey);
    }
    for (std::size_t i = 0; i < Fields.size(); ++i) {
        if (decoded[i]) ini.SetValue(Fields[i].internalSection, Fields[i].internalKey, decoded[i]->c_str());
    }
    const long mode = ini.GetLongValue("Settings", "UpscaleType", 0);
    ini.SetBoolValue("Settings", "DLSSNativeScale", native);
    if (mode == 0 && native) ini.SetLongValue("Settings", "UpscaleType", 3);
    ini.SetValue("Settings", "MipLodBias", automaticBias ? "0.0" : bias.c_str());
    ini.SetBoolValue("Settings", "Sharpening", ini.GetDoubleValue("Settings", "Sharpness", 0) > 0);
    const bool ordinary = ini.GetBoolValue("Experimental", "FsrOrdinaryPresenter", false);
    ini.SetLongValue("FrameGeneration", "Backend", ResolveGenerationBackend(
        static_cast<GenerationBackendPreference>(ini.GetLongValue("FrameGeneration", "BackendPreference", 0)),
        mode == 4 ? Upscaling::BackendKind::Fsr : Upscaling::BackendKind::Dlss, ordinary));
    if (mode == 4 && ordinary && ini.GetBoolValue("FrameGeneration", "Enabled", false))
        return "[Upscaling Advanced] FsrOrdinaryPresenter requires [FrameGeneration] Enabled=false.";
    RemoveRetired(ini);
    return {};
}

template<class Ini> std::string Encode(Ini& ini)
{
    const long mode = ini.GetLongValue("Settings", "UpscaleType", 3);
    if (mode != 0 && mode != 3 && mode != 4) return "Cannot save an unsupported upscaling mode.";
    const bool native = mode == 3 || (mode == 4 && ini.GetBoolValue("Settings", "DLSSNativeScale", true));
    std::vector<std::optional<std::string>> encoded;
    encoded.reserve(Fields.size());
    for (const auto& field : Fields) {
        const auto* raw = ini.GetValue(field.internalSection, field.internalKey, nullptr);
        if (!raw) { encoded.emplace_back(std::nullopt); continue; }
        std::string value(raw);
        const std::string_view section(field.section), key(field.key);
        if (section == "Upscaling" && key == "Upscaler") value = mode == 4 ? "FSR" : "DLSS";
        else if (section == "DLSS" && key == "Quality" && native) value = "Native";
        else if (section == "DLSS" && key == "Sharpness") {
            if (!ini.GetBoolValue("Settings", "Sharpening", false)) value = "0.0";
        } else if (std::string_view(field.codec) == "MipLodBias") {
            value = ini.GetBoolValue("Settings", "UseOptimalMipLodBias", true) ? "Auto" :
                ini.GetValue("Settings", "MipLodBias", "0.0");
        } else if (std::string_view(field.codec) == "Hotkey") {
            value = HotkeyName(ini.GetLongValue(field.internalSection, field.internalKey, 0x23));
        } else if (section == "Upscaling Advanced" && key == "FsrOrdinaryPresenter") {
            value = mode == 4 && ini.GetLongValue("FrameGeneration", "Backend", 2) == 0 ? "true" : "false";
        } else if (!field.values.empty()) {
            bool found{};
            for (const auto& entry : field.values) {
                if (section == "DLSS" && key == "Quality" && entry.publicValue == "Native") continue;
                if (value == entry.internalValue) { value = entry.publicValue; found = true; break; }
            }
            if (!found) return Invalid(field);
        }
        encoded.emplace_back(std::move(value));
    }
    for (const auto& field : Fields) {
        if (std::string_view(field.internalSection) != field.section || std::string_view(field.internalKey) != field.key)
            ini.Delete(field.internalSection, field.internalKey);
    }
    ini.Delete("Settings", "DLSSNativeScale");
    ini.Delete("Settings", "MipLodBias");
    ini.Delete("Settings", "Sharpening");
    ini.Delete("FrameGeneration", "Backend");
    for (std::size_t i = 0; i < Fields.size(); ++i) {
        if (encoded[i]) ini.SetValue(Fields[i].section, Fields[i].key, encoded[i]->c_str());
    }
    for (const auto section : {"Settings", "Performance", "Experimental"}) {
        if (ini.GetSectionSize(section) == 0) ini.Delete(section, nullptr);
    }
    RemoveRetired(ini);
    return {};
}
}
