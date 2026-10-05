# NR Before or After upscaling, before frame generation

Design revision based on the owner's 2026-10-04 request: "input -> DLSS -> NR
-> FG -> final". This supersedes the earlier requirement for a separate NR
evaluation on every generated image. This checkpoint changes documentation only.

## Render order

```text
Before: HUD-less input -> NR -> DLSS/DLAA or FSR/NativeAA -> optional FG -> native UI -> final
After:  HUD-less input -> DLSS/DLAA or FSR/NativeAA -> NR -> optional FG -> native UI -> final
```

One NR pass per eligible real source frame, at the selected placement. After
means **after upscaling, before frame generation**. FG synthesizes additional
images from enhanced real sources; generated images receive no separate NR
evaluation. With FG off, After still enhances the real upscaled image.
No simultaneous Before/After passes or duplicate legacy NR.

Enable/disable and Before/After placement remain live controls without restarting
Skyrim. Both SR providers and their existing supported FG routes remain in scope.
One GPU-specific model identity stays retained for the process. NVIDIA RTX
50/40/20-30 keep the exact supplied model catalog; AMD NR remains unsupported.
Actual local hardware qualification remains RTX 4080 SUPER.

## Source-stage contracts

Reuse the shared runtime owner, Stage, history and retained ticket/resource
machinery. Post-SR packets describe real source IDs/epochs, display-size color,
explicit color domain, real camera/depth/motion provenance, producer dependencies
and final-reader retirement. Keep generated packets rejected; do not relabel them.

Qualify render-to-display guide preparation against independent real-time scene
geometry: motion direction/units/scaling, depth convention, jitter and occlusion
boundaries. Native AA is the first case; equal dimensions do not qualify reduced
render extents. Unsupported guide/color combinations remain explicitly unavailable.

Preserve accepted SDR-byte full-tone/style behavior and source alpha. FSR's
internal linear output is not assumed equivalent to the accepted encoded SDR NR
input. No Stable colors or Tone=0 workaround. Post-SR output must not feed back
into SR temporal history, but must feed real presentation and FG exactly once.

Freeze settings per real source. Placement/re-enable/tuning transitions retire
old readers before switching resources, reset NR history and reconcile FG history
so old/new appearance is not mixed. Missing/unsafe retirement never authorizes
release. Menus, loading, camera cuts, resize and minimize retain explicit admission.

Dedicated native UI stays outside NR and is composed once per displayed image.
Preserve existing ENB/ReShade ownership. Initially post-SR NR follows existing
after-upscale ReShade work and precedes UI/FG consumption; record that order in
comparisons. No extra effects are invoked on generated images.

## Provider boundaries

The common SourceFrameEvaluator currently orders upscale, after-upscale ReShade
and generation preparation. Add an optional source-stage operation after source
effects and before FG consumption, even with FG off. NVIDIA's legacy preparation
hosts late real-source NR, but the community owner must support both providers
without initializing DLSS-G solely for NR. Suppress duplicate legacy execution.

FSR prepares retained guides and separately uploads scene color for presentation.
Updating only the D3D11 visible output is insufficient evidence: prove that the
scene captured/uploaded for FG comes from the NR result before producer submission.
Preserve prepared guide ownership and explicit color encoding. NVIDIA must
likewise tag enhanced HUD-less FG color rather than an earlier unenhanced texture.

No NR work runs in presentation workers. No private post-generation NVIDIA hook
or FSR per-generated-image callback is needed. Previous post-FG experiments remain
deferred research rather than prerequisites for this source-stage design.

## Validation and rollout

Owner direction, 2026-10-05: matched AIO19 performance comparison is excluded
by “we dont need performance comparison”. Retain the measured source/display
timing and standalone stage timings; make no AIO parity or performance-improvement
claim. Functional acceptance, guide/color restrictions and lifecycle requirements
remain required and are recorded separately.

Runtime RED/GREEN tests cover stage order, off bypass, exactly one source pass,
FG-off support, stale-guide rejection and history transitions. GPU readbacks must
prove distinct SR/NR output and the enhanced FG source, exact alpha/native UI and
actual delayed-reader retirement through toggles/resize/minimize.

Post-SR inference can cost more than Before at reduced render resolution; measure
source cost at matched model/settings/extents. No performance improvement is
promised. Generated images inherit enhancement but require normal visual/UI/pacing
acceptance; do not count them as NR evaluations.

Validate a separate package before the owner starts Skyrim. Preserve the working
Before trial, INI/profile/launch settings and rollback. No GPU probes alongside
Skyrim, installation with Skyrim/MO2 open, or launching/killing the game.
Carry the eight milestones forward without marking changed scope complete.
