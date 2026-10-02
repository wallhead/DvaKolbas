# AIO18 DLSS Neural Rendering / Neural Reconstruction RE — Pass 2

**Target archive:** `SkyrimUpscalerAIOBuild18-Hotfix1(1).7z`  
**Archive SHA-256:** `136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8`  
**Analysis type:** static reverse engineering only; target DLLs were not executed.  
**Pass goal:** recover the exact AIO18 external NR evaluation ABI, the internal NGX parameter block, setter types, subrect/resource rules, motion-scale semantics, core resource states, UI-correction ownership, and route-specific Backbuffer behavior.

## 1. Exact build identity

All addresses in this report are RVAs relative to these exact binaries:

| Module | SHA-256 |
|---|---|
| `SkyrimUpscaler.dll` | `5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81` |
| `PDPerfPlugin.dll` | `53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1` |
| `nvngx_dlssnr.dll` | `8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206` |

The NVIDIA runtime is file/product version **310.8.0.0**.

Pass 1 established the host/PD boundary and is retained in the v2 evidence package. The important AIO18 calls remain:

```text
SkyrimUpscaler +0x2A2300  common 0x138 evaluate producer
SkyrimUpscaler +0x2A25E0  -> PDPerfPlugin!EvaluateDLSSNR
PDPerfPlugin    +0xF4AE0   D3D11/interop routing
PDPerfPlugin    +0xA0B10   NR backend route selector
```

## 2. Critical structural correction: there are two different parameter structures

The external Skyrim/PD ABI and the actual NGX evaluation parameter block are **not the same structure**.

```text
SkyrimUpscaler
    builds 0x138-byte DLSSNREvaluateParams
              ↓
PD route / staging / resolve logic
              ↓
PD +0x9FDB0 transforms it
              ↓
0x110-byte internal NGX parameter block
              ↓
PD +0x9AF70 writes NGX parameters
              ↓
NVSDK_NGX_D3D12_EvaluateFeature
```

This distinction matters because PD changes resources, subrects and flags before NGX sees them. Copying external offsets directly into a new NGX integration would reproduce the ABI incorrectly.

## 3. Exact 0x138 external evaluation ABI

The following map is now supported by the common host producer plus the immediate/Present consumers and the internal builder.

### 3.1 Resource section

| External offset | Role | Common Skyrim producer |
|---:|---|---|
| `+0x00` | route/header dword; semantic name unresolved | `0` |
| `+0x08` | **Color** | caller-selected color |
| `+0x10` | **MVec** | prepared motion, host `+0xBA8` |
| `+0x18` | **Depth** | prepared depth, host `+0xB50` |
| `+0x20` | **Output** | initially aliases Color |
| `+0x28` | **ControlMask** | null |
| `+0x30` | **UI** | optional PD/private-correction resource |
| `+0x38` | **UIAlpha** | optional; independently confirmed by PD diagnostic |
| `+0x40` | **Backbuffer** | null in the common producer |
| `+0x48` | **BidirectionalDistortionField** | null in the common producer |

Evidence for the less-obvious names includes:

- `PD+0xA0957` rejects a null external `+0x20` with `DLSSNR: immediate D3D12 evaluate missing output.`
- `PD+0x9F5F1` contains `DLSSNR debug: UIAlpha overwrite requested but uiAlpha is null`, tied to the external `+0x38` route.
- Present handling treats external `+0x40` independently and contains Backbuffer/Color alias handling.
- The external resource order transforms into the exact NGX resource order in the 0x110 internal block.

### 3.2 Subrect section

Nine 16-byte subrects follow in the same resource order:

| External range | Role |
|---|---|
| `+0x50..+0x5F` | Color BaseX/BaseY/Width/Height |
| `+0x60..+0x6F` | MVec |
| `+0x70..+0x7F` | Depth |
| `+0x80..+0x8F` | Output |
| `+0x90..+0x9F` | ControlMask |
| `+0xA0..+0xAF` | UI |
| `+0xB0..+0xBF` | UIAlpha |
| `+0xC0..+0xCF` | Backbuffer |
| `+0xD0..+0xDF` | BidirectionalDistortionField |

**STATIC_OBSERVED:** the common Skyrim producer initializes these subrect blocks to zero. They are not equivalent to DvaKolbas's current explicit full-size subrect writes.

PD later transforms the blocks. When a private work/resolve size is active, `PD+0x9FDB0` explicitly overrides effective Color and Output width/height. When private UI correction is active and an external UI resource exists, the builder can select the external UI subrect as the effective Color rectangle.

An odd but currently inert detail: the builder copies external `+0xB0` into both internal UI and UIAlpha subrect areas. In the core path mapped below, both corresponding NGX resources are null.

### 3.3 Scalar/tuning/private-resolve tail

| External offset | Role |
|---:|---|
| `+0xE0` | **base MVecScaleX** |
| `+0xE4` | **base MVecScaleY** |
| `+0xE8` | Intensity |
| `+0xEC` | LocalToneStrength |
| `+0xF0` | LocalStructureStrength |
| `+0xF4` | SkinStructureStrength |
| `+0xF8` | UseAutoMask byte |
| `+0xFC` | Style dword |
| `+0x100` | Reset byte |
| `+0x101` | DepthInverted byte |
| `+0x102` | Enabled byte |
| `+0x103` | **PD-private UI-correction request** |
| `+0x108` | route/private qword; common producer writes zero; exact semantic unresolved |
| `+0x110` | route selector |
| `+0x114` | ResolveMethod |
| `+0x118` | InputResolutionScale |
| `+0x11C` | TransferStrength |
| `+0x120` | ColourStrength |
| `+0x124` | MaxRatio |
| `+0x128` | WhitePoint |
| `+0x12C` | effective ColorIsHdr byte |
| `+0x130` | effective pass count/mode dword |

### Pass-1 correction: `+0xE0/+0xE4` are not jitter

Pass 1 tentatively described these as jitter because the host copies its `+0x10/+0x14` values into them. The downstream builder proves they are **motion-vector scales**:

```text
PD +0x9FEEA: effectiveScale * external[+0xE0] -> internal MVecScaleX
PD +0x9FEFC: effectiveScale * external[+0xE4] -> internal MVecScaleY
```

The NGX writer then publishes those values as `DLSSNR.MVecScaleX/Y`.

This is also consistent with the earlier FSR RE, where AIO18's actual pixel jitter is stored in a different host pair (`+0x08/+0x0C`).

## 4. Exact Skyrim host setting fields

The AIO18 settings loader independently identifies these host offsets:

| Host offset | Setting |
|---:|---|
| `+0x2AA` | `mDLSSNRBeforeUpscaling` |
| `+0x2AC` | `mDLSSNRPreset` |
| `+0x2B0` | `mDLSSNRStyle` |
| `+0x2B4` | `mDLSSNRIntensity` |
| `+0x2B8` | `mDLSSNRLocalToneStrength` |
| `+0x2BC` | `mDLSSNRLocalStructureStrength` |
| `+0x2C0` | `mDLSSNRSkinStructureStrength` |
| `+0x2C4` | `mDLSSNRUseAutoMask` |
| `+0x2C5` | `mDLSSNRUICorrection` |
| `+0x2C8` | `mDLSSNRResolveMethod` |
| `+0x2CC` | `mDLSSNRInputResolutionScale` |
| `+0x2D0` | `mDLSSNRTransferStrength` |
| `+0x2D4` | `mDLSSNRColourStrength` |
| `+0x2D8` | `mDLSSNRMaxRatio` |
| `+0x2DC` | `mDLSSNRWhitePoint` |
| `+0x2E0` | `mDLSSNRColorIsHdr` |
| `+0x2E4` | `mDLSSNRPass` |

This corrects an earlier tentative shifted interpretation encountered while tracing the tail.

The common producer maps the four tuning floats from host `+0x2B4..+0x2C0` to external `+0xE8..+0xF4`, AutoMask to `+0xF8`, Style to `+0xFC`, depth convention to `+0x101`, and private resolve controls to `+0x114..+0x130`.

## 5. Exact internal 0x110 NGX block

The only observed core NGX parameter writer is at:

```text
PDPerfPlugin +0x9AF70
```

The actual path into it is:

```text
PD +0x9E5A0
  -> PD +0x9FDB0     external->internal builder
  -> PD +0x9B7D0
  -> PD +0x9AF70     NGX Set* writer
  -> PD +0xA4240     evaluate helper
```

### 5.1 Resource parameters

| Internal offset | NGX parameter | Setter |
|---:|---|---|
| `+0x00` | `DLSSNR.Color` | `SetD3d12Resource` |
| `+0x08` | `DLSSNR.MVec` | `SetD3d12Resource` |
| `+0x10` | `DLSSNR.Depth` | `SetD3d12Resource` |
| `+0x18` | `DLSSNR.Output` | `SetD3d12Resource` |
| `+0x20` | `DLSSNR.ControlMask` | `SetD3d12Resource` |
| `+0x28` | `DLSSNR.UI` | `SetD3d12Resource` |
| `+0x30` | `DLSSNR.UIAlpha` | `SetD3d12Resource` |
| `+0x38` | `DLSSNR.Backbuffer` | `SetD3d12Resource` |
| `+0x40` | `DLSSNR.BidirectionalDistortionField` | `SetD3d12Resource` |

### 5.2 Subrects

The internal block has the same nine-role ordering beginning at `+0x48`; every BaseX/BaseY/Width/Height value is published with **`NVSDK_NGX_Parameter_SetUI`**.

```text
+0x48..0x57  Color
+0x58..0x67  MVec
+0x68..0x77  Depth
+0x78..0x87  Output
+0x88..0x97  ControlMask
+0x98..0xA7  UI
+0xA8..0xB7  UIAlpha
+0xB8..0xC7  Backbuffer
+0xC8..0xD7  BidirectionalDistortionField
```

### 5.3 Scalars and exact NGX value types

| Internal offset | NGX key | Setter/type |
|---:|---|---|
| `+0xD8` | `DLSSNR.MVecScaleX` | `SetF` |
| `+0xDC` | `DLSSNR.MVecScaleY` | `SetF` |
| `+0xE0` | `DLSSNR.Intensity` | `SetF` |
| `+0xE4` | `DLSSNR.LocalToneStrength` | `SetF` |
| `+0xE8` | `DLSSNR.LocalStructureStrength` | `SetF` |
| `+0xEC` | `DLSSNR.SkinStructureStrength` | `SetF` |
| `+0xF0` | `DLSSNR.UseAutoMask` | **`SetI`** |
| `+0xF4` | `DLSSNR.Style` | **`SetUI`** |
| `+0xF8` | `DLSSNR.Reset` | **`SetI`** |
| `+0xFC` | `DLSSNR.DepthInverted` | **`SetI`** |
| `+0x100` | `DLSSNR.Enabled` | **`SetI`** |
| `+0x104` | `DLSSNR.UICorrection` | **`SetI`** |
| `+0x108` | `DLSS.Indicator.Invert.X.Axis` | **`SetI`** |
| `+0x10C` | `DLSS.Indicator.Invert.Y.Axis` | **`SetI`** |

The writer has a specific zero fallback for motion scales: exact zero becomes **1.0f** before `SetF`.

**STATIC_OBSERVED:** DvaKolbas's 310.8 reconstruction-build type policy matches these setter classes: AutoMask/Reset/DepthInverted/Enabled/UICorrection are signed integer setters, Style is unsigned, and the tuning/scales are floats.

## 6. Core AIO18 NGX path does not pass UI/UIAlpha to NR

This is the most important Pass-2 result.

At `PD+0x9E643` the core evaluator clears `ESI`:

```asm
xor esi, esi
```

It then writes that zero value into the internal resource block at:

```text
PD +0x9E7EB -> internal +0x28 = null  // DLSSNR.UI
PD +0x9E7F0 -> internal +0x30 = null  // DLSSNR.UIAlpha
```

The NGX writer later calls `SetD3d12Resource` for both keys with those null values.

So for the single mapped core NGX writer:

```text
DLSSNR.UI      = null
DLSSNR.UIAlpha = null
```

This does **not** mean AIO18 ignores UI. External payload `+0x30/+0x38` remain meaningful to PD's private layer. The wrapper contains the explicit diagnostic:

```text
DLSSNR debug: UIAlpha overwrite requested but uiAlpha is null
```

and tests external byte `+0x103` to decide private correction/composition behavior.

## 7. AIO18 also forces NGX `DLSSNR.UICorrection = 0`

`PD+0x9FDB0` reads the external private correction flag at `+0x103`, but the same builder explicitly stores zero into internal NGX field `+0x104`:

```asm
PD +0x9FF96:
    mov dword ptr [r10+0x104], ecx   ; ECX == 0
```

Therefore the core NGX call receives:

```text
DLSSNR.UICorrection = 0
```

while AIO18's private PD correction/composition layer is driven by external `+0x103`.

**STATIC_OBSERVED:** AIO18 separates the concepts:

```text
host mDLSSNRUICorrection
        ↓
external/private correction request (+0x103)
        ↓
PD private UI/reconstruction logic

NGX DLSSNR.UICorrection
        ↓
forced 0 in mapped core path
```

That distinction was not recoverable from parameter-name strings alone.

## 8. Backbuffer is a distinct route input, not simply Output

The common Skyrim producer leaves external `+0x40` Backbuffer null, but route logic can supply one.

The immediate D3D12 path `PD+0xA0910`:

1. requires external Output (`+0x20`),
2. loads Color/MVec/Depth/Output/ControlMask/UI/UIAlpha/BDF from the external payload,
3. selects Backbuffer from route/context state when available,
4. otherwise falls back to Output.

The Present path `PD+0x9FFB0` additionally consumes external `+0x40` and has special logic when the Backbuffer aliases Color.

**STATIC_OBSERVED:** requiring a non-null external Backbuffer is not an AIO18 host-ABI requirement. PD can select/fallback internally depending on the route.

## 9. Core D3D12 resource-state contract

The core evaluator at `PD+0x9E5A0` performs explicit state preparation.

When Color and Output differ and the copy path is accepted:

```text
Color  -> D3D12_RESOURCE_STATE_COPY_SOURCE (0x800)
Output -> D3D12_RESOURCE_STATE_COPY_DEST   (0x400)
CopyResource/related copy
```

Before NGX evaluation, the observed core states are:

```text
Color  -> NON_PIXEL_SHADER_RESOURCE (0x40)
Output -> UNORDERED_ACCESS           (0x08)
Depth  -> NON_PIXEL_SHADER_RESOURCE  (0x40), when present
MVec   -> NON_PIXEL_SHADER_RESOURCE  (0x40), when present
```

Optional ControlMask/UI/UIAlpha/BDF/Backbuffer state handling varies with route/staging and is intentionally not generalized here.

## 10. Local/companion staging preserves the semantic payload roles

The Present/local paths do not simply reinterpret external D3D11/shared pointers. They check each non-null semantic input and resolve/stage corresponding D3D12 resources before entering the core builder.

The immediate route directly demonstrates the external role order:

```text
external +0x08 Color
external +0x10 MVec
external +0x18 Depth
external +0x20 Output
external +0x28 ControlMask
external +0x30 UI
external +0x38 UIAlpha
external +0x48 BidirectionalDistortionField
```

Present routing adds the separate external Backbuffer at `+0x40`.

This reinforces Pass 1's conclusion that companion-GPU support is a real staging architecture, not pointer substitution.

## 11. Static comparison with DvaKolbas NR

Comparison target during this pass: DvaKolbas `codex/fsr-sr` at reviewed head `7f4ac08539b9cf655bfd2d0e6fc0f8c344daeb34`.

### Matches now strongly corroborated

DvaKolbas already matches AIO18 in several important 310.8 details:

- same NGX resource-key vocabulary,
- same reconstruction runtime hash `8270...cc206`,
- `SetI` semantics for AutoMask/Reset/DepthInverted/Enabled/UICorrection on the reconstruction build,
- `SetUI` for Style,
- float tuning and motion scales,
- independent private resolve/reconstruction layers,
- module-path compatibility handling.

### Static behavior differences

| Topic | AIO18 mapped behavior | DvaKolbas current behavior |
|---|---|---|
| NGX `UI` / `UIAlpha` | core block forces both null | forwards them when supplied |
| NGX `UICorrection` | forced to `0`; PD private path owns correction | requested `uiCorrection` is written into NGX |
| Subrect policy | common payload starts zero; PD selectively transforms effective rectangles | writes explicit full subrects for core/optional inputs |
| Backbuffer | external value may be null; route selects/falls back | `RecordEvaluation` requires non-null Backbuffer |

These are **not automatically DvaKolbas bugs**. They are implementation-contract differences that deserve targeted tests and Pass-3/Pass-4 review before altering a working path.

The UI difference is especially important: DvaKolbas currently both has its own private post-NR composition and can also request NGX UI correction. AIO18's mapped core route keeps these responsibilities separated.

## 12. Pass-2 corrections to earlier assumptions

1. External `+0xE0/+0xE4` are MVec scales, not jitter.
2. The external 0x138 ABI must not be used as the NGX parameter struct.
3. AIO18's external UI resources do not imply that they are sent to NGX.
4. AIO18's user-facing UI-correction setting does not imply `DLSSNR.UICorrection=1`; the mapped core path forces it off.
5. A non-null Backbuffer in the final NGX block can be selected by PD even when the external Skyrim payload leaves Backbuffer null.

## 13. Reproducibility / verification

The v2 verifier performs fresh checks against the original AIO18 files, including:

- exact archive and three module SHA-256 identities,
- selected host and PD instruction bytes at mapped RVAs,
- all required NR resource/scalar parameter strings,
- all host NR setting names,
- external map size `0x138`,
- internal NGX block size `0x110`,
- selected resource/tail offsets,
- the core null-UI/null-UIAlpha/zero-UICorrection invariant.

Fresh result for this package:

```text
PASS
86 checks
0 failures
```

This verifies static identity/map evidence only. It does not execute Skyrim, NGX or the GPU.

## 14. Pass 3 target

Pass 3 should now focus on the layer **around** the exact NGX contract rather than re-mapping it:

1. reconstruct Auto / Residual / Ratio resolve equations and shader/resource flow;
2. map `InputResolutionScale` to actual work dimensions and guide scaling;
3. determine exact color/HDR transfer behavior for `TransferStrength`, `ColourStrength`, `MaxRatio`, `WhitePoint`, `ColorIsHdr`;
4. reconstruct private UI correction/composition, including how external UI/UIAlpha are interpreted;
5. map extra-pass behavior and pass-to-pass resources;
6. compare those equations/resource flows directly against DvaKolbas's current `SourceDLSSGNeuralResolve*` and second-pass implementation.

At the end of Pass 2, the core NGX input contract is no longer the major unknown. The remaining complexity is AIO18's private reconstruction, color, UI and multi-pass machinery around that call.
