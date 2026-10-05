# NR branch review fixes — 2026-10-05

Baseline: `256e3870d39b`. Review source: the supplied “DvaKolbas Code Review — codex/nr branch”, originally targeting `5097338`.

## Changes

- **13:** distinguish rejected NR initialization from other quarantine reasons. When the retained device is healthy, the shim is still owned and no NR feature/frame work began, disable NR for the session and continue source rendering. Keep the partial owner for process lifetime; never retry initialization or attempt uncertain shutdown. Recheck device/shim safety on subsequent evaluations and retirement.
- **5:** unwind allocator/client/parameter ownership on explicit failures before entering vendor feature creation. Parameter destruction failure, unbalanced callbacks and all failures after entering vendor creation retain uncertain ownership.
- **3–4:** presenter selection changes the settings draft without disabling current FG. Apply sends an interpolation request only to the actual startup presenter. Persist the next-launch request separately when changing presenters. Preserve provider choices across menu round trips and retain intentional live checkbox changes.
- **7:** missing FG Enabled defaults off for the ordinary presenter, with matching startup and validation behavior. Explicit contradictory requests remain rejected; color encoding is still explicit.
- **16:** expose an explicit community-NR repair action for unsupported one-pass/native-SDR/UI-correction settings. Preserve visual tuning, NR enabled and Stable colors choices. Correct the shared RTX 40/50 runtime description. Weather/time preset integration is not added.
- **8–9:** report the requested presentation backend and interpolation request accurately; avoid resetting SR history for After NR tuning changes, while preserving placement-transition resets and NR/FG resets.
- **11–12:** remove the overwritten shared sharpness assignment and clarify the independent FSR slider. Sample a bounded central region for color/motion diagnostics, including ultrawide sources; log the sampled origin/extent and keep motion scaling in full-source pixels.
- **18:** retain the FSR runtime, device and bridge across ordinary resize after all readers retire. Recreate sized allocations/context, preserving full teardown and failed-retirement ownership rules.

The pipeline remains **DLSS/FSR → NR → FG → UI**. Live NR settings still apply through the existing Apply behavior. Runtime/driver pins, GPU classification and hardware qualification are unchanged.

## Deliberately retained limits

- **1:** normal intercepted Skyrim startup already displays the rejected CS + FSR FG error; that ownership combination remains unsupported.
- **2, 6:** recording, device and uncertain-reader failures remain terminal. Generic fallback cannot safely replace proof of GPU retirement.
- **10:** automatic replay of a deferred minimized resize was investigated and withdrawn. Reallocating buffers after returning success could invalidate the game's retained buffer references; restoration requires a separate rebinding design.
- **14–15, 17:** GPU/driver/provider qualification is not broadened. A signature or same-major version alone does not prove the private runtime interface compatible.

## Verification

The new settings, initialization and resize regressions reproduced the original failures before their fixes. The Release plugin and nine focused checks passed. The FSR startup fixture additionally exercises three real D3D resize/recreation cycles and checks retained runtime/device/bridge identity and output extents.

All **223 selected noninteractive checks passed**, including 57 GPU and six ReShade checks, with no failures or skips. Interactive checks and the existing NativeUIComposition, NativeUIBlendState and NeuralPeripheralPixels exclusions were not counted as passes. Logs and JUnit results are local artifacts under `out/research/nr-review-fixes-*`; the clean build identity is recorded in the staged update receipt.

Independent source review reported no P1/P2 findings. Initialization fault injection does not exercise a real installed shim or all production Open file leases; the existing runtime-owner GPU checks cover their normal ownership paths. Actual Skyrim buffer reconstruction still requires a new launch. These checks qualify no additional GPU family or driver build.

The installed trial INI, MO2 profile/launch settings and runtime payloads are preserved. Back up the existing DLL, INI and manifest before replacing only the plugin and its manifest.
