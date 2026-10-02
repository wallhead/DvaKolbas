# Skyrim FG presentation result — 2026-10-03

**Current closure (2026-10-03): 8/8 implementation milestones complete for the user-accepted V5.4 NO-LORE borderless-windowed trial.** The user confirmed color appearance. Formal color calibration and wider hardware/optical qualification remain unperformed, not passed. See [acceptance closure](FSR_FG_ACCEPTANCE.md); older milestone counts below are historical checkpoints.

**PASS within ETW display-update scope:** PresentMon 2.6.0 observed 58.35 updates/s with FG off and 115.12 updates/s with FG on, a 1.973× ratio. Both files belong to Skyrim PID 3152 and the same native DXGI swapchain `0x28BED6D1310`, in Hardware Independent Flip mode. Installed Standard/NR-off clean code is `be05d936e477`, using NativeAA at 2560×1440; the game reports a 165 Hz display.

| Metric | FG off | FG on |
| --- | ---: | ---: |
| Raw CSV records | 1,748 | 3,454 |
| Selected records after trimming 2 s per end | 1,515 | 2,989 |
| Mean display-change interval, ms | 17.138 | 8.687 |
| Display-update rate, Hz | 58.350 | 115.121 |
| Display-change p50 / p95 / p99, ms | 16.712 / 21.051 / 25.049 | 9.329 / 10.796 / 11.667 |
| Selected NA/nonpositive display intervals | 0 | 0 |

The rate uses the mean positive `MsBetweenDisplayChange`, rather than CSV row count or callback count. Percentiles use linear interpolation over individual selected intervals. Both capture stderr logs are empty apart from their encoding marker; stdout records start and stop. Raw durations are 29.958 and 29.969 seconds. All emitted rows use the same process, swapchain and flip mode; no alternate chain is silently combined.

Phase alignment uses collector UTC start plus its five-second delay: 00:35:43–00:36:13 and 00:36:39–00:37:09 Europe/Moscow. Skyrim logs FG disabled at 00:35:15.671, then enabled with successful preparation/generation at 00:36:26.510; neither requested state changes during its capture. The matching build log and zero logged error/API/callback failures were verified. The two previously recorded optional KreatE/OAR adapter hash warnings remain.

CSV wall-clock fields are three hours ahead of collector UTC converted to Moscow and the game log. They are retained unchanged. They are used for relative durations and trimming; absolute phase alignment uses the collector receipt. The analyzer explicitly handles timestamps without relying on the host's date parsing culture. Re-running the tracked analyzer reproduced the exact analysis hash.

Limitations: tearing is allowed, so display updates do not prove every image was displayed in full. There are no FrameType markers; individual generated records are not labelled. Present-to-display metrics do not measure input-to-photon latency. There is no optical scanout measurement, and absence of NA among emitted records does not establish zero drops across all possible events. The user accepted visual appearance; formal Gamma22/ENB calibration remains untested. Fullscreen/Alt+Enter testing is excluded by the user's 2026-10-03 borderless-windowed scope decision; it is not a passing test. Hardware/Graphics Tools limitations remain unchanged. Seven of eight milestones remain complete.

No game or MO2 settings were edited. NativeAA, startup FG and normal logging remain configured. Exact evidence hashes and the analysis are in [the capture receipt](FSR_FG_PRESENTATION_CAPTURE.json). Reproduce with `tools/fsr/Analyze-SkyrimPresentationCapture.ps1 -CaptureDirectory <recorded-directory>`.
