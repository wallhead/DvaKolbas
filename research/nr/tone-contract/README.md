# Local Tone camera-drift report: independent validation, 2026-10-04

The supplied report correctly identifies a change introduced in the NR path before SR/FG, and correctly treats Local Tone 0 as a mitigation rather than a discovered root-cause fix. Its exposure/Backbuffer ranking is hypothetical. Fresh standalone experiments do **not** reproduce an improvement from either proposed change. No shipping code, installed DLL, INI or MO2 setting was changed for this investigation. The drift fix remains open.

The user's latest requirement supersedes the earlier color-preservation policy: keep the styles' visible NR effect while stabilizing stationary materials during camera movement. **Stable colors is rejected as the solution** because it removes broad tone and medium structure. It remains disabled in the inspected trial INI. No deterministic replacement tone shader or temporal correction subsystem has been implemented or approved.

The later [targeted Backbuffer/runtime RE audit](BACKBUFFER_RE_VALIDATION.md) adds the previously missing output-initialization combination and direct effective-control observations. Its 32 initialization/Backbuffer runs and eight debugged runs also reproduce drift; use that update for the current next step.

## Report claims checked

| Claim | Finding |
| --- | --- |
| The shift occurs before SR/FG and stops with NR off | Supported by the retained Skyrim investigation and user comparisons. This locates the change within NR/preparation; it does not identify the model input error. |
| Local Tone 0 removes the reported effect | User-confirmed for that Skyrim scene. In the new context fixture, with structure/skin still at 1, Tone 0 reduces but does not eliminate drift. Do not generalize either observation to every scene. |
| Creation flags are `0x42`, with AutoExposure `0x40` and MVLowRes `0x02` | Confirmed in `RuntimeParameters.h` and the local pinned NGX SDK enumeration. Setting a generic flag does not prove this NR runtime uses it to estimate exposure. |
| Missing Backbuffer is the likely cause | Not established. Recovered AIO18 route code can fall back to Output, which is different from the report's proposed pre-NR copy. Both are experiments, not a proven required NR reference contract. |
| Gamma22 is a verified Skyrim/ENB transfer | Not established. The adapter explicitly uses that configured transfer; the producer's actual transfer still needs verification. |
| Clamp all strengths to 1 | Not justified by this defect. Test strengths were exactly 1; product sanitization remains unchanged. |
| Start with arbitrary motion sign/scale changes or expose an unproven exposure texture | No supporting evidence. Neither was changed. |

## Fresh controlled experiment

The throwaway derivative of the existing `ContextProbe.cpp` uses the real BeforeHost, preparation, shared stage and pinned vendor runtime. Each process records 160 sources at 640×360 with an unchanged textured central surface. Only the surroundings change: dark, bright, blue, dark, 40 sources per phase. ROI is `[220,420) × [120,240)`; statistics use the last 20 sources of each phase. Source ROI bytes are independently checked identical in all four saved captures. Depth is 0.5 and motion is zero. This tests context sensitivity without camera reprojection; it is **not** a Skyrim camera-motion acceptance test.

Preset 0, intensity/structure/skin 1, auto skin/UI correction off, Stable colors off, styles 0/1/4. Tone is 1 except the named Tone-0 controls. Every run checks all-pixel alpha, 160 evaluations, one initial reset and successful retirement. The independent receipt checks sequential source IDs, finite statistics, fixed source means and image hashes. It does not replace lifecycle/reader regression tests.

The final repeated run contains **27 processes / 4,320 NR evaluations**: 12 exposure/Backbuffer cases, 6 alternative-transfer cases, 3 Tone-0 controls and 6 exact-model comparisons. Readback is confined to the research executable. It refuses to run alongside Skyrim; no game was launched.

| Style | Gamma22, all four A–D cases | sRGB | Encoded values, conversion bypassed | Gamma22, Tone 0 |
| --- | ---: | ---: | ---: | ---: |
| 0 | 22.5525 | 19.2194 | 9.0486 | 11.8671 |
| 1 | 21.8355 | 17.5616 | 6.9151 | 11.5527 |
| 4 | 29.9268 | 21.0790 | 7.2887 | 12.5277 |

Values are maximum settled ROI **RGB-channel drift in 8-bit code values**, not physical luminance or a complete per-pixel temporal quality metric. CSV rounding accounts for the tiny difference from executable summaries. All alternatives exceed the existing context regression's 1-code-value threshold.

For each style, A (`0x42`, null), B (`0x02`, null), C (`0x42`, reference), D (`0x02`, reference) have **identical 160-row CSVs and all four saved full RGB images**. The reference is a separate retained per-slot FP16 texture copied from pre-NR Color, with COPY_SOURCE/COPY_DEST transitions restored to shader-read state and full-size subrects. It does not alias Color or Output. Flags are read back from the actual creation parameter object; Backbuffer identity is read back from every evaluation parameter object. These checks establish the requested bindings, not that the vendor consumes them. No debug-layer qualification is claimed for this derivative.

The original AIO19 NR DLL and supplied RTX40 DLL also produce identical captured RGB/CSV output in this host for all three styles. Pins differ (`8270b350…` versus `e67dee20…`), but selecting the original binary alone does not fix this fixture. The original uses the already-qualified research caller-identity shim profile. This does **not** test AIO19's own host/input/resolve pipeline.

The encoded-input diagnostic intentionally labels encoded UNORM values as Linear to bypass decode/encode. It is a **research contract violation**, not a qualified shipping color mode. Its lower drift neither eliminates the defect nor proves that Skyrim or AIO19 expects encoded NR input. Do not promote it based only on the smaller number.

## Bounded AIO19 shader inspection

Read-only disassembly of 192 embedded DXBC containers from the exact AIO19 host/PD binaries found identifiable NR shaders. `shader-witnesses.json` retains six selected container/disassembly hashes, offsets, debug-name strings and short instruction witnesses. The embedded `blit_cs` loads and stores color without explicit transfer arithmetic; `nr_box_downsample_cs` samples and averages source color without explicit gamma log/exp instructions. `nr_ratio_cs` contains sRGB conversion constants and log/exp operations. This is stronger evidence than inferring behavior from CPU parameter names, but **does not bind any shader to the user's active native Before route or establish its resource view format**. The native Auto path and source view/format still need tracing. Do not infer a proven encoded-input NR contract from the presence of a plain blit shader.

## Updated investigation order

1. Compare the same Skyrim surface in actual AIO19 and Dva with Tone 1 and Stable colors off. Current renderer differences must be controlled: source transfer, SR/sharpening, exposure and ReShade ordering. The saved Dva style was changed to 0 by gameplay after the earlier Style-1 matching receipt; re-check effective settings before calling a new comparison matched.
2. Recover/measure the actual AIO19 pre-NR color preparation and post-NR resolve. The supplied pre-NR reference hypothesis is not a substitute for proving that contract. Retain same-surface input and output, with valid motion/depth rejection, so lighting changes do not masquerade as added NR drift.
3. Use an in-game exposure/Backbuffer A–D build only if a game-specific difference remains after the above controls. The standalone null result lowers their priority; it does not universally rule them out.
4. If equivalent inputs expose the same vendor context sensitivity, design a style-preserving correction separately. It must retain style differences and visible tone strength, reject disocclusions and real lighting changes, reset on settings/cuts, and avoid ghosting. Smoothing that merely delays the eventual drift is insufficient. Neither the rejected broad-delta subtraction nor Tone 0 completes this requirement.

The supplied report's deterministic local-tone replacement is an unapproved fallback, not the implementation plan. Whole-game color/ghosting/performance acceptance remains pending. No NR milestone advances from these reproductions.

## Evidence and repeatability

The unchanged supplied report is preserved under `supplied/`. `validation.json` retains pins, generated-source/executable hashes, each capture's hashes/configuration/means and scope limits. The earlier model comparison receipt describes its earlier derivative source version; the final matrix and model comparison were repeated together with the final hashed executable.

Local throwaway sources and captures remain under `out/research/nr/tone-preserve-investigation/`; they are not shipping options. From this worktree, reconfigure its `probe` CMake directory into `out/build/nr-tone-model-investigation`, using the existing cache's pinned SDK include path; build `NrToneModelProbe`, run `RunToneMatrix.ps1`, then `AuditToneMatrix.py`. The script uses this machine's pinned driver-core path and fixture. Reproduction requires the locally supplied proprietary models; they are not in Git. The JSON is the durable review receipt; the local experiment source is inspectable but is not a portable published harness.
