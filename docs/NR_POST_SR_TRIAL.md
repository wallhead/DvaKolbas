# Native AA post-upscale NR trial

Packaging update, 2026-10-05: new stages use two physical NR DLLs. RTX40 and
RTX50 keep separate logical profiles but share `NR/rtx40/nvngx_dlssnr.dll`
(`e67dee...`) and its caller-identity compatibility policy. RTX20/30 retain the
existing FP16 DLL. This saves 158.16 MiB; actual RTX50 output remains NOT RUN.
Staging omits the old RTX50 payload and preserves the accepted source mod and
rollback files. The historical package/build receipts below remain unchanged.

Final functional acceptance, 2026-10-05: **8 of 8 milestones complete for the
tested native SDR setup**. The owner confirms NVIDIA inventory/map/dialogue
and minimize/restore, plus FSR Native AA After fast travel, completing the
previously accepted provider controls, color/HUD and lifecycle checks. NVIDIA
ETW display updates measure 46.52 Hz FG off -> 88.96 Hz on (1.912x), with sampled
NR active and no logged errors in the captured windows. See the
[final acceptance](../research/nr/post-sr/task8-functional-acceptance-20261005.json).
AIO19 performance comparison is excluded by owner, not passed. No parity claim.
Other GPUs, HDR/scaled After and optical scanout remain unqualified; AMD NR
remains unsupported. Existing working trial settings are retained.

Matrix qualification, 2026-10-05: clean Standard
NR-on/off Release builds identify source `b8028445dc89` and the expected
capabilities. NR-on passes 215 checks; NR-off passes 159 in CTest plus seven
initially skipped runtime checks rerun successfully with pinned arguments.
Three unavailable Graphics Tools prerequisites remain excluded. The separate
`out/packages/nr-final-b802844-20261005` package is hash/pin validated;
the installed trial and MO2 files remain unchanged. The fresh whole-change
review found no Critical/Important findings. See the
[matrix receipt](../research/nr/post-sr/final-matrix-20261005.json).
The staged package remains the exact validated Task7 artifact; later functional
acceptance is recorded separately. Its product implementation matches the
installed accepted DLL; their embedded source-revision markers differ.

Optional timing repeat uses the working installed trial. Start Skyrim through the
usual MO2 SKSE entry and load a save; leave NR enabled with identical controls
for both phases. Run `out/research/skyrim-presentation/Start-Skyrim-FG-Capture.cmd`
as administrator (ETW needs elevation on this setup). Follow the FG Off and
On prompts, close the overlay and keep the same scene foreground for 30 seconds
each. Do not Save as default. Leave FG on and close Skyrim normally afterward.
The helper changes no files or MO2 settings. Any new raw capture requires
swapchain/display-interval and game-log validation; phase labels are not proof
of generated-frame cadence. The existing NVIDIA capture is accepted within
ETW display-update scope; no further AIO comparison is requested.

Live-control acceptance, 2026-10-05: the owner confirms
stable camera colors and HUD during live style changes. Fifteen successful End
Apply actions, continuing NR and resumed NVIDIA x2 output close Task6 together
with the separate pending-reader and retirement proofs. See the
[completion receipt](../research/nr/post-sr/task6-completion-20261005.json).
Task6 acceptance does not establish physical display cadence.

Installed build, 2026-10-04: clean-source `effbf2c9b0b0` adds the outer
Present/Present1 preparation-failure guard. The updated DLL is installed in
the existing V5.4 NO-LORE trial; its INI, other mod files and MO2 settings are
unchanged. The working DLL/INI are backed up. The noninteractive suite passes
215 checks; three debug-device/interface prerequisite checks remain unavailable
and are not passes. See the [install receipt](../research/nr/post-sr/present-guard-trial-install.json).
Start with the usual MO2 SKSE entry and run the
[End-menu Apply checklist](../research/nr/post-sr/END_APPLY_CHECK.md).
The End Apply gate is now accepted within the native-size SDR scope above.

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

The standalone tests prove actual RTX 40 NR output, exact alpha, pending source
switches, resize, ReShade device ownership, actual FSR Native AA enhanced-source
upload and combined FSR presentation/retirement. Local FSR gameplay checks pass.
The actual production DLAA backend also passes NR source/tag readbacks at two
native extents. Its Streamline API sink is scripted: it does not prove vendor FG
output or native game UI composition. Later actual NVIDIA lifecycle probes
qualify pending readers and controlled retirement separately; owner gameplay
checks accept the listed local DLAA/FSR controls and image stability.
Other NVIDIA families are NOT RUN; AMD NR unsupported. Scaled After and matched
performance remain separate qualification gates.

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

NVIDIA pre-game checkpoint, 2026-10-04: the matched DLAA After/NVIDIA-FG INI
is prepared. Only UpscaleType=3 and FrameGenerationBackend=1 differ from the
accepted FSR INI. The working clean dc6a9abe3630 DLL is reused because its
production source is unchanged. Full Tone 1, Style 0, Stable colors off, native
UI and normal logging are preserved. After confirmed MO2/Skyrim closure, the
INI alone can be installed with an FSR INI backup. Start through the existing
MO2 SKSE entry; use the same save for NR/FG off/on, camera/color/UI, dialogue,
save/reload, fast travel and alt-tab/minimize. This qualifies the NVIDIA route;
the accepted FSR checks do not need repeating. The INI is now installed after
confirmed closure, with an FSR rollback and protected DLL/MO2 hashes verified.
See the [source proof](../research/nr/post-sr/nvidia-source-handoff.json) and
[installation receipt](../research/nr/post-sr/nvidia-pre-game-install.json).
