# AIO18 v3 — proposed runtime experiments

**Not executed.** All values are module-relative RVAs for the hashes in the report. The original preferred image base is not the live base. Resolve actual modules first; do not install these as SkyrimSE.exe offsets. Preserve existing v2 breakpoints for AMD Present, UI copies and fences.

## 1. UI depth-SRV reference balance

At `SkyrimUpscaler+0x2A9740`, record RCX=context, EDX=StartSlot, R8D=NumViews, R9=array; record the UI-phase byte at module+0xE81092. Only read array entries when the count and pointer are valid.

At `+0x2A977D` before the virtual call, RCX is the source view and RDX points to the returned-resource slot. At `+0x2A9780` after the call, read resource `[RSP+0x20]`; compare it against host+0x7D0. Record both matching and nonmatching resources. Track the corresponding AddRef/Release operations using an isolated diagnostic build or graphics debug instrumentation. Do not assume a COM return count by itself is a fully reliable lifetime oracle.

Expected static result: the hook acquires a resource reference but has no local Release. Determine actual hit rate, active renderer branch, and whether old resource identities survive a controlled resize after other owners retire. Distinguish leaked references to existing textures from newly allocated textures. A patch experiment should change only ownership balancing, not UI routing or fences.

## 2. Viewport and late-UI contract

At `SkyrimUpscaler+0x2A95F0`, record count and all valid viewport entries. At `+0x2A9724`, compare the forwarded array with the original. Capture host+0x270/+0x274 display dimensions and +0x278/+0x27C render dimensions.

Separate the normal UI branch (phase+count==1+size match) from the earlier special branch. The latter constructs one local viewport but retains the original count; specifically test whether count>1 is ever reached under its predicates. Do not trigger invalid counts just to obtain a crash in an ordinary game session.

Correlate with the v2 `OMSetRenderTargets` hook, render-target identity and depth SRV binding. The existence of these hooks does not identify the final Scaleform flush.

## 3. Jitter conventions

At host `+0x19FF11` (after GetJitterOffset), log raw jitter. At `+0x19FF74/+0x19FF79`, log the signed values stored into host+0x08/+0x0C. At PD `+0x100B70`, inspect RDX common payload+0x44/+0x48.

Record actual render width/height, frame index and camera projection. Expected engine route: engine offsets [-2*Jx/W,+2*Jy/H], backend pixel jitter [-Jx,-Jy]. Do not mix this with the separate exported host jitter bridge at +0x293DD0.

## 4. Depth change and SR/FG reset coverage

Host +0x296C73: convention store. +0x296C79/+0x296C81: near/far stores. +0x2965EF: FG parameter reconfiguration. +0x29665F: SR reinitialization. +0x296757: SR reset requested. +0x294FDD: SR host request cleared after PD export.

PD +0xF0360: FG resetPending set after disabling API type 1. +0xEF4AD..+0xEF4C0: effective Prepare reset includes payload+0x54 OR backend+0x1C9. +0xEF581/+0xEF719: Prepare/Configure calls and returned codes. +0xEF759: resetPending cleared only on success with enabled state.

Capture a normal enable/disable, camera cut, loading-screen transition, resize, and one controlled dispatch-failure experiment separately. Record whether each transition reaches a reset producer. The normal host FG payload reset is zero; backend and AMD reset mechanisms must also be observed before concluding a gap.

## 5. SR resource-role correction

PD +0xF5D96 injects default output at common payload+0x28. At +0x100B70, RDX points to the common payload. At +0x100E84 before ffxDispatch, inspect the descriptor at RSP+0x60 and the target address R8.

Descriptor resources: +0x18=color, +0x48=depth, +0x78=motion, +0xA8=external exposure(null), +0xD8=reactive, +0x108=T&C(null), +0x138=output. Confirm actual resources and formats; never pass common payload+0x20 as the output. Output chooses common+0x30 if nonnull else+0x28.

For DX11, +0xFF995 passes R8 as a different resource bundle. Its +0x30/+0x38 output slots are not common-payload offsets.

## 6. Sampler policy

Host +0x2A85AF checks active bias changes. +0x2A88A2 reads sampler description; +0x2A88AA/+0x2A88B4 determine eligibility. +0x2A88FF creates a replacement. Record original bias, MaxAnisotropy, Filter, host+0x2EC, pointer cache hits and stage. Verify all six shader-stage setter routes independently.

Do not infer unconditional anisotropic-filter selection from MaxAnisotropy>1, or the exact bias formula from an unresolved scalar math thunk.

## Capture record

Record archive/module hashes, game version, loaded bases, GPU/driver, renderer options, relevant INI settings, frame/token IDs, resource identity/descriptor, call stack, before/after state, one changed variable, and result. Label RUNTIME_OBSERVED only after the capture exists. No runtime entries are present in this package.
