#pragma once
#include <Windows.h>
#include <filesystem>

namespace TheosRenderPipeline::RendererUpgrade {
enum class Status { Ready, Migrated, Conflict, Failed };
struct Result { Status status{Status::Ready}; DWORD native{}; std::filesystem::path path; };
inline bool Missing(DWORD error) { return error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND; }
inline Result Prepare(const std::filesystem::path& directory, bool oldModuleLoaded) {
    const auto oldDll=directory/L"TheosRenderPipeline.dll";
    if(oldModuleLoaded)return {Status::Conflict,0,oldDll};
    auto attributes=GetFileAttributesW(oldDll.c_str());
    if(attributes!=INVALID_FILE_ATTRIBUTES)return {Status::Conflict,0,oldDll};
    auto error=GetLastError();
    if(!Missing(error))return {Status::Failed,error,oldDll};

    const auto current=directory/L"RaZkolbaS.ini",legacy=directory/L"TheosRenderPipeline.ini";
    attributes=GetFileAttributesW(current.c_str());
    if(attributes!=INVALID_FILE_ATTRIBUTES) {
        if(attributes&FILE_ATTRIBUTE_DIRECTORY)return {Status::Failed,ERROR_DIRECTORY,current};
        return {};
    }
    error=GetLastError();
    if(!Missing(error))return {Status::Failed,error,current};
    attributes=GetFileAttributesW(legacy.c_str());
    if(attributes==INVALID_FILE_ATTRIBUTES) {
        error=GetLastError();
        return Missing(error)?Result{}:Result{Status::Failed,error,legacy};
    }
    if(attributes&FILE_ATTRIBUTE_DIRECTORY)return {Status::Failed,ERROR_DIRECTORY,legacy};

    // Copy to a unique temporary file, then publish without replacing an INI
    // that may have appeared in the meantime. Keep the legacy file untouched.
    wchar_t temporary[MAX_PATH]{};
    if(!GetTempFileNameW(directory.c_str(),L"RZK",0,temporary))return {Status::Failed,GetLastError(),current};
    struct Temporary {const wchar_t* path;~Temporary(){DeleteFileW(path);}} cleanup{temporary};
    if(!CopyFileW(legacy.c_str(),temporary,FALSE))return {Status::Failed,GetLastError(),legacy};
    if(!MoveFileExW(temporary,current.c_str(),MOVEFILE_WRITE_THROUGH)) {
        error=GetLastError();
        if(error==ERROR_ALREADY_EXISTS||error==ERROR_FILE_EXISTS)return {};
        return {Status::Failed,error,current};
    }
    return {Status::Migrated,0,current};
}
}
