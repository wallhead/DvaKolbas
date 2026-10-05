# Next Skyrim gate: actual End-menu Apply

2026-10-05 result: owner reports the menu tested. The current clean build
logs nine successful Apply actions, NR off/on, After -> Before -> After,
changed tone/structure and resumed NVIDIA x2 output, with no error/critical
entries. All captured NR revisions still have Style 0; the missing style
Apply cycle was requested separately. See the
[bounded receipt](end-apply-game-20261005.json). The original checklist below
is retained for provenance; do not repeat already accepted steps wholesale.

The accepted NVIDIA trial uses DLAA, one After NR pass, NVIDIA FG, Style 0,
Tone 1, full tone and Stable colors off. Provider replacement remains staged
for restart. This gate exercises the real menu and game Present path; the CPU
wrapper/Controller fixtures cannot qualify mouse dispatch or Skyrim behavior.

Start with the existing MO2 SKSE entry and load the usual save. In End, use
**Apply** after each edit, then close End and watch the world for a few seconds:

1. NR off, then on.
2. After upscaling -> Before upscaling -> After upscaling.
3. Tone 1 -> 0.65 -> 1; Style 0 -> 1 -> 0.
4. FG off, then on. Keep DLAA/native size and the NVIDIA provider selected.
5. Open inventory/map, save and reload, then alt-tab/minimize and restore.

Check that Apply never freezes, each requested change takes effect, camera
colors and HUD remain intact, and NR/FG resume. Use Apply for this session;
Save as default is unnecessary for the test. Report skipped steps or failures.

Normal logging is sufficient. After the owner reports completion, retain the
fresh startup/build identity and `[Overlay] source settings applied`,
`[Community NR settings]`, `[Community NR frame]`, deferred source-settings
retirement and FG output/status records. Correlate revisions/placement and
successful post-menu/world evaluations; an old log or an unobserved skipped
step cannot close this gate. No extra Streamline state query is added: its
output-count delta is consumed by the existing Session snapshot.

Task6 stays partial, **5 of 8 milestones complete**, until its remaining
game/lifecycle gates are supported. Broader scene/hardware and display-timing
acceptance remain separate.
