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

Remaining: scene/UI transport and fences; AMD presentation/lifecycle; actual-GPU generation fixture; host/settings/ReShade integration; package and gameplay acceptance. FG is not activated in Skyrim.

## Source identity and suppression

Backend 2 is reserved for FSR's AMD presenter. The explicit resolver capability requires both SR and FG support plus analytical SR, SDR, fixed dimensions and NR off. Backend 0 remains ordinary SR, and 1 remains NVIDIA. Disabling generation retains an already selected AMD presenter. Host/INI/settings admission stays closed until standalone validation and Task 7 integration.

History admits each real source once. Duplicate/out-of-order/zero IDs cannot enter Configure; a forward gap admits one disabled source and clears reentry. SDK generation success remains separate from this eligibility decision. Reasons use static string views. `Decide` mutates its source/rate history; `AcknowledgePrepared` accepts only the current eligible pending source, and invalidation clears that acknowledgment.

Analytical rate policy: rolling mean of eight source deltas; suppress at three successive means below 60 FPS; enable after eight successive means at least 66 FPS. Initial eligible source is 15 for a steady qualifying input. Invalid time, a delta >=100ms, menus, non-temporal sources, incomplete UI, invalid guides/camera and camera/extent resets clear reentry. Reentry requires Prepare(reset=true); its success acknowledges history.

Audit: 165 source/package commits reachable from `HEAD`, `codex/dlss-fg-baseline` and `wallhead/codex/fsr-sr`; unrelated broken application capture refs excluded. The historical `mode==2` ternary selected FSR upscaling and set presentation backend 0. No existing backend 2 meaning was found in the named reachable history.

Validation: old resolver failed the new supported-FG request before implementation. New policy covers both compiled capability states, exact rate boundaries/hysteresis, duplicate/gapped sources, non-temporal/menu/UI suppression, stalls, invalid camera/time and failed/stale acknowledgment. The FG-enabled Standard Release build and full runnable suite passed 121/121 (22.57 seconds), including settings/actions, startup preferences and runtime regressions. No game-facing FG activation or DLL installation.

## Typed preparation and SDK session

`BuildFsrGenerationPrepare` maps source 41 and 16.6667 milliseconds directly into PrepareV2. It validates fixed extents, full subrect, known sRGB publication, guide formats, matching resource/command-list device, depth/motion context flags and camera basis/projection. Camera position and near/far stay in native world units; normalized view columns provide world-space right/up/forward, with an explicit world-units-to-meters factor. Rotated-camera and foreign-WARP-resource cases pass.

The session owns one mutex and stable callback state. A retained RAII token is valid only on its owning session/thread; recursive/nested session acquisition yields an invalid token. Configure admits a source once and precedes matching Prepare; suppression configures disabled and skips Prepare. Present explicitly enters/exits under that same token. Generation dispatch uses AMD's descriptor unchanged, inherits the lock, and records bounded scalar status without logging, allocation or game calls. Creation descriptor members and runtime/resource owners survive context lifetime.

Lifecycle detach requires stopped admissions. Context destruction requires callback detachment and externally proven Prepare/SDK-present retirement; the later presenter owns those proofs. An unfinished destructor retains owners, and its inspection is serialized. Tests observed then corrected open-admission teardown and concurrent destructor failures. Failure/timeout never fabricates GPU completion.

Ruling: actual scene/UI conversion belongs to the transport in Task 4; this task rejects incorrectly labeled Gamma22 publication. Swapchain ABI 3.1.7 is linked at the separate swapchain creation in Task 5, while FG creation links ABI 4.0.1. Adding a swapchain version descriptor to the FG context would confuse distinct effect contracts.

Real pinned module probe `--create-context`: analytical FG 3.1.6 ID 17726168133342859270; FG ABI 4.0.1; swapchain header ABI 3.1.7; 640x360 render -> 1280x720 display; GPU memory 23,592,960 bytes. Actual identity query, memory query and context destruction passed. No Prepare/generation/Present commands were submitted by that probe.

Validation: both typed preparation/context tests failed on missing implementation, then passed 2/2. The full FG-enabled Standard Release build and runnable suite passed 123/123 (22.86 seconds). Graphics Tools debug layers remain unavailable; these results do not claim those layers ran. Generated pixels, async lifetime and actual presentation remain future fixture gates.

## Completed scene/UI transport

The transport owns a native D3D11 scene target on the original producer, shared sRGB scene/UI textures and a D3D12-only UI publication texture. It reuses the existing scene converter. An owner-scoped UI converter merges HUD and overlay in their explicit native transfer, unpremultiplies, converts to sRGB and premultiplies again. This keeps alpha coverage and clears zero-alpha RGB. Cached shaders/views have the transport lifetime; no per-frame shader allocation or global converter cache.

D3D11 guide overwrites wait on the FrameGeneration fence submitted immediately after Prepare. Shared scene/UI overwrites separately wait on the SwapChain copy fence. Private UI remains borrowed until successful Present return with SDK internal buffering. SDK asynchronous presents require an explicit successful unregister/WaitForPresents proof; bridge Drain alone cannot release their UI. Retirement also seals native scene work queued before Upload. Failures retain the complete owner rather than inventing completion.

Validation: missing transport/converter tests failed before implementation. An additional unpublished-producer shutdown test reproduced premature release, then passed after sealing the final D3D11 producer. WARP readbacks independently verify half-alpha linear/gamma HUD pixels, zero-alpha edges, single overlay composition, gamma-to-sRGB scene pixels and context restoration. An actual held D3D12 reader observes old guide pixels while the later D3D11 producer advances only after release. Timeout and asynchronous-UI protocol tests retain owners. The Standard FG-enabled Release build and full runnable suite passed 125/125 (23.61 seconds); unavailable Graphics Tools cases remain excluded. Actual AMD generation/presentation is still a later gate; the installed SR package remains unchanged.

## AMD presenter and lifecycle

The presenter uses only NewDX12 with its retained matching D3D12 queue and two application buffers. Legacy HWND/extent/refresh/scanline/scaling/usage/supported flags are preserved; unsupported fullscreen, MSAA, format, usage and flags have explicit field diagnostics. A scoped internal-factory guard prevents future factory-hook recursion, and a short-held HWND reservation prevents a second owned AMD presenter. Context/version/override creation descriptors are members. No alternate creation API is retried after failure.

Under one SDK session lock, each source is admitted once, converted, configured, optionally prepared and submitted with its guide fence, copied to the application/UI buffers, registered on the separate swapchain context and presented through the production generation callback. Test and feature-less startup presents consume no source ID. Disabled sources still register complete native UI. The callback inherits the lock and dispatches the SDK descriptor unchanged; no manual interpolation list/output query enters production.

Shutdown stops admissions, detaches generation callbacks, unregisters UI, waits for SDK presents, seals/drains native producer/Prepare/copy work, destroys FG and swapchain contexts, releases final swapchain references and then releases transport/runtime. Any failed boundary retains its owners. Dimension changes replace a completely retired presenter with a new fixed-descriptor owner; host integration must clear borrowed inner references before replacement. Unsubmitted failed Prepare work can be closed/discarded without fabricating a fence.

The real SDK probe initially removed the device on startup. Pinned source `FrameInterpolationSwapchainDX12.cpp` shows replacement application buffers are allocated lazily by GetBuffer; Present uses them. A regression failed before initialization, then passed after obtaining both D3D12 application buffers before startup Present. The next real run exposed invalid null-chain teardown Configure on a never-configured FG context. Its regression failed, then passed by recognizing that creation installs no callback and is already detached. The exact pinned source remains local validation evidence; no private API is used in production.

Validation: both missing presenter/lifecycle tests failed before implementation. Vendor-double transaction/descriptor/failure tests and concurrent Present versus a second Prepare transaction, disabled transaction and destruction passed. Actual RTX 4080 SUPER / pinned runtime passed NewDX12 creation/identity, feature-less startup Present, deferred FG creation, UI unregister/WaitForPresents and ordered destruction. The latest Standard FG-enabled Release build and runnable suite passed 127/127 (25.24 seconds). Actual generated pixels, automatic UI and cadence remain Task 6 acceptance gates; the installed SR mod is unchanged.

## Actual generated-image fixture (Task 6)

The standalone fixture runs 1,000 sources and 25 recreated contexts in each of two modes. The first uses unchanged production automatic UI composition. The second intercepts only the public Configure descriptor in the test executable to install a documented D3D12 Present observer. Both use the production generation callback and actual pinned AMD runtime. No observer or SDK debug-checking default enters release integration.

The observer composes completed premultiplied UI, copies opaque/translucent sentinels and a moving-scene row to dedicated readback buffers, and reads only after SDK WaitForPresents and ordered retirement. Independent expected UI RGB is checked on real and generated images; generated source IDs must belong to enabled prepared sources, rows must have foreground contrast and change within every context. Native UNORM values are bounded; this does not establish intermediate floating-point finiteness. The observer matches the pinned SDK compositor, which uses scene RGB and outputs alpha 1; generated alpha is not scene coverage.

Preserved initial failures include context-free global diagnostics rejected by the loader, unsupported shared-guide heap classification, intentionally missing generated-image observations, and incorrect test-compositor alpha. GlobalDebug1 succeeds on both effect DLL public exports and collects bounded, thread-safe warning/error messages; validation contexts additionally enable SDK debug checking. The process verifies all three runtime SHA-256 pins and absence of Streamline/DLSS runtime modules.

The passing report [FSR_FG_GPU_VALIDATION.json](FSR_FG_GPU_VALIDATION.json) records exact fixture/observer hashes and source base 40692e7: 2,000 submitted sources, 550 Prepare submissions/generation callbacks, 50 retired contexts, 250 actual generated-image readbacks, 975 rendered-image readbacks, 225 changing generated samples and 1,225 UI sentinel checks. Callback counts are not generated-image or display counts: reset submissions can suppress a generated image, and shutdown can discard the last queued real image. Retired GPU memory ranged by 1,003,520 bytes across both modes. No SDK warnings/errors occurred. D3D11/D3D12 debug layers were unavailable and remain unverified.

Automatic-mode appearance and physical display cadence remain **pending controlled visible acceptance before Skyrim deployment**. The hidden automatic phase proves successful source handoffs/retirement; the observer proves callback-mode pixels. Neither proves scan-out, cadence or automatic-compositor pixels. The installed working SR mod remains untouched.

## Host integration boundaries (Task 7 in progress)

AMD selection now has an explicit creation/readiness/retirement route; an unwired route rejects creation instead of entering NVIDIA ownership. The existing two-operation ordinary/NVIDIA adapter remains compatible. The host target owner can retain one native D3D11 scene independently of the inner D3D12 application buffers, and FsrHostResources exposes its retained runtime without duplicate loading. CaptureDedicated freezes completed premultiplied HUD without blending it into the scene, rejecting missing or foreign immediate contexts; existing Compose uses that same capture operation.

The new FsrGenerationHost regression observed failures before each boundary existed and now verifies native scene/HUD pixels, retained producer identity against a real D3D12 chain, stable scene across physical buffer indices and runtime lifetime after SR-owner retirement. These checks cover integration boundaries, not the completed Skyrim host: factory, Present/Present1, resize, status and settings wiring remain pending. Milestone 7 is not complete; game-facing FG admission and the installed working SR package remain unchanged.
