#pragma once

#include "PublicIni.h"
#include <SimpleIni.h>
#include <cerrno>
#include <cstdio>
#include <memory>
#include <utility>

namespace TheosRenderPipeline::SettingsFile
{
inline std::pair<SI_Error, std::string> LoadRenderer(CSimpleIniA& ini, const wchar_t* path)
{
    const auto result = ini.LoadFile(path);
    return {result, result < 0 ? std::string{} : PublicIni::Decode(ini)};
}
inline SI_Error LoadForUpdate(CSimpleIniA& ini, const wchar_t* path)
{
    FILE* file = nullptr;
    const auto error = _wfopen_s(&file, path, L"rb");
    if (error != 0)
    {
        // A missing file may be created. An unreadable file must be preserved.
        return error == ENOENT ? SI_OK : SI_FILE;
    }
    const std::unique_ptr<FILE, decltype(&std::fclose)> input(file, &std::fclose);
    return ini.LoadFile(input.get());
}
inline std::pair<SI_Error, std::string> LoadRendererForUpdate(CSimpleIniA& ini, const wchar_t* path)
{
    const auto result = LoadForUpdate(ini, path);
    if (result < 0) return {result, {}};
    CSimpleIniA::TNamesDepend sections;
    ini.GetAllSections(sections);
    if (sections.empty()) ini.SetValue("Upscaling", "Upscaler", "DLSS");
    return {result, PublicIni::Decode(ini)};
}
} // namespace TheosRenderPipeline::SettingsFile
