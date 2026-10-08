#pragma once
#include "OverlayLayout.h"
#include <SimpleIni.h>
#include <string>

namespace TheosRenderPipeline::Overlay
{
struct LayoutFileResult
{
    Layout layout;
    std::string warning;
};

// Geometry is optional and has its own sanitization. Renderer configuration
// remains validated by the strict startup loader, before overlay initialization.
inline LayoutFileResult LoadMenuLayout(const wchar_t* path)
{
    CSimpleIniA ini;
    ini.SetUnicode();
    const auto result = ini.LoadFile(path);
    if (result < 0)
        return {Layout{}, "Menu layout INI read failed (rc=" + std::to_string(result) + "); using default layout."};
    return {LoadLayout(ini), {}};
}
}
