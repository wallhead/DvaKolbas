# AIO18 DLSS Neural Rendering / Neural Reconstruction RE — Pass 1

**Target archive:** `SkyrimUpscalerAIOBuild18-Hotfix1(1).7z`  
**Archive SHA-256:** `136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8`  
**Archive size:** 204,178,443 bytes  
**Analysis type:** static reverse engineering only; target DLLs were not executed.  
**Pass goal:** recover the AIO18 NR module boundary, host→PD call graph, init/evaluate/release ABI, scheduling modes, and enough resource/parameter evidence to scope the next pass.

## 1. Pass count estimate

For implementation-grade coverage of AIO18 NR, plan **4 focused static RE passes**, followed by an **optional but strongly recommended 5th runtime-validation pass**.

1. **Pass 1 — architecture and ABI** (this report): module identities, host→PD calls, create/evaluate/release, payload sizes, scheduling locations, local-vs-companion GPU architecture, NGX parameter inventory.
2. **Pass 2 — exact evaluation contract:** map all 0x138 payload offsets to NGX parameters/resources/types, subrects, motion scale/sign, depth convention, reset, control/UI inputs, resource states, immediate vs Present evaluation, and local/companion staging.
3. **Pass 3 — reconstruction/resolve pipeline:** input-resolution scaling, Auto/Residual/Ratio behavior, color/HDR transforms, private work surfaces, extra passes, UI correction/composition, debug paths, and before-vs-after-upscale resource flow.
4. **Pass 4 — lifecycle/integration correctness:** feature recreation, device loss, resize, reset coverage, ENB/ReShade/CS interaction, FG handoff, fences/resource lifetime, failure paths, and static bug/race audit.
5. **Pass 5 — runtime validation:** breakpoints/captures on the exact build to confirm active scheduling path, resource dimensions/formats/states, selected GPU, NGX parameter values, pass counts, and retirement ordering.

Four passes should give strong static implementation coverage. Pass 5 is what converts the remaining branch/resource-lifetime assumptions into runtime evidence.

## 2. Exact AIO18 module identities

| Module | SHA-256 | Notes |
|---|---|---|
| `SkyrimUpscaler.dll` | `5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81` | Skyrim-facing NR scheduler/payload producer. File version 1.0.2.0. |
| `PDPerfPlugin.dll` | `53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1` | D3D11/D3D12 bridge and NR backend owner. |
| `nvngx_dlssnr.dll` | `8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206` | NVIDIA NR runtime, file/product 310.8.0.0. |

`PDPerfPlugin.dll` CodeView identity:

```text
GUID 5505ca9d-fcdf-40c6-8bf2-1f3b26ce2b25
age  1
PDB  ...\current-release-deploy-20260927-085609\plugin\PDPerfPlugin.pdb
```

`nvngx_dlssnr.dll` CodeView identity:

```text
GUID 4413bf99-b777-416c-907a-c4e6fcf26fdd
age  1
PDB  ...\NGX\snippets\rel_310_8\source\features\dlssnr\...\nvngx_dlssnr.pdb
```

The NVIDIA runtime exports the standard D3D12 NGX surface, including:

```text
NVSDK_NGX_D3D12_Init                    +0x13F50
NVSDK_NGX_D3D12_CreateFeature           +0x15750
NVSDK_NGX_D3D12_EvaluateFeature         +0x159C0
NVSDK_NGX_D3D12_GetFeatureRequirements  +0x15BD0
NVSDK_NGX_D3D12_GetScratchBufferSize    +0x15CF0
NVSDK_NGX_D3D12_Init_Ext                +0x15DF0
NVSDK_NGX_D3D12_PopulateParameters_Impl +0x15F20
NVSDK_NGX_D3D12_ReleaseFeature          +0x16040
NVSDK_NGX_D3D12_Shutdown                +0x16130
NVSDK_NGX_D3D12_Shutdown1               +0x16240
```

**STATIC_OBSERVED:** AIO18 is not using a mysterious one-off NR ABI. PD wraps a normal NGX D3D12 feature runtime, but adds substantial scheduling, interop, resolve, and multi-GPU logic around it.

## 3. Host → PD NR ABI changed from older builds

The AIO18 Skyrim host uses delay imports from `PDPerfPlugin.dll`:

| Import | host delay-IAT RVA |
|---|---:|
| `EvaluateDLSSNR` | `0x472C60` |
| `ReleaseDLSSNR` | `0x472C80` |
| `IsDLSSNRAvailable` | `0x472C88` |
| `InitDLSSNR` | `0x472C90` |

AIO18 PD exports:

| Export | PD RVA |
|---|---:|
| `IsDLSSNRAvailable` | `0xF9F80` |
| `InitDLSSNR` | `0xFA150` |
| `EvaluateDLSSNR` | `0xFA180` |
| `ReleaseDLSSNR` | `0xFA240` |

These addresses are specific to the exact hashes above. The earlier Build16 offsets must not be reused.

## 4. Host NR initialization chain

The host NR initializer begins at:

```text
SkyrimUpscaler +0x2A2120
```

Observed sequence:

```text
+0x2A2137 -> PDPerfPlugin!ReleaseDLSSNR(0)
             ↓
host +0x2A9 = false
             ↓
+0x2A2144 -> PDPerfPlugin!IsDLSSNRAvailable()
             ↓
build init payload
             ↓
+0x2A226B -> PDPerfPlugin!InitDLSSNR(&payload)
             ↓
host +0x2A9 = returned success flag
```

The initializer chooses dimensions from different host fields depending on two mode bytes, then includes additional settings/state in the payload. Important observed inputs include host offsets `+0x29C`, `+0x2A4`, `+0x2AC`, `+0x2FC`, the cached `+0x3F0`, and a conditional interop/companion block sourced from `+0xA38/+0xA60/+0xA70/+0xA80/+0xA88`.

**Pass-1 boundary:** those fields are kept as offsets unless their semantics are independently established. Pass 2 will map them against PD's init consumer and NGX creation parameters.

## 5. PD init selects a real local/companion GPU backend

`PDPerfPlugin!InitDLSSNR` is a thin virtual dispatch to `UpscaleAPI_D3D11` slot `+0xF0`:

```text
PD export +0xFA150
    -> UpscaleAPI_D3D11 vtable +0xF0
    -> internal InitDLSSNR +0xF4960
```

The `UpscaleAPI_D3D11` vtable is at PD RVA `0x11AB7C0`. Relevant slots:

```text
+0xF0  -> +0xF4960  InitDLSSNR internal
+0xF8  -> +0xF4AE0  EvaluateDLSSNR internal
+0x100 -> +0xF4E30  ReleaseDLSSNR internal
+0x108 -> +0xF5160  related device/context query path
```

At `PD+0xF4960`, the code chooses between two D3D12 device paths. It then allocates a dedicated backend object:

```text
PD +0xF4A7F: allocation size = 0x748 bytes
PD +0xF4A97: backend constructor/helper +0x9B800
PD +0xF4AC6: backend initialization +0x9C9A0
UpscaleAPI_D3D11 +0x78 = NR backend pointer
```

Embedded diagnostics independently describe both modes:

```text
DLSSNR: running on companion GPU adapterIndex=%d companionDevice=%p
DLSSNR: running on game GPU (local D3D12) adapterIndex=%d localDevice=%p companionDevice=%p
DLSSNR Init: adapterIndex=%d onCompanion=%d ... NR will run on %s
```

**STATIC_OBSERVED:** AIO18 supports both NR on the game GPU via a local D3D12 device and NR on a companion GPU. This is not merely an adapter-name UI option; the backend has distinct device/staging paths.

## 6. Evaluate ABI: 0x138-byte payload confirmed on AIO18

`PDPerfPlugin!EvaluateDLSSNR` at `+0xFA180` copies the incoming structure to a stack-local copy and then calls virtual slot `+0xF8`.

The copy consists of two 0x80-byte chunks followed by 0x38 bytes:

```text
0x80 + 0x80 + 0x38 = 0x138 bytes
```

So the AIO18 `DLSSNREvaluateParams` size is **0x138 bytes**.

This independently confirms the older static observation, but on the AIO18 module hash.

## 7. Common Skyrim NR evaluation producer

The host builds the NR evaluate structure in one shared function:

```text
SkyrimUpscaler +0x2A2300
    ... validation/resource selection ...
    +0x2A25E0 -> PDPerfPlugin!EvaluateDLSSNR
```

Before building the payload it requires, among other state:

```text
host +0x2A8  != 0
host +0x2A9  != 0   // successful NR initialization
input pointer != null
host +0xB50  != 0   // prepared depth resource
host +0xBA8  != 0   // prepared motion resource
```

The first resource slots of the 0x138-byte payload are directly recoverable because `rcx` at the export call is `rsp+0x20`:

| Payload offset | STATIC_OBSERVED value in common producer |
|---:|---|
| `+0x00` | zero dword in the inspected initialization path |
| `+0x08` | caller-selected color/resource (`*rdi`) |
| `+0x10` | prepared motion (`host +0xBA8`) |
| `+0x18` | prepared depth (`host +0xB50`) |
| `+0x20` | same initial color/resource again |
| `+0x28` | null |
| `+0x30` | optional resource selected later into `rsi` |
| `+0x38` | optional resource selected later into `rcx` |

The function also copies the host jitter pair at `+0x10/+0x14`, tuning/state fields around host `+0x2B0..+0x2E4`, and conditionally changes resource/selector behavior based on the two byte arguments supplied by callers.

**Pass-1 qualification:** resource slots `+0x08..+0x38` are intentionally not all named `Color/UI/Backbuffer/...` yet. The PD backend has explicit `DLSSNR.Color`, `UI`, `UIAlpha`, `Backbuffer`, `ControlMask`, and distortion-field parameters, so naming those payload offsets without tracing the consumer would be guesswork. Pass 2 is dedicated to this mapping.

## 8. AIO18 NR has multiple scheduling sites

The shared host NR producer `+0x2A2300` has at least these direct call sites:

```text
SkyrimUpscaler +0x18FA4B
SkyrimUpscaler +0x265B0A
SkyrimUpscaler +0x294E24
SkyrimUpscaler +0x297AE6
SkyrimUpscaler +0x2AB0CF
SkyrimUpscaler +0x2AB5CC
SkyrimUpscaler +0x2ABF50
```

This matters: there is **not one universal NR evaluation point**.

### 8.1 Before-upscaling mode

At `SkyrimUpscaler+0x294E05`, the host reinitializes NR when its tracked work-size/input state changes. Immediately afterward:

```text
+0x294E0A: check host byte +0x2AA
+0x294E24: call common NR evaluate +0x2A2300
```

The shipped INI documents:

```ini
# Apply DLSS NR before upscaling to gain more performance.
mDLSSNRBeforeUpscaling = false
```

**CONFIG_BACKED_INFERENCE:** host byte `+0x2AA` is the before-upscaling mode flag or a directly coupled internal form of it. When enabled, NR evaluation occurs in the source-upscaling path before the following upscale work.

### 8.2 Default after-upscaling / Present scheduling

The INI explicitly says that when `mDLSSNRBeforeUpscaling` is disabled, NR runs on the upscaled result at Present.

Late calls at `+0x2AB0CF`, `+0x2AB5CC`, and `+0x2ABF50` are inside the late/Present-side host path and feed the same common NR producer with different mode-byte combinations and resource wrappers.

**STATIC_OBSERVED + CONFIG_BACKED:** the default AIO18 NR architecture is an after-upscale/Present-side path; before-upscale is an alternate mode.

### 8.3 Community-Shaders/live bridge path

`+0x265B0A` uses a resource wrapper at host `+0xAF8` and calls the common NR evaluator with both boolean mode arguments set. It occurs in the same broad host region previously identified as the Community Shaders live-postprocessing bridge.

**INFERRED:** this is a specialized renderer integration feeding the common NR ABI, analogous to the specialized CS guide preparation we recovered for FSR. Exact CS ownership and the resource represented by `+0xAF8` remain Pass-2/Pass-4 work.

## 9. PD internal evaluate chooses more than one execution route

The `UpscaleAPI_D3D11` evaluator is:

```text
PDPerfPlugin +0xF4AE0
```

It requires the NR backend pointer at `this+0x78`.

One branch checks another PD object/state and, when active, explicitly clears payload byte:

```text
payload +0x110 = 0
```

It then builds helper state through `PD+0xF4300`, checks another condition via `+0x4BB40`, and ultimately reaches the dedicated NR backend evaluation helper:

```text
PD +0xA0B10
```

Other branches copy the complete caller payload and reach the same backend evaluator directly.

**STATIC_OBSERVED:** `+0xF4AE0` is a routing/interop wrapper; `+0xA0B10` is the deeper NR backend evaluator. Pass 2 should concentrate on `+0xA0B10` and its parameter writes rather than treating the exported wrapper as the final NGX call.

## 10. PD release path

Release flows through:

```text
PD export ReleaseDLSSNR +0xFA240
 -> UpscaleAPI_D3D11 vtable +0x100
 -> PD internal +0xF4E30
 -> backend release helper +0xA1D20
```

The internal release also runs broader interop/device cleanup helpers before releasing the NR backend.

## 11. Confirmed NGX parameter vocabulary

The exact AIO18 PD binary contains the following NR parameter names, among others:

### Creation/general

```text
CreationNodeMask
VisibilityNodeMask
PerfQualityValue
DLSSNR.Width
DLSSNR.Height
DLSSNR.Hint.Render.Preset
DLSSNR.ScalingRatio
DLSSNRComputeScalingRatioCallback
```

### Resource inputs/outputs

```text
DLSSNR.Color
DLSSNR.Depth
DLSSNR.MVec
DLSSNR.ControlMask
DLSSNR.Output
DLSSNR.UIAlpha
DLSSNR.UI
DLSSNR.BidirectionalDistortionField
DLSSNR.Backbuffer
```

### Subrects

AIO18 sets or knows explicit BaseX/BaseY/Width/Height keys for Color, MVec, Depth, Output, ControlMask, UI, UIAlpha, Backbuffer, and BidirectionalDistortionField.

### Per-evaluation state/tuning

```text
DLSSNR.MVecScaleX
DLSSNR.MVecScaleY
DLSSNR.LocalToneStrength
DLSSNR.Intensity
DLSSNR.SkinStructureStrength
DLSSNR.LocalStructureStrength
DLSSNR.Style
DLSSNR.UseAutoMask
DLSSNR.DepthInverted
DLSSNR.Reset
DLSSNR.UICorrection
DLSSNR.Enabled
DLSS.Indicator.Invert.X.Axis
DLSS.Indicator.Invert.Y.Axis
```

This strongly corroborates much of DvaKolbas's existing `NeuralRenderingRuntimeContract`, but **parameter presence is not proof of exact value type, resource state, payload offset, or per-mode usage**. Those are Pass 2 targets.

## 12. AIO18 has immediate and Present-time evaluation implementations

PD diagnostics explicitly distinguish:

```text
... immediate local D3D11 ...
... immediate D3D12 ...
... present evaluate ...
... local present ...
```

There are also separate diagnostics for missing committed staging, missing output share pairs, and failure to begin a local command list.

Examples:

```text
DLSSNR: BeginLocalCommandList failed for immediate evaluate.
DLSSNR: missing output share pair for immediate evaluate.
DLSSNR: immediate D3D12 evaluate missing output.
DLSSNR: BeginLocalCommandList failed for present evaluate.
DLSSNR: skip local eval, depth=%p motion=%p (need committed GPU1 staging, not NT-shared)
DLSSNR: failed to copy NT-shared backbuffer to local staging
```

**STATIC_OBSERVED:** multi-GPU/local execution is not a trivial pointer substitution. Some paths require committed local staging rather than directly consuming NT-shared textures.

## 13. AIO18 includes a private reconstruction/resolve layer around NGX NR

The binary contains diagnostics and PSO labels for:

```text
downsample
ratio
residual
ui compose
```

It also reports:

```text
DLSSNR: using external input scale %.3f work=%dx%d
DLSSNR: resolve scratch alloc failed, falling back to 1:1
DLSSNR: resolve pipeline unavailable, falling back to 1:1
DLSSNR: work-surface alloc failed, falling back to one pass
DLSSNR: extra pass %u failed, keeping %u pass(es)
DLSSNR: UI compose pipeline unavailable, skipping UI composite
```

The shipped INI exposes the corresponding controls:

```text
mDLSSNRResolveMethod = 0   # Auto / Residual / Ratio
mDLSSNRInputResolutionScale = 1.0
mDLSSNRTransferStrength
mDLSSNRColourStrength
mDLSSNRMaxRatio
mDLSSNRWhitePoint
mDLSSNRColorIsHdr
mDLSSNRPass = 1
```

**STATIC_OBSERVED:** AIO18's NR feature is more than “NGX color+depth+MV → output”. It can resize the NR work domain, run private resolve kernels, perform extra NR passes, and optionally compose/preserve UI around the result.

Pass 3 should reconstruct this pipeline in detail.

## 14. Module-path hook is a real compatibility mechanism

PD includes an `NGXNRHook` which:

- locates/loads `nvngx_dlssnr.dll`,
- requires Create/Evaluate/Release exports,
- IAT-hooks `GetModuleFileNameW` in the NR snippet,
- spoofs an `nvngx.dll`-style path,
- calls the snippet's D3D12 Init when available,
- restores the IAT hook on cleanup.

Representative diagnostics:

```text
NGXNRHook: spoof path = %ls
NGXNRHook: failed to IAT-hook GetModuleFileNameW on nvngx_dlssnr.dll.
NGXNRHook: installed IAT hook on nvngx_dlssnr.dll.
NGXNRHook: calling snippet D3D12_Init device=%p
```

This validates that DvaKolbas's existing module-path compatibility work is targeting a real AIO18 behavior, but Pass 2 should compare the exact path semantics and init sequence before declaring parity.

## 15. High-value differences to verify against DvaKolbas

The existing DvaKolbas NR implementation already contains several concepts now supported by AIO18 static evidence:

- pinned `nvngx_dlssnr.dll` 310.8 runtime identity,
- feature creation through NGX D3D12 exports,
- module-path hook,
- `DLSSNR.Width/Height/Preset/ScalingRatio`,
- tuning/reset/depth-inverted/UI-correction keys,
- full-resolution reconstruction contract for the 310.8 build,
- optional second/pass reconstruction logic,
- Ratio/Residual resolve paths,
- UI kept separate from the reconstructed scene.

However, this pass does **not** yet prove DvaKolbas matches AIO18 in:

- exact NGX parameter types,
- full `DLSSNREvaluateParams` offset map,
- exact resource-to-key mapping,
- subrect values,
- motion-vector sign/scale,
- `Backbuffer` aliasing semantics,
- UI/UIAlpha/control-mask rules,
- input/output resource states,
- local-vs-companion staging,
- resolve equations and pass ordering,
- reset/recreation timing.

Those are the main Pass-2 and Pass-3 comparison items.

## 16. Recommended Pass 2 target list

Trace `PD+0xA0B10` end-to-end and recover every NGX `Set` call used for one evaluation. Produce a table:

```text
DLSSNREvaluateParams offset
 -> semantic field
 -> NGX key
 -> value type
 -> D3D11/D3D12 resource
 -> dimensions/subrect
 -> expected state
 -> nullability
 -> immediate/present/local/companion branches
```

Priority fields:

1. Color, Output and Backbuffer distinction.
2. Depth and MVec including scales and jitter convention.
3. UI vs UIAlpha vs ControlMask.
4. BidirectionalDistortionField.
5. Reset and DepthInverted.
6. full set of subrects.
7. tuning value types for 310.8.
8. exact selector at payload `+0x110`.
9. local/companion staging and fence ownership.

## 17. Runtime breakpoints for later validation

For this exact AIO18 hash set:

```text
SkyrimUpscaler +0x2A2120  common NR init producer
SkyrimUpscaler +0x2A226B  InitDLSSNR call
SkyrimUpscaler +0x2A2300  common evaluate payload producer
SkyrimUpscaler +0x2A25E0  EvaluateDLSSNR call
SkyrimUpscaler +0x294E24  before-upscale NR call
SkyrimUpscaler +0x2AB0CF  late/Present NR call
SkyrimUpscaler +0x2AB5CC  late conditional NR call
SkyrimUpscaler +0x2ABF50  late conditional NR call

PDPerfPlugin +0xF4960      internal NR init
PDPerfPlugin +0xF4AE0      D3D11/interop evaluate router
PDPerfPlugin +0xA0B10      deeper NR backend evaluator
PDPerfPlugin +0xF4E30      release router
PDPerfPlugin +0xA1D20      backend release helper

nvngx_dlssnr +0x15750      NVSDK_NGX_D3D12_CreateFeature
nvngx_dlssnr +0x159C0      NVSDK_NGX_D3D12_EvaluateFeature
nvngx_dlssnr +0x16040      NVSDK_NGX_D3D12_ReleaseFeature
```

All RVAs above apply only to the exact module hashes in section 2.

## 18. Pass-1 conclusion

The architecture is now bounded enough to guide implementation work:

```text
Skyrim render integration
  -> SkyrimUpscaler NR scheduler / 0x138 payload producer
      -> PDPerfPlugin UpscaleAPI_D3D11 routing
          -> local game-GPU D3D12 OR companion-GPU backend
              -> private staging/resolve/multi-pass layer
                  -> nvngx_dlssnr.dll NGX D3D12 feature
              -> result / UI handling
  -> before-upscale OR late/Present scheduling depending mode
```

The most important architectural result is that AIO18 NR is **not** a single fixed postprocess call. It supports different scheduling points, multiple device ownership modes, immediate/Present evaluation, private reconstruction/resolve, and extra passes. Reproducing only the NGX parameter names would miss a large part of the integration contract.

