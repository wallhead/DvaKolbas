#include "StartupSettings.h"
#include <Windows.h>
#include <array>
namespace TheosRenderPipeline::NeuralRendering {
Result<std::filesystem::path> StartupSettings::ResolveDriverCore() const {
    if(!driverCore.empty()) {
        if(!driverCore.is_absolute())
            return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR driver-core override must be resolved to an absolute path"});
        return driverCore;
    }
    struct Driver {
        HMODULE module{};
        ~Driver(){if(module)FreeLibrary(module);}
    } driver;
    // Device creation loads the NVIDIA user-mode rendering driver. Retain it
    // while reading its path, then select only its sibling NGX core.
    if(!GetModuleHandleExW(0,L"nvwgf2umx.dll",&driver.module)) {
        const auto code=GetLastError();
        return std::unexpected(Error{ErrorKind::Unsupported,code,
            "NR automatic driver-core discovery requires a loaded NVIDIA rendering driver (nvwgf2umx.dll); native="+std::to_string(code)});
    }
    std::array<wchar_t,32768> path{};
    const auto size=GetModuleFileNameW(driver.module,path.data(),static_cast<DWORD>(path.size()));
    if(!size||size>=path.size()) {
        const auto code=size>=path.size()?ERROR_INSUFFICIENT_BUFFER:GetLastError();
        return std::unexpected(Error{ErrorKind::Io,code,"Cannot inspect active NVIDIA rendering-driver path; native="+std::to_string(code)});
    }
    const std::filesystem::path loaded(std::wstring_view(path.data(),size));
    if(!loaded.is_absolute())
        return std::unexpected(Error{ErrorKind::InvalidInput,0,"Active NVIDIA rendering-driver path is not absolute"});
    return loaded.parent_path()/L"_nvngx.dll";
}
}
