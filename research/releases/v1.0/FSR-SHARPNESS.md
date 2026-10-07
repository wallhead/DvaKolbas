# FSR sharpness investigation — 2026-10-07

A tester reported no sharpness effect; the selected control, hardware and active
pipeline were not supplied. The report has not been reproduced locally.

The FSR slider is independent of the legacy NR-tab DLSS sharpening. The
production controller queues its live value into the existing FSR session.
After successful Present, the host adopts the dispatch-only sharpness change
without recreating the upscaler. Temporal dispatches read that effective value
and set the official SDK enableSharpening/sharpness fields. Main/loading screens
use spatial recovery, which does not dispatch FSR sharpening.

New characterization checks exercise the production controller and host with
0.8 → 0 → 1, verify effective settings after Present and preserve separately
staged render-scale changes. GPU comparisons render identical inputs, camera,
jitter, depth, motion and source IDs in fresh matched contexts at sharpness 0
and 1. Both Quality and Native AA produce different pixels. Output hashes repeat
exactly across runs. This covers the Analytical provider on the local RTX 4080
SUPER, not the tester's environment. The 128-frame comparison and existing
1,000-frame smoke pass, including UI sentinels, input recovery, in-flight work
and allocation retirement. All 75 selected settings/compatibility/GPU checks pass.

The End menu title reads `RazKolbaS by WallHeaD`, retaining its ImGui window ID.
FSR controls show Active FSR sharpness, explain 0=off/1=maximum and apply-on-release,
and distinguish the DLSS control. Startup logs, live application logs and opt-in
frame-detail logs include FSR sharpness. No sharpening algorithm or pipeline-order
change was needed.

Tester check: select FSR and restart if changing provider, load a save, verify
FSR active, then compare Sharpness 0 and 1 in the DLSS/FSR tab with NR and FG off.
Release the slider and check Active FSR sharpness. If there is still no visible
effect, retain the log and report GPU, render scale, NR and ReShade settings.
Menus/loading screens do not qualify this comparison.
