# AIO18 Neural Rendering Reverse-Engineering Log — Passes 1–5

**Target:** PureDark Skyrim Upscaler AIO18 Hotfix 1  
**Archive:** `SkyrimUpscalerAIOBuild18-Hotfix1(1).7z`  
**Archive SHA-256:** `136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8`  
**Static RE passes:** 4 discovery passes + 1 cross-pass review  
**Execution scope:** the AIO18 target binaries were not executed by this analysis. Runtime-only claims are explicitly marked unresolved.

---

## 1. Final architecture at a glance

```text
Skyrim / renderer integration
        ↓
SkyrimUpscaler.dll
  - NR scheduling
  - 0x138 external evaluate payload
        ↓
PDPerfPlugin.dll
  - D3D11/D3D12 routing
  - local vs companion GPU staging
  - transform 0x138 external ABI -> 0x110 NGX block
  - private work-resolution / resolve / multi-pass layer
        ↓
nvngx_dlssnr.dll 310.8.0.0
  - NGX D3D12 feature creation/evaluation
        ↓
PD private reconstruction
  - Direct / Residual / Ratio
  - optional additional NR passes
  - private UI composition
        ↓
SR or presentation / FG depending scheduling mode
```

Default late path:

```text
scene → SR → NR → FG preparation/presentation
```

Alternate before-upscale path:

```text
scene/guides → NR → SR
```

---

## 2. Exact module identities

| Module | SHA-256 | Role |
|---|---|---|
| `SkyrimUpscaler.dll` | `5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81` | Skyrim-facing scheduler and external payload producer |
| `PDPerfPlugin.dll` | `53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1` | interop/backend/reconstruction owner |
| `nvngx_dlssnr.dll` | `8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206` | NVIDIA NR runtime 310.8.0.0 |

PD CodeView identity recovered in Pass 1:

```text
GUID 5505ca9d-fcdf-40c6-8bf2-1f3b26ce2b25
age  1
```

NR runtime CodeView identity:

```text
GUID 4413bf99-b777-416c-907a-c4e6fcf26fdd
age  1
```

All RVAs below are valid only for these exact module hashes.

---

## 3. Host → PD exported NR ABI

AIO18 host delay-IAT:

| Export | Host delay-IAT RVA |
|---|---:|
| `EvaluateDLSSNR` | `0x472C60` |
| `ReleaseDLSSNR` | `0x472C80` |
| `IsDLSSNRAvailable` | `0x472C88` |
| `InitDLSSNR` | `0x472C90` |

PD exports:

| Export | PD RVA |
|---|---:|
| `IsDLSSNRAvailable` | `0xF9F80` |
| `InitDLSSNR` | `0xFA150` |
| `EvaluateDLSSNR` | `0xFA180` |
| `ReleaseDLSSNR` | `0xFA240` |

Initialization chain:

```text
SkyrimUpscaler +0x2A2120
  -> ReleaseDLSSNR(0)
  -> IsDLSSNRAvailable()
  -> build init descriptor
  -> +0x2A226B InitDLSSNR

PD export +0xFA150
  -> UpscaleAPI_D3D11 vslot +0xF0
  -> PD +0xF4960
  -> allocate 0x748-byte NR backend
  -> constructor/helper +0x9B800
  -> backend initialization +0x9C9A0
```

The PD backend can choose local game-GPU D3D12 execution or a companion-GPU route.

---

## 4. External evaluation ABI: `0x138` bytes

`PDPerfPlugin!EvaluateDLSSNR +0xFA180` copies exactly:

```text
0x80 + 0x80 + 0x38 = 0x138 bytes
```

### 4.1 Resource fields

| Offset | Role |
|---:|---|
| `+0x008` | Color |
| `+0x010` | Motion vectors |
| `+0x018` | Depth |
| `+0x020` | Output |
| `+0x028` | ControlMask |
| `+0x030` | UI |
| `+0x038` | UIAlpha |
| `+0x040` | Backbuffer |
| `+0x048` | BidirectionalDistortionField |

The common Skyrim producer initially supplies:

```text
Color      = selected current color
MVec       = prepared host resource +0xBA8
Depth      = prepared host resource +0xB50
Output     = initial Color alias
Control    = null
UI         = route-dependent optional resource
UIAlpha    = route-dependent optional resource
Backbuffer = null
BDF        = null
```

PD subsequently stages/replaces resources depending on immediate/Present and local/companion execution.

### 4.2 Subrect fields

The external payload carries 4-dword subrect blocks in this order:

```text
+0x050 Color
+0x060 MVec
+0x070 Depth
+0x080 Output
+0x090 ControlMask
+0x0A0 UI
+0x0B0 UIAlpha
+0x0C0 Backbuffer
+0x0D0 BidirectionalDistortionField
```

Each is `{BaseX, BaseY, Width, Height}`.

### 4.3 Important tail fields

Pass 2 corrected the early interpretation of this area:

```text
+0x0E0  base MVecScaleX
+0x0E4  base MVecScaleY
+0x0E8  Intensity
+0x0EC  LocalToneStrength
+0x0F0  LocalStructureStrength
+0x0F4  SkinStructureStrength
+0x0F8  UseAutoMask
+0x0FC  Style
+0x100  Reset
+0x101  DepthInverted
+0x103  private UI-correction request
+0x114  ResolveMethod
+0x118.. private resolve/input-scale control region
+0x130  requested pass count/mode
```

The full field map is in `evidence/v2/external_payload_map.json`.

---

## 5. PD internal NGX block: `0x110` bytes

PD does **not** pass the external structure directly to NGX. It builds an internal `0x110` block whose resource prefix is:

| Offset | NGX role |
|---:|---|
| `+0x000` | Color |
| `+0x008` | MVec |
| `+0x010` | Depth |
| `+0x018` | Output |
| `+0x020` | ControlMask |
| `+0x028` | UI |
| `+0x030` | UIAlpha |
| `+0x038` | Backbuffer |
| `+0x040` | BidirectionalDistortionField |

The NGX writer is centered at:

```text
PDPerfPlugin +0x9AF70
```

Observed setter types:

```text
resources                      -> SetD3d12Resource
subrects                       -> SetUI
MVecScaleX/Y                   -> SetF
Intensity                      -> SetF
LocalToneStrength              -> SetF
LocalStructureStrength         -> SetF
SkinStructureStrength          -> SetF
UseAutoMask                    -> SetI
Style                          -> SetUI
Reset                          -> SetI
DepthInverted                  -> SetI
Enabled                        -> SetI
UICorrection                   -> SetI
DLSS indicator invert X/Y      -> SetI
```

Exact-zero MVec scales are normalized to `1.0f` by PD before NGX receives them.

---

## 6. Core NGX UI contract — one of the most important findings

Despite the exported parameter vocabulary, the mapped core AIO18 route does:

```text
DLSSNR.UI           = null
DLSSNR.UIAlpha      = null
DLSSNR.UICorrection = 0
```

Representative evidence:

```text
PD +0x9E643   zero source register used for UI inputs
PD +0x9E7EB   internal UI slot = null
PD +0x9E7F0   internal UIAlpha slot = null
PD +0x9FF96   internal UICorrection = 0
```

The host/private UI-correction request is therefore **not** passed through as NGX UI correction. PD uses it to drive its own post-NR UI handling.

Final AIO18 UI model:

```text
NGX NR on scene/guides
        ↓
private NR reconstruction
        ↓
PD private UI composition/correction
        ↓
final output
```

---

## 7. Core resource-state contract

Around NGX evaluation, the mapped D3D12 states are:

```text
Color  -> NON_PIXEL_SHADER_RESOURCE
Depth  -> NON_PIXEL_SHADER_RESOURCE
MVec   -> NON_PIXEL_SHADER_RESOURCE
Output -> UNORDERED_ACCESS
```

When Color and Output differ and an initial copy is required:

```text
Color  -> COPY_SOURCE
Output -> COPY_DEST
CopyResource
then transition to NGX read/write states
```

Optional-route resource states vary and should be validated live before copying them generically.

---

## 8. Scheduling sites and ordering

The common host evaluator is:

```text
SkyrimUpscaler +0x2A2300
  -> +0x2A25E0 EvaluateDLSSNR
```

Direct call sites recovered include:

```text
+0x18FA4B
+0x265B0A
+0x294E24
+0x297AE6
+0x2AB0CF
+0x2AB5CC
+0x2ABF50
```

### Before-upscale mode

```text
+0x294E24 NR
+0x294FBD SR / EvaluateUpscaler
```

Order:

```text
scene/guides → NR → SR
```

### Default late mode

The shipped setting defaults `mDLSSNRBeforeUpscaling=false`.

Representative normal sequence:

```text
SR landmark from FSR RE: +0x19FA14
late NR:                 +0x2AB0CF
FSR FG producer:          +0x2AB349
```

Order:

```text
scene → SR → late NR → FG preparation/presentation
```

### Community Shaders

`+0x265B0A` enters the same common evaluator with both mode arguments set to 1 and a resource wrapper sourced from host `+0xAF8`.

No NR-specific named ENB render callback was recovered. ReShade can alter late ordering through its already-mapped begin/finish/present events.

---

## 9. Input-resolution scale and reconstruction mode

Recovered input-scale behavior:

```text
0 or >= 1.0  -> 1.0 native scale
0 < scale <1 -> clamp to at least 0.25
```

### Mode selection

```text
Auto + native scale   -> Direct
Auto + reduced scale  -> Residual
Residual              -> Residual
Ratio                 -> Ratio
```

Auto is therefore policy, not a third reconstruction shader.

---

## 10. Residual reconstruction

The recovered high-level equation is:

```text
I_native + Lanczos3(NR_low - I_low)
```

AIO18 reconstructs the learned **change** instead of merely enlarging the low-resolution NR result.

The implementation is separable: one pass reconstructs a horizontal residual field, then the second reconstructs vertically and adds it to the original/native image.

---

## 11. Ratio reconstruction

AIO18's Ratio path is a luma-ratio + OkLab family reconstruction. Recovered user controls:

```text
TransferStrength
ColourStrength
MaxRatio
WhitePoint
ColorIsHdr
```

Embedded defaults:

```text
1.0, 1.0, 2.0, 1.0
```

DvaKolbas follows the same broad architecture but is **not formula-identical** and includes additional gamut/producer-domain logic. Treat it as an inspired extension, not byte-for-byte parity.

---

## 12. Multi-pass NR

AIO18's additional passes use separate feature ownership.

Recovered behavior:

```text
requested passes
  -> reconcile feature pool
  -> feature 0 evaluates
  -> feature 1 evaluates previous output
  -> ...
  -> final successful result enters reconstruction/UI pipeline
```

Observed feature pool bound:

```text
16 slots
```

New feature creation arms a per-feature reset byte. Successful evaluation clears that byte. If an extra pass fails, AIO18 keeps the last successful result:

```text
extra pass N failed, keeping K pass(es)
```

Optional work-surface failure can fall back to one pass rather than failing the whole frame.

---

## 13. Reset/history behavior

### Primary pass

The common host producer writes:

```text
Reset = 0
```

on every mapped common evaluation.

Evidence:

```text
SkyrimUpscaler +0x2A23BB  zero source
SkyrimUpscaler +0x2A24E5  store to external +0x100
```

No camera-cut writer to this primary reset field was recovered in the mapped common path.

### Extra passes

Per-feature reset bytes behave as first-use latches:

```text
new feature -> reset = 1
successful evaluate -> reset = 0
release -> reset = 0
```

No normal camera-cut re-arm was found in the mapped region.

### Implementation guidance

This is **not** behavior to copy blindly. DvaKolbas's explicit camera/history reset policy is stronger and should remain.

---

## 14. Lifecycle and recovery

AIO18 uses at least two recovery levels.

### Lightweight recovery

Feature creation can rebuild local command-list/swapchain resources and retry after BeginCommandList failure.

### Full device-recreate recovery

Representative path:

```text
PD +0xA1830  recovery entry
+0xA185B     full shutdown
+0xA1878     full reinitialization
```

It snapshots configuration, tears down backend/device ownership and initializes again.

### Feature pool / work-size changes

Work-size or pass-count changes can release/create feature handles and rebuild private work surfaces. This is not merely a change of subrect metadata.

---

## 15. Release and synchronization

Release chain:

```text
PD export ReleaseDLSSNR +0xFA240
 -> routing +0xF4E30
 -> backend registry release +0xA1D20
 -> backend shutdown/release helpers
 -> ReleaseFeature / DestroyParameters
```

Mapped release code invokes synchronization/ownership helpers before dropping resources, but static RE has **not proven the exact fence/event completion semantics**.

One cleanup concern remains:

```text
ReleaseFeature attempted
possible failure diagnostic
feature pointer cleared anyway
```

Whether this is benign device-loss cleanup or abandoned vendor ownership is runtime-only.

---

## 16. Local vs companion GPU

AIO18 supports both local game-GPU and companion-GPU NR.

Companion/local execution is not a simple shared-pointer route. Diagnostics prove requirements for committed staging:

```text
need committed GPU1 staging, not NT-shared
failed to copy NT-shared backbuffer to local staging
skip present eval, committed color/output missing
```

So any future DvaKolbas companion-GPU implementation must design explicit committed local staging and cross-adapter synchronization.

---

## 17. Module-path compatibility hook

AIO18 includes a real `NGXNRHook` that:

- locates/loads `nvngx_dlssnr.dll`;
- requires Create/Evaluate/Release exports;
- IAT-hooks `GetModuleFileNameW` in the NR snippet;
- spoofs an `nvngx.dll`-style module path;
- initializes the D3D12 NR snippet when needed;
- restores the IAT hook on cleanup.

This strongly corroborates DvaKolbas's module-path compatibility mechanism.

---

## 18. DvaKolbas current parity review

Current source identity used for this final review:

```text
wallhead/DvaKolbas
codex/fsr-sr
f6a29f75db0029acf013227627a52e2e490733c6
```

The relevant NR Git blobs are unchanged from the Pass-4 review.

### Strong matches

- pinned 310.8 NR runtime identity;
- direct NGX D3D12 feature creation/evaluation;
- module-path hook;
- Width/Height/Preset/ScalingRatio creation keys;
- correct NGX parameter types for the 310.8 contract;
- same high-level Auto/Residual/Ratio model;
- same minimum reduced scale 0.25;
- private reconstruction and private UI composition;
- optional multiple feature passes;
- work-size-driven recreation;
- explicit local D3D11↔D3D12 retirement.

### DvaKolbas behavior that is stronger and should remain

```text
camera/history reset propagation
second-pass reset inheritance
retire/drain before feature destruction
retain owner after failed/unsubmitted NR recording
```

### Highest-confidence parity mismatch

AIO18 core:

```text
UI=null
UIAlpha=null
UICorrection=0
private composition=yes
```

DvaKolbas current path:

```text
UI=a_input.ui
UIAlpha=a_input.uiAlpha
UICorrection=tuning.uiCorrection
private composition=yes
```

Default Dva tuning has `uiCorrection=false`, which reduces the practical difference in default configuration, but the resource inputs are still supplied and the setting is user-controllable.

Classification:

```text
PARITY MISMATCH — NOT YET A PROVEN BUG
```

Recommended experiment: A/B the AIO-style NGX UI-null contract without changing the private Dva composition path.

### DvaKolbas extensions, not AIO18 parity

- peripheral 80/90 compression;
- fused preparation;
- producer-color HDR proxy;
- independently sized/configured pass 2;
- custom extended-range/AP1/gamut reconstruction.

---

## 19. Corrections timeline

| Earlier interpretation | Final corrected interpretation | Closed in |
|---|---|---:|
| `+0xE0/+0xE4` may be jitter | MVecScaleX/MVecScaleY | Pass 2 |
| external `0x138` may directly represent NGX structure | separate transformed `0x110` internal NGX block | Pass 2 |
| presence of UI strings may mean NGX UI correction is active | core path nulls UI/UIAlpha and forces UICorrection=0 | Pass 2 |
| Auto may be a reconstruction shader | Auto is selector: Direct or Residual by scale | Pass 3 |
| multipass may mean repeated use of one feature | separate feature handles, bounded pool, sequential passes | Pass 3 |

Passes 4 and 5 found no contradiction reversing these corrections.

---

## 20. Confidence / evidence classification

### High-confidence static facts

- exact target hashes and exports;
- 0x138 external payload size;
- 0x110 internal NGX block size;
- key resource offsets and NGX setter types;
- core NGX UI/UIAlpha null and UICorrection zero;
- input-scale clamp and Auto/Residual/Ratio policy;
- Residual architecture;
- multi-feature pass pool and fail-soft extra-pass behavior;
- primary common Reset zero;
- before-SR and normal SR→NR→FG ordering landmarks;
- local/companion committed-staging requirement.

### Medium-confidence/integration-dependent

- exact semantic ownership of some specialized host wrappers such as CS `host+0xAF8`;
- optional-route Backbuffer/ControlMask/BDF usage outside the mapped core;
- exact ENB/ReShade interposition order in every configuration.

### Runtime-only unresolved

- actual live Reset under camera cuts/loading;
- exact release fence/event completion;
- feature handle behavior during device loss;
- active companion-GPU selection and latency;
- exact resource states/dimensions in every live branch;
- visual/performance impact of AIO-style UI-null vs current Dva contract.

---

## 21. Runtime validation breakpoint set

```text
SkyrimUpscaler +0x2A2120  NR init producer
SkyrimUpscaler +0x2A226B  InitDLSSNR call
SkyrimUpscaler +0x2A2300  external evaluate payload producer
SkyrimUpscaler +0x2A25E0  EvaluateDLSSNR call
SkyrimUpscaler +0x294E24  before-upscale NR
SkyrimUpscaler +0x2AB0CF  normal late NR
SkyrimUpscaler +0x2AB349  following FSR-FG producer

PDPerfPlugin +0x9AF70      NGX block writer
PDPerfPlugin +0x9F3A2      extra-pass reset load
PDPerfPlugin +0x9F408      extra-pass reset clear
PDPerfPlugin +0xA1770      pre-release synchronization helper
PDPerfPlugin +0xA18DC      extra ReleaseFeature
PDPerfPlugin +0xA1C4F      primary ReleaseFeature
PDPerfPlugin +0xA185B      full device-recreate shutdown
PDPerfPlugin +0xA1878      full device-recreate reinit

nvngx_dlssnr +0x15750      CreateFeature
nvngx_dlssnr +0x159C0      EvaluateFeature
nvngx_dlssnr +0x16040      ReleaseFeature
```

At `EvaluateFeature`, capture:

```text
feature handle
command list / queue
all resource pointers + D3D12 descriptions
subrect values
MVecScaleX/Y
Reset
DepthInverted
UI/UIAlpha/UICorrection
current resource states
```

At release, capture queue/fence values both before and after the PD synchronization helper.

---

## 22. Final implementation guidance

For DvaKolbas:

1. **Keep** the stronger reset/history model.
2. **Keep** drain-before-destruction and fail-retain ownership.
3. **Keep** private UI composition as the authoritative UI stage.
4. **A/B test** null NGX UI/UIAlpha and force UICorrection=0 before changing any UI behavior permanently.
5. Do not claim peripheral compression, fused preparation, independent pass-2 sizing or custom HDR/gamut reconstruction as AIO18 parity.
6. If companion-GPU NR is added, design committed local staging and explicit cross-adapter synchronization first.
7. Runtime validation should now target the small unresolved set above; another broad static pass has diminishing value.

---

## 23. Verification status

Fresh Pass-5 review reran the Pass-1 through Pass-4 verifiers against the original AIO18 module bytes:

```text
Pass 1: 13 identity/string checks — pass
Pass 2: 86 checks — 0 failures
Pass 3: 14 checks — 0 failures
Pass 4: 43 checks — 0 failures
```

Pass 5 adds cross-pass consistency checks, current Dva source-identity checks, package-integrity checks and correction-history assertions. See `evidence/v5/verification_v5.json`.

---

## 24. Final status

The **static NR RE is complete for implementation purposes**. The recovered architecture is detailed enough to audit and guide DvaKolbas without guessing at the major contracts. The remaining uncertainty is concentrated in runtime-only behavior rather than missing static architecture.
