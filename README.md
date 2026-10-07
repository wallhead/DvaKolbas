# RaZkolbaS

Formerly DvaKolbas; based on [Theo's Render Pipeline](https://github.com/theosw/theosrenderpipeline).
The plugin, menu, configuration, log and resource directory use the RaZkolbaS name.
Upgrade with the complete package: disable/remove the old renderer mod first so
`TheosRenderPipeline.dll` and `RaZkolbaS.dll` are never loaded together.
Startup also rejects a leftover `TheosRenderPipeline.dll` in `SKSE/Plugins`,
even before it loads. Remove that old DLL or disable the mod providing it.

NVIDIA rendering integration for Skyrim: DLSS/DLAA, frame generation, optional
Neural Rendering (NR), and native-resolution menus and HUD. Current version:
**0.3.5**. See [CHANGELOG.md](CHANGELOG.md) for release changes.

## Features

- DLSS Super Resolution, DLAA, model presets and sharpening.
- Frame generation and multi-frame generation (MFG).
- NR before or after DLSS/DLAA, one or two independently configured passes,
  input scaling and tuning, in both editions. NR defaults off.
- Optional peripheral compression and combined NR preparation, both off by default.
- Optional weather and time presets for any NR setting and sharpening, with smooth
  transitions and separate interior settings. Presets are shareable files. See
  [presets](package/APPEARANCE-PROFILES.md).
- ReShade source-frame effects, explicit depth and placement controls.
- Native UI composition, inventory/spell previews, loading artwork and external
  ImGui integration, with live GPU measurements beside the settings.

**End** opens settings. Toggles apply immediately; numeric fields and sliders
apply when editing ends (Enter, focus loss or drag release).
**Save as default** persists settings and window layout. Select **100% | Native**
in DLSS Render scale for native anti-aliasing. Mode and render scale require restart. NR keyboard
shortcuts are opt-in. HDR is experimental, through Community Shaders or TRP's own
HDR output; see [HDR](#hdr-experimental).

The menu starts at 640 pixels wide and can shrink to 480 pixels. Each tab's
scroll area follows the window size. Save as default stays below the tabs;
long status messages scroll separately. Existing saved window sizes are retained.

AMD renderers use FSR upscaling and optional FSR frame generation; DLSS, DLAA,
NVIDIA FG and NR are unavailable. Before the first AMD launch, set `[FSR]
SourceColorEncoding` in `SKSE/Plugins/RaZkolbaS.ini` to match the verified
Skyrim/ENB source. The packaged value `Unknown` intentionally stops startup with
setup instructions. FSR3 (3.1.5) remains available alongside experimental FSR4.
Auto uses the official runtime's actual device catalog; explicit FSR4 selects
official ML on AMD or the separate hash-pinned INT8 runtime on NVIDIA (SM6.6).
SR and FG providers are independent. Known unavailable FSR4 choices are disabled
with a reason; unverified support is stated explicitly and rechecked at startup.
If an explicit selection prevents startup, restore `[FSR] Provider=FSR3`
and/or `[FrameGeneration] FsrProvider=FSR3` in `RaZkolbaS.ini`.
See [AMD setup](package/INI-SETTINGS.md#amd-first-launch).

FSR4 ML FG remains experimental pending real AMD rendering and Skyrim validation.
AMD documents Windows 11, RX9000+ and DirectX 12 Agility SDK 1.4.9+ for ML FG;
this branch does not claim Agility deployment. The NVIDIA INT8 SR qualification
measured about 665 MiB of process-local GPU memory after retirement, with a 7 MiB
range across 32 recreations. This is bounded in that test; its ownership/cause
is not established, and it can matter on cards with limited VRAM. Revalidate
the mixed INT8 SR / official FG combination whenever either runtime changes.

The main INI groups upscaling, frame generation, Neural Rendering and individual
`[NR PASS 1]` / `[NR PASS 2]` / `[NR PASS 3]` controls separately. Choose
`[Upscaling] Upscaler=DLSS` or `FSR`, then `Quality=Native` in that provider's
section. Advanced options and runtime paths are lower down; menu geometry is
last. Install the named INI with its matching DLL. The game rejects old layouts;
the explicit offline converter writes a separate file and preserves the source.
**Save as default** preserves unknown keys and comments. See the
[INI reference and conversion instructions](package/INI-SETTINGS.md).

With two NR passes selected, optional **One pass in combat** and **One pass while
weapons/spells are drawn** controls temporarily skip the second pass. The return
delay defaults to five seconds after all selected conditions clear and pauses
with the game. Your saved pass count, resolution and tuning remain unchanged.
The second pass stays allocated and its history resets when it resumes. These
options default off; their effect on responsiveness depends on the workload.

## Install

Use SKSE64 and Address Library matching your Skyrim executable. Install packages
are supplied separately; this repository contains source, not the NVIDIA runtime
DLLs. Follow the [Standard installation guide](package/STANDARD-README.md) or
[Universal installation guide](package/README.md).

Standard includes the SR/FG and NR runtime bundle. Universal adds the compatibility
paths and can use Standard's runtimes when installed after it in MO2, or separately
supplied matching runtimes. Both editions retain NR. Enable only one winning
renderer DLL. The NVIDIA host is required even when interpolation is off.

The DLL selects Community Shaders integration when CommunityShaders.dll is loaded;
otherwise TRP owns upscaling, with optional ENB. With CS, disable its frame
generation and Reflex; CS retains its shading, upscaling, UI and HDR output.
Keep SSE ReShade Helper disabled when using TRP's ReShade integration. Use one shading
setup per profile and disable competing upscaler/frame-generation injectors.

## ReShade tested scope

ReShade 6.8 was tested with Universal on Skyrim 1.6.1170, Cabbage ENB and
an RTX 4080 SUPER: ordinary 6.8.0.2158 and full add-on 6.8.0.2155, with
early NR and x4. Sky Reflection Fix's ReShade registration was excluded;
Rumble passed a separate initial test. ReGrade+, combined third-party add-ons,
Standard gameplay and CS with ReShade remain unverified. These were earlier
development builds; the exact 0.3.5 package still needs its final game test.

## HDR (experimental)

With Community Shaders, HDR uses its HDR Display (tested with CS 1.9.1 and
HDR Display 1.2.2). CS composes the HDR10 image; TRP adds frame generation and NR.
Enable Windows HDR and use borderless windowed. In CS's HDR Display settings, set
peak brightness to your monitor's value and start paper white near the Windows SDR
content brightness, then adjust to taste.

Weathers and lighting built for ENB, such as NAT.ENB, need CS Effects 11 with a
preset; without one the image is much darker in both SDR and HDR. Two
full-resolution NR passes after upscaling are expensive; prefer one pass or NR
before upscaling. HDR through Special K, RenoDX or Linear Lighting is not
supported, and most ReShade effects are not HDR-aware.

Evidence is one Universal RTX 4080 SUPER LoreRim UltraCS setup with Effects 11:
x4 generation, both NR placements and HDR on/off switching. Standard, other
hardware and physical frame cadence remain untested.

### TRP HDR output (ENB and non-CS setups)

Without Community Shaders, **Image > HDR output** expands the finished SDR image,
including ENB's, to HDR10. This is inverse tone mapping: SDR white is shown at
paper white, the brightest areas are expanded towards peak brightness, and the
native UI, menus and TRP overlay use their own brightness. Highlights the preset
already clipped cannot be recovered, and 8-bit output can band in bright gradients.
Frame generation receives matching HDR10 HUD-less and UI images. NR and ReShade
still run on the SDR image.

Enable Windows HDR, turn on **HDR output**, save as default and restart. By
default, paper white and UI brightness follow Windows' SDR content brightness
(**Match Windows SDR brightness**). Paper white, peak, UI brightness, highlight
strength, expansion start and SDR decoding apply live. When Windows HDR is off for the game's display, output stays SDR.
Leave ENB's own HDR-like effects as they are; this does not change the preset.
The panel shows the output pass's GPU time.

Evidence is one Universal RTX 4080 SUPER LoreRim ENB setup at 5120x1440 on a
1015-nit display: x4 generation, NR before upscaling, a loading door and live
calibration. That run logged more NVIDIA "flip queue is empty" messages than a
matched HDR-off run, without a visible hitch. Recurring vendor flip-queue errors
remain unresolved. Loading screens forced by **Request loading artwork** now
fade in from black. Later ENB checks also cover Wheeler and HUD/End over menus.
Standard, other hardware and physical frame cadence remain untested.

## Compatibility

| Edition | Frame-generation capabilities |
| --- | --- |
| Standard | Native NVIDIA capabilities; RTX 40 uses native x2 |
| Universal | Experimental RTX 20/30 compatibility, RTX 40 MFG unlock, native RTX 50 capabilities |

The loader supports Steam Skyrim **1.5.97, 1.6.640, 1.6.1170 and 1.7.104**.
Versions 1.5.97, 1.6.640 and 1.7.104 remain experimental; 1.6.640 and 1.7.104
are untested in-game. Nolvus Awakening 6.0.20 has scoped positive Universal
input/x5/NR feedback on 1.5.97. The plugin/SKSE identity is `RaZkolbaS`;
existing `SolFG_*` companion exports retain their names and layouts.

RTX 20 evidence is limited to an RTX 2060 volunteer run reporting x2/x3/x4/x6,
NR and loading recovery; weapon jitter remains unresolved. GTX 16 is excluded.
See [RTX 20 setup and test limits](package/RTX20-TEST.md). RTX 30 evidence includes
a 3060 Laptop/CS run with x2/x3/x4 and NR, plus reported grass-edge artifacts and
occasional hitches. Native RTX 50 operation remains unverified.

The combined 0.3.0 release has positive ENB/RTX 4080 SUPER feedback with verified
x4/NR and loading recovery. Earlier runs cover Standard native x2, Universal MFG,
ReShade/CS integration and DLAA with NR before/after at 5120x1440. These results
do not establish every GPU, modlist or checkbox. Recurring vendor RSYNC errors
remain recorded in some runs; generated-FPS counters do not establish physical
display cadence. DLAA/NR on RTX 30 and every CS build remain unverified.

## Build

Requires Windows, Visual Studio 2022 C++ tools/Windows SDK, CMake and vcpkg.
Use CommonLibSSE-NG 8.1.0 at `3c0f5a87c3b166c9a6712d5c3bd180e9ac5ad0fd`,
Streamline 2.11.1 public headers and NGX SDK headers/import library, and
Nukem9/detours at `cc5a2e4a58ef462821877b35ad30215f0e16bba1`. CommonLib's
patch diagnostics fetch HDE64 from MinHook v1.3.4. Other libraries are listed
in `vcpkg.json`; runtime DLLs are not needed to compile.

Place supplied dependencies under ignored `.dependencies/`, and adjust these
example paths to your installations:

```powershell
cmake --preset arp-nvidia `
  "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake" `
  "-DTRP_COMMONLIBSSE_NG_DIR=C:/deps/CommonLibSSE-NG" `
  "-DTRP_NGX_SDK_DIR=C:/deps/Streamline/external/ngx-sdk" `
  "-DTRP_STREAMLINE_INCLUDE_DIR=C:/deps/Streamline/include" `
  "-DARP_DETOURS_DIR=C:/deps/detours"
cmake --build --preset arp-nvidia --parallel 2
```

`ARPBaseline` builds the renderer in `out/build/nvidia/Release` without deploying
or launching. The build compiles and embeds NR shader bytecode; generated headers
stay in the build directory. Configure separate directories for Standard
(`TRP_ENABLE_OPTIONAL_FEATURES=OFF`) and Universal (`ON`, default), keeping
`TRP_ENABLE_NEURAL_RENDERING=ON` for both. Store workstation paths in ignored
`CMakeUserPresets.json`. Set `TRP_NGX_LIB` if the NGX import library is elsewhere.

Enable `TRP_BUILD_COMPATIBILITY_TESTS=ON` for the standalone checks, build all
targets with `cmake --build <build-directory> --config Release`, then run
`ctest --test-dir <build-directory> -C Release --output-on-failure`. These checks
do not establish game acceptance. Build output alone is not a complete install;
use the installation guides for configuration, shaders and required runtimes.

The optional ReShade lifecycle fixture compares automatic and exported D3D11
runtimes, single effect execution, effects-off GUI completion, shared input and
paired teardown. It needs a supplied x64 ReShade DLL and an accepted hardware
adapter; no DLL is downloaded, bundled or installed. Configure with
`-DTRP_RESHADE_TEST_RUNTIME=C:/path/to/dxgi.dll`, build
`TRPReShadeLifecycleTests`, then run `ctest -R ReShadeLifecycleRuntime` in that
build directory with `-C Release --output-on-failure`. Each run copies the DLL
and fixture into fresh folders under the build directory and records its hash.
Logs and configurations are retained; disposable executable/DLL copies are removed
after successful cases. The runner's `--keep-binaries` option retains them too.
Children run in a kill-on-close Windows job so terminating the runner retires them.
`TRP_RESHADE_TEST_INPUT_POLICY` defaults to `observe`; use `every-runtime` for
6.3.3 or `first-runtime` for 6.8.0 to enforce the version-specific input check.
Observation records both runtimes' raw key states without imposing either policy.
These are upstream runtime contracts, not a ReGrade+ reproduction, race test,
performance benchmark or Skyrim compatibility guarantee. The fixture can also
be built independently with `cmake -S tests/reshade-lifecycle -B out/reshade`.

See [LICENSE](LICENSE) and [third-party notices](THIRD-PARTY.md) for project,
dependency and contribution terms. Vendor runtimes retain their own licenses.
