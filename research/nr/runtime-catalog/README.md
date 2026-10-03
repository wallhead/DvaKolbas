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
