# Intel trial black-scene correction

The first Skyrim run of `23efd2a8f99b` displayed Skyrim UI and the End menu over a black scene. FSR reported temporal execution, and Intel reported successful real-only output with interpolation disabled. The world trace contained only render boundary 2, source ID 0 and no ordered sources. Game timing remains unqualified.

The Intel final compositor reused the premultiplied UI converter for its scene input. Scene alpha from an upscaler is not coverage: zero alpha caused that converter to discard valid RGB, while HUD pixels survived. A native GPU regression reproduced this before the correction by publishing nonzero scene RGB with alpha 0, 128 and 255 into transparent HUD regions and reading back the final composed image.

`SdrAlphaMode::OpaqueScene` now normalizes alpha only on Intel's owned world-scene copy, before HUD composition. Existing converter callers default to preserving alpha; the separate premultiplied HUD retains its coverage. The same readback regression passes after the correction. Twenty-three selected Intel/source/color/host checks pass, including the actual Intel SDK probes. GPU debug layers were unavailable. Independent static review found no critical or important issue.

The next game trace also records bounded update/input/render callback counts and whether installed call-site bytes remain intact, plus the preserved callee prefix. These are read-only diagnostics; no missing timing markers are synthesized and no hook is automatically reapplied. External read-only instruction capture was denied by Windows during the first run.

The visible Skyrim result requires another user check. Progress remains 3 of 8; Intel FG must stay off until genuine engine ordering is qualified. Broader NVIDIA override and NR/FSR/ReShade probe failures from round 16 remain recorded separately.
