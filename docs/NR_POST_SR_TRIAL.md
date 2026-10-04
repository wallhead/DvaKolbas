# Native AA post-upscale NR trial

The order is **DLAA or FSR Native AA -> one NR pass -> optional FG -> native UI**.
Generated images inherit enhanced real sources. They receive no separate NR pass.
Scaled DLSS/FSR remain supported with Before NR; their After guides are unavailable.

This separate package preserves the accepted INI except `NRBeforeUpscaling=false`.
It retains full vendor tone/styles, Stable colors off and the qualified SDR-byte
runtime route. Model/driver paths stay fixed for the process. No MO2 settings are
changed by staging.

In End -> Neural Rendering, choose Before or After upscaling and Apply. NR off/on,
tuning and placement are live; mode/provider changes retain their existing
restart requirements. `[` disables NR and `]` enables it. After currently needs
DLAA or FSR Native AA. Menus/loading or invalid world guides bypass NR.

The standalone tests prove actual RTX 40 NR output, exact alpha, stage switches
with a pending encoder, resize, ReShade device ownership, and actual FSR Native
AA enhanced-source upload. They do not establish the actual NVIDIA vendor chain,
combined FSR generated-output quality or Skyrim acceptance of After placement.
Those checks remain open. Other NVIDIA families are NOT RUN; AMD NR unsupported.

Install only when Skyrim and MO2 are closed, keeping the accepted Before mod for
rollback. Start manually using the existing MO2 SKSE entry. Compare the same scene
with Before/After and NR/FG off/on, then inspect HUD/inventory/map/dialogue, camera
motion and tone, save/load, fast travel and alt-tab/minimize. Check logs for
`NR active after upscaling, before FG`, resets/evaluation counts, failures and
FG resumption. A visually changed source is not proof that FG is generating.

The stage manifest records the clean DLL identity, protected source hashes and
all staged file hashes. Staging never activates the mod or launches the game.

Installed checkpoint, 2026-10-04: clean-source dc6a9abe3630 is installed in the
existing V5.4 NO-LORE trial. The accepted DLL/INI backup is in
`out/game-test-backups/nr-post-sr-dc6a9ab-20261004-143234`. Only the DLL and
placement changed; DLAA, NVIDIA FG, Tone 1, Style 0 and Stable colors off remain.
The [receipt](../research/nr/post-sr/clean-build-install.json) records 196 passing
checks and three excluded Graphics Tools checks. Game acceptance is pending.

FSR comparison checkpoint, 2026-10-04: after the successful first DLAA game
trial and combined standalone FSR/NR/FG presentation checks, the FSR Native AA
After/FSR-FG INI is installed. Only UpscaleType=4 and FrameGenerationBackend=2
changed; placement stays After, FG stays enabled, Style 0/Tone 1 and Stable
colors off stay intact. The DLAA INI is backed up; clean dc6a9abe3630 DLL and
MO2 settings are unchanged. [Install receipt](../research/nr/post-sr/fsr-native-aa-trial-install.json).
Start through the usual MO2 SKSE entry, load a save, check FSR active / NR active
after upscaling, and compare NR/FG off/on, camera rotation, HUD/inventory/map,
save/reload and alt-tab/minimize. Report drift, flicker, damaged UI or freezing.
