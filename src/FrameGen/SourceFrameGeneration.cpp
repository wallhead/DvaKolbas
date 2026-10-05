#include "SourceFrameGeneration.h"
#include <PCH.h>
#include <SimpleIni.h>
#include "RendererGpuPolicy.h"

void SourceFrameGeneration::LoadINI(std::uint32_t adapterVendorId)
{
    CSimpleIniA ini;
    ini.SetUnicode();
    const auto result = ini.LoadFile(L"Data\\SKSE\\Plugins\\RaZkolbaS.ini");
    TheosRenderPipeline::ApplyRendererGpuPolicy(ini, adapterVendorId,
#if defined(TRP_ENABLE_FSR_FG)
        true
#else
        false
#endif
    );
    LoadStartupPreferences(ini);
    const TheosRenderPipeline::IniLayout::ReadView read(ini);
    logger::info("[NvidiaHost] startup INI=Data/SKSE/Plugins/RaZkolbaS.ini readResult={} SourceDLSSGMFGUnlock={} origin={} raw={}",
        static_cast<int>(result), settings.sourceDLSSGMFGUnlock,
        settings.sourceDLSSGMFGUnlockPresent ? "INI" : "packaged-default",
        read.GetValue("Experimental", "SourceDLSSGMFGUnlock", "<missing>"));
    logger::info("[NvidiaHost] required; startup interpolation={} UI composition mode={}", settings.enabled, settings.nativeUICompositionMode);
}

double SourceFrameGeneration::GetRefreshRate(HWND a_window)
{
    const auto monitor = ::MonitorFromWindow(a_window, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFOEXW monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!::GetMonitorInfoW(monitor, &monitorInfo))
    {
        return 0.0;
    }
    DEVMODEW devMode{};
    devMode.dmSize = sizeof(devMode);
    if (!::EnumDisplaySettingsW(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode))
    {
        return 0.0;
    }
    return static_cast<double>(devMode.dmDisplayFrequency);
}
