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
