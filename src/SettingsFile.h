#pragma once

#include "PublicIni.h"
#include <SimpleIni.h>
#include <cerrno>
#include <cstdio>
#include <memory>
#include <utility>

namespace TheosRenderPipeline::SettingsFile
{
inline std::string LegacyRuntimeNotice(const CSimpleIniA& ini)
{
    const auto* retired=ini.GetValue("NeuralRendering Advanced", "Runtime", "");
    if (std::string_view(retired) == "Legacy")
        return "Saved [NeuralRendering Advanced] Runtime=Legacy is retired and ignored; NR selects the bundled model for the render GPU. For intentional Legacy diagnostics use [Debug] NRLegacyRuntime=true.";
    return {};
}
inline std::pair<SI_Error, std::string> LoadRenderer(CSimpleIniA& ini, const wchar_t* path, std::string* notice = nullptr)
{
    if (notice) notice->clear();
    const auto result = ini.LoadFile(path);
    if (result < 0) return {result, "Cannot read Data/SKSE/Plugins/RaZkolbaS.ini. Install the matching INI from the package; startup will not guess source encoding or feature settings."};
    if (notice) {
        *notice=LegacyRuntimeNotice(ini);
        if(std::string_view(ini.GetValue("Upscaling", "Upscaler", ""))=="XeSS" &&
            std::string_view(ini.GetValue("FrameGeneration", "Backend", ""))=="NVIDIA") {
            if(!notice->empty())notice->append(" ");
            notice->append("[FrameGeneration] Backend=NVIDIA is unavailable with XeSS; using Auto, SR-only presentation and FG off. Select Backend=FSR and restart for experimental XeSS + FSR FG.");
        }
    }
    return {result, PublicIni::Decode(ini)};
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
