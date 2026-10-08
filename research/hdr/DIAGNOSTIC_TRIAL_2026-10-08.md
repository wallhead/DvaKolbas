# ENB HDR diagnostic trial 1.3.4

This trial measures the current HDR curve; it does not change highlight expansion, source decoding, saved calibration or the installed mod. Delivery is a DLL-only tester update for the existing HDR package. The local mod installation remains untouched.

## Measurements

- Normal calibration logging identifies scene/UI composition, whole-frame expansion or UI-brightness fallback, effective brightness settings and the FG request.
- `[Debug] LogFrameDiagnostics = true` enables a 16 by 9 readback grid approximately every two seconds. Source SDR luminance is expressed at paper white; expected expanded world, actual HUDless PQ and actual output PQ luminance are reported separately. Input/output UI alpha helps identify foreground coverage.
- Readback uses the existing command-slot retirement fence. No additional GPU wait is introduced. Allocation/readback rejection disables diagnostic sampling while rendering continues.
- Sampled maxima are sparse-grid maxima, not the true full-frame peak or a physical panel measurement. Each report retains the calibration values used when its frame was recorded.
- With HDR and frame diagnostics enabled, the End menu offers temporary `HDR calibration patches`: top-left 100, 200, 500 and 1000-nit patches, left to right. These bypass highlight expansion, carry opaque foreground tags for FG and reset on restart. The production curve is unchanged outside the patch area.

## Validation

The full Release plugin and HDR unit/GPU targets build successfully after adding the explicit PerformanceTuning include to the overlay. Six focused CTest checks pass: OverlayLayout, OverlaySettingsCommit, SourceDLSSGSession, HDROutput, HDROutputPass and SourceNvidiaFrameEvaluation.

The new GPU checks cover deferred report availability, consumed-once reports, calibration snapshots across live changes, composed and encode-only routes, UI coverage, four known PQ patch values and nonfatal rejection of SDR sampling. These checks do not qualify the final Windows compositor or a physical HDR display. The tester's log and HDRScopes capture are the next required evidence.

## RenoDX comparison

Supplied `renodx-skyrimse.addon64`: 2,516,480 bytes; SHA-256 `247d5adf5e82a1eef2a8a7fe059b93619449fae7844cd902ec8d49e5118d34c4`. Inspected statically; not loaded. Its exact source revision is unproven.

The tester reports successful HDR without ENB. A public Skyrim implementation in [RenoDX PR 558](https://github.com/clshortfuse/renodx/pull/558), pinned to `cd2cfcc94a0e2c9d17bf8aee586fafcc3596bcac`, replaces a vanilla game's [tone-mapping shader](https://github.com/legoliamneeson/renodx/blob/cd2cfcc94a0e2c9d17bf8aee586fafcc3596bcac/src/games/skyrimse/tonemap_0x936CE1A3.ps_5_0.hlsl). It accesses colour/bloom/adaptation inputs, applies an HDR boost before HDR tone mapping and separately configures display peak. Its swapchain proxy then performs a SwapChainPass. The PR is closed and not merged; this does not prove the supplied binary matches that source.

This is useful evidence for separating boost from display peak. It is a different producer from our finished ENB SDR input, so the reported non-ENB result does not validate our ENB conversion or justify substituting its vanilla shader hook. No RenoDX code or binary is bundled into the trial.

## Next steps

1. Deliver measurement trial and obtain known-patch plus normal-scene captures.
2. Separate highlight boost from display peak using those measurements.
3. Improve the fixed ENB expansion curve without camera-dependent adaptation.
4. Qualify colour, UI and FG consistency in game.
