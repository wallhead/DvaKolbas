# P4 owned linear FSR handoff

P4 removes the NR game-format encode followed by FSR decode and duplicate guide preparation. A move-only source/epoch/extent/context lease carries linear FP16 color, R32 depth and RG16 motion. FSR copies into its own retained images and registers the actual D3D11 copy-reader before releasing the lease. A per-slot admission fence prevents early collection during that registration. Abandonment faults and retains the producer. Before-upscale ReShade and diagnostic captures retain the encoded P3 route so their edits remain effective; DLSS retains that route too.

NativeAA and Quality fixtures verify source identity, source-off passthrough, pending FSR-reader ownership, abandoned lease quarantine and finite ramps including near-black/highlights. Each full color fixture checks 979,200 linear NR alpha values against an independently decoded GPU reference, 1,036,800 original-source alpha values and 2,937,600 RGB samples. Original Gamma input is unchanged. No tested RGB sample was out of gamut; maximum removed UNORM/Gamma roundtrip error was 0.00488281 in linear color (bound 0.012). This precision improvement can visibly differ from the old roundtrip and still needs game acceptance.

Actual ReShade combined NR→FSR→FG fixtures exercise 170 NR evaluations, six bypasses, 140 direct and 30 encoded deliveries per NativeAA/reduced route, including resize, suspension, off/on and dedicated UI. Standalone coverage includes the real wrapped device, delayed readers, strict public pending-producer rejection, unrelated higher fence signals and retained parameter/descriptor slots.

The clean historical [P3/P4 comparison](p4-comparison.json) captures source `7f9afcb`: three 300-source repeats plus 120 warmup per repeat, pinned RTX40 model, tone/style 0 and FSR 3.1.5 NativeAA at 2560×1440. P4 transaction medians were 9.124–9.543 ms versus P3 8.958–9.040 ms. Vendor GPU time drifted too. **These captures do not establish an isolated P4 performance gain or Skyrim FPS.** Removed conversion stages are confirmed by instrumentation, not a recovered-FPS claim. Alpha preservation measured about 0.04 ms and is retained; no speculative fusion was added.

Separate dirty-source diagnostics reproduced Windows timer dependence in old `Sleep(1)` pressure polling: 13.3443 ms at default timer versus 9.2603 ms with a requested 1 ms timer. Event-based waits measured 9.258 ms without requesting that timer. These diagnostic samples are not matched performance qualification.

Fresh whole-change review found two Important defects before installation. The final fix source `ea8b84d` addresses both:

- Safely discarded FSR dispatch failure remains spatial, including consecutive direct NR inputs. The failing frame uses the retained enhanced input; later frames draw the enhanced leased input and attach a genuine draw-reader signal without declaring temporal dispatch. Four-frame recovery, enhanced RGBA comparisons, a delayed spatial draw and honest poisoned-context retirement pass.
- Capacity admission wakes on progress from any occupied slot and rescans the complete pool. Before/Prepared regressions failed at about 1.62 seconds when the oldest reader stayed gated despite another reader completing at 250 ms; they now return while that oldest reader is still pending. Explicit selected delivery/lifecycle waits and the overall 20-second terminal deadline remain.

All 47 standalone tests pass after the fixes. The Minor getter-lifetime finding is documented: COM retention after lease consumption does not preserve image contents after slot reuse. [Qualification](p4-qualification.json) records clean current receipts and RED/GREEN log hashes; [acceptance](../../../docs/NR_PERFORMANCE_ACCEPTANCE.md) records the separate product and game gates.

Original NR progress remains **1 of 8**. RTX20/30/50 output, AMD support, literal After FG, matched Skyrim performance and camera-motion color acceptance remain open. Tone 0 remains the user-accepted mitigation.
