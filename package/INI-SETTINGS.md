# INI settings

`SKSE/Plugins/RaZkolbaS.ini` uses layout version 2. Comments beside each
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

AMD currently uses **Analytical 3.1.5**. The menu shows this fixed provider and
does not offer Compatible/ML until that path has been validated. This provider
policy is not a claim of completed AMD hardware testing.

`[FrameGeneration] Backend=0` selects FSR without FG; `Backend=2` selects the FSR
FG presenter. Backend changes require restart. With backend 2 already active,
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

Old `[Settings] QualityLevel/DLSSPreset`, `[SourceDLSSG]`, `[Experimental]` runtime
selectors and `[Overlay]` geometry remain accepted. Canonical new keys take
precedence individually, including explicit zero, false and empty path values.
Missing pass-2 overrides keep the existing pass-1 inheritance.

**Save as default** migrates recognized old keys to the new sections, removes the
recognized duplicates and retains unknown settings. Startup paths edited on disk
since launch are retained. Older DLLs do not understand the new sections: roll
back the DLL and its matching old INI together.

For an offline conversion, with Skyrim and MO2 closed before installing the result:

```powershell
pwsh -NoProfile -File tools/ini/Reorganize-Ini.ps1 `
  -SourceIni 'path/to/current/RaZkolbaS.ini' `
  -OutputIni 'path/to/new/RaZkolbaS.ini'
```

The converter uses the commented packaged template for order and descriptions.
It writes a new file, checks that existing values are preserved, and never fills
missing optional settings with template defaults. Unknown keys and their attached
comments remain. Back up the working DLL and INI before installing a new build.
