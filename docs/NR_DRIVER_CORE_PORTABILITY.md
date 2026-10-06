# NVIDIA driver-core portability

Leave `[Runtime] NRDriverCore` blank to discover NGX from the active rendering adapter's installed driver. An explicit absolute path remains available for diagnostics.

Production no longer requires the SHA256 or byte count of the locally tested 617.14 `_nvngx.dll`. Different genuine driver releases have different bytes. The old gate rejected a tester's 616.64 driver before NR initialization.

Each candidate must still pass these checks:

- Its filename is `_nvngx.dll` or `nvngx.dll` and its absolute file is held against writes/deletion, with reparse points rejected.
- Its embedded version resource identifies NVIDIA Corporation and original filename `nvngx.dll`. Resource-only mapping does not execute the candidate or use external MUI metadata.
- Windows verifies its embedded Authenticode signature or installed catalog, signed by NVIDIA Corporation or Microsoft Windows Hardware Compatibility Publisher. Verification uses local cached trust data only.
- The loaded module matches the held file. Required NGX exports and the parameter-interface roundtrip must succeed before evaluation.

NR feature/model DLL sizes and hashes remain pinned. Driver-core SHA256 is still recorded in standalone probe receipts and trust-failure diagnostics, but does not select accepted driver versions.

The regression uses a copy of the installed NVIDIA core with a changed PE checksum: its complete-file SHA changes while its Windows catalog membership remains valid. Acceptance is checked without executing that copy. Changing executable bytes instead must fail trust verification. File ownership, wrong metadata, relative paths and missing files are also covered.

This removes the fixed-driver hash barrier; it does not establish execution compatibility on every NVIDIA driver or GPU. The tester must launch again and provide the new log if NR remains inactive. A DLL-only update is sufficient for an existing complete RaZkolbaS installation; no driver-core DLL should be bundled or copied from another PC.

Legacy relative `NRRuntimeRoot = TheosRenderPipeline` (and its subdirectories) now resolves existing sibling legacy resources first, then existing renamed RaZkolbaS resources. Existing targets, explicit absolute overrides and unrelated custom paths retain their meaning. The NR cache is already selected from the current RaZkolbaS resource directory, independently of that INI root.

Application-setting filtering recognizes a single named `GetProcAddress` import from Kernel32, KernelBase, or supported library-loader API contracts. Ambiguous or unsupported layouts still stop setup rather than silently allowing vendor overrides. Failure logs identify the module or configured path; a driver with a different layout still needs its actual log inspected. Smooth Motion guidance is separate from the rendering failure message and is not a diagnosis of that failure.

Round 6 validation on RTX4080SUPER / 617.14: full Standard build succeeded and 278 of 281 tests passed. `NativeUIComposition`, `NativeUIBlendState`, and `NeuralPeripheralPixels` failed before rendering because the Windows debug layer is unavailable (`DXGI_ERROR_SDK_COMPONENT_MISSING`, `0x887A002D`); ordinary WARP device creation succeeds. Both new regressions were observed failing before their fixes, and the real NGX/Streamline override-filter tests passed. Other driver/GPU combinations still require tester execution.
