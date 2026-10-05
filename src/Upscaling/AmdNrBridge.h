#pragma once
#include "UpscalerBackend.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>

struct IDXGIAdapter;

// Optional bridge to the separately installed "DLSS NR on AMD" mod
// (https://github.com/danielblnc/DLSS-NR-on-AMD). TRP never ships, renames or
// patches that DLL. When enabled, TRP loads the user's own copy into the
// process before the FSR runtime is created; the mod then detours the
// FidelityFX entry points TRP already calls (ffxCreateContext/ffxDispatch) and
// runs DLSS 5 Neural Rendering on the AMD GPU through HIP. TRP has no other
// interface to it: settings live in the mod's own ini next to its DLL.
namespace TheosRenderPipeline::Upscaling::AmdNr
{
    enum class Mode { Off, AmdOnly, AnyGpu };
    inline constexpr std::uint32_t kAmdVendorId = 0x1002;

    struct Settings
    {
        Mode mode{Mode::Off};
        // Relative to Data/SKSE/Plugins, or absolute. Empty: only adopt a copy
        // that the game already loaded as a game-folder proxy DLL.
        std::string module;
        bool operator==(const Settings&) const = default;
    };

    inline const char* ModeName(Mode value)
    {
        switch (value) {
        case Mode::Off: return "Off";
        case Mode::AmdOnly: return "AMD";
        case Mode::AnyGpu: return "Any";
        }
        return "Invalid";
    }

    template<class Ini> Result<Settings> ReadSettings(const Ini& ini)
    {
        Settings result;
        const std::string_view mode = ini.GetValue("NeuralRendering", "AmdBridge", "Off");
        if (mode == "Off") result.mode = Mode::Off;
        else if (mode == "AMD") result.mode = Mode::AmdOnly;
        else if (mode == "Any") result.mode = Mode::AnyGpu;
        else return std::unexpected(RuntimeError{ErrorKind::InvalidInput, 0, "[NeuralRendering] AmdBridge must be Off, AMD or Any."});
        result.module = ini.GetValue("Runtime", "AmdNRModule", "");
        return result;
    }

    enum class Action { Skip, AdoptLoaded, Load };
    struct Decision { Action action{Action::Skip}; const char* reason{""}; };

    // Pure policy: what to do for this adapter and process state.
    inline Decision Decide(const Settings& settings, std::uint32_t adapterVendorId, bool alreadyLoaded)
    {
        if (settings.mode == Mode::Off) return {Action::Skip, "off ([NeuralRendering] AmdBridge=Off)"};
        if (alreadyLoaded) return {Action::AdoptLoaded, "already loaded by the game as a proxy DLL; TRP does not load a second copy"};
        if (settings.mode == Mode::AmdOnly && adapterVendorId != kAmdVendorId)
            return {Action::Skip, "skipped: the FSR adapter is not an AMD GPU (AmdBridge=Any loads it anyway for diagnostics)"};
        if (settings.module.empty())
            return {Action::Skip, "not loaded: [Runtime] AmdNRModule is empty and no proxy install is loaded"};
        return {Action::Load, "loading the configured module"};
    }

    // The mod is the only proxy DLL that carries a HIP fat-binary section.
    // Accepts a file prefix (at least the PE headers) or a mapped image header.
    bool HasHipFatBinarySection(std::span<const std::byte> image) noexcept;

    // Process-wide. Configure once at plugin load; EnsureLoaded acts at most
    // once per process and never unloads, because the module installs detours.
    void Configure(Settings settings, std::filesystem::path pluginDirectory);
    void EnsureLoaded(IDXGIAdapter* adapter) noexcept;
    std::string Status();
}
