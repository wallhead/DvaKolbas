# Independent FSR FG qualification, 2026-10-08

The new mixed route uses production NGX DLSS/DLAA reconstruction, community NR After, and the official FSR FG presenter. After ordering is **DLSS/FSR -> NR -> FG -> UI**. Selecting Backend is a saved startup choice; Enabled remains live.

## Hardware results

Local hardware: NVIDIA vendor 10DE / device 2702 (RTX 4080 SUPER). The serialized actual-GPU Native, Quality and Performance cases passed. Each case reconstructed 160 real sources with NGX, enhanced 152 sources with NR, exercised one and three NR passes, then generated and read back changing interpolated images. Each created/dispatched zero FSR SR contexts, even with an inactive ML FSR SR preference. Each passed premultiplied, opaque and transparent HUD samples, enhanced real-source color conversion, duplicate/missing/malformed guide suppression, non-temporal source suppression, off/on/menu/style reset reentry, resize and two gated real guide-reader retirements during suspend/resume.

Raw executable/runtime identities and counts are in [the receipts](independent-fsr-fg/native.json), [Quality](independent-fsr-fg/quality.json), [Performance](independent-fsr-fg/performance.json) and the [negative control](independent-fsr-fg/negative-control.json). Receipts record base revision eaf3187 plus the exact built executable hash; the review fixes were uncommitted at execution. Each positive case also covers invalid guides before NGX/NR admission, spatial real-frame recovery, and 14 reset reentries. The negative control ran genuine NGX and FG while omitting NR and correctly failed with `no actual NR enhancement observed`. It cannot qualify the mixed route merely by producing frames.

The selected FG algorithm was **3.1.6**, from the official 4.0.1 API module. FSR4 ML FG was explicitly unavailable in this NVIDIA device's discovered FG catalog; this is a provider availability observation, not an AMD hardware pass. The current FSR SR provider preference did not load its module in the mixed route.

## Scope and reproducibility

`tools/fg/Run-Dlss-Fsr-FG.ps1 -BuildDirectory <configured Universal build> -OutputDirectory <fresh directory>` reads payloads from the build cache, verifies report/executable identity and all mandatory counters, and exercises the negative control. Skyrim must be closed. CTest cases: `DlssFsrGenerationGpu-Native`, `DlssFsrGenerationGpu-Quality`, `DlssFsrGenerationGpu-Performance`.

The callback observer uses the public SDK Present callback and its pinned premultiplied composition formula. Its pixel readbacks qualify that observation mode; they do not measure monitor cadence or prove the automatic compositor's appearance in Skyrim. Windows graphics debug layers were unavailable. The prior FSR-only automatic/composition and lifecycle regressions remain separate checks. No new AMD or RTX 20/30/50 hardware qualification is claimed.

## Delivery gate

The whole-change review found five issues, now addressed: invalid-guide recovery before vendor evaluation; shared camera propagation on the existing FSR SR route; preservation of NR world/UI exclusion gates; observer runtime lifetime through retirement; and client-window extent normalization before NGX sizing. Targeted regressions passed. The real-GPU fixture also checks the conservative suppression of NR's first reset source after a guide gap before FG resumes.

The broad run selected 216 tests: 214 passed, the official FSR ML GPU case skipped, and one obsolete portable-package assertion failed. That assertion and its route validator were updated to the current independent backend contract; the affected 10 configuration/package tests then all passed. Final coverage is 215 passing tests and one hardware/runtime skip. Existing FSR-only automatic presentation, NR Before/After, scaled sources, three-pass/style changes, reader errors, UI pixels/state, sharpening and suspension/resize lifecycle checks remain passing. Graphics debug-layer validation is unavailable on this host.

Skyrim gameplay qualification is pending. Check DLAA/DLSS with FSR FG, then the existing FSR SR + FSR FG route; exercise NR/FG live toggles, camera/HUD, inventory/map, save/reload and alt-tab/minimize. Backend changes require Save as default and restart. User INI choices, MO2 profile and launch settings must be preserved during installation.

## Prepared delivery

Clean Universal build **9705ba2f4488**, version 1.2.0.0, was packed into the separate `RaZKolbaS DLSS FSR FG NR v1.2 - independent FSR FG test.zip`. Archive SHA256: `b7699a94b76f136984a8722fc47ccc10e7894d2b7991be82729b63a5bfb89443`. All 19 entries passed CRC and content verification; runtime payloads match the original v1.2 archive, which remains unchanged. Portable defaults are Backend Auto, DLSS Native and FSR Native.

The V5.4 NO-LORE v1.2 mod received the verified DLL, current metadata and its existing INI with Backend Auto added. All 112 prior setting values, the mod-list hash and MO2 launch settings were preserved and a rollback backup retained. Auto preserves the current NVIDIA presenter with DLSS. To exercise the new route, select **End → Frame generation → Backend FSR → Save as default**, then restart. This is **6 of 6 prepared**, with Skyrim gameplay still pending.
