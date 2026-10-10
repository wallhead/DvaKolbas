# XeSS FG pre-input timing correction

The installed `f088a3af7187` game trace reported FG requested, temporal FSR
evaluation successful, `frames=1`, `sdk=0` and `orderedSources=0`. Input and render
callbacks ran 2,583 and 2,584 times respectively, while the outer-update callback
ran zero times. All installed call-site witnesses were intact. The preserved
outer-update callee had an existing indirect detour; this does not establish
which mod bypassed the unused caller.

Source admission depended on that unused caller, leaving subsequent input and
render boundaries without an active timing ID. The correction starts the source
at the observed main PollInputDevices call (36564 + 0x567), before its preserved
callee, then records input completion after the callee. The inspected main render
call (36555 + 0x47) retains simulation-end/render-start ownership. The unused
outer-update patch is removed. Both remaining sites retain full argument and
relative callee witnesses; unrelated input/render callers remain excluded.

Source reservation uses `mRenderedFrameCount + 1`. The existing jitter hook
increments that count inside RendererBegin before evaluator snapshots, matching
the source's reserved ID. SDK Sleep and SimulationStart remain before actual
input; render and Present markers are not synthesized to bypass admission.

The adapter-order regression failed before correction. After correction, it
passes and drives the production timing owner through input, render and
same-ID Present completion without an outer-update callback. Release compilation
and the 23 selected Intel/source/color/host tests pass, including physical Intel
SDK probes. Independent static review found no important issues.

This establishes the tested correction, not in-game interpolation readiness.
The next Skyrim run must confirm ordered source admission, nonzero SDK IDs and
two-frame output with FG enabled. Milestone progress remains 3 of 8.

The broader suite still has outstanding NVIDIA override probe and NR/FSR/ReShade
route failures recorded in the previous delivery receipt; no full-suite pass is
claimed by this scoped validation.
