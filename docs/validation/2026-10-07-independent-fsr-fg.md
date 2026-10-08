# Independent FSR FG qualification, 2026-10-08

The new mixed route uses production NGX DLSS/DLAA reconstruction, community NR After, and the official FSR FG presenter. After ordering is **DLSS/FSR -> NR -> FG -> UI**. Selecting Backend is a saved startup choice; Enabled remains live.

## Hardware results

Local hardware: NVIDIA vendor 10DE / device 2702 (RTX 4080 SUPER). The serialized actual-GPU Native, Quality and Performance cases passed. Each case reconstructed 160 real sources with NGX, enhanced 152 sources with NR, exercised one and three NR passes, then generated and read back changing interpolated images. Each created/dispatched zero FSR SR contexts, even with an inactive ML FSR SR preference. Each passed premultiplied, opaque and transparent HUD samples, enhanced real-source color conversion, duplicate/missing/malformed guide suppression, non-temporal source suppression, off/on/menu/style reset reentry, resize and two gated real guide-reader retirements during suspend/resume.

Raw executable/runtime identities and counts are in [the receipts](independent-fsr-fg/native.json), [Quality](independent-fsr-fg/quality.json), [Performance](independent-fsr-fg/performance.json) and the [negative control](independent-fsr-fg/negative-control.json). Receipts record the production base revision plus the exact built executable hash; the fixture source was still uncommitted at execution. The negative control ran genuine NGX and FG while omitting NR and correctly failed with `no actual NR enhancement observed`. It cannot qualify the mixed route merely by producing frames.

The selected FG algorithm was **3.1.6**, from the official 4.0.1 API module. FSR4 ML FG was explicitly unavailable in this NVIDIA device's discovered FG catalog; this is a provider availability observation, not an AMD hardware pass. The current FSR SR provider preference did not load its module in the mixed route.

## Scope and reproducibility

`tools/fg/Run-Dlss-Fsr-FG.ps1 -BuildDirectory <configured Universal build> -OutputDirectory <fresh directory>` reads payloads from the build cache, verifies report/executable identity and all mandatory counters, and exercises the negative control. Skyrim must be closed. CTest cases: `DlssFsrGenerationGpu-Native`, `DlssFsrGenerationGpu-Quality`, `DlssFsrGenerationGpu-Performance`.

The callback observer uses the public SDK Present callback and its pinned premultiplied composition formula. Its pixel readbacks qualify that observation mode; they do not measure monitor cadence or prove the automatic compositor's appearance in Skyrim. Windows graphics debug layers were unavailable. The prior FSR-only automatic/composition and lifecycle regressions remain separate checks. No new AMD or RTX 20/30/50 hardware qualification is claimed.

## Delivery gate

Skyrim gameplay qualification is pending. Check DLAA/DLSS with FSR FG, then the existing FSR SR + FSR FG route; exercise NR/FG live toggles, camera/HUD, inventory/map, save/reload and alt-tab/minimize. Backend changes require Save as default and restart. User INI choices, MO2 profile and launch settings must be preserved during installation.
