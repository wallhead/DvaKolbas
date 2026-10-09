#include "XessRuntime.h"
#include <fstream>
#include <vector>

namespace TheosRenderPipeline::Upscaling
{
    namespace
    {
        RuntimeError Error(ErrorKind kind, std::int64_t code, const char* message)
        { return {kind,code,message}; }
        bool SamePath(const std::filesystem::path& requested, HMODULE module)
        {
            std::vector<wchar_t> buffer(32768);
            const auto count=GetModuleFileNameW(module,buffer.data(),static_cast<DWORD>(buffer.size()));
            if (!count || count>=buffer.size()) return false;
            std::error_code error;
            return std::filesystem::equivalent(requested,std::filesystem::path(buffer.data()),error) && !error;
        }
        Result<void> CheckPe(const std::filesystem::path& path)
        {
            std::ifstream file(path,std::ios::binary|std::ios::ate);
            const auto size=file.tellg();
            if (!file || size<64) return std::unexpected(Error(ErrorKind::IncompatibleAbi,0,"XeSS DLL has an invalid PE header"));
            std::uint16_t dos{},machine{};std::uint32_t offset{},signature{};
            file.seekg(0);file.read(reinterpret_cast<char*>(&dos),2);
            file.seekg(0x3c);file.read(reinterpret_cast<char*>(&offset),4);
            if (dos!=IMAGE_DOS_SIGNATURE || offset<64 || static_cast<std::uint64_t>(offset)+24>static_cast<std::uint64_t>(size))
                return std::unexpected(Error(ErrorKind::IncompatibleAbi,0,"XeSS DLL has an invalid PE offset"));
            file.seekg(offset);file.read(reinterpret_cast<char*>(&signature),4);file.read(reinterpret_cast<char*>(&machine),2);
            if (!file || signature!=IMAGE_NT_SIGNATURE) return std::unexpected(Error(ErrorKind::IncompatibleAbi,0,"XeSS DLL has an invalid PE signature"));
            if (machine!=IMAGE_FILE_MACHINE_AMD64) return std::unexpected(Error(ErrorKind::WrongArchitecture,machine,"XeSS requires an x64 runtime DLL"));
            return {};
        }
    }
    XessRuntime::~XessRuntime()
    {
        if (module_) FreeLibrary(module_);
        if (file_!=INVALID_HANDLE_VALUE) CloseHandle(file_);
    }
    Result<void> XessRuntime::Load(const std::filesystem::path& pluginDirectory)
    {
        if (!pluginDirectory.is_absolute()) return std::unexpected(Error(ErrorKind::InvalidInput,0,"XeSS plugin directory must be absolute"));
        const auto path=pluginDirectory/L"RaZkolbaS/XeSS/libxess.dll";
        if (module_) {
            if (SamePath(path,module_)) return {};
            return std::unexpected(Error(ErrorKind::IncompatibleAbi,0,"Cannot replace a retained XeSS runtime"));
        }
        const auto file=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if (file==INVALID_HANDLE_VALUE) {
            const auto native=GetLastError();
            return std::unexpected(Error(native==ERROR_FILE_NOT_FOUND || native==ERROR_PATH_NOT_FOUND ? ErrorKind::MissingRuntime : ErrorKind::IncompatibleAbi,
                native,"Cannot open XeSS runtime at SKSE/Plugins/RaZkolbaS/XeSS/libxess.dll"));
        }
        const auto pe=CheckPe(path);
        if (!pe) { CloseHandle(file);return pe; }
        if (const auto loaded=GetModuleHandleW(L"libxess.dll");loaded && !SamePath(path,loaded)) {
            CloseHandle(file);
            return std::unexpected(Error(ErrorKind::IncompatibleAbi,0,"A different libxess.dll is already loaded"));
        }
        const auto module=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            const auto native=GetLastError();CloseHandle(file);
            return std::unexpected(Error(ErrorKind::IncompatibleAbi,native,"Failed to load XeSS runtime or a dependency"));
        }
        XessFunctions functions{};
        auto unwind=[&](RuntimeError error)->Result<void> { FreeLibrary(module);CloseHandle(file);return std::unexpected(std::move(error)); };
        if (!SamePath(path,module)) return unwind(Error(ErrorKind::IncompatibleAbi,0,"Loaded XeSS runtime does not match requested path"));
#define XESS_RESOLVE(member,name) \
        functions.member=reinterpret_cast<decltype(functions.member)>(GetProcAddress(module,#name)); \
        if (!functions.member) return unwind(Error(ErrorKind::MissingExport,ERROR_PROC_NOT_FOUND,"XeSS export missing: " #name));
        XESS_RESOLVE(GetVersion,xessGetVersion)
        XESS_RESOLVE(GetIntelXeFXVersion,xessGetIntelXeFXVersion)
        XESS_RESOLVE(CreateContext,xessD3D12CreateContext)
        XESS_RESOLVE(GetOptimalInputResolution,xessGetOptimalInputResolution)
        XESS_RESOLVE(GetProperties,xessGetProperties)
        XESS_RESOLVE(BuildPipelines,xessD3D12BuildPipelines)
        XESS_RESOLVE(Init,xessD3D12Init)
        XESS_RESOLVE(GetInitParams,xessD3D12GetInitParams)
        XESS_RESOLVE(Execute,xessD3D12Execute)
        XESS_RESOLVE(SetJitterScale,xessSetJitterScale)
        XESS_RESOLVE(SetVelocityScale,xessSetVelocityScale)
        XESS_RESOLVE(SetLoggingCallback,xessSetLoggingCallback)
        XESS_RESOLVE(IsOptimalDriver,xessIsOptimalDriver)
        XESS_RESOLVE(DestroyContext,xessDestroyContext)
#undef XESS_RESOLVE
        xess_version_t version{};
        const auto result=functions.GetVersion(&version);
        if (result!=XESS_RESULT_SUCCESS || version.major!=2)
            return unwind(Error(ErrorKind::IncompatibleAbi,result,"XeSS dispatcher does not report the supported SR 2.x API"));
        module_=module;file_=file;path_=path;functions_=functions;
        return {};
    }
}
