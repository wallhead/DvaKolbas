# XeSS FG adjacent-frame pacing investigation

Status: investigation, not a stutter fix. Milestones remain 5 of 8 complete.

The user reports regular stutter with FSR Native -> NR After -> XeSS FG on
RTX 4080 SUPER. The FSR FG comparison graph is flatter. Both source sessions
used the `eae31179f682` presentation-pacing build.

## Evidence

The controlled FSR FG comparison retained NR revision 8, Style 0, Tone 1,
one After pass. Settled rolling medians with FG off/on:

| Metric | FG off | FG on |
| --- | ---: | ---: |
| Source cadence | 23.036 ms | 25.238 ms |
| D3D11 elapsed GPU span | 22.334 ms | 24.592 ms |
| CPU source Present | 0.670 ms | 1.701 ms |

The later XeSS session retained NR revision 1, Style 0, Tone 1, one After
pass. At 23:08:21 its median GPU span was 24.172 ms; source cadence median
26.016 ms / p95 37.300 ms; CPU source Present median 3.300 ms / p95
17.252 ms. The user's graph shows alternating short/long source intervals.
Periodic SDK status reports two frames, result 0 and active generation.
This establishes successful generation with uneven source pacing; it does
not establish evenly spaced physical display frames.

CPU source Present currently includes preparation, Intel proxy Present and
the next source's post-Present XeLL Sleep. The existing aggregate timing
cannot identify which API wait alternates. D3D11 elapsed spans include queue
dependencies/scheduling; CPU and GPU durations overlap and must not be added.

Raw logs and screenshots are preserved under ignored
`out/research/xess-fg-reference/nr-fg-overhead-2026-10-10/`:
`fsr-nr-held-fg-toggle-capture.log`, `fsr-nr-held-fg-toggle-summary.json`,
`xess-nr-sawtooth-capture.log`, `fsr-fg-graph.png`, `xess-fg-graph.png`.

## Diagnostic trial

Under Stage timings AND LogPerformanceMetrics, capture 64 consecutive world
sources per 600 diagnostic sources. Write one buffered burst, not per-frame
log calls. Each row retains source/epoch, current SDK ID, actual next Sleep
SDK ID, incoming frameRenderTime, preparation CPU time, exact proxy Present
CPU time, exact next XeLL Sleep CPU time, enclosing source-call CPU time,
SyncInterval/flags, SDK frame count and requested FG state. `-1` means the
measured API was not called. Sleep IDs are retained even if Sleep or its
following marker fails; successful reservation is still separately required
by the unchanged engine timing path.

Drop partial bursts on source gaps, epoch/owner lifecycle changes,
startup/test/error Present, suspend/resume, menus and diagnostic disable.
Logging itself occurs after the measured sample and can perturb the following
frame; compare the interior of later bursts, after startup traces end.

No marker order, resource lifetime wait, SDK pacing input, FG admission or
XeLL mode is changed. In particular, do not call an aggregate CPU wait
"XeLL cost" before the split capture supports that attribution. The official
guide's non-Intel frameRenderTime pacing heuristic is a possible later A/B
investigation, not a validated root cause.

Ruling: this is the bounded diagnostics/performance qualification work in
Tasks 5/7/13 of the approved XeSS FG plan. Do not mark provider/NR qualification
complete until the reported stutter is reproduced and corrected.

Validation: Release plugin build succeeded. All 19 `^XessFg` checks passed,
including seven GPU checks (24.65 seconds). Regression additions cover missing
timing/stale identity, executed Sleep failure and post-Sleep marker failure,
plus bounded burst reset on source, epoch and lifecycle changes. Independent
review identified the lifecycle/failed-Sleep attribution cases; both were
corrected. Non-S_OK (including occlusion) also drops partial bursts.
