#pragma once
#include <algorithm>
#include <filesystem>
#include <string>
#include <system_error>
#include <vector>

namespace TheosRenderPipeline::ModlistProfiles
{
    struct Discovery
    {
        std::filesystem::path root;
        std::vector<std::filesystem::path> profiles;
        std::error_code error;
    };

    inline std::string Utf8(const std::filesystem::path& path)
    {
        const auto value = path.generic_u8string();
        return {reinterpret_cast<const char*>(value.data()), value.size()};
    }

    inline bool IsModsDirectory(const std::filesystem::path& path)
    {
        auto name = path.filename().wstring();
        for (auto& c : name) { if (c >= L'A' && c <= L'Z') { c += L'a' - L'A'; } }
        return name == L"mods";
    }

    inline Discovery Discover(const std::filesystem::path& moduleFile,
        const std::filesystem::path& virtualPluginsDirectory)
    {
        Discovery result;
        // Prefer the physical mod's own list. Only inspect nearby ancestors;
        // never recursively scan the game drive or enumerate other modlists.
        auto moduleDirectory = moduleFile.empty() ? std::filesystem::path{} : moduleFile.parent_path();
        auto directory = moduleDirectory;
        for (unsigned depth = 0; depth < 12 && !directory.empty() && directory != directory.root_path();
            ++depth, directory = directory.parent_path()) {
            if (IsModsDirectory(directory)) { result.root = directory.parent_path(); break; }
        }
        if (result.root.empty()) {
            // MO2 can expose the plugin as Stock Game/Data/SKSE/Plugins instead.
            // An executable/virtual Data path is also usable when the DLL lives elsewhere.
            for (const auto& start : {moduleDirectory, virtualPluginsDirectory}) {
                auto ancestor = start;
                for (unsigned depth = 0; depth < 12 && !ancestor.empty() && ancestor != ancestor.root_path();
                    ++depth, ancestor = ancestor.parent_path()) {
                    std::error_code error;
                    const bool exists = std::filesystem::exists(ancestor / L"profiles", error);
                    if (exists || error) {
                        result.root = ancestor;
                        result.error = error;
                        break;
                    }
                }
                if (!result.root.empty()) { break; }
            }
        }
        if (result.root.empty() || result.error) { return result; }

        std::filesystem::directory_iterator entry(result.root / L"profiles", result.error), end;
        while (!result.error && entry != end) {
            std::error_code error;
            if (entry->is_directory(error)) {
                result.profiles.emplace_back(std::filesystem::path(L"profiles") / entry->path().filename());
            }
            if (error) { result.error = error; break; }
            entry.increment(result.error);
        }
        std::sort(result.profiles.begin(), result.profiles.end());
        return result;
    }
}
