# Application-controlled NVIDIA settings

RaZkolbaS filters NVIDIA App's DLSS/NR/FG override reads inside its NVIDIA runtime modules. It keeps the configured runtime bundle and lets the menu/INI control render mode, model preset and FG. It does not write NVIDIA driver profiles, the registry, or NVIDIA DLL files on disk. AMD's FSR path does not call this code.

Before NGX initialization, the host finds `_nvngx.dll` beside the NVIDIA user-mode driver loaded by the rendering device. It retains that core and replaces its `GetProcAddress` import with a scoped forwarding resolver. Streamline's configured `sl.interposer.dll` receives the same treatment before `slInit`. The native route also filters `sl.common.dll`'s import; Universal's RTX 20/30 route composes the reader inside its existing compatibility wrapper instead. The resolver forwards existing compatibility hooks, intercepting only the System32 NVAPI module's `nvapi_QueryInterface`. Only `NvAPI_DRS_GetSetting` reads of the explicit override key list are filtered. Missing settings, errors, unrelated options and returned metadata remain unchanged. This is an import shim, not a documented NVIDIA opt-out API.

Keys and default values come from [NVIDIA's public driver settings header](https://github.com/NVIDIA/nvapi/blob/main/NvApiDriverSettings.h). The minimal `NVDRS_SETTING` prefix uses the public V1 layout; the tested driver also uses that prefix in its larger V4 structure. References and callback code remain resident until process exit. Startup preparation failures stop initialization; the existing exact configured-runtime ownership checks remain in force.

## Validation on RTX 4080 SUPER

Universal's RTX 20/30 compatibility route keeps its checked `sl.common.dll` resolver import unchanged. The existing compatibility and provider NVAPI query wrappers compose the settings reader directly, preserving their architecture behavior and exact import ownership checks. The active driver core and Streamline interposer still receive their scoped import filters. A GPU fixture checks that the compatibility-owned import stays intact during real Streamline startup; Ampere/Turing wrapper tests exercise the production composition without claiming physical hardware qualification.

The negative GPU control simulates the FG override through a test-only read wrapper, without saving settings. Real NGX selects the cached 310.9.0 override and the configured `nvngx_dlssg.dll` is absent. With the production filter, the same simulated flag is returned as application-controlled and real NGX selects the configured 310.8.0 runtime. A third test verifies public Streamline initialization, device binding and FG support with the filter installed. Other NVIDIA generations and driver versions still need hardware qualification.

Run `NvidiaAppSettings`, `NvidiaAppSettingsGpu-control`, `NvidiaAppSettingsGpu-filtered`, `NvidiaAppSettingsGpu-streamline` and `NvidiaAppSettingsGpu-compatibility` in the Standard build. The control skips if the machine has no cached override runtime. No test changes Skyrim's or NVIDIA App's saved settings.

The game log records `[NVIDIA App Settings]` installation and suppressed values, alongside the existing `[SourceDLSSG] loaded` paths. If another module initialized NGX before this shim, it may have cached an unfiltered resolver; an unexpected selected runtime remains a startup error. Restarting the process is required after changing installed runtime files.

## Driver effects and startup diagnostics

On the actual NVIDIA rendering adapter, startup also reads the executable's DRS
profile, including inherited values. It uses the full executable path to select
the profile applied to that installation, and uses the current global profile
only when NVAPI reports that the application is absent. Failed reads remain unknown. The snapshot uses a separate,
read-only DRS session, destroyed on every exit; NVAPI initialization is balanced
with unload. Snapshot failure is diagnostic and does not block rendering. AMD
renderers skip these NVIDIA calls, and NVIDIA renderers get the snapshot when
using FSR too.

The snapshot reports Smooth Motion enable/API mask, RTX HDR, RTX Dynamic
Vibrance, frame limiting and the community-documented DLSS forced model profile.
These are **observations, not additional filtered settings**. Their IDs and Smooth
Motion's API-mask semantics are listed in [NVIDIA Profile Inspector's setting
metadata](https://github.com/Orbmu2k/nvidiaProfileInspector/blob/master/nvidiaProfileInspector/CustomSettingNames.xml).
The Smooth Motion/HDR/vibrance/profile-selection keys are not a published NVIDIA
runtime opt-out contract. In particular, do not infer that the absence of a
Smooth Motion setting means the driver effect is disabled.

Smooth Motion configured for DX11 produces a startup log warning. NVIDIA provides
a [per-program control in NVIDIA App](https://nvidia.custhelp.com/app/answers/detail/a_id/5621):
Graphics → Skyrim Program settings → Driver Settings → Smooth Motion → Off.
Change it before launching Skyrim when using RaZkolbaS frame generation. This
keeps other programs' preferences. RaZkolbaS does not save that change itself.

The log separately records whether `NvPresent64.dll` is loaded at the startup
boundary and its path when present. Module presence alone does not prove active
interpolation; absence at that boundary does not exclude a later load. The local
driver's NvPresent exports provide device/swapchain creation, destruction and
initialization entry points, with no exported disable operation. NVIDIA's public
NVAPI headers do not currently document a Smooth Motion runtime opt-out. A wider
NGX key whitelist therefore does not establish Smooth Motion suppression. A
process-local suppression route needs separate consumer/timing evidence and
hardware validation before shipping.

## Portable packaging and remaining driver qualification

The distributable INI leaves both NR runtime-root and driver-core overrides blank.
The NR model paths resolve within the game's virtual Data tree; core discovery
uses the NVIDIA rendering driver actually loaded after device creation. Trial
stagers clear both current and legacy path aliases. Their local core path stays
in research receipts for validation, and the MO2 metadata contains only the ZIP
basename. Probe wrappers require explicit local model/core inputs instead of
assuming a developer's Downloads directory or DriverStore folder.

**Discovery is not driver-version qualification.** NR still accepts the exact
qualified core identity in `RuntimeOwner.cpp`; an unfamiliar core returns the
observed and expected size/hash with its path. Supporting it requires a validated
core qualification change. The patched NR model hashes remain pinned, and a core
from another installed driver directory is not a portable fallback.

