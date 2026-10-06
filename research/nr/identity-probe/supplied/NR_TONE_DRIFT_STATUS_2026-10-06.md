# NR tone drift during camera turns — status update

Date: 2026-10-06. Branch: `codex/nr` (head c444b76). Updates `NR_DRIFT_FINDINGS.md` (2026-10-05).

Method: code reading plus earlier research receipts and game logs. No new in-game capture or build was made.

## Symptom

With community NR on, colours and tone shift slightly while the camera turns, then settle when it stops. Turning NR off removes it.

Older builds using the same NVIDIA model are stable at Style 0 / Tone 1:
- AIO19;
- the RazKolbas build.

None of today's commits targets the drift.

## Ruled out

| Candidate | Evidence |
| --- | --- |
| NR history resetting too often | Logs: ~30 resets right after load (the upscaler's post-load reset window), then 0–5 per 600 frames. Source IDs advance by exactly 600 per 600 frames, so there are no gaps. |
| Exposure flag, backbuffer binding | No change in earlier research (`BACKBUFFER_RE_VALIDATION.md`, tone-contract README) |
| Stable colours as a fix | Reduced drift but weakened styles. Removed from the code path in 34c1702 (`StableColorResolve.cpp/.h` remain, unused). |

## Known amplifiers

| Factor | Effect | Current state |
| --- | --- | --- |
| Linear FP16 colour route | 22–30 colour-level drift in a synthetic test vs 7–9 for encoded 8-bit. Gamma 2.2 re-encode roughly doubles small changes in dark areas. | The SDR byte route (`[NeuralRendering] SdrBytesTrial = true`) "seemed fixed" in your trial, but the packaged `RaZkolbaS.ini` still defaults to `false`. |
| NR after upscaling | Before placement, the upscaler's temporal accumulation smooths NR's frame-to-frame changes. After placement, they reach the screen directly. | 6374310 now allows After NR at reduced DLSS/FSR scales (Quality, Balanced, Performance). Reconstructed detail changes more between frames there, so drift may be more visible than at Native. |
| Multiple NR passes | Each pass is a separate NR feature with its own rotating resources and parameters (34c1702). | Any per-pass instability compounds. |

## Main suspect (still present): resource and parameter rotation

| | RazKolbas `NrStage.cpp` (stable) | Current RaZkolbaS `codex/nr` |
| --- | --- | --- |
| NR Color / Depth / MVec / Output | One fixed set, copied into every frame | Three slots cycling. `BeforeUpscale.cpp:25` (`std::array<Slot,3>`, `nextSlot`); `Stage.cpp:76`, `:211` |
| Parameter object at evaluate | The creation object (`impl_->parameters`) | Per slot **and per pass** (`Stage.cpp:238–244`, `slot.passes[i].parameters`). Features are created with different objects (`features[i].parameters`). |
| Pass 2/3 inputs | n/a | Per-slot intermediate textures, so input identity also changes every frame |
| Frames in flight | Waits for the previous evaluation, then reuses the same textures | Up to three frames in flight on different textures |
| Colour format | Always `R8G8B8A8_UNORM` | Linear FP16 unless `SdrBytesTrial = true` |
| Motion scale | Width / height | Width / height (render). Scaled After converts render to display pixels (c086c9e). |

**Hypothesis.** If NVIDIA's NR runtime ties any temporal history to the output resource, the last inputs or the parameter object, then each frame it reads history from three frames earlier, while the motion vectors describe one frame of motion. That only matters during motion, which matches the symptom.

This is a hypothesis, not a measurement.

## New since 2026-10-05

- **34c1702 (three passes):** the rotation is now repeated per pass, with per-slot intermediate textures between passes.
- **6374310 / c086c9e (scaled After NR):**
  - Guides are now render-sized and jittered, while the colour is display-sized and unjittered (review R7-1). That can add edge instability, separate from tone drift. Rule it out when testing.
- **Motion diagnostic (review R7-3):** `NrMotionDiagnostic::Before` reads render-sized motion but scales it with display dimensions, so it overstates motion by about 1.5× at Quality and 2× at Performance. Fix it before using its numbers for drift analysis.

## Recommended tests (in order)

1. **Single fixed slot.**
   - **The build:** NR always receives one persistent Color/Depth/MVec/Output set (and one intermediate per extra pass). It is evaluated with the feature's creation parameter object, and waits for the previous evaluation before reusing the set, as RazKolbas does.
   - **The run:** same save and location, slow and fast turns, NR off and on.
   - **If the drift disappears:** keep NR's resources and parameters fixed, and overlap only the work around it (copies, upscaling, FG).
2. **Placement and scale matrix.** Same build and scene:
   - NR Before DLSS;
   - NR After at Native;
   - NR After at Quality;
   - NR After at Performance.
3. **Colour route.** Repeat with `SdrBytesTrial = true` and `false`. If SDR bytes remains better, make it the community-NR default in the packaged INI.
4. **Passes.** Compare one pass with two or three passes at the same style and tone.

Record for each run:
- the build hash;
- the INI `[NeuralRendering]` and `[NR PASS n]` values;
- the upscaler mode and scale;
- a short capture or a written description of the turn.

## Still not established

- Whether the NR runtime actually keys history to resource identity or to the parameter object.
- Pixel-level drift numbers in Skyrim; all game results so far are visual reports.
- Behaviour on other styles, with FG on, or on other GPUs and drivers.
