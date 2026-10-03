# Direct NR runtime checkpoint

This is research code, not an installed Skyrim feature. No runtime DLLs are committed. The three exact artifact pins and qualified local driver core are in [runtime-pin.json](../../../tools/nr/runtime-pin.json). `driver-id-evidence.json` independently maps all 65 catalog desktop IDs against the exact local driver INF. Eligibility does not imply tested hardware output.

## Reproduce locally

Configure the standalone test tree with Visual Studio 2022 x64 and explicit local files:

```powershell
cmake -S tests/nr-runtime -B out/build/nr-runtime -G 'Visual Studio 17 2022' -A x64 `
  -DTRP_NR_RTX40_DLL=C:/Users/user/Downloads/nvngx_dlssnr_4_series/nvngx_dlssnr.dll `
  -DTRP_NR_DRIVER_CORE=C:/Windows/System32/DriverStore/FileRepository/nv_dispi.inf_amd64_da865124972e1f80/_nvngx.dll `
  -DTRP_NR_NGX_INCLUDE_DIR=C:/Users/user/Documents/ChatGPT/DvaKolbas/.dependencies/streamline-sdk-v2.11.1/external/ngx-sdk/include
cmake --build out/build/nr-runtime --config Release
ctest --test-dir out/build/nr-runtime -C Release --output-on-failure
```

With Skyrim closed, invoke `tools/nr/Run-RuntimeProbe.ps1`. Its default mode tries the justified RTX 40 caller shim. Use `-CallerShim off` in a fresh process for the negative control; rejection with raw Init `0xBAD00002` is expected on this exact local stack. The native probe also refuses to run when Skyrim is present or process enumeration fails. Do not run GPU probes concurrently.

For product-tree builds set `VCPKG_VISUAL_STUDIO_PATH` to the VS 2022 BuildTools installation on every configure/build invocation, including automatic reconfiguration. Otherwise local vcpkg may select the installed VS 18 toolchain for dependencies.

## Observed results and scope

The reviewed shared owner at clean source `04fe79b5f735487b1bb38975889681b117aab9fe` produced 30 distinct **RGB** GPU output hashes on RTX 4080 SUPER `10de:2702`, standalone adapter LUID `66328:0`. All 6,912,000 measured RGB pixels were finite, all three channels overwritten, and changed versus input. All 30 frames contained spatial contrast. Resource callbacks balanced 4/4. Init/Create/Evaluate/Release/DestroyParameters/Shutdown each returned raw success 1. Output readers retired and the shim restored before module unload. The [positive receipt](rtx40-04fe79b-shimmed.json) records compiled source hashes and distinct RGB hashes; the [negative control](rtx40-04fe79b-unshimmed.json) records the unshimmed initialization rejection, without pretending teardown/output succeeded.

The tests cover missing exports, foreign device LUID, resident runtime conflict, outstanding clients, failed shutdown, changed shim ownership, held-file replacement and destructor no-retry. Uncertain ownership intentionally retains modules, files and the device until process exit rather than retrying vendor shutdown or releasing an uncertain recording.

This fixture uses static depth and zero motion with changing patterned color. It proves direct output and lifetime behavior, not moving-scene temporal quality, HUD preservation, game integration, performance or generated-frame guides. The exact current driver core is qualified only for this experiment. RTX 20/30/50 remain NOT RUN. The old game runtime owner and its recognized legacy profiles remain unchanged until the shared stage adapter is implemented.

## Shared Stage and native Before bridge

Clean source `da7b929751cdd7cdc5219cff6bd7c04ae798e902` passed the [240-frame shared Stage probe](rtx40-da7b929-stage.json): 240 distinct RGB hashes, 55,296,000 finite/all-RGB-written/changed pixels and exact variable source alpha, 4/4 callback allocations/releases and raw success 1 teardown. A second DIRECT queue performed a real output copy delayed behind a gate; neither ticket nor feature could retire until its completion fence advanced. The [unshimmed control](rtx40-da7b929-unshimmed.json) reproduced Init `0xBAD00002` in a fresh process, retaining uncertain partial ownership.

The [native Before bridge receipt](rtx40-da7b929-before.json) contains original/returned RGBA and evaluated RGB hashes. It ran 240 D3D11 → shared NR → D3D11 frames at 320×180: 232 one-pass evaluations with 232 distinct RGB hashes, eight disabled frames with all 460,800 pixels unchanged, and all 13,824,000 source-alpha pixels preserved. First-frame, re-enable and explicit history resets passed; actual copy-back readers, feature/parameters and callback resources retired before runtime shutdown.

The product-tree NR/legacy contract suite passed 21/21 without skips after the identity fixes. [Checkpoint review](STAGE_REVIEW.md) records the stale-ticket finding and its deterministic RED→GREEN regression. Stage and Before currently support native linear FP16 world color/R32 depth/RG16 motion. Game color preparation, reduced-size reconstruction and DLSS/FSR host call sites are still pending. No shared Stage or Before GPU result implies post-FG/generated-guide qualification, HUD/game visual acceptance or optimized performance.

After the reconstruction admission fix, clean source `68345f5` was rebuilt and both 240-frame probes passed again: [shared Stage](rtx40-68345f5-stage.json), [native Before](rtx40-68345f5-before.json). The receipts include exact compiled source hashes; non-direct reconstruction settings are now explicitly rejected until prepared by an adapter. The earlier negative control remains a control for the unchanged runtime-loader/shim path.

## Native SDR Before game trial checkpoint (1077266)

[Clean host receipt](rtx40-1077266-before-host.json): actual RTX 4080 SUPER, explicit Gamma22 UNORM source -> linear FP16 NR -> original source domain, typeless depth and raw RG16 motion. 240 real-source frames, 239 NR evaluations, live off/on, one resize, 8,640,000 exact source-alpha pixels, 8,582,279 changed RGB pixels, 57,600 unchanged bypass pixels and explicit reader/owner retirement. The JSON embeds hashes of compiled inputs and the clean build revision. Static depth/zero motion remain synthetic; Skyrim model-domain/color/temporal quality is pending.

The Standard NR-enabled and NR-disabled plugins compile. The product suite passed 158 checks with three Graphics Tools checks excluded; final targeted NR/integration/policy/settings regressions passed 30/30 without skips. A bounded integration review found and fixed continuous SR resets on unavailable NR, unchecked custom AMD resize retirement, missed camera cuts and missing per-frame HUD-less admission. The first local trial is native SDR Before only. True After FG and reduced-model paths remain unqualified; full NR progress is still 1/8.

## NR + FSR SR/FG + ReShade ownership checkpoint (7eb33a7)

The first fence fix reached one successful Skyrim NR evaluation, then FSR rejected its command/resource device. The [combined regression receipt](fsr-ownership-fix-7eb33a7.json) records this second boundary failure and the source-1 RED reproduction using the installed ReShade. NR now retains the presenter's D3D12 device and uses its own DIRECT queue, with adapter/removal admission and strict ownership checks retained.

Performance/SRGB and NativeAA/Gamma22 each pass 176 synthetic sources: 170 NR evaluations, six live/menu bypasses, temporal SR, real AMD generation callbacks, intact completed HUD/foreground, two resize cycles and two suspension/restoration cycles. The clean Standard NR/SR/FG plugin passes 160 runnable product checks; three unavailable Graphics Tools checks are explicitly excluded. Standalone NR passes 24/24 without skips. Standard NR-disabled compilation also passes. The corrected clean DLL is installed with the trial INIs and MO2 settings preserved. Skyrim loading, color/model suitability and moving-scene quality remain pending; no physical cadence, After FG or other-GPU acceptance is claimed. Full NR progress remains 1/8.

## Skyrim processing smoke and open motion defect

The [Skyrim smoke receipt](skyrim-before-smoke-7eb33a7.json) confirms completed save loading, active NR source processing, live off/on recovery, temporal FSR and successful generation records. The user then reported motion-dependent image changes while NR remained active. This is **not** visual acceptance. Zero weather/time presets loaded; the FSR NR path reads Base directly. [Controlled motion probes](../motion-probe/README.md) do not reproduce an on/off-like collapse with correct synthetic guides. The actual Skyrim source/FG comparison remains the next gate. No production change is justified by these probes alone.

## Local-tone color-shift mitigation

[Actual game color/motion capture](../motion-probe/skyrim-color-5b1eb7e.md) confirmed scene-dependent RGB changes inside NR while its settings stayed stable. Changing only the trial's `NRLocalTone` from 1 to 0 removed the reported effect according to the user. The subsequent log retained active NR, live off/on recovery, temporal FSR and FG generation without a rendering error. Keep Local tone 0 in this V5.4 trial; product defaults and the live control remain unchanged. This is a workaround that disables the implicated feature, not a root-cause fix. Model behavior versus color/exposure integration remains an open investigation, alongside full NR qualification and true After. Progress remains 1 of 8.
