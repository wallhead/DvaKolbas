#include "XessGenerationRuntime.h"
#include <fstream>
#include <vector>

namespace TheosRenderPipeline
{
    namespace
    {
        using Upscaling::ErrorKind;
        using Upscaling::RuntimeError;
        bool SamePath(const std::filesystem::path& requested, HMODULE module)
        {
            std::vector<wchar_t> buffer(32768);
            const auto count = GetModuleFileNameW(module, buffer.data(), static_cast<DWORD>(buffer.size()));
            if (!count || count >= buffer.size()) return false;
            std::error_code error;
            return std::filesystem::equivalent(requested, std::filesystem::path(buffer.data()), error) && !error;
        }
        Upscaling::Result<void> CheckPe(const std::filesystem::path& path)
        {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            const auto size = file.tellg();
            if (!file || size < 64)
                return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi, 0, "Intel FG/XeLL DLL has an invalid PE header"});
            std::uint16_t dos{}, machine{};
            std::uint32_t offset{}, signature{};
            file.seekg(0); file.read(reinterpret_cast<char*>(&dos), 2);
            file.seekg(0x3c); file.read(reinterpret_cast<char*>(&offset), 4);
            if (dos != IMAGE_DOS_SIGNATURE || offset < 64 || static_cast<std::uint64_t>(offset) + 24 > static_cast<std::uint64_t>(size))
                return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi, 0, "Intel FG/XeLL DLL has an invalid PE offset"});
            file.seekg(offset); file.read(reinterpret_cast<char*>(&signature), 4); file.read(reinterpret_cast<char*>(&machine), 2);
            if (!file || signature != IMAGE_NT_SIGNATURE)
                return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi, 0, "Intel FG/XeLL DLL has an invalid PE signature"});
            if (machine != IMAGE_FILE_MACHINE_AMD64)
                return std::unexpected(RuntimeError{ErrorKind::WrongArchitecture, machine, "Intel FG/XeLL requires x64 runtime DLLs"});
            return {};
        }
    }
    struct XessGenerationRuntime::Modules
    {
        HMODULE generation{}, latency{};
        HANDLE generationFile{INVALID_HANDLE_VALUE}, latencyFile{INVALID_HANDLE_VALUE};
        ~Modules()
        {
            // Context owners retain this runtime and destroy FG before XeLL.
            if (generation) FreeLibrary(generation);
            if (latency) FreeLibrary(latency);
            if (generationFile != INVALID_HANDLE_VALUE) CloseHandle(generationFile);
            if (latencyFile != INVALID_HANDLE_VALUE) CloseHandle(latencyFile);
        }
    };
    XessGenerationRuntime::XessGenerationRuntime() = default;
    XessGenerationRuntime::~XessGenerationRuntime() = default;
    Upscaling::Result<std::shared_ptr<XessGenerationRuntime>> XessGenerationRuntime::Load(const std::filesystem::path& pluginDirectory)
    {
        if (!pluginDirectory.is_absolute())
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput, 0, "Intel FG plugin directory must be absolute"});
        auto modules = std::make_unique<Modules>();
        const auto folder = pluginDirectory / L"RaZkolbaS/XeSS";
        auto open = [&](const wchar_t* name, HANDLE& file) -> Upscaling::Result<void> {
            const auto path = folder / name;
            file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file == INVALID_HANDLE_VALUE) {
                const auto native = GetLastError();
                return std::unexpected(RuntimeError{native == ERROR_FILE_NOT_FOUND || native == ERROR_PATH_NOT_FOUND ? ErrorKind::MissingRuntime : ErrorKind::IncompatibleAbi,
                    native, "Cannot open/lock Intel FG or XeLL runtime beneath SKSE/Plugins/RaZkolbaS/XeSS"});
            }
            if (auto pe = CheckPe(path); !pe) return pe;
            if (const auto loaded = GetModuleHandleW(name); loaded && !SamePath(path, loaded))
                return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi, 0, "A different Intel FG/XeLL DLL is already loaded"});
            return {};
        };
        // Validate and retain both files before executing either library.
        if (auto result = open(L"libxess_fg.dll", modules->generationFile); !result) return std::unexpected(result.error());
        if (auto result = open(L"libxell.dll", modules->latencyFile); !result) return std::unexpected(result.error());
        auto load = [&](const wchar_t* name, HMODULE& module) -> Upscaling::Result<void> {
            const auto path = folder / name;
            module = LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (!module)
                return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi, GetLastError(), "Failed to load Intel FG/XeLL runtime or a dependency"});
            if (!SamePath(path, module))
                return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi, 0, "Loaded Intel FG/XeLL module differs from the retained file"});
            return {};
        };
        if (auto result = load(L"libxell.dll", modules->latency); !result) return std::unexpected(result.error());
        if (auto result = load(L"libxess_fg.dll", modules->generation); !result) return std::unexpected(result.error());
        XessGenerationFunctions generation{};
        XellFunctions latency{};
#define FG_RESOLVE(member, name) \
        generation.member = reinterpret_cast<decltype(generation.member)>(GetProcAddress(modules->generation, #name)); \
        if (!generation.member) return std::unexpected(RuntimeError{ErrorKind::MissingExport, ERROR_PROC_NOT_FOUND, "Intel FG export missing: " #name});
        FG_RESOLVE(GetVersion, xefgSwapChainGetVersion)
        FG_RESOLVE(CreateContext, xefgSwapChainD3D12CreateContext)
        FG_RESOLVE(BuildPipelines, xefgSwapChainD3D12BuildPipelines)
        FG_RESOLVE(GetProperties, xefgSwapChainGetProperties)
        FG_RESOLVE(GetD3D12Properties, xefgSwapChainD3D12GetProperties)
        FG_RESOLVE(InitFromSwapChainDesc, xefgSwapChainD3D12InitFromSwapChainDesc)
        FG_RESOLVE(GetSwapChainPtr, xefgSwapChainD3D12GetSwapChainPtr)
        FG_RESOLVE(GetInitializationParameters, xefgSwapChainD3D12GetInitializationParameters)
        FG_RESOLVE(TagFrameResource, xefgSwapChainD3D12TagFrameResource)
        FG_RESOLVE(TagFrameConstants, xefgSwapChainTagFrameConstants)
        FG_RESOLVE(SetEnabled, xefgSwapChainSetEnabled)
        FG_RESOLVE(SetPresentId, xefgSwapChainSetPresentId)
        FG_RESOLVE(GetLastPresentStatus, xefgSwapChainGetLastPresentStatus)
        FG_RESOLVE(SetLoggingCallback, xefgSwapChainSetLoggingCallback)
        FG_RESOLVE(SetLatencyReduction, xefgSwapChainSetLatencyReduction)
        FG_RESOLVE(SetUiCompositionState, xefgSwapChainSetUiCompositionState)
        FG_RESOLVE(Destroy, xefgSwapChainDestroy)
#undef FG_RESOLVE
#define XELL_RESOLVE(member, name) \
        latency.member = reinterpret_cast<decltype(latency.member)>(GetProcAddress(modules->latency, #name)); \
        if (!latency.member) return std::unexpected(RuntimeError{ErrorKind::MissingExport, ERROR_PROC_NOT_FOUND, "XeLL export missing: " #name});
        XELL_RESOLVE(GetVersion, xellGetVersion)
        XELL_RESOLVE(CreateContext, xellD3D12CreateContext)
        XELL_RESOLVE(SetSleepMode, xellSetSleepMode)
        XELL_RESOLVE(Sleep, xellSleep)
        XELL_RESOLVE(AddMarkerData, xellAddMarkerData)
        XELL_RESOLVE(SetLoggingCallback, xellSetLoggingCallback)
        XELL_RESOLVE(DestroyContext, xellDestroyContext)
#undef XELL_RESOLVE
        xefg_swapchain_version_t fgVersion{};
        xell_version_t latencyVersion{};
        const auto fgResult = generation.GetVersion(&fgVersion);
        const auto latencyResult = latency.GetVersion(&latencyVersion);
        if (fgResult != XEFG_SWAPCHAIN_RESULT_SUCCESS || fgVersion.major != 1 || fgVersion.minor < 3)
            return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi, fgResult, "Intel FG runtime must report the supported 1.3+ API"});
        if (latencyResult != XELL_RESULT_SUCCESS || latencyVersion.major != 1 || latencyVersion.minor < 3)
            return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi, latencyResult, "XeLL runtime must report the supported 1.3+ API"});
        auto runtime = std::shared_ptr<XessGenerationRuntime>(new XessGenerationRuntime);
        runtime->modules_ = std::move(modules);
        runtime->generation_ = generation;
        runtime->latency_ = latency;
        return runtime;
    }
}
