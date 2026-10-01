# Skyrim Upscaler AIO18 Hotfix1 — FSR SR and frame-generation reverse engineering

**Analysis date:** 1 October 2026  
**Revision:** 2 — expanded host/UI, synchronization and resize analysis  
**Exact input:** `SkyrimUpscalerAIOBuild18-Hotfix1(1).7z`  
**Scope:** fresh, read-only static analysis of the supplied archive. No game, Windows DLL, or GPU workload was executed. No binary was patched.

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
| `+0x20` | Output resource pointer |
| `+0x28`, `+0x30` | Optional SR inputs; roles vary by backend path; preserve uncertainty |
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
