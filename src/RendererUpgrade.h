#pragma once
#include <Windows.h>
#include <filesystem>

namespace TheosRenderPipeline::RendererUpgrade {
enum class Status { Ready, Conflict, Failed };
struct Result { Status status{Status::Ready}; DWORD native{}; std::filesystem::path path; };
inline bool Missing(DWORD error) { return error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND; }
inline Result Prepare(const std::filesystem::path& directory, bool oldModuleLoaded) {
    const auto oldDll=directory/L"TheosRenderPipeline.dll";
    if(oldModuleLoaded)return {Status::Conflict,0,oldDll};
    auto attributes=GetFileAttributesW(oldDll.c_str());
    if(attributes!=INVALID_FILE_ATTRIBUTES)return {Status::Conflict,0,oldDll};
    auto error=GetLastError();
    if(!Missing(error))return {Status::Failed,error,oldDll};

    const auto current=directory/L"RaZkolbaS.ini";
    attributes=GetFileAttributesW(current.c_str());
    if(attributes!=INVALID_FILE_ATTRIBUTES) {
        if(attributes&FILE_ATTRIBUTE_DIRECTORY)return {Status::Failed,ERROR_DIRECTORY,current};
        return {};
    }
    error=GetLastError();
    if(!Missing(error))return {Status::Failed,error,current};
    return {};
}
}
