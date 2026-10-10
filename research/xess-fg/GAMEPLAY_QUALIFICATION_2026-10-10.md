# XeSS FG: first Skyrim gameplay qualification

Progress: **5 of 8 milestones done** for the user-approved presentation-pacing scope. Completed milestones: evidence/design, runtime/device probe, source/SDK timing contract, standalone presentation and the first Skyrim route. NR/provider combinations, the full lifecycle matrix and release readiness remain open. Pre-input latency is deferred and is not qualified.

## Qualified route

RTX 4080 SUPER, V5.4 NO-LORE, clean DLL `eae31179f682`: FSR 3.1.5 Native at 2560×1440, Gamma22 source, XeSS FG presentation, NR off and mod HDR off. The user reported successful gameplay and then “all good” after being asked to toggle FG off/on twice and check inventory/map and minimize/restore for stable HUD and resumed FG.

The running log identifies the expected installed DLL. After the loading/fader screens closed, source 9021 enabled generation. Periodic completed temporal gameplay sources then report `sourceProof=true`, `requested=true`, `frames=2`, `sdk=0`, successful Present and an active accepted-source reason. This is actual Intel-reported two-frame output, not a requested FPS multiplier or a menu-only result.

The log contains two explicit `active=false requested=false` transitions, followed by successful active states. A loading transition at source 20636 used real-only output on the loading thread; source 21167 resumed generation on the render owner. Subsequent sources 21600 through 25800 again reported successful two-frame output. One interrupted untagged cycle was skipped during recovery; it did not cause a persistent warmup or terminal failure. No error/critical lines were present in the bounded snapshot.

Inventory/map and minimize/restore visual behavior are user-confirmed. The periodic log retained `drainSuspends=0`; this capture does not independently demonstrate the GPU-drain minimize path or teardown. Dedicated standalone fence/lifecycle tests provide separate coverage. No numeric graph reading, end-to-end latency or generated-frame image capture was supplied in this gameplay check.

## Timing scope

The user explicitly chose AIO19-style presentation pacing first and latency qualification afterward. The validated hook locations supply the actual render boundary; post-Present XeLL reservation, markers, final scene identity, resource tags and Present share checked IDs. Native input remains diagnostic. This completes milestone 3's frame/marker policy under that approved scope, and milestone 5's first working Skyrim route; it does not establish Sleep-before-engine-input.

See [the pacing design and limits](PRESENTATION_PACING_2026-10-10.md) and [the build validation receipt](presentation-pacing-validation-2026-10-10.json). The five broader-suite failures remain recorded and are not erased by this gameplay result.

## Remaining checks

- Enable NR After on this route; check camera/HUD and live NR/style/pass toggles.
- Qualify DLAA/DLSS and XeSS SR with Intel FG, then scaled guide/image behavior.
- Complete dialogue, save/reload, fast travel, the remaining focus/lifecycle checks and shutdown evidence; keep user-confirmed checks distinct from independently logged observations.
- Measure latency and confirm the output graph separately, then prepare a release package for the qualified scope. Other physical GPU vendors remain unqualified for Intel FG.
