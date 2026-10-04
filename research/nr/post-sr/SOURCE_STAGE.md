# Post-SR source stage checkpoint, 2026-10-04

Owner ordering: `input -> DLSS/DLAA/FSR -> NR -> optional FG -> UI/final`.
The revised After pass consumes real sources only. Initial qualification is
native-size DLAA/FSR Native AA; reduced guides remain explicitly unavailable.

Task 1 is bounded qualification: 29 metadata assertions, five independent
geometry assertions, and 48 GPU preparation sources /116,736 pixels passed.
Nearest reduced guides fail at thin surfaces (4 mismatches) and unaligned edges
(8 mismatches); these negative cases are retained rather than promoted to support.
The initial compiled metadata stub failed 24/29 assertions before implementation.

Task 3 adapter proof: 240 native sources, 239 NR evaluations, one unchanged off
source, 13,824,000 exact alpha values and first/re-enable history resets. Actual
NR output changes RGB with full Tone 1 and Stable colors off. The initial adapter
delegating to the Before-only bridge failed After placement and metadata binding.
Working Before/prepared FSR regressions passed ten affected checks.

The host switches After -> Before -> After with an extent change and recovers
from rejected reduced guides. A genuine D3D11 preparation/encoder dependency is
held by a shared GPU fence, confirmed pending, and released independently after
300 ms; the next placement switch waits at least 100 ms. That source deliberately
skips Map, which would otherwise drain the pending work. Both native devices and
the actual supplied ReShade wrapper pass. External FG reader integration and
pending-reader resize/minimize/Apply in Skyrim remain open.

Provider wiring runs after completed source effects, before source generation
preparation, with FG off too. Spatial/recovery paths only drain NR. NVIDIA tags
the enhanced output; FSR copies it into the cached scene target that Present
uploads. Legacy NR remains disabled under the community owner.

Actual pinned FSR Native AA -> NR -> FG source upload passed 240 temporal sources,
239 NR evaluations, 13,824,000 exact alpha values, 13,764,970 changed RGB pixels,
57,600 unchanged off pixels, and 55,296,000 exact uploaded source bytes. HUD and
retained SR output were unchanged for all 240 sources; transport/readers/owner
retired. This does not run combined FG generation/presentation or Skyrim.
NVIDIA source/tag tests use real D3D11 copies with scripted vendor operations;
actual NVIDIA SR/NR/FG chaining remains a game/provider gate.

Live settings expose Before/After upscaling (before FG), explicitly rejecting
scaled After requests. Settings RED caught the missing native-size restriction.
The independent review found one FSR-off compile guard error, now guarded. That variant also exposed a pre-existing Windows max macro conflict in motion diagnostics; the macro-safe call fixes it, and the NR-enabled/FSR-disabled DLL build passes. The review found
no other confirmed correctness blocker. Its qualification limits are retained.

The final clean-source product suite passed **196/196 checks** (42 GPU, four
ReShade), excluding NativeUIComposition, NativeUIBlendState and
NeuralPeripheralPixels because Graphics Tools is unavailable. All three DLL
variants built: NR/FSR/FG on, NR off with FSR/FG on, and NR on with FSR/FG off.
These builds use retained dependency/build caches and clean Git source dc6a9abe3630;
they are not empty-checkout builds. See the [build/install receipt](clean-build-install.json).

The After trial is installed in the existing V5.4 NO-LORE Dva mod. Only the DLL
and NRBeforeUpscaling=false changed. Working files are backed up; DLAA, NVIDIA
FG, Style 0, Tone 1, Stable colors off, remaining mod files and MO2 settings were
verified preserved. Skyrim has not been launched for this After trial. No
Before/After or AIO performance parity claim is made.

First Skyrim trial: the owner reports "Seems working". The [running-game
receipt](skyrim-dlaa-after-game-result.json) observes native DLAA After NR,
NVIDIA x2 output while After is selected, live Before/After and NR off/on.
No error/critical entries occur in the captured prefix. This closes the initial
NVIDIA execution observation gate on RTX 4080 SUPER, with broader quality,
lifecycle, physical cadence and performance checks still pending. Latest logged
placement is Before. This capture changes no game/INI/MO2 files; count stays 3/8.

Next FSR qualification: [combined presentation](fsr-combined-presentation.json)
passes automatic and observer modes, 240 actual Native AA sources /239 NR
evaluations each, 197 actual generation callbacks each, and independent
off/menu/re-enable/suspend-resume checks. Observer mode retains 195 generated
images (194 changed), 238 real images, 456,960 exact enhanced-real RGB samples
and 1,108,480 UI blend/alpha samples. The deliberately unenhanced actual
Presenter source fails the RGB oracle. Portable placement alignment failed
before copying, then passed with 512-byte row spacing. Three material fixture
review findings are fixed; 14 affected regressions pass. Graphics debug queues
are unavailable, not counted as passing. This extends the older upload-only
checkpoint: automatic UI appearance, physical cadence, resize and Skyrim FSR
remain open. Installed dc6a9abe3630 DLL/INI and MO2 settings are unchanged.

First Skyrim FSR trial: owner reports "seems fine". The [running-game
receipt](skyrim-fsr-after-game-result.json) confirms Native AA After NR,
successful FSR generation callbacks while After is selected, live placement
and Style0/2 changes, and generation resumption. No error/critical log entries
occur. Last sampled NR count is cumulative across placements, not After-only.
Full toggles/UI/lifecycle acceptance and performance remain pending; count3/8.

Latest qualification: the owner subsequently passed NR/FG off/on, save/reload,
inventory/map, alt-tab and minimize/restore for FSR After. The [renderer lifecycle
receipt](fsr-pending-lifecycle.json) adds genuine pending FG queue/public SDK
reader retirement and same-chain resize: 192 sources/189 NR evaluations per
mode at 320x180 -> 384x216 -> 320x180, six delayed boundaries, 18 independently
checked recovery transitions and no SDK warnings/errors. Observer mode verifies
all mandatory real/generated source identities, 380,928 enhanced-real RGB
channels and 909,312 UI blend/alpha channels. All 12,681,216 source alpha values
are preserved; bypass and retained temporal SR/UI resources are unchanged.
Only the two deliberately queued boundary images per epoch may be detached
before their public Present callback. They still require real work retirement;
the completion marker is not a private AMD worker/generated-image fence.
Missing real/generated observations were deliberately omitted, causing six
coverage failures, then restored. Fresh follow-up review reports no remaining
important blocker. Twenty-four affected regressions passed before the narrow
coverage fix; both changed lifecycle tests were rebuilt and passed afterward.
Automatic mode proves execution, not compositor pixels. Graphics debug queues,
physical cadence, broader hardware/scene and matched performance remain
unqualified. This completes Task4 for native-size SDR After: **4 of 8**.
No installed DLL, INI or MO2 settings changed during this test/document step.

NVIDIA follow-up: the [source handoff receipt](nvidia-source-handoff.json) records
128 actual production DLAA sources and 126 RTX40 NR evaluations, two native
extents, NR off/on and FG configured off/on/off. Every enhanced HUD-less tag byte
is read back through the production D3D11/D3D12 interop and Session. All source
alpha pixels, frozen temporal SR input and independent UI are preserved. A
deliberately unenhanced tag fails the exact-byte oracle. Public Streamline API
callbacks are scripted; this does not prove vendor FG output, presentation,
physical cadence or game UI composition. Synthetic guides have static depth,
zero motion and zero jitter; moving-scene quality is a separate game check.

The probe's exception lifetime guard was corrected after review. Confirmed
retirement releases its capsule; refused/throwing retirement permanently retains
submitted sources/tags/readback/device/queue/runtime owner without retry. The
CPU negative control fails four assertions, and final CPU/GPU checks pass.
Process-owned D3D11 NGX subsystem/capabilities are not fully shut down by this
fixture. The earlier full suite passed 201 checks; only the two affected tests
were rerun after the fixture fix. Debug queues remain unavailable.

The [installed NVIDIA trial](nvidia-pre-game-install.json) changes only the two
provider INI keys to DLAA/backend1, retaining After, FG enabled, full Tone 1,
Style 0, Stable colors off and native UI. The clean dc6a9abe3630 DLL and MO2
settings are unchanged; accepted FSR INI is backed up. Fuller NVIDIA Skyrim
acceptance is next. Task5 stays open and the count remains **4 of 8**.

The owner subsequently [passes the listed NVIDIA checklist](nvidia-after-gameplay-checklist.json).
Available local logs still describe the prior FSR run; no new NVIDIA runtime
telemetry is attributed to that report. Accept the listed manual steps separately.

The [pending live-settings source fixture](live-settings-pending-source.json)
extends shared BeforeHost coverage through five genuine pending boundaries in
both native and actual ReShade modes. Tone, style, NR off/on and placement changes
preserve one evaluation per eligible source, alpha, caller RTV and exact bypass;
style/off/placement wait for actual old source completion. A coverage negative
fails. A test-harness reset assumption was corrected to match the production
caller; production code is unchanged. Both modes and all 18 related regressions
pass. Original and resized source textures are retained on uncertain retirement,
and the independent gate release starts before GPU submission so exceptions
cannot leave an unstarted release helper. This does not prove End UI dispatch,
actual vendor FG reader retirement, provider creation changes or failure stress.
Task6 stays partial; installed DLL/INI and MO2 settings are unchanged. **4 of 8**.

The [NVIDIA public-reader contract probe](nvidia-pending-readers.json) extends
actual DLAA/NR tagged-source coverage with an independent same-device DIRECT
queue copying the real enhanced HUD-less texture. Four captures at two native
extents preserve all 1,124,352 old-image bytes while production Session/Interop
orders producer reuse and retirement against the returned real completion
fence. CPU releases only the test gate; reader completion is signalled by GPU
after the actual texture copy. The explicit omitted-wait control fails two
producer-wait checks and the old-image oracle (747,292/1,124,352 exact bytes).
The initial test accidentally drained the reader with a late HUD readback;
eight pending/wait assertions failed, and the observation was moved before
submission. No production defect or change is claimed.

Three scripted public API failure cases reject a missing/nonzero-null fence,
state-query error or owner-retirement refusal before any subsequent producer
token/API reuse. Failed Stop leaves the submission capsule, independent reader
and NR owner quarantined without retry. Actual independent copy completion is
then observed for safe shutdown; it neither clears the fault nor releases the
capsule. Full vendor/NGX teardown is not claimed. Eleven affected regressions
pass. Public Streamline responses are scripted and there is no actual vendor
FG output/presentation/cadence evidence in this fixture. Task5/6 remain partial,
and installed DLL/INI/MO2 settings are unchanged. **4 of 8**.
