# Implement FSR in Theo's Render Pipeline

You are implementing this feature, not merely researching feasibility or writing a proposal.

Repository: https://github.com/theosw/theosrenderpipeline
Reference revision: `246d152c42a83308beb7a6f4930e7c472ef0d4ce` — release 0.3.5, October 1, 2026.

Work from the user's actual checkout. Record its revision and reconcile differences from the reference before editing. Preserve unrelated modifications. Follow AGENTS.md and existing project conventions. Use an isolated branch/worktree where appropriate; do not push, publish a release, or alter the user's installed game without explicit authorization.

## 1. Objective and delivery order

Add a real AMD FSR backend while preserving the existing DLSS/DLAA renderer, ENB integration, ReShade integration, and native-resolution UI.

Deliver in this order:

1. **FSR Super Resolution with frame generation off, including NVIDIA-independent startup and presentation.** This is the mandatory first working milestone.
2. **Correctness, lifecycle, packaging, and regression coverage for that path.** Missing NVIDIA runtime DLLs must not prevent FSR operation on a supported adapter.
3. **FSR frame generation as a separate selectable backend.** Implement after the first path's contracts are established; keep it experimental and off by default until runtime acceptance is demonstrated.

Initially validate the SDK's analytical FSR 3.1.x upscaler on supported NVIDIA/AMD/Intel hardware. Permit supported ML providers through the official API, but query and report what actually runs. Do not confuse the SDK release, upscaler version, frame-generation version, and swapchain provider version.

An RTX 4080 Super is the user's available baseline GPU. Do not claim AMD or Intel compatibility was tested merely because an RTX run passes.

Do not substitute FSR 1, a sharpening shader, a driver toggle, a renamed DLSS option, or downsampling a native-rendered image for real reduced-resolution temporal upscaling.

## 2. Inspect these integration points first

These paths existed at the reference revision; locate their equivalents if upstream changed:

- `src/UpscaleType.h`: preserves `DLSS = 0` and `DLAA = 3`.
- `src/NvidiaBaselinePolicy.h` and its caller in `src/XSEPlugin.cpp`: currently reject non-NVIDIA ownership/configuration.
- `src/FrameGen/NvidiaHostStartup.cpp`: swapchain creation, stable game-facing buffers, render-size selection, deferred upscaler initialization.
- `src/FrameGen/NvidiaHost.cpp`, `NvidiaHost.h`, and `GameSwapChain.*`: frame and presentation ownership; inspect lifecycle methods before changing them.
- `src/FrameGen/SourceNvidiaEvaluation.cpp`: `EvaluateDLSS()`, loading-screen spatial route, frame input assembly, native UI resource publication.
- `src/FrameGen/SourceNvidiaFrameEvaluator.h`: ordering of input copy, optional NR, ReShade, upscaling, and frame-generation preparation.
- `src/FrameGen/SourceNvidiaFramePreparation.h`, `SourceGenerationPolicy.h`, and `SourceFrameGeneration.*`: preparation and generation gating.
- `src/FrameGen/SourceDLSSGInterop.*`, `SourceDLSSGBackend.*`, and `SourceDLSSGSession.*`: separate reusable resource/fence mechanics from NVIDIA-specific lifetime contracts.
- `src/DLSSBackend.*`, `src/RenderPipeline.*`, `src/DRS.*`, and `src/CommunityShaderIntegration.cpp`: resource production, jitter, settings, and external upscaler ownership.
- `CMakeLists.txt`, `cmake/BaselineSources.cmake`, `CMakePresets.json`, `package/SKSE/Plugins/TheosRenderPipeline.ini`, installation documentation, and third-party notices.

Trace the startup, per-source-frame, native-UI, Present, resize, and shutdown paths. Search all uses of DLSS-only enums, NVIDIA readiness checks, Streamline constants, runtime loading, quality selection, and failure reporting. A modified dispatch call alone is insufficient.

Record a compact dependency/ownership map in `docs/FSR_IMPLEMENTATION.md`, with current file/function references. Distinguish verified code behavior from assumptions that need a capture.

Critical existing contracts to preserve:

- The game-facing stable buffer is published during factory interception, before D3D11 creation returns. Do not install the wrapper too late.
- Full upscaler initialization is deferred until the original D3D11 device-creation call, including its startup Present, has returned.
- Non-CS gameplay separates reduced scene dimensions from native output/UI dimensions.
- The UI texture's identity is published before its contents finish drawing. A non-null pointer is not proof of complete, consumable UI data.
- Existing loading, warmup, transition, GPU retirement, and ReShade ownership protections are intentional.

## 3. SDK selection and loading

Start by inspecting the official FSR SDK **v2.3.0** release and matching headers/sample. Pin a release/commit and runtime hashes. Adopt another release only for a documented technical reason; do not depend on a moving branch.

References:

- https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/releases/tag/v2.3.0
- https://gpuopen.com/amd-fsr-sdk/
- https://gpuopen.com/manuals/fsr_sdk/getting-started/ffx-api/
- https://gpuopen.com/manuals/fsr_sdk/techniques/super-resolution-upscaler/
- https://gpuopen.com/manuals/fsr_sdk/techniques/frame-interpolation-api/

Use the official signed loader/effect runtimes. Verify filenames in the actual package: documentation may use shortened names, while the API guide names `amd_fidelityfx_loader_dx12.dll`, `amd_fidelityfx_upscaler_dx12.dll`, and `amd_fidelityfx_framegeneration_dx12.dll`. Do not rename DLLs to match an assumption.

Prefer optional runtime loading through a typed function table for `ffxCreateContext`, `ffxDestroyContext`, `ffxQuery`, `ffxConfigure`, and `ffxDispatch`. Derive declarations from the pinned headers. Do not accidentally use statically linked C++ API-call wrappers when implementing dynamic loading.

Use plugin-relative absolute paths and an appropriately scoped Windows loader search policy. Do not globally change the process DLL directory, load from arbitrary working directories, or auto-download binaries during game startup. Respect MO2's existing plugin-path resolution.

Enumerate compatible providers, retain copied names alongside IDs, and query the created context's actual provider. Never hard-code provider IDs. Use consistent provider/device information for pre-creation queries and context creation; initialize descriptor types and extension chains correctly.

Check all API returns. Log unavailable DLLs, exports, providers, device capabilities, and creation failures distinctly. Keep loaded modules alive until contexts, callbacks, and GPU users retire. Missing FSR runtimes must not break existing NVIDIA configurations.

## 4. Architecture: separate upscaling from presentation

Introduce the smallest useful boundaries; do not rewrite the renderer or bulk-rename NVIDIA files just for appearance.

Separate these responsibilities:

- **Upscaler selection:** DLSS/DLAA, FSR, or an explicit fallback/external owner.
- **Presentation and frame generation:** ordinary presentation, existing NVIDIA presentation, or AMD frame-generation presentation.
- **Shared GPU services:** adapter selection, D3D11/D3D12 resources, submission, synchronization, and retirement.

The mandatory combination is `FSR SR + frame generation off` without any NVIDIA runtime dependency. Preserve existing NVIDIA combinations. After AMD FG works, support `FSR SR + FSR FG`; support `DLSS/DLAA + FSR FG` where the inputs and ownership can be validated. Additional cross-vendor combinations are not a prerequisite for the first milestone and must be explicitly unavailable rather than silently misrouted.

Suggested new modules, adapting names to repository conventions:

- `src/Upscaling/UpscalerBackend.h`: backend-neutral configuration, frame-input contract, results, and capabilities.
- `src/Upscaling/FSRRuntime.{h,cpp}`: DLL function table, provider queries, diagnostics.
- `src/Upscaling/FSRUpscaler.{h,cpp}`: context lifecycle, parameters, recording dispatches.
- `src/Graphics/D3D11D3D12Interop.{h,cpp}`: common bridge, only if extraction is justified.
- `src/FrameGen/FSRFrameGeneration.{h,cpp}` and a focused presentation adapter for milestone 3.

Do not leak `sl::Constants` or NGX enums into FSR/common interfaces. Share underlying camera measurements, not NVIDIA's parameter structs. Give frame inputs explicit units, extents, formats, color conventions, source-frame identity, reset state, and resource-validity/lifetime semantics.

The common evaluator must distinguish “frame-generation preparation not requested” from “preparation failed.” Successful SR with FG off must not produce an error every frame.

## 5. NVIDIA-independent startup and presentation

Replace unconditional NVIDIA-baseline validation with backend-aware validation; do not simply remove validation altogether. Preserve rejection of unrelated obsolete experiments.

Provide a real non-Streamline presentation route for FSR-only operation, reusing the stable game-facing wrapper, native UI, and lifecycle code. An ordinary D3D11 inner swapchain plus a same-adapter D3D12 upscaling device is a candidate for SR-only operation; justify the selected approach in the ownership map.

Audit plugin loading and startup for hard imports, static initialization, mandatory NGX calls, Reflex setup, NR configuration, NVIDIA adapter checks, and default backend access. Inspect the resulting DLL's imports/dependencies and test a clean process without NVIDIA runtime files. A configuration flag is not proof of independence.

NVIDIA SDKs may remain build-time dependencies initially if needed to preserve the existing build; clearly distinguish this from forbidden runtime dependence of the FSR-only route.

Resolve FSR render dimensions before publishing game-facing targets. Use the chosen provider's supported sizing contract; never call NVIDIA's sizing function for FSR. If the API needs a D3D12 device for a pre-creation size query, satisfy that dependency without violating the tested deferred feature-initialization boundary.

Choose the actual D3D11 adapter by LUID. Verify D3D12, shader model, resource formats, sharing, and fence support. Do not choose adapter zero, force an AMD adapter, use cross-adapter copies accidentally, or treat WARP as equivalent hardware acceptance.

## 6. Resource bridge and synchronization

Reuse proven pieces of `SourceDLSSGInterop`, but do not inherit its Streamline completion-fence or DLSS-G queue-mode assumptions.

Define and document a complete dependency sequence:

1. D3D11 finishes producing/copying the current inputs.
2. D3D11 signals the appropriate shared fence; D3D12 waits before reading.
3. D3D12 transitions resources, records FSR, restores agreed interop states, executes, and signals completion.
4. D3D11 waits before sampling/copying the result or reusing shared inputs.
5. Any later FG/presentation readers retire before reuse or destruction.

Use GPU queue/context waits for normal dependencies. Do not add `Sleep`, busy loops, routine full-device drains, or CPU readback to the production frame path. Ensure D3D11 work is actually submitted when required; a signal sitting in an unsubmitted stream must not deadlock D3D12.

Track resource state and ownership explicitly. Use legal shared descriptors and real format conversion passes where necessary. A raw copy does not convert depth, channel ordering, gamma, or dimensions. Handle incompatible typeless/depth/MSAA sources deliberately.

Keep allocator, descriptor, and shared-resource slots alive through their recorded fence values; backbuffer count and submission-slot count are not interchangeable. Test multiple frames in flight and delayed readers.

Make resize, provider/quality changes, shutdown, and error unwinding retire all relevant queues and callbacks before releasing resources. Preserve progress-aware stall diagnostics and device-removal handling. Never signal fake completion to escape a wait or free potentially in-flight objects as if a timeout proved retirement.

Preserve the D3D11 state needed by subsequent engine, ENB, ReShade, and UI work. Detect resource binding hazards. Add D3D debug-layer validation to the standalone fixture.

## 7. Real FSR SR dispatch

Replace the direct DLSS operation in the common evaluator with backend selection while preserving the surrounding pipeline. FSR must consume the current genuinely reduced scene and produce native output before native UI composition.

Do not execute DLSS, Skyrim temporal AA, and FSR sequentially on the same scene. Audit the actual TAA/jitter bypass rather than assuming changing the enum is enough.

Create a validated parameter adapter with tests for:

- Current color, depth, motion, output, optional exposure and masks.
- Actual render/output extents, active subrects, and allocation maxima.
- Motion direction, Y convention, units, resolution, and jitter inclusion.
- Jitter applied to projection versus the offset passed to FSR; one source-frame index, not a generated-frame counter.
- Normal/reversed depth, finite/infinite far plane, near/far values, vertical FOV, and world-unit conversion where required.
- Positive finite source-frame delta in milliseconds, independent of paused simulation time.
- Exposure/pre-exposure, HDR flags, and input/output color encoding.
- History reset, sharpening selection, and provider-specific resource requirements.

Prove motion/jitter conventions with controlled camera translation and moving-object tests. Do not paste `renderWidth/renderHeight` scale values or negate motion solely because another backend does so. Do not invent replacement zero motion/depth for normal gameplay.

Document ENB/non-ENB color at the selected handoff. If the provider needs a different encoding or UAV-compatible format, use an explicit GPU conversion/intermediate and restore the downstream contract. Do not infer color space from `UNORM` alone or set HDR because the monitor supports it. Preserve TRP's distinction between SDR processing and its optional final HDR-output conversion.

Query/match quality and jitter behavior to the selected provider. Support Quality, Balanced, Performance, and native-AA where available. Validate unusual/ultrawide/odd dimensions. Initially reject unsupported dynamic-resolution changes explicitly rather than silently desynchronizing allocations, jitter, and dispatch sizes.

Apply sharpening once. Preserve existing controls or provide a clear FSR-specific mapping; avoid stacking FSR sharpening with the existing RCAS stage unintentionally. Audit existing mip-bias handling before adding another adjustment.

For reactive/transparency masks, first ship a correct documented absent-mask path. Investigate a valid opaque-only capture and post-transparent capture. Never manufacture an opaque input by reusing the same final image, or present a speculative mask heuristic as validated. Test transparent effects before claiming production image quality.

## 8. Preserve UI, ENB, ReShade, and CS ownership

Keep the existing native UI pipeline. Validate HUD, subtitles, menus, world-space widgets, cursor hit-testing, inventory/spell previews, loading artwork, scissor/viewport transforms, and external ImGui overlays.

Preserve exactly one ReShade execution at the configured source-frame position, with correct color/depth sizes. Do not rerun gameplay effects for generated frames or create another competing ReShade runtime.

NVIDIA NR is not an FSR prerequisite. Preserve it for existing supported NVIDIA routes; disable it with a clear explanation for unvalidated FSR-only combinations. Do not silently load NVIDIA just to service unused NR settings.

When Community Shaders owns upscaling, TRP must not perform a second SR pass. Report external ownership in the UI and maintain the existing boundary. AMD FG with CS is a later compatibility validation, not evidence that TRP's FSR SR ran.

Preserve existing HDR behavior on unaffected paths. Keep unvalidated FSR HDR combinations explicitly gated; do not advertise support based only on swapchain format.

## 9. History, transitions, and failure policy

Create/reset temporal contexts deliberately for first use, camera discontinuities, load/new-game transitions, spatial-loading-route exit, relevant dimension/format/provider changes, device recreation, and re-entry after skipped invalid frames. Use existing transition signals where possible; do not reset history every frame as an artifact workaround.

Keep FG blocked during incomplete inputs, warmup, loading, resize, and unsafe transitions. Re-enter only with valid current data and deliberate history reset.

Define failures before coding recovery:

- FSR unavailable before ownership changes: select an explicitly reported supported fallback, or reject configuration with an actionable startup error.
- SR dispatch unavailable after reduced targets exist: use a validated spatial-to-native recovery path with FG disabled, not an unscaled copy or indefinite black frame.
- Device loss or retirement failure: use the existing safe fault path; do not pretend normal rendering can continue on invalid resources.

Distinguish requested backend from active backend. Rate-limit repeated errors, but retain the first failure and state transitions. Do not switch providers silently or call a fallback “FSR active.”

## 10. Settings and compatibility

Preserve old INI numeric meanings, defaults, plugin identity, and `SolFG_*` companion ABI. Allocate a new FSR mode value only after checking current/historical collisions; do not renumber DLSS/DLAA.

Expose one authoritative upscaler selection, separate FG backend/enabled selection, FSR quality/provider policy, runtime status, and sharpening behavior. Avoid competing legacy/new settings that can each claim ownership.

Use the existing Apply / Save as default / Discard model. Mark backend, swapchain, or allocation changes as restart-required unless safe live recreation has been implemented and tested. Do not show a selection as active before the transition succeeds.

FSR FG defaults off. Existing installations retain their previous renderer. Unsupported combinations show the reason; do not silently change user settings during validation.

Document new settings and ship coherent example FSR-only and FSR-FG configurations without changing the default NVIDIA package unexpectedly.

## 11. FSR frame generation: separate implementation stage

After SR and common lifecycle tests pass, implement the official AMD D3D12 FG/presentation integration. A D3D11 swapchain pointer cannot simply be treated as the AMD D3D12 proxy. Build a presentation adapter that preserves the D3D11-facing contract while AMD receives the correct D3D12 queue/resources.

Use the pinned SDK's current creation extensions and preparation structures. For SDK 2.3, inspect the required FG version descriptor and `ffxDispatchDescFrameGenerationPrepareV2`; do not paste a deprecated prepare example. Keep configuration/preparation/dispatch source-frame IDs consistent.

Start with one generated frame per source-frame interval. Do not add unsupported MFG modes. Use one FG/pacing owner, never nested NVIDIA and AMD FG proxies. Reflex or NVIDIA NR must not become dependencies of AMD-only operation.

Prefer TRP's completed dedicated UI texture, not interpolated HUD pixels. Validate alpha convention and compose exactly once on both source and generated images, including when generation is disabled but AMD's presenter remains active.

Resolve the early-publication hazard: synchronize/copy/register UI only when that frame's drawing is complete. Enable the SDK's supported internal UI buffering or implement proven external buffering/retirement. Follow documented asynchronous lifetime and shutdown rules even when async interpolation itself is disabled.

Keep callbacks free of Skyrim/Scaleform game-thread work. Serialize context mutation/presentation according to the SDK contract without introducing callback re-entrancy deadlocks. Test toggles, queued frames, shutdown, and resizing.

Validate effects that distort scene coordinates against depth/motion. Do not claim arbitrary ReShade distortion is compatible without the required correspondence or distortion data.

Add source-frame, generated-frame, and actual presentation diagnostics separately. Do not claim physical cadence, latency improvement, or exact frame multiplication from an overlay counter alone. Leave FG experimental where hardware/gameplay verification is unavailable.

## 12. Tests: write contract tests before changing ownership

Extend existing test infrastructure under `TRP_BUILD_COMPATIBILITY_TESTS`. Relevant existing coverage includes source-frame evaluation, game-facing targets, native UI copy/blend, DLSS quality, loading fades, Community Shaders frames, ReShade lifecycle, plugin paths, and interop fences.

Create focused new tests; these names describe required behavior, not necessarily fixed filenames:

- `LegacyConfigPreserved`: existing DLSS/DLAA and Standard/Universal settings retain semantics.
- `FsrDoesNotRequireNvidia`: FSR-only selection neither requires nor attempts unused NVIDIA initialization.
- `FsrMissingRuntimeIsExplicit`: missing/wrong-architecture DLL, missing export, incompatible ABI/provider, and creation failure yield distinct recoverable results.
- `FsrProviderIdentity`: paired IDs/names, consistent query/create override, actual-provider reporting.
- `FsrParameterMapping`: units, milliseconds, jitter/motion signs, depth modes, FOV, extents, and invalid/NaN input rejection.
- `UpscaleExactlyOnce`: backend routing executes one temporal pass; CS ownership skips TRP SR.
- `NoGenerationIsNotFailure`: FG-off SR succeeds without requiring FG camera/preparation readiness.
- `SourceFrameOrdering`: copy/effects/upscale/UI/prepare ordering and no duplicated ReShade execution.
- `RetirementBeforeReuse`: delayed input/output/UI readers prevent overwrite or premature allocator reset.
- `TransitionRecovery`: spatial loading, invalid input, reset, and controlled temporal re-entry.
- `SettingsLifecycle`: Apply/Discard/Save and restart-required states do not misreport active settings.
- `FsrFgLifecycle`: single FG owner, matching IDs, completed UI acquisition, safe disable/resize/shutdown.

Mocks prove routing/contracts, not FSR image quality or GPU synchronization.

Add a Windows D3D11/D3D12 standalone smoke fixture using a real supported GPU, the production FSR adapter, and official runtimes. Exercise synthetic color/depth/motion, reduced-to-native dispatch, output readback for validation, multiple frames in flight, repeated context lifecycle, and debug-layer error collection. Require changing, nonempty output with expected dimensions and finite values for the controlled fixture. Exercise at least 1,000 frames and 25 recreation cycles, checking for unbounded resource growth and unexpected debug-layer errors. Production must not perform that readback.

Report absent runtime/hardware as SKIPPED, not PASSED. WARP-based common-code tests remain useful but are not cross-vendor FSR acceptance.

## 13. Build and package reproducibly

Add an explicit FSR build option and pinned SDK path convention, such as `TRP_ENABLE_FSR` and `TRP_FSR_SDK_DIR`, adapting to project style. FSR-disabled builds must remain buildable without FSR dependencies. Add sources to the actual CMake source lists.

Preserve Standard/Universal and NR-on/off build behavior. Build both relevant editions and exercise enabled/disabled FSR configurations. Use the documented preset and actual configured paths, not invented workstation locations.

With compatibility tests enabled, build all test targets before running:

    cmake --build <configured-build-directory> --config Release --parallel 2
    ctest --test-dir <configured-build-directory> -C Release --output-on-failure

Inspect available presets/configuration first; supply concrete executed commands in the final report. Do not report configuration as a successful build.

Stage packages separately from the live game. Include required shaders/configuration and permitted runtime files, or precise installation instructions where redistribution is not appropriate. Record SDK/runtime versions, checksums, licenses, filenames, and architecture. Do not bundle NVIDIA binaries as a hidden FSR-only requirement.

## 14. Runtime acceptance and performance evidence

Prepare a reproducible test matrix, marking each case run, failed, or not run:

- RTX 4080 Super: unchanged DLSS/DLAA baseline; FSR SR with FG off; NVIDIA runtime files absent in an isolated test install; FSR SR + FSR FG; DLSS + FSR FG when implemented.
- Supported AMD and Intel GPUs: independently validate SR and FG capabilities. Do not imply identical support requirements.
- Vanilla/non-ENB and ENB; ReShade before/after; CS external-upscaler ownership.
- Native-AA, reduced quality modes, ultrawide output, alt-tab/minimize, supported resize, save/load, fast travel, interior/exterior, camera changes, menus and previews.
- Moving characters/weapons, grass, fences, particles, fog, fire, water, transparencies, and HUD/world-space widgets.

Capture render/output dimensions, source-frame timing, provider identity, image artifacts, and resource-lifetime evidence. Prove reduced rendering from actual scene render targets and viewports before the upscale stage, not just dispatch parameters or an FSR-enabled log line. Measure GPU timings for bridge copies, conversions, FSR, and presentation separately. Compare matched scenes/settings, with FG off for SR performance comparisons.

Do not promise improved real FPS in a CPU/draw-call-bound scene. Track CPU frame time, GPU time, added VRAM, and synchronization stalls rather than only generated FPS. No per-frame resource allocation, disk logging, provider discovery, or busy-waiting in the release path.

Use captures/profilers only when available and authorized. Source inspection, unit tests, a GPU fixture, and Skyrim gameplay establish different levels of evidence; keep them separate.

## 15. Execution checkpoints and final deliverables

Work in reviewable increments:

A. Ownership/configuration separation with unchanged NVIDIA regression tests.
B. Optional AMD loader and tested parameter/provider handling.
C. Real FSR SR plus NVIDIA-independent no-FG presentation.
D. UI/effects/lifecycle validation, error recovery, packaging, and diagnostics.
E. AMD FG adapter and its tests, experimental until runtime acceptance.

For each increment, write the relevant failing contract tests, implement, run targeted checks, then run broader regressions. Do not weaken tests simply to permit a regression. Continue repairing failed prerequisites rather than layering FG on an unstable SR path.

Deliver source changes, tests, build/package updates, and `docs/FSR_IMPLEMENTATION.md` plus a runnable test checklist. Include:

- Starting/final revisions and changed components.
- Exact SDK/runtime pin and actual provider selection.
- Concrete build/test commands and outcomes, including skips.
- Whether the NVIDIA-free startup case was really exercised.
- Which hardware, rendering combinations, and game versions were actually tested.
- Remaining bugs, evidence-backed blockers, and experimental features.

Binary analysis and targeted engine hooks are allowed when a missing contract requires them. Prefer existing hooks and source evidence. For new binary-specific work, record executable/DLL hashes, runtime version, relocation/RVA, expected instructions, validation, and rollback. Do not invent universal addresses or transplant an offset across Skyrim versions.

Do not stop at a design document, checkbox, stub backend, or successful DLL load. Implement real dispatch and output consumption. If the environment lacks Windows, the game, a runtime, or another GPU, complete all feasible code and tests, clearly identify unverified acceptance gates, and never fabricate build or gameplay success.
