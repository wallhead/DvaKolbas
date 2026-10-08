# GPU startup routing validation — v1.3.2

2026-10-08; branch codex/nr.

## Trigger and change

Tester log `RaZkolbaS (11).log` identifies GTX 1070 (PCI 1B81, Pascal,
CUDA 6.1), saved DLAA, FG off, NR off; Streamline feature 1000 failed
with result 34. v1.3.1 normalized only AMD. Startup now queries the actual
render D3D11 device's DXGI adapter/LUID and software flag, reads matched
public NVAPI architecture/product evidence, and probes D3D12 FL12_0
without creating an additional device (S_FALSE is successful).

RTX keeps NVIDIA routes. Non-RTX NVIDIA, AMD and Intel use FSR-only
selection. Non-RTX NVIDIA uses FSR3, disables NR/HDR/NVIDIA compatibility
probing, and clears incompatible live/menu requests. Saved NVIDIA FG on
is never promoted to FSR FG on. Explicit FSR FG is preserved. Effective
normalization is in memory; the INI changes only with an explicit save.
Unknown or foreign architecture evidence cannot grant an RTX-only route.
FSR source encoding remains explicit; an invalid INI is not guessed.

Software, missing renderer identity, missing D3D11 FL11_0 or missing
D3D12 FL12_0 stop with a specific unsupported GPU/driver message before
vendor backend loading. This documents a universal product baseline;
even the ordinary Community Shaders diagnostic presenter must meet it.
FSR presenter plus Community Shaders remains unsupported due to source
ownership (the existing AMD limitation).

## Tests

- Red: initial classifier stub failed 115 assertions across the initial
  26-fixture/208-route matrix. GTX regression and typed/menu guards failed
  before implementation.
- Green: **28 fixtures / 600 simulated supported startup routes / zero
  failures**, plus negative software, missing API, feature-level and LUID
  cases. Four saved configurations, three FSR scales (Native, Quality,
  Performance), with NVAPI evidence present or absent.
- **16 focused CTest checks passed**: BackendSelection, RendererGpuMatrix,
  RendererSettingsActions, StartupPreferences, PublicIni, PublicIniSchema,
  IniMessages, OverlayNeuralTab, OverlayFsrGenerationTab, NrGpuArchitecture,
  NrPortablePackage, FsrSettings, RendererUpgrade, NvidiaAppSettings,
  NvidiaDriverSettings and IniLayout.
- Live capability probe: RTX 4080 SUPER / PCI 2702 / Ada architecture190 /
  matched NVAPI RTX product / D3D12 probe S_FALSE / NVIDIA route retained.
  No vendor feature context or Skyrim launch was performed for this probe.
- Independent read-only review caught Auto remaining selectable in the
  FSR3-only provider dropdown; fixed with analyticalOnly availability and
  an interaction regression. The regression failed before the fix and
  passes afterward for both SR/FG menus' shared control.
- Release and DLL version bumped to 1.3.2. v1.3.1 archive is preserved with
  SHA256 b3d70dcb30ca92f6f91d8fb703ede28fd1a2695923005803d2aa73481df96201.
  No installed MO2 trial/profile/INI was changed.

## Fixtures and limits

Names are descriptive test fixtures. Most device IDs are deliberately
synthetic to exercise unknown/laptop fallback; known desktop IDs and
GTX1070 are explicit regressions. Architecture and D3D capability are
injected inputs, not physical qualification. Laptop power modes, VRAM,
actual driver exports, DLSS/NR/FSR feature creation, rendering and FG
quality cannot be emulated by these CPU tests. The older rejected fixtures
exercise absent required feature support. A real driver is probed at
startup; routing never guarantees rendering success on an untested card.
FSR4 requests are retained for AMD/RTX for actual capability checks;
non-RTX NVIDIA is normalized to FSR3. GTX FG remains unqualified.

| Fixture | Expected route with injected capabilities |
| --- | --- |
| NVIDIA GeForce RTX 2080 | NVIDIA RTX |
| NVIDIA GeForce RTX 2060 Laptop GPU | NVIDIA RTX |
| NVIDIA GeForce RTX 3080 | NVIDIA RTX |
| NVIDIA GeForce RTX 3050 Laptop GPU | NVIDIA RTX |
| NVIDIA GeForce RTX 4080 | NVIDIA RTX |
| NVIDIA GeForce RTX 4060 Laptop GPU | NVIDIA RTX |
| NVIDIA GeForce RTX 5090 | NVIDIA RTX |
| NVIDIA GeForce RTX 5090 Laptop GPU | NVIDIA RTX |
| NVIDIA GeForce GTX 1070 | FSR-only |
| NVIDIA GeForce GTX 1060 | FSR-only |
| NVIDIA GeForce GTX 1050 Ti Laptop GPU | FSR-only |
| NVIDIA GeForce GTX 1650 Laptop GPU | FSR-only |
| NVIDIA GeForce GTX 1660 SUPER | FSR-only |
| NVIDIA GeForce GTX 980 | FSR-only |
| NVIDIA GeForce GTX 780 | Reject: injected FL12_0 absent |
| NVIDIA GeForce GTX 750 Ti | Reject: injected FL12_0 absent |
| NVIDIA GeForce GTX 480 | Reject: injected FL12_0 absent |
| NVIDIA GeForce MX150 | FSR-only |
| NVIDIA Quadro P2000 | FSR-only |
| NVIDIA RTX A4000 | NVIDIA RTX |
| AMD Radeon RX 9070 | FSR-only |
| AMD Radeon RX 7900 | FSR-only |
| AMD Radeon RX 6700 | FSR-only |
| AMD Radeon RX 5700 | FSR-only |
| AMD Radeon RX 590 | FSR-only |
| AMD Radeon 780M | FSR-only |
| Intel Arc A770 | FSR-only |
| Intel Iris Xe | FSR-only |

Reproduce from the worktree:

```powershell
out/build/fsr-fg-universal/Release/TRPRendererGpuMatrixTests.exe package/SKSE/Plugins/RaZkolbaS.ini
# Optional read-only local hardware capability probe:
out/build/fsr-fg-universal/Release/TRPRendererGpuMatrixTests.exe package/SKSE/Plugins/RaZkolbaS.ini --live
```
