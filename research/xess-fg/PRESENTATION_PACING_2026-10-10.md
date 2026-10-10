# XeSS FG presentation pacing

Status: implementation under validation; Skyrim gameplay generation remains unqualified. Milestones: 3 of 8.

The user approved AIO19-style presentation pacing first, followed by FG qualification and latency measurement. This supersedes the native input handoff as a runtime admission requirement; its strict contract and regression coverage remain available.

## Evidence and design

The installed `000ccf57b3c7` run recorded temporal gameplay sources with FG requested, but `inputProof=false`, `frames=1`, zero ordered sources, and continuously increasing skipped cycles. Worker input jobs ran, yet their completion did not meet the after-Sleep admission window. The bounded worker/render snapshots are advisory and do not establish atomic frame pairing. The AIO19 scheduling witnesses separately show next-ID Sleep/SimulationStart after Present and render/Present markers without a physical worker input proof. See [the scheduling evidence](AIO19_XESS_SCHEDULING_2026-10-10.md).

The production host now explicitly selects `PresentationPacing`. After a successful genuine temporal world Present, it reserves the next exact source/epoch and executes XeLL Sleep/SimulationStart. The inspected render boundary supplies SimulationEnd/RenderStart for that reservation. Final completed scene publication must match the reserved source, epoch and timing owner, and the timing contract must be at RenderStart before tags are admitted. RenderEnd and PresentStart/End retain the same SDK ID. Source/epoch mismatch, missing render boundaries and foreign-thread publication keep real output.

Native worker timing remains diagnostic. No worker completion is manufactured, and no SDK calls run on worker threads. This schedule does **not** establish Sleep-before-engine-input or measure gameplay latency. Logs explicitly label presentation pacing and unqualified pre-input latency.

The production 128-consecutive-input-frame gate is removed. Existing history invalidation and one-source reset remain, along with guide validation, dedicated HUD, final-scene composition, minimize/restore ownership and GPU drain requirements. Loading/repeated/recovery/error output cannot invent fresh completed temporal sources. The strict `VerifiedInput` timing mode remains the default for existing callers and contract tests.

## Verification

The new CPU regression first failed at render admission without a worker input proof, then passed with the explicit pacing mode. It also covers source/epoch mismatch, foreign-thread rejection, interruption recovery and monotonic SDK IDs. The paced GPU host regression checks owned tags, reset versus subsequent generation admission, rejection of an invalid final source proof, and recovery after one history frame rather than a 128-frame streak. The public SDK double enables tagged reset frames to warm history; SetEnabled alone is not evidence of a generated frame.

Release build succeeded. Initial selected validation: 20/20 Intel FG/package tests passed, seven GPU-labelled tests. Independent read-only review found no actionable defects. The refreshed full suite and clean committed build receipt will be recorded separately; these checks do not qualify Skyrim gameplay.

## Next acceptance

Install a unique hash-verified candidate with the current INI, menu layout and MO2 launch settings preserved. In a loaded save, enable FG and inspect sustained `frames=2` gameplay output with matching source proof. Then check FG off/on, camera motion, HUD, pause/menu and minimize/restore. Measure latency separately after gameplay generation is confirmed.
