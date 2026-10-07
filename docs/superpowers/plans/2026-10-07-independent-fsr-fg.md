# Independent FSR Frame Generation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Restore selectable DLSS/DLAA + FSR FG alongside the existing FSR + FSR FG and DLSS/DLAA + NVIDIA FG routes.

**Architecture:** Separate the saved FG preference from the active presentation owner. Extend the existing FSR resource owner with an explicit generation-only mode that shares guides without creating FSR SR, and reuse the existing presenter, UI transport and retirement machinery. Route successful DLSS and optional NR output to that owner with one immutable source snapshot.

**Tech Stack:** C++23, D3D11/D3D12 interop, NGX DLSS, existing community NR runtime, qualified FidelityFX SR/FG runtimes, ImGui, CMake/CTest, PowerShell, generated named INI schema.

**Spec:** `docs/superpowers/specs/2026-10-07-independent-fsr-fg-design.md` (approved).

## Global Constraints

- After ordering is **DLSS/FSR → NR → FG → UI**; Before NR retains its existing position. NR must not process the HUD.
- FG on/off remains live. Selecting a different presentation backend requires Save as default and restart.
- Public `[FrameGeneration] Backend` accepts `Auto`, `NVIDIA`, `FSR`; saving preserves `Auto`.
- Ordinary/backend 0 stays available only through the existing diagnostic option.
- AMD supports FSR SR + FSR FG. FSR SR + NVIDIA FG remains unavailable with an explicit explanation.
- The mixed route must not create or dispatch FSR SR, or select its INT8 loader from inactive SR preferences.
- Retain existing FSR3/FSR4 hardware restrictions and the FSR presenter's SDR/fixed-sizing contract.
- One presentation owner; preserve callback and source/guide/scene/UI reader lifetimes through retirement.
- No performance comparison, fullscreen work, AMD NR, new MFG algorithm, driver setting changes, DLL patches or old-INI migration.
- Work in the existing `codex/nr` worktree. Build Universal, retain version 1.2.0 unless separately authorized, and do not deploy intermediate commits.
- Preserve the current installed INI and MO2 launch/profile settings. Install only with Skyrim and MO2 closed.

## Review Focus

1. Save while a different backend is staged: retain the configured preference and operate live toggles on the current owner (Tasks 1 and 4).
2. DLSS paired with inactive FSR4 SR/INT8 preferences: use the official FG runtime and create zero FSR SR contexts (Tasks 2 and 5).
3. Failed SR, repeated source identity or missing guides: suppress interpolation, then reset history on valid recovery (Tasks 2–4).
4. Resize or NR/style reentry with pending SDK readers: retire every relevant reader before reuse, without calling an absent NVIDIA owner (Tasks 3–5).
5. AMD or diagnostic backend 0 plus a saved NVIDIA preference: resolve/validate explicitly without enabling an unsupported or stacked owner (Tasks 1 and 4).

## File map

- `src/FrameGen/GenerationBackendPreference.h` (new): typed configured preference and pure resolver.
- `tools/ini/schema.json`, `tools/ini/Generate-Ini.py`, `src/PublicIni.h`, `tools/ini/IniLayoutCommon.ps1`: public decode/encode, comments and tooling parity. Regenerate `src/PublicIniSchema.h`, package INIs and INI reference with `tools/ini/Generate-Ini.py`.
- `src/RendererSettings.h`, `src/RendererSettingsEdits.cpp`, `src/RendererSettingsController.cpp`, `src/RendererBackendPolicy.h`, `src/NvidiaBaselinePolicy.h`, `src/RenderPipeline.cpp`, `src/FrameGen/SourceFrameGeneration.h`, `src/FrameGen/SourceFrameGeneration.cpp`: saved preference, validation and active-owner separation.
- `src/Upscaling/FSRHostResources.h/.cpp`: existing SR owner plus explicit external-upscaler resource mode.
- `src/Upscaling/FSRGenerationGuideAdapter.h/.cpp` (new): validated external D3D11 depth/motion conversion into shared resources; no SR dispatch.
- `src/FrameGen/FSRHostPresentation.h/.cpp`: external source creation, sizing and presentation using existing low-level `FSRPresentation`.
- `src/FrameGen/NvidiaHost.h`, `NvidiaHost.cpp`, `NvidiaHostStartup.cpp`, `NvidiaHostLifecycle.cpp`, `NvidiaHostNeural.cpp`, `SourceNvidiaEvaluation.cpp`, `SourceUpscalerConfiguration.cpp`, `SourceFsrEvaluation.cpp`, `GameSwapChain.cpp`: select and retire actual presenter, preserve NR and publish DLSS sources.
- `src/FrameGen/SourceNvidiaFrameEvaluator.h`, `src/OverlayFrameGenerationPanel.cpp`, `src/OverlayFsrGenerationControls.h`, `src/OverlayFrameView.h`: presenter-neutral preparation and current/pending menu state.
- Existing tests named below plus new `tests/FSRExternalSourceTests.cpp`, `tests/fsr-fg/DlssFsrGenerationGpuTests.cpp` and `tools/fg/Run-Dlss-Fsr-FG.ps1`: unit, interop and actual mixed-route evidence.
- `CMakeLists.txt`, `cmake/FSR.cmake`, `tests/fsr-fg/CMakeLists.txt`: register new files/targets in the existing runtime/build structure.
- `package/README.md` and a tracked validation note: supported combinations and concrete verification results.

## Verification commands

Run from the worktree in PowerShell. Resolve the existing CMake tools once:

```powershell
$cmake = 'C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe'
$ctest = 'C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe'
$build = 'out/build/fsr-fg-universal'
```

For each red/green cycle, build affected test targets with `& $cmake --build $build --config Release --target <targets>` before running CTest. Red means an assertion fails for the intended missing behavior; a build failure due to a new declared interface may establish the first red state, but repair unrelated failures before proceeding. Reconfigure after CMake additions using the existing cache with `& $cmake -S . -B $build`. Exit 77/skips are **not qualification** for required GPU tests.

### Task 1 / milestone 1: Independent saved preference and route policy

**Files:** New preference header; named INI, settings, startup, backend policy and save files from the map. Tests: `tests/PublicIniTests.cpp`, `tests/PublicIniPackageTests.ps1`, `tests/BackendSelectionTests.cpp`, `tests/StartupPreferencesTests.cpp`, `tests/RendererSettingsActionTests.cpp`, `tests/settings-controller/ControllerTests.cpp`.

**Interfaces:** In `TheosRenderPipeline`, produce `enum class GenerationBackendPreference { Auto, Nvidia, Fsr };` and `long ResolveGenerationBackend(GenerationBackendPreference, Upscaling::BackendKind, bool ordinaryDiagnostic)`. Its return values remain existing 0/1/2. Add `generationBackendPreference` to settings drafts/startup settings; retain `generationBackend` as the effective value consumed by owners. Public schema maps the new choice to internal `FrameGeneration/BackendPreference`, while `FrameGeneration/Backend` remains derived runtime data.

- [ ] Write regressions `AutoRoundTripDoesNotBecomeEffectiveBackend` (`Encode(Decode(Auto)) == Auto`), `DlssExplicitFsrSelectsFsrPresenter` (resolved 2), `AutoFollowsUpscaler` (DLSS/DLAA 1, FSR 2), `AmdRejectsNvidiaPresenter`, `FsrRejectsNvidiaPresenter`, and `OrdinaryIsDiagnosticOnly` (0 only for diagnostic FSR with FG off). Cover C++ and PowerShell public codec parity and unknown choice rejection.
- [ ] Build/run `& $ctest --test-dir $build -C Release -R '^(PublicIni.*|StartupPreferences|BackendSelection|RendererSettingsActions|RendererSettingsController)$' --output-on-failure`; expect the new assertions to fail against automatic-only selection.
- [ ] Implement resolver, public schema/comments and preference persistence. Decode the public Backend string into BackendPreference before writing derived internal Backend; Encode reads BackendPreference and removes the derived integer before emitting the public choice. Validate SDR, NativeUI and fixed sizing by **presentation kind**, not only FSR SR. Keep per-upscaler preferences without replacing an explicit saved FG selection; Auto follows the selected SR. Update generator prose that currently says the presenter always follows SR. Generate with `C:/Python314/python.exe tools/ini/Generate-Ini.py`; verify with `C:/Python314/python.exe tools/ini/Generate-Ini.py --check`. Keep mixed-route activation unavailable in the startup owner until Task 4; no intermediate package.
- [ ] Run the same tests; expect zero failures and inspect the generated choice comment on its own line. Update portable package expectations for the extra known setting.
- [ ] Commit `feat: separate frame generation backend preference from upscaler`; report **1 of 6 done**.

### Task 2 / milestone 2: Generation-only resources and current-source guides

**Files:** `FSRHostResources.h/.cpp`; new guide adapter; `tests/FSRExternalSourceTests.cpp`; root/runtime CMake registration.

**Interfaces:** In `Upscaling`, add to `FsrHostResources`: `Result<void> PrepareExternalSizing(ID3D11Device*, Extent render, Extent display, DXGI_FORMAT, ColorEncoding, FsrInputPolicy)`; `Result<void> CompleteExternalStartup()`; `Result<void> ResizeExternalSizingAfterRetirement(Extent render, Extent display, DXGI_FORMAT)`; `bool ExternalSource() const`; `bool GenerationInputsReady() const`; `FsrInputPolicy GenerationInputPolicy() const`; `Extent RenderExtent() const`. Existing `Bridge()`, `Runtime()`, `Resources()` and retirement methods serve both modes. Produce `FsrGenerationGuideAdapter` with constructor `(std::shared_ptr<Graphics::D3D11D3D12Interop>, GpuFrameResources, ID3D11Texture2D* depth, ID3D11Texture2D* motion)` and `Result<void> Prepare(const UpscaleFrame&)`; retain adapter/resources until bridge retirement.

- [ ] Write `ExternalSourceCreatesNoSrContext` (no SR query/create/dispatch), `InactiveMlSrDoesNotSelectInt8` (official loader/FG path), `GuidesRequireCurrentMatchingSource` (wrong extents/device, stale identity and missing guides rejected), and `ExternalGuidesPreserveJitterAndMotionConvention` (snapshot equals actual source, not reconstructed defaults). Add a real interop case copying depth/motion on the same adapter; verify texture contents and retirement before destruction.
- [ ] Register CPU test `FsrExternalSource` and serial GPU test `FsrExternalGuidesGpu`; run `& $ctest --test-dir $build -C Release -R '^FsrExternal(Source|GuidesGpu)$' --output-on-failure`; expect missing-interface/assertion failures.
- [ ] Implement explicit SR/external modes. Extract shared device/format/queue initialization locally from existing owner without creating another runtime owner. The external mode loads the qualified official FG loader/module only, allocates shared render-sized guides, copies via the existing context isolation/depth conversion machinery and publishes producer synchronization. Do not fabricate an SR outcome or use `FsrFrameAdapter` to dispatch SR. Invalid frames do not publish a current valid source; recovery requests temporal reset.
- [ ] Rebuild/run new tests plus existing FSR host/resource tests; expect zero failures, real GPU contents checked and SR count zero.
- [ ] Commit `feat: prepare FSR generation inputs for external upscalers`; report **2 of 6 done**.

### Task 3 / milestone 3: Presenter lifecycle without an FSR SR feature

**Files:** `FSRHostPresentation.h/.cpp`; new guide adapter integration; `tests/FSRGenerationHostTests.cpp`, `tests/FSRGenerationHostOwnerTests.cpp`, `tests/fsr-host-resize/FsrHostResizeTests.cpp`, `tests/FSRGenerationPresentationTests.cpp`.

**Interfaces:** Add `Result<Extent> FsrHostPresentation::CreateExternal(IDXGIFactory*, ID3D11Device*, std::shared_ptr<Upscaling::FsrHostResources>, const DXGI_SWAP_CHAIN_DESC&, const Upscaling::FsrSettings&, Upscaling::Extent render, Upscaling::FsrInputPolicy)` and `Result<FsrHostResize> ResizeExternal(const DXGI_SWAP_CHAIN_DESC&, Upscaling::Extent render)`. Existing `Present(const UpscaleFrame&, UpscaleOutcome, ID3D11Texture2D*, ID3D11ShaderResourceView*, bool, bool, bool, UINT, UINT)`, `Suspend/Resume/Retire/BeforeResize` remain common. They consume resources and input policy from Task 2 without dereferencing `Upscaler()` for an external source.

- [ ] Write `ExternalPresenterRequiresNoSrFeature`, `MissingOrFailedSourceCannotGenerate`, `ResumeResetsGeneration`, and `ExternalResizeRetainsAllReaders` (guide/scene/UI/callback readers held until retirement; no sized-resource replacement on timeout). Check the existing FSR SR presenter still obtains its own limits and dispatches normally.
- [ ] Run `& $ctest --test-dir $build -C Release -R '^(FsrGenerationHost.*|FsrGenerationPresentation|FsrHostResize)$' --output-on-failure`; expect external owner/readiness failures.
- [ ] Implement shared presenter creation and mode-specific resource readiness/sizing. Feed validated external guides and post-upscale world scene into existing low-level `FsrPresentation::Present`. Preserve UI registration, SDK serialization, single HWND ownership and independent reader fences. Every transition uses the actual presentation owner; failed retirement retains ownership and refuses reuse.
- [ ] Rebuild/run the same tests and existing transport tests; expect zero failures. No actual runtime qualification claim yet.
- [ ] Commit `feat: support external-upscaler FSR presentation lifecycle`; report **3 of 6 done**.

### Task 4 / milestone 4: DLSS/NR integration and correct menu controls

**Files:** Nvidia host/startup/lifecycle/neural/evaluation/configuration and GameSwapChain files; source evaluator and overlay files from the map. Tests: `tests/SourceNvidiaFrameEvaluatorTests.cpp`, `tests/OverlayFsrGenerationTabTests.cpp`, settings-controller tests and game-swapchain tests.

**Interfaces:** `NvidiaHost::FsrFgActive() const` reports effective FSR presentation regardless of SR. Add private `Upscaling::Result<void> QuiesceActivePresentation()` and `Upscaling::Result<void> ResumeActivePresentation()` used by upscaler/NR reconfiguration; quiescence retains the presenter while proving its readers retired (FSR Suspend/Resume or existing NVIDIA Quiesce/Resume), rather than destroying its swapchain. Produce `Upscaling::Result<void> PrepareExternalGeneration(const Upscaling::UpscaleFrame&, Upscaling::UpscaleOutcome)` on the host; it consumes Task 2 guides and Task 3 presenter. Adapt `SourceNvidiaFrameEvaluator::Evaluate` operations with `PrepareGeneration(const SourceNvidiaFrameInputs&, const Upscaling::UpscaleFrame&) -> Upscaling::GenerationPreparationStatus`, selected by actual owner. One source snapshot supplies current camera, guide conventions, identity, jitter and dimensions to NR and FG.

- [ ] Write `DlssFsrOwnerDoesNotPrepareStreamline`, `AfterNrIsFsrGenerationScene` (order DLSS→NR→FG, native HUD excluded), `ExternalFailureSuppressesThenResets`, `StagedBackendDoesNotRedirectLiveToggle`, `FsrControlsVisibleWithDlss`, and `ConfiguredPreferenceSurvivesSave`. Exercise current NVIDIA vs pending FSR and current FSR vs pending NVIDIA; three NR passes and Before/After/style reentry; GPU policy rejects unsupported AMD/FSR+NVIDIA combinations with actionable text.
- [ ] Run `& $ctest --test-dir $build -C Release -R '^(SourceNvidiaFrameEvaluation|OverlayFsrGenerationTab|RendererSettings.*|GameSwapChain.*|BackendSelection|StartupPreferences)$' --output-on-failure`; expect preparation/menu/retirement routing failures.
- [ ] Implement mixed-route startup using NGX render-size query and Task 3 external presenter; acquire the compatible producer device on the actual adapter. Configure common NR independently of Streamline swapchain creation. DLSS sizing, jitter and dispatch stay NGX; publish final post-NR output and guides once. Preserve legacy NR only on its valid NVIDIA-owner route; explain unavailable legacy NR when community runtime is disabled on mixed FSR presentation. Route presents/resizes/suspension and retirement through the actual owner. Enable mixed-route activation only after these changes. Show Backend Auto/NVIDIA/FSR, active/pending restart state and FSR provider controls independently of SR; live FG toggles use the running owner.
- [ ] Rebuild plugin and run the same tests plus all existing NR settings/reader contracts. Inspect startup messages: separate SR, effective FG backend, FG provider and inactive SR policy; expect zero failures and no absent-owner retirement calls.
- [ ] Commit `feat: route DLSS and NR output through selectable FSR frame generation`; report **4 of 6 done**.

### Task 5 / milestone 5: Actual mixed-route GPU qualification and review

**Files:** New `tests/fsr-fg/DlssFsrGenerationGpuTests.cpp`, `tests/fsr-fg/CMakeLists.txt`, `tools/fg/Run-Dlss-Fsr-FG.ps1`; reuse source-fixture support from `tests/nr-postsr` and presenter observation from `tests/fsr-fg`. Add tracked `docs/validation/2026-10-07-independent-fsr-fg.md` receipt summary.

**Interfaces:** Target `TRPDlssFsrGenerationGpuTests`; CLI `--quality native|quality|performance --nr <path> --core <path> --runtime <official-fsr-path> --output <json>`. Register serial tests `DlssFsrGenerationGpu-Native`, `DlssFsrGenerationGpu-Quality`, `DlssFsrGenerationGpu-Performance`. Runner parameters `[string]$BuildDirectory`, `[string]$OutputDirectory`; obtain qualified payload paths from existing cache, never installed INI paths. Record source revision, executable/runtime hashes, device, actual SR dispatches, FG callbacks, reader retirements, pixel/HUD observations and failures.

- [ ] Write assertions: `dlssDispatches > 0`, `fsrSrCreates == 0`, `fsrSrDispatches == 0`, `generatedCallbacks > 0`, `uiFailures == 0`, `retirementFailures == 0`. Require real DLAA/scaled DLSS input then actual NR then FSR FG. Cover FG off/on, NR off/on, three-pass/style reentry, stale/missing guides, menu inhibition, resize, suspend/resume and pending readers. A missing runtime or loss of required observation produces NOT QUALIFIED, never PASS.
- [ ] With Skyrim closed, run new serial GPU tests via CTest; expect the new qualification assertions to reveal any incomplete mixed-route behavior. If foreground observation is necessary, launch a visible fixture explicitly and request only the needed observation.
- [ ] Implement the fixture using production NGX/resource/presenter code; resolve any failures in owning production task/tests. Include an inactive FSR4 SR preference case and supported FSR3/FSR4 FG provider availability without claiming unavailable hardware passed. Do not use a synthetic FSR-only source as mixed-route evidence.
- [ ] Run the complete relevant configuration, overlay, source, interop, FSR SR/FG and NR suites once after final changes; build Universal plugin. Require all mandatory mixed-route GPU cases to pass. Native execution uses one fresh whole-change reviewer; address actionable findings and rerun only affected verification plus final build when changed. Summarize actual supported combinations and remaining hardware qualification limits.
- [ ] Commit code and validation evidence, push `wallhead codex/nr`; report **5 of 6 done** only after required GPU qualification and review pass.

### Task 6 / milestone 6: Matched archive and safe Skyrim handoff

**Files:** `package/README.md`; existing `tools/nr/Pack-Release.ps1` used as-is if possible; ignored packaging/install receipts and backups under `out`.

**Interfaces:** Existing packer accepts `-TemplateArchive`, `-PluginDll`, `-OutputArchive`, `-StagingDirectory` and requires the canonical archive basename. Pack to a fresh `out/packages/independent-fsr-fg-<revision>/RaZKolbaS DLSS FSR FG NR v1.2.zip`, with a separate fresh staging subdirectory, then copy the verified ZIP to `C:/Users/user/Downloads/RaZKolbaS DLSS FSR FG NR v1.2 - independent FSR FG test.zip`. Use metadata version 1.2 and generated matched INI; keep the validated original v1.2 ZIP intact. Installed mod remains its current v1.2 directory/profile entry.

- [ ] Package from clean committed Universal build using the original validated v1.2 archive as template. Preserve qualified runtime payloads and portable defaults; add Backend=Auto. Verify every extracted hash, 7z CRC, clean revision and metadata, and absence of music/extra root documents. Archive verification must reject mismatched DLL/INI or omitted FG runtime.
- [ ] Check Skyrim/MO2 processes. If either is open, finish staging and request closure once before modifying installed files. Back up working DLL, current INI and any changed resource; add the new default preference to the **current** user INI without replacing user choices or rewriting unrelated settings.
- [ ] Install only validated changed files; verify installed hashes and INI preservation against the backup. Keep MO2 launch/profile settings and mod priority unchanged.
- [ ] Record package/install receipts, push final documentation and report **6 of 6 prepared**. Clearly state Skyrim validation is still pending; do not claim the game route passed.
- [ ] Ask the user to start Skyrim and check DLAA/DLSS→NR→FSR FG then existing FSR→NR→FSR FG: FG/NR off/on, camera/HUD, menus, save/reload and alt-tab/minimize. Changing backend requires Save as default + restart. Inspect the resulting log and close the milestone as **6 of 6 done** only after that check passes.

## Plan self-review and execution handoff

Spec coverage maps to Tasks 1–4 (settings and ownership), Task 5 (actual GPU proof/review) and Task 6 (matched delivery/game boundary). The five review focus cases each have named regressions. Type/signature dependencies use the same source snapshot and resource owner throughout. Existing SR APIs remain intact; external mode is explicit and generation-only. No intermediate commit is installed.

Recommended execution: **Native** in this session, with one fresh whole-change reviewer before delivery. The six milestones share renderer/resource interfaces, so per-task agent contexts add coordination cost. User reviews this written plan and selects Native or Subagent-driven before implementation.
