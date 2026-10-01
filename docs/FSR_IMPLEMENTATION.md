# FSR implementation record

Starting revision: `246d152c42a83308beb7a6f4930e7c472ef0d4ce` (TRP 0.3.5).
Branch: `codex/fsr-sr`.
Status: written spec approved; implementation plan review pending. No FSR product changes or runtime installation yet.

The user approved shared buffers/native UI, NVIDIA-independent FSR SR first, then FSR FG, and the [written spec](superpowers/specs/2026-10-01-fsr-sr-design.md). The [implementation plan](superpowers/plans/2026-10-01-fsr-sr.md) defines eight tested increments and awaits review/execution-method selection. Pushes to `https://github.com/wallhead/DvaKolbas` are authorized. This record will gain executed commands, artifacts, hashes, and acceptance outcomes as implementation proceeds.

## Verified current ownership map

References below were inspected at the starting revision. Line numbers describe that revision and will change with edits.

| Responsibility | Current implementation | FSR impact |
| --- | --- | --- |
| Startup policy | `src/NvidiaBaselinePolicy.h:18`, `ValidateNvidiaBaseline`; called by `src/XSEPlugin.cpp` | Unconditionally requires DLSS/DLAA and NVIDIA ownership; needs backend-aware validation while retaining obsolete-setting rejection. |
| Early stable buffer publication | `src/UpscalerDeviceHooks.cpp:64`, factory hook; `src/FrameGen/NvidiaHostStartup.cpp:13`, `CreateSwapChain` | The outer wrapper and game-facing buffer exist before original device creation returns. Preserve this timing with an ordinary D3D11 inner presenter. |
| Deferred feature initialization | `src/UpscalerDeviceHooks.cpp:170`, original device creation; `src/FrameGen/NvidiaHostStartup.cpp:209`, `CompleteStartupAfterDeviceCreation` | Full FSR context initialization remains after original creation/startup Present; pre-query device/capabilities may precede buffer sizing. |
| Allocation and size query | `src/FrameGen/NvidiaHostStartup.cpp:119`, `CreateGameFacingResources`; `:257`, `InitializeSourceUpscaler` | Currently uses NVIDIA sizing/feature creation. FSR must query its own provider before stable allocations. |
| Device and jitter | `src/RenderPipeline.cpp:179`, `SetupSwapChain`; `:192`, `GetJitters`; `:247`, `InitUpscaler` | Setup/jitter currently access DLSS directly; TAA is disabled in initialization. Route through active backend and audit projection jitter producer. |
| Source frame evaluation | `src/FrameGen/NvidiaHost.cpp:75`; `src/FrameGen/SourceNvidiaEvaluation.cpp:119`, `EvaluateSourceNvidiaFrame`; `:54`, `EvaluateDLSS`; `SourceNvidiaFrameEvaluator.h` | Existing copy, NR, ReShade, temporal/spatial, and prepare ordering must remain intentional; select one temporal backend. |
| Generation preparation | `src/FrameGen/SourceNvidiaFramePreparation.h` | Preparation also serves current NVIDIA NR behavior even when FG is off. Ordinary FSR must distinguish not requested from failed without weakening NVIDIA behavior. |
| Presentation readiness | `src/FrameGen/NvidiaHost.cpp:259`, `PresentationBackendReadyForEvaluation` | Unconditionally checks SourceDLSSG readiness; ordinary FSR needs independent readiness. |
| Native UI completion | `src/FrameGen/NvidiaHost.cpp:211`, `FinishNativeUIPassForPresent`; `src/FrameGen/SourceNativeUI.cpp`; `NativeUIComposition.*` | Early texture identity is not completed content. Preserve completion boundary and compose native UI once after native SR output. |
| Camera conversion | `src/FrameGen/SourceDLSSGCameraMath.cpp`, camera history/conversion | Current Streamline conversion describes normalized motion. Share underlying camera measurements; prove FSR conventions with captures. |
| Shared GPU work | `src/FrameGen/SourceDLSSGInterop.cpp:160`, `SignalD3D11`; `:173`, `WaitD3D12`; `:180`, `WaitD3D11`; `:293`, `Begin`; `:310`, `Submit`; `:326`, `Drain` | Same-adapter shared textures/fences and submission slots are reusable. NVIDIA reader-completion/queue-mode assumptions are not common SR contracts. |
| Resize/shutdown | `src/FrameGen/NvidiaHostLifecycle.cpp:68`, `BeforeResizeBuffers`; `:79`, `AfterResizeBuffers`; `:121`, `ReleaseSourceUpscaler`; `SourceHostLifecycle.h` | Current retirement depends on SourceDLSSG quiescence. Select active backend retirement before freeing allocations/features. |
| Outer swapchain | `src/FrameGen/GameSwapChain.cpp`, Present and ResizeBuffers variants | Preserve D3D11-facing stable wrapper and host lifecycle; do not substitute a late wrapper or cast it to AMD's D3D12 FG proxy. |
| Build/source list | `CMakeLists.txt`, `cmake/BaselineSources.cmake` | Add optional FSR sources and SDK path without changing FSR-off dependency requirements or Standard/Universal/NR variants. |

## Proposed changes, not implemented

The user's subsequent AIO archive was inspected offline with Ghidra and Capstone. [The RE evidence record](FSR_AIO_RE_NOTES.md) identifies separate SR/FG classes, a D3D11-to-D3D12 bridge/presenter, PrepareV2/matching source IDs, millisecond timing, and UI buffering. Those findings support the plan but are not FSR implementation or gameplay acceptance. The archive's binaries were not executed or deployed.

The supplied evidence v2 was compared against the same local binaries. Hashes, 142 instruction records, 40 new code spans, 22 AMD vtable slots, and PDB layouts passed the documented static checks. Its additional host/UI/retirement mapping and camera/order concerns are incorporated into the evidence record and plan; no runtime acceptance or product implementation is implied.

Introduce a neutral upscaler input/result contract, optional typed AMD runtime loader, FSR context/dispatch adapter, ordinary presentation policy, and narrowly shared interop services. Keep the existing NVIDIA feature/presenter implementation and companion ABI intact. FSR-only startup must neither require nor attempt unused NVIDIA services.

The producer signal/submission -> D3D12 wait/dispatch/signal -> D3D11 output wait sequence and recorded slot retirement are mandatory. The later FG stage adds completed UI and asynchronous presentation readers to retirement rather than replacing those obligations.

## Evidence gaps and acceptance state

* FSR runtime archive/per-DLL hashes, signing, exact package layout, and licenses: not verified locally yet.
* FSR mode numeric audit: reachable baseline `UpscaleType.h` history preserves DLSS 0/DLAA 3 from publication `8164c1a`; a reachable source/package history search found no FSR/upscaler selector assigned 4. The spec selects FSR 4 and leaves legacy 1/2 rejected. Unpublished/disconnected history is outside this evidence.
* Skyrim motion/jitter and ENB/non-ENB encoding at the FSR handoff: require controlled captures/measurements; NVIDIA adapter comments are not proof.
* FSR dispatch, native output consumption, NVIDIA-free clean-process startup, GPU fixture, packaging, and gameplay: not implemented/tested.
* Available RTX 4080 Super: existing DLSS initial gameplay passed. AMD/Intel: not tested.
* Graphics Tools-dependent validation: unavailable after installation error 5; do not report those checks passed.
* FSR FG: subsequent implementation stage, not active or tested.

The live V5.4 NO-LORE installation remains the existing tested DLSS baseline. No FSR deployment or MO2 modification has been performed.
