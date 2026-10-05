#include "Upscaling/AmdNrBridge.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

static void Check(bool value, const char* message)
{
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

namespace
{
    using namespace TheosRenderPipeline::Upscaling::AmdNr;
    using TheosRenderPipeline::Upscaling::Result;

    template<class T> void Put(std::vector<std::byte>& image, std::size_t offset, T value)
    {
        if (image.size() < offset + sizeof(T)) image.resize(offset + sizeof(T));
        std::memcpy(image.data() + offset, &value, sizeof(T));
    }

    // Minimal PE header: DOS stub, NT signature, file header, empty optional
    // header and the requested section names.
    std::vector<std::byte> Image(std::initializer_list<const char*> sections, std::uint16_t machine = 0x8664)
    {
        std::vector<std::byte> image(0x200);
        Put<std::uint16_t>(image, 0, 0x5A4D);
        Put<std::uint32_t>(image, 0x3C, 0x80);
        Put<std::uint32_t>(image, 0x80, 0x00004550);
        Put<std::uint16_t>(image, 0x84, machine);
        Put<std::uint16_t>(image, 0x86, static_cast<std::uint16_t>(sections.size()));
        Put<std::uint16_t>(image, 0x94, 0xF0);
        std::size_t offset = 0x80 + 24 + 0xF0;
        for (const auto* name : sections) {
            image.resize(offset + 40);
            std::memcpy(image.data() + offset, name, std::strlen(name) < 8 ? std::strlen(name) : 8);
            offset += 40;
        }
        return image;
    }

    Result<Settings> Read(const char* text)
    {
        CSimpleIniA ini;
        Check(ini.LoadData(text) >= 0, "test ini parses");
        return ReadSettings(ini);
    }
}

int main()
{
    // Settings: absent keys keep the bridge off; values are exact.
    auto settings = Read("[Settings]\nUpscaleType=4\n");
    Check(settings && settings->mode == Mode::Off && settings->module.empty(), "missing keys default to Off and no module");
    settings = Read("[NeuralRendering]\nAmdBridge=AMD\n[Runtime]\nAmdNRModule=TheosRenderPipeline/AMD-NR/version.dll\n");
    Check(settings && settings->mode == Mode::AmdOnly && settings->module == "TheosRenderPipeline/AMD-NR/version.dll", "AMD mode and module path read");
    settings = Read("[NeuralRendering]\nAmdBridge=Any\n");
    Check(settings && settings->mode == Mode::AnyGpu, "Any mode read");
    Check(!Read("[NeuralRendering]\nAmdBridge=on\n"), "unknown mode rejected");

    // Policy.
    const Settings off{};
    const Settings amd{Mode::AmdOnly, "AMD-NR/version.dll"};
    const Settings amdNoPath{Mode::AmdOnly, ""};
    const Settings any{Mode::AnyGpu, "AMD-NR/version.dll"};
    Check(Decide(off, kAmdVendorId, true).action == Action::Skip, "Off ignores even a loaded proxy");
    Check(Decide(amd, kAmdVendorId, false).action == Action::Load, "AMD adapter loads the configured module");
    Check(Decide(amd, 0x10DE, false).action == Action::Skip, "AMD mode skips an NVIDIA adapter");
    Check(Decide(any, 0x10DE, false).action == Action::Load, "Any mode loads on NVIDIA for diagnostics");
    Check(Decide(amdNoPath, kAmdVendorId, false).action == Action::Skip, "empty module path with no proxy skips");
    Check(Decide(amdNoPath, kAmdVendorId, true).action == Action::AdoptLoaded, "a proxy install is adopted, not loaded twice");
    Check(Decide(amd, 0x10DE, true).action == Action::AdoptLoaded, "a loaded proxy is reported whatever the vendor");

    // Module fingerprint.
    auto mod = Image({".text", ".rdata", ".hip_fat", ".reloc"});
    Check(HasHipFatBinarySection(mod), "HIP fat-binary section recognised");
    Check(!HasHipFatBinarySection(Image({".text", ".rdata", ".rsrc"})), "ordinary system DLL rejected");
    Check(!HasHipFatBinarySection(Image({".hip_fat"}, 0x014C)), "x86 image rejected");
    mod.resize(0x80 + 24 + 0xF0 + 40 + 20);
    Check(!HasHipFatBinarySection(mod), "truncated section table rejected");
    Check(!HasHipFatBinarySection({}), "empty input rejected");
    std::vector<std::byte> badOffset(0x40);
    Put<std::uint16_t>(badOffset, 0, 0x5A4D);
    Put<std::uint32_t>(badOffset, 0x3C, 0xFFFFFFF0u);
    Check(!HasHipFatBinarySection(badOffset), "out-of-range NT offset rejected");

    // Unconfigured process state stays inert.
    EnsureLoaded(nullptr);
    Check(Status() == "not configured", "EnsureLoaded is a no-op before Configure");

    std::puts("AmdNrBridge tests passed");
    return 0;
}
