# AIO18 DLSS Neural Rendering / Neural Reconstruction RE — Pass 5 Review

**Target:** `SkyrimUpscalerAIOBuild18-Hotfix1(1).7z`  
**Target archive SHA-256:** `136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8`  
**Pass type:** cross-pass static verification and implementation review; **no AIO18/Skyrim/GPU execution**.  
**Purpose:** reconcile Passes 1–4, retain corrections, re-check DvaKolbas at its current head, and define the remaining runtime-only validation boundary.

## 1. Result

The four static RE passes form a consistent model. Pass 5 found **no contradiction that invalidates the recovered architecture**.

The strongest conclusions remain:

- AIO18 exposes a `0x138` host→PD NR evaluation ABI, transformed by PD into a separate `0x110` internal NGX block.
- Default scheduling is `scene → SR → late NR → FG/presentation`; the alternate before-upscale route is `scene/guides → NR → SR`.
- The core AIO18 NGX path writes `DLSSNR.UI = null`, `DLSSNR.UIAlpha = null`, and `DLSSNR.UICorrection = 0`; UI correction/composition is private PD work after NR.
- Auto/Residual/Ratio and input-scale behavior recovered in Pass 3 remain consistent with Pass-2 payload semantics and Pass-4 lifecycle behavior.
- AIO18's common primary NR reset is zero; extra-pass reset bytes are first-use latches in the mapped region.
- AIO18 has layered local/companion-GPU staging, private reconstruction, a bounded feature pool, and separate lightweight vs full-device recovery.
- No mapped static evidence proves a core NR use-after-free. `ReleaseFeature` failure followed by pointer clearing remains a cleanup robustness concern that needs runtime evidence.

## 2. Current DvaKolbas review identity

DvaKolbas is now at:

```text
wallhead/DvaKolbas
branch: codex/fsr-sr
head: f6a29f75db0029acf013227627a52e2e490733c6
```

The six NR implementation blobs checked in Pass 4 are byte-identical Git blobs at the current head, so the Pass-4 NR comparison remains current. Changes after `78d6e4bd...` are FG validation/documentation work, not NR source changes.

## 3. Corrections retained across the series

1. External `+0xE0/+0xE4` are **MVecScaleX/Y**, not jitter.
2. The `0x138` external host payload is not the NGX parameter structure; PD creates a separate `0x110` internal block.
3. Presence of `DLSSNR.UI`, `UIAlpha`, and `UICorrection` strings does not mean AIO18's core path uses them. The mapped core path nulls UI/UIAlpha and forces UICorrection zero.
4. Auto is a selector, not a third reconstruction shader: direct at native scale, Residual below native scale.
5. Multi-pass is separate NGX feature ownership with sequential evaluation, not one handle blindly evaluated N times.

No later pass reverses those corrections.

## 4. Final DvaKolbas parity assessment

### Keep — stronger than literal AIO18 behavior

Do **not** regress these to AIO18 parity:

- camera/history reset propagation;
- pass-2 reset inheritance;
- interop drain before feature destruction;
- retaining ownership after failed/unsubmitted NR command recording.

These are easier to reason about and safer than the static AIO18 behavior recovered here.

### A/B test — high-confidence parity mismatch

AIO18 core:

```text
DLSSNR.UI           = null
DLSSNR.UIAlpha      = null
DLSSNR.UICorrection = 0
private UI composition after NR = yes
```

Current DvaKolbas:

```text
DLSSNR.UI           = a_input.ui
DLSSNR.UIAlpha      = a_input.uiAlpha
DLSSNR.UICorrection = tuning.uiCorrection (default false, but user-controllable)
private UI composition after NR = yes
```

This is the highest-value parity experiment. It is **not yet a proven DvaKolbas bug**. Test the AIO-style NGX contract while holding all private composition/reconstruction behavior constant.

Suggested comparisons:

- fine HUD/text edges;
- translucent UI halos;
- world pixels immediately behind UI;
- UI motion trails during camera motion;
- menu enter/exit history;
- generated frames when FG is active;
- performance/GPU time.

### Keep labeled as DvaKolbas extensions

The static AIO18 work does not establish parity for:

- peripheral 80/90 compression;
- fused color/guide preparation;
- producer-color HDR proxy;
- independently sized/configured second pass;
- DvaKolbas-specific extended-range/AP1/gamut reconstruction.

They may be useful features, but should not be justified as copied AIO18 behavior.

## 5. Static race/lifetime review

### AIO18

No equally concrete NR resource leak/use-after-free was found in the mapped core. Release is layered and invokes synchronization/ownership helpers before dropping feature/resource ownership. Static analysis still cannot prove the exact fence-completion contract.

The remaining cleanup concern is:

```text
ReleaseFeature(handle)
    ↓
possible failure diagnostic
    ↓
feature pointer cleared anyway
```

This may be acceptable under device-loss semantics, or it may abandon vendor ownership. Only a live device-loss/recreate trace can decide.

### DvaKolbas

The current retirement model is clearer:

```text
stop session
signal final D3D11 work
Interop::Drain()
release guide users
retire telemetry
reset NeuralPass / NGX feature ownership
```

The backend also intentionally refuses to destroy a failed, unsubmitted NR recording. Pass 5 found no reason to replace that model with AIO18's less explicit static lifetime behavior.

## 6. Runtime-only boundary

Static RE is now saturated enough that another broad disassembly pass has low value. The remaining questions require runtime observation:

1. AIO18 `Reset` under actual camera cut/load/teleport.
2. Exact PD release fence/event values immediately before `ReleaseFeature`.
3. Actual resources, formats, extents and D3D12 states at each active NR scheduling site.
4. Real local-vs-companion-GPU selection and cross-adapter copy timing.
5. UI-null/UICorrection=0 A/B against DvaKolbas current path.
6. CS/ENB/ReShade branch selection in representative configurations.
7. Feature handle and staging ownership through real resize/device-loss recovery.

## 7. Recommended runtime breakpoints

For the exact AIO18 hashes:

```text
SkyrimUpscaler +0x2A2300  build external evaluate payload
SkyrimUpscaler +0x2A25E0  EvaluateDLSSNR call
SkyrimUpscaler +0x294E24  before-upscale NR site
SkyrimUpscaler +0x2AB0CF  normal late NR site
SkyrimUpscaler +0x2AB349  following FSR-FG producer

PDPerfPlugin +0x9AF70      internal NGX parameter writer
PDPerfPlugin +0x9F3A2      extra-pass reset read
PDPerfPlugin +0x9F408      extra-pass reset clear
PDPerfPlugin +0xA1770      pre-release synchronization helper
PDPerfPlugin +0xA18DC      extra-feature ReleaseFeature
PDPerfPlugin +0xA1C4F      primary ReleaseFeature
PDPerfPlugin +0xA185B      full device-recreate shutdown
PDPerfPlugin +0xA1878      full device-recreate reinit

nvngx_dlssnr +0x159C0      NVSDK_NGX_D3D12_EvaluateFeature
nvngx_dlssnr +0x16040      NVSDK_NGX_D3D12_ReleaseFeature
```

Capture the external `0x138` block, internal `0x110` block, feature handle, command queue/list, all resource pointers/descriptions, and the relevant fence completed/current values at these points.

## 8. Pass-5 conclusion

The five-pass review leaves a stable implementation model and a small runtime validation surface. The main action for DvaKolbas is **not** to copy AIO18 wholesale. Preserve the stronger reset/retirement behavior, use AIO18 as the reference for resource/scheduling/reconstruction semantics, and experimentally isolate the NGX UI-correction difference before changing it.
