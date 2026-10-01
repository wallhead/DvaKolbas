# FSR implementation record

Starting revision: `246d152c42a83308beb7a6f4930e7c472ef0d4ce` (TRP 0.3.5).
Branch: `codex/fsr-sr`.
Status: approved SR design executing inline. Backend/frame contracts and optional runtime/provider boundary implemented; startup still rejects FSR until the ordinary host and GPU integration are ready. FSR is not installed in Skyrim.

The user approved shared buffers/native UI, NVIDIA-independent FSR SR first, then FSR FG, and the [written spec](superpowers/specs/2026-10-01-fsr-sr-design.md). The [implementation plan](superpowers/plans/2026-10-01-fsr-sr.md) defines eight tested increments. The user’s “next step” continued implementation inline in the existing FSR worktree. Pushes to `https://github.com/wallhead/DvaKolbas` are authorized. This record will gain executed commands, artifacts, hashes, and acceptance outcomes as implementation proceeds.

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

Evidence v3 is now indexed in the [plain RE database](RE_DATABASE.md): 325 cumulative instruction records, 49 new spans/6,107 excerpt boundaries, 11 constants, 34 PD vtable entries, and 16 pure-Python reference-model tests passed locally. Its corrected supplied SR map, UI reference imbalance, jitter/depth producers, and reset acknowledgment findings remain static evidence; no FSR implementation or game deployment has occurred.

Evidence v4 adds eight locally checked host excerpts (720 instruction comparisons) and 16 routing landmarks. ReShade save/clear/restore brackets, target-binding-before-UI-phase ordering, and StatsMenu’s embedded scene support scoped routing and nested scene/UI validation. Scissor coverage and ENB render chronology remain live evidence gaps. See the [RE findings](FSR_AIO_RE_NOTES.md) for exact scope; this is research only.

Evidence v5 connects CS capture and generic acquisition to prepared guides shared by SR/FG, and narrows mask/exposure policy. Its 38 new landmarks and 11 string offsets passed local checks. The supplied transform-helper excerpt is misnamed and some excerpt edges are misaligned; the actual local callee decode supports a draw but leaves caller-wide state restoration unresolved. See the [qualified V5 findings](FSR_AIO_RE_NOTES.md#revision-5-temporal-input-provenance-and-evidence-qualifications). These findings inform validation while preserving the current external-renderer ownership policy.

Introduce a neutral upscaler input/result contract, optional typed AMD runtime loader, FSR context/dispatch adapter, ordinary presentation policy, and narrowly shared interop services. Keep the existing NVIDIA feature/presenter implementation and companion ABI intact. FSR-only startup must neither require nor attempt unused NVIDIA services.

The producer signal/submission -> D3D12 wait/dispatch/signal -> D3D11 output wait sequence and recorded slot retirement are mandatory. The later FG stage adds completed UI and asynchronous presentation readers to retirement rather than replacing those obligations.

## Evidence gaps and acceptance state

* FSR runtime archive/per-DLL hashes, signing, exact package layout, and licenses: verified in increment 2 below.
* FSR mode numeric audit: reachable baseline `UpscaleType.h` history preserves DLSS 0/DLAA 3 from publication `8164c1a`; a reachable source/package history search found no FSR/upscaler selector assigned 4. The spec selects FSR 4 and leaves legacy 1/2 rejected. Unpublished/disconnected history is outside this evidence.
* Skyrim motion/jitter and ENB/non-ENB encoding at the FSR handoff: require controlled captures/measurements; NVIDIA adapter comments are not proof.
* FSR dispatch, native output consumption, NVIDIA-free clean-process startup, GPU fixture, packaging, and gameplay: not implemented/tested.
* Available RTX 4080 Super: existing DLSS initial gameplay passed. AMD/Intel: not tested.
* Graphics Tools-dependent validation: unavailable after installation error 5; do not report those checks passed.
* FSR FG: subsequent implementation stage, not active or tested.

The live V5.4 NO-LORE installation remains the existing tested DLSS baseline. No FSR deployment or MO2 modification has been performed.

## Executed increment 1: backend and frame contracts

Added independent renderer/generation decisions and the backend-neutral source-frame evaluator. Kept DLSS=0/DLAA=3, added FSR=4, retained legacy rejection, and kept unfinished FSR unavailable at plugin startup. The NVIDIA adapter preserves its existing reconstruction, ReShade, and camera/NR preparation behavior. Spatial recovery cannot acknowledge temporal history or arm generation.

Both FSR-disabled Standard and Universal Release plugins built. Verification: `C:/Python314/python.exe out/research/run_task1_tests.py`, running the complete CTest suite in each build except the three known Graphics Tools-dependent checks. Standard: 103/103 passed; Universal: 103/103 passed. `NativeUIComposition`, `NativeUIBlendState`, and `NeuralPeripheralPixels` remain SKIPPED in each edition. This is contract/regression evidence, not FSR dispatch or gameplay evidence.

The first Standard attempt overlapped Universal’s vcpkg regeneration in a shared dependency directory and encountered temporarily missing dependency headers. Retrying after regeneration completed built successfully; future configurations sharing that installation run sequentially.

SDK research downloaded the pinned official v2.3.0 archive under ignored `out/research`, with its expected SHA-256 verified. The two SR DLLs are x64 and have valid Advanced Micro Devices signatures. No runtime DLL was loaded or installed into the game during this increment. Acquisition, loader/provider tests, and dispatch remain later increments.

## Executed increment 2: optional FidelityFX runtime boundary

Added `TRP_ENABLE_FSR` (OFF by default), pinned-header SHA checks, dynamic typed C exports, safe absolute plugin-relative `FSR/` DLL loading, foreign-module collision rejection, bounded provider enumeration with copied names, provider sizing, and actual-context provider verification. No FidelityFX import library is linked. `tools/fsr/Acquire-Runtime.ps1` verifies the release archive, headers/license, runtime sizes/hashes, x64 machine type, file versions, and the pinned valid AMD signer. Metadata is in `tools/fsr/runtime-pin.json`; runtime DLLs remain ignored local artifacts.

Official release: SDK v2.3.0 at `60f4ea81909200d8542eca14dccb2628b763a9a3`. Archive SHA-256: `f90890b9323bb2f4f2404ac4cdc9395e8495ecdac6f7aa0bcdf1ad1848422273`. Loader file version: 2.3.0.2740; upscaler file version/API header: 4.1.1.2740 / 4.1.1. The SDK binary license is retained under `Kits/FidelityFX/docs/license.md`; the C API headers carry separate MIT notices.

Standalone official-runtime probe used the actual D3D11 adapter to create the matching D3D12 device (LUID `00000000:00010541`). It discovered SR providers `3.1.5` and `2.3.4`; analytical selection chose discovered ID `17700776140660019205`. Performance sizing returned 960x540 for 1921x1081 output. Official context creation, queried actual provider identity, and destruction succeeded. The ID is recorded evidence, never a constant in production selection. The probe recorded/dispatched no GPU commands, so it provides no FSR image-output or product-startup acceptance.

Loader fixtures exercise missing runtime, relative paths, wrong architecture, each missing export, partial module cleanup, foreign basename collisions, unsupported ABI, empty/unstable/oversized provider lists, copied name lifetime, null names/device, quality/dimension rejection, sizing/create override agreement, and actual provider mismatch. Acquisition checks exercise altered archive/header rejection and verified reuse. Header-pin rejection was observed failing before the hash check and passing after it.

Verification command: `C:/Python314/python.exe out/research/run_task2_tests.py`. FSR-enabled Standard Release: 104/104 runnable CTest cases passed. FSR-disabled Standard and Universal Release: 103/103 each passed, configured with deliberately nonexistent `C:/deliberately-missing-fidelityfx-sdk`. All three builds completed; three Graphics Tools-dependent checks remain SKIPPED in each. Acquisition integrity and the official provider/context probe also passed. Import inspection found no static FidelityFX imports in the Standard DLL or standalone probe. This does not satisfy the later clean-process FSR product startup gate.

Reproducible provider-only probe: configure `cmake -S tools/fsr -B out/build/fsr-provider-probe -A x64 -DTRP_FSR_SDK_DIR=<verified SDK root>`, build Release, then run `TRPFsrProviderProbe.exe <absolute plugin root>` where `<plugin root>/FSR/` contains the two verified SR DLLs. The supplied tool uses the actual D3D11 adapter for D3D12 and checks the matching LUID. For acquisition/check scripts, the tested shell is PowerShell 7 (`pwsh`). SDK binaries and generated receipts are not checked into Git.

## Executed increment 3: shared GPU transfer and retirement

Extracted the existing same-adapter texture/fence/three-slot mechanics into `Graphics::D3D11D3D12Interop`. The NVIDIA wrapper keeps its completion-fence bridge. Ordinary SR now requires a submitted D3D11 producer before dispatch and explicitly retires final D3D11 output readers before destruction. Shared texture owners must retain their own allocations after a failed retirement; the common core retains its potentially live command objects.

Hardware tests first reproduced dispatch without a producer and premature Drain during a gated final reader. Both now pass. Additional real GPU checks cover exact changing pixel copies, foreign adapters/devices, illegal descriptors, three outstanding submissions, blocked fourth-slot reuse, progress after delay, and delayed output readers preserving earlier pixels. Controlled fence/allocator fixtures verify the device-removal sentinel and safe abandonment without fake completion. They do not physically remove a GPU. Existing `SourceDLSSGInteropFence` remains passing.

All three Release builds completed. Runnable CTest results: FSR-enabled Standard 106/106; FSR-disabled Standard and Universal 105/105 each. The three previously excluded Graphics Tools checks remain SKIPPED. The new interop probes independently reported both D3D11 and D3D12 debug layers unavailable. No FSR dispatch, product startup, gameplay, or cross-vendor acceptance is claimed by these transfer checks.
