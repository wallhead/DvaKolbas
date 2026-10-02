# FSR frame generation implementation evidence

Approved for implementation by the user's "start" after the revised plan `8d0df1e`.
Work follows [the plan](superpowers/plans/2026-10-02-fsr-fg.md) in the existing `codex/fsr-sr` worktree. The installed V5.4 NO-LORE SR DLL and INI remain unchanged.

## Runtime and provider boundary

* `TRP_ENABLE_FSR_FG` defaults off and requires SR support. FG headers are hash checked only when enabled; SR-only builds need no FG headers or DLL.
* `Acquire-Runtime.ps1 -IncludeFrameGeneration` verifies the same pinned archive, three additional headers, exact FG DLL bytes/hash, AMD signature and file version. Default acquisition retains two SR modules. Acquisition tests also reject a modified FG DLL.
* `FsrRuntime::LoadFrameGeneration` loads the absolute plugin-relative module explicitly; missing, incomplete and foreign-basename FG failures leave the existing SR function table usable. No global DLL search-path mutations.
* New queries use effect-tagged catalogs and the exact public descriptor classes: upscale, FG, and NewDX12 swapchain. Counts/retries and copied names retain the existing bounds.
* Selection compares observed opaque ID/name identities from the pinned DLL, independent of enumeration order. Context verification rejects actual ID/name mismatch. No implicit ML or cross-effect fallback.

Real probe: RTX 4080 SUPER; D3D11/D3D12 LUID `00000000:00010318`; pinned SDK commit `60f4ea81909200d8542eca14dccb2628b763a9a3`.

| Effect | Observed ID | Name |
| --- | --- | --- |
| SR | 17700776140660019205 | 3.1.5 |
| SR | 17700776140655833092 | 2.3.4 |
| FG | 17726168133342859270 | 3.1.6 |
| Swapchain | 17752306900579389447 | 3.1.7 |

Probe command: `TRPFsrGenerationProviderProbe <absolute plugin root containing FSR/>`. This confirms effect discovery on this adapter; it does not prove generated pixels, UI composition or pacing. Module file version 4.0.1.2740 and FG header ABI 4.0.1 are distinct from analytical provider 3.1.6.

Validation: observed failing optional-load, deterministic-selection and typed-created-provider tests before implementation; targeted `FsrRuntime` and `FsrGenerationRuntime` passed 2/2. `FSRGenerationAcquisitionTests.ps1` passed SR-only, opt-in and tamper cases. The SR-only Standard Release build succeeded and its full runnable suite passed 119/119 (22.28 seconds). Three Graphics Tools-dependent tests remain unavailable and excluded as previously authorized.

Remaining: source/rate policy; typed FG context and SDK session; scene/UI transport and fences; AMD presentation/lifecycle; actual-GPU generation fixture; host/settings/ReShade integration; package and gameplay acceptance. FG is not activated in Skyrim.

## Source identity and suppression

Backend 2 is reserved for FSR's AMD presenter. The explicit resolver capability requires both SR and FG support plus analytical SR, SDR, fixed dimensions and NR off. Backend 0 remains ordinary SR, and 1 remains NVIDIA. Disabling generation retains an already selected AMD presenter. Host/INI/settings admission stays closed until standalone validation and Task 7 integration.

History admits each real source once. Duplicate/out-of-order/zero IDs cannot enter Configure; a forward gap admits one disabled source and clears reentry. SDK generation success remains separate from this eligibility decision. Reasons use static string views. `Decide` mutates its source/rate history; `AcknowledgePrepared` accepts only the current eligible pending source, and invalidation clears that acknowledgment.

Analytical rate policy: rolling mean of eight source deltas; suppress at three successive means below 60 FPS; enable after eight successive means at least 66 FPS. Initial eligible source is 15 for a steady qualifying input. Invalid time, a delta >=100ms, menus, non-temporal sources, incomplete UI, invalid guides/camera and camera/extent resets clear reentry. Reentry requires Prepare(reset=true); its success acknowledges history.

Audit: 165 source/package commits reachable from `HEAD`, `codex/dlss-fg-baseline` and `wallhead/codex/fsr-sr`; unrelated broken application capture refs excluded. The historical `mode==2` ternary selected FSR upscaling and set presentation backend 0. No existing backend 2 meaning was found in the named reachable history.

Validation: old resolver failed the new supported-FG request before implementation. New policy covers both compiled capability states, exact rate boundaries/hysteresis, duplicate/gapped sources, non-temporal/menu/UI suppression, stalls, invalid camera/time and failed/stale acknowledgment. The FG-enabled Standard Release build and full runnable suite passed 121/121 (22.57 seconds), including settings/actions, startup preferences and runtime regressions. No game-facing FG activation or DLL installation.
