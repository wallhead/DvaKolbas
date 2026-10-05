#include "AmdNrBridge.h"
#include <Windows.h>
#include <dxgi.h>
#include <array>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <mutex>
#include <string_view>
#include <vector>

namespace TheosRenderPipeline::Upscaling::AmdNr
{
    namespace
    {
        template<class T> bool ReadAt(std::span<const std::byte> image, std::size_t offset, T& value) noexcept
        {
            if (offset > image.size() || image.size() - offset < sizeof(T)) return false;
            std::memcpy(&value, image.data() + offset, sizeof(T));
            return true;
        }

        // The six system DLL names the mod can stand in for in the game folder.
        constexpr std::array<const wchar_t*, 6> kProxyNames{
            L"version.dll", L"winmm.dll", L"dbghelp.dll", L"wininet.dll", L"winhttp.dll", L"dxgi.dll"};

        struct State
        {
            std::mutex mutex;
            Settings settings;
            std::filesystem::path pluginDirectory;
            bool configured{}, attempted{};
            HMODULE module{};
            std::string status{"not configured"};
        };
        State& Global() { static State state; return state; }

        bool LoadedModuleIsMod(HMODULE module) noexcept
        {
            if (!module) return false;
            // A mapped image keeps its headers at the module base.
            MEMORY_BASIC_INFORMATION region{};
            if (!VirtualQuery(module, &region, sizeof(region)) || region.State != MEM_COMMIT) return false;
            const auto base = reinterpret_cast<const std::byte*>(module);
            const auto available = static_cast<std::size_t>(region.RegionSize - (base - static_cast<const std::byte*>(region.BaseAddress)));
            return HasHipFatBinarySection({base, available < 4096 ? available : 4096});
        }

        HMODULE FindLoadedProxy(std::wstring& name) noexcept
        {
            for (const auto* candidate : kProxyNames) {
                if (const auto module = GetModuleHandleW(candidate); LoadedModuleIsMod(module)) { name = candidate; return module; }
            }
            return nullptr;
        }

        std::string Narrow(const std::wstring& text)
        {
            if (text.empty()) return {};
            const auto size = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
            std::string result(static_cast<std::size_t>(size > 0 ? size : 0), '\0');
            if (size > 0) WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), size, nullptr, nullptr);
            return result;
        }

        std::string LoadConfigured(State& state)
        {
            std::filesystem::path path(state.settings.module);
            if (path.is_relative()) path = state.pluginDirectory / path;
            std::error_code ec;
            path = std::filesystem::weakly_canonical(path, ec);
            const auto shown = Narrow(path.wstring());
            if (ec || !std::filesystem::is_regular_file(path, ec)) return "not loaded: module not found at " + shown;

            std::vector<std::byte> header(4096);
            {
                std::ifstream file(path, std::ios::binary);
                file.read(reinterpret_cast<char*>(header.data()), static_cast<std::streamsize>(header.size()));
                header.resize(static_cast<std::size_t>(file.gcount()));
            }
            if (!HasHipFatBinarySection(header))
                return "not loaded: " + shown + " is not an x64 DLSS-NR-on-AMD build (no HIP kernel section)";

            // The mod resolves amdhip64_7.dll itself (DLL search path or HIP_PATH).
            const auto module = LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (!module) return "not loaded: LoadLibraryEx failed for " + shown + " (error " + std::to_string(GetLastError()) + ")";
            state.module = module;
            return "loaded " + shown + "; it hooks FSR when TRP creates the FSR context. Its log and ini are next to that DLL";
        }
    }

    bool HasHipFatBinarySection(std::span<const std::byte> image) noexcept
    {
        std::uint16_t magic{}; std::uint32_t ntOffset{};
        if (!ReadAt(image, 0, magic) || magic != 0x5A4D || !ReadAt(image, 0x3C, ntOffset)) return false;
        std::uint32_t signature{}; std::uint16_t machine{}, sections{}, optionalSize{};
        if (!ReadAt(image, ntOffset, signature) || signature != 0x00004550) return false;
        if (!ReadAt(image, std::size_t{ntOffset} + 4, machine) || machine != 0x8664) return false;
        if (!ReadAt(image, std::size_t{ntOffset} + 6, sections) || !ReadAt(image, std::size_t{ntOffset} + 20, optionalSize)) return false;
        const auto table = std::size_t{ntOffset} + 24 + optionalSize;
        for (std::size_t i = 0; i < sections && i < 96; ++i) {
            std::array<char, 8> name{};
            if (!ReadAt(image, table + i * 40, name)) return false;
            if (std::string_view(name.data(), strnlen(name.data(), name.size())) == ".hip_fat") return true;
        }
        return false;
    }

    void Configure(Settings settings, std::filesystem::path pluginDirectory)
    {
        auto& state = Global();
        std::scoped_lock lock(state.mutex);
        if (state.attempted) return;
        state.settings = std::move(settings);
        state.pluginDirectory = std::move(pluginDirectory);
        state.configured = true;
        state.status = std::string("configured (mode ") + ModeName(state.settings.mode) + "), waiting for FSR startup";
    }

    void EnsureLoaded(IDXGIAdapter* adapter) noexcept
    {
        auto& state = Global();
        std::scoped_lock lock(state.mutex);
        if (!state.configured || state.attempted) return;
        state.attempted = true;
        try {
            DXGI_ADAPTER_DESC desc{};
            const std::uint32_t vendor = adapter && SUCCEEDED(adapter->GetDesc(&desc)) ? desc.VendorId : 0;
            std::wstring proxyName;
            const auto loaded = state.settings.mode == Mode::Off ? nullptr : FindLoadedProxy(proxyName);
            const auto decision = Decide(state.settings, vendor, loaded != nullptr);
            switch (decision.action) {
            case Action::Skip: state.status = decision.reason; break;
            case Action::AdoptLoaded: state.module = loaded; state.status = "using the copy loaded as " + Narrow(proxyName) + ": " + decision.reason; break;
            case Action::Load: state.status = LoadConfigured(state); break;
            }
            char vendorText[16]{};
            std::snprintf(vendorText, sizeof(vendorText), "0x%04X", vendor);
            state.status += std::string(" [adapter vendor ") + vendorText + "]";
        } catch (...) {
            state.status = "not loaded: unexpected failure while resolving the module";
        }
    }

    std::string Status()
    {
        auto& state = Global();
        std::scoped_lock lock(state.mutex);
        return state.status;
    }
}
