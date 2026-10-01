# Skyrim Upscaler AIO18 Hotfix1 — FSR SR and frame-generation reverse engineering

**Analysis date:** 1 October 2026  
**Revision:** 5 — temporal-guide provenance, Community Shaders bridge, mask/exposure policy, shared SR/FG inputs  
**Exact input:** `SkyrimUpscalerAIOBuild18-Hotfix1(1).7z`  
**Scope:** fresh, read-only static analysis of the supplied archive. No game, Windows DLL, or GPU workload was executed. No binary was patched.

> **New in revision 5:** sections 32–36 trace the Community Shaders live-postprocessing input bridge, shader-based depth/motion capture, generic fallback discovery, the normal null reactive/T&C policy, FSR auto-exposure flags, and the shared prepared depth/motion resources consumed by both SR and FG. Reset-writer coverage is surveyed without inventing camera-cut attribution. Start with section 32 and `verification_v5.json`.

> **New in revision 4:** sections 28–31 map ReShade begin/finish/present bracketing, show the viewport-without-general-scissor hook set, trace StatsMenu's nested scene/UI transition, and narrow named ENB integration to a presence probe.

> **New in revision 3:** sections 22–27 add jitter/motion/depth producer traces, depth-convention reconfiguration, independent FG reset acknowledgment, viewport/depth-SRV hooks and a static COM reference leak. **Section 10.1 is corrected: DX12 FSR SR payload +0x20 is reactive, not output.** All earlier evidence is retained. Start with section 22 and verification_v3.json.

> **New in revision 2:** sections 16–21 map the actual pre-UI SR call, ReShade's guarded execution and conditional final-overlay submission, persistent UI render-target redirection, shared API fences, AMD internal resource lifetimes, and the resize/drain path. All first-pass evidence is retained. Start with **section 16** for the new results, and **verification_v2.json** for the new checks.

## 1. Results at a glance

This package contains **two distinct FSR super-resolution integrations and a separate, D3D12-based FSR frame-generation integration for Skyrim's D3D11 renderer**.

- The DX11 SR route loads the bundled `ffx_fsr3_x64.dll`, which calls `ffx_fsr3upscaler_x64.dll`. The latter's effect-version export returns **3.1.2**.
- The FSR4-labelled SR route uses `UpscaleMethod_DX11WrapperForDX12`, a companion D3D12 path, and the FidelityFX API loader. Its packaged upscaler DLL reports product version **4.0.2.0**.
- FSR FG uses `FrameGenMethod_DX11WrapperForDX12` and `FrameGenMethod_FSR3_FFXAPI`. Skyrim's per-frame evaluation prepares depth/motion inputs and configures FG/UI. The **actual interpolation dispatch is called back from the AMD swapchain's Present path**.
- The packaged AMD FG DLL has both a classical FSR3 provider and an ML provider. “Using FSR4 Frame Generation” is selected by module-availability checks in the host backend; that message alone does **not** identify the selected AMD provider.
- Two concrete ML-integration concerns are visible: the inspected PrepareV2 descriptor leaves all four camera vectors zero, and the per-frame method submits Prepare before its same-frame Configure. These are static findings requiring runtime validation, not proof that classical FSR3 FG fails.
- UI texture and HUD-less detection are separate paths. The UI texture registration explicitly requests internal double-buffering and optionally premultiplied alpha.

**Important default:** the shipped INI selects **DLSS SR + DLSS FG**, not either FSR route. The findings below reconstruct code that is available when those FSR options are selected; they are not a trace of the shipped defaults running.

### Evidence labels

**B** = observed in this archive's bytes, disassembly, RTTI, imports, or matching PDB.  
**S** = external primary documentation, identified in section 15.  
**I** = interpretation/integration implication based on B or S.  
**U** = unresolved without further static work or a runtime trace.

Relative `evidence/` paths refer to the accompanying ZIP. Module addresses are **RVAs**, not file offsets, Skyrim engine addresses, or Address Library IDs. All three principal images have preferred image base `0x180000000`; runtime addresses are `actual_loaded_module_base + RVA`.

## 2. Exact input identities

Archive: **204,178,443 bytes**; **25 regular files**.

```text
Archive SHA-256
136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8

SkyrimUpscaler.dll
5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81

PDPerfPlugin.dll
53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1

amd_fidelityfx_framegeneration_dx12.dll
3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347

amd_fidelityfx_loader_dx12.dll
2f36843c3bb8c059621c10574e586a883ef337f2a549c67ecf3a82b3959ac238

amd_fidelityfx_upscaler_dx12.dll
2604c0b392072d715b400b2f89434274de31995a4b6e68ce38250ebbd3f6c5fc

ffx_fsr3_x64.dll
2734c4ffa563f675df9ccb33c6cd6af60ca014e0c7d3eea284816befa83bd12a

ffx_fsr3upscaler_x64.dll
dbd5e7a1a6cb3074617c8e82c1d12ce49721a013ab382385d5b4657edda7f62d

ffx_backend_dx11_x64.dll
d6005576edeb783b0e109aa160ffe06671cc801b84b7bf841cb521ef4b053c5f
```

The host and PD backend hashes differ from the earlier AIO16 evidence. **Do not carry AIO16 offsets into this build.** The file name AIO18 is package provenance; the host's PE version resource is `1.0.2.0`, not an independent AIO18 version certificate.

### AMD symbol identities

| Component | PE file version / product version | Matching PDB GUID | Age |
|---|---|---|---:|
| AMD frame generation | `4.0.0.604` / `4.0.0.0` | `f977ea5e-eaed-467f-800d-e437ac3b9e74` | 1 |
| AMD loader | `2.1.0.604` / `2.1.0.0` | `f782f3f7-029d-4dc9-a3b7-c0a71dcac9da` | 1 |
| AMD upscaler | `4.0.2.0` / `4.0.2.0` | No supplied PDB used | — |
| Legacy AMD combined DX12 library | `1.0.1.0` / `1.0.1.0` | No supplied PDB used | — |

The parser extracted 13,871 address-bearing FG symbol records and 324 loader records, including duplicates, data, and templates; these are **not counts of distinct functions**. No matching SkyrimUpscaler or PDPerfPlugin PDB was supplied. PD class names are grounded in RTTI and strings, with roles recovered from dataflow. [B: `manifest.json`, `pe_inventory.json`, `amd_fg.identity.json`, `amd_loader.identity.json`]

## 3. Configuration and backend separation

Relevant shipped values and comments:

```ini
[Settings]
# 0 DLSS; 1 FSR2 deprecated; 2 XeSS; 3 FSR3.1 DX11; 4 FSR4 DX12 interop
mUpscaleType = 0
mAutoExposure = true
mQualityLevel = 2
mRenderReShadeBeforeUpscaling = true
mSharpening = true
mSharpness = 0.600000
mFOV = 90.000000
mTargetResolutionScale = 1.000000

# Values below appear in their respective original INI sections.
mEnableFrameGeneration = true
# 0 None; 1 FSR3 FG; 2 DLSS FG; 3 XeSS FG
mFrameGenType = 2
mFGFrames = 2
mEnableHUDFix = true
# 0 UI Texture; 1 Hudless Detection
mHUDFixMethod = 0
mPDFrameWarpMode = 0
mEnableDLSSNR = false
```

This is an excerpt, **not a replacement INI**. SR and FG have independent selectors and separate evaluation exports. The SR factory at PD `0xF5170` maps deprecated method `1` to method `3`; method `4` goes through the DX11-to-DX12 SR wrapper. That wrapper's backend factory at `0xCFA50` creates the FFXAPI SR backend at callsite `0xCFB5D` when the required API path is available. [B: shipped INI; `pd_dx11_sr_factory.asm`, `pd_dx11_sr_bridge_create_backend.asm`]

## 4. Skyrim-facing handoff

The host imports PD functions through a **delay-import table**. Looking only at normal PE imports misses this API boundary.

| Role | SkyrimUpscaler callsite RVA | Delay-IAT RVA | PD export RVA |
|---|---:|---:|---:|
| Evaluate super resolution | `0x294FBD` | `0x472C20` | `0xF8D20` |
| Evaluate frame generation | `0x2948E0` | `0x472C58` | `0xF8DB0` |
| Initialize upscaler | Multiple | `0x472C08` | `0xF8BE0` |
| Set FG parameters | Multiple | `0x472BF0` | See PE export inventory |
| Enable/select FG | Multiple | `0x472C68` | See PE export inventory |
| Create swapchain proxy | Multiple | `0x472CC0` | `0xF8F50` |

The SR-producing host function is `0x2949C0`; its diagnostic string identifies `SkyrimUpscaler::EvaluateUpscaling(ImageWrapper*, ImageWrapper*)`. The FG-producing function starts at `0x2941A0`.

A larger host function at `0x2AA8A0` identifies itself in diagnostics as **`BeforePresent()`**. It contains SR callsite `0x2AAE18` and FG callsites `0x2AB349` and `0x2ABD02`, on different branches. There are also other SR and FG callers. **The address order inside this branched function is not a proof of one universal ENB/ReShade/UI ordering.** [B: `host_before_present.asm`, `host_evaluate_sr.asm`, `host_evaluate_fg.asm`]

Both PD evaluation exports copy **0xB0 bytes** from the incoming payload, then invoke different `UpscaleAPI_D3D11` virtual slots:

```text
PD EvaluateUpscaler       +0xF8D20 -> API slot +0x28 -> +0xF5CF0
PD EvaluateFrameGeneration+0xF8DB0 -> API slot +0x40 -> +0xF6120
```

This is a recovered **PD wrapper copy extent**, not the size of an AMD SDK structure or a public ABI guarantee. [B: export disassembly; API RTTI/vtable records]

## 5. FSR SR route A — native D3D11

```text
SkyrimUpscaler +0x2949C0
  -> host call +0x294FBD
  -> PDPerfPlugin!EvaluateUpscaler +0xF8D20
  -> UpscaleAPI_D3D11 evaluator +0xF5CF0
  -> UpscaleMethod_FSR3_DX11::Evaluate +0xFF520
     -> descriptor / dispatch helper +0xFEDC0
        -> indirect SDK call +0xFF475
        -> ffx_fsr3_x64!ffxFsr3ContextDispatchUpscale +0x28B0
           -> call +0x31FF -> import thunk +0x3EBA
           -> ffx_fsr3upscaler_x64!ffxFsr3UpscalerContextDispatch +0x3F50
```

**Evidence strength:** the host export calls, class vtables, SDK loader, and last DLL-to-DLL import chain were inspected. Selection and exceptional/queued paths still depend on runtime state. [B: `pd_dx11_sr.asm`, `pd_dx11_sr_dispatch.asm`, `pd_dx11_library_loader.asm`, `ffx_dx11_upscale_forward.asm`, PE imports]

Important landmarks:

| PD RVA | Role |
|---:|---|
| `0xFDDB0` | DX11 FSR3 class constructor |
| `0xFE2E0` | DX11 interface/context creation path |
| `0xFE5B0` | Context destruction helper |
| `0xFF520` | Per-frame evaluator |
| `0xFEDC0` | SDK dispatch-description construction |
| `0xFF475` | Indirect `ffxFsr3ContextDispatchUpscale` call |
| `0xF3EC0` | Resolves DX11 backend and FSR3 exports |

The descriptor builder contains resource labels for **colour, depth, motion vectors, exposure, reactive map, output colour, and transparency/composition map**. It also transfers render dimensions, jitter, motion-vector scale, reset, sharpness, camera planes/FOV, and a frame identifier. This is a temporal renderer integration, not a screen-only spatial resize.

However, the presence of optional resource slots or labels does not prove that a valid non-null exposure/reactive/composition input is supplied on every host branch. The exact Skyrim material coverage and validity of those masks remain U.

### Actual DX11 effect version

At `ffx_fsr3upscaler_x64+0x42E0`, `ffxFsr3UpscalerGetEffectVersion` is:

```asm
mov eax, 0x00C01002
ret
```

Using AMD's `FFX_MAKE_VERSION(major, minor, patch) = (major << 22) | (minor << 12) | patch`, this decodes to **3.1.2**. This identifies the exported effect version; it is not a claim that all bundled libraries belong to one SDK release. [B: `dx11_fsr_version.json`; S5]

## 6. FSR SR route B — DX12 interop / FSR4-labelled backend

```text
Skyrim D3D11 resources
  -> PD UpscaleMethod_DX11WrapperForDX12
       constructor +0xCF080
       backend creation +0xCFA50
       per-frame evaluator +0xD0CB0
  -> companion-resource copies / synchronization / D3D12 command recording
  -> UpscaleMethod_FSR3_FFXAPI
       constructor +0x1003E0
       context creation +0x100620
       evaluator +0x100B70
       ffxDispatch call +0x100E84
  -> selected FFX API function table
  -> AMD loader / selected SR provider
  -> result handoff back to the D3D11 side
```

The generic wrapper is confirmed by RTTI at vtable RVA `0x11A82D8`. Its per-frame method contains resource-description checks, resource-copy calls, fence operations, D3D12 submission helpers, calls to the child backend, and output-copy paths. It is **not** equivalent to passing an `ID3D11Texture2D*` to an AMD DX12 dispatcher. [B: `pd_dx11_sr_bridge_evaluate.asm`, `vtable_and_enum_proof.json`]

The FFXAPI evaluator builds descriptor type `0x10001` and uses the API's Dispatch function at `+0x100E84`. Colour, depth, motion, output, and optional inputs are wrapped into API resource records. It does not use the DX11 `ffxFsr3ContextDispatchUpscale` entry point, despite reusing an error-message string with that name. [B: `pd_dx12_sr.asm`]

### Loader selection, not provider identification

PD `0xCC8C0` resolves separate function sets from:

```text
amd_fidelityfx_dx12.dll         legacy combined library
amd_fidelityfx_loader_dx12.dll  newer provider loader
```

PD `0xCCC80` chooses the active table. Its first five slots are:

```text
+0x00 Configure
+0x08 CreateContext
+0x10 DestroyContext
+0x18 Dispatch
+0x20 Query
```

For the D3D12 API, internal mode `4` selects the loader table. Presence of the FSR4-named DLL or a “Using FSR4” string is **not** by itself proof of the runtime's chosen algorithm, adapter support, successful context creation, or dispatch success. Inspect the selected context/provider and return values. [B: `pd_fsr_library_init.asm`, `pd_fsr_library_select.asm`]

## 7. FSR FG — separate preparation and interpolation stages

### 7.1 Skyrim/D3D11-to-D3D12 preparation

```text
SkyrimUpscaler FG producer +0x2941A0
  -> call +0x2948E0
  -> PD EvaluateFrameGeneration +0xF8DB0
  -> UpscaleAPI_D3D11 evaluator +0xF6120
  -> FrameGenMethod_DX11WrapperForDX12::Evaluate +0xEA650
  -> native-submission helper +0xEAD90
  -> FrameGenMethod_FSR3_FFXAPI::Evaluate +0xEF230
```

The DX11 wrapper constructor is `0xE9DC0`. Its child backend is constructed through `0xEC080`, which calls the FFXAPI FG constructor `0xEE680` at `0xEC0FA`.

The wrapper's normal path has the expected cross-API synchronization structure: D3D11 context vtable offsets `+0x4A0` (Wait), `+0x178` (CopyResource), `+0x498` (Signal), and `+0x378` (Flush) appear around the copy/submission operations. Actual wait/signal callsites include `0xEAAAF`, `0xEAAD2`, and `0xEABBB`. Microsoft describes Context4 fence waiting explicitly as applicable to D3D11/D3D12 interop. [B: `pd_dx11_fg_bridge.asm`; S6]

The submission helper `0xEAD90` replaces payload resource fields with companion D3D12 resources and writes the D3D12 command-list pointer at payload `+0x68`. It forwards the transformed 0xB0-byte payload to the child at `0xEB03E`, then calls its submission helper at `0xEB050`. There is also a queued FrameWarp branch; it is not part of the simple normal-mode graph above. [B: `pd_dx11_fg_submit.asm`]

**Revision 2 narrows this gap in sections 18–20. U:** a complete allocation/handle/fence ownership proof across every branch, cross-adapter behavior, and exact retirement under every queue mode. The inspected code demonstrates more than a copy-only bridge, but static inspection does not establish absence of synchronization bugs.

### 7.2 Native FFX context and per-frame configuration

Native initialization starts at **PD `0xEE870`**, not at the per-frame prepare method. It creates the FG context at indirect call `0xEEC1C`, using the device/backend extension and a version descriptor (type `0x2000E`, version value `0x01000000`). It also makes an initialization-time Configure call at `0xEED07`. The version-extension value is an **API compatibility value**, not an “FSR1” algorithm version. [B: `pd_fsr_fg_init.asm`]

Separate process-global context locations are:

```text
PD +0x121F9C0  swapchain context
PD +0x121F9C8  frame-generation context
PD +0x11F9DD0  reusable FG configure description
```

The inspected native per-frame method `0xEF230` performs:

```text
1. Check readiness, enablement, inputs and dimensions.
2. Build PrepareV2 (type 0x2000C).
3. ffxDispatch(PrepareV2)             call +0xEF581
4. Build/update FG Configure:
     enabled / async / HUD-less colour / frame ID / callback
5. ffxConfigure(FG context)          call +0xEF719
6. Register or clear UI texture:
     DX12 type 0x30002
7. ffxConfigure(swapchain context)   call +0xEF834
```

Preparation consumes depth and motion vectors; the later interpolation dispatcher gets its presentation colour and output surfaces from the swapchain callback descriptor. Do not conflate the **guide-preparation call** with generation of the final interpolated image. [B: `pd_fsr_fg_evaluate.asm`, `pd_fg_callback.asm`]

### 7.3 Present-driven interpolation — independently verified in the AMD DLL

AMD PDB symbols and the swapchain vtable provide this chain:

```text
amd_fidelityfx_framegeneration_dx12.dll
  Present                                      +0xBAB0
    -> virtual +0x168
  presentInterpolated                          +0x98E0
    -> virtual +0x160
  dispatchInterpolationCommands                +0x94A0
    -> registered callback at +0x97F1

PDPerfPlugin.dll
  registered frameGenerationCallback           +0xEF880
    -> ffxDispatch at +0xEF8C6

AMD runtime
  dispatch through the FG context's selected provider
    -> classical FSR3 provider OR ML provider
```

The vtable at AMD FG `0x1073E8` maps `+0x168` to `0x98E0` and `+0x160` to `0x94A0`; `dispatchInterpolationCommands` loads its callback/user-context from object fields `+0x1AA0`/`+0x1AA8` before the indirect call. PD sets its callback at `0xEF68A`–`0xEF691` and its context pointer at `0xEF6A5`.

This independently agrees with AMD's documented callback-in-Present architecture. It is **not** a transfer of a DLSS-G/Streamline conclusion to FSR. [B: three AMD swapchain excerpts, `pd_fg_callback.asm`, `vtable_and_enum_proof.json`; S1]

The PD callback reads the returned `numGeneratedFrames` at descriptor `+0x108` and processes up to four output-resource slots beginning at `+0x48`. That establishes callback capacity, **not** proof that this configuration produces four synthetic frames or implements working multi-frame generation on a particular GPU.

## 8. What the AMD FG provider does

The matching FG PDB names both providers:

| AMD FG RVA | Symbol / role |
|---:|---|
| `0x6D90` | Provider selection helper |
| `0xECA90` | `ffxProvider_MLFrameGeneration::IsSupported` |
| `0xF0BD0` | ML provider Dispatch |
| `0xF25C0` | Classical FSR3 provider CreateContext |
| `0xF2F50` | Classical FSR3 provider Configure |
| `0xF4050` | Classical FSR3 provider Dispatch |
| `0xF8C40` | `ffxFrameInterpolationPrepare` |
| `0xF8EC0` | `ffxFrameInterpolationDispatch` |
| `0xFC940` | Optical-flow dispatch implementation |

The classical provider dispatch method has separate branches for preparation and generation. Its callsites include:

```text
+0xF457A -> ffxFrameInterpolationPrepare +0xF8C40
+0xF4750 -> optical-flow dispatch        +0xFC940
+0xF4ACC -> ffxFrameInterpolationDispatch+0xF8EC0
```

PDB symbols additionally identify shader-permutation helpers for reconstruction/dilation, game motion-vector fields, optical-flow vector fields, previous-depth reconstruction, disocclusion masks, inpainting pyramids, interpolation, and inpainting. These establish the algorithm components present. They are **not substituted for a captured GPU pass order**, barrier trace, or shader-quality analysis. [B: `amd_classic_fg_dispatch.asm`, selected symbol index]

### Why the FSR4 FG log is insufficient

In PD `0xCC8C0`, the loader availability flag is set separately from the result of `GetModuleHandleW("amd_fidelityfx_framegeneration_dx12.dll")`. The FG constructor at `0xEE680` starts with internal mode `3`, changes to `4` when those availability flags are set, and logs “Using FSR4 Frame Generation.”

The AMD runtime still has provider selection and support checks. Therefore, the valid static conclusion is **“the newer loader/FG DLL route was selected,” not “ML FG successfully runs.”** A live provider pointer, successful context, and provider Dispatch hit are needed for the latter. [B: `pd_fsr_fg_constructor.asm`, `pd_fsr_library_init.asm`, `amd_fg_provider_select.asm`, `amd_ml_support.asm`]

## 9. UI/HUD preservation and pacing

In the common PD payload:

- `+0x88` supplies the separate UI resource.
- `+0x90` supplies the HUD-less colour resource.
- `+0x9C` selects the premultiplied-alpha flag in UI registration.

The native FG evaluator places HUD-less colour into the FG configuration when the UI texture is absent and the special FrameWarp branch does not suppress it. UI texture registration instead goes to the **swapchain** context.

From the matching PDB, `FfxApiUiCompositionFlags` is:

```text
0x1 = FFX_FRAMEGENERATION_UI_COMPOSITION_FLAG_USE_PREMUL_ALPHA
0x2 = FFX_FRAMEGENERATION_UI_COMPOSITION_FLAG_ENABLE_INTERNAL_UI_DOUBLE_BUFFERING
```

At PD `0xEF7C7`–`0xEF7FA`, AIO18 writes flags `2` or `3`, so internal UI double-buffering is selected and premultiplied alpha is conditional. [B: `pd_fsr_fg_evaluate.asm`, `vtable_and_enum_proof.json`]

Related AMD implementations, identified by PDB:

```text
registerUiResource          +0xADC0
copyUiResource              +0x9EA0
presenterThread             +0xCBD0
compositeSwapChainFrame     +0xE4C0
ffxFrameInterpolationUiComposition +0xF5600
```

**Implication:** preserve a stable UI input and its alpha convention across generated and real presentations. Do not recycle it simply because the Skyrim render call returned. AMD documents that UI composition and paced presentation are separate from interpolation dispatch. [S1, S2; I]

**Revision 2 now maps the pre-UI SR/UI switch and the later OMSetRenderTargets redirection (section 16). U:** the exact shared Skyrim/Scaleform flush at which every host texture becomes final, including ENB, TrueHUD, third-party late widgets, and every ReShade configuration. The archive does not include the matching Skyrim executable or a GPU capture. The renderer-level integration above does not solve all late-UI cases by itself.

## 10. Recovered payload and AMD descriptor layouts

### 10.1 PD 0xB0-byte evaluation payload: recovered subset

This table is for inspection of **this PD build**, not a proposed public SDK API.

| Offset | Observed meaning / handling |
|---:|---|
| `+0x08` | Colour resource pointer |
| `+0x10` | Motion-vector resource pointer |
| `+0x18` | Depth resource pointer |
| `+0x20` | **Corrected in v3:** reactive-mask resource in DX12 FSR SR; do not treat as universal output. See §26.1. |
| `+0x28`, `+0x30` | **Corrected in v3:** DX12 FSR SR default output / preferred explicit output, respectively. Other private routes require separate tracing. |
| `+0x38`, `+0x3C` | Render width/height stored as floats and converted for AMD descriptors |
| `+0x40` | Sharpness in SR descriptor construction |
| `+0x44`, `+0x48` | Jitter offset |
| `+0x4C`, `+0x50` | Motion-vector scale |
| `+0x54` | Reset byte |
| `+0x58`, `+0x5C` | Camera-plane inputs; interpretation includes depth-mode and conversion helper |
| `+0x60` | FOV input copied to vertical-FOV field; producer units require runtime validation |
| `+0x68` | Command-list pointer on the native D3D12 side |
| `+0x88` | UI texture |
| `+0x90` | HUD-less colour |
| `+0x98` | 32-bit source frame identifier, zero-extended into AMD 64-bit fields |
| `+0x9C` | UI premultiplied-alpha selector |

Unlisted bytes are not assigned invented meanings. An input pointer may refer to D3D11 or D3D12 objects depending on where it is observed in the bridge. [B: native SR/FG evaluators and bridge]

### 10.2 PrepareV2: matched AMD PDB, size **0xE8**

| Offset | Field |
|---:|---|
| `0x00` | `ffxApiHeader` (type + next pointer, 0x10 bytes) |
| `0x10` | `uint64_t frameID` |
| `0x18` | `uint32_t flags` |
| `0x20` | `commandList` |
| `0x28` | `renderSize` |
| `0x30` | `jitterOffset` |
| `0x38` | `motionVectorScale` |
| `0x40` | `frameTimeDelta` |
| `0x44` | `reset` |
| `0x48`, `0x4C` | `cameraNear`, `cameraFar` |
| `0x50` | `cameraFovAngleVertical` |
| `0x54` | `viewSpaceToMetersFactor` |
| `0x58` | `depth` resource record |
| `0x88` | `motionVectors` resource record |
| `0xB8` | `cameraPosition[3]` |
| `0xC4` | `cameraUp[3]` |
| `0xD0` | `cameraRight[3]` |
| `0xDC` | `cameraForward[3]` |

Each `FfxApiResource` is **0x30 bytes**: pointer at `+0`, description at `+8`, state at `+0x28`. The matching PDB calls these inputs `depth` and `motionVectors`. Do not transplant field semantics from a newer moving `main` header that describes dilated inputs.

Other recovered complete types in `amd_fg_struct_layouts.json`:

```text
ffxConfigureDescFrameGeneration                    0x90
ffxDispatchDescFrameGeneration                     0x138
ffxCreateContextDescFrameGeneration                 0x28
ffxConfigureDescFrameGenerationSwapChainRegisterUiResourceDX12 0x48
```

The **0x138** AMD FG dispatch size here is unrelated to the earlier PD neural-rendering wrapper's 0x138 copy extent. These are different structures in different APIs.

## 11. Two concrete ML-integration concerns

### A. PrepareV2 camera vectors remain zero in the inspected PD path

At PD `0xEF430`–`0xEF45E`, the stack region holding PrepareV2's resource records and camera-vector tail is zeroed. Depth and motion-vector records are subsequently overwritten; the four vectors at `+0xB8..+0xE7` are not. The final helper `0xF0B60` merely clears `header.pNext` and returns the same pointer; it does not populate camera data.

At `0xEF4D2`, `viewSpaceToMetersFactor` is also explicitly written as zero. The frame then reaches Dispatch at `0xEF581`. [B: `pd_fsr_fg_evaluate.asm`, `pd_prepare_header_finalize.asm`]

The bundled AMD DLL independently contains checks/warnings for uninitialized camera vectors in its ML and classical provider code, including references at `0xF1977` and `0xF455E`. Whether messages are emitted depends on its diagnostic flags. Current AMD ML FG documentation also requires valid camera position/orientation. [B: provider dispatch excerpts; S3]

**Conclusion:** this path does not supply complete ML camera metadata. It is a real static integration gap, but does not establish which provider a user's machine selects or whether a visible artifact occurs. A separate PD `SetCameraData` call elsewhere is not proof that the AMD PrepareV2 fields were filled.

### B. Per-frame Prepare precedes that method's Configure

In the same native method, the calls are `0xEF581` Prepare, then `0xEF719` Configure. They use the same payload-derived frame ID. AMD's documented integration orders Configure before Prepare; its ML provider also contains an order-check message at `0xF0F49`. [B: both dispatch excerpts; S3, S4]

**Qualification:** AIO18 also makes Configure calls during initialization and enable/disable handling. Therefore, do not claim “Configure is never called before Prepare.” The narrower, proven result is that the inspected per-frame method prepares before updating its same-frame configuration. A live trace must establish the complete cross-caller sequence and the provider state.

**Implementation implication:** an independent integration should configure the current frame, prepare its guides, then let Present invoke interpolation. Supply real camera metadata and check all statuses. Do not mechanically copy this suspicious ordering or use a blind instruction swap; enablement, reset, HUD, and error branches are intertwined. [I]

## 12. Teardown and resize

PD release method **`0xEEF20`** visibly clears HUD-less colour, disables FG, clears the present callback, configures those changes, clears the registered UI resource, and destroys the FG and swapchain contexts, subject to its state branches. Important calls are:

```text
0xEF011 Configure disabled FG state
0xEF0BD Configure cleared UI registration
0xEF0F6 Destroy FG context
0xEF126 Destroy swapchain context
```

The supplied AMD implementation has real swapchain resize/wait methods:

```text
ResizeBuffers     +0xBEC0
ResizeBuffers1    +0xC490
waitForPresents   +0xAAF0
shutdown          +0x8950
```

These are implemented functions, not evidence of a no-op ResizeBuffers stub. A full proof that every PD resize/backend-switch branch drains all D3D11, D3D12, interpolation, and presenter ownership has **not** been established. AMD explicitly warns about destroying in-use FG resources and unsynchronized context operations. [B: `pd_fg_release.asm`, AMD resize/wait excerpts; S2]

## 13. Runtime confirmation plan and breakpoint map

This is the next experiment, **not a trace already collected**. Run the original build with a controlled configuration, and validate module hashes before using addresses.

| Breakpoint | What to record |
|---|---|
| Host `+0x294FBD` | Raw 0xB0 SR payload; resource descriptors, render dimensions, jitter and reset |
| PD `+0xFF475` | DX11 SR SDK call; confirm method 3 actually reaches it |
| PD `+0x100E84` | DX12 SR Dispatch and concrete function-pointer target; context/provider identity |
| Host `+0x2948E0` | Raw FG payload and frame ID |
| PD `+0xEAD90` | Bridge input/output resource identity, command list, fence values |
| PD `+0xEEC1C` | FG context creation descriptor chain and return status |
| PD `+0xEF581` | PrepareV2 descriptor, frame ID, vectors, reset, call sequence, target function |
| PD `+0xEF719` | Same-frame FG Configure and its return status |
| PD `+0xEF834` | UI registration descriptor, flags and stable resource identity |
| AMD FG `+0xBAB0` / `+0x97F1` | Present-to-callback stack |
| PD `+0xEF8C6` | Actual interpolation Dispatch descriptor and concrete target |
| AMD FG `+0xF4050` / `+0xF0BD0` | Which provider actually dispatches |
| AMD FG `+0xAAF0` | Drain behavior during resize, disable and backend switches |

At the native FFX callsites, `RCX` is normally the context pointer argument and `RDX` the descriptor pointer under the observed Windows x64 calling convention. At the PD export entry, `RCX` points to the common input payload. Record register values at the exact instruction, not after stepping into a callee that repurposes them.

For each real frame, correlate one source frame ID across Configure, Prepare and generation; separate real and generated presentations. Record GPU adapter identity, API provider/version, texture dimensions/formats, colour transfer space, depth convention, motion scale, jitter sign, reset cause, queue/fence values, and resource lifetime.

A useful minimal matrix is: DX11 SR alone; DX12 SR alone; FSR FG with a known SR; UI texture versus HUD-less; ENB off/on; then ReShade off/on. Add loading transitions, menus, alt-tab, resize and backend toggles after the baseline is stable. This is an experiment plan, not a promise of compatibility.

## 14. What this means for a new Skyrim renderer integration

The reusable architecture is not “call FSR during Present.” It has distinct responsibilities:

```text
Skyrim input capture and render/native-UI boundary
        |
        +-- Temporal SR backend
        |     DX11 FSR3, or D3D12-interoperated FFX SR
        |
        +-- FG guide preparation with frame identity
              |
              +-- AMD interpolation swapchain
                    Present callback -> interpolation
                    UI composition -> paced real/generated presents
                    explicit drain/recreation lifecycle
```

Reimplement the interface contracts rather than requiring PDPerfPlugin as a dependency. Keep SR and FG selections independent. Track API compatibility separately from algorithm/provider version. Preserve motion, depth, camera, colour-space, UI-alpha and fence semantics; these are at least as important as getting a Dispatch return code of success.

This report supplies build-specific landmarks and real dataflow evidence. It does **not** supply a verified Skyrim engine hook RVA, a complete ENB/ReShade/Scaleform flush chain, a proven low-resolution scene/native-resolution UI split for every mod, a recovered ML network, a fully disassembled shader pipeline, or runtime performance/quality measurements. Those boundaries should remain explicit in any Codex handoff.

## 15. Sources and reproducibility

### Primary external references

These references contextualize the binary evidence; the supplied PDB remains authoritative for this DLL's layout.

- **S1** AMD, FSR Frame Generation Swapchain: callback-driven Present, UI composition and paced presentation. https://gpuopen.com/manuals/fsr_sdk/techniques/frame-interpolation-swap-chain/
- **S2** AMD, FSR Frame Generation API: context, dispatch, shutdown and thread-safety contracts. https://gpuopen.com/manuals/fsr_sdk/techniques/frame-interpolation-api/
- **S3** AMD, ML Frame Generation 4.0.1: mandatory call order and camera inputs. This is newer documentation, not a claim that the supplied 4.0.0 DLL is 4.0.1. https://gpuopen.com/manuals/fsr_sdk/techniques/frame-interpolation-ml/
- **S4** AMD, FSR3.1.4 integration: independent FG prepare and configure-before-prepare. https://gpuopen.com/manuals/fidelityfx_sdk/techniques/super-resolution-interpolation/
- **S5** AMD, SDK Utilities: FFX_MAKE_VERSION packing. https://gpuopen.com/manuals/fidelityfx_sdk/fidelityfx_sdk-group__utils/
- **S6** Microsoft, ID3D11DeviceContext4::Wait: interop fence semantics. https://learn.microsoft.com/en-us/windows/win32/api/d3d11_3/nf-d3d11_3-id3d11devicecontext4-wait

### Analysis method

Extraction used system libarchive through ctypes, with path/link checks. Pure Python parsers read PE metadata, imports including delay imports, exports, CodeView identities, exception records, PDB MSF/CodeView symbols and TPI field layouts. GNU objdump 2.44 supplied x64 disassembly. Python-side reference indexing tied direct calls, RIP-relative references, RTTI and vtables to the matching modules.

No Ghidra, Capstone, PDB decompiler, malware sandbox execution, GPU profiler, or game execution was used. String annotations can include coincidental printable bytes; names/claims in this report were corroborated with API dataflow, RTTI, PDB records or explicit diagnostic text rather than treating arbitrary strings as symbols.

Function-range handling follows chained x64 unwind entries. The host's actual PE exception directory, not a section-name assumption, was used; it contains 10,058 valid entries in this build. A generic tool's interpretation of the named `.pdata` section is not the analysis ground truth.

The ZIP contains hashes, inventories, matching-PDB identity records, recovered layouts, selected symbol records, targeted disassembly, landmark byte checks, and read-only analysis scripts. It intentionally omits the original DLLs, PDBs, and large embedded shader/string dumps. See `README.md` and `verification.json` in the package.

---

## 16. Revision 2 — actual pre-UI SR placement and persistent render-target redirection

This revision adds new disassembly of the same uploaded bytes. It is **not a new game execution**. The most useful advance is that the Skyrim-facing hook is now connected to an actual SR call, optional ReShade execution, and the later UI render-target transition. This narrows, but does not remove, the game-side uncertainties in sections 4, 9 and 14.

### 16.1 AIO18's pre-UI hook installation is now mapped

The installer at `SkyrimUpscaler+0x1A0900` contains the following installation region:

| Host instruction RVA | Observed operation |
|---|---|
| `0x1A16F2` | First relocation ID literal `0x1384B` = **79947** |
| `0x1A16FB` | Second relocation ID literal `0x140A4` = **82084** |
| `0x1A170E`, `0x1A1715` | Displacements **`0x17A`** and **`0x16F`** |
| `0x1A171B`–`0x1A1722` | Runtime-tag comparison selects a displacement and adds it to the resolved address |
| `0x1A1736`–`0x1A173E` | Recovers the target of the original relative call |
| `0x1A1744` | Replacement function is **`SkyrimUpscaler+0x19F420`** |
| `0x1A1756` | Saves original call target in global **`SkyrimUpscaler+0x47D3B0`** |
| `0x1A17B5` | Adjacent diagnostic names this hook `Main_DrawWorld_PreUI` |

The exact runtime test is `byte[version_helper()+0x118] == 1`: true selects `0x17A`, otherwise `0x16F`. The pair and displacements are byte-backed. The usual SE/AE interpretation is not needed to prove the installation; **this archive alone does not resolve a SkyrimSE.exe RVA for either runtime**. The literal IDs are not executable addresses. [B: `v2/disassembly/host_preui_hook_install.asm`]

### 16.2 The normal branch calls SR before switching to UI drawing

The new handler map is:

```text
SkyrimUpscaler+0x19F420  pre-UI handler
    |
    +0x19F583  call saved original engine target [+0x47D3B0]
    |
    [eligible processing branch]
    +0x19F6EA  host+0x489 = 1: permit controlled ReShade effects
    |
    optional pre-SR ReShade render_effects at +0x19F8FF
    |
    +0x19FA14  call +0x2949C0: Skyrim SR producer
    |             input wrapper = proxy+0x40 + index*0x58
    |             output wrapper argument = proxy+0x148
    |
    optional post-SR ReShade render_effects at +0x19FAD0
    |
    +0x19FB28  host+0x489 = 0
    |
    UI selection / depth state / render-target binding
    +0x19FC85  separate UI image candidate at host+0x9E0
    +0x19FCF3  ID3D11DeviceContext::OMSetRenderTargets
    set UI-phase state; conditional UI clear
```

The pre-/post-ReShade selector is not guessed: `mRenderReShadeBeforeUpscaling` is parsed and stored to **host+`0x487`** at `0x29B4A9`. The runtime pointer is **host+`0x1690`**. Its vtable slot **9**, byte offset **`0x48`**, is independently identified by the hook installer and the adjacent `Hooked ReShade render_effects` diagnostic. [B: `host_preui_handler.asm`, `host_reshade_setting.asm`, `host_reshade_vtable_hook_install.asm`, under `v2/disassembly/`]

This sequence is **conditional**. The top branch controlled by host+`0x343` and additional rendering-mode conditions follows other paths. Another predicate can move effects to the pre-SR portion even when the user selector is false. Therefore the map is an observed normal-path ordering, not one order for every ENB, Community Shaders, menu, neural-rendering or FrameWarp configuration. It does not assert a universal post-tonemap or post-ENB boundary. [B/I]

### 16.3 ReShade effects are deliberately suppressed outside the controlled window

`hk_ReShadeCreateEffectRuntime` at `0x2A68C0` patches runtime slot 9 to **`0x2A6AA0`**. That small function reads host+`0x489`: when zero it returns; when nonzero it tailcalls the saved original `render_effects` pointer at global `0x47D410`.

That is a concrete **duplicate-execution guard**, not merely a name for a callback. An independent renderer should model whether its manual ReShade effects call and ReShade's automatic call can both execute; blindly invoking effects from both SR and Present would not reproduce this behavior. It does **not** imply that every ReShade overlay/add-on event is suppressed by the same guard. [B/I: `v2/disassembly/host_reshade_render_effects_guard.asm`]

### 16.4 UI separation persists across subsequent render-target bindings

The handler does more than select a render target once. A separate **`OMSetRenderTargets` hook at `0x2A9B30`** is installed into context vtable slot **33** (`0x108`) at `0x19EDDD`–`0x19EDF2`. The original function is saved at global `0x47D440`. A context-specific/TLS forwarding path is also present. [B: `host_om_hook_install.asm`, `host_omsetrendertargets_redirect.asm`]

The hook reads **UI-phase global `0xE81092`**, examines the requested RTV's underlying resource, compares it with recognized proxy/rendering targets, and conditionally substitutes the first RTV in a local binding array. The separate-image branch resolves **host+`0x9E0`** at `0x2A9E9E` and replaces the first RTV at `0x2A9EEB`. It also has depth and multi-target branches; it is **not an unconditional replacement of all RTVs on the device**. The phase global is set by the pre-UI handler after its target transition. [B]

`CheckUIBuffer` at **`0x2A4730`** establishes the image's identity. It copies the description of proxy buffer 0, changes bind/misc flags, and creates the texture into host+`0x9E0` at `0x2A49E4`. The failure diagnostic calls this resource **`mTempUIColorForRender`**. Host+`0x988` is the separately named **`mTempUIColor`** allocation. In this creation branch, UI width/height are inherited from the proxy buffer's description—not independently assigned a smaller SR-input size. That proves **proxy-buffer-sized UI storage**, not a live measurement that the proxy is native-sized under every setup. [B: `host_check_ui_buffer.asm`]

Two configuration fields are also now mapped: `mEnableHUDFix` is host+`0x4FA`, and `mHUDFixMethod` is host+`0x4FC`. The selection predicate at `0x293F20` tests the method and several rendering-mode conditions; method zero is the separate UI texture option described by the shipped INI. A HUD-less mode need not take the same UI-target branch. [B: `host_hudfix_settings.asm`, `host_preui_separate_ui_predicate.asm`; original `shipped_settings.json`]

**Implication for the earlier late-widget problem:** the relevant contract is not just “hook before each menu's PostDisplay.” AIO18 combines a phase boundary with later API-level render-target redirection. This is a reason to trace the final shared UI submission and its bindings. It does not identify the final Scaleform flush, prove which mod submitted a widget, or show that every third-party UI draw is successfully redirected. [I/U]

## 17. Revision 2 — FSR preparation versus ReShade's final-overlay gate

### 17.1 The early and late FG calls are not interchangeable

`mFrameGenType` is parsed into host+`0x3E0`; its active value is copied to **host+`0x3DC`** at `0x29BF70` and updated at a frame boundary. The shipped INI assigns **1 = FSR FG, 2 = DLSS FG, 3 = XeSS FG**. [B: `host_fg_type_setting.asm`; original settings evidence]

The early `BeforePresent` FG call at **`0x2AB349`** is guarded by:

```text
host+0x4E4 == 0
host+0x3DC != 2          // not the DLSS FG selector
host+0x4A5 == 0          // FrameWarp-active boolean
```

The FrameWarp interpretation of `+0x4A5` is backed by the `mPDFrameWarpMode` configuration path: a nonzero parsed mode becomes this boolean at `0x29C113`. The additional `+0x4E4` presentation-mode flag is retained explicitly rather than assigned an invented complete enum. Normal FSR selection with both gates clear reaches this **early preparation** site. Actual interpolation still occurs later through AMD Present as established in section 7. [B: `host_before_present_early_fg.asm`, `host_framewarp_active_setting.asm`]

The late branch at `0x2ABCD1`–`0x2ABD02` instead requires **DLSS FG type 2 or active FrameWarp**, as well as surrounding frame/mode checks. A valid ReShade runtime plus the relevant gate can defer that submission by setting global **`0xE80F9F`**. Without that deferral condition, the same late branch immediately calls the FG producer at `0x2ABD02`.

**Correction to a tempting overgeneralization:** “AIO18 always waits for ReShade's final overlay before FSR preparation” is false for the inspected normal FSR branch. FSR-plus-FrameWarp configurations can enter different handling; the branch distinction matters. [B/I: `host_before_present_late_fg.asm`]

### 17.2 Exact callback registration and pending-source behavior

Registration at **`0x284A70`** passes **event ID `0x4B` = 75** and callback **`0x284D30`** to the resolved `ReShadeRegisterEvent`. ReShade's primary header defines event 75 as `reshade_present`, after effects and overlay rendering. The event number, callback and dataflow are observed in this binary; the public header supplies the event semantics. No ReShade DLL was supplied for an independent full runtime ABI identity check. [B: `host_reshade_register.asm`; S: V2-S1]

The registered function:

```text
+0x284D45  incoming runtime must match host+0x1690
+0x284D52  pending flag +0xE80F9F must be set
+0x284D5F  proxy pointer +0xE80F20 must exist
+0x284D85  clear pending before invoking downstream processing
+0x284D93  obtain current proxy backbuffer index
+0x284DA2  invoke FG producer +0x2941A0 for that source
```

A same-purpose clone exists at `0x2AA630`, but the registration shown above points to **`0x284D30`**. Using only the clone as a breakpoint risks missing the registered path. [B: `host_reshade_complete_registered.asm`, `host_reshade_complete_clone.asm`]

When that callback did not consume a pending source, `AfterPresent` at **`0x2AC050`** logs the missing-final-submission condition and clears the pending bit at **`0x2AC105`**. There is no new call to the ordinary FG producer in that inspected `AfterPresent` method. Thus this path does not establish a fallback that magically obtains a valid completed source. This is a static failure path, not a claim that the callback actually fails on the user's machine. [B: `host_after_present.asm`]

## 18. Revision 2 — actual AMD UI ownership, frame clocks and resource reuse

### 18.1 Internal PDB layouts now distinguish six fence roles

The richer C13/TPI parser recovers nine additional complete layouts, including four zero-offset inheritance wrappers, with **91 direct data fields**. Base/method/nested records are parsed instead of silently stopping at the first non-data member. Unknown field kinds fail explicitly. [B: `v2/amd_internal_class_layouts.json`; tool: `scripts/pdb_class_layout.py`]

The selected swapchain template (TPI **`0x3200`**) has size **`0x1AC0`**, with `presentInfo` embedded at root offset **`0x8`**. Its nested present-info layout (TPI `0x332C`) has size `0x1758`. The four wrapper types demonstrate the relevant inheritance at offset zero.

**Add the embedding offset before using a nested field as a root-object offset.** For this concrete swapchain:

| Root-object offset | PDB field / role |
|---:|---|
| `0x16D0` | gameQueue |
| `0x16D8` | presentQueue |
| `0x16E0` | gameFence |
| `0x16E8` | interpolationFence |
| `0x16F0` | presentFence |
| `0x16F8` | replacementBufferFence |
| `0x1700` | compositionFenceCPU |
| `0x1708` | compositionFenceGPU |
| `0x1868` | interpolationFenceValue |
| `0x1870` | gameFenceValue |
| `0x1A60` | currentFrameID |
| `0x1A68` | framesSentForPresentation |

The vtable at **AMD FG `0x1073E8`** is checked against 22 method pointers, tying `Present`, UI copy, resource destruction and drain calls to this concrete implementation. These are internal implementation details for the pinned DLL, not a supported ABI for an independent plugin. [B: `v2/amd_swapchain_vtable.json`]

### 18.2 UI registration is not the copy operation

`registerUiResource` at **AMD `0xADC0`** locks, copies the caller's `0x30`-byte resource record into root+`0x1688`, and stores flags at root+`0x16B8`. Its inspected body does not AddRef the supplied resource. A null resource clears the internal-copy flag. [B: `amd_ui_register.asm`]

The actual private UI copy happens **inside `Present`**:

```text
AMD Present +0xBAB0
    +0xBB5D  CPU wait: compositionFenceCPU, prior framesSentForPresentation
    +0xBB76  gameQueue waits on compositionFenceGPU at the same snapshot
    +0xBB7F  verify/recreate UI duplicate if necessary
    +0xBBA2  call copyUiResource when flags&2 and UI is non-null
    ...
    +0xBBD6  proceed to interpolated presentation when selected
```

`copyUiResource` at **`0x9EA0`** uses the registered caller texture at root+`0x1688` and private texture at root+`0x1A30`. It emits resource barriers (`0xA002`), a **CopyResource** (`0xA014`), reverse barriers (`0xA045`), command-list submission (`0xA06C`), and the command-pool fence signal (`0xA07D`). It then clears the caller pointer (`0xA080`). Recording/submitting a copy is not a CPU guarantee that the GPU has already completed it. [B: `amd_present.asm`, `amd_ui_copy.asm`]

`verifyUiDuplicateResource` at **`0x9C60`** compares resource dimensions/format and waits on GPU composition before destroying an incompatible duplicate. It creates a texture explicitly named **`AMD FSR Internal Ui Resource`**. [B: `amd_verify_ui_resource.asm`]

**The internal layout contains one UI replacement resource, two interpolation output resources, and a separate replacement-backbuffer array.** Do not describe the “double-buffering” API flag as proof of two private UI textures or confuse those textures with the interpolation outputs.

The narrower lifetime conclusion is: the caller's source must remain valid through the SDK's capture/submission contract; the selected internal UI-copy mode exists to decouple later asynchronous composition from reuse of that caller source. AMD's current API documentation expressly permits reuse after Present returns when this flag is selected. That API guidance complements, rather than replaces, the above binary synchronization evidence, and should not be misread as immediate GPU completion or permission to destroy unrelated in-flight inputs. [I/S: V2-S2]

### 18.3 Three indexing concepts must not be merged

The AMD PDB and Present arithmetic distinguish:

| Storage/counter | Observed meaning |
|---|---|
| `replacementSwapBuffers[16]`, root+`0x1910` | Capacity for game-facing replacement resources; active count is separate |
| `gameBufferCount`, root+`0x1848` | Runtime count used for modulo indexing |
| `interpolationOutputs[2]`, root+`0x1A10` | Alternating generated-output resources |
| `presentCount`, root+`0x1A48` | Drives index advancement |
| `currentFrameID`, root+`0x1A60` | Temporal/source identity |
| `framesSentForPresentation`, root+`0x1A68` | Presentation completion progress |

At `0xBC72`, Present stamps the just-used replacement resource with an availability value. At `0xBCA7`, it stores the alternating interpolation-output index. At `0xBCB2`, it stores the next game-buffer index after modulo `gameBufferCount`. Before returning, **`0xBCE3` waits on replacementBufferFence for that next resource's availability value**. [B: `amd_present.asm`; matching class layouts]

The maximum array capacity **16 does not mean 16 active backbuffers or 16 generated frames**. Likewise, the three-slot companion ring in PD's transfer layer is not AMD's two-slot interpolation-output array. Tracking one generic “frame number” for all of them loses important lifetime information. [B/I]

## 19. Revision 2 — shared D3D11/D3D12 fences and an attribution trap

### 19.1 Shared fence creation now has a concrete API chain

The generic interop initializer at **PD `0x72810`**, reached by companion-wrapper initialization paths, contains:

```text
+0x72A33  D3D12 device creates fence
+0x72A75  D3D12 device creates shared handle for that fence
+0x72AAF  D3D11Device5::OpenSharedFence
+0x72AF9  optional additional D3D12 device opens shared handle
+0x72B19  CloseHandle after the API objects have imported it
```

This is evidence of a **shared synchronization object**, not merely coincidentally similar independent counters. Microsoft documents OpenSharedFence specifically for D3D11/D3D12 fence interoperation. [B: `pd_interop_fence_init.asm`; S: V2-S3]

The texture side has multiple branches. In the path around `0x75567`, PD obtains a legacy shared handle or falls back to creating an NT shared handle (`0x7559A`). It calls the companion open helper at `0x755DB`; helper `0x6E850` performs D3D12 `OpenSharedHandle` at `0x6E884` with the resource IID and checks failure. Another path exports an existing D3D12 resource to D3D11. The larger allocator also contains a separately diagnosed cross-adapter buffer-bounce path. **Do not label every route “zero-copy” or equate sharing a resource with automatically synchronizing its GPU writes.** [B/I: `pd_interop_resource_create.asm`, `pd_dx12_open_shared_resource.asm`]

### 19.2 Companion-ring reuse is separate from final presentation retirement

FG bridge submission at **PD `0xEAD90`** derives a slot modulo three, calls the slot wait helper at `0xEAEAC`, records/remaps companion resources and a D3D12 command list, stores the selected slot at `0xEAF15`, invokes the child backend at `0xEB03E`, and submits at `0xEB050`. Helper **`0x77940`** waits recorded slot values and also contains an optional external-fence path. [B: `pd_fg_prepare_handoff.asm`, `pd_interop_slot_wait.asm`]

Those waits establish staging reuse logic. They do not, by themselves, identify every resource retained by an AMD presentation callback, or prove that any particular external-fence pointer is live for FSR on the user's machine. [U]

### 19.3 The diagnostic named “FGFence” is not sufficient evidence of FSR ownership

The generic fence-publication function at **PD `0x77D60`** contains FGFence diagnostics. However, the direct-call index for this exact disassembly finds its caller at **`0x7F579`**, inside **`0x7EE20`**, whose preceding path resolves/calls **`slDLSSGGetState`** and reports DLSS-G capability information.

Therefore this pass **does not attribute that publication path to FSR**. A runtime or independently proven indirect caller would be needed to do so. This is a useful negative control: neither the word “FG” nor proximity to a generic wrapper proves that a fence belongs to AMD interpolation. The AMD fence roles in section 18 are instead backed by the matching AMD PDB and AMD methods themselves. [B/I: `pd_dlssg_external_fence_source.asm`, `pd_interop_fg_retire.asm`, `v2/pipeline_map_v2.json`]

## 20. Revision 2 — exact AMD resize/drain sequence and stall diagnostics

### 20.1 Resize drains the AMD layer before underlying buffer resize

The concrete AMD vtable maps:

```text
vtable+0x188 -> +0xA360  destroyReplacementResources
vtable+0x190 -> +0xA5C0  killPresenterThread
vtable+0x198 -> +0xA630  spawnPresenterThread
vtable+0x1A0 -> +0xA6C0  discardOutstandingInterpolationCommandLists
vtable+0x1B8 -> +0xAAF0  waitForPresents
```

`ResizeBuffers` at **AMD `0xBEC0`** first invokes the destruction helper at `0xBEEC`. That helper locks, calls `waitForPresents`, stops an existing presenter thread, discards outstanding interpolation command lists, releases replacement resources, interpolation outputs and internal UI storage, resets counters/fence state, marks a temporal reset, and restarts the presenter if one existed. The helper releases all allocated entries in the fixed-capacity replacement array, not just a guessed current-buffer pointer. [B: `amd_swapchain_resize.asm`, `amd_destroy_replacement_resources.asm`]

The drain method at **`0xAAF0`** waits:

| Call RVA | Fence | Target value |
|---|---|---|
| `0xAB13` | root+`0x16E0` gameFence | root+`0x1870` gameFenceValue |
| `0xAB32` | root+`0x16E8` interpolationFence | root+`0x1868` interpolationFenceValue |
| `0xAB51` | root+`0x16F0` presentFence | root+`0x1A68` framesSentForPresentation |

After resource cleanup, the resize implementation forwards the underlying swapchain resize at **`0xBF55`** and preserves its HRESULT through return at **`0xBF68`**. It tracks the game-visible buffer count separately and passes zero to the underlying swapchain's BufferCount parameter to retain that swapchain's count. [B: `amd_wait_presents.asm`, `amd_swapchain_resize.asm`]

**Limits of the result:** this proves a meaningful AMD drain/release/resize path, not that every PD backend switch reaches it, or that a failed real ResizeBuffers fully restores all prior resources. No rollback or device-removal recovery guarantee is established by these excerpts.

### 20.2 A one-millisecond wait is not a one-millisecond overall timeout

The shared AMD helper **`waitForFenceValue`, `0xF4C30`**, has two modes. Its polling branch repeatedly reads `GetCompletedValue` and loops while below target (`0xF4D35`). Its event branch registers an event, calls `WaitForSingleObject` with **1 ms** at `0xF4DC6`, and retries at `0xF4DE5` until signaled. An optional wait callback is invoked, but its return is not used as a cancellation decision in this helper. No overall deadline is imposed by these loops. [B: `amd_wait_fence_value.asm`]

In particular, `waitForPresents` calls the polling mode and does not propagate the individual boolean results; its final return is true. **Do not infer successful GPU retirement from that return value alone.** The actual fence, requested target, completed value and producer queue must be inspected. This is a static diagnostic observation, not evidence of a reproduced hang or a universal fault in the AMD runtime. [B/I]

Useful stall breakpoints are `0xBB5D` for previous composition, `0xBCE3` for next replacement-buffer reuse, and `0xAB13`/`0xAB32`/`0xAB51` for the three drain stages. Their meanings are now concrete rather than “some wait near Present.”

## 21. Revision 2 — verification, new files and remaining work

### 21.1 What was actually added and checked

The revised package retains all first-pass evidence and adds:

- **40 new annotated disassembly excerpts**, with input code-range hashes and excerpt-file hashes.
- **92 additional instruction records**, all independently decoded again by GNU objdump and compared with the original DLL bytes. Across both passes there are **142 records at 140 distinct addresses**; two addresses intentionally carry overlapping first-/second-pass context.
- **9 new matched-PDB class/inheritance layouts**, with **91 direct data fields**, in addition to the original 20 structures and 122 fields.
- **22 AMD vtable checks**, a machine-readable pipeline map, an expanded CodeView class parser, and a new verification command.
- Regression comparison of the richer parser against all **20 original plain-structure layouts**, rejection of a deliberately altered instruction expectation, and rejection of an unsupported CodeView field kind.

`verification_v2.json` records the checks actually run. It also embeds a fresh passing result for the original input/identity/50-landmark verifier. **PASS means the stored evidence is reproducible against these bytes and PDBs; it is not a runtime compatibility, visual-quality or semantic-completeness certificate.**

```bash
python scripts/verify_v2.py work/extracted \
  --archive /path/to/SkyrimUpscalerAIOBuild18-Hotfix1.7z \
  --output work/verification_v2.json
```

The verifier uses Python's standard library and GNU objdump; it never loads or executes a Windows DLL. The PDB reader is a fail-closed subset for this investigation, not a comprehensive or security-hardened general PDB implementation. The archive includes neither the original DLLs/PDBs nor generated full-module disassembly.

### 21.2 Highest-value implementation implications

A new integration should separate **scene reconstruction**, **UI drawing/composition**, **guide preparation**, and **paced presentation**. AIO18's normal FSR path provides a concrete example of preparing guides before the final AMD presentation callback, while maintaining a separate UI composition contract. Its ReShade effects guard and OMSetRenderTargets redirect show why “just upscale at Present” or “restore one viewport once” is not equivalent behavior. These are implementation implications, not recommendations to link a new plugin to undocumented PD internal classes.

For validation, correlate the source frame ID, active provider, game-buffer index, interpolation-output index, and each relevant fence target separately. Record which conditional hook path executed. Resource pointers without their dimensions, formats, producer/consumer stage and completion values are insufficient evidence of a correct transfer.

The new breakpoints and observations do **not** close the following gaps: SkyrimSE.exe addresses and binary identity; the shared Scaleform flush/call chain; exact ENB and Community Shaders branch ordering; all viewport, scissor and transform assumptions; which mod draws a specific late widget; live ML provider selection; and end-to-end GPU lifetime correctness across failures and backend transitions. None of those is declared solved by static success checks.

### 21.3 Additional primary sources used in revision 2

**V2-S1 — ReShade event semantics.** Official `reshade_events.hpp`: event `reshade_present = 75`, callback after effects/overlay. Current header was consulted as API context; the uploaded package does not pin an independently inspected ReShade DLL.  
https://raw.githubusercontent.com/crosire/reshade/main/include/reshade_events.hpp

**V2-S2 — AMD UI ownership contract.** Current Frame Generation API documentation, particularly `FFX_UI_COMPOSITION_FLAG_ENABLE_INTERNAL_UI_DOUBLE_BUFFERING`. This is documentation context; the packaged AMD 4.0.0.604 DLL, not the latest documentation, is the source for this report's addresses and layouts.  
https://gpuopen.com/manuals/fsr_sdk/techniques/frame-interpolation-api/

**V2-S3 — D3D11/D3D12 shared fences.** Microsoft `ID3D11Device5::OpenSharedFence`.  
https://learn.microsoft.com/en-us/windows/win32/api/d3d11_4/nf-d3d11_4-id3d11device5-opensharedfence

**V2-S4 — CodeView record formats.** Microsoft's public PDB type definitions, including LF_BCLASS, LF_ONEMETHOD, LF_NESTTYPE and LF_INDEX; used to extend the parser without silently truncating class layouts.  
https://raw.githubusercontent.com/microsoft/microsoft-pdb/master/include/cvinfo.h

Primary sources checked on 1 October 2026. No external source substitutes for the archived byte and matching-PDB evidence.

---

## 22. Revision 3 — scope, evidence, and an important correction

This pass follows the host-side producers and D3D11 hooks rather than inferring their behavior from AMD consumer structures. It adds 49 annotated excerpts covering 6,107 instruction lines and 183 selected instruction records. All input identities are unchanged. No game, GPU, Windows DLL, or live debugger was executed.

**Correction to section 10.1:** the original table called common payload `+0x20` an output pointer. The DX12 FSR SR consumer proves that interpretation wrong for this route. It constructs `reactive` from `+0x20`; output is selected from `+0x30`, falling back to `+0x28`. The D3D11 API layer injects the default output at `+0x28` before dispatching the backend. This revision corrects the live table and preserves this explanation of what changed. These are reverse-engineered private layouts, not public ABI declarations. [B: `pd_sr_api_output_injection.asm`, `pd_sr_ffxapi_descriptor.asm`; all new filenames below are under `evidence/v3/disassembly/` unless otherwise specified.]

Do not turn every resource-shaped field into an interchangeable texture pointer. In the DX11 FSR implementation, an intermediate resource bundle is separately assembled and passed as a third argument to the SDK builder. Its `+0x30`/`+0x38` output slots correspond to common payload `+0x28`/`+0x30`, respectively. Its reactive slot is populated through an internal resource path. Treating that intermediate layout as the common PD payload would introduce another offset error. [B: `pd_dx11_sr_payload_expansion.asm`, `pd_dx11_sr_mask_bindings.asm`.]

## 23. Revision 3 — UI preservation involves three D3D11 state domains

### 23.1 Installation evidence

The inspected context installer contains these slot/handler pairs. They agree with Microsoft's ID3D11DeviceContext method order. The installer skips this group when host byte `+0x343` is set; this is not evidence that every renderer/configuration uses these hooks. [B: `host_context_hook_install.asm`; S: V3-S1.]

| Context method | Slot | Host handler RVA |
|---|---:|---:|
| PSSetSamplers | 10 | `0x2A8510` |
| VSSetSamplers | 26 | `0x2A84B0` |
| GSSetSamplers | 32 | `0x2A8450` |
| HSSetSamplers | 61 | `0x2A83F0` |
| DSSetSamplers | 65 | `0x2A8390` |
| CSSetSamplers | 70 | `0x2A8330` |
| PSSetShaderResources | 8 | `0x2A9740` |
| RSSetViewports | 44 | `0x2A95F0` |
| OMSetRenderTargets | 33 | `0x2A9B30`, already traced in v2 |

This extends v2's target-redirection finding: UI preservation also touches rasterizer dimensions and depth sampling. The inspected installation sequence does not add an RSSetScissorRects hook. That is deliberately narrower than claiming the entire program never modifies scissors or transforms.

### 23.2 The normal UI viewport rewrite is conditional

At `SkyrimUpscaler+0x2A95F0`, the normal UI branch requires the UI-phase byte at host-module RVA `0xE81092`, exactly one viewport, and width/height equal to host render dimensions `+0x278/+0x27C`. It substitutes display width/height `+0x270/+0x274`. TopLeftX/Y and MinDepth/MaxDepth are retained. Calls not matching the conditions are forwarded unchanged. [B: `host_rs_set_viewports.asm`, especially `+0x2A968E..+0x2A9724`.]

Conceptual reconstruction, not original source:

```text
if uiPhase && count == 1 && viewport.size == renderSize:
    replacement = viewport
    replacement.size = displaySize
    originalRSSetViewports(context, count, &replacement)
else:
    originalRSSetViewports(context, count, viewports)
```

There is an **earlier special branch** gated by different predicates. It writes render-sized dimensions into one local viewport but forwards the original count. Unlike the normal UI branch, that branch has no local `count == 1` test. A qualifying multi-viewport call would therefore require scrutiny: the API expects an array consistent with the count. No such call was observed in a running game; the special predicates may constrain actual callers. This is a latent contract risk, not a demonstrated crash. [B: same excerpt, `+0x2A9635/+0x2A963D` and shared forward `+0x2A971F/+0x2A9724`; S: V3-S2.]

### 23.3 Depth SRVs are substituted during the UI phase

The PSSetShaderResources hook at `+0x2A9740` checks UI phase, one supplied view, and a non-null view pointer. It calls `ID3D11View::GetResource` at `+0x2A977D`, compares the result against host `+0x7D0` (the original scene-depth resource), and on a match obtains the SRV from the full-size UI-depth wrapper at host `+0x930`. StartSlot and count are preserved. [B: `host_ps_set_shader_resources.asm`; `host_setup_depth.asm` ties the wrapper to the depth-resource path.]

That is not equivalent to simply stretching UI geometry. A shader asking to sample scene depth can be given the UI-specific depth view in this phase. What each late Scaleform or mod widget actually samples still requires a runtime trace.

### 23.4 Static COM ownership defect in that hook

**The entire `+0x2A9740..+0x2A97D6` function has no balancing Release for the queried resource.** Both the matching and nonmatching resource branches discard the pointer obtained at `+0x2A977D`. The replacement-SRV helper receives the UI wrapper, not the queried resource; the forwarded API receives views, not ownership of that extra resource reference. [B: complete `host_ps_set_shader_resources.asm`, checked with GNU and selected LLVM decoding.]

Microsoft specifies that GetResource adds one reference and requires the caller to release it. Under that contract, each qualifying call leaks one resource reference. It can also affect non-depth views because the query precedes the depth comparison. [S: V3-S3.]

**What this does and does not establish:** it is a concrete static lifetime defect under standard D3D11 semantics. It can prevent old resources from being destroyed when their other references are released, including around resizes or switches. It does not prove one new texture allocation per call, a measured VRAM-growth rate, or the cause of a user's crash. The minimal repair concept is to balance the acquired reference on both branches after pointer comparison, preferably with scoped COM ownership; no patch was made here.

## 24. Revision 3 — jitter, motion vectors, and depth-resource provenance

### 24.1 Jitter signs are now recovered end to end for the engine-hook route

The engine-update hook `+0x19FD90` first calls its saved original callee at `+0x19FE8D`. In its local enabled path it gets a phase count, advances a host float counter, converts the index to an integer, and calls PD GetJitterOffset at `+0x19FF0B`. Raw returned jitter `(Jx,Jy)` is consumed as follows:

```text
engine field +0x44 = -2 * Jx / renderWidth
engine field +0x48 = +2 * Jy / renderHeight
host field   +0x08 = -Jx
host field   +0x0C = -Jy
```

The last two are the pixel-jitter values forwarded in the PD evaluation payload. Exact instructions are `+0x19FF2A..+0x19FF79`; numeric constants and the sign mask are byte-pinned in `constants_and_imports.json`. The inactive branch zeros engine jitter and the host pixel-jitter pair. The hook skips the local rewrite in configurations indicated by host `+0x343`. [B: `host_engine_update_jitter.asm`, `host_sr_payload_producer.asm`.]

The separately exported host GetJitterOffset-style bridge at `+0x293DD0` returns utility jitter without this same sign conversion. Do not merge it with the engine-hook route. These signs are a recovered convention, not evidence that one axis is wrong; camera projection and motion-vector conventions must be considered together. [B: `host_api_jitter_offset.asm`.]

### 24.2 PD uses a Halton sequence and truncates its computed phase count

`PD+0xFA390` obtains width values through the API dimension getters. The RenderWidth export independently identifies vtable slot `+0x48`; the getter at slot `+0x58` returns the init record's display-width field `+0x1C`. The helper at `+0xFA2C0` computes, for positive supported dimensions:

```text
phaseCount = trunc(8 * powf(displayWidth / renderWidth, 2))
```

The exact operation is single-precision division/pow/multiply followed by `cvttss2si`; it is not a proof of arbitrary-precision arithmetic. At `+0xFA3E0`, GetJitterOffset clamps the period to at least eight, computes `(phase % period)+1`, then evaluates bases 2 and 3 and subtracts 0.5. [B: `pd_export_GetJitterPhaseCount.asm`, `pd_api_dimension_getters.asm`, `pd_export_GetRenderWidth.asm`, `pd_jitter_phase_formula.asm`, `pd_halton_sequence.asm`, `pd_export_GetJitterOffset.asm`; API vtable in `pd_api_dimension_vtable.json`.]

The reconstruction tests intentionally cover nonnegative phase indices. They do not claim negative counters, zero render width, float-counter exhaustion, or unsupported configurations are safe. AMD's public upscaler documentation is useful for the jitter contract, not as a substitute for the observed conversion instruction. [S: V3-S4.]

### 24.3 Motion vectors have input-sized and display-sized companion resources

The setup function at host `+0x293820` stores the input motion-vector resource at `+0x828`. In the inspected branch it creates a prepared resource wrapper at `+0xBA8` using the incoming texture description and ORs its bind flags with `0x80` (unordered-access capability). A separate wrapper at `+0x880` uses display width/height `+0x270/+0x274`. Creation calls are `+0x2938B4` and `+0x29395D`. [B: `host_setup_motion_vectors.asm`; S: D3D11 header V3-S1.]

The depth setup at `+0x293C30` stores original depth at `+0x7D0`, creates full-size depth and full-size UI-depth wrappers (`+0x8D8/+0x930`), and creates the prepared-depth wrapper at `+0xB50` in the relevant branch. The UI depth-stencil view is cleared with flags 3, depth 1.0 and stencil 0 at `+0x293D5A`. This is a specific clear operation, not proof of the complete depth content supplied later. [B: `host_setup_depth.asm`, `host_depth_resource_helper.asm`.]

### 24.4 Normal FSR FG uses prepared-depth dimensions for guides

The host FG producer `+0x2941A0` obtains the prepared-depth description at `+0x294433`, applies its fallback/validity checks, and writes those width/height values into both render dimensions and motion-vector scale fields. In the normal FSR branch these are not blindly copied from display dimensions. Host SR has its own metadata path; the saved scale values are passed through integer conversions in its producer. [B: `host_fg_payload_producer.asm`, `host_sr_payload_producer.asm`, `host_initial_motion_scale.asm`.]

A late FOV/motion override is gated on DLSS FG or active FrameWarp. Normal FSR skips it. Its fallback FOV scalar is formed from host FOV times display-height/display-width times pi/180. That reconstructs arithmetic, not the exact physical camera model: determining whether the source FOV uses the assumed horizontal/vertical convention requires the matching game camera and a trace. [B: `host_fg_payload_producer.asm`, `host_compute_vertical_fov.asm`.]

## 25. Revision 3 — depth convention changes and distinct SR/FG resets

### 25.1 Reversed depth is observed from projection-related data

The function at host `+0x2967A0` is associated with the diagnostic `ObserveDepthConvention(FrameBuffer)`. It checks its capture/camera prerequisites, rejects unsuitable or nonfinite projection-related inputs, derives depth endpoints, and stores:

| Host offset | Role in inspected path | Store RVA |
|---:|---|---:|
| `+0x2FD` | Depth convention confirmed | `0x296C6C` |
| `+0x2FC` | Reversed-depth boolean | `0x296C73` |
| `+0x2F8` | Near distance | `0x296C79` |
| `+0x2F4` | Far distance | `0x296C81` |
| `+0x2FE` | Latched change of reversed-depth boolean | `0x296C89` |

The latch represents a convention-bit change; it is not set merely because a plane distance drifts. The report does not assign complete Skyrim matrix member names to the source data without the executable contract. [B: `host_observe_depth_convention.asm`.]

### 25.2 A convention change can rebuild SR and reconfigure FG

The SR readiness path at `+0x296550`, called from `+0x2949EB`, refreshes the depth setup, compares cached FG/SR depth conventions, and:

```text
SetFrameGenParams                  host +0x2965EF
InitUpscaler when SR state differs host +0x29665F
restore motion scale exports      host +0x2966F7 / +0x296704
clear convention-changed latch    host +0x29674D
set SR reset request              host +0x296757
```

Failure from FG parameter update or SR initialization stops this readiness path; it does not simply update the cache and continue as if successful. Resource rebinding/refcount handling is present around the recreation. [B: `host_sr_readiness_depth_recreate.asm`.]

The called helper at `+0x266DB0` is now more narrowly identified: it looks up `SubmitFgIdleWideBackground` and submits zero arguments to clear that source state, then writes `-1` to host `+0x11A0`. It must not be renamed a universal temporal-history clear. [B: `host_clear_fg_idle_wide_source.asm`, pinned export-name bytes.]

### 25.3 SR reset is a host request; FG also owns a private latch

SR reads host `+0x264` into payload `+0x54` at `+0x294F8C/+0x294FA0`, calls EvaluateUpscaler, then clears the host request at `+0x294FDD`. The PD export is void; this host clear is not a propagated success acknowledgment from the AMD dispatch. Explicit reset/event handlers and convention changes set the request. [B: `host_sr_payload_producer.asm`, `host_explicit_sr_reset.asm`, `host_reset_event_handler.asm`.]

The normal host FG producer writes its payload reset byte to zero at `+0x2947B6`. **That does not mean FG never resets.** The native PD FSR evaluator combines it with backend byte `+0x1C9`:

```text
prepare.reset = payload.reset || backend.resetPending
```

At PD `+0xF0320`, the enable/disable setter writes the enable state at backend `+0x144`. For API type 1, disabling sets `+0x1C9 = 1` at `+0xF0360`. Re-enabling while a reset is pending takes a guarded path rather than immediately assuming valid interpolation inputs. In Evaluate, the latch survives failed preparation or configuration. It is cleared at `+0xEF759` only after successful Prepare, successful Configure, and enabled state. [B: `pd_fg_disable_reset_latch.asm`, `pd_fg_prepare_reset_consumer.asm`, especially `+0xEF4AD..+0xEF4C0` and `+0xEF730..+0xEF759`.]

This is a useful reliability distinction: FG has a success-dependent reset acknowledgment whereas the inspected SR host request clears after the export returns. It is still unresolved whether every camera cut, load transition or unusual backend switch reaches an appropriate reset producer. AMD also has its own frame-ID/context/scene-change handling; no single host byte is the whole temporal state. [I; AMD per-frame reset context: V3-S5.]

### 25.4 Infinite-far sanitization is not a literal infinity pass-through

The shared PD helper at `+0xEE440` detects FLT_MAX and a positive-infinity classification for the far input. When the near value is positive and has a permitted finite classification, it forms a finite surrogate:

```text
candidate = float(min(near * 1e20, FLT_MAX / 4))
return candidate if candidate > near else originalFar
```

Other cases keep the original far value. Double intermediates and conversion instructions are visible. The FLT_MAX, multiplier and limit bytes are pinned. Native SR and FG callers then place the near/far values into AMD descriptors according to the reversed-depth setting; the reversed path exchanges their descriptor assignments. [B: `pd_far_plane_sanitizer.asm`, `pd_sr_ffxapi_descriptor.asm`, `pd_fg_prepare_reset_consumer.asm`, `pd_math_import_thunks.asm`.]

Do not confuse the host's physical near/far storage with the order AMD receives under reversed depth. No runtime precision/artifact test was performed for this surrogate.

## 26. Revision 3 — SR masks, outputs and sampler quality policy

### 26.1 Exact DX12 FSR descriptor resource map

At PD `+0x100B70`, after the prologue, `RBP = RSP + 0x100`; the descriptor begins at `RSP + 0x60`. Each FfxApiResource occupies 0x30 bytes, consistent with the matched AMD type information and the public declaration. The field order makes the following dataflow explicit. [B: `pd_sr_ffxapi_descriptor.asm`; S: V3-S5.]

| Descriptor offset | AMD resource role | PD common payload source |
|---:|---|---|
| `+0x18` | colour | `+0x08` |
| `+0x48` | depth | `+0x18` |
| `+0x78` | motion vectors | `+0x10` |
| `+0xA8` | exposure | null |
| `+0xD8` | reactive | `+0x20` |
| `+0x108` | transparency/composition | null |
| `+0x138` | output | `+0x30` when non-null, otherwise `+0x28` |

The output selection is at `+0x100BCF/+0x100C0B`; the reactive conversion at `+0x100CC4` is stored into the descriptor at `+0x100CD7`. PD's API layer writes its default output pointer to payload `+0x28` at `+0xF5D96`.

Consequently, the presence of resource-slot names in a DLL does **not** establish that external exposure or a transparency/composition mask is actually bound. In the inspected DX12 builder both are null. The native DX11 builder separately uses null input exposure and null T&C; its diagnostic strings identify those conversions. Null external exposure is not automatically a defect: internal/auto-exposure settings must be considered separately. Reactive masks have a distinct resource path, and this pass does not certify their per-pixel semantic correctness. [B: `pd_dx11_sr_mask_bindings.asm`.]

### 26.2 Sampler bias is a cached, conditional rewrite across six shader stages

The six sampler setters call host helper `+0x2A8570`, then the saved original setter. The helper copies the pointer array, checks its caches, and for new eligible candidates reads D3D11_SAMPLER_DESC. The positive path requires original MipLODBias equal to zero and MaxAnisotropy greater than one; other candidates go to the exclusion/cache path. It updates the bias field, creates a replacement sampler at `+0x2A88FF`, caches it, and substitutes the pointer. Additional menu/context predicates are present. [B: `host_sampler_replacement.asm` and the six `host_*_set_samplers.asm` wrappers; S: V3-S6.]

This is **not** a unconditional override of every sampler, and MaxAnisotropy > 1 is not itself a recovered test of the Filter enum. The cache is invalidated when active bias changes (`+0x2A85AF`, `+0x2A8627/+0x2A8633`). Host `+0x2E8` stores the computed bias, `+0x2EC` the active bias. The producer clamps the scalar result to [-3,+3]. [B: `host_render_size_and_bias_update.asm`, constant bytes.]

The scalar math thunk called before clamping was not conclusively resolved through this host's original/rebuilt import arrangement. **No exact log2 formula is claimed in v3.** A rebuilt import named log2f is insufficient to prove the target of a separate original thunk. This keeps a plausible interpretation from becoming a false recovered fact.

## 27. Revision 3 — reproducibility, follow-up trace targets and limitations

### 27.1 What was verified afresh

The v3 verifier runs v2, which runs v1, against the newly extracted original archive. It then checks all new instruction bytes and exact excerpt-file/code-range hashes. GNU objdump redecodes every new excerpt instruction line; LLVM independently agrees on selected landmark boundaries and bytes. These checks confirm repeatable evidence, not all inferred semantics.

| New v3 verification | Count/result |
|---|---:|
| Selected instruction records | 183 |
| Selected landmarks independently decoded with GNU | 183 |
| LLVM landmark instruction-boundary/byte agreement | 183 |
| New excerpt file and source-range hashes | 49 |
| New excerpt instruction lines redecoded with GNU | 6,107 |
| Pinned numeric/string records | 11 |
| PD API vtable pointer checks | 34 |
| Pure Python reference-contract tests | 16, passed |
| Deliberately corrupted expected instruction rejected | yes |

Cumulative evidence contains **128 disassembly excerpts and 325 instruction records at 316 distinct addresses**. Overlapping excerpts and deliberately repeated earlier landmarks are not counted as distinct functions. Earlier PDB identities, 20 structure layouts and nine additional class layouts are retained and rechecked through the old verifiers.

`reference_models_v3.py` tests jitter-period arithmetic, normal viewport predicates, output/reactive field separation and the FG-reset acknowledgment conditions. These are transparent executable reconstructions, **not** calls into PD/AMD binaries. No native tests passed because no native workload was run.

Reproduction and new files are documented in README.md. `RUNTIME_BREAKPOINTS_v3.md` records the next live experiments with the exact module RVAs. The ZIP contains no original DLLs, PDBs, game executables, shader binaries or font files.

### 27.2 What this pass still does not prove

The missing engine executable and runtime observations remain important. This work does not identify the final shared Scaleform flush, attribute particular widgets to TrueHUD/BTPS, reconstruct all matrix/scissor paths, observe live resource dimensions or reset frequency, select the GPU's actual AMD provider, or validate visual quality and transition stability under ENB/ReShade.

The highest-value new experiment is to count GetResource calls and releases around the UI hook, then trigger a controlled resize while tracking the affected texture identities. In parallel, record host SR reset, PD FG resetPending, prepared-depth dimensions and camera metadata through a camera cut or loading transition. This distinguishes a proven static reference imbalance from a measured failure and a reset implementation from actual coverage of all transitions.

### 27.3 Additional primary sources used in v3

Accessed 1 October 2026. These identify public contracts; bundled build identities and private offsets come from this archive.

- **V3-S1:** Microsoft win32metadata, generated d3d11.h, ID3D11DeviceContext method order and interface declarations: https://raw.githubusercontent.com/microsoft/win32metadata/main/generation/WinSDK/RecompiledIdlHeaders/um/d3d11.h
- **V3-S2:** Microsoft RSSetViewports array/count contract: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-rssetviewports
- **V3-S3:** Microsoft ID3D11View::GetResource acquired-reference/Release contract: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11view-getresource
- **V3-S4:** AMD super-resolution upscaler jitter/input documentation: https://gpuopen.com/manuals/fidelityfx_sdk/techniques/super-resolution-upscaler/
- **V3-S5:** AMD FidelityFX API upscaling descriptor declaration and per-frame FG configuration: https://gpuopen.com/manuals/fidelityfx_sdk/techniques/super-resolution-interpolation/
- **V3-S6:** Microsoft D3D11_SAMPLER_DESC field semantics: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_sampler_desc

## 28. Revision 4 — host chronology: ReShade brackets, UI routing, scissor coverage, and ENB presence

Revision 4 deliberately stays on the host side. The goal was to close the gap between the already-recovered SR/FG backend work and the exact state-management chronology around UI and ReShade. No game executable or runtime capture was added, so conclusions below distinguish installed hooks and binary control flow from live rendering behavior.

### 28.1 The context hook set still has no RSSetScissorRects replacement

The D3D11 immediate-context installation path at `SkyrimUpscaler+0x19EC61..0x19EE02` installs hooks at slots including:

- slot `8` -> `+0x2A9740` (`PSSetShaderResources` path already analyzed in v3),
- slot `33` -> `+0x2A9B30` (`OMSetRenderTargets`),
- slot `44` -> `+0x2A95F0` (`RSSetViewports`).

There is **no slot 45 hook** in this installer path. In the D3D11 context vtable, slot 45 is `RSSetScissorRects`. The archived installer excerpt is `evidence/v4/disassembly/context_hook_install.asm`.

A complete search of this host disassembly finds two direct calls through vtable offset `+0x168`, the byte offset corresponding to slot 45. Both are in the bundled Dear ImGui DX11 renderer around `+0x21350E` and `+0x2135AC`: one installs draw-command clip rectangles and the other restores the saved scissor state. That local renderer also restores viewport/rasterizer state afterward. [B: `imgui_scissor_calls.asm`.]

**Static conclusion:** AIO18 has an explicit general viewport rewrite but no equivalent general scissor rewrite in the recovered immediate-context hook set. This does not prove Skyrim/Scaleform always needs a scissor transform: a caller may already submit display-space scissors, may disable scissoring, or may set compatible rectangles later. It does mean an independent reduced-resolution implementation must not assume that AIO18 demonstrates a complete viewport+scissor transform contract. The scissor state remains a runtime verification target.

### 28.2 ReShade registers six relevant callbacks with exact event IDs

The registration routine at `+0x284A70` resolves `ReShadeRegisterEvent` lazily and registers the following callbacks:

| Event ID | Callback RVA | Recovered public event |
|---:|---:|---|
| `9` | `+0x285050` | `init_effect_runtime` |
| `10` | `+0x284F50` | `destroy_effect_runtime` |
| `0x56` / 86 | `+0x284F10` | `reshade_open_overlay` |
| `0x4C` / 76 | `+0x284EC0` | `reshade_begin_effects` |
| `0x4D` / 77 | `+0x284E70` | `reshade_finish_effects` |
| `0x4B` / 75 | `+0x284D30` | `reshade_present` |

The event identities match the public ReShade `addon_event` enum for the inspected header generation. [B: `reshade_event_registration.asm`; S: V4-S1.]

This materially sharpens the v2 “final overlay” finding: `+0x284D30` is not merely a generic present callback. It is explicitly `reshade_present`, which ReShade documents as running **after its overlay has rendered**.

### 28.3 ReShade effects temporarily suspend AIO18's two routing flags

The begin/finish callbacks form a matched save/clear/restore pair for global bytes `0x180E81091` and `0x180E81092`:

```text
reshade_begin_effects (+0x284EC0)
    if runtime == tracked runtime:
        savedA = routeA
        savedB = uiPhase
        inEffects = true
        routeA = false
        uiPhase = false

reshade_finish_effects (+0x284E70)
    if runtime == tracked runtime && inEffects:
        routeA = savedA
        uiPhase = savedB
        inEffects = false
```

The `uiPhase` identity for `0x180E81092` is strongly supported by its direct use in the v3 viewport and depth-SRV hooks and by the pre-UI transition stores. The exact semantic name of `0x180E81091` is still inferred; its consumers show that it participates in ReShade/overlay render-target routing. [B: `reshade_callbacks.asm`, `reshade_omsetrt_routing.asm`, earlier v3 hook excerpts.]

**Chronology consequence:** while ReShade's own effect passes execute, AIO18 deliberately disables the state that would otherwise redirect UI-phase viewport/depth/render-target behavior. It then restores that state immediately after effects. This is stronger evidence than treating ReShade as simply “before SR” or “after SR”: AIO18 also protects ReShade's internal effect execution from its UI redirection hooks.

### 28.4 `reshade_present` is the final overlay completion point for deferred submissions

The `reshade_present` callback at `+0x284D30` first verifies the callback belongs to the tracked runtime and that the pending-final-source byte is set. It clears that pending byte, resolves the active backbuffer entry through the runtime, and calls the host FG/direct-submission function at `+0x2941A0`. It then records that the final-overlay callback has been consumed. [B: `reshade_callbacks.asm`.]

This keeps the earlier distinction intact:

```text
normal FSR path:
    prepare temporal/FG inputs earlier
    -> AMD swapchain Present later performs interpolation

paths requiring the final ReShade-composited source:
    mark a source pending
    -> ReShade renders effects/overlay
    -> reshade_present callback
    -> host submits the completed source
```

AIO18 also has an `AfterPresent` diagnostic when a pending source was not consumed by this callback. The static code therefore treats the final-overlay callback as a real ordering contract, not merely logging decoration.

### 28.5 The OMSetRenderTargets ReShade wrapper explicitly avoids redirection during effect execution

The wrapper beginning at `+0x2A97E0` reads the `inEffects` flag before applying ReShade-specific target substitution. When it is processing the tracked context outside the bracketed effect pass, it can identify a render target belonging to the tracked ReShade runtime/backbuffer set and substitute either the UI temporary color resource or a selected backbuffer resource. During the bracketed effects phase, it falls through to the underlying path instead. [B: `reshade_omsetrt_routing.asm`.]

This supplies the missing connection between the event callbacks and the D3D11 hooks: the save/clear/restore pair is not isolated state. It directly gates later render-target redirection.

## 29. Revision 4 — UI phase transitions and a special StatsMenu suspension

### 29.1 The normal pre-UI path turns the UI phase on only after selecting UI targets

The tail of the main pre-UI handler selects a color/depth target, binds it through the context at `+0x19FCF3`, and then writes `1` to the global UI-phase byte at `+0x19FD00`. Subsequent viewport and depth-SRV hooks therefore observe UI phase only **after** the target switch on this path. [B: `preui_ui_phase_transition.asm`.]

That ordering is useful for reproduction:

```text
scene rendering
 -> optional controlled ReShade-before-SR work
 -> SR dispatch
 -> optional controlled ReShade-after-SR work
 -> choose/bind UI color + depth target
 -> set uiPhase = true
 -> later UI draws are eligible for viewport/depth/RT substitution
```

The static code still does not identify the final shared Scaleform flush or guarantee every third-party widget occurs inside this phase.

### 29.2 StatsMenu deliberately suspends the UI phase around its scene render

The installer contains a separately identified `StatsMenu_RenderScene` hook. Its thunk at `+0x1A07F0` checks the relevant feature state, clears `uiPhase` at `+0x1A0827`, calls the saved original StatsMenu render function, then invokes host post-processing at `+0x1A0000`. That post-processing path can bind the UI target and re-enable `uiPhase` at `+0x1A00BF`. [B: `statsmenu_ui_phase_suspend.asm`; the installer diagnostic identifies the hook as `UpscalerHooks::StatsMenu_RenderScene::Install`.]

This is direct evidence that AIO18 does **not** model “UI” as one uninterrupted final phase. A menu with its own 3D scene can temporarily leave UI-space state, render a reduced/scene-space pass, then return to the UI target. Any independent implementation that only flips a single global `after_scene = true` flag once per frame would miss this class of nested render sequence.

## 30. Revision 4 — what ENB integration is and is not visible statically

The only named ENB SDK symbol string present in `SkyrimUpscaler.dll` is `ENBGetSDKVersion`. The small helper at `+0x299750` performs:

```text
GetModuleHandle("d3d11.dll")
GetProcAddress(module, "ENBGetSDKVersion")
return proc != nullptr
```

The larger environment-detection routine uses the same probe while deciding Community Shaders / reverse-Z behavior. [B: `enb_presence_probe.asm`; full-string inventory.]

No additional named ENB SDK callback or ENB-specific render-stage export was found in the host string inventory. Therefore the current static evidence supports **ENB presence detection**, not an explicit ENB callback-driven render chronology inside this DLL. Actual ordering can still arise through the D3D11 proxy/interposition chain and the saved original calls. Dynamic symbol construction or undocumented ordinal-based access cannot be ruled out solely from strings, but no such path has been established here.

For RazKolbas this means the next live experiment should record call stacks around the host pre-UI hook and D3D11 target binds with ENB both enabled and disabled, rather than assuming there is a hidden ENB SDK stage that can be copied.

## 31. Revision 4 — implementation implications and remaining runtime questions

The strongest new implementation implications are:

1. **Treat viewport and scissor as separate contracts.** AIO18 proves an explicit viewport rewrite, but not a global scissor rewrite.
2. **Bracket ReShade effect execution.** UI/overlay target routing is suspended during `reshade_begin_effects` and restored at `reshade_finish_effects`.
3. **Use `reshade_present` only for sources that truly require final ReShade overlay inclusion.** It is after the overlay, not merely after effect techniques.
4. **Support nested scene/UI transitions.** StatsMenu demonstrates that a menu can suspend UI-space behavior for an embedded scene and then restore it.
5. **Do not model ENB as an explicit SDK callback without runtime evidence.** In this build, the named ENB integration recovered statically is a presence probe.

Still unresolved after pass 4:

- exact SkyrimSE.exe callers and RVAs for the normal pre-UI hook on each supported runtime;
- the final Scaleform/shared UI flush and exact late-widget submission order;
- live scissor values before/after SR and whether unmodified scissors are actually incorrect in affected UI paths;
- ENB call-stack placement relative to the saved original call, host SR dispatch and target transition;
- which third-party widgets are submitted after the main UI phase or through independent contexts;
- whether all ReShade versions used in the field preserve the same event ordering expected by this build.

### 31.1 Additional primary source used in v4

- **V4-S1:** ReShade public `addon_event` definitions, including `reshade_present = 75`, `reshade_begin_effects`, `reshade_finish_effects`, runtime lifecycle and overlay events: https://raw.githubusercontent.com/crosire/reshade/main/include/reshade_events.hpp

## 32. Revision 5 — Community Shaders temporal-input bridge

Revision 5 focuses on where FSR's temporal guides actually come from. The key result is that AIO18 does not rely exclusively on the generic D3D11 resource-detection hooks when Community Shaders (CS) is present. It installs a dedicated live-postprocessing bridge, prepares CS-specific inputs, and converges those inputs into the same prepared depth/motion pair later consumed by both SR and FG.

### 32.1 A live-postprocessing bridge calls `PrepareCSUpscaleInput`

The host contains source-path/function diagnostics for `CSUpscalingBridge::Install`, `SkyrimUpscaler::PrepareCSUpscaleInput`, `SkyrimUpscaler::CaptureCSDepthAndMotion(bool)`, and `SkyrimUpscaler::FinishCSUpscaleFrame`. Its installation diagnostic states that the bridge is installed at **live postprocessing**, with a render-rect NR path while native depth/HDR/postprocessing are retained. These source strings are provenance clues, not symbols recovered from a public PDB. [B: full host string inventory; `evidence/v5/disassembly/cs_bridge_install.asm`.]

The installed runtime hook is at host `+0x19DCC0`. In two inspected branches it calls host `+0x2654B0` at:

```text
SkyrimUpscaler +0x19DD14 -> +0x2654B0
SkyrimUpscaler +0x19DDC4 -> +0x2654B0
```

The `+0x2654B0` body matches the embedded function-name diagnostic `bool __cdecl SkyrimUpscaler::PrepareCSUpscaleInput(void)`. Within it, `+0x2656F5` calls `+0x2A15B0` with `DL=1`, matching the embedded `CaptureCSDepthAndMotion(bool)` name. The function then requires both prepared guide resources before proceeding. [B: `cs_bridge_runtime.asm`, `cs_prepare_input.asm`.]

A conservative static chronology is therefore:

```text
Community Shaders live-postprocessing boundary
    -> AIO18 CS bridge +0x19DCC0
    -> PrepareCSUpscaleInput +0x2654B0
    -> CaptureCSDepthAndMotion(true) +0x2A15B0
    -> prepared depth  host +0xB50
    -> prepared motion host +0xBA8
    -> later normal SR / FG producers
```

This establishes the host-side convergence point. It does **not** establish the exact Community Shaders source function/RVA without the matching CS binary, nor does it prove the hook fires in every CS version.

### 32.2 `CaptureCSDepthAndMotion` validates, allocates and shader-processes the guides

The capture routine begins by requiring the relevant D3D11 objects and a motion-vector source at host `+0x828`. Its depth candidate comes from helper `+0x2A4410`, with host `+0x7D0` as a fallback. Render dimensions are read from host `+0x34C/+0x350`; when those are unavailable, the routine falls back to display dimensions `+0x270/+0x274`. [B: `cs_capture_depth_motion.asm`.]

The routine obtains source texture descriptions and rejects sources smaller than the desired render rectangle. The prepared resources are stored/reused at:

```text
host +0xB50 = prepared depth
host +0xBA8 = prepared motion vectors
```

The function invokes the draw-based helper at host `+0x2A1240` from `+0x2A1D74` and `+0x2A20E5`. The helper saves a broad set of D3D11 pipeline state, updates a constant buffer, binds shaders/resources/samplers and a render target, issues a draw, and restores the captured state. Thus the CS guides are **not merely pointer aliases or unconditional CopyResource results**; at least some capture branches run them through an AIO18 shader transform. [B: `cs_transform_helper.asm`, `cs_capture_depth_motion.asm`.]

Embedded shader-reflection strings in this host include `jitterOffset`, `dynamicResScale`, `screenSize`, and, on the depth side, `depthTex` / `SV_DEPTH`. No DXBC decompilation is claimed here: those names show reflected interface metadata but do not recover the shader equations.

### 32.3 The capture stores source-to-render scale factors

The desired width/height are loaded from host `+0x34C/+0x350` (or the display fallback), converted to floats, and divided by the relevant source-description dimensions before being stored at host `+0x1618/+0x161C`. The same two host fields are updated on an alternate capture branch. [B: `cs_capture_depth_motion.asm`, stores `+0x2A1B0E/+0x2A1B4E` and `+0x2A2043/+0x2A205C`.]

The safest recovered semantics are **capture scale factors used while conforming CS inputs to AIO18's render rectangle**. They should not be conflated with the FidelityFX context's dynamic-resolution flag, which is a separate API contract discussed in section 33.3.

## 33. Revision 5 — generic fallback capture and FSR mask/exposure policy

### 33.1 Generic resource detection remains available alongside the CS bridge

The host's D3D11 resource-detection path around `+0x2A8990` still provides a fallback mechanism. When the motion-vector slot is empty and a candidate meets the inspected format/shape conditions, it calls `SetupMotionVector` at host `+0x293820` (`+0x2A8AAB`) and logs `Motion Vertor Found : {} x {}`. For depth-compatible format ranges and matching render dimensions, when host `+0x7D0` is empty it calls `SetupDepth` at host `+0x293C30` (`+0x2A8BA4`) and logs `Depth Buffer Found : {} x {}`. [B: `generic_resource_detection.asm`.]

This means the CS path is a specialization, not a replacement for every generic input-discovery mechanism. AIO18 can obtain raw candidates through the generic hooks and, under the CS route, normalize/capture them into the shared prepared resources.

### 33.2 The normal Skyrim SR producer explicitly supplies no external reactive mask

In the main host SR producer at `+0x2949C0`, the temporary payload begins at local `[rbp-0x20]`. At `+0x294F09`, `[rbp+0x00]` is written from zeroed `R15`; relative to the payload base this is **payload `+0x20`**. Prior consumer-side evidence identifies common payload `+0x20` as the FSR reactive input in both the native DX11 and FFX API SR routes. [B: `host_sr_temporal_payload.asm`, `pd_dx11_sr_mask_bindings.asm`, earlier v3 FFX API descriptor evidence.]

Therefore, on this normal host producer path:

```text
external reactive mask = null
```

The native DX11 builder independently passes a null external exposure resource (`PD +0xFF096`) and a null transparency/composition resource (`PD +0xFF2DD`). The FFX API SR builder from v3 likewise leaves external exposure and T&C null.

AMD documents exposure, reactive, and transparency/composition as optional dispatch resources. It also states that without a reactive mask the upscaler falls back to its internal shading-change detection, while supplying a reactive mask is recommended for optimal handling of pixels such as particles/alpha-blended content. [S: V5-S1, V5-S2.]

Static conclusion: **null reactive/T&C is intentional dataflow in the inspected producer, not a failure to find a hidden pointer.** It is nevertheless a quality trade-off; an independent implementation need not reproduce that omission if Skyrim-specific masks can be generated correctly.

### 33.3 Null external exposure is consistent with AIO18's DX11 context flags

The native DX11 FSR context builder chooses `0x220`, or `0x228` when its depth-inverted byte is set, then conditionally ORs bit 0 and bit 1. [B: `pd_dx11_sr_context_flags.asm`.]

For the bundled FSR3 upscaler flag definitions, the relevant public bits are:

```text
bit 3 = depth inverted
bit 5 = auto exposure
bit 6 = dynamic resolution
bit 9 = RCAS lower-limiter compensation
```

Thus base `0x220` contains **bit 5 + bit 9**, and `0x228` additionally contains **bit 3**. This makes the null external exposure resource coherent with the explicit auto-exposure policy. AMD recommends its auto-exposure mode unless the application has a reason to supply a different exposure path. [S: V5-S2, V5-S3.]

The same inspected DX11 context creation does **not** set bit 6. This is separate from the CS shader's `dynamicResScale` reflected constant and the host capture scale factors. Static evidence therefore shows AIO18 doing its own input conformance/scaling work without setting the FSR3 DX11 context's dynamic-resolution bit on this path. Whether render dimensions actually vary while the context remains alive must be measured before treating this as a compatibility defect.

The FFX API/DX12 context builder has the same `0x220/0x228` base and conditional bits 0/1, and additionally sets bit 7 (`bts eax,0x7`). The exact semantic compatibility of that bit belongs to the selected FFX API provider/version; v5 does not project the legacy DX11 enum blindly onto every newer provider. [B: `pd_ffxapi_context_flags.asm`.]

## 34. Revision 5 — SR and FG share the same prepared depth/motion pair

This pass connects the producer and both consumers directly.

### 34.1 Super resolution consumes host `+0xB50/+0xBA8`

The normal SR producer uses prepared depth from host `+0xB50` and prepared motion vectors from `+0xBA8`. In one branch it copies raw host resources into these prepared destinations through the D3D11 context; later, the payload assembly loads `+0xB50` at `+0x294CF5` (unless the depth-suppression condition at host `+0x772` is active) and loads `+0xBA8` at `+0x294D03`. It then calls the PD `EvaluateUpscaler` import at `+0x294FBD`. [B: `host_sr_temporal_payload.asm`.]

### 34.2 Frame generation requires and consumes the same resources

The FG producer at host `+0x2941A0` requires non-null prepared depth (`+0xB50`) at `+0x29438E` and prepared motion (`+0xBA8`) at `+0x29439E`. Payload construction loads depth at `+0x2944D9` (again conditionally suppressible) and motion at `+0x2944E7`; the same prepared depth is referenced later in the function as well. [B: `host_fg_temporal_payload.asm`.]

Consequently the recovered guide graph is:

```text
vanilla/generic detection -----------+
                                      |
Community Shaders capture/transform -+-> host prepared depth +0xB50
                                      +-> host prepared motion +0xBA8
                                                     |
                              +----------------------+------------------+
                              |                                         |
                       SR payload +0x2949C0                    FG payload +0x2941A0
                              |                                         |
                        FSR SR backend                            FSR FG Prepare
```

This is an important implementation boundary: **SR and FG are separate algorithms/backends, but AIO18 intentionally gives them a common host-side temporal-guide normalization layer.** Maintaining separate, independently guessed depth/MV acquisition paths for SR and FG would diverge from this design.

## 35. Revision 5 — reset coverage: multiple writers, incomplete event attribution

Revision 3 recovered the distinct SR and FG reset mechanisms. This pass surveyed additional stores to the host SR reset byte at `+0x264`. Multiple writer sites exist, including the already-understood readiness/depth-convention path and several functions in the `+0x27D.../+0x27E.../+0x27F...` ranges. [B: `sr_reset_writer_survey.asm`.]

The nearby menu-event process toggles state based on menu names, but the inspected excerpt does **not** directly establish that it writes `+0x264`. It is retained in the archive as a negative/coverage control rather than being relabeled as a camera-cut reset source. [B: `menu_event_negative_control.asm`.]

Therefore v5 deliberately does **not** claim a complete mapping such as "every loading screen/camera cut/menu transition sets SR reset." Static evidence proves multiple reset producers and the previously recovered depth-convention producer; the semantic attribution of several remaining writers needs either deeper call-graph work with SkyrimSE.exe or a live trace through known transitions.

For runtime validation, log independently:

```text
host SR reset          host +0x264
PD FSR-FG resetPending backend +0x1C9
prepared depth         host +0xB50
prepared motion        host +0xBA8
capture scales         host +0x1618/+0x161C
render dimensions      host +0x34C/+0x350
raw depth/MV           host +0x7D0/+0x828
```

Then trigger a camera cut, loading transition, menu with embedded 3D, resize, and CS enable/disable transition. This will show reset coverage rather than merely reset capability.

## 36. Revision 5 — implementation implications, verification and remaining boundary

### 36.1 Implementation implications

The strongest implications from pass 5 are:

1. **Use one temporal-guide normalization layer for SR and FG.** AIO18 converges generic and CS-specific capture into prepared depth/MV resources shared by both consumers.
2. **Treat Community Shaders as an explicit integration boundary.** AIO18 hooks its live-postprocessing chronology and shader-processes/crops guides instead of assuming the generic D3D11 candidates are already in the exact desired domain.
3. **Do not blindly copy AIO18's null reactive/T&C policy.** It is a verified behavior, but AMD's public guidance says a reactive mask materially improves cases that color/depth/motion alone cannot identify.
4. **Null exposure is not itself a bug here.** The DX11 context explicitly enables FSR auto exposure.
5. **Do not confuse CS `dynamicResScale` with FidelityFX dynamic-resolution enablement.** They are separate contracts; bit 6 is absent from the inspected DX11 FSR context flags.
6. **Keep reset mechanisms independently observable.** SR reset acknowledgment and FG resetPending acknowledgment have different ownership and success semantics.

### 36.2 Primary sources added in v5

Accessed 1 October 2026. These documents define public AMD contracts; all AIO18 offsets/field dataflow above come from the supplied archive.

- **V5-S1:** AMD `FfxFsr3UpscalerDispatchDescription`: exposure, reactive and transparency/composition are optional dispatch resources: https://gpuopen.com/manuals/fidelityfx_sdk/reference_documentation/structs/ffx_fsr3_upscaler_dispatch_description/
- **V5-S2:** AMD FSR3 upscaler integration documentation: reactive-mask fallback/quality guidance and optional exposure with auto exposure: https://gpuopen.com/manuals/fidelityfx_sdk/techniques/super-resolution-upscaler/
- **V5-S3:** AMD FSR3 upscaler initialization flags, including depth-inverted, auto-exposure, dynamic-resolution and RCAS bits: https://gpuopen.com/manuals/fidelityfx_sdk/reference_documentation/sdk/effect_components/fidelityfx_fsr3/ffx_fsr3_upscaler/

### 36.3 What is still unresolved after five static passes

The static architecture is now broad enough that additional blind disassembly has diminishing returns. The highest-value unresolved questions are live contracts:

- which CS versions/layouts actually take each bridge branch;
- actual source/prepared depth and motion formats/dimensions each frame;
- whether the FSR context remains alive across changing render dimensions while the dynamic-resolution flag is clear;
- visual impact of the null reactive/T&C resources in Skyrim transparency, particles, water, vegetation and animated materials;
- reset coverage across camera cuts, loads and nested menu scenes;
- live FSR provider selection, camera metadata and fence/resource lifetimes already identified in earlier revisions;
- final Scaleform/late-widget ordering and the unmodified general scissor path from revision 4.

`RUNTIME_BREAKPOINTS_v5.md` turns these into concrete experiments. Fresh `verification_v5.json` checked the exact archive hash, both principal module hashes, **38 instruction-byte landmarks**, **11 embedded strings**, **13 curated v5 disassembly files**, and **8 transparent reference-model tests**; all checks passed. The distribution scan also found no `.dll`, `.exe`, or `.pdb` files in the evidence package. No game, Windows DLL or GPU workload was executed in pass 5.
