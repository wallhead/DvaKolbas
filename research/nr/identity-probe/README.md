# Remaining NR tone drift — 2026-10-06

The owner reports remaining drift and confirms that **After NR also drifts at
Native**, so reduced render scale alone cannot explain it. Stable AIO19 and older
RazKolbas gameplay reports remain valid positive visual references. This
investigation does not claim that either reference actually drifts.

The [supplied report](supplied/NR_TONE_DRIFT_STATUS_2026-10-06.md) is evidence to
check, not an instruction to replace the production ring. No installed DLL,
INI, MO2 selection, tone/style controls or shipping ownership behavior changed.

## Current run versus the report

The latest local log is clean source `63743105beb6`, before the installed
Review-7 update. It shows DLSS Quality, After NR, Gamma22 **SDR bytes**, one pass,
Style 0 / Tone 1 and zero loaded appearance presets. Its final active sample
records source 14017, 6000 evaluations and 31 cumulative resets. It contains no
new color/guide pixel capture. Native drift is a separate owner report, not
inferred from this Quality log. [Provenance](provenance.json).

The packaged INI defaults to `SdrBytesTrial=false`, but also defaults to
`CommunityRuntime=false`. That template does not describe the active trial.
Community After explicitly bypasses unless SDR bytes is enabled; it does not
silently evaluate After in FP16. The packaged false setting does not explain
the active SDR After run.

Old `motionPx` logs measured display-pixel displacement from UV motion. Review 7
now labels render/display units separately. Its admission/logging corrections
do not claim to fix tone or jittered guide alignment.

## Identity and queue experiment

Private copies of production Stage/Before code use the same RTX40 model and
current driver on RTX4080SUPER. Native color/guides are 640x360, depth is 0.5,
jitter is zero, Style0/Tone1 and intensity/structure/skin1 remain enabled, and
auto skin is off. No SR, FG, ReShade or game is present. Every source/output
alpha pixel is checked.

Seven cases cover 2/16/64-pixel pans, a fixed textured surface with changing
surroundings, one/three passes, and SDR bytes versus linear FP16. This artificial
stress scene does not measure Skyrim drift or establish a quality threshold.

| Variant | Vendor textures, including intermediates | Evaluation parameters | Retirement |
| --- | --- | --- | --- |
| Rotating product path | Three sets | Three objects per pass | Ordinary bounded ring |
| Fixed parameters | Three sets | Feature creation object per pass | Drain before mutation |
| Fixed resources | One set | One separate slot object per pass | Drain before reuse |
| Fixed both | One set | Feature creation object per pass | Drain before reuse |
| Fixed IO, rotating parameters | One set | Three objects per pass | Drain before reuse |
| Drain-only control | Three sets | Three objects per pass | Drain every source |

The independent fixed-IO case keeps Stage's parameter ring at three slots while
fixing the shared bridge and intermediates. All variants preserve real reader
retirement. Logged vendor-export arguments verify address counts and whether
the feature creation object is actually used; option names alone are not proof.

Five variants first use synchronous readback each source. All six then use
separate per-source staging copies and **Map only after dispatch ends**, with
CPU input preparation completed beforehand. The latter rotating path actually
reaches **three pending tickets** in its first twelve sources; drain-only and
fixed reuse reach one. Tickets represent retained submission/readers, not a
measurement of simultaneous shader execution. Actual queue ordering is retained.

Across **77 enabled runs / 10,560 real sources**, every full RGBA8 output SHA-256
matches its serial rotating baseline, source by source. Four off controls
preserve another **560 sources**. Enabled runs have one initial reset and
contiguous sources. [Full results and identities](validation.json). Seven compact
baseline CSVs are in [data](data); readbacks/builds remain local outside Git.

| Color route / passes | Maximum settled RGB-channel difference, 8-bit code levels |
| --- | ---: |
| SDR bytes, one pass | 8.838865 |
| SDR bytes, three passes | 32.319455 |
| Linear FP16, one pass | 22.552465 |

The input patch has zero difference. Phase means use the last twenty sources
of each forty-source phase, excluding transitions. All identity/queue variants
retain these context-dependent changes. Neither fixed IO nor serialized
evaluation fixes this fixture's tone response. There is no measured reason to
serialize the shipping ring and incur its performance cost. A different game
input/guide distribution can still expose behavior absent from this scene.

## Next game boundary

The game cause remains **unresolved**. Preserve full tone/styles; do not restore
Stable colors, lower Tone to zero or call the single-slot variant a fix. Native
still has temporal jitter: Native drift rules out reduced scale alone, not every
reconstructed-color versus jittered-guide correspondence issue.

Next compare **Before versus After at Native**, same save/build, one pass,
Style0/Tone1, sharpening off and FG off. Use slow/fast building pans plus an
NR-off control. If Before is stable and After drifts, prioritize completed-SR
color/guide correspondence. If both drift, compare actual pre-model bytes/guides
and effective controls against the stable reference. The historical AIO passive
reader measures retained requested parameters, not atomic network inputs/final
callback coefficients; it does not establish pixel parity.

No new Skyrim pixel capture was made. Scaled progress remains **3 of 6**; prior
native functional acceptance does not close this reopened visual defect.

## Reproduce

From the repository root, run `Run-Identity-Probe.ps1` with existing
`-RuntimeRoot`, `-DriverCore` and `-NgxInclude` paths, with Skyrim closed. Default
execution records serial cases. Repeat with `-ResourceIsolationOnly` for the
independent IO case. Use `-DeferredCapture -OutputRoot
out/research/nr-identity-deferred` for queued cases, then the same options plus
`-ResourceIsolationOnly` for independent queued IO.

`Analyze.py --serial out/research/nr-identity --deferred
out/research/nr-identity-deferred --output <receipt.json>` checks controls,
identities, frame continuity, checksums and comparisons. Changed research
anchors fail configuration rather than silently selecting a different patch.
No large model copies or mod writes are performed. Only RTX4080SUPER is tested.
