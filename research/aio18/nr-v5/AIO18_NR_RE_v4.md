# AIO18 DLSS Neural Rendering / Neural Reconstruction RE — Pass 4

**Target:** `SkyrimUpscalerAIOBuild18-Hotfix1(1).7z`  
**Archive SHA-256:** `136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8`  
**Analysis:** static reverse engineering of the exact AIO18 binaries. No Skyrim/GPU execution in this pass.  
**Goal:** close the planned static NR work by tracing history/reset behavior, feature recreation, resize/device-recovery paths, local/companion-GPU resource lifetime, release ordering, NR→SR/FG integration, and direct lifecycle comparison with DvaKolbas.

## 1. Status after Pass 4

The planned **4 / 4 static NR passes are complete** for this exact AIO18 build.

The remaining high-value work is **Pass 5 — runtime validation**. It should confirm live branch selection, actual NGX parameter values, resource dimensions/states, camera-cut behavior, feature-release/retirement ordering, and the AIO18-vs-DvaKolbas UI-correction difference.

The static coverage is now implementation-grade for architecture and code review, but runtime behavior should not be inferred where this report explicitly marks a finding as a concern or unresolved synchronization contract.

## 2. Exact binary identities

- `SkyrimUpscaler.dll` SHA-256 `5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81`
- `PDPerfPlugin.dll` SHA-256 `53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1`
- `nvngx_dlssnr.dll` SHA-256 `8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206`

Every RVA in this report is specific to those exact binaries.

## 3. Primary NR history reset is not propagated in the common AIO18 producer

Pass 2 mapped external payload `+0x100` as `DLSSNR.Reset` after PD converts the 0x138-byte host payload into the internal NGX parameter block.

Pass 4 traces its producer.

At the common host evaluator:

```text
SkyrimUpscaler +0x2A23BB
    xor r12d,r12d

...

SkyrimUpscaler +0x2A24E5
    mov byte ptr [rbp+0x20],r12b
```

At this point `[rbp+0x20]` is external evaluate payload `+0x100`, therefore:

```text
common primary Reset = 0
```

No alternative write to that stack field occurs in the mapped common producer before `EvaluateDLSSNR`.

Nearby fields are independently populated:

```text
external +0x101 = DepthInverted, sourced from host +0x2FC
external +0x102 = Enabled, sourced from host +0x2A8
```

**STATIC_OBSERVED:** the common AIO18 primary NR evaluation does not forward an explicit camera-cut/reset pulse through `DLSSNR.Reset`.

### Qualification

This does **not** prove that AIO18 never clears NR temporal state. Feature recreation, device recreation, changed work dimensions, and other backend rebuilds can destroy/recreate history indirectly. But a normal camera-cut-style reset is not visible in this common per-evaluation field.

### Implementation implication

Do **not** copy this behavior into DvaKolbas merely for parity. DvaKolbas already has an explicit history/reset policy, and retaining it is safer.

## 4. Extra NR passes have per-feature first-use reset latches

The multi-pass loop has a different reset mechanism for pass 2+.

Relevant code:

```text
PD +0x9F37A
    first pass uses external primary Reset at +0x100

PD +0x9F3A2
    load current extra-feature reset byte

PD +0x9F408
    clear that reset byte after successful evaluation
```

Feature creation sets the per-feature byte:

```text
PD +0xA1B3B
    resetLatch[index] = 1
```

Feature release clears it:

```text
PD +0xA193A
    resetLatch[index] = 0
```

The latch array begins at backend `+0xC8` and is indexed alongside feature slots.

**STATIC_OBSERVED:** an extra NR feature receives a reset on first use after creation. After a successful evaluation the latch is cleared.

Within the mapped feature-pool/evaluation region, no second writer was found that re-arms these per-pass reset bytes for ordinary camera cuts.

This produces a split history model:

```text
primary feature:
    external Reset = 0 in common host path

extra feature:
    Reset = 1 when newly created
    Reset = 0 after first successful evaluation
```

Again, that is an observation of AIO18, not a recommended design.

## 5. Feature pool and pass-count changes

Pass 3 established up to 16 feature slots. Pass 4 maps the lifecycle more precisely.

### Release one feature

`PD+0xA18B0` owns single-slot release. It:

1. identifies the indexed feature;
2. calls the dynamically resolved NGX `ReleaseFeature` path;
3. emits success/failure diagnostics;
4. clears the feature pointer;
5. clears its per-feature reset latch.

Landmarks:

```text
+0xA18DC  ReleaseFeature call
+0xA1931  feature pointer cleared
+0xA193A  reset latch cleared
```

### Reconcile feature count

`PD+0xA19E0` adjusts the live pool to the desired count:

```text
desired < current:
    release excess feature slots

desired > current:
    create missing feature slots
    mark each new slot reset=1

maximum observed slots = 16
```

### Work-size rebuild

`PD+0x9C870` is a higher-level rebuild path for work-size/scale changes. It coordinates feature-pool ownership, recomputes effective work dimensions and creates the necessary feature at the new work size.

**STATIC_OBSERVED:** changing the private NR work domain is a feature-lifecycle operation. It is not implemented purely as a different per-frame subrect over one permanent feature.

This agrees with the Pass-3 architecture where `InputResolutionScale` selects a private model domain.

## 6. AIO18 has two recovery levels

### 6.1 Lightweight command-list / swapchain-resource recovery

Feature creation contains a recoverable command-list path with the diagnostic:

```text
DLSSNR: BeginCommandList failed
(lost=%d device=%p queue=%p list=%p fence=%p),
recreating SwapChain resources
```

It attempts to recreate the relevant command-list/swapchain-side resources and retry.

A second diagnostic marks failure after retry:

```text
DLSSNR: BeginCommandList still failed while creating feature
```

This is a narrower recovery path; it does not necessarily rebuild the entire NR backend.

### 6.2 Full device-recreate recovery

`PD+0xA1830` handles a broader device recreation.

The observed sequence is approximately:

```text
snapshot cached initialization descriptor/state
        ↓
PD +0xA185B -> full backend shutdown +0xA1E40
        ↓
clear old device/context ownership fields
        ↓
PD +0xA1878 -> backend initialization +0x9C9A0
        ↓
log success/failure
```

Diagnostic:

```text
DLSSNR: ReinitAfterDeviceRecreate %s
```

**STATIC_OBSERVED:** device recreation is treated as a complete NR backend lifecycle event, not just a new output texture.

## 7. Release path is layered

The public host-side release flow remains:

```text
PD ReleaseDLSSNR export +0xFA240
    ↓
UpscaleAPI_D3D11 release router +0xF4E30
    ↓
backend registry release +0xA1D20
    ↓
backend record release +0xA1BB0
    ↓
feature/resource cleanup
```

The router at `+0xF4E30` first invokes broader interop/global cleanup helpers before dispatching to the registered NR backend.

The backend record cleanup at `+0xA1BB0`:

- calls a broader synchronization/ownership helper at `+0xA1770`;
- releases extra feature slots;
- releases the primary feature;
- removes the backend registry entry.

The full backend shutdown at `+0xA1E40` additionally walks and destroys broad backend state, COM/resource containers and NGX parameter ownership. The mapped `NVSDK_NGX_D3D12_DestroyParameters` call is at:

```text
PD +0xA2466
```

and the shutdown path also reaches NGX shutdown handling before clearing initialization/device state.

### Important limitation

**STATIC_UNRESOLVED:** the exact GPU-retirement semantics of helper `+0xA1770` and the surrounding global/interop helpers are not completely reconstructed from static code alone.

The order strongly suggests deliberate synchronization before feature/resource destruction, but this pass does **not** claim a proven fence-completion contract equivalent to a runtime-observed queue drain.

Pass 5 should instrument this path before we declare release synchronization fully understood.

## 8. ReleaseFeature failure is a cleanup robustness concern

The extra-feature release path calls NGX ReleaseFeature at `PD+0xA18DC` and logs the result.

The feature pointer and its reset latch are then cleared at:

```text
PD +0xA1931
PD +0xA193A
```

regardless of the logged result.

The primary backend-release path shows the same broad policy: attempt ReleaseFeature, log failure if necessary, then continue dropping backend registry ownership.

**STATIC_CONCERN:** if a failed `NVSDK_NGX_D3D12_ReleaseFeature` means the caller still owns a live vendor handle, AIO18 has discarded the pointer needed for a retry.

This is **not a proven leak**. During device loss the vendor may intentionally consider the handle unrecoverable/invalid even when the API returns failure. Runtime/device-loss testing is required before assigning a visible bug.

For a clean implementation, DvaKolbas's existing “retain ownership when retirement is unconfirmed” philosophy is preferable.

## 9. Local/companion-GPU evaluation requires committed staging

Pass 1 established local-game-GPU and companion-GPU modes. Pass 4 confirms that their lifetime contract is substantially stricter than pointer sharing.

The mapped Present evaluator contains diagnostics including:

```text
DLSSNR: skip local eval, depth=%p motion=%p
(need committed GPU1 staging, not NT-shared)

DLSSNR: failed to copy NT-shared backbuffer to local staging

DLSSNR: skip present eval, committed color/output missing
```

The immediate path has analogous missing-staging diagnostics.

**STATIC_OBSERVED:** AIO18 does not treat NT-shared resources as sufficient for every NR input on the execution GPU.

In particular:

- depth and motion require committed execution-device staging in the mapped local/companion route;
- Present backbuffer data can be copied from an NT-shared resource into committed local staging;
- color/output committed staging must exist before the local evaluation can run.

This is important if DvaKolbas ever implements companion-GPU NR. Sharing handles alone is not AIO18 parity.

## 10. Before-upscale order is exact

The before-SR branch has the sequence:

```text
SkyrimUpscaler +0x294E24
    -> common NR evaluator +0x2A2300

...

SkyrimUpscaler +0x294FBD
    -> PDPerfPlugin!EvaluateUpscaler
```

Therefore, on this exact branch:

```text
scene + guides
    ↓
NR
    ↓
SR/upscaler
```

**STATIC_OBSERVED:** `mDLSSNRBeforeUpscaling=true` really means NR runs before the following super-resolution evaluation; this is not merely an INI label.

## 11. Normal late order is SR → NR → FSR FG preparation

The earlier AIO18 FSR RE established the normal pre-UI SR invocation around:

```text
SkyrimUpscaler +0x19FA14
```

In the normal late path mapped here:

```text
SkyrimUpscaler +0x2AB0CF
    -> common NR evaluator +0x2A2300

SkyrimUpscaler +0x2AB349
    -> FSR FG input/prepare producer +0x2941A0
```

So the normal after-upscale branch is:

```text
scene
    ↓
SR
    ↓
late NR
    ↓
FG preparation / later presentation
```

This establishes a real integration relationship: the frame-generation/presentation side can consume a scene that has already passed through NR.

### Qualification

AIO18 also has specialized DLSS-G/FrameWarp/deferred branches recovered during the FSR work. Do not generalize this one local sequence into a claim that every presentation branch has exactly the same final callback/overlay order.

## 12. Community Shaders NR bridge

The specialized call at:

```text
SkyrimUpscaler +0x265B0A
```

sets both byte-mode arguments true immediately before entering the common NR evaluator:

```text
+0x265AF9  r9b = 1
+0x265AFC  r8d = zero_extend(r9b) = 1
+0x265B0A  call common NR evaluator
```

The selected resource wrapper comes from host `+0xAF8`.

**STATIC_OBSERVED:** Community-Shaders/live-postprocessing integration does not bypass NR's common host ABI; it supplies a specialized resource/mode combination to it.

The exact semantic identity of host `+0xAF8` remains renderer-integration detail rather than something this pass renames by guess.

## 13. ENB and ReShade ordering

Pass 4 did not uncover a new NR-specific named ENB render callback.

The broader AIO18 static evidence from the FSR passes remains the best description:

- named ENB SDK probing is narrow (`ENBGetSDKVersion`); much of the ordering comes through the D3D11 proxy/interposition chain;
- ReShade has explicit begin-effects / finish-effects routing gates;
- `reshade_present` exists for final/deferred presentation work;
- StatsMenu contains a nested scene/UI exception.

For NR, the new strong statement is limited to the host-local orders proven above:

```text
before-SR mode: NR -> SR
normal late mode: SR -> NR -> FSR-FG prepare
```

Exact ENB visual order and all final ReShade-overlay branches remain runtime targets.

## 14. UI ownership ends outside the core NGX NR evaluation

Pass 2 recovered the core AIO18 NGX call contract:

```text
DLSSNR.UI           = null
DLSSNR.UIAlpha      = null
DLSSNR.UICorrection = 0
```

Pass 3 recovered AIO18's private post-NR UI composition.

Pass 4's lifecycle implication is important:

```text
NGX NR evaluation lifetime:
    scene/depth/motion/output/backbuffer-side resources

PD private UI lifetime:
    continues after final NR result
    through private UI composition
```

So in the mapped core path, the UI texture is not an NGX-reader lifetime problem. It is a PD private-compositor lifetime problem.

This separation is a useful implementation pattern: do not unnecessarily retain UI for NGX if private UI composition is the chosen architecture.

## 15. No equally concrete NR COM-reference leak found in the mapped core

The earlier FSR/UI work found a specific unbalanced D3D11 `GetResource` reference in a UI-depth hook.

Pass 4's mapped NR core does **not** reveal an equally direct AddRef/GetResource-without-Release defect.

That is not proof that the full NR backend is leak-free. It only means this pass found no similarly concrete reference imbalance in the analyzed NR init/evaluate/rebuild/release paths.

## 16. DvaKolbas comparison uses current head

Current repository snapshot reviewed for this comparison:

```text
wallhead/DvaKolbas
branch: codex/fsr-sr
commit: 78d6e4bd9db2146104a7485d46174684b954c3f1
```

### 16.1 DvaKolbas retirement is clearer/safer than literal AIO18 static evidence

Current `Backend::Quiesce()` performs:

```text
disable generation
stop the presentation/session path
signal final D3D11 work
Interop::Drain()
release guide ownership
only then destroy/reset NeuralPass
invalidate NR history
```

Its source comments explicitly require destroying `NeuralPass` only after interop retirement.

This gives DvaKolbas a concrete, inspectable GPU-retirement condition before NGX feature destruction.

AIO18 likely has corresponding synchronization hidden behind `+0xA1770`/global helpers, but Pass 4 cannot prove an equivalent fence contract statically.

**RECOMMENDATION:** preserve the DvaKolbas drain-before-destruction design.

### 16.2 DvaKolbas reset handling is stronger

Current DvaKolbas `NeuralHistory::ResetFor()` resets on:

- camera reset;
- activation/inactivation transitions;
- image-affecting option changes.

Its second-pass logic additionally computes:

```text
second.reset = first.reset
            || secondHistoryInvalid
            || secondFeature has never evaluated
```

This is materially stronger than the AIO18 common path's primary `Reset=0` plus first-use extra-pass latches.

**RECOMMENDATION:** do not regress DvaKolbas reset behavior to match AIO18.

## 17. The most important remaining AIO18-vs-DvaKolbas parity mismatch is UI correction

At current DvaKolbas head, `FeatureSession::RecordEvaluation()` writes:

```text
DLSSNR.UI      = a_input.ui
DLSSNR.UIAlpha = a_input.uiAlpha
```

and its reconstruction-runtime tuning writer can write:

```text
DLSSNR.UICorrection = 1
```

DvaKolbas also performs its own private post-NR UI composition.

AIO18's mapped core does instead:

```text
DLSSNR.UI      = null
DLSSNR.UIAlpha = null
DLSSNR.UICorrection = 0

then private UI composition after NR
```

This is now a **HIGH-CONFIDENCE PARITY MISMATCH**.

It is **not yet a proven DvaKolbas bug**. NVIDIA's runtime can legitimately support its UI parameters, and DvaKolbas may intentionally exploit them.

But it creates a plausible double-treatment difference:

```text
DvaKolbas current:
    NGX can be UI-aware
    + Dva private UI composition

AIO18:
    NGX scene-only
    + one private UI composition stage
```

### Recommended next action

Do not patch blindly.

Pass 5 should A/B the same scene/UI workload with:

```text
A: current DvaKolbas NGX UI/UIAlpha/UICorrection behavior
B: AIO-style UI=null, UIAlpha=null, UICorrection=0
```

while keeping the same private post-NR UI composition in both cases.

Compare:

- text/HUD edge stability;
- transparent UI halos;
- world pixels immediately behind UI;
- UI motion trails;
- history behavior when menus appear/disappear;
- exact generated/real-frame output if FG is also active.

Only after that A/B should Codex change the production contract.

## 18. DvaKolbas differences that remain extensions, not AIO parity

Passes 1–4 still do not establish AIO18 equivalents for DvaKolbas's:

- peripheral 80/90 compression;
- fused color/guide preparation;
- producer-color HDR proxy;
- independently sized/configured second NR pass;
- exact DvaKolbas extended-range/AP1-style reconstruction details.

These can be useful features. They should simply stay labeled as DvaKolbas-specific rather than “copied from AIO18.”

## 19. Static concern ranking

### AIO18 concerns

**Medium — primary/reset propagation**  
The common primary evaluate path supplies Reset=0; extra passes reset primarily on feature creation. This could make camera discontinuities more history-sensitive. Runtime image evidence is required.

**Medium/Low — ReleaseFeature failure ownership**  
AIO18 discards feature pointers after attempted release even when release diagnostics indicate failure. Potential retained vendor ownership is unproven, especially under device loss.

**Unresolved — exact retirement primitive**  
The release path clearly invokes synchronization/ownership helpers before destruction, but their exact GPU completion semantics remain a Pass-5 runtime target.

### DvaKolbas concerns

**High-value parity test — UI correction contract**  
Current Dva can combine NGX UI-aware inputs/correction with its own private UI composition, unlike AIO18.

**Keep as-is — reset and retirement**  
DvaKolbas's explicit reset propagation and drain-before-destruction behavior are stronger and should not be weakened for binary parity.

## 20. Recommended Pass 5 breakpoints / runtime captures

For the exact AIO18 hashes:

```text
SkyrimUpscaler +0x2A24E5  primary Reset store
SkyrimUpscaler +0x294E24  before-SR NR evaluation
SkyrimUpscaler +0x294FBD  following SR evaluation
SkyrimUpscaler +0x2AB0CF  normal late NR evaluation
SkyrimUpscaler +0x2AB349  following FSR-FG producer
SkyrimUpscaler +0x265B0A  CS/live NR bridge

PDPerfPlugin +0x9F3A2     extra-pass reset latch load
PDPerfPlugin +0x9F408     reset-latch clear after success
PDPerfPlugin +0xA18B0     one-feature release path
PDPerfPlugin +0xA19E0     feature-pool reconciliation
PDPerfPlugin +0xA1830     full device-recreate path
PDPerfPlugin +0xA1BB0     backend record release
PDPerfPlugin +0xA1E40     full backend shutdown
PDPerfPlugin +0xA12E0     local/companion Present staging region
PDPerfPlugin +0x9AF70     NGX parameter writer

nvngx_dlssnr +0x159C0     NVSDK_NGX_D3D12_EvaluateFeature
nvngx_dlssnr +0x16040     NVSDK_NGX_D3D12_ReleaseFeature
```

At runtime capture:

1. actual primary/extra-pass Reset values across a forced camera cut;
2. feature handles before/after camera cut, scale change, pass-count change and resize;
3. queue/fence values entering `+0xA1770` and at ReleaseFeature;
4. whether release failure can be induced on device removal and what NGX owns afterward;
5. game-GPU vs companion-GPU resource pointers, heaps and staging resources;
6. exact SR→NR→FG order on normal gameplay, menus and ReShade overlay frames;
7. UI/UIAlpha/UICorrection NGX values and final private composition;
8. all resource states immediately around EvaluateFeature.

## 21. Four-pass static architecture summary

AIO18 NR can now be summarized as:

```text
Skyrim / CS renderer integration
        ↓
0x138 host NR payload
        ↓
PD route / D3D11↔D3D12 / optional companion staging
        ↓
private work-domain selection
        ↓
NGX feature(s), up to 16 owned slots
        ↓
pass 1 + optional sequential extra passes
        ↓
last successful NR result
        ↓
Direct / Residual / Ratio reconstruction
        ↓
PD-private UI composition
        ↓
following SR (before-SR mode)
OR presentation / FG path (default late mode)
```

Lifecycle is correspondingly layered:

```text
per-frame reset state
feature-slot reset/history state
work-size feature rebuild
command-list/swapchain-resource recovery
device-wide backend reinitialization
backend registry release/full shutdown
```

## 22. Pass 4 conclusion

The static investigation is now broad enough to use AIO18 as an implementation reference without treating it as a black box.

The most important conclusions for DvaKolbas are:

1. **Keep DvaKolbas's stronger reset propagation.** Literal AIO18 reset parity would be a regression in defensive temporal handling.
2. **Keep drain-before-feature-destruction ownership.** Static AIO18 release synchronization exists but its exact fence semantics are not sufficiently proven to justify weakening Dva's explicit retirement contract.
3. **Do not add companion-GPU NR by sharing NT handles alone.** AIO18 requires committed execution-device staging for key resources.
4. **Preserve the recovered ordering.** Before-SR mode is NR→SR; the normal late path is SR→NR→FG preparation.
5. **Treat NGX UI correction as the main parity experiment.** AIO18 deliberately keeps UI/UIAlpha/UICorrection out of the core NGX evaluation and composes UI privately afterward; current DvaKolbas can do both.
6. **Do not copy AIO18's questionable cleanup/reset details merely for parity.** They are reference behavior, not automatically best practice.

Pass 5 should now be a targeted runtime verification exercise rather than another broad static disassembly sweep.
