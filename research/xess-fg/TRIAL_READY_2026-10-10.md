# Intel XeSS FG trial ready for Skyrim timing capture

This experimental package uses the clean plugin source revision `28cebeedf606`. It is installed and active in MO2 profile **V5.4 NO-LORE**. Skyrim gameplay timing, visual output and lifecycle are still unqualified. Progress remains **3 of 8 milestones complete**.

## Build and checks

- Full Release build succeeded with FSR SR/FG, XeSS SR/FG and NR enabled.
- CPU suite: 213 passed, one optional FsrMlGpu test skipped, zero failures.
- Seven focused GPU checks passed. Some checks use public SDK doubles over native GPU resources; they do not establish real SDK interpolation.
- Real Intel presentation probe: 33 eligible sources, 33 interpolated sources, 127 real-only sources, zero errors and clean retirement. It reported 117 lost-focus frames. This numerical result does not replace visual qualification.
- Actual XeSS Native/Performance and FSR 3.1.5 Native/Performance source matrices each generated 49 of 49 eligible sources, processed 24 Before and 24 After NR sources, passed seven opaque HUD readbacks, and retired cleanly without SDK errors. Actual DLSS/FSR4 coupling with Intel FG remains unrun.
- Six optional-feature configurations build. DLSS + Intel-only initially failed because a common FSR accessor referenced a disabled member; revision `2991ebe` guards that accessor, and the same configuration then built. Baseline without Intel, Intel without XeSS SR, Intel without AMD FG, Intel without NR, and all FG off also built.
- GPU debug layers were unavailable; no debug-layer qualification is claimed.

The earlier standalone visual confirmation applies to its recorded Task 6 build, not automatically to this new installed plugin.

## Installed configuration and rollback

Active mod: `RaZKolbaS XeSS FG trial - 28cebeedf606` under `D:/TESV54BETA/BETA_TRUEAE_V54/mods`.

Only three saved INI values changed from the previous working trial:

- `[FrameGeneration] Backend`: FSR to XeSS.
- `[FrameGeneration] Enabled`: true to false.
- `[NeuralRendering] Enabled`: true to false.

FSR Native, HDR off, inactive NR tuning and ImGui layout were preserved. The previous mod `RaZKolbaS XeSS FSR FG trial - 09816a0ccd16` remains unchanged and disabled. AIO and other renderer trials remain disabled. MO2 launch settings were verified unchanged.

Rollback files are under `out/backups/xess-fg-install-28cebeedf606`: original profile modlist, DLL and INI. Skyrim and MO2 were absent during installation.

## Immutable archive

File: `C:/Users/user/Downloads/RaZKolbaS XeSS FG trial - 28cebeedf606.zip`.

- Archive size: 352082048 bytes.
- Archive SHA256: `29e969a403f9aa8c5e056fc456ec919e80025cd7c45a0c1616c531a6a77d2896`.
- Plugin SHA256: `3e21610d30fa24f08d4d68d922e8a320d996b294501a9ce316b4512840454799`.
- Experimental metadata: `1.3.5-xess-fg-28cebeedf606`; DLL file version: `1.3.5.0`.

No published archive was replaced. The staging folder remains because automatic approval review blocked its deletion.

## Next required check

Start Skyrim through the usual MO2 entry, load a save and leave NR/FG off for about 30 seconds. Inspect the actual engine input, simulation, render and Present ordering before enabling Intel interpolation. Then qualify live FG controls, output graph, visual HUD preservation, provider combinations and gameplay lifecycle. First-trial success does not qualify other physical GPUs.
