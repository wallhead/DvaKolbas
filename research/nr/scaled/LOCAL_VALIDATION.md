# Fixed-scale After NR — 2026-10-06

After NR processes display-sized encoded SDR color using retained render-sized
depth/motion. Guide subrects remain independent. Render-pixel motion converts
once to display-pixel motion; camera history tracks guide dimensions.
The order remains **DLSS/FSR -> NR -> optional FG -> native UI**. Generated images
inherit the enhanced real source and receive no separate NR evaluation.

Validated locally on RTX 4080 SUPER (10DE:2702), driver 617.14:

| Coverage | Result |
| --- | --- |
| Direct vendor moving-scene guides: native, half, nonintegral | 3 cases, 30 frames each; finite output, exact alpha/guides, delayed reader retirement |
| SDR bridge; fixed display with guide-only changes; one-to-three passes | Native/scaled bridge and retained-host checks passed |
| Actual DLSS sources | All five fixed quality indexes and pending-reader case passed |
| Actual FSR sources | Quality, Balanced, Performance passed |
| Scaled FSR FG | All three qualities in observer and automatic presentation passed |
| FSR pending suspend/resize | Native and all three scaled qualities, observer/automatic; 8 tests passed |
| Scaled NVIDIA FG | Foreground vendor lifecycle passed; actual output-count and reader checks |
| Live settings and package route admission | Fixed After accepted, dynamic resolution rejected, repair preserves After |

Pixel checks retain source alpha, separate UI and unchanged SR input/history.
FSR observers read generated and real callback rows and independently check UI
composition. NVIDIA's opaque generated pixels and physical display cadence are
not established by its API/readback checks. Graphics Tools queues were absent.

A fresh independent whole-change review found no actionable code bugs. The
review inspected real game source routing and guide shapes; it did not execute
GPU tests or claim hardware/game acceptance.

Dynamic resolution, HDR NR and AMD NR remain unavailable. The accepted optional
Before route and one-to-three NR passes remain available. Exact model leases and
genuine driver signature/catalog qualification are unchanged. Other cards are
software eligible through their existing profiles; actual RTX20/30/50 scaled
qualification is **NOT RUN**. Skyrim scaled-image acceptance is **PENDING**.

The original native eight milestones remain accepted. The scaled extension has
**3 of 6** complete; portable cross-family qualification and owner gameplay
acceptance are separate work. Historical plan requirements for removing Before,
limiting passes to one and restoring fixed driver hashes are superseded by the
accepted optional placement/multipass features and later portability fixes.
