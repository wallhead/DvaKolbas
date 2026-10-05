# Common post-upscale NR, scaled sources and GPU qualification

Status: conversational direction approved; written spec awaiting owner review.
Date: 2026-10-05. Baseline: `509733894da7fd231e0b6db36fdac62cd28e7091`.

## Owner intent and scope

The owner requests scaled After NR and support/validation for RTX 20, 30 and
50, and confirms: "keeping DLSS/FSR -> NR -> FG -> UI ... across all NR paths".

This design uses one common source order for every NR model/profile:

```text
HUD-less input -> DLSS/DLAA or FSR/Native AA -> NR -> optional FG -> native UI -> final
```

Interpretation to confirm in written-spec review: "all NR paths" includes
placement as well as GPU profiles. The old user-facing Before placement is
therefore replaced by the common post-upscale order. Previously saved
`neuralBeforeUpscaling=true` is migrated to post-upscale; it must not silently
run an early pass. Historical Before validation and rollback files remain valid
records of the previous implementation. This spec supersedes the placement
choice in `2026-10-04-nr-post-sr-design.md` for the new extension only.

NR runs once on each admitted real upscaled source. FG receives that enhanced
source. With FG off, the real presented image still receives NR. Generated
frames inherit enhancement through FG and receive no separate NR evaluation.
Off/on, style and tone controls remain live, without restarting Skyrim.

Fixed DLSS/FSR Quality, Balanced, Performance and Ultra Performance modes are
in scope wherever the selected SR backend supports them, alongside the accepted
native modes. Dynamic resolution, HDR NR, multiple NR passes and reduced NR
model/input scales remain outside this extension. AMD NR remains explicitly
unsupported. There is no new AIO performance comparison requirement.

## Choice of guide contract

Recommended approach: qualify display-sized color with the original render-sized
depth/motion textures through the vendor NR interface. The older NVIDIA adapter
already describes separate color and guide subrectangles, but that is evidence
for an experiment, not proof that the current three supplied models accept them.

Nearest-neighbour enlargement is not the production approach: the independent
scene fixture demonstrates lost thin surfaces and mismatched depth edges.
A new full-resolution game guide pass would require a separate rendering
extension. If the direct smaller-guide experiment fails, keep that combination
unavailable and report its actual failure before selecting another approach.
Do not weaken the independent geometry oracle or relabel enlarged guides as
full-resolution geometry to manufacture a passing result.

Use explicit color/output extent and depth/motion extent throughout metadata,
allocations, copying, packet validation, creation/evaluation parameters and
resource recreation. For After, color equals display and guides equal render;
render dimensions must be positive and no larger than display. Keep both
dimensions in the preparation/recreation key, even when display size is fixed.
Preserve actual texture bounds and formats at each boundary.

Color/output subrectangles use display dimensions. Depth/motion subrectangles
use their actual render dimensions. Motion remains current-to-previous with
jitter excluded. Derive NR motion units from the declared source convention
and independently test normalized, render-pixel and output-pixel displacement;
do not multiply an already normalized/output-scaled vector twice. Depth
inversion remains explicit. Verify NR parameter types/flags with the pinned
runtime; a similarly named DLSS flag is not sufficient proof of NR semantics.

Every packet retains matching source ID, epoch and source/guide time, actual
resource ownership and producer completion. Spatial recovery, stale guides,
generated images, unsupported encoding and unavailable SR providers remain
ineligible for active NR. Unsupported inputs bypass through the existing safe
path with a clear reason; they must not be reported as an evaluated NR frame.

## Common stage and lifecycle

Reuse the existing runtime owner, retained slot ring and ticket/fence machinery.
Separate color and guide allocation extents within that machinery rather than
adding another NR owner or running NR in a presentation worker. Keep stable
process model identity and the existing injector/device ownership checks.
Internal Before-named bridge classes can be reused; renaming them is not required
to enforce the common pipeline.

The common evaluator invokes NR after SR and existing after-upscale source
effects, before either FG backend captures/uploads/tags its real source. UI
remains outside NR and is composed once per displayed image. Suppress any legacy
duplicate NR execution. Preserve ENB/ReShade ownership and existing effect order.

An NR result must never overwrite or become feedback for SR temporal history.
Prove separately that real presentation and the retained FG source use NR output,
including the FSR upload and NVIDIA tagged source. FG-off operation must not
initialize an otherwise unnecessary NVIDIA FG runtime.

Keep the accepted encoded SDR byte/full-tone contract, exact source alpha and
native UI. Do not introduce Stable colors or Tone=0 as drift workarounds.
FSR linear output must pass the established explicit SDR transfer path before
NR; no encoded/linear mislabelling or uninitialized output/background aliases.

Quality/provider/resolution changes retire all old producer, NR and FG readers
before reallocating either color or guides. Reset affected NR and FG histories
at the source boundary. Live tuning uses immutable per-source settings. Menus,
loading, camera cuts, minimize/restore, resize and unavailable providers keep
their admission/reset policies. Failed or uncertain retirement never permits
release or resource reuse; retain the established terminal handling.

## GPU profiles and driver qualification

Keep the supplied exact model catalog: `rtx20-30` for RTX 20/30, `rtx40` for RTX
40 and `rtx50` for RTX 50. Select using the actual renderer adapter ID and LUID.
Do not spoof the local RTX 4080 SUPER as a different family. Cross-running a
model on this card does not establish compatibility with its intended hardware.

Retain the existing narrowly scoped caller-identity compatibility policy for
the patched RTX 20/30/40 models. RTX 50 retains its SignedDirect policy. No
general signature bypass or unrelated DLL modification is introduced.

The current driver core is pinned to the tested local binary. Other machines
must not load an arbitrary core merely because its filename matches, nor receive
a copied local driver core in the package. The portable validator discovers the
selected adapter's installed driver/core and records its path, version, size,
SHA-256 and signature/provenance. A different core produces an explicit
unqualified result under the normal production policy.

An isolated qualification mode may evaluate a new core only after verifying
that it belongs to the selected installed NVIDIA driver, holding its immutable
file identity, validating its Windows signature/catalog provenance and required
exports, and checking the parameter ABI with typed Set/Get round trips. This
mode does not silently alter production trust or approve itself. Successful
Init/Create/Evaluate, image and lifecycle receipts are reviewed before adding
that exact core to the qualified production catalog. Failure at any boundary
is recorded with its native code and retained ownership state.

Build a portable validator from the existing production NR code and fixtures.
It selects the real adapter/profile and accepts explicit model/runtime paths;
it must not assume `rtx40`, this user's drive layout or a local DriverStore folder.
Validate NR on RTX 20 and RTX 30 separately despite their shared model. NVIDIA
FG qualification is requested only on eligible hardware; FSR FG is the separate
route for older cards. Do not turn unsupported NVIDIA FG into an NR failure.

## Evidence and supported status

Receipts bind source revision, executable hash, adapter IDs/LUID, driver/core
identity, model hash, SR/FG runtimes, color/guide extents, motion convention,
settings, return codes, readbacks and retirement results. Distinguish failure,
unavailable/skipped and passed cases; no successful exit from an unsupported
case counts as GPU output qualification.

For each actual GPU family, record software eligibility and hardware validation
separately. A profile may be software-enabled while its hardware run is NOT RUN.
Only passed output/lifecycle runs on actual cards establish the tested support
matrix, and coverage applies to those device/driver/model combinations. The
local card is RTX 4080 SUPER; no RTX 20/30/50 output claim is possible locally.
Unavailable external hardware remains an open qualification gate, not a reason
to invent a pass or disable an otherwise eligible catalog entry.

## Validation and delivery

Use deterministic failing regressions before implementation for unequal extents,
guide subrectangles, motion units, common ordering/legacy Before migration and
recreation when only render size changes. Preserve stale/foreign-resource and
generated-image rejection. Extend independent moving-scene tests to thin edges,
occlusion, camera movement and non-integral scale ratios; distinguish observed
quality from exact geometry preservation.

Actual local DLSS and FSR source fixtures must read back scaled SR and NR results,
prove the enhanced FG source, exact alpha/UI, FG-off behavior, off/on resets and
pending-reader retirement through scale/provider changes. Exercise representative
settings across all supported fixed modes; do not infer all ratios from Native AA.
Retest live Apply/style/tone and preserve the accepted color behavior.

After affected tests, run the appropriate clean NR-on/off build matrix and final
review, validate a separate trial package and preserve the existing native DLL,
INI and rollback. Reuse build/dependency trees and keep compact hashed receipts;
avoid redundant full runtime packages. Graphics Tools dependent exclusions remain
explicit rather than being counted as passes.

No GPU probes alongside Skyrim. No installed mod/INI/profile update with Skyrim
or MO2 open; no automatic game launch/termination. Migration changes only the
NR placement needed for the approved common order and preserves other settings.
Owner gameplay acceptance covers scaled DLSS and FSR with NR/FG off/on, camera
colors, HUD/inventory/map/dialogue, Apply, save/reload, fast travel and alt-tab/
minimize. Actual RTX 20/30/50 runs remain separately required.

Six extension milestones will be reported independently of the completed native
8/8 baseline: (1) vendor guide contract qualification; (2) retained scaled bridge
and common order/migration; (3) local scaled DLSS/FSR plus FG/lifecycle validation;
(4) portable per-profile validator and driver qualification policy; (5) clean
matrix, review and staged package; (6) Skyrim and actual per-family acceptance.
Report each completed milestone as N of 6; local work cannot close missing
hardware acceptance. Written-spec approval precedes the implementation plan.
