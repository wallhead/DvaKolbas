# XeSS FG adjacent-frame pacing investigation

Status: SDK hint correction prepared; Skyrim confirmation pending. Milestones
remain 5 of 8 complete.

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

## Split capture and controlled hint experiment

The `4957b4b6af88` Skyrim capture confirms the alternating wait is the exact
Intel proxy Present call. For sources 9654–9717, all 64 sources generated two
frames with interval 0 / flags 512. Preparation median was 0.208 ms, next
XeLL Sleep 0.023 ms; proxy Present alternated low median 0.616 ms / high
15.497 ms with a 100% high/low transition rate. Input cadence versus current
Present correlation was -0.992; current Present versus the next source's
cadence was +0.995. Later bursts reproduced the pattern.

`RenderPipeline::BeginSourceFrame` measures source-to-source cadence, including
the preceding proxy wait. Host forwarded that as the optional Intel
`frameRenderTime` pacing hint. Intel's [pinned SDK guide](https://github.com/intel/xess/blob/8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0/doc/xess_fg_developer_guide_english.md#frame-constants)
allows zero when a reliable hint is unavailable; non-Intel pacing can consume
the hint. This creates a candidate feedback mechanism, separate from XeLL.

The existing real-SDK source probe now accepts `--pacing-hint-cadence` and
`--pacing-hint-zero`. These opt-in diagnostic modes retain FSR 3.1.5 Native,
2560x1440, actual one-pass NR After Style 0, guide/HUD transport, Intel FG and
tearing flags. Each executes 160 sources. Both use an identical 12 ms CPU
sleep as a bounded stand-in for additional Skyrim world work; neither changes
SR/NR source time. Only the copied FG input's hint changes. No paced FPS cap
or per-frame GPU drain is added; a final readback checks the real HUD.

Initial settled samples (IDs 41–160):

| Metric | Cadence hint | Unavailable (zero) hint |
| --- | ---: | ---: |
| Source cadence median | 24.587 ms | 22.329 ms |
| Source cadence p95 | 27.080 ms | 24.510 ms |
| Adjacent source difference median | 8.764 ms | 2.389 ms |
| Adjacent source difference p95 | 9.862 ms | 4.761 ms |
| Cadence standard deviation | 4.393 ms | 1.441 ms |
| Proxy Present p95 | 10.092 ms | 7.529 ms |

Both cases generated 159 of 159 eligible sources, SDK result 0, with the real
HUD intact and clean retirement. Zero still had negative adjacent cadence
correlation (-0.901); the experiment demonstrates reduction, not elimination
of pacing oscillation. CPU sleep, checker generation, stdout, fixed NR
timestamps and separate process runs limit extrapolation to gameplay.
Inspect the reverse-order repeat and Skyrim capture before claiming success.

The reverse-order repeat (zero first, cadence second) reproduced reduced
variation: adjacent difference median 3.538 vs 8.067 ms, standard deviation
2.268 vs 4.088 ms, proxy Present p95 8.558 vs 9.647 ms. Both again generated
159/159 eligible sources with SDK result 0, HUD readback and clean retirement.
Median cadence was essentially unchanged in that repeat (21.577 vs 21.408 ms),
so do not claim a reliable average-FPS gain. The repeat supports reduced
oscillation independently of run order; residual jitter remains.

The Host correction validates the original source time, conventions and
history admission first; then zeroes only the copied FG timing hint before
preparation. Original SR/NR/simulation values and all source IDs, admission,
resource lifetime waits, markers, Sleep mode and Present parameters remain
unchanged. Host regression observes the actual public TagFrameConstants call,
rejects feedback hints for alternating 17/33 ms source times, and verifies
negative/NaN/infinite original times cannot warm qualification or tag guides.
Both regressions were observed failing before their respective corrections.
Independent review caught the initial invalid-time masking; the final ordering
preserves that rejection. Test observation storage is reset to avoid vacuous
checks after its bounded buffer fills.

Artifacts: `xess-pacing-split-capture.log` and `.summary.json` in the ignored
capture directory; `pacing-hint-cadence.log`, `pacing-hint-zero.log` and
`pacing-hint-summary.json` in `out/research/xess-fg-reference/`. The SDK probe
runs on the actual RTX 4080 SUPER. D3D11/D3D12 debug layers were unavailable
in this session; runtime generation/status/readback/lifetime checks still ran,
but these captures do not constitute graphics debug-layer validation.

Final targeted validation before clean build: all 19 `^XessFg` checks passed,
including seven GPU checks (24.73 seconds), with source, SDK constant and
invalid-clock regressions in the rebuilt Host test. This is a candidate pacing
fix; in-game graph/visual confirmation is still required.

## Gameplay result for the zero-hint candidate

The subsequent Skyrim capture verifies installed source `d56fea8ab659`, FSR
Native, NR After one pass Style 0 on the RTX 4080 SUPER. The user reports
"smoother" motion. Nine complete 64-source diagnostic bursts contained only
requested FG with two SDK-queued frames per source and result 0 in periodic
status records. No error/critical entries were present in the saved snapshot.

The regular CPU cadence oscillation remains. In the final seven bursts,
adjacent source-time difference medians were 12.87–14.16 ms; exact proxy
Present lower-half medians were 0.84–2.61 ms and upper-half medians
14.03–16.24 ms. Prepare medians remained 0.208–0.223 ms and next XeLL Sleep
0.024–0.025 ms. The previous capture's adjacent-difference medians were
13.38–15.41 ms. Separate sessions and differing world workload prevent a
controlled improvement claim from those ranges. Removing the first row of
each burst avoids the buffered logging boundary; neither capture measures
physical display intervals.

The synthetic improvement did not reproduce at the same magnitude in Skyrim.
Retain the user-visible improvement as a partial result, not a completed
stutter fix. Qualification stays at milestone 5 of 8. The next investigation
must explain the remaining proxy Present alternation: compare actual SDK
queue/presentation configuration and resource submission against AIO19,
then test one evidenced difference at a time. Existing AIO19 static evidence
shows the same zero-minimum-interval low-latency XeLL mode and post-Present
next-frame Sleep schedule; it does not establish actual screen pacing or
justify removing ownership waits or adding an arbitrary FPS cap.

Saved evidence under the ignored capture directory:
`xess-pacing-zero-gameplay-capture.log` and
`xess-pacing-zero-gameplay-summary.json`. The local parser
`out/research/xess-fg-reference/analyze-gameplay-pacing.py` records burst
boundaries, requires all sampled rows to be generated, and keeps adjacent
differences within each burst. No installed files or settings changed during
this follow-up.
