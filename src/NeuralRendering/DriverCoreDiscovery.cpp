#include "StartupSettings.h"
#include <Windows.h>
#include <array>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
bool MissingDriverStoreOverride(const std::filesystem::path& requested) {
    const auto path=requested.lexically_normal();
    if(_wcsicmp(path.filename().c_str(),L"_nvngx.dll")!=0)return false;
    // Recover only the OS DriverStore layout used by older portable packages.
    // Never replace a custom file, an existing override or an access failure.
    std::array<wchar_t,32768> windows{};
    const auto size=GetWindowsDirectoryW(windows.data(),static_cast<UINT>(windows.size()));
    if(!size||size>=windows.size())return false;
    const auto repository=(std::filesystem::path(windows.data())/L"System32/DriverStore/FileRepository").lexically_normal();
    if(_wcsicmp(path.parent_path().parent_path().c_str(),repository.c_str())!=0)return false;
    if(GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES)return false;
    const auto error=GetLastError();
    return error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND;
}
}
Result<std::filesystem::path> StartupSettings::ResolveDriverCore() const {
    bool recovered{};
    if(!driverCore.empty()) {
        if(!driverCore.is_absolute())
            return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR driver-core override must be resolved to an absolute path"});
        recovered=MissingDriverStoreOverride(driverCore);
        if(!recovered)return driverCore;
    }
    const auto discoveryFailure=[&](ErrorKind kind,DWORD code,std::string message) {
        if(recovered)message="NR missing DriverStore override "+driverCore.string()+"; automatic discovery attempted: "+message;
        return std::unexpected(Error{kind,code,std::move(message)});
    };
    struct Driver {
        HMODULE module{};
        ~Driver(){if(module)FreeLibrary(module);}
    } driver;
    // Device creation loads the NVIDIA user-mode rendering driver. Retain it
    // while reading its path, then select only its sibling NGX core.
    if(!GetModuleHandleExW(0,L"nvwgf2umx.dll",&driver.module)) {
        const auto code=GetLastError();
        return discoveryFailure(ErrorKind::Unsupported,code,
            "NR automatic driver-core discovery requires a loaded NVIDIA rendering driver (nvwgf2umx.dll); native="+std::to_string(code));
    }
    std::array<wchar_t,32768> path{};
    const auto size=GetModuleFileNameW(driver.module,path.data(),static_cast<DWORD>(path.size()));
    if(!size||size>=path.size()) {
        const auto code=size>=path.size()?ERROR_INSUFFICIENT_BUFFER:GetLastError();
        return discoveryFailure(ErrorKind::Io,code,"Cannot inspect active NVIDIA rendering-driver path; native="+std::to_string(code));
    }
    const std::filesystem::path loaded(std::wstring_view(path.data(),size));
    if(!loaded.is_absolute())
        return discoveryFailure(ErrorKind::InvalidInput,0,"Active NVIDIA rendering-driver path is not absolute");
    return loaded.parent_path()/L"_nvngx.dll";
}
}
