# Application-controlled NVIDIA settings

RaZkolbaS filters NVIDIA App's DLSS/NR/FG override reads inside its NVIDIA runtime modules. It keeps the configured runtime bundle and lets the menu/INI control render mode, model preset and FG. It does not write NVIDIA driver profiles, the registry, or NVIDIA DLL files on disk. AMD's FSR path does not call this code.

Before NGX initialization, the host finds `_nvngx.dll` beside the NVIDIA user-mode driver loaded by the rendering device. It retains that core and replaces its `GetProcAddress` import with a scoped forwarding resolver. Streamline's configured `sl.interposer.dll` receives the same treatment before `slInit`. The native route also filters `sl.common.dll`'s import; Universal's RTX 20/30 route composes the reader inside its existing compatibility wrapper instead. The resolver forwards existing compatibility hooks, intercepting only the System32 NVAPI module's `nvapi_QueryInterface`. Only `NvAPI_DRS_GetSetting` reads of the explicit override key list are filtered. Missing settings, errors, unrelated options and returned metadata remain unchanged. This is an import shim, not a documented NVIDIA opt-out API.

Keys and default values come from [NVIDIA's public driver settings header](https://github.com/NVIDIA/nvapi/blob/main/NvApiDriverSettings.h). The minimal `NVDRS_SETTING` prefix uses the public V1 layout; the tested driver also uses that prefix in its larger V4 structure. References and callback code remain resident until process exit. Startup preparation failures stop initialization; the existing exact configured-runtime ownership checks remain in force.

## Validation on RTX 4080 SUPER

Universal's RTX 20/30 compatibility route keeps its checked `sl.common.dll` resolver import unchanged. The existing compatibility and provider NVAPI query wrappers compose the settings reader directly, preserving their architecture behavior and exact import ownership checks. The active driver core and Streamline interposer still receive their scoped import filters. A GPU fixture checks that the compatibility-owned import stays intact during real Streamline startup; Ampere/Turing wrapper tests exercise the production composition without claiming physical hardware qualification.

The negative GPU control simulates the FG override through a test-only read wrapper, without saving settings. Real NGX selects the cached 310.9.0 override and the configured `nvngx_dlssg.dll` is absent. With the production filter, the same simulated flag is returned as application-controlled and real NGX selects the configured 310.8.0 runtime. A third test verifies public Streamline initialization, device binding and FG support with the filter installed. Other NVIDIA generations and driver versions still need hardware qualification.

Run `NvidiaAppSettings`, `NvidiaAppSettingsGpu-control`, `NvidiaAppSettingsGpu-filtered`, `NvidiaAppSettingsGpu-streamline` and `NvidiaAppSettingsGpu-compatibility` in the Standard build. The control skips if the machine has no cached override runtime. No test changes Skyrim's or NVIDIA App's saved settings.

The game log records `[NVIDIA App Settings]` installation and suppressed values, alongside the existing `[SourceDLSSG] loaded` paths. If another module initialized NGX before this shim, it may have cached an unfiltered resolver; an unexpected selected runtime remains a startup error. Restarting the process is required after changing installed runtime files.

