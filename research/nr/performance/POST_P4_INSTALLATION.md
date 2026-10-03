# Post-P4 trial installation — 2026-10-04

The qualified Standard build at compiled source `bc2ef9d4ed06` is installed in the existing **TRP NR Before - DvaKolbas trial** mod for **V5.4 NO-LORE**. [Qualification](POST_P4_FIXES.md) records the three source fixes and 717 passing selected checks. This installation is a preparation step; gameplay acceptance of the new source is pending.

The profile initially had AIO19 enabled and the Dva trial disabled. After the user closed Skyrim and MO2, installation replaced only the trial DLL and manifest, enabled the existing trial entry and disabled AIO19. Exact comparison against the backed-up modlist confirms only those two entry prefixes changed. MO2 launch settings, other profile files, the complete trial INI/ImGui settings, all runtime assets and the accepted rollback mods were preserved. All 105 protected file hashes match the snapshot taken after MO2 saved its settings on exit.

The [installation receipt](post-p4-installation.json) records installed identities/hashes, protected files and the local rollback directory. The prepared package passed the existing strict FSR Native AA validator. The installed INI is the user's current configuration, so installation separately verified the complete installed manifest, pinned immutable assets, embedded identity and imports; it does not claim that the current DLAA selectors satisfy the FSR staging validator.

Current saved selectors, preserved byte for byte:

| Setting | Value |
|---|---|
| Upscaler | DLAA (`UpscaleType=3`) |
| FG enabled | false |
| FG backend | NVIDIA (`FrameGenerationBackend=1`) |
| Community NR | enabled, one Before pass |
| Local tone | 1.018 |
| Intensity | 1.043 |

The tone-0 mitigation was not reapplied. Camera-dependent color behavior at a nonzero tone remains an open issue; this smoke test must not be described as its root-cause fix. The FSR reference INI was not copied over these user settings.

Next, start Skyrim through the normal MO2 entry and load the same save. Leave FG off for the initial smoke check. Inspect the End overlay and camera/HUD image with NR off (`[`) and on (`]`), then repeat off/on once. Check for loading-screen retention, black output, a crash, color instability or failed toggle recovery. Inspect the new source identity and NR records in the game log after the user launch. This current-settings smoke is separate from the later matched three-repeat source-performance/FSR comparison and independent FG cadence/lifecycle checks.

No Skyrim launch or GPU fixture was performed during installation. Original NR progress remains **1 of 8** and P5 gameplay acceptance remains pending. The earlier qualification and acceptance receipts remain historical records of their respective staged/installed candidates.

## Failed launch and runtime repair

The next user launch failed before NR initialization with `load interposer failed HRESULT=0x8007007E`. The log also records a failed `nvngx_dlss.dll` preload. The installed FSR-only asset set lacked all six configured Streamline modules and the DLSS SR DLL, although the preserved user INI selected DLAA. The preceding installation was incomplete for that selected provider; its FSR qualification and immutable-file checks did not validate this dependency boundary.

The missing seven DLLs and Streamline license were added from the previously working DLSS baseline, after verifying each file against that baseline's historical manifest. Existing trial files, user INI, renderer DLL and MO2/profile settings were preserved. The trial manifest was regenerated and verified. A standalone Windows loader check now loads and releases the interposer and DLSS SR with the renderer's `LoadLibraryExW` search flags; it calls no SDK/GPU initialization. The [repair receipt](post-p4-runtime-repair.json) records the failed-log hash, missing-file evidence, exact added-file hashes and successful loader check.

[NVIDIA asset preflight](../../../tools/nr/Validate-NvidiaTrialAssets.ps1) now checks the current DLSS/DLAA-selected trial's required runtime files and license against its manifest. Run it before activating a trial that preserves NVIDIA selectors; the strict FSR staging validator alone is insufficient. MO2 must be restarted to refresh the virtual file mapping after this asset addition. Skyrim relaunch and visual/toggle acceptance remain pending.
