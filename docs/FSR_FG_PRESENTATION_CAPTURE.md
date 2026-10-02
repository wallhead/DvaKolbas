# Skyrim display-cadence capture

The base-frame timing capture passed its logged live-toggle smoke checks. On 2026-10-03 the user additionally reported that the image looked fine. This accepts observed appearance for the tested setup; Gamma22/ENB transfer remains provisional rather than formally calibrated.

Next capture uses the existing Intel-signed PresentMon 2.6.0 portable console binary. SHA256: `b2a706bc6ad475749e3b7e3409263aa1e6906d45bdcf993f6dbc0f660188f1af`. Its signature was valid for Intel Corporation. It was copied from Downloads into ignored `out/research/presentmon`; binaries are not committed. [PresentMon's documentation](https://github.com/GameTechDev/PresentMon/blob/v2.6.0/README-ConsoleApplication.md) describes display metrics and their scope.

The non-elevated two-second local probe failed with exit 6, `failed to start trace session: access denied`. PresentMon reported that administrator rights or Performance Log Users membership is required. The current account is not elevated. The helper's parser, launcher target and read-only preflight passed; live ETW and resulting CSV display metrics are not yet validated.

1. Start Skyrim manually through the usual MO2 SKSE entry and load a save. Keep one scene/camera.
2. Right-click `out/research/skyrim-presentation/Start-Skyrim-FG-Capture.cmd` and choose **Run as administrator**. Accept the Windows prompt if available.
3. Follow its two prompts. Set FG off, close the overlay, return to the capture console and press Enter. Refocus Skyrim within five seconds and keep it foreground for 30 seconds. Then repeat with FG on. Do not Save as default.
4. Leave FG on, close Skyrim normally and report `capture done`, including any visible problem. If Windows denies elevation, report that instead; the failed capture is not a cadence pass.

The helper targets only the observed Skyrim process ID, creates unique ETW session/output names, and writes two CSVs, console logs, Skyrim log snapshots and a metadata receipt under ignored `out/research/skyrim-presentation`. It does not launch Skyrim, install a service or edit game/MO2/INI settings. Preserve all CSV swapchains and dropped-frame records for analysis; identify the native display swapchain before comparing display intervals. User-selected mode labels, CSV row counts and source/API Present rates alone do not establish displayed generation. Frame-type classification requires compatible instrumentation and must not be assumed from the phase label.

Automatic Alt+Enter remains untested; the supported trial is windowed SDR with exclusive fullscreen rejected. No new fullscreen capability is introduced by this capture. Seven of eight milestones remain complete, with actual presentation capture pending and previous hardware/Graphics Tools limitations retained.
