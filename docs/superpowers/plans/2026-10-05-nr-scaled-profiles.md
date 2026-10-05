# Common Scaled NR and GPU Qualification Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task if the owner confirms native execution. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Enable fixed scaled DLSS/FSR sources through one post-upscale NR stage and deliver reproducible, real-device NR qualification for the supplied NVIDIA profiles.

**Architecture:** Keep DLSS/FSR -> NR -> optional FG -> native UI. Extend existing retained NR resources to carry independent display/color and render/guide extents, qualify the direct vendor smaller-guide contract, and reuse production stages in a portable validator. Keep software eligibility distinct from hardware acceptance.

**Tech Stack:** Windows x64, C++23/MSVC, D3D11/D3D12, pinned NGX/Streamline/FidelityFX, CMake/CTest, PowerShell, Windows driver/signature APIs.

**Spec:** [Approved design](../specs/2026-10-05-nr-scaled-profiles-design.md), approved by the owner's 2026-10-05 "go". Plan/execution approval is pending. Implementation baseline `f0de5c6`; existing native qualification baseline `5097338`.

## Global Constraints

- One common order for all NR profiles; migrate old Before settings to post-upscale.
- NR runs once per eligible real source; generated images receive no separate NR evaluation.
- Fixed scaled modes only where the SR backend supports them. Dynamic resolution, HDR NR, multiple NR passes and reduced NR model/input scales remain outside this extension.
- Keep the supplied exact model catalog: `rtx20-30`, `rtx40`, `rtx50`; AMD NR remains explicitly unsupported.
- Retain the accepted encoded SDR byte/full-tone contract, exact source alpha and native UI. No Stable colors or Tone=0 drift workaround.
- Preserve retained tickets/readers and injector/device identity checks; uncertain retirement never permits release or reuse.
- Local hardware is RTX 4080 SUPER. Actual RTX 20/30/50 runs are required; model cross-runs on RTX40 do not qualify those GPUs.
- Do not distribute the local driver core or silently trust a different core. No general signature bypass or unrelated DLL modification.
- No GPU probes alongside Skyrim, installation with Skyrim/MO2 open, or automatic game launch/termination.
- No new AIO performance comparison. Reuse build/dependency trees and compact receipts; preserve installed native rollback/settings.
- Graphics Tools exclusions: `NativeUIComposition`, `NativeUIBlendState`, `NeuralPeripheralPixels`; exclusions are not passes. Interactive vendor cases need their own foreground-qualified evidence.
- Six new milestones; report N of 6 when each is complete. The old native 8/8 remains historical acceptance.

## Review Focus

1. Display size stays fixed but guide size changes: retire old readers, recreate guides and reset history (Tasks 2/3).
2. Normalized or already output-scaled motion is scaled twice: explicitly name the scale domain and check independent displacement (Task 1).
3. Weather/preset import re-enables Before behind the common-order menu: enforce post-upscale at load/Apply and final source/legacy consumers (Task 2).
4. Wrong GPU/model or another installed driver core is accepted: bind adapter/LUID/model/file identity and reject production use of an unqualified core (Task 4).
5. An unavailable runtime or interrupted GPU test exits cleanly and becomes a PASS: receipts and aggregation must require actual evaluated output and retirement (Tasks 4/5).

## Worktree, Commands and Evidence

Use existing managed worktree `C:/Users/user/.codex/worktrees/fsr-sr/DvaKolbas`, branch `codex/nr`. Do not create a duplicate checkout. Verify the existing worktree/branch and read the spec before execution. Only remote `wallhead` is authorized; do not push `origin`.

Reuse `out/build/fsr-fg-standard`. In the command examples, define `$cmake` and `$ctest` to the corresponding executables under `C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin`. Run from the worktree. Check each exit code before dependent actions. New CPU targets enter existing CMake test wiring; new GPU targets carry `GPU`, `RUN_SERIAL` and explicit timeouts.

Create progress ledger `.superpowers/sdd/2026-10-05-nr-scaled-profiles/progress.md` with baseline, each task's files, RED/GREEN commands/results, actual GPU/device evidence and remaining gates. Keep small reviewed receipts under `research/nr/scaled/`; large raw outputs stay ignored under `out/research/nr/scaled/`. Record source/executable/runtime hashes and distinguish PASS, FAIL, UNAVAILABLE and NOT RUN. Commit each independently verified task with only its intended files.

## Task 1: Qualify the Native Smaller-guide Vendor Contract

**Files:** modify `src/NeuralRendering/PostSrContract.h`, `RuntimeParameters.h`, `Stage.cpp`; test `tests/NrRuntimeParameterTests.cpp`, `tests/nr-postsr/PostSrContractTests.cpp`, `PostSrGuideTests.cpp`, `CMakeLists.txt`; create `tests/nr-postsr/ScaledNrContractGpuTests.cpp`. Reuse independent `tests/nr-postfg/SyntheticScene.h` and existing GPU lifetime guards.

**Interfaces:** keep `DirectCreationContract` independent `colorExtent`/`guideExtent`. Add NR-local `enum class MotionScaleDomain { RenderPixels, DisplayPixels };` and `PostSrSourceContract::motionScaleDomain` defaulting to `RenderPixels`: it names the units produced AFTER multiplying texture motion by `motion.scaleX/Y`. Keep `Result<PostSrGuidePlan> ValidatePostSrSourceContract(const PostSrSourceContract&)`; its `extent` becomes actual guide extent and its motion scales convert to output-pixel displacement. Add `template<class Parameters> Result<void> WriteDirectSubrectParameters(Parameters&, ImageExtent color, ImageExtent guides)`; no resource binding/ownership occurs inside this parameter helper.

- [ ] **1. Add failing contract/parameter tests.** Use native 640x360 and color 1280x720/guides 640x360. Assert separate subrect sizes, typed unsigned extents, float ratio 1, preset 0, no NR upscaling, invalid extents rejected before parameter mutation. Test motion units:

```cpp
// Texture values * declared scale = render-pixel displacement.
source.motionScaleDomain = MotionScaleDomain::RenderPixels;
source.motion = {640, 360, true, false}; // normalized vectors
Require(ValidatePostSrSourceContract(source)->motionScaleX == 1280);
source.motion = {1, 1, true, false}; // render-pixel vectors
Require(ValidatePostSrSourceContract(source)->motionScaleX == 2);
source.motionScaleDomain = MotionScaleDomain::DisplayPixels;
Require(ValidatePostSrSourceContract(source)->motionScaleX == 1);
```

- [ ] **2. Run RED:** build `TRPNrRuntimeParameterTests`, `TRPNrPostSrContractTests`, `TRPNrPostSrGuideTests`; run `& $ctest --test-dir out/build/fsr-fg-standard -C Release --output-on-failure -R '^Nr(RuntimeParameters|PostSrContract|PostSrGuides)$'`. Record failure from unequal-extent/incorrect-subrect rejection, or a compiler failure for the newly introduced declared interface. Preserve stale/time/generated/jitter and independent lost-thin-geometry tests.
- [ ] **3. Implement metadata and parameter changes.** Accept positive render/guides no larger than display/color, each dimension <=16384. For RenderPixels multiply declared scales by display/render separately per axis; DisplayPixels leaves scales unchanged. Reject non-finite results. Creation color/model extents remain display-sized, scaling ratio 1 and preset 0. Stage uses actual guide subrectangles and independently bounded guide extents. Keep ordinary product scaled admission disabled until Task 3 passes.
- [ ] **4. Run GREEN and an isolated actual NR experiment.** Build new target `TRPNrScaledContractGpuTests`; register `NrScaledContractGpu`. Use actual RTX40 model/core and independent moving geometry at native, 2x and non-integral guide ratios. Assert successful actual Create/Evaluate, fully initialized changed RGB output, exact alpha, correct stored render guides, output-pixel motion math and complete delayed-reader retirement. Retain the thin-edge mismatch observation; do not assert invented recovered geometry. Change only guide bounds/units in negative controls and require the contract or independent oracle to reject them.
- [ ] **5. Gate and commit.** If the vendor rejects the smaller-guide recipe, retain the receipt and stop scaled shipping work for design revision; do not substitute nearest enlargement. Otherwise save `research/nr/scaled/vendor-guide-contract.json`, including actual parameter values/native codes and narrow local profile coverage; commit `Qualify direct NR display color with render-sized guides`. Report **1 of 6**.

## Task 2: Retained Scaled Bridge and Common Order Migration

**Files:** modify `src/NeuralRendering/BeforeUpscale.cpp`, `PreparedBeforeUpscale.cpp`, `BeforeHost.cpp`, `PostUpscale.cpp`, `BeforeSettings.h`; modify `src/FrameGen/SourceDLSSGSettings.h`, `NvidiaHostNeural.cpp`, `NvidiaHostStartup.cpp`, `SourceNvidiaFrameEvaluator.h`, `SourceNvidiaEvaluation.cpp`, `SourceFsrEvaluation.cpp`; modify `src/RendererSettings.h`, `RendererSettingsController.cpp`, `OverlayNeuralPanel.cpp`, `OverlayFrameView.h/.cpp`, `OverlayPresetsPanel.cpp`, `WeatherAppearanceRuntime.cpp`. Review legacy `SourceDLSSGNeuralStage.cpp`, `SourceDLSSGBackend.cpp` final consumer paths. Tests: `NrBeforeHostFramesTests.cpp`, `NrPreparedBeforeTests.cpp`, `NrBeforeUpscaleTests.cpp`, `NeuralPassSettingsTests.cpp`, `RendererSettingsActionTests.cpp`, `settings-controller/ControllerTests.cpp`, `SourceFrameEvaluatorTests.cpp`, `NeuralWorldContractTests.cpp`.

**Interfaces:** preserve existing bridge Initialize/Evaluate/WaitDelivery/TrackReader/Retire signatures and `BeforeHost::EvaluatePost(const PostSrInput&, const SettingsSnapshot&)`. Internal Before fixtures remain usable. Production preferences retain the deprecated serialized `NRBeforeUpscaling` key with value false for compatibility. Existing `Preferences SanitizePreferences(Preferences)` enforces false; production snapshots/legacy options always use After. `PostUpscale::Evaluate` validates resource binding against Task 1's source contract and calculated motion scales.

- [ ] **1. Add failing bridge, ordering and migration regressions.** Hold a genuine independent GPU reader while guide extent changes 640x360 -> 426x240 with display fixed 1280x720; require old resource lifetime and history reset. Test invalid guide texture bounds, source IDs and foreign devices. Load `NRBeforeUpscaling=true`, import a preset requesting Before and Apply a draft requesting Before; each must execute one post-SR pass and serialize false, retaining unrelated tuning/provider/FG values. Assert temporal SR -> source effects -> NR -> FG; off/spatial recovery has no NR evaluation and FG-off still has one NR source pass. Legacy/common NR cannot both run.

```cpp
auto prefs = LoadPreferences(legacyIni);
Require(!prefs.neuralBeforeUpscaling);
Require(prefs.neuralTuning == savedTuning);
// Independent operations trace asserts one NR, after SR and before FG.
```

- [ ] **2. Run RED** on affected bridge, settings-controller and source evaluator targets via the existing CTest names discovered with `& $ctest --test-dir out/build/fsr-fg-standard -N`. Record each meaningful failure; do not claim an unchanged existing pass is RED.
- [ ] **3. Separate retained color/guide sizes.** Allocate/copy color at display extent and depth/motion at guide extent in all three retained slots and interop resources. Prepared validation uses each actual shape; source-alpha/color conversion remains display-sized. Recreate when either extent changes, after genuine registered readers retire. Use Task 1's planned motion scales in both metadata and actual packet. Check the source camera at the real render extent; diagnostic readbacks must also respect independent texture dimensions.
- [ ] **4. Enforce common order at every production entry.** Pre-SR callback may publish settings/reset but cannot evaluate NR. Post callback is the sole real-source evaluation. Normalize legacy INI/defaults, drafts, preset/weather consumption and final options; remove user placement choices and show `After upscaling, before frame generation`. Keep historical internal Before tests, not a hidden user-selectable Before route. Preserve live tuning/off-on and deferred Apply/retirement behavior.
- [ ] **5. Run GREEN**, including native bridge/color regressions and real pending-reader scale change. Save `research/nr/scaled/common-order-and-retirement.json`; commit `Retain scaled NR guides and enforce common post-upscale order`. Report **2 of 6**. Product scaled UI admission is still gated until provider proof.

## Task 3: Scaled DLSS/FSR Source, FG and Lifecycle Proof

**Files:** extend `tests/nr-postsr/PostDlssSourceGpuTests.cpp`, `PostUpscaleGpuTests.cpp`, `PostFsrLifecycleGpuTests.cpp`, `TaggedSourceReader.h`, `CMakeLists.txt`; modify actual source adapters from Task 2 only where proved incorrect. Update `src/NeuralRendering/BeforeSettings.h` and corresponding renderer settings tests after qualification.

**Interfaces:** existing production SR backends/evaluator and retained source readers. Add fixture mode `--scaled` to actual DLSS and FSR source tests, `--scaled-lifecycle` to lifecycle cases; reuse registered vendor presentation machinery and output receipts. No new SR quality enum is required: test existing backend modes only. FSR currently exposes Quality/Balanced/Performance/NativeAA; do not invent FSR Ultra Performance.

- [ ] **1. Add failing actual-source checks.** For each existing fixed scaled quality, assert actual render-sized guides, actual temporal SR, one NR evaluation per admitted source, exact alpha and enhanced scene/tag bytes consumed by FG. Keep a separate unmodified SR-history/input capture proving no NR feedback. Test FG off and off/on, style 0 -> 1 -> 0/full Tone 1, menu/loading, camera cuts, guide-only size change and pending old FG readers. Add unsafe/no-reader completion negative controls; require rejection rather than success through skipped evaluation.
- [ ] **2. Register/build and run RED:** `NrPostDlssScaledSourceGpu`, `NrPostFsrScaledSourceGpu`, `NrPostFsrScaledLifecycleGpu`; optional foreground vendor case `NrPostDlssScaledVendorGpu` carries `GPU;Interactive`. Failure from current product admission must be distinguished from failed vendor inference or resource ownership.
- [ ] **3. Wire qualified delivery where needed.** Keep enhanced real color in separate storage; FSR must upload the NR result and NVIDIA must tag the NR result. Reuse existing source delivery/reader holds. All outputs keep explicit SDR encoding, one UI composition and real-source epoch/history. Once non-interactive provider tests pass, remove fixed-scale/native-only checks in production admission while keeping DRS/HDR/reconstruction/ownership restrictions and profile/runtime qualification diagnostics.
- [ ] **4. Run GREEN:** `& $ctest --test-dir out/build/fsr-fg-standard -C Release --output-on-failure -R '^NrPost(DlssScaledSource|FsrScaledSource|FsrScaledLifecycle)Gpu$'`. Run native source/lifecycle cases too. Qualify actual FSR generation/presentation and NVIDIA foreground output on eligible local hardware; no background/skipped vendor case counts as passed. Readbacks prove enhanced real/FG source and alpha/UI; vendor API success alone is insufficient. Preserve precise limitations for opaque generated pixels and visual quality.
- [ ] **5. Save** `research/nr/scaled/local-provider-validation.json` with tested qualities/extents/readers and actual card. Commit `Validate scaled post-SR NR with DLSS FSR and FG`. Report **3 of 6** only for genuinely completed local source/presentation/lifecycle coverage.

## Task 4: Portable Per-profile and Driver Qualification

**Files:** create `src/NeuralRendering/DriverCoreLease.h/.cpp`, `tests/NrDriverCorePolicyTests.cpp`, `tests/nr-runtime/PortableProbeReport.h/.cpp`, `tests/NrPortableProbeReportTests.cpp`, `tools/nr/Run-GpuQualification.ps1`, `tests/nr-runtime/GpuQualificationTests.ps1`; modify `src/NeuralRendering/RuntimeOwner.h/.cpp`, `BeforeHost.cpp`, `StartupSettings.h`, `tests/NrStartupSettingsTests.cpp`, `NrBeforeHostTests.cpp`, `RuntimeCatalog.cpp` only if verified IDs need updating, `cmake/NeuralRuntime.cmake`, `tests/nr-runtime/RuntimeProbe.cpp`, `RuntimeProbeReport.h/.cpp`, `CMakeLists.txt`, `tests/nr-postsr` fixtures to accept actual-profile selection and receipt paths. Retain existing direct probe CLI compatibility.

**Interfaces:** add `enum class DriverCorePolicy { Production, Qualification };`, default Production in `RuntimeOwnerPaths`. `Result<std::filesystem::path> DiscoverInstalledDriverCore(AdapterIdentity)` identifies the selected adapter's installed core; `DriverCoreLease::Open(const std::filesystem::path&, AdapterIdentity, DriverCorePolicy)` returns `Result<DriverCoreLease>` and internally validates trusted installed-driver provenance; it cannot accept a caller-created "verified" boolean. It retains exact held file identity and exposes Path/Matches/hash/size evidence to RuntimeOwner. Production uses reviewed exact core pins; Qualification does not change them. Existing `RuntimeOwner::Open` remains the production execution interface. Add internal `Result<void> CheckParameterAbi(const RuntimeExports&)` before feature use, with failure preventing further runtime execution.

- [ ] **1. Add failing policy/report tests.** Assert RTX20 and RTX30 both select only `rtx20-30`, RTX50 selects only SignedDirect `rtx50`; mismatched adapter/LUID/model/hash fails. A different core cannot pass Production policy. Qualification rejects unsigned/unrelated/replaced cores and missing/wrong ABI exports. Mock pure trust-policy decisions only; real signature/provenance checks get integration evidence. Failed/incomplete output, allocation leaks, unknown family, unsupported requested FG and unretired readers must not aggregate to a GPU pass.
- [ ] **2. Run RED:** build new CPU targets and run `NrDriverCorePolicy`, `NrPortableProbeReport`, `NrGpuQualificationRunner`; validate script parsing and orchestration with test receipts, including paths containing spaces and per-case failure exits. No model/GPU spoofing in actual hardware fixtures.
- [ ] **3. Implement portable discovery and ownership.** Discover the actual render adapter and its installed NVIDIA driver package via Windows device/driver APIs; verify driver/core provenance with Windows signature/catalog APIs, hash/hold the actual core and match loaded identity. An empty game `DriverCore` setting uses actual-adapter discovery under Production policy; a supplied path is still strictly verified rather than silently replaced. Keep the current installed explicit path unchanged. Test empty-setting resolution, discovery failure and wrong explicit paths without vendor initialization. The isolated qualification policy allows a new verified core for probing only. Test typed int/unsigned/float/pointer Set/Get ABI and proper parameter destruction before recording NR work; quarantine uncertain partial owners. Retain exact model pins and each model's own compatibility policy. Do not copy driver cores into validator packages.
- [ ] **4. Implement validator CLI and receipt.** `Run-GpuQualification.ps1 -RuntimeRoot <models-root> -ExecutableRoot <validated-exes> -OutputDirectory <receipts> [-QualificationDriver] [-DriverCore <actual-installed-core>]`. Auto-select the real eligible model, use actual core discovery, and run native/scaled NR source/output plus pending-reader cases from production code. Record separate SR/FSR-FG/NVIDIA-FG eligibility and results; unsupported NVIDIA FG on RTX20/30 is UNAVAILABLE, not NR failure. Preserve existing export/Init/Create/Evaluate/Release/Shutdown native results and actual driver/device/runtime identities.
- [ ] **5. Run GREEN** on CPU/orchestration tests and the real local RTX40 native/scaled qualification. Explicitly leave actual RTX20, RTX30 and RTX50 NOT RUN until receipt collection on those cards. Save `research/nr/scaled/portable-validation.json` and `support-matrix.json`; commit `Add portable real-GPU NR qualification and driver policy`. Report **4 of 6** for validator readiness/local proof, without implying other hardware passed.

## Task 5: Clean Matrix, Final Review and Trial Package

**Files:** modify `tools/nr/Stage-PostSrTrial.ps1`, `Validate-NvidiaTrialAssets.ps1`, `tools/nr/runtime-pin.json` only for reviewed actual evidence; extend relevant package tests. Create `research/nr/scaled/final-matrix.json`, `final-review.json`, `trial-manifest.json`; update `docs/NR_PROGRESS.md` with new extension count and precise support scope.

**Interfaces:** staged mod retains existing package layout/model hashes. Driver policy and migrated post-upscale placement are versioned in the manifest. Standalone validator bundle contains executables/scripts/manifests and model path requirements, not copied installed driver files. Support matrix distinguishes SOFTWARE ELIGIBLE from actual card/driver/model tested entries.

- [ ] **1. Add failing package/report checks** for missing profile/model, stale source/hash, a copied driver core, old Before enabled, missing GPU receipt and skipped cases counted as passed. Keep accepted installed DLL/INI/profile/launch state out of build/test writes.
- [ ] **2. Build clean NR-on and NR-off product configurations** using existing dependency/cache values; retain compact DLL/config/test evidence rather than duplicate large packages. Run affected packaging tests GREEN and full noninteractive runnable suite:

```powershell
& $ctest --test-dir out/build/fsr-fg-standard -C Release --output-on-failure -LE Interactive -E '^(NativeUIComposition|NativeUIBlendState|NeuralPeripheralPixels)$'
```

Record NR-off tests independently. Runtime-dependent skips stay skips unless actually rerun with required pinned arguments. Do not rerun a completed suite absent new changes/failures/concerns.
- [ ] **3. Perform one fresh whole-extension review** of product, runtime and fixture changes, using the executing-plans final review workflow if native execution is confirmed. Review Focus must include the five named cases and source/color/FG reader ownership. Fix material findings with focused RED/GREEN tests, then rerun only invalidated verification and update final matrix receipts.
- [ ] **4. Stage one validated trial package** using `Stage-PostSrTrial.ps1` with actual BuildDirectory/AcceptedModDirectory/OutputDirectory paths; preserve current native package/backups. Validate no automatic installation and no driver-core distribution. Document migration/common order and per-hardware run instructions. Keep any new core pin absent until its real qualification evidence has been reviewed.
- [ ] **5. Commit** `Stage validated common scaled NR trial and GPU validator`; record manifest/build/review evidence and report **5 of 6**. Installation is a later safe boundary with Skyrim/MO2 closed.

## Task 6: Skyrim and Actual RTX20/30/50 Acceptance

**Files:** create `research/nr/scaled/skyrim-acceptance.json`, actual per-device receipts and `hardware-acceptance.json`; update `support-matrix.json`, `docs/NR_PROGRESS.md` and the extension ledger. Product/core pins change only when a reviewed real result requires them.

**Interfaces:** owner-reported visuals plus correlated logs and actual validator receipts; each binds installed/source/runtime identities. A support entry names actual PCI device, driver/core, model and tested native/scaled modes, not an untested entire marketing generation.

- [ ] **1. Install only at the safe boundary.** Confirm Skyrim/MO2 closed, back up the working native DLL/INI and profile state, replace validated product files and perform only required post-upscale migration/provider/quality trial changes. Preserve launch/profile settings and rollback. Ask the owner to start Skyrim; the agent does not launch it.
- [ ] **2. Accept scaled DLSS then scaled FSR** with NR/FG off-on, source color/HUD, Style 0 -> 1 -> 0/full Tone 1, menu Apply, inventory/map/dialogue, save/reload, fast travel, alt-tab and minimize/restore. Inspect matching logs for real scales, one NR evaluation/source, active FG source and errors. Failed cases reopen their affected task; absent visuals do not become passes from logs alone.
- [ ] **3. Obtain actual separate RTX20, RTX30 and RTX50 validator runs.** Supply the portable bundle/instructions; collect actual GPU output/lifecycle receipts and appropriate FG eligibility. No hardware currently available locally means these entries remain NOT RUN. Review any newly tested exact driver core before production catalog admission and retest only affected cases.
- [ ] **4. Publish precise support/acceptance and commit.** Mark the extension **6 of 6** only after local Skyrim and required actual card qualification pass. If hardware is unavailable, leave Task 6 partially complete and state which device/driver runs remain required. Preserve the previously completed native 8/8 baseline and do not label software readiness as hardware qualification.

## Plan Self-review and Execution Handoff

Spec coverage: common order/migration and live controls -> Task 2; scaled geometry/units -> Task 1; SR-history isolation/enhanced FG/alpha/UI/lifecycle -> Tasks 2/3; profiles/driver provenance and actual output status -> Task 4; clean builds/review/package/rollback -> Task 5; Skyrim/external GPUs -> Task 6. Review Focus cases each have named tests in their owning tasks. All neighboring tasks preserve existing signatures except explicitly declared Task 1 metadata/parameter helpers and Task 4 driver lease/policy. No unknown GPU is promoted, unsupported mode invented or nearest-guide oracle weakened.

Recommended execution: **native/inline**, reusing the current worktree and fixtures; the tasks share runtime/retirement interfaces and local GPU runs serialize. One fresh whole-extension review comes at Task 5. Written plan and execution choice must be confirmed before implementation starts.
