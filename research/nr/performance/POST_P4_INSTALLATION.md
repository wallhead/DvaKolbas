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
