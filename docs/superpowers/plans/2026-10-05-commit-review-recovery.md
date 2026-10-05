# Commit review recovery implementation plan

> **For agentic workers:** Use the parallel-agent workflow for independent NR recovery and configuration work. Follow regression-first tests and verify the integrated build.

**Goal:** Keep rendering after proven safe NR startup failures, preserve three-pass preferences across legacy runtime switches, and correct AMD compatibility claims.

**Architecture:** Propagate explicit cleanup proof through Stage and preparation wrappers; never infer safety from an empty handle or error category. Legacy execution uses at most two passes while the stored preference and third-pass overrides remain intact. AMD documentation stays on its separate experimental branch.

**Tech stack:** C++23, MSVC, D3D11/D3D12, CMake/CTest, Markdown.

**Spec:** User-authorized appropriate fixes after validation of `COMMIT_REVIEW_2026-10-05.md`.

## Constraints

- Preserve DLSS/FSR → NR → FG → UI, live FG preference behavior and runtime ownership checks.
- Never release uncertain vendor work, retry an uncertain teardown or broaden GPU qualification.
- Preserve installed INI and MO2 settings; installation requires a closed game.

## Review focus

- Failed first creation call returning no feature still retains ownership.
- Partial multipass creation and failed cleanup never recover as source passthrough.
- Device removal and changed shim remain fatal at retained-owner boundaries.
- Legacy two-pass execution must not erase the saved third-pass choice or overrides.
- Experimental AMD loading does not establish working NR output or RX 6000 support.

## Tasks

### 1. NR startup recovery

Files: `src/NeuralRendering/{Stage,BeforeUpscale,PreparedBeforeUpscale,PostUpscale,BeforeHost}.{h,cpp}`; initialization/preparation/host regressions.

- [x] Add failing Before/After full-wrapper and host tests for safe early failures, failed cleanup and vendor creation attempts.
- [x] Add a typed proof of successfully rolled-back initialization before vendor feature creation; propagate it without clearing uncertain ownership.
- [x] Disable NR for the session with a temporal reset only after health/ownership checks; retain all terminal guards.
- [x] Verify targeted fault tests and existing lifecycle/GPU chains.

### 2. Legacy runtime pass limit

Files: `RendererBackendPolicy.h`, `RendererSettings.h`, `SourceDLSSGNeuralStage.cpp`, `OverlayNeuralPanel.cpp`; backend/actions/pass-settings tests.

- [x] Reproduce startup and Apply failures with saved community PassCount=3 on legacy runtime.
- [x] Allow the saved preference; keep legacy execution capped at two and display/log the cap.
- [x] Verify save/reload retains PassCount=3 and independent third-pass overrides; unsupported FSR/backend contracts still reject.

### 3. AMD documentation and final validation

Files on separate AMD branch: `package/AMD-NR.md`; current branch validation receipt.

- [x] Qualify D3D11 warning as diagnostic until runtime rejection is demonstrated.
- [x] Remove unqualified RX 6000 support and pin published requirement limits.
- [x] Build the plugin and run configured noninteractive tests; report environmental exclusions accurately.
- [x] Review the integrated diff and record final results before committing or preparing installation.
