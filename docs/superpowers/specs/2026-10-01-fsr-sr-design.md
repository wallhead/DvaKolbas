# FSR Super Resolution design

Date: 2026-10-01. Status: approved written spec.

## Intent and scope

The user asked to implement FSR after confirming the DLSS frame-generation baseline in Skyrim. They approved sharing TRP's game-facing buffers and native UI, delivering NVIDIA-independent FSR Super Resolution first, then implementing FSR frame generation. The attached `Codex_TRP_FSR_Implementation_Prompt.md` supplies technical requirements; it does not authorize changing the installed game or publishing binaries.

This spec covers the first working milestone and its correctness, lifecycle, build, and packaging requirements. Its output must be genuine temporal FSR: reduced scene color, depth, and motion produce native output consumed before native UI composition. DLL loading alone, native rendering followed by downsampling, and sharpening alone do not meet acceptance.

FSR frame generation follows only after these contracts pass. It will require a separate design for AMD's D3D12 presentation adapter. The SR interfaces reserve source-frame identity, completed UI availability, and reader retirement so that work can be added without nesting presenters.

The checkout matches reference `246d152c42a83308beb7a6f4930e7c472ef0d4ce` (TRP 0.3.5). Work is isolated on `codex/fsr-sr`; the installed V5.4 NO-LORE DLSS baseline remains the regression reference. No installed game or MO2 settings change is part of this milestone.

## Architecture choice

Use the existing stable outer `GameSwapChain`, game-facing target allocation, source-frame coordination, loading behavior, and native UI composition. Introduce focused boundaries for upscaler selection, presentation readiness/lifetime, and common GPU interop. Preserve the NVIDIA implementation behind those boundaries; do not bulk-rename NVIDIA files or rewrite the renderer.

For FSR SR with FG off, the inner presenter is a normal D3D11 DXGI swapchain. A D3D12 device and direct queue on the same adapter run FSR. FSR readiness depends on that presenter, interop, and the FSR context, never on Streamline, NGX, Reflex, DLSS-G, or NVIDIA NR. NVIDIA SDK headers/libraries may remain build dependencies initially, but the FSR route must operate without NVIDIA runtime DLLs.

A duplicate FSR host would reduce changes in NVIDIA files but duplicate early buffer publication, native UI, resize, and retirement logic. A Streamline presenter with only the dispatch replaced would be smaller initially but fail NVIDIA-free operation. The shared host with independent policies is the approved direction.

`docs/FSR_IMPLEMENTATION.md` records verified current ownership and proposed changes separately. It will become the implementation and evidence record as code lands.

## Components and contracts

* A backend-neutral upscaler interface owns configuration, supported dimensions/quality, jitter, feature creation, temporal dispatch, reset, and retirement. Inputs contain explicit render/output extents, resource formats and encoding, source-frame ID, monotonic delta milliseconds, motion/jitter conventions, camera measurements, and history-reset state. No Streamline constants or NGX enums appear in this contract.
* `FSRRuntime` owns optional AMD modules, typed exports, compatible provider enumeration, copied provider names paired with IDs, supported queries, actual created-provider identity, and diagnostic results. It outlives every context and GPU/callback user.
* `FSRUpscaler` owns the context and validates/maps neutral inputs to the pinned SDK descriptors. It records FSR on the production D3D12 command list and reports temporal success, spatial recovery, invalid input, or fatal device/lifetime failure distinctly.
* A presentation policy supplies inner swapchain creation, readiness, generation preparation status, and retirement. Ordinary presentation does not request generation preparation. NVIDIA presentation retains its existing queue/completion and NR contracts.
* Shared D3D11/D3D12 services extract only proven adapter, resource-copy, fence, command-slot, and retirement mechanics. Streamline completion-fence bridging remains NVIDIA-specific. Three reusable submission slots are tracked independently of swapchain backbuffer count.

Common evaluation executes exactly one upscaler. FG preparation has three outcomes: not requested, successful, and failed. FG-off SR is successful without FG camera/preparation readiness. CS external-upscaler ownership skips TRP temporal SR explicitly. ReShade executes once per source frame at its configured before/after position.

## Startup and stable dimensions

Backend-aware configuration validation replaces the unconditional NVIDIA-baseline requirement while retaining obsolete-experiment rejection. Requested and active selections are separate.

During factory interception, acquire the actual D3D11 adapter LUID and create the matching D3D12 device/queue needed for provider capability and size queries. Check shader model, format/UAV support, shared resources, D3D11 device/context fence interfaces, and device identity. Adapter zero and cross-adapter copies are forbidden substitutes.

Resolve render dimensions through the selected FSR provider before allocating/publishing the stable reduced game-facing buffer. Allocate native output and UI separately. Return the existing stable wrapper before the original D3D11 device-creation call returns; its startup Present remains legal. Create the full temporal context and arm valid-frame warmup only after that original call and its startup Present finish. Pre-query device creation does not permit early temporal feature initialization.

The first implementation uses fixed allocations. Quality, provider, backend, format, or output dimension changes require restart unless a tested safe recreation path handles them. Unvalidated dynamic resolution is rejected with a reason.

## SDK and runtime policy

Pin official FidelityFX SDK v2.3.0 to commit `60f4ea81909200d8542eca14dccb2628b763a9a3`. The release asset is `FidelityFX-Samples-v2.3.0-prebuilt.zip`; GitHub advertises SHA-256 `f90890b9323bb2f4f2404ac4cdc9395e8495ecdac6f7aa0bcdf1ad1848422273`. The downloaded archive and individual runtime files still require verification during implementation; no runtime is installed by this design work.

Initial validation selects the compatible analytical FSR 3.1.5 upscaler supplied by this SDK. Supported ML selection is permitted through official provider discovery, never a hard-coded provider ID. Retain compatible IDs and names together, apply consistent provider/device information to pre-query and context creation, and query/report the actual provider after creation. If the requested provider is unavailable, report it rather than silently substituting another.

Derive the five typed dynamic functions (`ffxCreateContext`, `ffxDestroyContext`, `ffxQuery`, `ffxConfigure`, `ffxDispatch`) from pinned C headers. Avoid statically linked API-call helpers. Inspect the actual signed package before deciding filenames and license/redistribution treatment; guide names are `amd_fidelityfx_loader_dx12.dll` and `amd_fidelityfx_upscaler_dx12.dll`.

Load from absolute plugin-relative paths compatible with MO2, using a scoped safe Windows loader policy. Do not change the global DLL directory or download during startup. Distinguish absent DLL, wrong architecture, absent export, ABI/provider incompatibility, capability failure, and context-creation failure. Missing FSR modules must not affect DLSS configurations.

## Resource flow and synchronization

Each source frame follows this dependency sequence:

1. D3D11 finishes scene/effect production and copies or converts color, depth, and motion into shareable inputs. A non-null UI resource means identity only; UI remains unfinished at this point.
2. D3D11 signals a shared producer fence and submits its stream. The D3D12 queue waits on that value before reading.
3. D3D12 transitions inputs to the SDK-required read states and output to UAV, dispatches FSR, restores agreed interop states, executes, and signals completion.
4. D3D11 waits on completion before sampling/copying native output, drawing/compositing the completed native UI, or overwriting shared inputs.
5. Reuse and destruction wait for all later output/UI/presentation readers as well as dispatch completion.

Resource state and recorded fence values belong to each slot. Allocators/descriptors/resources cannot reset merely because a new backbuffer is current. Shared textures use legal single-sample formats; depth, typeless, channel, encoding, and size mismatches require explicit GPU conversion. Raw copies are not conversions. Preserve D3D11 bindings/state needed by Skyrim, ENB, ReShade, and UI, and remove read/write binding hazards.

Normal dependencies use queue/context GPU waits. No per-frame resource allocation, provider discovery, disk logging, CPU readback, Sleep, busy polling, or full-device drain is added. CPU waits are restricted to bounded slot reuse and lifecycle retirement, with progress-aware diagnostics and device-removal handling. A timeout is not retirement: do not fake completion or release potentially live objects.

## Temporal and image correctness

Audit the existing TAA bypass and projection jitter producer. Route quality/render-size and jitter selection through the active backend. Use one source-frame index, never a generated-frame counter. Dispatch only one temporal pass for the scene. Apply sharpening once: the FSR sharpness control maps to its supported sharpening, and the existing separate RCAS must not also run on that route. Apply one deliberate mip-bias policy, derived from actual render/output ratio and checked against existing hooks.

The parameter adapter validates finite positive monotonic frame delta in milliseconds, finite camera/jitter/motion metadata, near/far/FOV, actual depth convention, and extents/subrects against allocations and provider limits. Camera measurements are shared beneath the NVIDIA parameter conversion. Never use fabricated zero motion/depth for gameplay.

Source inspection currently describes Skyrim motion as normalized, but that is insufficient acceptance evidence. Controlled camera translation and moving-object captures must establish direction, Y, units, resolution, and jitter inclusion. Compare projection jitter with the SDK offset. Validate normal/reversed depth and finite/infinite far planes against actual camera/resource behavior. Reject inconsistent inputs instead of guessing signs/scales.

Record the ENB/non-ENB handoff's color encoding, exposure/pre-exposure, and alpha contract. Choose provider flags/conversions from that evidence; UNORM and monitor HDR capability do not determine scene encoding. Initially expose SDR FSR only and reject unvalidated FSR HDR/NR combinations with clear reasons. NVIDIA NR/HDR behavior remains available on supported NVIDIA paths.

Support Quality, Balanced, Performance, and native AA where the actual provider supports them. Query provider sizing/jitter behavior, including odd and ultrawide dimensions. The first mask path supplies no reactive/transparency masks and documents that limitation. Transparent-effect image quality requires gameplay validation; no speculative opaque capture or mask is advertised as validated.

## History, recovery, and lifetime

Reset on first valid temporal frame, camera discontinuity, new game/load, exit from spatial loading, relevant dimension/format/provider change, device recreation, and re-entry after skipped invalid frames. Use existing transition signals; do not reset on every normal frame. Loading artwork retains the spatial route. FG remains off throughout this milestone.

Before reduced-buffer ownership is committed, an unavailable FSR setup produces an actionable startup rejection. Do not silently switch to DLSS or label a fallback FSR. If a recoverable temporal dispatch problem occurs after reduced targets exist, use the existing spatial-to-native route only after its format/extent/output contract is validated, disable generation, and log the requested versus active recovery state. Re-entry requires valid inputs and a deliberate reset. If spatial recovery cannot safely operate, enter the existing fault path.

Resize, shutdown, failed creation, and recreation retire producer, dispatch, output, and presentation readers before freeing objects or unloading modules. Device loss and retirement failure use the safe fault path. Rate-limit repeated diagnostics but retain the first failure and state transitions.

## Settings and compatibility

Preserve `DLSS = 0`, `DLAA = 3`, existing defaults, plugin identity, and `SolFG_*` ABI. Use `FSR = 4`; legacy values 1 and 2 remain rejected. The reachable baseline history of `UpscaleType.h` contains only its initial publication (`8164c1a`), with the same DLSS/DLAA values. Searching reachable source/package history for an FSR/upscaler selector assigned 4 found no collision. This audit does not claim to cover unpublished forks or disconnected branches.

Expose one authoritative upscaler selector and a separate generation backend/enabled selector. Expose FSR quality, analytical/compatible provider policy, actual runtime/provider status, and sharpening. Existing Apply / Save as default / Discard behavior applies. Restart-required changes stay pending until successful startup/recreation; validation never edits user settings silently.

In the first milestone, FSR supports ordinary presentation with FG off. FSR plus NVIDIA FG, FSR plus NR, and unvalidated HDR combinations are explicitly unavailable. Existing NVIDIA combinations retain their prior meaning. A later FSR FG design covers FSR SR + FSR FG and validated DLSS/DLAA + FSR FG with one pacing owner, matching source-frame IDs, completed dedicated UI, safe callbacks, and async retirement.

## Verification and delivery gates

Contract tests precede ownership changes and cover legacy settings, FSR-only NVIDIA-call absence, loader failure distinctions, provider identity, parameter mapping and NaN rejection, exactly one temporal upscale, CS external ownership, not-requested FG preparation, source-frame/ReShade ordering, delayed reader retirement, transition recovery, and settings lifecycle. Existing NVIDIA regression tests remain meaningful; do not weaken them to make new routing pass.

A standalone real-GPU D3D11/D3D12 fixture uses the production adapter and official runtimes, synthetic reduced color/depth/motion, and test-only native output readback. Require finite, nonempty, changing output at expected dimensions, multiple frames in flight, at least 1,000 frames and 25 recreation cycles, bounded resource growth, and debug-layer error collection when available. WARP/mocks do not prove FSR image or cross-vendor acceptance.

Add `TRP_ENABLE_FSR` (off by default for unchanged builds) and pinned `TRP_FSR_SDK_DIR`. FSR-off builds need no FSR SDK. Build Standard/Universal and NR-on/off variants with compatibility test targets built before CTest. Inspect final DLL imports and clean-process startup with NVIDIA runtime files absent. A configuration flag alone does not pass that gate.

Stage packages in the worktree, separately from the live game. Preserve the default NVIDIA package, and provide an FSR-only example package/configuration without a hidden NVIDIA runtime dependency. Record actual runtime filenames, architecture, versions, hashes, signing/license information, SDK pin, concrete commands/results, and source revision in `docs/FSR_IMPLEMENTATION.md` and a runnable checklist.

Skyrim acceptance is a separate level of evidence: actual reduced scene targets/viewports; native HUD/menus/subtitles/cursor/previews/loading art; ENB and non-ENB; ReShade before/after; CS ownership; camera/object motion; transparencies; quality/native AA/odd/ultrawide dimensions; load/fast travel/camera changes; alt-tab/minimize/resize; repeated lifecycle. Measure matched scenes with FG off, separating bridge, conversion, FSR, and presentation GPU costs, CPU timing, VRAM, and stalls. Do not promise FPS improvements in CPU-bound scenes.

The available GPU is RTX 4080 Super. AMD/Intel validation is not run and cannot be inferred from it. Windows Graphics Tools installation previously failed; three baseline debug-dependent checks remain unvalidated. Continue feasible checks without changing system permissions or retrying that installation; mark unavailable layers SKIPPED. Existing baseline build/initial gameplay evidence is not evidence that FSR works.

SR is complete only when real dispatch and output consumption, NVIDIA-independent startup/presentation, safe resource lifetime, build/package delivery, and feasible tests are implemented, with remaining gameplay/hardware gates explicitly identified. No claims of FSR FG, physical presentation cadence, or cross-vendor acceptance are made by this spec.

## Sources

* User reference: `C:/Users/user/Downloads/Codex_TRP_FSR_Implementation_Prompt.md`.
* [Official SDK v2.3.0 release](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/releases/tag/v2.3.0).
* [Official FFX API guide](https://gpuopen.com/manuals/fsr_sdk/getting-started/ffx-api/).
* [Official upscaler guide](https://gpuopen.com/manuals/fsr_sdk/techniques/super-resolution-upscaler/).

## Review state

The conversational design direction and this written spec are approved. The user also authorized pushing to `https://github.com/wallhead/DvaKolbas`. The user’s “next step” resumed implementation inline in the existing FSR worktree. This document records intended behavior; executed evidence is kept in FSR_IMPLEMENTATION.md and the task ledger.
