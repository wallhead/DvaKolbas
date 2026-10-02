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

The shared owner produced 30 distinct GPU output hashes on RTX 4080 SUPER `10de:2702`, standalone adapter LUID `66328:0`. All 6,912,000 measured RGB pixels were finite and overwritten, with changed pixels versus input. Resource callbacks balanced 4/4. Init/Create/Evaluate/Release/DestroyParameters/Shutdown each returned raw success 1. Output readers retired and the shim restored before module unload. A later strengthened run additionally requires spatial contrast in every frame; its receipt is saved separately with compiled source hashes.

The tests cover missing exports, foreign device LUID, resident runtime conflict, outstanding clients, failed shutdown, changed shim ownership, held-file replacement and destructor no-retry. Uncertain ownership intentionally retains modules, files and the device until process exit rather than retrying vendor shutdown or releasing an uncertain recording.

This fixture uses static depth and zero motion with changing patterned color. It proves direct output and lifetime behavior, not moving-scene temporal quality, HUD preservation, game integration, performance or generated-frame guides. The exact current driver core is qualified only for this experiment. RTX 20/30/50 remain NOT RUN. The old game runtime owner and its recognized legacy profiles remain unchanged until the shared stage adapter is implemented.
