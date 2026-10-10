# XeSS Frame Generation and XeLL design

Status: advanced to implementation planning by the user's next-step request on 2026-10-10. The written plan awaits review; product implementation has not started.

## Goal and existing agreement

The user selected XeSS upscaling first, then Intel XeSS FG, and now requested the FG implementation. Add a selectable Intel FG backend while retaining the working DLSS/FSR/XeSS SR and NVIDIA/FSR FG paths. Preserve scene -> selected SR -> NR After -> selected FG -> UI -> output, live FG off/on, borderless Skyrim and the standard End menu.

The intended combinations are DLSS/DLAA, FSR 3.1.5/4 and XeSS SR with Intel FG. NR keeps its existing GPU/runtime eligibility and maximum three passes. Existing NR Before remains selectable. First qualification uses the local RTX 4080 SUPER, SDR, Native render scale and fixed display dimensions. The first Skyrim trial should use the user's preferred FSR Native -> NR After -> XeSS FG path after standalone validation.

The previous XeSS SR spec explicitly deferred Intel FG/XeLL to its own presentation design. This document covers that new owner, latency integration and qualification. It does not retroactively mark the remaining SR milestone 7/8 checks complete.

## References and evidence

- Official SDK 3.0.2, commit `8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0`: [release](https://github.com/intel/xess/releases/tag/v3.0.2), [FG guide](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/doc/xess_fg_developer_guide_english.md), [XeLL guide](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/doc/xell_developer_guide_english.md).
- Public headers `inc/xess_fg/xefg_swapchain.h`, `xefg_swapchain_d3d12.h`, `inc/xell/xell.h` and `xell_d3d12.h` were read at that revision.
- AIO19 ships byte-identical official FG/XeLL runtimes. Existing streamed archive witnesses record their identities in `research/xess/aio19-sdk-witnesses-2026-10-09.json`.
- New read-only Capstone inspection of the hash-pinned `PDPerfPlugin.dll` found twelve FG/XeLL API names and a bounded `GetProcAddress` witness for `xefgSwapChainD3D12CreateContext` at RVA `0xb5190`, resolver call `0xb519b`, unwind fragment `0xb5050..0xb5464`. This is static loader evidence, not proof of executed FG, resource states or AIO's UI composition policy.
- Follow-up [AIO19 call-site inspection](../../../research/xess-fg/AIO19_XESS_FG_IMPLEMENTATION_2026-10-09.md) verifies 33 public FG/XeLL delay-import calls, matching tag/Present IDs, next-frame post-Present sleep, three retained-resource tags, actual last-Present statistics and FG-before-XeLL destruction. The byte witnesses are reproducible with `tools/xess/Inspect-Aio19Generation.py`. Its main wrapper tags UI/motion/depth with `RV_UNTIL_NEXT_PRESENT` and initializes UI mode AUTO; explicit composition enable, complete GPU retirement and pre-input timing are not proven. The earlier GetProcAddress fragment is provider-specific helper evidence, not the complete normal loader path.
- The current repository has source-frame, dedicated HUD, D3D11/D3D12 interop, native ReShade ownership and ordinary/NVIDIA/AMD presentation boundaries. `FsrPresentationTransport` contains AMD-specific reader retirement; its acknowledgement protocol cannot be copied into Intel ownership unchanged.

## Approach

Use an optional Intel SDK owner alongside the existing providers, extending only the presentation selection and shared source handoff needed by that owner. Reuse validated colour/guide conversions, frame identity, HUD capture, native-device acquisition and generation admission rules.

A full presenter abstraction rewrite could unify all providers before Intel integration, but would enlarge the change and require requalifying every existing route. The selected approach keeps provider-specific contexts and retirement rules explicit and extracts a shared helper only when both paths use the same proven contract.

The SDK proxy becomes the only presentation owner when Intel FG is selected. The AMD FG swap chain and NVIDIA DLSS-G proxy must not be initialized underneath it. A Streamline SR runtime may still serve DLSS; Intel mode must explicitly disable the mod's Reflex latency policy and avoid loading/enabling DLSS-G. ReShade native-device capture remains strict for Intel presentation.

## User settings and compatibility

- Add canonical `[FrameGeneration] Backend = XeSS` and the visible menu option `XeSS FG (2x)`. Preserve existing enum values; append the new internal value.
- Backend changes require Save and restart because they replace the presentation owner. `Enabled` remains a live control and keeps the selected owner initialized while interpolation is off.
- First implementation generates at most one extra frame on every vendor. Intel 3x/4x is a later qualified extension, so no misleading multiplier control is added now.
- Auto retains the existing tested GPU policy; installing Intel DLLs alone must not change the default owner. Ordinary backend 0 stays a debug option.
- SDK capability comes from the actual rendering device/LUID, OS requirements, Shader Model support, context creation and SDK properties, rather than a fixed list of marketing names. A simulated card description is routing-test evidence only.
- Missing optional runtimes do not affect other backends. If Intel FG is explicitly requested and cannot initialize, emit a diagnostic identifying the SDK stage and suggested existing provider. Any startup fallback must happen before game targets/jitter are committed, report the effective provider and retain the user's request. No partially initialized Intel owner is relabelled as another backend.
- SDR is the first qualified output. An incompatible HDR request gets a clear preflight message; future HDR10 support is a separate test stage. No silent HDR-to-SDR substitution.
- Existing Smooth Motion/conflicting external FG warnings stay active. Selecting Intel mode disables the mod's competing Reflex latency path, without writing NVIDIA global/profile settings.
- No new minimum-FPS gate is introduced. SDK readiness, fresh temporal inputs, camera transitions and HUD completeness determine eligibility.

## Components and ownership

1. `XessGenerationRuntime`: load pinned `RaZkolbaS/XeSS/libxess_fg.dll` and `libxell.dll` with local paths, typed public exports and retained module leases. Extend SDK acquisition/packaging without duplicating the existing SR payload or modifying process-wide DLL search paths.
2. `XellSession`: own the same-adapter native D3D12 latency context, marker sequence, frame IDs, sleep and low-latency mode. Context survives through Intel FG destruction.
3. `XessGenerationFrameAdapter`: validate scene/HUD/guide extents, encoding, motion/depth convention, camera matrices, jitter, reset and finite frame-render milliseconds. Produce Intel public tag/constant descriptions independently of GPU submission.
4. `XessGenerationPresentation`: own the direct queue, SDK context and DXGI proxy; query properties, initialize once, explicitly enable UI composition, present and report actual SDK status. Guard internal factory creation against recursive game hooks.
5. `XessGenerationTransport`: own D3D11 producer copies, D3D12 upload/tag resources and fences. It publishes the accepted post-SR/post-NR/sharpened image and separately captured HUD/overlay. A slot is reusable only after its producing/tagging work is retired.
6. Existing host/policy/UI boundaries choose Intel creation, source submission, present, readiness and retirement. Provider labels and output statistics always describe the actual owner.

The game still receives its stable D3D11-compatible outer swap chain. Its device/resource identity remains the game's original D3D11 device; the Intel DXGI proxy is an inner D3D12 presentation object. `GetBuffer`, `GetDevice`, resize and window operations must retain that separation.

## Frame and UI contract

Allocate a checked 32-bit SDK frame ID at the verified engine pre-input boundary, with the engine source ID and epoch recorded alongside it for source freshness. XeLL marker IDs, tagged resources/constants and the corresponding Intel Present ID must agree. Drain/reset before SDK-ID wrap; never submit mismatched IDs or manufacture a new simulation ID inside Present.

Depth and motion are the original producer guides, with declared low/high-resolution semantics and appropriate jitter/depth flags. Matrices are finite, unjittered and converted explicitly to Intel's row-major convention. Motion scales come from actual guide dimensions/units, not an unconditional display-size multiplier. NR settings/styles and source/provider changes trigger a history reset through the existing epoch policy.

Default SDK UI interpolation is unsuitable for the intended pipeline. Enable UI composition explicitly and select `HUDLESS_UITEXTURE` only after verifying the dedicated premultiplied HUD/overlay obeys the SDK's blending contract. If the existing texture does not satisfy it, use a validated conversion or qualify `BACKBUFFER_HUDLESS_UITEXTURE` with pixel tests; do not assume alpha conventions match. Missing/incomplete HUD input suspends generation for that frame and presents the real scene safely.

Use `RV_ONLY_NOW` tagging on a recorded command list initially so Intel captures the scene, motion, depth and UI instead of asynchronously retaining mutable game resources. Queue ordering and a signalled tagging fence protect reuse; `Present` return alone is not an all-GPU retirement proof. Track incoming/outgoing resource states, resource bases and valid extents explicitly. A later zero-copy optimization requires separate reader-retirement evidence.

This deliberately differs from AIO19's observed `RV_UNTIL_NEXT_PRESENT` tags. Intel's public copy mode supplies a clearer lifetime boundary for our existing D3D11 producer and mutable guide resources. AIO's UI tag and AUTO initialization do not establish dedicated HUD composition; our explicit HUD qualification remains required.

Duplicate source, spatial recovery, menu, loading, camera cut and missing-guide cases must not generate from an unaccepted source. Retained completed SR output can still be presented as a real frame; generation readiness resets/re-enters according to the source-epoch policy. Extra loading/duplicate Presents do not invent sleep/simulation marker cycles. Qualify their interpolation-disabled proxy Present behavior in the real SDK harness before game integration, including last-Present status and resumption on the next complete engine frame.

## XeLL integration

Use actual engine boundaries for sleep before input sampling, simulation start/end, render-submit start/end and Present start/end. The current `BeginSourceFrame` jitter/render hook is not evidence of a pre-input simulation boundary. Locate and verify an appropriate engine hook for each supported Skyrim runtime before qualification; a cluster of markers inserted at Present cannot be called a complete XeLL integration.

AIO19 normally increments the next frame ID and sleeps immediately after the preceding Present, then emits SimulationStart. Investigate this existing owner boundary as a lower-impact candidate before adding an engine hook. It is acceptable only if call-flow and runtime traces prove it precedes the next input sample, pairs correctly with real simulation/render boundaries and cannot repeat during extra/loading Presents. AIO's guarded fallback markers are not evidence that these conditions hold in our pipeline.

Only one sleep call occurs per new SDK frame ID and returns before that frame's first marker. State-machine tests enforce all marker pairs and allowed phase overlap. Initial frame limiting remains off (`minimumIntervalUs=0`); retain borderless tearing/VSync semantics required by the SDK. Changing sleep mode follows GPU quiescence and executes on the owner boundary, including live FG off/on. Non-Intel latency reduction is enabled only with supported Intel interpolation; no standalone XeLL qualification is claimed there.

## Lifecycle and failure behavior

All lifecycle operations stop admissions before releasing or replacing resources. Live disable/menu entry stops interpolation and drains any work required for a XeLL mode change, while preserving SR and the selected presentation owner. Resume supplies fresh matching tags and resets history as needed.

Minimize suppresses generation. Same-descriptor restore avoids rebuilding owners. Resize to a different display extent must first prove producer/tag queue completion and SDK/proxy quiescence, then rebuild/tag valid resources. Until that path is qualified, reject changed display dimensions before retiring the running owner with a precise restart message.

Shutdown stops new calls, disables generation, drains owned GPU work, releases all references to the proxy, destroys Intel FG, then destroys XeLL, then releases queue/device/runtime owners. Treat SDK destroy failure or a fence timeout as ownership still pending; keep the owner alive and report the failure. Do not manufacture a retirement acknowledgement from a timeout or unrelated FSR fence.

Device loss remains fatal. Recoverable missing input/history warnings suspend interpolation and keep delivering real frames. Failed tags or SDK errors are not counted as accepted generated frames. Log requested/effective backend, actual device/LUID, runtime/algorithm versions, UI mode, resource conventions, latency state, reset reason and last-Present status. Record output FPS from actual SDK presented-frame counts or a qualified DXGI counter, never nominal 2x multiplication.

## Qualification and milestones

1. Evidence, approved written design and implementation plan.
2. Optional runtime loader, real 4080 SUPER FG/XeLL capability and context probe, safe partial-init unwind.
3. Frame/tag contract, frame-ID and XeLL marker policy, with verified engine timing hook locations.
4. Standalone D3D12 Intel proxy: actual generated presentation status, moving scene, scene/HUD pixel checks, live off/on, focus/minimize/restore and clean shutdown.
5. Skyrim routing/menu/INI, first FSR Native SDR trial with NR off, stable game-facing device/buffers and real XeLL marker sequence.
6. NR After and DLSS/DLAA/FSR/XeSS combinations, Native/scaled guides, repeat/recovery and accepted source ownership.
7. Gameplay lifecycle: menu/HUD/inventory/map/dialogue, live FG/NR/style toggles, save/reload, fast travel, alt-tab/minimize, same-size restore and teardown.
8. Clean Release build, relevant regressions, immutable MO2 archive and qualification report distinguishing local hardware from untested vendors.

CPU fakes prove call/ownership policy only. Standalone real SDK execution and readback/presentation evidence precede any Skyrim installation. Physical visual feedback is required for moving image and HUD quality. AMD/Intel/other NVIDIA hardware remains unqualified until real tester evidence exists. New work uses the current worktree/build directory and preserves installed DLL/INI and MO2 launch/profile settings until a validated trial is ready.

## Explicit limits

The first trial targets one generated frame, SDR, borderless windows and fixed output dimensions. HDR10, Intel MFG, external renderer ownership, every injector version and physical cross-vendor validation remain follow-up qualification. Existing SR and FG functionality must continue to build and run when the Intel FG feature or runtimes are absent.
