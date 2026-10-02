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
