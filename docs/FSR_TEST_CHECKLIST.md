# FSR SR gameplay acceptance checklist

Status: **NOT RUN in Skyrim.** Local staging is complete only after validation; staging does not install or alter MO2. The user starts Skyrim manually. Target: **TESV54BETA / V5.4, V5.4 NO-LORE**. Keep the working DLSS mod available for rollback and avoid concurrent upscalers.

## First launch

1. Close Skyrim and MO2 before installation is authorized. Back up the active profile and existing TRP mod. Install the staged FSR package as a separate mod only after explicit authorization; enable that mod in the agreed profile. Retain the user's MO2 settings and launch target.
2. Verify `UpscaleType=4`, `[FSR] Quality=Quality`, `ProviderPolicy=Analytical`, `Sharpness=0`, frame generation disabled, experimental backend 0, NR/HDR/dynamic resolution off. Native UI stays on. The package includes exactly the two pinned AMD DLLs in `SKSE/Plugins/FSR`.
3. Launch manually through the existing MO2 SKSE entry. Record the plugin/package hash, game version, GPU/driver, resolution, renderer/ENB configuration, log and startup outcome. Inspect the **Skyrim process** module inventory: no `sl.*`, `nvngx*`, `_nvngx*` or `nvapi*` runtime loaded. A fixture's plugin LoadLibrary success is separate evidence.
4. Reach a loaded world. Confirm the log reports the actual discovered provider, measured render/output extents, real source IDs, elapsed milliseconds, camera/depth flags/world scale and motion format/scales. Confirm a successful temporal dispatch and consumed native output; a requested setting, initialized context or overlay FPS is insufficient. Record any spatial recovery/error.

## Guides, image quality and native UI

- Capture the actual scene target formats, render-sized viewports, depth SRV and motion texture at the reconstruction boundary. Inspect motion for a stationary camera, panning camera, moving actor/object and camera/object motion together. Confirm current-to-previous UV units, width/height conversion, jitter cancellation, projection/depth sense and physical world scale against captures of the installed shader.
- Calibrate the explicit Gamma22 finished-SDR handoff against ENB-off and ENB-on scene captures, known dark/bright/color patches and UI alpha. Verify linear FSR input and native encoded output. Linear/sRGB helper tests are not proof of the game's encoding. Correct the selected transfer before accepting if captures disagree.
- Inspect foliage, particles, transparent materials, water, distant edges, specular motion and disocclusions. Reactive/transparency masks are currently unavailable; record ghosting/shimmer limitations. Confirm exactly one sharpening stage and compare 0 versus a moderate FSR sharpness.
- Check HUD/text, inventory and magic previews, cursor, StatsMenu, dialogue, loading art and fades at native output. Verify UI completion occurs after reconstruction and the next scene resumes without stale UI or history.
- Run with ReShade absent, before upscaling and after upscaling; confirm one effects execution at the selected position and restored rendering state. Test ENB on/off separately. With Community Shaders, confirm external ownership, no extra TRP FSR pass, ordinary output, and unavailable TRP NR/FG/Reflex controls.

## Settings and lifecycle

- Test Quality, Balanced, Performance and NativeAA. Mode/quality/provider edits stay requested until Save and restart; active allocation does not change mid-frame. Test Analytical and Compatible policies separately; report the **actual** provider, rather than assuming Compatible chooses a neural provider.
- Test normal, odd and ultrawide native resolutions. Confirm the queried render extent is authoritative. Test paused simulation, camera switch/cut, new game, save/load, fast travel, loading exit and consecutive menus. Accepted source IDs advance; valid monotonic timing survives pause. History resets once on re-entry, with no repeated normal-frame reset.
- Test alt-tab, minimize/restore, borderless/fullscreen behavior and resize. Confirm old buffers retire before release, new extents are published safely, and failed resize enters a clearly reported safe fault. Test runtime absent/wrong version in a separate controlled setup; no working-directory DLL fallback or accidental NVIDIA initialization.
- Confirm dispatch failure delivers clearly labeled spatial recovery until restart. Device/retirement failure must stop rendering and retain owners; do not count recovery as active temporal FSR.

## Measurements and acceptance

Record GPU reconstruction/transfer times, source and presentation counts separately, CPU frame times and VRAM over a repeatable route. Compare a GPU-bound scene and NativeAA versus reduced rendering. CPU-bound FPS and UI counters alone do not prove correctness or performance. Record source IDs/output captures, provider, settings and errors alongside results.

Local real-GPU fixture: 1,000 temporal frames / 25 contexts, native pixel readbacks, UI sentinels, outstanding work, loading/invalid re-entry and bounded allocation counts. Fresh-process fixture: built plugin import load plus separately invoked production ordinary/FSR startup helpers and 12 changing native pixel readbacks. Both are **separate from Skyrim SKSE startup/gameplay**.

Graphics Tools-dependent debug checks: SKIPPED, installation failed with error 5; do not retry. AMD and Intel hardware: NOT RUN. FSR frame generation: NOT IMPLEMENTED / NOT RUN. Gameplay acceptance remains open until the checklist's captured results are recorded.
