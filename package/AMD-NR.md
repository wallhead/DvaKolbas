# NR on AMD GPUs (experimental bridge)

TRP's own NR path needs NVIDIA hardware. On Radeon GPUs, TRP can instead load
the separately distributed **DLSS NR on AMD** mod by danielblnc
(<https://github.com/danielblnc/DLSS-NR-on-AMD/releases>). That mod runs DLSS 5
Neural Rendering on RDNA2/3/4 GPUs through AMD HIP.

TRP does not ship, rename, patch or redistribute that mod. Its licence forbids
bundling, so download it from the official release page and install it yourself.

## How the bridge works

The mod hooks AMD FidelityFX (`ffxCreateContext` / `ffxDispatch`) and runs the
network on the colour, depth, motion vectors and exposure the game passes to FSR.
Skyrim renders in D3D11, so on its own the mod finds nothing to hook. TRP's FSR path
already provides what it needs: TRP shares Skyrim's frames with a native D3D12
device, calls FidelityFX there and, in FSR presentation mode, presents through
AMD's D3D12 frame-generation swapchain.

With `AmdBridge` enabled, TRP loads the mod into the process just before it
loads the FidelityFX runtime, so the mod's detours are in place when TRP creates
the FSR context. TRP has no other interface to the mod. Its settings, overlay
(End key by default) and log belong to the mod and live next to its DLL.

## Install

1. Use TRP with FSR upscaling (`[Settings] UpscaleType` for FSR). Select the FSR
   frame-generation backend so TRP presents through AMD's D3D12 swapchain (FG itself
   may stay off). The mod expects a D3D12 present.
2. Install the AMD HIP runtime 7 the mod requires: Adrenalin 26.x on RX 7000/9000,
   or the HIP SDK 7.2 on RX 6000.
3. Run the mod's setup once against any folder to get its DLL, or copy the DLL
   from its release. Place it in its own folder under `Data/SKSE/Plugins`, for
   example `TheosRenderPipeline/AMD-NR/version.dll`. Keep one of the six proxy
   names the mod expects (`version.dll`, `winmm.dll`, `dbghelp.dll`,
   `wininet.dll`, `winhttp.dll`, `dxgi.dll`); it forwards that DLL's exports to
   System32. Copy your own `nvngx_dlssnr.dll` (DLSS 5 NR 310.8.0.0) into the same
   folder; the mod builds its weight file from it.
4. In `TheosRenderPipeline.ini`:

   ```ini
   [NeuralRendering]
   AmdBridge = AMD
   [Runtime]
   AmdNRModule = TheosRenderPipeline/AMD-NR/version.dll
   ```

5. Keep TRP's own NR (`[NeuralRendering] Enabled`) off; it needs NVIDIA.

If the mod is already installed in the Skyrim folder as a proxy DLL, leave
`AmdNRModule` empty. TRP detects the loaded copy and does not load a second one.

## Settings

| Key | Values | Meaning |
| --- | --- | --- |
| `[NeuralRendering] AmdBridge` | `Off` (default), `AMD`, `Any` | `AMD` loads the module only when the FSR adapter is AMD (vendor 0x1002). `Any` loads it on every GPU. On NVIDIA it then only hooks and logs (no HIP), which checks the hook path without AMD hardware. |
| `[Runtime] AmdNRModule` | path | Mod DLL, relative to `Data/SKSE/Plugins` or absolute. TRP accepts only an x64 image with the mod's HIP kernel section. |

Both are read at startup. TRP never unloads the module.

## Checking it

The TRP log has one `[AMD NR]` line at FSR startup that says what happened:
off, skipped (not AMD), adopted (proxy install), loaded, or why loading failed.
The mod writes its own log next to its DLL (`*_dlssnr_on_amd.log`). Look for:

- `hooked …!ffxDispatch` and `first ffxDispatch`: the mod sees TRP's FSR calls.
- `device … (from the first presented swapchain)` and `staging ready`: it found
  TRP's D3D12 device and inputs.
- `this game renders with D3D11: unsupported`, `dummy swapchain failed` or
  `ignoring a swapchain that is not on our device`: it rejects this setup. That
  needs a change in the mod, not in TRP.

## Status

Untested on AMD hardware. Whether the mod accepts a D3D11 game whose FSR runs on
a separate D3D12 device, and whether it catches presents through AMD's
frame-generation swapchain, is unverified. Use `AmdBridge = Any` on any GPU to
collect the mod's hook log, and report results with both logs.
