# Review 7 — 2026-10-06

R7-1 is an outstanding visual alignment check. Skyrim supplies jitter-free UV
motion values, but depth/motion are sampled on the current jittered render grid.
After SR color is reconstructed at display resolution. Vendor inference,
finite/alpha/ownership checks and the current synthetic source tests do not
establish correspondence at moving depth discontinuities. No unqualified guide
resampling or vendor jitter parameter is introduced. Logs now retain actual
guide jitter in render pixels; the NVIDIA completed-source metadata copies the
same jitter already supplied to DLSS. Scaled visual acceptance remains pending.

R7-2: previously the motion-scale fallback survived failed metadata validation,
but BeforeHost independently rejected that metadata before NR evaluation. The
new binding function clears both scales on failure. The game adapter explicitly
disables that source's NR request, invalidates camera history and reports the
original rejection in the End status/log. Retained work is drained through the
normal disabled-source path; no guessed scales reach NR.

R7-3: multiplying normalized UV motion by display dimensions measures display
pixels correctly. It is 1.5–2 times the corresponding render-pixel displacement
at those ratios, so the old unlabeled `motionPx` values must not be interpreted
as render pixels. Diagnostic logs now report both `motionRenderPx` and
`motionDisplayPx`, using the declared motion scales and actual guide size.
This clarifies diagnostic units; it does not change evaluated NR motion.

R7-4 remains an in-game quality comparison, not a confirmed tone bug. Keep full
Tone 1, Style 0, one pass, sharpening off and the same SDR encoding/scene for
Native and Performance. Keep FG off initially. Pan slowly past a building,
thin geometry and foreground/background boundaries; compare NR off/on for
edge shimmer, halos and brightness/color pumping. Then repeat with FG on.
Repeat for DLSS and FSR after provider/quality restarts. Record skipped steps.
Do not recommend scaled After as visually accepted before this comparison.

Verification: regressions fail without their binding/domain helpers; both new
CPU contracts pass. Fifteen focused CPU/real-GPU checks passed, including native
placement/settings/pass changes, guide-only resize and actual FSR Quality,
Balanced and Performance source processing, all five fixed DLSS qualities and
pending DLSS reader retirement. The production plugin also builds. No Skyrim visual test was run by
the agent. The scaled milestone count stays **3 of 6**.
