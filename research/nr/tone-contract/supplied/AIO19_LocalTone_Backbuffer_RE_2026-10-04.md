# AIO19 Local Tone / Backbuffer — targeted reverse engineering

**Build:** Skyrim Upscaler AIO Build 19 Hotfix 1  
**Date:** 2026-10-04  
**Purpose:** investigate Local Tone color/brightness drift during camera motion, following the user's negative AutoExposure-off experiment.  
**Method:** fresh static x64 disassembly, PE64 mapping, control/data-flow inspection, and byte verification against the original archive. No target DLL, Skyrim, or GPU workload was executed.  
**Status:** concrete integration facts recovered; the cause of the gameplay drift is **not yet established**.

## 1. Findings that change the earlier advice

The prior Local Tone report overranked both AutoExposure and a missing original-scene Backbuffer. This report supersedes those causal rankings and its proposed “pre-NR reference is AIO parity” premise.

**AIO19 does not universally supply a separate pre-NR luminance-reference Backbuffer.** Its mapped local route initially selects the local NR output resource as Backbuffer. An alternate condition can select the mapped input. The downstream evaluation builder again has an Output fallback. Separately, that builder can initialize Output by copying Color before NR evaluation. These are concrete implementation differences worth testing together and independently; neither proves why Local Tone drifts. [E3–E6]

**AIO19's final NGX scalar setter is straightforward, but the whole chain is not just an INI-float passthrough.** A conditional control-mask route packages pass controls into mask-building data and overwrites scalar Tone, Structure, and Intensity with `1.0` before NGX. A scalar-only Dva path and an AIO mask path can therefore have different effective inputs even when menu values look matched. The exact live branch and mask-channel semantics remain unmeasured. [E1, E2, E5, E7]

**The bundled NR runtime can override controls after reading the NGX parameter object.** It invokes a runtime-parameter callback; when a returned Local Tone entry is present, it overwrites the parsed tone. It subsequently applies selected configuration coefficients and checks for control changes before invoking its network manager. Therefore, the value in Dva's overlay or NGX setter is not by itself evidence of the final control state inside AIO's bundled runtime. Callback activity in either game process is unconfirmed. [E8–E13]

**Local Tone reaches both neural-network argument preparation and an optional tone-scaled configuration block.** It is not established as merely generic DLSS AutoExposure or a standalone external brightness blend. The network's GPU/device-code equations were not reconstructed in this pass. [E12–E14]

## 2. Exact identities and address convention

All addresses below are **RVAs**, not file offsets. Each named PE has preferred image base `0x180000000`; under ASLR use the actual loaded module base plus the RVA. Do not reuse an NR RVA for Dva's different `e67dee…` DLL without independently matching its code.

| Input | Bytes | SHA-256 |
|---|---:|---|
| `SkyrimUpscalerAIOBuild19-Hotfix1(1).7z` | 204,042,400 | `49e7f7dabf426937915d1aeed664fc40a7cc7d89f42092a69c205b22c4687439` |
| `SkyrimUpscaler.dll` | 15,975,424 | `ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a` |
| `PDPerfPlugin.dll` | 20,332,032 | `ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435` |
| `nvngx_dlssnr.dll` | 165,840,496 | `8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206` |
| Shipped `SkyrimUpscaler.ini` | 15,300 | `ad0b46779903a9fec921a3fedb4f03c84eb393fd646ba889e5875608a6979967` |

The source comparison is pinned to Dva `codex/nr` commit `e72452884ede7d1de48eb3d8a20fba32991e9024`. `Stage.cpp` blob is `e051292e2864dcb70b4a57475c867776e3d333f1`; `PreparedBeforeUpscale.cpp` blob is `8d146c04311c5b780d4180377430e938c85fde95`. This report does not claim a full review of that repository revision. [S1, S2]

## 3. The Local Tone data path

### 3.1 Chain storage, validation, and resolution

The version-1 chain packet has a pass table at `+0x17C`, with stride `0x68`. The selected pass's Tone is at **pass `+0x10`**, hence first-pass packet **`+0x18C`**.

At `PD+0x10F9E4`, the validator loads that float. The following comparisons at `+0x10F9FF` and `+0x10FA08` bound it to `0..1`; the adjacent floating-point classification call rejects non-finite inputs. This mapping agrees with the shipped `[NR PASS 1] Tone` range. [E1, E16]

The chain is copied into backend storage at `backend+0x3240` (`PD+0xA6104`). In the chain evaluator:

```text
PD+A3802: load selected pass Tone from backend + passIndex*0x68 + 0x33CC
PD+A380B: store it into evaluation payload +0xEC
```

The relationship is `0x3240 + 0x17C + 0x10 = 0x33CC`. [E2]

### 3.2 Important conditional override: the control-mask route

The selected pass value is not necessarily the final scalar sent to NGX.

At `PD+0xA38E4..0xA3900`, resource/state checks can bypass or enter additional control-mask preparation. On the entered route, the pass controls are copied into preparation data:

```text
PD+A3918: Intensity -> preparation data
PD+A3920: Tone      -> preparation data
PD+A3928: Structure -> preparation data
```

After that route's resource/preparation work, the evaluator writes:

```text
PD+A3F97: scalar Structure = 1.0
PD+A3FA1: scalar Tone      = 1.0
PD+A3FAB: scalar Intensity = 1.0
PD+A3FB5: AutoMask byte    = 0
```

The following call at `PD+0xA40A8` enters the evaluation builder with a control resource. This establishes that the mask route is not equivalent to merely forwarding the three original scalar sliders. [E2, E5]

**Limits:** the precise shader equation, mask-channel packing, and runtime activation of that branch were not recovered here. Do not infer activation solely from the `MaskGroups` menu value. The shipped INI has `MaskGroups=false`, but that is not a capture of the user's installed settings or of the effective control resource at dispatch. Do not synthesize a constant mask based on guessed channel meanings.

### 3.3 Final PD payload → NGX setter

Once the evaluation payload is resolved, its scalar transmission is direct:

```text
PD+A52CC: payload[+0xEC] -> temporary
PD+A52D3: temporary -> internal NGX block[+0xE4]
PD+9E909: load internal block[+0xE4] as float
PD+9E911: select "DLSSNR.LocalToneStrength"
PD+9E91B: call float setter helper +0x1217A0
```

No luminance statistic or temporal smoothing is applied in these transfer instructions. That narrow observation does not rule out model temporal processing or earlier mask/configuration transformations. [E6, E7]

Keep the three structures distinct:

| Structure | Tone | Backbuffer |
|---|---:|---:|
| Chain packet, first pass | `+0x18C` | separate frame-level routing |
| Legacy-shaped evaluation payload | `+0xEC` | `+0x40` |
| PD internal NGX block | `+0xE4` | `+0x38` |
| Bundled NR's parsed evaluation data | `+0xE4` | `+0xA8` |

The equal Tone offsets in the last two structures are coincidental layout facts, not evidence that the structures are interchangeable.

## 4. What AIO19's Backbuffer actually is

### 4.1 Local route selection

In `PD+0xA5360..0xA5CF8`, local mappings are obtained and a local committed NR output is selected/created via `PD+0xA06D0`.

At the relevant selection:

```text
PD+A56F0: r12 = r15              // initialize Backbuffer candidate to NR output
PD+A56F3: read external Backbuffer from payload +0x40
...      compare with external Color at payload +0x08
...      conditional selection of mapped Color
PD+A575C: pass selected Backbuffer onward
```

Reconstructed selection for this mapped path:

```cpp
// Descriptive pseudocode, not recovered source or a universal route contract.
backbuffer = localNrOutput;
if (externalBackbuffer != nullptr &&
    externalBackbuffer == externalColor &&
    mappedColor != nullptr) {
    backbuffer = mappedColor;
}
```

This is not a guaranteed independent pre-NR image. In particular, the initially selected resource is Output. [E3]

### 4.2 Downstream fallback

In the evaluation builder `PD+0xA2380`:

```text
PD+A2642: one private-work branch puts Output into internal Backbuffer
PD+A264C..A2664:
    internal Backbuffer = supplied Backbuffer ? supplied Backbuffer : Output
```

The final writer at `PD+0x9E4E0` explicitly sets `DLSSNR.Backbuffer` from internal `+0x38`, as well as its subrect keys. [E5, E7]

### 4.3 Backbuffer has several runtime roles, not a proved exposure role

The bundled NR parameter reader queries Backbuffer at `NR+0x1A6C4`; failed retrieval leaves its parsed resource pointer null (`+0x1A7AE`). Its main evaluation routine uses Backbuffer in bypass/reference selection and optional composition/UI-related branches, including the conditional routing at `NR+0x19016..0x191BD`. [E8, E9]

This proves the key can affect behavior. It does **not** establish “Backbuffer is the luminance reference Local Tone requires,” nor “missing Backbuffer is the drift's leading cause.” The earlier report should not be used as proof of either claim.

## 5. Output initialization: another concrete parity difference

Before evaluation, `PD+0xA2380` can initialize Output from Color:

```text
PD+A2428: compare Color and Output pointers
PD+A2433: check copy-compatible dimensions / format family
PD+A2443: prepare Color as COPY_SOURCE (0x800)
PD+A245F: prepare Output as COPY_DEST (0x400)
PD+A2480: command-list CopyResource(Output, Color)
PD+A248D: prepare Color as NON_PIXEL_SHADER_RESOURCE (0x40)
PD+A24A9: prepare Output as UNORDERED_ACCESS (0x08)
```

The copy is conditional: pointers must differ and helper `PD+0xA09D0` must accept the resources. That helper reads both descriptions, compares dimensions, and compares normalized format-family values. [E4, E5]

Microsoft's contract confirms `CopyResource` takes destination then source, copies compatible resources, and does not apply a transfer-function conversion. The state constants and call ordering above identify an initialization copy, not a gamma-decoding shader. [S3]

The inspected Dva `Stage::RecordInternal()` sets its resource parameters, evaluates NR, and then restores alpha. It does not contain this pre-evaluation Color→Output initialization copy. Verify the surrounding adapter at the candidate revision as well before adding or counting a copy. This is a parity candidate, **not proof of an uninitialized-read bug**: the runtime may overwrite Output fully on Dva's chosen route. [S1]

AIO's `Backbuffer=Output` selection must therefore be evaluated together with its output initialization. Treating “Backbuffer points at Output” in isolation loses potentially important setup.

## 6. Inside the bundled NVIDIA runtime

### 6.1 Parsing and defaults

`NR+0x19F30` reads the NGX parameters. Tone is addressed at parsed `+0xE4` at `+0x1A974`, requested through the float getter, and defaults to `1.0f` on unsuccessful retrieval at `+0x1A9A5`. [E8]

### 6.2 A callback can overwrite the parsed Tone

The main evaluation routine calls **`NR+0x1AC30` at `NR+0x18866`** after parsing.

The routine contains the original diagnostic name `NGXCG2R::PollRuntimeParams`. It obtains a callback through a getter at `NR+0xEFD0` (global callback slot RVA `0x11527B0`). A missing callback or a callback returning false leaves the ordinary control path in place. For a successful response with the Local Tone entry marked present:

```text
NR+1ADC6: test returned-entry presence
NR+1ADCB: load returned double value
NR+1ADD0: convert double to float
NR+1ADD4: overwrite parsed LocalTone at +0xE4
```

Other present entries can overwrite local structure, intensity, and additional controls. The call requests feature `0x12` and eight descriptors; the external provider's exact policy and activation are outside this pass. [E9, E10]

**Diagnostic consequence:** capture both requested and post-callback Tone. Do not claim this callback is currently active, is changing with camera movement, or is the cause. Its existence establishes a measurement point, not a diagnosis.

### 6.3 Local Tone scales optional configuration coefficients

Before network evaluation, `NR+0x18E64` calls a configuration-selection helper `NR+0x1D7C0`. When it finds an applicable record, it calls **`NR+0x1D5F0`**.

This helper reads parsed Local Tone and clamps a local working copy to `[0,1]`. For enabled bits in a configuration mask, it computes a set of 14 coefficients at parsed `+0x124..+0x158`.

For finite Tone `t`, the directly recovered arithmetic includes:

```cpp
t = clamp(parsedLocalTone, 0.0f, 1.0f);
if (mask & 0x1) coeff[0] = 0.0f + t * (configured[0] - 0.0f);
if (mask & 0x2) coeff[1] = 1.0f + t * (configured[1] - 1.0f);
// Enabled coefficients 2..13 interpolate from 0.0f in the inspected helper.
```

The instructions are at `NR+0x1D5F0..0x1D7B9`; coefficient 0's store is `+0x1D62C`, coefficient 1's is `+0x1D649`. **Names such as exposure, gamma, contrast, or color-matrix element were not assigned without evidence.** The selected record contents and its meaning must be captured before proposing a patch. [E12]

This local clamp is not proof that every network consumer clamps Tone to `[0,1]`. The network-manager path separately loads the parsed tone value.

### 6.4 Tone reaches network argument preparation

The main evaluator calls the function identified by embedded diagnostics as `CG2RNetworkManager::Evaluate` at `NR+0x19811 -> +0x21BB0`.

Within it:

```text
NR+22563: load LocalTone from parsed data +0xE4
NR+225CB: put it in a stack argument
NR+22636: call network argument/preparation helper +0x3F490
```

This is positive data-flow evidence that Tone enters network preparation, not just a logging value. The learned GPU kernels and complete image equation were not decompiled. [E13, E14]

### 6.5 Control changes can reset temporal history

`NR+0x179D0` contains the diagnostic name `CG2R_ResetTemporalHistoryOnControlChange`.

It compares cached and current controls, including Tone at `NR+0x17AFC..0x17B22`. The absolute difference threshold is approximately `0.00001f` (literal at RVA `0xAFC20`). When its enabling state is set and initialization/configuration or qualifying control changes are detected, it writes parsed Reset=`1` at `NR+0x17B98`. [E11]

It is therefore useful to distinguish:

```text
A. Requested Tone stable; effective Tone or selected configuration changes.
B. Tone/configuration stable; runtime Reset changes.
C. All controls/reset stable; same-surface image correction changes.
```

Case C would direct attention toward actual image/guide/model behavior; A or B would point to control/history provenance. AIO's bundled runtime behavior does not justify disabling Dva's explicit camera-cut and lifecycle resets.

## 7. AutoExposure: what this pass does and does not establish

The generic NVIDIA DLSS enum defines `MVLowRes=1<<1` and `AutoExposure=1<<6`. Thus the numeric decomposition of `0x42` is correct for that enum. **That does not establish that NR feature `0x12` consumes the same flag.** [S4]

The mapped AIO NR creation writer `PD+0x9EC40..0x9ED33` writes node masks, NR width/height/preset, quality and optional scaling ratio, then calls its NR create wrapper. It does not set `DLSS.Feature.Create.Flags` in that routine. [E7]

Fresh full-file ASCII and UTF-16LE searches of AIO's exact `nvngx_dlssnr.dll` did not find the literals `DLSS.Feature.Create.Flags` or `ExposureTexture`. The parser inspected in this pass does not read them. The larger PD module contains generic DLSS strings outside the mapped NR creation writer; their presence alone is not evidence of NR use. [E15]

These observations support deprioritizing the unsuccessful flag experiment. They do not prove that the network has no exposure-like adaptation, no hidden configuration, or no hashed/indirect controls. Keep the user's “off gives the same result” as a negative result; do not retest it repeatedly or claim that it identifies a specific remaining cause.

There is also a PD write of `DLSSNR.GlobalToneStrength=1.0` at `PD+0xA25DA..0xA2611`. No matching literal appears in the bundled NR image or mapped getter. Record the write, but do not present adding it to Dva as a supported fix. [E5, E15]

## 8. Color-domain comparison remains unresolved

The local output allocator `PD+0xA06D0` obtains a resource description and forwards its dimensions and Format to allocation; the relevant format forwarding is at `PD+0xA075C..0xA0777`. It does not universally force the local output to linear RGBA16F. [E3]

Dva's inspected preparation explicitly converts source color from its configured encoding into a linear working image before NR. [S2]

That establishes a comparison requirement, not an answer that AIO “wants gamma.” Texture format alone is insufficient: an sRGB view, upstream shader, or private work path can alter sampling semantics. Capture producer values, view formats, resource formats, actual NR working dimensions, and selected reconstruction/mask route. Do not simply replace Gamma22 with sRGB until the actual source contract or a controlled image comparison supports it.

## 9. Breakpoint / instrumentation map for Codex

These are **observation addresses for the exact hashes**, not safe inline-hook prologues. Some PE unwind entries split one routine; never infer a hookable function boundary from one short fragment. Use module-relative breakpoints or separately validated hooks.

| Module/RVA | What to observe |
|---|---|
| PD `+0xA3802` | Selected chain Tone before mask-specific override |
| PD `+0xA3FA1` | Whether the control-mask scalar override path is reached |
| PD `+0x9E4E0` | Final internal NGX block: Tone `+0xE4`, ControlMask `+0x20`, Backbuffer `+0x38`, Color `+0`, Output `+0x18`, subrects |
| PD `+0xA2480` | Whether Color→Output initialization copy is executed; source/destination identities |
| PD `+0xA56F0..+0xA575C` | Local Backbuffer selection and relation to input/output |
| NR `+0x1886B` | After runtime callback: evaluation-data base is `rbp+0x20`; record effective Tone/Intensity/Style and callback result |
| NR `+0x18E69` | After configuration application: inspect coefficient block `frame+0x124..+0x158` |
| NR `+0x17B98` | Internal control-change reset event and its triggering values |
| NR `+0x21BB0` entry | `r8` is the network evaluator's frame-data argument; inspect final Tone `+0xE4`, Reset `+0x100`, Color `+0`, Output `+0x48`, ControlMask `+0x60`, Backbuffer `+0xA8` |

Observe actual values for a bounded frame sample, not continuously paused gameplay used as a performance benchmark. AIO's bundled NR hash differs from Dva's RTX40 pin; map Dva independently. A source-level Dva setter log cannot replace an inside-runtime effective-control observation.

## 10. Revised experimental order

### Step A — establish equivalent effective input

Use one pass, Tone exactly `1.0`, matched Intensity/Structure/Style, identical effective placement and source size, FG disabled, and the same view/lighting/ReShade order. Record AIO's final ControlMask presence and selected branch, not just `MaskGroups`/slider text. Record callback activity and resolved coefficient/reset state. Preserve a Tone=0 mitigation/control.

### Step B — test the concrete output/Backbuffer difference safely

Use isolated research variants with identical model, color preparation, controls, and history policy:

| Variant | Initialize Output from Color? | NGX Backbuffer |
|---|---|---|
| Current baseline | Current behavior | Current behavior / absent |
| Initialization-only | Yes, compatible full copy | Absent |
| Mapped-fallback parity candidate | Yes | Same resource as Output, valid matching subrect |

This distinguishes a preseed effect from the additional Backbuffer route. Do not make an uninitialized Output into a supposed original-scene reference. A separate pre-NR Backbuffer can be tested later, but it is an additional semantic experiment, **not the recovered universal AIO design**.

Keep Color and Output as distinct resources. Sequence the initialization after the real producer dependency and before evaluation; transition actual resource states correctly. Register Backbuffer as the already-retained Output role in the experimental contract rather than weakening all alias validation. Preserve alpha, slot ownership, error handling and deferred retirement. Do not add a CPU wait.

### Step C — compare the observed mask / color route

If AIO actually uses the mask branch while Dva uses only scalar controls, first capture the mask and resolve its semantics. Do not implement guessed channels or borrow a different DLL's parameter table. If AIO and Dva both use scalar-only inputs but color values differ, investigate producer transfer/view formats and where the scene is sampled.

### Step D — measure same-surface correction, not full-screen means

Capture a static/pan/stop/return sequence. Compare source and NR output on reprojected persistent surfaces, rejecting disocclusions, sky transitions, invalid depth, and out-of-bounds motion. Compare the **NR-induced correction** across matched surface points, rather than concluding drift from two different full-screen RGB means. Keep readback bounded and separate from timing measurements.

If effective controls, configuration, reset behavior, guides and color inputs match and the enabled-tone correction still differs, compare the exact runtimes on identical recorded input sequences. Previous performance A/B does not establish identical Local Tone image behavior.

## 11. Corrections and limits for the previous Codex report

- Replace “missing Backbuffer is HIGH confidence” with “Backbuffer/output initialization is a recovered, testable route difference; causality unproven.”
- Replace “AIO uses a separate original linear reference” with the actual Output fallback and conditional mapped-Color selection.
- Do not infer NR support from generic DLSS AutoExposure enums.
- Do not assume the final Tone equals the UI slider: mask preparation and runtime callbacks are additional stages.
- Do not treat Tone as only exposure: it reaches network preparation and optional selected coefficients.
- Do not infer that a non-temporal custom local-tone shader guarantees camera-invariant surfaces. A current-frame algorithm can still change when the visible neighborhood or histogram changes.
- Do not disable resets, change motion sign, add CPU waits, or change all input domains simultaneously as a speculative fix.
- No conclusion here proves that AIO19 is stable in the user's exact scene, that Dva's missing parameter is a bug, or that any proposed variant removes the defect.

**What is still missing:** live AIO/Dva boundary captures, activation of the optional callback/configuration/mask routes, mask shader/channel semantics, full learned GPU Local Tone equations, and a reproduced enabled-tone gameplay fix. The current LocalTone=0 setting remains a mitigation rather than a root-cause repair.

## 12. Evidence, verification, and source references

The evidence ZIP intentionally excludes proprietary DLLs and the original archive. It contains 20 disassembly intervals, 46 selected byte witnesses, exact input hashes, literal searches, an INI excerpt, and a standard-library verifier.

The recorded verification result is **144/144 identity/byte/literal checks**, with **7,289 disassembled instruction lines byte-checked**. This verifies the exported evidence against the exact inputs, not semantic completeness or a visual fix. Run the verifier again with the original extracted files:

```text
python tools/verify.py --extracted-root <AIO19-extracted-root> \
  --archive <SkyrimUpscalerAIOBuild19-Hotfix1.7z> --output verification.json
```

The extracted root must contain `SKSE/Plugins/SkyrimUpscaler.dll`, its INI, and `UpscalerBasePlugin/PDPerfPlugin.dll` / `nvngx_dlssnr.dll`. The optional archive argument permits either filename as long as the expected bytes match.

### Evidence index

| Reference | Files in `evidence/` |
|---|---|
| E1 | `pd_chain_validation.asm` |
| E2 | `pd_chain_submit.asm`, `pd_chain_evaluator.asm` |
| E3 | `pd_local_route.asm`, `pd_output_clone.asm` |
| E4 | `pd_copy_compatibility.asm` |
| E5 | `pd_evaluation_builder.asm` |
| E6 | `pd_payload_to_ngx.asm` |
| E7 | `pd_ngx_writer.asm`, `pd_ngx_create_writer.asm` |
| E8 | `nr_parameter_reader.asm` |
| E9 | `nr_evaluate.asm` |
| E10 | `nr_poll_runtime_params.asm`, `nr_runtime_callback_getter.asm` |
| E11 | `nr_control_change_reset.asm` |
| E12 | `nr_config_selection.asm`, `nr_tone_scaled_config.asm` |
| E13 | `nr_network_evaluate.asm` |
| E14 | `nr_network_argument_pack.asm` |
| E15 | `literal_scan.json` |
| E16 | `shipped_nr_ini_excerpt.txt` |
| Supplement | `nr_optional_postprocess.asm`, `witnesses.json`, `manifest.json`, `verification.json` |

### External/source checkpoints

S1. Dva Stage at the inspected commit:  
https://github.com/wallhead/DvaKolbas/blob/e72452884ede7d1de48eb3d8a20fba32991e9024/src/NeuralRendering/Stage.cpp

S2. Dva prepared-input conversion at the inspected commit:  
https://github.com/wallhead/DvaKolbas/blob/e72452884ede7d1de48eb3d8a20fba32991e9024/src/NeuralRendering/PreparedBeforeUpscale.cpp

S3. Microsoft `ID3D12GraphicsCommandList::CopyResource` documentation:  
https://learn.microsoft.com/en-us/windows/desktop/api/d3d12/nf-d3d12-id3d12graphicscommandlist-copyresource

S4. NVIDIA generic DLSS flag definitions; not proof of NR consumption:  
https://github.com/NVIDIA/DLSS/blob/main/include/nvsdk_ngx_defs.h
