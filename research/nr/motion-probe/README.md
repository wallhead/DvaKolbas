# NR camera-motion investigation, 2026-10-03

The user reports that NR's appearance changes when rotating the Skyrim camera,
while the overlay continues to report NR active. The successful save-load receipt
is therefore a loading/processing smoke test, **not visual acceptance**.

The game log loaded **zero appearance presets**. The FSR community NR source path
reads the authoritative base settings directly and does not invoke the weather
controller. During the reported period its settings revision and reset count
remained stable. Weather/time automation does not explain this observed run.
`[Appearance] Enabled=false` alone is not sufficient evidence: format 4 derives
automation from active preset files, so the actual loaded count and call site were
checked too.

This isolated probe exercises the unchanged native SDR BeforeHost on the qualified
RTX 4080 SUPER. Each process supplies 120 sources: 40 stationary, 40 with a known
horizontal camera pan, then 40 stationary. It records raw input/output PPM images,
per-source NR/reset state and two measurements on the interior surface:

- Mean absolute RGB change from the source, in 8-bit code values.
- Mean absolute RGB difference from the previous output after exact surface-point
  reprojection. Exposed borders are excluded. The first frame has no history.

The receipt summarizes **20-source windows**, not complete phases: stationary
sources 21–40, panning sources 51–70, and stationary-after-pan sources 101–120,
inclusive. Initial stationary history and the immediate movement/stop transitions
are excluded to compare settled behavior. The panning window also stays away from
the phase end. All 120 per-source measurements remain in each CSV so transient
behavior can be inspected separately. This window choice does not establish a
universal visual-quality threshold.

Guide controls use correct current-to-previous UV motion, reversed motion or zero
motion, with the product's positive width/height scales. The off control passes
through the identical source sequence. Correct-motion probes additionally use
16- and 64-pixel pans per source. The surface is textured, with fixed depth 0.5;
this does not recreate a perspective Skyrim scene, its real guide buffers,
disocclusion, sky, ENB/ReShade effects, SR or FG.

An explicit research-only CMake option builds a **private copy** of Stage.cpp with
`Backbuffer=Output` and matching subrect, following the recovered AIO immediate
route's fallback. It never edits the product Stage or installed mod. The saved
correct-motion outputs match the unmodified Stage byte for byte in this fixture;
this parameter difference is not evidence of the game's root cause.

Both configurations compile. Every enabled route evaluated all 120 sources with
one initial reset. The off route evaluated none. Correct guides produced smaller
surface reprojection differences than reversed/zero guides. Enhancement did not
collapse during these simple pans. See [bounded measurements](results.json).
These observations do **not** establish the cause or fix the reported game defect.

Build this directory as a standalone CMake project, supplying `NR_NGX_INCLUDE`
with the existing local SDK. The executable takes:

```text
NrMotionProbe.exe <three-profile runtime root> <pinned driver core> <output directory> <correct|reverse|zero|off> [pan pixels per source]
```

Run variants serially and only with Skyrim closed. The executable checks this
before GPU use. `NR_RE_BACKBUFFER_OUTPUT=ON` selects the isolated parity experiment.
The next game comparison is the same stationary/pan scene with **NR on, FG off**,
then FG on, followed by NR off as a source-image control. If it persists without
FG, actual source/guide capture is needed before changing guide conversions or
model parameters. The installed DLL, INI and MO2 configuration were not changed.
