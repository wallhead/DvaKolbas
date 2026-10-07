# FSR 3.1.5 and FSR4 choice implementation plan

> **For agentic workers:** Use superpowers:executing-plans to implement this plan inline. Preserve the existing production worktree and finish with a focused review.

**Goal:** Keep FSR 3.1.5 available and add the locally tested FSR4 INT8 runtime as an explicit NVIDIA choice.

**Architecture:** Select one SR runtime profile at startup from the saved policy and actual adapter vendor. Official SDK 2.3.0 retains FSR 3.1.5 and AMD FSR4; the separately pinned 4.0.2b INT8 pair uses its verified 4.0.3 API contract. Both feed the same production frame adapter and existing independent analytical FG presenter.

**Tech Stack:** C++23, D3D11/D3D12 interop, FidelityFX C API, Windows CNG, CMake/CTest, PowerShell.

**Spec:** User requests selectable FSR 3.1.5 and FSR4, NVIDIA testing first; research/fsr4/nvidia-int8/qualification.json records initial GPU evidence.

## Global constraints

- Preserve DLSS/FSR → NR → FG → UI.
- Analytical stays the official 3.1.5 runtime; no substitution with AIO's older analytical implementation.
- Runtime/profile changes require save and restart; no cross-runtime hot replacement.
- Explicit NVIDIA FSR4 loads only the hash-pinned INT8 pair, requires SM6.6 and verifies actual provider identity.
- Auto retains its current official-runtime behavior on NVIDIA. Explicit FSR4 never silently falls back to FSR3.
- Leave stable v1.1 archive, installed INIs, MO2 settings and launch configuration unchanged while preparing the candidate.

## Review focus

- Mismatched runtime/API profiles fail before game targets are published.
- Corrupt or replaced INT8 binaries fail before execution; leases remain while modules are owned.
- Context recreation retains selected runtime; no legacy analytical retry through the INT8 loader.
- INT8 SR can coexist with the pinned official FG runtime and preserve output/UI ownership.
- Private model allocations are not observed by SDK callbacks; record bounded recreation memory separately and do not claim zero private allocations.

### Task 1: Runtime selection and contracts

Files: src/Upscaling/FSRRuntimeProfile.h, FSRRuntime.h/.cpp, FSRProviderPolicy.h, FSRUpscaler.cpp, FSRHostResources.cpp; tests/FSRRuntimeTests.cpp.

- [x] Add failing profile-choice and cross-vendor tests.
- [x] Add explicit profiles, pinned file leases, profile-specific API versions and provider admission.
- [x] Run runtime, settings and backend admission tests; verify both choices remain.

### Task 2: Production GPU integration

Files: tests/fsr-gpu/FSRGpuSmoke.cpp, tests/fsr-fg/FSRGenerationGpuSmoke.cpp, tools/fsr/int8-runtime-pin.json.

- [x] Exercise unmodified production runtime/upscaler with INT8, 1000 frames, recreation and sharpness.
- [x] Exercise pinned official FG through the selected INT8 loader, including menu suppression and output readers.
- [x] Record remaining model-memory limitations and correct any integration failures.

### Task 3: Candidate handoff

Files: src/OverlayImagePanel.cpp, package INI documentation, tools/fsr candidate staging, research/fsr4 receipts.

- [x] Keep explicit FSR3 and FSR4 menu/INI options and explain restart.
- [x] Build and stage a separate NVIDIA FSR4 candidate containing both runtime paths; verify payload hashes and portable metadata.
- [x] Finish focused review; hand off only when Skyrim start is needed. Keep AMD FSR4 and Skyrim acceptance open until actually tested.

## Verification ledger

- Initial profile tests failed to compile because profile selection did not exist; implemented and passed.
- Runtime tests reject wrong size and a right-sized wrong SHA256 before module execution.
- Rebuilt selected regression suite: 27 passed, 1 skipped (official AMD ML unavailable on local NVIDIA), 0 failures.
- Production INT8: 1600 frames/32 recreations, 128-frame sharpening comparison, 720 SR+FG source frames, fresh-process host startup with 12 changing temporal frames.
- Leases reject concurrent write opens; contexts and DLL modules unload after host retirement. Private model allocations are uninstrumented; retired-memory range over32 recreations was7MiB, not a zero-allocation claim.
- Focused independent review found no critical/important issue; per-effect diagnostic reporting was corrected.
- Ruling: NVIDIA Auto stays on the official runtime; explicit FSR4 selects INT8. This avoids a cross-runtime fallback through the unqualified older analytical resource-query path.

- Candidate archive: RaZKolbaS FSR3-FSR4 NVIDIA preview.zip, SHA256 b0c28c79906476c7bc4eecd9dd7659888342325510a3ae8989b23f087d8e0fb6. Clean Universal build a2147d030716; every archived file verified, both runtime folders retained, Native AA/explicit FSR4, NR and FG initially off. Not installed.
