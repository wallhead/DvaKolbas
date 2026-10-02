# FSR implementation record

Starting revision: `246d152c42a83308beb7a6f4930e7c472ef0d4ce` (TRP 0.3.5).
Branch: `codex/fsr-sr`.
Status: the local FSR SR milestone is complete. Six-build/GPU/package checks and the final review fix pass passed. Enabled builds accept coherent FSR settings, while FSR-off builds reject FSR. Built code revision: `49cb69f8ccb1532d42ed2b0a166010f870d4a121`. FSR is not installed in Skyrim; gameplay acceptance and FSR frame generation remain open. See the [review and preserved decisions](FSR_REVIEW.md), [validation evidence](FSR_VALIDATION.json) and [final package hashes](FSR_PACKAGES.json).

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

## Planning evidence before implementation

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
* FSR dispatch/native output consumption, source-frame integration, production startup helpers and packaging: tested in the isolated fixtures through increment 8. Actual Skyrim/SKSE startup and gameplay remain pending.
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

## Executed increment 4: real temporal FSR dispatch

Added typed descriptor mapping, official context creation/actual-provider verification, provider quality/jitter queries, strict source identity and parameter validation, and explicit COMMON/read/UAV state transitions. The context retains its runtime, retirement bridge, and fixed shared resources. Failed GPU retirement or context destruction retains that entire state until process exit. Recorded but unsubmitted commands also prevent destruction. Shared camera measurements preserve existing forward/reversed LH/RH NVIDIA conversion results.

The GPU fixture uses the production loader, parameter adapter, context, and interop with controlled jittered textured planes, independent camera/object translation, matching depth, and unjittered screen-pixel motion. It consumes the native output through a D3D11 reader. It exercises all four qualities, odd dimensions, forward/inverted depth, and three actual outstanding submissions per context. CPU readback occurs only in this fixture.

Executed `TRPFsrGpuSmoke --frames 1000 --recreate 25 --debug auto --runtime out/research/sdk-v2.3.0/runtime --output out/validation/fsr-gpu.json`: 1,000 dispatches, 25 successful context lifetimes, 75 finite/nonempty output readbacks, 74 changing samples, and 25 verified groups of three outstanding submissions. Actual analytical provider: 3.1.5, ID `17700776140660019205`, on adapter LUID `00000000:00010318`. The ID remains discovered evidence only. SDK allocation callbacks observed 650 committed resources, a peak of 26 per context, and zero outstanding allocations after each retirement/destruction. This provider made zero explicit heap allocations. Total local GPU memory peaked at 56,336,384 bytes; retired usage was stable at 22,781,952 bytes. Both debug layers were unavailable and remain unvalidated.

The first fixture allocation identified a real sharing requirement: a UAV-only FP16 texture created/shared on D3D12 but could not open on D3D11; adding RT capability made all three boundaries succeed. The bridge intermediates therefore use validated RT+UAV descriptors. The pinned backend source confirms that dispatch unregisters external resources back to their declared entry states; the adapter then explicitly restores COMMON.

Verification: `C:/Python314/python.exe out/research/run_task4_tests.py`. All three Release builds completed; FSR-enabled Standard passed 108/108 runnable cases, and FSR-disabled Standard/Universal passed 105/105 each. The official GPU fixture is part of the enabled suite. Three baseline Graphics Tools checks remain excluded/SKIPPED per edition. This proves isolated GPU dispatch/output/lifetime on the RTX 4080 Super. It does not prove Skyrim guide conventions/color quality, NVIDIA-free product startup, gameplay, AMD/Intel operation, or FSR FG. Plugin admission still rejects FSR until the ordinary host and frame integration are complete. No game or MO2 settings changed.

## Executed increment 5: ordinary startup and retirement

Added ordinary D3D11 FLIP_DISCARD/two-buffer presentation through the original factory function, preserving the stable outer game-facing swapchain. FSR uses the actual D3D11 adapter for pre-query sizing, probes shared format/fence capabilities before publishing reduced targets, and defers context creation until the original device-creation call returns. Startup, readiness, generation-disable, resize and shutdown route around NVIDIA services on the FSR branch. Context, module and texture owners remain retained after a real retirement stall; readiness stays false even if the independently gated work later completes.

The startup fixture presents before feature creation, publishes provider-sized 960x540 targets for odd 1921x1081 output, then creates the context. Its creation/readiness/retirement policy spies observe zero NVIDIA operations. The lifecycle fixture delays real D3D11 readers and proves failed retirement retains the FSR resources and context; it separately proves ordinary retirement refuses resize and cannot clear its terminal fault. These helpers and policy tests are not Skyrim SKSE startup proof. Plugin admission remains disabled until source-frame integration and authoritative FSR settings are complete.

Verification command: `C:/Python314/python.exe out/research/run_task5_tests.py`. Standard and Universal Release both built successfully and passed 110/110 runnable CTest cases each, including NVIDIA regressions and the official 1,000-frame/25-context GPU fixture. Three Graphics Tools checks are SKIPPED in each edition. VS2022 dumpbin DEPENDENTS and IMPORTS inspection of both editions found no mandatory normal/delayed NVIDIA or AMD runtime imports; the runtimes remain optional. Clean-process loading and real game startup remain separate gates in increment 8. No MO2 or game settings changed.

## Executed increment 6: source frames, image conversion and recovery

Connected FSR to the shared source-frame evaluator and existing native UI/presentation coordinator. The adapter consumes the fixed prepared textures, copies actual signed RG16_FLOAT motion, converts depth through the appropriate SRV into R32_FLOAT, decodes finished SDR into FP16 linear color, and returns FSR output to the native handoff transfer before the ordinary presenter reads it. Separate reusable converters preserve the producer's full D3D11 state; only the SDK applies temporal sharpening. ReShade runs once at the selected before/after position. FSR never receives a published-but-unfinished UI texture: native UI remains composed at the final presentation boundary.

The source ID comes from the existing render/jitter hook, and elapsed milliseconds come from a steady clock at that boundary. The neutral camera producer selects the retained player view, measures the actual unjittered projection and frustum, and reads the engine's world scale. Depth flags are measured, not assumed; a convention change retires real readers before recreating the context with unchanged allocations/provider/quality. First frames, camera changes/jumps, loading exit, skipped inputs and discontinuities reset history once. The 15-frame NVIDIA reset counter remains unchanged on NVIDIA paths; FSR acknowledges it once after successful temporal delivery. CS retains external temporal ownership and uses ordinary presentation without querying NVIDIA services; its existing effects boundaries remain in use.

A failed SDK dispatch may leave partial command-list state and CPU history. The adapter discards that never-submitted list, drains real preceding work, retains the poisoned context until normal teardown, and labels subsequent frames spatial recovery until restart. It records no fake submission/completion. Device or retirement failure stops rendering and retains owners. Recovery is bilinear native-size delivery, never reported as active FSR.

GPU tests verify explicit Linear/Gamma22/sRGB transfers against known SDR patches, independent alpha and encoded round trips, restored graphics state, forced vendor-error recovery pixels, discarded commands, and simulated device-loss classification. The official analytical provider now runs through the production frame adapter for 1,000 temporal dispatches and 25 contexts, with 25 additional spatial loading frames, 25 rejected invalid-time attempts, and exactly 75 reset frames. All 75 readbacks retain a completed native UI sentinel; 74 output samples change. Three outstanding submissions are still checked per context. SDK allocation counts remain 650 created, peak 26, zero remaining after retirement. Both debug layers remain unavailable.

Motion/jitter evidence: TRP captures R16G16_FLOAT motion at its render extent. Its jitter hook applies `(-2*x/width, +2*y/height)` and retains dispatch offsets `(-x,-y)`. Public reconstructed vanilla lighting uses unjittered current/previous projection to write current-to-previous UV motion; the integration uses positive width/height scales and no UNORM remapping. Reference revision `b54d184a5c3acc25c60e6666a46c68b09a3759ed`, file SHA-256 `529681df2c10f1ee50149e845725e556d04a8b58a90d711483e41e39f7c35459`: [reconstructed vanilla lighting source](https://github.com/aers/Skyrim-SE-Shader-Tools/blob/b54d184a5c3acc25c60e6666a46c68b09a3759ed/old/shaders/Lighting/BSLightingShader.ps.hlsl). This supports the conversion design; matching the installed shader/captures, camera/object motion quality, ENB/non-ENB output transfer and transparent effects remain gameplay acceptance gaps. The explicit Gamma22 finished-SDR policy follows the existing TRP handoff convention; known-patch tests do not prove Skyrim's actual encoding. AMD's [3.1.5 integration manual](https://gpuopen.com/manuals/fsr_sdk/techniques/super-resolution-upscaler/) supports the linear input, explicit motion scales, applied jitter and mip-bias contracts.

Verification command: `C:/Python314/python.exe out/research/run_task6_tests.py`. Standard and Universal Release both built and passed 113/113 runnable CTest cases each, including the production-adapter GPU run and NVIDIA regressions. VS2022 normal/delayed import inspection found no mandatory NVIDIA/AMD runtime DLLs in either edition. The three baseline Graphics Tools tests remain excluded/SKIPPED per edition. Plugin FSR admission remains disabled until authoritative settings are implemented. No game/MO2 configuration changed, and no Skyrim or AMD/Intel gameplay acceptance is claimed.

## Executed increment 7: authoritative settings and runtime status

FSR quality, provider policy and finite sharpness now round-trip through the existing Apply/Discard/Save draft and startup/requested/effective snapshots. Backend, quality and provider changes require restart; sharpness applies after Present without replacing allocations. The FSR mode choice explicitly prepares generation off/backend 0/NR off/HDR off. Contradictory or invalid requests are rejected before controller mutation. Packaged NVIDIA defaults remain selected; package/examples/FSR-SR supplies the separate FSR-only configuration. Enabled plugin admission now validates FSR selectors and settings. FSR-off builds still reject FSR.

Status requires an actual successful temporal frame and cached provider identity. Waiting, spatial recovery, terminal failure, requested changes and Community Shaders ownership are distinct. UI/settings queries avoid NVIDIA runtime services during FSR sessions. New settings tests first failed on the absent production header, then passed round trips, invalid numeric/quality input, allocation staging, discard, persistence, unsupported combinations and requested-versus-active checks. Both enabled Standard/Universal Release builds passed 114/114 runnable tests and normal/delayed import inspection. Three Graphics Tools cases remain SKIPPED per edition. No live setup changed.

## Executed increment 8: build matrix, fresh process and packages (2026-10-02)

All six Release configurations passed. The exact plugin SHA-256 values, actual provider identity, GPU allocations/VRAM and fixture outcomes are in [FSR_VALIDATION.json](FSR_VALIDATION.json).

| Configuration | Runnable CTest cases | Result |
| --- | ---: | --- |
| Standard FSR, NR compiled | 118 | PASS |
| Universal FSR, NR compiled | 118 | PASS |
| Standard FSR, NR excluded | 118 | PASS |
| Universal FSR, NR excluded | 118 | PASS |
| Standard FSR excluded | 108 | PASS |
| Universal FSR excluded | 108 | PASS |

The two FSR-off builds configured and built against `C:/deliberately-missing-fidelityfx-sdk`, which does not exist. Normal and delayed PE import inspections found no mandatory NVIDIA/AMD runtime imports in any plugin. Exactly `NativeUIComposition`, `NativeUIBlendState` and `NeuralPeripheralPixels` remain Graphics Tools-dependent SKIPPED checks per configuration; no test body or exclusion was weakened.

Each enabled edition ran the official analytical 3.1.5 provider through the production frame adapter for 1,000 temporal frames / 25 context lifetimes, 75 native readbacks / 74 changing samples, native UI sentinels, three outstanding transfers per context, 25 loading/spatial frames, 25 invalid attempts and 75 deliberate resets. All 650 SDK committed allocations were released (peak 26); no SDK heaps were created by this provider. VRAM observations remain bounded, rather than claimed zero or constant. Both debug layers were unavailable.

The separate fresh-process smoke loaded the built plugin through its normal imports, then exercised production ordinary-presenter, sizing, deferred FSR startup and frame helpers. Twelve temporal dispatches produced 12 native readbacks / 11 changing samples, followed by successful resource retirement and AMD module unloading. Missing approved runtimes failed even with the real DLLs in the working directory. NVIDIA runtime module count was zero. NVIDIA graphics-driver modules are expected on the RTX 4080 Super and are distinct from Streamline/NGX/NVAPI runtime dependencies. The fixture does not invoke the Skyrim SKSE entrypoint and explicitly reports Skyrim startup/gameplay as NOT RUN.

Package scripts validate source/header/license/runtime pins, selected build edition, manifest hashes, required files, selector combinations and normal/delayed imports before creating a ZIP. Positive fixtures and independent mutations reject altered runtimes, even after an altered manifest, missing shaders, the wrong edition, an extra AMD DLL, invalid quality spelling and unsupported delegation. Each negative case restores the valid package before the next mutation. Binary redistribution license and API MIT notices are retained verbatim. Existing NVIDIA defaults/packages remain intact.

The matrix exposed ordinary presentation's accidental reliance on an FSR library's `NOMINMAX` definition. `OrdinaryMacroEnvironment` reproduced the compilation failure with Windows macros enabled; parenthesized standard-library `max` calls made the actual presenter independent of that optional library. The dedicated regression and all six rebuilt suites pass.

Executed commands used VS2022 pinned through `VCPKG_VISUAL_STUDIO_PATH`, with dependency mutation kept sequential:

```powershell
cmake --preset <each of the six fsr-* presets>
cmake --build out/build/<preset> --config Release --parallel 2 -- /p:UseMultiToolTask=true /p:MultiProcMaxCount=4 /p:EnforceProcessCountAcrossBuilds=true
ctest --test-dir out/build/<preset> -C Release --output-on-failure -E '^(NativeUIComposition|NativeUIBlendState|NeuralPeripheralPixels)$'
pwsh -NoProfile -File tests/FSRPackageTests.ps1 -Edition <Standard|Universal> -BuildDirectory out/build/<preset> -RuntimeDirectory out/research/sdk-v2.3.0/runtime -ScratchRoot out/research/package-tests
pwsh -NoProfile -File tools/fsr/Stage-Package.ps1 -Edition <Standard|Universal> -BuildDirectory out/build/<preset> -RuntimeDirectory out/research/sdk-v2.3.0/runtime -OutputDirectory out/packages/2026-10-02/final/<preset>
```

The four final enabled packages are staged under `out/packages/2026-10-02/final/`, each with an edition-specific ZIP, complete notices and a manifest. ZIP contents were independently checked against the manifests, verified build bytes and clean embedded source revision `49cb69f8ccb1`. They are not installed. The [gameplay checklist](FSR_TEST_CHECKLIST.md) covers installed motion/jitter/depth and color calibration, ENB, transparency, ReShade/CS, native UI/previews, quality/odd/ultrawide dimensions, loading/camera/resize/alt-tab and GPU/CPU/VRAM measurements. Skyrim gameplay and AMD/Intel hardware remain NOT RUN. FSR frame generation remains NOT IMPLEMENTED / NOT RUN; this milestone establishes its SR prerequisite and does not advertise generated frames.

## Final review and regression fixes

The fresh whole-branch review found two Important regressions and no Critical or Minor findings. Ordinary FSR presentation now establishes ReShade manual-effects ownership through the public native swapchain handle, releases its automatic runtime before publication and rejects unprovable ownership. The NVIDIA FG checkbox now synchronizes its settings draft, preventing Apply/Save from reversing a visible immediate toggle. Both behavioral failures were reproduced before correction.

The supplied ReShade 6.7.3.2150 DLL (SHA-256 `059168b9d8aaa694a02a64342409fa26dfdf335035f2c0184cc61581deffc3bc`) was tested in isolated temporary directories with the actual ordinary presenter and the original NVIDIA presenter fixture. Both before/after placements, one effects pass, untouched native UI, source input, screenshots and runtime lifetime passed. Each of the six configurations now runs `ReShadePresentationRuntime` through optional `TRP_RESHADE_INTEGRATION_RUNTIME`; no injector is downloaded or installed by the build. FG Apply/Save is covered in both toggle directions with unrelated edits and saved-startup round trips. Fresh verification ran before committing the fixes, then all six builds/suites ran again from the clean committed code revision used by the final ZIPs: 688 runnable passes. The review record preserves all decisions and the separate game/hardware gates.

## 2026-10-02 external review corrections

The user-authorized correction pass addresses the review of `02981e4fbd7c`. Handoff format admission now shares the converter's exact whitelist and executes in `PrepareSizing` before any D3D12 device/bridge/context or reduced target is created. Unsupported R10, sRGB and typeless handoffs report the actual DXGI value instead of failing the first reconstructed frame. Supported formats remain RGBA8/BGRA8 UNORM, FP16 and FP32; format does not select transfer encoding.

`[FSR] SourceColorEncoding` is explicit: `Linear`, `Gamma22` or `SRGB`. Missing/`Unknown` stays unknown and prevents FSR startup or Apply. The setting persists in the existing authoritative draft/creation snapshot, appears in the Image tab, and requires Save/restart. The active adapter receives the immutable encoding admitted by its startup owner. The example INI explicitly selects provisional Gamma22; that is configuration, not evidence of installed Skyrim/ENB calibration. Startup logs the selected format and encoding once. HDR remains disabled and SDR values remain bounded to [0,1].

Camera measurement, history and dispatch now share one finite/infinite depth-range rule. Infinite mode requires positive infinity and a projection with the matching near endpoint/far asymptote; the producer evaluates the far limit without infinity-times-zero arithmetic. History rejects contradictory finite-far/infinite-flag combinations and resets on convention changes. Context recreation retires outstanding work and retains fixed texture identities.

Prepared texture descriptors and capability checks are role-specific. Color and motion use RTV+SRV; depth and native output retain RTV+SRV+UAV. Motion no longer requires typed UAV store. RTV capability remains on every shared intermediate because the real GPU probe rejects the proposed SRV-only motion and UAV-only depth/output descriptors at D3D11 shared opening. The existing COMMON/simultaneous-access policy and fence ordering are preserved.

External exposure/reactive/transparency guides remain unsupported in the production adapter and now produce an explicit diagnostic. Obsolete diagnostics are cleared for a new non-recovery evaluation; vendor-error recovery retains its original error. Provider selection stays centralized/tested and actual IDs remain verified. The pinned C API uses bit 9 for debug visualization, so AIO's old RCAS-compensation bit is deliberately not copied.

Regression coverage includes early format/unknown-encoding rejection without GPU ownership, role-specific motion capabilities and real allocation flags, finite/infinite LH/RH and reversed camera projections, history resets and context recreation, encoding persistence/restart behavior, invalid package encodings, and successful temporal plus forced-failure spatial delivery for all three transfers. The successful fixture records an actual D3D12 write of known linear output, retains descriptor heaps until retired context destruction, and independently reads decoded FP16 input and encoded native output. This isolates the integration contract; it is separate from the official vendor's 1,000-frame GPU test and from Skyrim image acceptance.
