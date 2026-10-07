# INI settings

Edit `SKSE/Plugins/RaZkolbaS.ini`. This build reads the current named layout only; old files must be converted explicitly or replaced with the matching packaged INI. No ConfigVersion is needed.

`[Upscaling] Upscaler` chooses DLSS or FSR. Both provider sections use `Quality=Native`. DLSS Native selects DLAA internally. Each provider retains its own quality and sharpness. FG on/off is live and keeps its presenter available; the normal presenter is derived from the upscaler.

FSR providers are independent: `[FSR] Provider=FSR4` requires ML upscaling, while `[FrameGeneration] FsrProvider=FSR4` requires ML frame generation. `Auto` allows the qualified analytical fallback. Explicit FSR4 reports an error when unavailable. FSR4 FG remains experimental. API versions in the log are not algorithm versions.

FSR and NR source color encoding remain independent because they can process different pipeline formats. Do not infer encoding from texture format. Unknown prevents the corresponding feature from starting. Provider, runtime, encoding, quality and keys marked restart require Save and restart.

The file stays organized as one INI: ordinary controls first, advanced settings below, program-owned menu geometry last. Linked NR passes retain their independent saved overrides so unlinking after a restart restores them. Unknown user keys and comments survive menu saves.

Defaults and this table are generated from `tools/ini/schema.json` by `tools/ini/Generate-Ini.py`.

To convert a previous organized INI, run `pwsh -NoProfile -File tools/ini/Reorganize-Ini.ps1 -SourceIni "old.ini" -OutputIni "named.ini"` from the source checkout. Choose a new output path; the source is never overwritten. Review the output before installing it with the matching DLL. Obsolete legacy aliases are discarded rather than promoted.

| Section | Setting | Default | Restart | Meaning |
|---|---|---|---|---|
| Upscaling | Upscaler | `DLSS` | Yes | DLSS or FSR. Native is selected in the provider quality section. Values: DLSS, FSR. |
| Upscaling | MipLodBias | `Auto` | No | Auto, or a numeric manual bias such as -1.0. Values: Auto, or a numeric bias. |
| Upscaling | EnableJitter | `true` | No | Apply sub-pixel camera jitter for temporal reconstruction. |
| Upscaling | AutoExposure | `true` | No | Allow the upscaler to estimate scene exposure. |
| DLSS | Sharpness | `0.672` | No | DLSS sharpening, 0-1. Zero disables sharpening. |
| DLSS | Quality | `Native` | Yes | Render scale; Native means DLAA at output resolution. Values: Native, Quality, Balanced, Performance, UltraPerformance, UltraQuality. |
| DLSS | Preset | `K` | Yes | Requested DLSS model preset; the NVIDIA runtime determines availability. Values: Default, E, F, J, K, L, M. |
| FSR | Quality | `Native` | Yes | Render scale for FSR. Values: Native, Quality, Balanced, Performance. |
| FSR | Provider | `FSR3` | Yes | FSR3 keeps the analytical runtime; Auto allows fallback; FSR4 requires ML. Values: FSR3, Auto, FSR4. |
| FSR | Sharpness | `0.0` | No | FSR sharpening, 0-1; 0 disables it. |
| FSR | SourceColorEncoding | `Unknown` | Yes | Unknown, Linear, Gamma22 or SRGB. Select the actual source encoding before enabling FSR. |
| FrameGeneration | Enabled | `true` | No | FG toggle. Disabling interpolation retains the selected presentation host. |
| FrameGeneration | FsrProvider | `FSR3` | Yes | FG provider, independent of upscaling. FSR4 ML FG is experimental; unsupported devices report an error. Values: FSR3, Auto, FSR4. |
| FrameGeneration | NvidiaGeneratedFrames | `1` | No | NVIDIA: generated frames exclude the real frame. 1=x2, 2=x3, ... 5=x6. Effective count is limited by the GPU/runtime. FSR FG currently generates one frame. |
| FrameGeneration | NvidiaDynamicMFG | `false` | No | NVIDIA dynamic MFG, if supported by the runtime. |
| FrameGeneration | DynamicTargetFPS | `0` | No | 0=monitor refresh; otherwise 61-1000 output FPS. |
| FrameGeneration | NvidiaUIRecomposition | `true` | No | NVIDIA: interpolate the HUD-less scene and UI separately. |
| Latency | OutputFPSLimit | `0` | No | NVIDIA output FPS cap, 0=unlimited; 1-1000. |
| Latency | Reflex | `On` | No | NVIDIA Reflex mode. Values: Off, On, Boost. |
| Interface | NativeUI | `true` | No | Keep HUD/menu rendering at the output resolution. Required by community NR/FSR FG. |
| Interface | RequestLoadingArtwork | `true` | No | Request loading artwork on eligible cell transitions. |
| Interface | UIComposition | `Dedicated` | Yes | FSR FG and community NR require Dedicated. Values: Dedicated, HudlessDetection. |
| NeuralRendering | Enabled | `false` | No | NR toggle; requires a compatible installed runtime. |
| NeuralRendering | Placement | `Before` | No | After order: DLSS/FSR -> NR -> FG -> UI. Values: Before, After. |
| NeuralRendering | PassCount | `1` | No | Requested passes: 1-3. Legacy executes at most 2 and keeps the saved choice. |
| NeuralRendering | OnePassInCombat | `false` | No | Temporarily reduce two requested passes to one under selected conditions. |
| NeuralRendering | OnePassWeaponsDrawn | `false` | No | Temporarily reduce two requested passes to one while weapons or spells are drawn. |
| NeuralRendering | PassRecoverySeconds | `5` | No | Recovery delay after conditions clear, 0-30 seconds of active gameplay. |
| NR PASS 1 | Style | `0` | No | Style sent to the model, 0-7; community game trials have checked 0-2. |
| NR PASS 1 | Intensity | `1.0` | No | Effect/tone/structure strengths, 0-2. 0 disables the corresponding contribution. |
| NR PASS 1 | Tone | `1.0` | No | NR tone contribution, 0-2; zero disables it. |
| NR PASS 1 | Structure | `1.0` | No | NR structure contribution, 0-2; zero disables it. |
| NR PASS 1 | SkinStructure | `1.0` | No | -1 copies structure automatically when the runtime supports it; otherwise 0-2. |
| NR PASS 1 | AutoSkin | `false` | No | Let the NR runtime choose skin structure automatically. |
| NR PASS 1 | UICorrection | `false` | No | Keep false for a world-only pass with dedicated UI. |
| NR PASS 2 | UseSameSettings | `true` | No | Runs when PassCount is 2 or 3. Linked settings use pass 1. Unlinking restores these independent overrides; relinking does not erase them. |
| NR PASS 2 | Style | `0` | No | Style sent to the model, 0-7; community game trials have checked 0-2. |
| NR PASS 2 | Intensity | `1.0` | No | Effect/tone/structure strengths, 0-2. 0 disables the corresponding contribution. |
| NR PASS 2 | Tone | `1.0` | No | NR tone contribution, 0-2; zero disables it. |
| NR PASS 2 | Structure | `1.0` | No | NR structure contribution, 0-2; zero disables it. |
| NR PASS 2 | SkinStructure | `1.0` | No | -1 copies structure automatically when the runtime supports it; otherwise 0-2. |
| NR PASS 2 | AutoSkin | `false` | No | Let the NR runtime choose skin structure automatically. |
| NR PASS 2 | UICorrection | `false` | No | Keep false for a world-only pass with dedicated UI. |
| NR PASS 3 | UseSameSettings | `true` | No | Community NR only. Runs when PassCount=3; linked settings use pass 1. |
| NR PASS 3 | Style | `0` | No | Style sent to the model, 0-7; community game trials have checked 0-2. |
| NR PASS 3 | Intensity | `1.0` | No | Effect/tone/structure strengths, 0-2. 0 disables the corresponding contribution. |
| NR PASS 3 | Tone | `1.0` | No | NR tone contribution, 0-2; zero disables it. |
| NR PASS 3 | Structure | `1.0` | No | NR structure contribution, 0-2; zero disables it. |
| NR PASS 3 | SkinStructure | `1.0` | No | -1 copies structure automatically when the runtime supports it; otherwise 0-2. |
| NR PASS 3 | AutoSkin | `false` | No | Let the NR runtime choose skin structure automatically. |
| NR PASS 3 | UICorrection | `false` | No | Keep false for a world-only pass with dedicated UI. |
| Hotkeys | ToggleOverlay | `End` | Yes | Key name (End, Insert, F1-F12) or a hex virtual-key code. Values: key name, hex code or decimal virtual-key code. |
| Hotkeys | EnableNRHotkeys | `false` | Yes | opt-in to [=NR off and ]=NR on, including with the menu closed. |
| Appearance | Enabled | `false` | No | Optional weather/time/interior profiles for NR and sharpening. Edit in the Neural Rendering tab; shared profile files remain separate. |
| Appearance | SmoothingSeconds | `2.0` | No | Transition time for appearance presets, in seconds. |
| Appearance | WeatherCount | `0` | No | Program-owned count of saved weather appearance entries. |
| Upscaling Advanced | FsrOrdinaryPresenter | `false` | Yes | Diagnostic only: ordinary FSR presentation, with FG unavailable. Leave false. |
| NeuralRendering Advanced | Runtime | `Legacy` | Yes | Select the installed NR runtime path. Values: Legacy, Community. |
| NeuralRendering Advanced | Profile | `Auto` | Yes | GPU profile: Auto, rtx20-30, rtx40 or rtx50. AMD NR is not yet supported. |
| NeuralRendering Advanced | SourceColorEncoding | `Unknown` | Yes | NR source encoding: Unknown, Linear, Gamma22 or SRGB. |
| NeuralRendering Advanced | SdrBytesTrial | `false` | Yes | encoded SDR byte path. Required by the qualified community SDR trial. |
| NR PASS 1 Advanced | Preset | `0` | No | Model preset: 0=default, 1=shipping. Community trial requires 0. |
| NR PASS 1 Advanced | InputScale | `1.0` | No | 0 or >=1=native; otherwise 0.25-1. Community trial requires 1. |
| NR PASS 1 Advanced | ResolveMethod | `Auto` | No | Advanced reconstruction mode; qualified SDR path uses Auto. Values: Auto, Residual, Ratio. |
| NR PASS 1 Advanced | TransferStrength | `1.0` | No | Ratio resolve controls; strengths 0-2. |
| NR PASS 1 Advanced | ColourStrength | `1.0` | No | NR colour contribution sent to the model. |
| NR PASS 1 Advanced | MaxRatio | `2.0` | No | Ratio clamp 0.01-16; white point 0.0001-10000. |
| NR PASS 1 Advanced | WhitePoint | `1.0` | No | NR input white-point multiplier. |
| NR PASS 1 Advanced | ColorIsHDR | `false` | No | Advanced NR colour-domain flag; must match the actual input. |
| NR PASS 1 Advanced | PeripheralCompression | `false` | No | Experimental preparation options; retain false for the qualified community trial. |
| NR PASS 1 Advanced | FusedPreparation | `false` | No | Experimental combined NR preparation; leave disabled unless testing. |
| NR PASS 2 Advanced | Preset | `0` | No | Model preset: 0=default, 1=shipping. Community trial requires 0. |
| NR PASS 2 Advanced | InputScale | `1.0` | No | 0 or >=1=native; otherwise 0.25-1. Community trial requires 1. |
| NR PASS 3 Advanced | Preset | `0` | No | Model preset: 0=default, 1=shipping. Community trial requires 0. |
| NR PASS 3 Advanced | InputScale | `1.0` | No | 0 or >=1=native; otherwise 0.25-1. Community trial requires 1. |
| HDROutput | Enabled | `false` | Yes | Experimental HDR10 from a finished SDR image. Requires Windows HDR and a restart. FSR does not support this path. With Community Shaders use its HDR Display. |
| HDROutput | MatchWindowsSDRBrightness | `true` | No | Use the Windows SDR brightness setting for HDR paper white. |
| HDROutput | PaperWhiteNits | `200` | No | HDR paper-white brightness in nits. |
| HDROutput | PeakNits | `1000` | No | HDR peak display brightness in nits. |
| HDROutput | UIBrightnessNits | `200` | No | HDR HUD/menu brightness in nits. |
| HDROutput | HighlightStrength | `1` | No | Expansion strength 0-1; expansion start 0.1-0.95. |
| HDROutput | ExpansionStart | `0.7` | No | Start of SDR-to-HDR highlight expansion, 0-1. |
| HDROutput | SDRTransfer | `Gamma22` | No | SDR decoding for experimental HDR output. Values: Gamma22, SRGB. |
| Compatibility | ReShadeBeforeUpscaling | `false` | No | ReShade effects run before (true) or after (false) upscaling. |
| Compatibility | WheelerLateOverlayBridge | `true` | No | Enable the compatibility bridge for Wheeler overlays. |
| Compatibility | NvidiaMFGUnlock | `true` | No | Universal-build MFG compatibility; ignored by the Standard build. |
| Runtime | StreamlineDirectory | `RaZkolbaS/NVIDIA/Streamline` | Yes | paths relative to virtual Data/SKSE/Plugins, or intentional absolute paths. |
| Runtime | NRRuntimePath | `RaZkolbaS/NVIDIA/nvngx_dlssnr.dll` | Yes | Legacy NR runtime path, relative to SKSE/Plugins unless absolute. |
| Runtime | NRRuntimeRoot | `` | Yes | Community catalog root and driver core use Data/SKSE/Plugins/RaZkolbaS as base. Empty catalog root selects the packaged root. Empty driver core selects the active rendering driver's NGX core. Keep NRDriverCore blank for portable installs; explicit paths are diagnostic overrides. |
| Runtime | NRDriverCore | `` | Yes | Optional NVIDIA driver-core override. Blank discovers the active driver core. |
| DynamicResolution | Enabled | `false` | Yes | Currently unsupported; leave disabled. |
| DynamicResolution | Oscillate | `false` | Yes | Diagnostic dynamic-resolution oscillation; leave disabled. |
| Debug | EnableGPUTimings | `true` | No | Collect GPU timing measurements for the status display. |
| Debug | EnableFrameTrace | `false` | No | Write detailed per-frame diagnostic traces; normally disabled. |
| Debug | DirectRCASOutput | `false` | No | Diagnostic direct sharpening output route; normally disabled. |
| Debug | DirectDLSSOutput | `false` | No | Diagnostic direct DLSS output route; normally disabled. |
| Debug | LogFrameDiagnostics | `false` | No | identity and errors are always logged. Detailed samples are opt-in. |
| Debug | LogPerformanceMetrics | `false` | No | Write periodic performance measurements to the log. |
| Debug | PerformanceLogIntervalSeconds | `10` | No | Interval 1-120 seconds, used only with performance logging enabled. |
| Debug | LogMenuMetrics | `false` | No | Write menu/source diagnostics to the log; normally disabled. |
| Menu | WindowX | `40` | No | Program-owned menu geometry; saved by the End menu. |
| Menu | WindowY | `40` | No | Program-owned menu geometry; saved by the End menu. |
| Menu | WindowWidth | `640` | No | Program-owned menu geometry; saved by the End menu. |
| Menu | WindowHeight | `720` | No | Program-owned menu geometry; saved by the End menu. |
| Menu | LeftColumnFraction | `0.5` | No | Program-owned menu geometry; saved by the End menu. |
