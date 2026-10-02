# FSR FG whole-plan review and corrections

The read-only review covered working SR base `8e78f5136772fbbbb23c2aafe341ab931256c475` through `a1f98bcab658472e60744ba784cacc2c232c9a05`, followed by a review of the corrections. It inspected runtime loading, source transactions, callback serialization, producer/SDK retirement, startup, resize/suspension, UI/color, settings, tests and packaging against the approved spec and amended plan. The user-requested removal of the 60/66 FPS gate is intentional.

The initial verdict was **with fixes**: no Critical findings, two Important findings and one Minor help-text issue. Both Important findings were reproduced before their fixes. The final read-only recheck found no remaining Critical, Important or Minor implementation issue, conditional on successful production build, full suite and package validation. This is a code-review result; it does not establish unmeasured acceptance outcomes.

## Corrected findings

1. `FsrPresentationUiConverter` compared a foreground SRV's device with the context producer. ReShade exposes a native device through views and a producer proxy through resources; a valid foreground could permanently fault Present. The converter now obtains the foreground texture's device, retains strict COM identity and all alias/extent/format/view checks. The actual-ReShade regression failed at `RealReShadeForegroundResourceOwnership` before the fix. It now independently checks half-alpha foreground blending and exact preservation of producer pixels across 96 AMD/ReShade transactions. Native WARP tests still reject a foreign foreground. The clear is observed as `127,0,0,127`; the literal half-alpha/blend checks allow one UNORM quantization step, while preservation compares exact before/after bytes.
2. Apply validated only the requested presenter, but immediately changed Native UI in the current session. Choosing DLSS/DLAA or Ordinary for restart could therefore disable UI separation while the AMD presenter still owned presentation. Capabilities now carry the effective presenter state from `NvidiaHost::FsrFgActive()`. Validation rejects disabling Native UI before any settings mutation, independently of the pending presenter. The action regression failed at `EffectiveAmdPresenterRequiresNativeUiEvenWhenPendingPresenterChanges` before the guard. Pending DLSS, DLAA and Ordinary choices with Native UI retained remain valid; non-AMD native-UI opt-out remains valid.
3. Overlay help mentioned sustained source-rate eligibility after that gate was removed. It now describes completed UI, temporal guides and the retained menu/loading/stall gates.

The two-test RED run failed both regressions. The corrected targeted suite passed 3/3 in 12.51 seconds. Full-build and delivery identities are recorded separately in `FSR_FG_FINAL_REVIEW_VALIDATION.json` and the package/install receipts when available.

## Acceptance and limits

The user reported all requested HUD/inventory/map/dialogue, live FG toggle, save/reload, two fast travels, three alt-tabs, minimize/restore and same-scene FG off/on color/flicker checks passed. These are human observations. Existing standalone automatic composition and native DXGI cadence measurements, GPU readbacks and Skyrim callback logs retain their individual evidence scopes.

The reviewer declined to judge the following; the executor retains each limitation explicitly:

| Area | Executor disposition |
| --- | --- |
| Measured Skyrim pacing, scanout and latency | Bounded base-frame timing capture completed on 2026-10-03; see FSR_FG_SKYRIM_TIMING.md. No game scanout or latency claim. |
| Calibrated Skyrim/ENB transfer and color | Gamma22 remains provisional. FG toggle appearance passed; formal source/ENB calibration remains pending. |
| AMD/Intel hardware | No available execution evidence; pending hardware acceptance. Current tested GPU is RTX 4080 Super. |
| Three Graphics Tools checks | Unavailable and excluded under the user's instruction to continue; never counted as passes. |
| Automatic Alt+Enter | Not covered by alt-tab acceptance. Keep a separate pending acceptance item; explicit unsupported fullscreen calls are gated. |
| Arbitrary concurrent outer-host lifecycle calls | Retain the existing render/host-thread ownership assumption; SDK session serialization does not create an unrestricted concurrent host API. |
| ML FSR4, other upscaler/FG combinations, HDR/exclusive fullscreen, dynamic resolution, CS expansion, multiple generated frames and async compute | Explicitly deferred features in the approved first-stage scope. |
| Archived RE conclusions and other runtime versions | Historical research is not the implementation ABI. Acceptance applies to the pinned public SDK/providers and supplied ReShade runtime. |

Seven of eight milestones remain complete until the outstanding acceptance evidence is resolved. The review does not relabel pending or unavailable checks as passing.
