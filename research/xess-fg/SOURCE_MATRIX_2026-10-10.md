# Intel FG completed-source checkpoint

Baseline: `638c374246fd5257d94f41ddd01368b5c48e5505`. The accompanying receipt identifies the dirty source fixture and executable hashes. This is standalone numerical evidence, not Skyrim qualification.

Production `SourceFrameEvaluator` tests enforce input → optional Before NR → SR → optional After NR → FG preparation. Completed scene publication preserves the real source ID/epoch and measures guide extents on the retained D3D11 producer. Spatial recovery and repeated output cannot prepare fresh Intel generation. Changed NR/source epochs reset interpolation.

| Source | Contract/routing | Actual source + NR + Intel SDK |
|---|---|---|
| DLAA / scaled DLSS | Passed | Not run |
| FSR 3.1.5 Native / Performance | Passed | Passed on RTX 4080 SUPER |
| FSR4 Native / scaled | Shares FSR route | Not qualified by this probe |
| XeSS Native / Performance | Passed | Passed on RTX 4080 SUPER |

Each actual case ran 56 real sources: eight with NR off, 24 with Before NR, and 24 with After NR. Both placements exercised one, two, and three passes. All 49 eligible sources reported two presented frames and SDK success. Seven reset/transition sources stayed real-only. Seven opaque HUD pixels were checked in the actual native real-image publication. The probe does not expose private generated buffers or establish visual quality at these scales. Zero SDK errors; all owners retired cleanly.

The actual model was the configured RTX 40 NR profile. FSR's selected provider name was checked for 3.1.5; no ML fallback is reported as FSR4. Graphics Tools/debug layers were unavailable and are explicitly skipped.

The game host still restricts its first trial to Native, SDR, a fixed borderless extent, and 128 correctly ordered compatible world sources. Standalone scaled source evidence does not remove that first-trial restriction. Milestones 3, 5, 6, and 7 await gameplay qualification.
