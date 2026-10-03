# Bounded post-P4 fixes — 2026-10-04

Three fixes from the [validated review](POST_P4_REVIEW_VALIDATION.md) are implemented and qualified at compiled source `bc2ef9d4ed060eacc15e56ce6bec909a4a64a23e`. The [machine receipt](post-p4-fixes-qualification.json) records source identities, evidence hashes, timing summaries and the separately staged package. Gameplay performance acceptance remains pending.

## Changes and regression evidence

1. **One producer queue wait in Before NR.** A private Stage-producer interop entry preserves allocator retirement and recording admission while leaving the dependency wait to `Stage::RecordQueued`. Ordinary interop/FSR Begin behavior is unchanged. The whole Before fixture first failed with two waits, then verified exactly one wait on each of 232 evaluated frames and zero on eight bypass frames. An injected wait failure records no vendor evaluation and retains terminal owners.
2. **Actual NR-to-FSR route diagnostics.** Startup/change-only logging reports the actual prepared or encoded route, ReShade order, diagnostic capture, model SHA-256 and work extent. Stable frames remain silent. The route policy covers contradictory requested settings versus the actual lease and reset/reentry.
3. **Consumed prepared-input access is cleared.** Successful reader registration clears all public COM resources and frame identity; failure preserves ownership. Precision fixtures schedule their independent reference reads before consumption and protect those reads with the genuine FSR copy/draw reader, retaining the existing image and alpha comparisons.

The combined product configure exposed a duplicate route-test target; nested registration now reuses the root target. Fresh independent review found no Critical or Important issues. One Minor remains: the route log's raw NR status can precede resolution of a menu/camera/unavailable bypass reason; the adjacent frame log contains that resolved reason. Resource pool topology, retained slots and bridge/Prepared lifetime edges are unchanged.

## Verification

| Clean build configuration | Selected checks passed |
|---|---:|
| Standard, NR enabled | 177 / 177 |
| Standard, NR disabled | 157 / 157 |
| Universal, NR enabled | 177 / 177 |
| Universal, NR disabled | 157 / 157 |
| Full standalone NR suite | 49 / 49 |
| **Total** | **717 / 717** |

No selected checks were skipped. The three existing Graphics Tools exclusions remain `NativeUIComposition`, `NativeUIBlendState` and `NeuralPeripheralPixels`. All four product identities match clean source `bc2ef9d4ed06`, and all four optional-vendor import inspections pass. An earlier GPU attempt stopped at the game-running guard; it is retained as an interrupted environmental run and superseded by the complete clean suite.

The separate native-resolution correctness capture produced 35 NR/direct deliveries and 36 distinct FSR output hashes, preserved all **132,710,400** expected source-alpha pixels, dropped no GPU timing records, retired successfully, shut down once and restored the shim.

## Synthetic timing limits

Each timing case has 300 measured sources after 120 warmup sources at 2560×1440, FSR 3.1.5 Native AA, NR tone/style 0 and the normal Windows timer (`timerPeriodMs=0`). Cases ran sequentially with Skyrim closed. The receipt preserves model/runtime hashes, exact configuration and raw capture hashes; all seven comparison cases have zero validation issues.

| Case | Transaction median, ms | p95, ms | p99, ms | Vendor NR GPU median, ms |
|---|---:|---:|---:|---:|
| NR on, repeat 1 | 9.2562 | 9.9212 | 10.2737 | 7.4757 |
| NR on, repeat 2 | 9.1547 | 9.9046 | 10.1665 | 7.4184 |
| NR on, repeat 3 | 9.2306 | 10.1466 | 10.4585 | 7.5213 |
| NR off, repeat 1 | 1.0195 | 1.3000 | 1.4261 | — |
| NR off, repeat 2 | 1.0159 | 1.2953 | 1.4145 | — |
| NR off, repeat 3 | 1.0210 | 1.2790 | 1.3628 | — |
| NR on, instrumentation off | 9.0289 | 9.6307 | 9.9377 | Unmeasured |

These NR-on medians are higher than historical clean P5 transaction medians of 8.525–8.561 ms, and the vendor interval also rose from 6.920–6.955 ms. There was no interleaved old/new control or GPU clock/thermal control, so these captures establish neither an isolated gain nor an isolated regression. The verified optimization removes one redundant queue wait. They do not establish recovered Skyrim FPS or AIO parity, and overlapping phase waits must not be added as recoverable time. NR-on steady counters remain four Flushes and zero descriptor creations per source.

The unchanged three-slot bridge/Prepared texture payload estimate is 421.875 MiB at 1440p. Actual D3D12 allocation delta remains unmeasured.

## Package and remaining gates

`out/package/nr-before-post-p4-qualified-bc2ef9d` is validated and **not installed**. Its Standard DLL SHA-256 is `b4c3b357ee646f8d0eaf65404e045163e46d0258f85f5a7a492fd740afce53e7`. Staging regenerated the NR section from the prior qualified FSR Native AA/tone-0 reference and checked the pinned models, driver core, AMD assets, manifest and import contract.

The installed working candidate and MO2 settings were preserved. The current user INI selects DLAA, NVIDIA FG backend, NR Before and tone 1.018; these settings were not overwritten by the FSR staging reference. An incomplete rejected staging directory is not the qualified package above.

This bounded fix pass is **3 of 3 complete**. Original NR milestones remain **1 of 8**. Matched three-repeat Skyrim performance, visual/FG/lifecycle acceptance, automatic benchmark metadata enrichment, and ownership-qualified pool/lifetime optimization remain open. Historical acceptance receipts still describe the earlier installed candidate, not this staged build.

Subsequently, the user requested the next step and closed Skyrim/MO2. [Installation follow-up](POST_P4_INSTALLATION.md) records the qualified DLL update and exact AIO19-to-trial activation while preserving the current DLAA INI. The staged-package and unchanged-installation fields above describe the earlier qualification action; they are not the later installation receipt.
