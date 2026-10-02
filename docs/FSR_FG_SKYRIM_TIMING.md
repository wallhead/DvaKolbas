# Skyrim FG timing capture — 2026-10-03

Installed clean build `be05d936e477` reached the world in V5.4 NO-LORE with NativeAA at 2560×1440. The user reported completing FG on/off/on/off. The log confirms those four requested states, successful generation in both on phases, zero logged FG API/callback failures and zero error/critical entries. The two optional KreatE/OAR adapter hash warnings remain unchanged. Generation-state entries are transitions, not generated-frame totals.

| Phase | Selected rolling summaries | Base cadence p50, ms | Base cadence p95, ms | Pre-Present D3D11 p50, ms |
| --- | ---: | ---: | ---: | ---: |
| On | 4 | 17.207 | 22.642 | 12.322 |
| Off | 4 | 17.188 | 20.999 | 11.374 |
| On | 1 | 17.204 | 21.025 | 11.878 |
| Off | 4 | 17.290 | 21.897 | 11.543 |

Values are medians of logged rolling summaries. Selection uses the first 30 seconds of each requested-state phase, clipped at the next toggle; it discards the first 20 seconds and final two seconds. The 20-second settling period is a heuristic for the 1,024-frame windows at about 58 base FPS. It does not prove exact sample-window boundaries. The second on phase lasted 23.871 seconds, leaving only one selected summary. Overlapping windows are correlated; the log cannot verify an unchanged camera position or closed overlay. The final off analysis ends before the shutdown-related timing drop.

Base cadence stayed similar across the selected phases. These measurements do not establish displayed generated-frame cadence, latency or isolated FG GPU cost: D3D11 frame timing ends before Present. Independent presentation evidence, formal ENB/source-color calibration and automatic Alt+Enter remain pending. Hardware and Graphics Tools limitations retain their previous scope. Seven of eight milestones are complete.

With Skyrim and MO2 closed, normal logging was restored and the installed package validated again. The main INI returned exactly to its original SHA256 `2dd464f0cf0e6859b03f4dd6cd0d2ed0995d57730b5e4b9c0cb0bcbd516e4984`; NativeAA and startup FG remain enabled. [Machine-readable evidence](FSR_FG_SKYRIM_TIMING.json) records snapshot hashes, selection, results and restoration backup.
