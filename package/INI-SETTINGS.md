# INI settings

`SKSE/Plugins/RaZkolbaS.ini` uses the current section layout. Comments beside each
key describe its range, backend and restart requirements. The layout resembles
AIO19's grouped settings, but values retain DvaKolbas's meanings: DLSS preset
numbers, FG backend numbers and generated-frame counts are not AIO19 indexes.

| Section | Purpose |
| --- | --- |
| Settings | Upscaler selection, shared sharpening, jitter, exposure, native UI |
| DLSS | NVIDIA quality and NGX model preset |
| FSR | Quality, provider, sharpening and source color encoding |
| FrameGeneration | Backend, live enable request, NVIDIA multiplier, Reflex, HUD handling |
| NeuralRendering | Live enable request, placement, pass count, combat policy, startup profile |
| NR PASS 1 | First-pass style, tone, structure, preset, scale and resolve controls |
| NR PASS 2 | Saved independent second-pass overrides and linking |
| NR PASS 3 | Saved independent third-pass overrides and linking |
| Hotkeys / Menu | Keyboard shortcuts and saved window layout |
| Appearance | Optional weather, time and interior presets |
| Compatibility / Runtime | Integration preferences and startup runtime paths |
| HDROutput / DynamicResolution / Performance / Debug | Display, unsupported dynamic resolution, timing and logging |

Current qualified community After rendering order remains **DLSS/FSR → NR → FG → UI**.
After supports fixed DLSS and FSR render scales, including Native. Its enhanced
image stays at display size while depth and motion stay at render size.
Dynamic resolution and HDR NR remain unavailable.
Community NR supports one to three sequential passes. The legacy NR runtime
executes at most two passes, with a warning when a saved three-pass preference
is used. Saving unrelated settings retains that preference and the Pass 3
overrides for a later return to the community runtime.
Reorganizing settings does not extend hardware qualification. Scaled After has
standalone GPU evidence on RTX 4080 SUPER; its Skyrim visual check is pending.
AMD NR remains unsupported. These limits are enforced by the same runtime policies.

## AMD first launch

AMD uses **FSR / Native AA → optional FSR FG → UI**. DLSS, DLAA, NVIDIA FG and
NR are unavailable. Startup detects Skyrim's actual rendering adapter and
normalizes incompatible NVIDIA choices in memory. Save as default persists the
effective startup settings; merely launching does not rewrite the INI.

Before launching, edit `SKSE/Plugins/RaZkolbaS.ini` in the installed mod
(the virtual `Data` tree when using MO2). In `[FSR]`, set `SourceColorEncoding` to
`Linear`, `Gamma22` or `SRGB`, matching the verified Skyrim/ENB source. Preserve
an existing known-working value for the same setup. GPU model and texture format
do not establish color encoding. If the source is not verified, keep `Unknown`:
startup stops with instructions rather than processing incorrectly encoded color.

`ProviderPolicy=Analytical` selects FSR3.1.5. `Compatible` selects official FSR4
when the actual AMD adapter advertises it, otherwise FSR3. `MachineLearning`
requires FSR4 and reports an error if unavailable. Changes require a restart.
On NVIDIA, explicit `MachineLearning` selects the separately pinned INT8 4.0.2b
runtime in `SKSE/Plugins/FSR/INT8`, using its verified 4.0.3 API contract.
`Analytical` and `Compatible` retain the official runtime in `SKSE/Plugins/FSR`;
there is no live runtime swap and no silent FSR3 fallback from explicit FSR4.
The INT8 path requires shader model 6.6. It has standalone qualification on the
RTX4080 SUPER; other NVIDIA cards and Skyrim gameplay remain to be qualified.
Official FSR4 requires shader model 6.6; the SDK's device catalog determines
availability. The bundled SDK2.3.0 contains FSR4.1.1. Positive FSR4 GPU/gameplay
qualification is still pending on AMD hardware for this preview.

Auto can retry FSR3 after an FSR4 startup failure only after successful context
cleanup and only when both providers use the already allocated render size.
Device loss, failed cleanup and dispatch errors do not trigger a retry.
`[FrameGeneration] FsrProviderPolicy` selects FG independently of SR:
`Analytical` keeps FG 3.1.6 (the default), `Compatible` prefers device-catalog
ML FG 4.0.1 and otherwise selects 3.1.6, and `MachineLearning` requires ML FG
4.0.1 without an implicit analytical fallback. Save and restart to change the
provider; interpolation off/on remains live. Swapchain 3.1.7 is retained.
ML FG is experimental pending RX 9070 qualification. AMD documents Windows 11,
RX 9000 or later, and DirectX 12 Agility SDK 1.4.9 or later as requirements:
https://gpuopen.com/manuals/fsr_sdk/techniques/frame-interpolation-ml/ .
The RTX 4080 SUPER catalogs expose only analytical FG; the NVIDIA INT8 SR
runtime supplies FSR4 upscaling, not ML FG. Logs/menu report the selected and
created FG algorithm separately from the API 4.0.1 version.
Ordering remains FSR → NR (where supported) → FG → UI.

Normal provider selection uses `[FrameGeneration] Backend=1` for DLSS/DLAA on
NVIDIA, and `Backend=2` for FSR on NVIDIA or AMD. The FG toggle retains the backend
when interpolation is off. `Backend=0` is reserved for diagnostic SR-only testing;
it is available through the INI or developer controls, rather than the normal menu.
Backend changes require restart. With backend 2 already active,
the FG checkbox toggles interpolation live. A saved NVIDIA FG request is not
automatically converted into enabled FSR FG.

When saving FSR for a later restart from a legacy NVIDIA NR session, the current
NR pass stays active. Live NR off/on still addresses the current NVIDIA owner;
the next-launch FSR NR preference remains disabled.

## Existing files

For portable NR installs, leave `[Runtime] NRDriverCore` empty. After device
creation, NR selects `_nvngx.dll` beside the game's loaded NVIDIA rendering
driver (`nvwgf2umx.dll`). It does not search other installed driver versions.
An explicit path overrides discovery and is retained when saving settings.
The startup log records the selected path and its origin; file failures name
the exact artifact and Windows error code. The existing driver-core size/hash
qualification still applies: automatic path discovery is not qualification of
a different driver binary.

Only the current layout is supported. Obsolete `[Settings] QualityLevel/DLSSPreset`,
`[SourceDLSSG]`, `[Experimental]` runtime selectors and `[Overlay]` geometry are
ignored. There is no `ConfigVersion` marker or automatic copy from
`TheosRenderPipeline.ini`. Relative runtime overrides resolve exactly as written;
old resource folder names are not redirected.

**Save as default** writes current sections and removes recognized obsolete keys.
Unknown settings and current startup paths edited on disk since launch are retained.
Missing pass-2 and pass-3 overrides inherit pass 1. Install the current packaged
INI when upgrading from an old layout; retain the matching old DLL/INI for rollback.

The optional `tools/ini/Reorganize-Ini.ps1` formatter writes a separate current-layout
file with template ordering and comments. It discards obsolete keys without
converting their values and never fills missing settings from the template.
