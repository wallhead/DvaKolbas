# FSR frame generation: first integration milestone

Status: approved by the user's “proceed.” The [implementation plan](../plans/2026-10-02-fsr-fg.md) is ready for review. No FG implementation or activation is implied.
Base: working FSR SR code `8e78f5136772fbbbb23c2aafe341ab931256c475`; V5.4 NO-LORE.

## Intent and scope

The user approved shared TRP buffers and native UI, NVIDIA-independent FSR SR first, then FSR frame generation. Loaded-world SR now works; the user confirmed the corrected NR tab no longer crashes. The next milestone implements FSR SR plus FSR FG, producing one interpolated frame between source frames when the provider accepts generation. FG remains opt-in and off in the installed SR mod until a separately validated test package is ready.

First-stage limits: SDR, fixed render/output dimensions, dedicated native UI, Analytical SR, and no NR. Loading/spatial recovery and invalid camera/guides suppress generation. DLSS/DLAA plus FSR FG, HDR, dynamic resolution, Community Shaders ownership, multiple generated frames and async-compute optimization are later milestones. Existing NVIDIA presentation retains its contracts. No general host rewrite is part of this milestone.

SR save/load, fast travel, alt-tab/minimize, UI previews and ENB/Gamma22 calibration remain open acceptance items. They can be checked while the standalone FG work proceeds; a Skyrim FG trial requires those results to be recorded, with any defects repaired or restrictions stated.

## Chosen approach

Use the official SDK's D3D12 interpolation swapchain behind TRP's stable game-facing D3D11 wrapper. Reuse the matching-adapter bridge and temporal guides. AMD owns final generation/pacing and UI texture composition; TRP owns scene production and completion boundaries.

Alternatives considered: a custom D3D11 pacer would require implementing presentation scheduling and retirement ourselves; reusing NVIDIA's presenter would preserve an unwanted NVIDIA dependency and impose a different completion contract. The official AMD presenter is the preferred architecture, subject to the standalone fixture proving it works with this bridge.

The SDK is pinned to the existing v2.3.0 commit `60f4ea81909200d8542eca14dccb2628b763a9a3`. Local archive inspection confirms `amd_fidelityfx_framegeneration_dx12.dll` is present (40,085,776 bytes). The current package pins only loader/upscaler modules. Before loading FG, extend acquisition to verify the FG module's exact hash, signature, architecture, exports, header ABI and actual FG/swapchain provider identities. Do not infer a provider version from the upscaler or require a speculative separate swapchain DLL.

## Components and integration boundaries

* Extend `FSRRuntime` with separately requested FG module lifetime and provider queries. SR-only startup must work without FG files. Every context and callback owner retains the runtime until retirement and destruction.
* Add an FSR generation adapter for typed context creation, version descriptors, `PrepareV2`, per-source-frame configuration and generation dispatch. Inputs use the existing neutral camera, motion, depth, time and reset measurements.
* Add an AMD presentation implementation alongside `OrdinaryPresentation`. It owns the D3D12 proxy swapchain, queue dependencies, native scene/UI transport, and retirement. Presentation policy selection is independent of the upscaler selection.
* Adapt `GameSwapChain` and the host only where the AMD path needs different transport. Return the original D3D11 producer device from the game-facing `GetDevice` on this path. Its `GetBuffer` still exposes TRP's stable D3D11 render target. Never cast an AMD D3D12 backbuffer to a D3D11 texture or run the ordinary D3D11 `GetBuffer(0)` cache on it. Internal AMD backbuffers use actual D3D12 indexing and explicit copies from shared resources.
* Keep native UI completion in the existing host boundary. Transport a completed, native-size UI texture and the HUD-less scene separately. Overlay UI is rendered on the source thread and included in the completed foreground; AMD worker threads must never call Skyrim, ImGui or the D3D11 immediate context.

Exactly one real presenter owns the HWND. Decide the requested presentation policy before publishing the outer wrapper; deferred startup and startup Present remain legal. No late replacement of targets already cached by the game.

## Frame contract

1. D3D11 finishes scene/effect production. Capture/convert guides using the existing source-frame measurements, then signal the producer fence. D3D12 waits before reading those resources.
2. Perform exactly one SR pass. FG preparation uses the same source ID, reset, extents, jitter, motion convention and camera values. Delta time is milliseconds; provider-required units and color encoding are explicit conversions rather than assumptions.
3. Deliver a native-size, HUD-less scene to AMD's application backbuffer. Run ReShade at its selected before/after position exactly once per source frame. Generated frames do not rerun Skyrim effects.
4. At the existing native UI completion boundary, finish the dedicated HUD and overlay foreground, copy/convert into a shareable UI texture, and signal its completed producer work. Merely knowing the UI texture address never makes it ready.
5. Configure FG and publish scene/UI with matching source IDs. Use SDK UI texture composition with its documented internal UI buffering, validated with sentinel pixels and delayed readers. Configure, PrepareV2 and generation dispatch for a source frame must agree; discontinuities reset history. Source IDs never increment for generated presents.
6. Present through AMD's proxy. Initially use direct generation dispatch on the main D3D12 graphics queue, with async compute disabled. SDK UI/presentation may still execute asynchronously; retirement must cover those readers too.

Resource states and submission values belong to their slots. SR dispatch completion alone is not sufficient to overwrite guides, scene or UI. Preserve producer device identity across ReShade wrappers. No per-frame resource allocation, CPU readback, sleeps or full-device drains belong in production.

## Compatibility, state and failures

ReShade/ENB compatibility is a delivery gate. The new inner D3D12 presenter must preserve the public native-chain ownership/effect-routing contract already used by TRP. The integration may not leave an automatic ReShade effects pass alongside TRP's manual pass. A configuration whose ownership cannot be established is rejected with an actionable reason; the installed ordinary SR package remains available. Validate using the actual V5.4 ReShade 6.8 runtime and then ENB in Skyrim.

Retain generation backend values 0 (ordinary/off) and 1 (NVIDIA). Audit reachable source/package history before assigning a new FSR value; unknown values remain rejected. Restart is required to change presenter ownership. Within an AMD-presenter session, enabled/disabled requests affect generation without replacing the swapchain; disabled generation must still compose native UI correctly. UI status reports requested and active generation, provider, suppression reason and errors. Apply/Save/Discard use the existing authoritative settings flow. Unsupported NR remains unavailable safely.

Suppress generation during menus/loading, skipped or spatial frames, missing guides, invalid camera state and warmup after resets. Resume only with valid sequential temporal inputs and an acknowledged reset. Generation failure may retain SR-only presentation through the existing AMD proxy only when that route remains safe; device loss, presentation failure or failed retirement use the host fault path. Never silently switch presenter ownership mid-session.

Resize/shutdown first stop new dispatches, disable FG, wait for SDK presentation/UI users and recorded bridge work, then destroy contexts/resources. Keep callbacks/modules alive throughout. A timeout is a failure, not completed retirement. Coordinate SDK operations using the pinned API's thread-safety rules; no render-thread mutex may deadlock the proxy's retirement or worker callbacks.

## Verification and handoff

Begin with contract tests for policy/legacy settings, matching IDs, milliseconds and reset mapping, menu/spatial suppression, UI completion, module absence, and delayed-reader lifetime. Tests must fail on the old unsupported FG policy before changes land.

Next use a standalone real-GPU fixture with the actual D3D11 producer, shared textures/fences, official FG contexts and AMD presenter. Check at least 1,000 source frames and 25 recreation cycles; changing scene pixels; native UI sentinels on source and generated images; bounded resource growth; FG off/on/reset transitions; and deliberate delayed readers. Capture actual generated/presented outcomes and cadence separately from dispatch success. Hidden-window dispatch alone does not prove physical display pacing. Use a user-visible controlled presentation check only when requested.

Then integrate the host and repeat actual V5.4 ReShade fixtures, existing SR/NVIDIA regressions, FSR-disabled builds, imports, pinned runtime/package validation and clean-process absence of NVIDIA runtime dependencies. The three unavailable Graphics Tools checks stay marked unavailable; do not retry installation. Cross-vendor GPU acceptance remains untested until corresponding hardware is available.

Only after those gates pass stage a separate FG test package, retain the current SR DLL/configuration for rollback, and install under the user's existing V5.4 scope while Skyrim is closed. Preserve MO2 launch/profile settings. The user launches Skyrim manually for image/HUD/menus, load/fast-travel, alt-tab/minimize and measured pacing/latency checks. No FPS or quality improvement is claimed before measurement.

## Evidence

* [Pinned AMD FG API](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/docs/techniques/frame-interpolation-api.md): typed preparation/configuration, source IDs, UI lifetime and thread-safety obligations.
* [Pinned AMD interpolation swapchain](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/docs/techniques/frame-interpolation-swap-chain.md): D3D12 presentation/pacing and UI handling.
* [Prior SR design](2026-10-01-fsr-sr-design.md), [Skyrim results](../../FSR_SKYRIM_TEST_LOG.md), [qualified AIO RE evidence](../../FSR_AIO_RE_NOTES.md). RE informs research; pinned public APIs and actual fixtures determine implementation.
