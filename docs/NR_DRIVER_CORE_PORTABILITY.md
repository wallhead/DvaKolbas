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
