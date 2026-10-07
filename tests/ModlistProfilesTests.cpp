#include "ModlistProfiles.h"
#include <chrono>
#include <cstdio>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;
using TheosRenderPipeline::ModlistProfiles::Discover;

void Require(bool value, const char* message)
{
    if (!value) { throw std::runtime_error(message); }
}

int main(int argc, char** argv)
{
    if (argc == 2) {
        const auto discovered = Discover(fs::u8path(argv[1]), {});
        for (const auto& profile : discovered.profiles) {
            std::printf("profile=%s\n", TheosRenderPipeline::ModlistProfiles::Utf8(profile).c_str());
        }
        std::printf("count=%zu native=%d\n", discovered.profiles.size(), discovered.error.value());
        return discovered.root.empty() || discovered.error ? 1 : 0;
    }
    const auto fixture = fs::temp_directory_path() / ("RaZkolbaS-profiles-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    struct Cleanup {
        fs::path path;
        ~Cleanup() { std::error_code ec; fs::remove_all(path, ec); }
    } cleanup{fixture};
    try {
        const auto root = fixture / "Modlist";
        const auto module = root / "mods" / "Renderer" / "SKSE" / "Plugins" / "RaZkolbaS.dll";
        fs::create_directories(module.parent_path());
        fs::create_directories(root / "profiles" / "V5.4 NO-LORE");
        fs::create_directories(root / "profiles" / "Creation Kit");
        std::ofstream(root / "profiles" / "not-a-profile.txt") << "ignore files";
        auto discovered = Discover(module, {});
        Require(discovered.root == root && !discovered.error, "find profiles beside the physical mods directory");
        Require(discovered.profiles == std::vector<fs::path>{"profiles/Creation Kit", "profiles/V5.4 NO-LORE"},
            "report all profile directories in deterministic order using relative paths");

        const auto virtualPlugins = root / "Stock Game" / "Data" / "SKSE" / "Plugins";
        discovered = Discover(virtualPlugins / "RaZkolbaS.dll", virtualPlugins);
        Require(discovered.root == root && discovered.profiles.size() == 2,
            "resolve a virtual Data module through nearby game ancestors");
        discovered = Discover(fixture / "unrelated" / "RaZkolbaS.dll", virtualPlugins);
        Require(discovered.root == root && discovered.profiles.size() == 2,
            "game path fallback works when the module is loaded outside the modlist");

        fs::create_directories(fixture / "Other" / "profiles" / "Other Profile");
        discovered = Discover(module, fixture / "Other" / "Data" / "SKSE" / "Plugins");
        Require(discovered.root == root && discovered.profiles.size() == 2,
            "physical modlist takes precedence over unrelated game profiles");

        const auto upper = fixture / "Upper";
        fs::create_directories(upper / "MODS" / "Renderer");
        fs::create_directories(upper / "profiles" / fs::path(L"\u041f\u0440\u043e\u0444\u0438\u043b\u044c"));
        discovered = Discover(upper / "MODS" / "Renderer" / "RaZkolbaS.dll", {});
        Require(discovered.root == upper && discovered.profiles.size() == 1,
            "Windows mods folder matching is case insensitive and Unicode profile names survive");
        Require(TheosRenderPipeline::ModlistProfiles::Utf8(discovered.profiles.front()) ==
            "profiles/\xD0\x9F\xD1\x80\xD0\xBE\xD1\x84\xD0\xB8\xD0\xBB\xD1\x8C",
            "profile paths reach the logger as lossless UTF-8 without machine-specific prefixes");

        fs::create_directories(fixture / "Empty" / "mods" / "Renderer");
        fs::create_directories(fixture / "Empty" / "profiles");
        discovered = Discover(fixture / "Empty" / "mods" / "Renderer" / "RaZkolbaS.dll", {});
        Require(discovered.root == fixture / "Empty" && discovered.profiles.empty() && !discovered.error,
            "an empty profiles directory is a valid empty discovery");
        discovered = Discover(fixture / "Standalone" / "Data" / "SKSE" / "Plugins" / "RaZkolbaS.dll", {});
        Require(discovered.root.empty() && discovered.profiles.empty(), "standalone installation has no guessed profile");
        discovered = Discover({}, {});
        Require(discovered.root.empty(), "missing paths do not scan the current working directory");

        fs::create_directories(fixture / "Broken" / "mods" / "Renderer");
        std::ofstream(fixture / "Broken" / "profiles") << "not a directory";
        discovered = Discover(fixture / "Broken" / "mods" / "Renderer" / "RaZkolbaS.dll", {});
        Require(discovered.root == fixture / "Broken" && discovered.error && discovered.profiles.empty(),
            "an unreadable or invalid profiles location returns a diagnostic without throwing");
        std::puts("PASS: physical/virtual MO2 layouts, multiple profiles, relative paths, Unicode, empty/missing/invalid folders");
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "FAIL: %s\n", e.what());
        return 1;
    }
}
