# AIO18 DLSS Neural Rendering / Neural Reconstruction RE — Pass 3

**Target:** `SkyrimUpscalerAIOBuild18-Hotfix1(1).7z`  
**Archive SHA-256:** `136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8`  
**Analysis:** static reverse engineering of the exact AIO18 binaries. No Skyrim/GPU execution in this pass.  
**Goal:** reconstruct AIO18's private reconstruction layer around NGX NR: work resolution, Auto/Residual/Ratio selection, multi-pass behavior, color controls, and private UI composition.

## 1. Status after Pass 3

Three of the four planned static passes are now complete.

The remaining static pass should focus on lifecycle/integration correctness: reset/recreation, resize, device loss, companion/local GPU retirement, before/after-SR scheduling, ENB/ReShade/CS interaction, FG handoff, and race/resource-lifetime audit.

A fifth runtime pass remains strongly recommended.

## 2. Exact binaries

- `SkyrimUpscaler.dll` SHA-256 `5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81`
- `PDPerfPlugin.dll` SHA-256 `53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1`
- `nvngx_dlssnr.dll` SHA-256 `8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206`

All RVAs below are valid only for this exact `PDPerfPlugin.dll`.

## 3. Input-scale policy is now recovered exactly

Helper `PD+0x9CFC0` normalizes `mDLSSNRInputResolutionScale`:

```text
if 0 < scale < 1:
    effectiveScale = max(scale, 0.25)
else:
    effectiveScale = 1.0
```

The literal constants at the referenced read-only data are exactly `0.25f` and `1.0f`.

This matches the shipped INI comment:

```text
0 or >=1 = no downsample. Otherwise clamped to [0.25, 1).
```

Work width/height are produced by multiplying the selected base dimensions by that effective scale and rounding to integer dimensions, with a minimum effective extent of one pixel.

**STATIC_OBSERVED:** external input scaling does not change Skyrim's source textures directly. It creates a private NR work domain inside PD.

## 4. Auto / Residual / Ratio selection

Helper `PD+0x9CFF0` and the reconstruction setup establish that the three user-visible modes are not three independent pipelines.

The shipped configuration semantics are confirmed by code:

```text
ResolveMethod 0 = Auto
ResolveMethod 1 = Residual
ResolveMethod 2 = Ratio
```

Effective behavior:

```text
Explicit Ratio:
    always use Ratio reconstruction

Explicit Residual:
    use Residual reconstruction

Auto:
    effectiveScale >= 1.0 -> direct 1:1 NR result
    effectiveScale <  1.0 -> Residual reconstruction
```

So **Auto is a selector**, not its own reconstruction shader.

This matches the AIO18 INI text exactly:

```text
Auto: scale>=1 uses 1:1; scale<1 uses Residual
Residual: I + Lanczos3(NR_low - I_low)
Ratio: luma ratio + OkLab
```

## 5. Direct 1:1 route

When no reduced work domain requires reconstruction, AIO18 avoids a private reconstruction resolve and uses the normal NR result directly.

The route still runs normal resource-state preparation and NGX evaluation, but there is no Residual/Ratio restoration step required solely to return to native dimensions.

This is important for implementation: `mDLSSNRResolveMethod=Auto` with scale 1.0 is a materially simpler path than `Ratio` at scale 1.0.

## 6. Residual reconstruction

The binary and shipped configuration identify Residual as:

```text
I + Lanczos3(NR_low - I_low)
```

The surrounding control flow establishes the resource pattern:

1. create/retain reduced work surfaces;
2. produce the reduced model input;
3. run NGX NR on the reduced domain;
4. form the NR delta against the matching reduced original/input;
5. reconstruct that delta back to the original domain;
6. add it to the original image rather than reconstructing the whole image from the reduced result.

**STATIC_OBSERVED + CONFIG_BACKED:** the restoration is intentionally change/residual based. It is not a simple bilinear upscale of the NR output.

That architecture preserves source detail outside the learned correction much better than upscaling the complete low-resolution NR image.

## 7. Ratio reconstruction

The public configuration explicitly describes the Ratio route as:

```text
luma ratio + OkLab
```

The following controls belong only to this reconstruction family:

```text
TransferStrength
ColourStrength
MaxRatio
WhitePoint
ColorIsHdr
```

The default constant block embedded in PD is:

```text
TransferStrength = 1.0
ColourStrength   = 1.0
MaxRatio         = 2.0
WhitePoint       = 1.0
```

The route therefore separates luminance/transfer reconstruction from chroma reconstruction rather than applying one RGB multiplier blindly.

The exact compiled shader arithmetic is still less directly recoverable from CPU disassembly than the surrounding resource/control contract. The strongest static statement for AIO18 is therefore the shipped formula description plus the recovered control plumbing, not a fabricated decompilation of the embedded shader.

## 8. DvaKolbas comparison: Ratio/Residual architecture is close

DvaKolbas currently implements:

- `ResolveMethod::{Auto, Residual, Ratio}`;
- the same `[0.25,1)` input-scale normalization;
- Auto -> direct at native scale, Residual below native scale;
- Lanczos3 residual reconstruction;
- luma/OkLab Ratio reconstruction;
- TransferStrength / ColourStrength / MaxRatio / WhitePoint;
- optional HDR/producer-domain handling.

That is strongly aligned with the recovered AIO18 architecture.

However, DvaKolbas includes additional experimental behavior not established by AIO18 Pass 3:

- peripheral 80/90 compression;
- fused color/guide preparation;
- producer-color range proxy logic;
- AP1-style gamut clipping details;
- its exact extended-range reconstruction equations.

Those should remain labeled as DvaKolbas-specific extensions, not AIO18 parity features.

## 9. Work-surface allocation is opportunistic and fail-soft

AIO18 does not make reduced-domain reconstruction a hard requirement.

Diagnostics and control flow prove fallback behavior:

```text
resolve scratch alloc failed -> fall back to 1:1
resolve pipeline unavailable -> fall back to 1:1
work-surface alloc failed -> fall back to one pass
```

So failure hierarchy is deliberately graceful:

```text
requested reduced reconstruction
    ↓ allocation/pipeline failure
native 1:1 NR
```

and:

```text
requested multiple NR passes
    ↓ extra work-surface failure
one NR pass
```

This is useful guidance for DvaKolbas: optional reconstruction enhancements should not convert an otherwise usable NR frame into a terminal renderer failure unless a core NGX/resource contract itself fails.

## 10. Multi-pass NR architecture

The pass-count path is now recovered.

External `+0x130` is the requested NR pass count. PD normalizes zero/negative values to one, then bounds the request against the feature instances/resources actually available.

The feature-instance walk is capped internally at **16**.

The effective sequence is:

```text
pass 1:
    original/private work input -> NR output A

pass 2:
    output A -> NR output B

pass 3:
    output B -> NR output A

...
```

AIO18 alternates two private work/output resources after the first pass rather than allocating a new full output for every frame/pass.

### Pass failure behavior

If pass N fails after earlier passes succeeded:

```text
DLSSNR: extra pass N failed, keeping K pass(es)
```

AIO18 keeps the most recent successful output and continues the frame instead of discarding the complete NR result.

**STATIC_OBSERVED:** multi-pass failure is fail-soft.

## 11. Extra feature instances are real NGX features

The backend stores multiple feature handles and has explicit create/release diagnostics:

```text
DLSSNR extra pass %u created (%d x %d)
DLSSNR: extra pass %u would not create; running %u pass(es)
DLSSNR extra pass %u released
```

This is not one NGX feature evaluated repeatedly without separate feature ownership. Additional passes can own additional NGX feature instances.

That matters for temporal history and teardown: every pass can have its own feature lifetime/history.

## 12. Pass dimensions

The primary reduced work dimensions derive from `InputResolutionScale`.

The multi-pass loop can operate over the private work surfaces, while the final resolve returns the last successful result to the requested output domain.

AIO18's configuration exposes only one global pass count and one input scale; Pass 3 does **not** find user-visible independent per-pass resolution controls analogous to DvaKolbas's more experimental second-pass configuration.

Therefore DvaKolbas's independent second-pass resolution/preset behavior should be treated as an extension rather than proven AIO18 behavior.

## 13. Final resolve occurs after the last successful NR pass

The multi-pass loop first determines the last successfully evaluated work surface. A separate final resolve/copy stage then converts that result into the output expected by the rest of AIO18.

Conceptually:

```text
work input
  -> NR pass 1
  -> optional NR pass 2..N
  -> choose last successful result
  -> final direct / Residual / Ratio resolve
  -> optional private UI composition
  -> output
```

This ordering is important: reconstruction is based on the final accepted NR pass, not necessarily pass 1.

## 14. UI correction is private post-NR composition

Pass 2 established:

```text
NGX DLSSNR.UI         = null
NGX DLSSNR.UIAlpha    = null
NGX DLSSNR.UICorrection = 0
```

Pass 3 traces what happens instead.

When external UI correction is requested and the required UI resources exist, AIO18:

1. checks/creates its own UI composition pipeline;
2. allocates a private composition scratch/output resource;
3. transitions the NR result and UI resources to read states;
4. transitions the composition target to UAV;
5. dispatches its private UI composition shader;
6. returns that private composite as the final corrected output.

Fallbacks are explicit:

```text
UI compose pipeline unavailable -> skip UI composite
UI compose scratch alloc failed -> skip UI composite
```

So UI correction failure does **not** invalidate the NR result itself.

## 15. AIO18 distinguishes UI-color and UI-alpha debug/resource roles

The outer payload carries separate UI and UIAlpha pointers. The route has separate debug overwrite branches and independently checks them.

However, they are consumed by PD's private composition/debug machinery, not handed to NGX in the core 310.8 evaluation path.

That resolves the apparent contradiction from Pass 1/2: the external ABI supports UI resources because PD itself needs them, even though the NGX parameter writer nulls them.

## 16. DvaKolbas comparison: UI behavior differs materially

DvaKolbas currently supports passing:

```text
DLSSNR.UI
DLSSNR.UIAlpha
DLSSNR.UICorrection
```

to NGX, while also having its own post-NR composition path.

AIO18 instead uses:

```text
NGX UI = null
NGX UIAlpha = null
NGX UICorrection = 0
private PD UI composition = optional
```

This is now a **high-confidence parity difference**.

It does not prove DvaKolbas is wrong; NVIDIA's runtime accepts these keys and another integration can validly use them. But if the goal is AIO18 parity and avoiding double UI treatment, DvaKolbas should test an AIO-style mode where NGX UI correction is disabled and UI is composed only once afterward.

## 17. Color-domain controls are reconstruction controls, not NGX tuning

`TransferStrength`, `ColourStrength`, `MaxRatio`, `WhitePoint`, and `ColorIsHdr` live in AIO18's external/private reconstruction contract.

They are not part of the core NGX tuning writer recovered in Pass 2.

So the architecture is:

```text
NGX NR tuning:
    Intensity
    LocalToneStrength
    LocalStructureStrength
    SkinStructureStrength
    AutoMask
    Style
    Reset
    DepthInverted

PD reconstruction tuning:
    ResolveMethod
    InputResolutionScale
    TransferStrength
    ColourStrength
    MaxRatio
    WhitePoint
    ColorIsHdr
```

Keeping those layers separate is important for a clean implementation.

## 18. Reset/history implications of multi-pass

Because extra passes are separate feature instances, history reset cannot be treated as a single global postprocess bit if multiple feature histories are active.

The loop forwards reset/evaluation state per pass and owns per-pass feature creation/release state.

Pass 4 should verify exactly how AIO18 propagates camera cuts/recreation resets to every live feature instance and what happens when pass count changes at runtime.

## 19. Strong parity points with DvaKolbas

Current DvaKolbas architecture is now supported by AIO18 static evidence in these areas:

- explicit work-resolution scaling;
- minimum scale 0.25;
- Auto/Residual/Ratio abstraction;
- direct native-scale Auto path;
- residual reconstruction of learned change;
- Ratio family with luma/OkLab concept;
- separate reconstruction tuning from NGX tuning;
- optional additional NR passes;
- private post-NR UI composition;
- graceful fallback when optional reconstruction resources are unavailable.

## 20. Important DvaKolbas differences to keep explicit

Not established as AIO18 behavior:

- peripheral-compression mapping;
- fused guide preparation;
- DvaKolbas's exact producer-color HDR proxy;
- independently sized/configured second pass;
- exact DvaKolbas Ratio gamut/extended-range math;
- feeding UI/UIAlpha/UI correction directly into NGX.

The last item is the most important compatibility question for Pass 4/runtime testing.

## 21. High-value Pass 4 targets

1. Reset propagation to all feature instances.
2. Feature recreation on input scale / pass count / preset / dimensions changes.
3. Resize and device-loss teardown order.
4. Local-vs-companion GPU fence ownership and staging lifetime.
5. Before-upscale vs after-upscale resource retirement.
6. ReShade / ENB / Community Shaders scheduling.
7. NR output -> DLSS SR / FG handoff ordering.
8. UI composition lifetime and late-overlay behavior.
9. Static leak/race audit.
10. Direct comparison against DvaKolbas lifecycle code.

## 22. Pass 3 conclusion

AIO18 NR can now be represented as:

```text
source color + depth + motion
        ↓
choose private work resolution
        ↓
optional downsample/preparation
        ↓
NGX NR pass 1
        ↓
optional NGX NR pass 2..N
   alternating private outputs
   keep last successful pass on failure
        ↓
Auto:
   native scale -> direct result
   reduced scale -> Residual
Explicit Residual:
   I + Lanczos3(NR_low - I_low)
Explicit Ratio:
   luma ratio + OkLab reconstruction
        ↓
optional PD-owned UI composition
        ↓
final NR output / following SR or Present path
```

The main remaining unknowns are no longer reconstruction architecture; they are lifecycle, synchronization, integration ordering, and runtime confirmation.
