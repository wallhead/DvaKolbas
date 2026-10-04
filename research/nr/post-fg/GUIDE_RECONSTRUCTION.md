# Generated guide feasibility checkpoint, 2026-10-04

True After-FG NR remains unqualified. A research-only CPU candidate now
compares source-pair depth/motion reconstruction with geometry independently
rasterized at the requested intermediate time. No product code, installed mod,
INI or MO2 settings changed. No GPU work was performed for this checkpoint.

The endpoint interface accepts depth in metres, current-to-history motion in
pixels, source IDs, epochs and timestamps. It cannot consume reference geometry,
color or object IDs. The candidate derives constant image velocity from each
endpoint's actual history interval, projects pixel centres to the intermediate
time, resolves collisions by nearest depth, and scales motion to the requested
output history interval. Uncovered pixels stay explicit. This is a bounded
orthographic experiment with constant depth and velocity, rather than a
perspective/reduced-resolution production guide recipe.

The compiled borrowed-current-guide baseline failed 17 of 25 assertions.
The implementation passes those assertions; an additional subpixel negative
case brings the final candidate test to 26 passing assertions. The full
standalone CTest suite passes 3/3, including the existing independent renderer
and evidence validator. A passing negative test means it detects an inadequate
recipe; it does not qualify that recipe. See [measured CPU receipt](pair-guide-cpu-result.json).

| Case | Observed result |
|---|---|
| Integer midpoint movement, disocclusion, static near occluder | Covered guides match independent geometry. |
| Horizontal camera/object cancellation and vertical pan | Covered motion matches; missing offscreen data stays uncovered. |
| Different output history interval | Vectors scale to that interval rather than the full source-frame interval. |
| Fractional surface boundary | Four covered guide mismatches; point splatting is insufficient. |
| Surface hidden in both endpoints but visible at midpoint | Identical endpoint guides for two distinct scenes, but different reference midpoint depth; four covered guide mismatches. |
| Invalid pair/epoch/extent/time/depth/motion | Rejected before reconstruction. |

The hidden-surface case establishes that these endpoint guides alone cannot
uniquely recover arbitrary midpoint geometry. It does **not** establish that
post-FG NR is impossible: a provider can synthesize color differently from the
physical midpoint, and guides consistent with that synthesis remain a research
option. It also does not prove that FSR's internal reconstruction is correct.
Coverage and successful execution are insufficient acceptance criteria.

## FSR source candidates

The pinned local public header is byte-identical to the official SDK v2.3.0
header at commit `60f4ea81909200d8542eca14dccb2628b763a9a3`. Its presentation
callback supplies real/generated color, separate UI, destination and a command
list, but no depth/motion/fraction. This is a header comparison, not proof that
every local runtime implementation matches the source tree.

The [official resource definitions](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/framegeneration/fsr3/include/gpu/frameinterpolation/ffx_frameinterpolation_resources.h)
identify reconstructed intermediate depth (13), packed game vector fields
(15/16), optical-flow fields (17/18), and a disocclusion mask (14).
The [host implementation](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/framegeneration/fsr3/internal/ffx_frameinterpolation.cpp)
declares the depth and game fields as aliasable R32_UINT textures at maximum
render extent. Dispatch schedules preparation, vector inpainting, color
interpolation and color inpainting, then restores internal states. Aliasing,
later dispatches and reset paths prevent assuming that a resource name implies
valid data at the presentation callback.

The [depth shader](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/framegeneration/fsr3/include/gpu/frameinterpolation/ffx_frameinterpolation_reconstruct_previous_depth.h)
reprojects dilated depth using half the current motion. The
[vector shader](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/framegeneration/fsr3/include/gpu/frameinterpolation/ffx_frameinterpolation_game_motion_vector_field.h)
packs half-motion with priorities; these R32_UINT fields are not directly
NR-compatible float motion textures. The final color shader also combines game
motion and optical flow using disocclusion logic and inpainting. Copying game
vectors alone is therefore not established as a guide for every generated pixel.
Exact files, hashes and line observations are [retained](fsr-2.3.0-guide-candidates.json).

## Next bounded work

1. Establish a research-only exact-runtime access point for the internal
   resources. Prove native identity, extent, representation and producer order;
   never locate them by an unpinned guessed address or resource name alone.
2. Snapshot guides into owned, non-aliasing resources while they are valid,
   with the producing queue/list and actual reader retirement recorded.
3. Decode depth/motion and compare against independent camera/occlusion scenes,
   including subpixel edges and color inpainting. Keep unavailable guide
   confidence and fraction contracts explicit. Validate reset/resize paths.
4. Only after these observations, evaluate NR on the actual generated color in
   the existing separate-UI presentation fixture. NVIDIA's output boundary
   remains a separate unqualified contract.

Do not remove the shipping generated-image rejection or enable After from this
checkpoint. Task 1 remains open; **1 of 8 milestones complete**.
