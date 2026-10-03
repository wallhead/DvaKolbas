# AIO19 NR performance: independent validation, 2026-10-03

The initial static audit below retains its original measurement limits. The later **New analysis and gameplay scout** section updates the model A/B and gameplay evidence.

The supplied source checkpoint is accurate at Dva revision `27ad903`. The main AIO19 scheduling observations are supported by independent PE parsing and Capstone disassembly of the supplied archive. They identify credible optimization targets, **not a measured explanation of the user's approximately 15 FPS gap**. No AIO19 binary was executed, installed, or loaded into Dva during this audit.

## Inputs and reproduction

User-supplied documents are preserved unchanged under [supplied](supplied/). Their recommendations are review material, not additional authorization. The archive filename differs from the report's `(1)` filename, but SHA-256 and size match exactly. The bounded machine-readable [receipt](performance-validation.json) records identities, eight source blobs, six exports and 32 instruction witnesses, including raw bytes. It also lists what static inspection cannot establish.

With Python, pefile and Capstone available, extract only these members using 7-Zip into an ignored directory: `SKSE/Plugins/SkyrimUpscaler.dll`, `SKSE/Plugins/SkyrimUpscaler.ini`, `UpscalerBasePlugin/PDPerfPlugin.dll`, `UpscalerBasePlugin/nvngx_dlssnr.dll`. Run from the repository root:

```powershell
C:/Python314/python.exe tools/nr/Inspect-Aio19Performance.py --archive C:/Users/user/Downloads/SkyrimUpscalerAIOBuild19-Hotfix1.7z --extracted out/research/aio19/extracted --revision 27ad903 --documents research/aio19/supplied/DvaKolbas_current_codex_nr_perf_checkpoint_2026-10-03.md research/aio19/supplied/AIO19_NR_PERF_RE_v5_POLISHED.md --output out/research/aio19/reproduced-validation.json
```

The inspector fails on an artifact, source blob, export or witness mismatch. Witness windows are short instruction sequences, not guessed function boundaries. In particular, unwind entries split the slot selector at `0x75bd0` and chain worker at `0xa632f`; following those continuations was necessary to validate the reported paths. Do not reuse RVAs on another build.

## Findings

| Claim | Audit result | Evidence / limit |
| --- | --- | --- |
| The archive and three reported DLL identities match | Confirmed | Archive SHA `49e7f7da…`; host `ac699b58…`; PD `ff6c5839…`; NR `8270b350…`. Full hashes/sizes in the receipt. |
| The checkpoint still describes current Before code | Confirmed | All eight blobs match; exactly four commits from `de4662a` to `27ad903`; core blobs unchanged. |
| AIO19 uses the same NR DLL as AIO18 | Confirmed against retained AIO18 inventories | `8270b350…` matches `research/aio18/local-analysis/aio-build18-inventory.json`. This rules out a newer model DLL between those archives, not all runtime/configuration differences. |
| The NR DLL matches Dva's RTX40 runtime | False; report correctly distinguishes them | Dva pin `e67dee20…` differs from AIO's `8270b350…`. No same-host runtime A/B performed. |
| Chain ABI is version 1, size `0x598`, pass count 1–10, table `+0x17c`, stride `0x68` | Confirmed selected fields | Export checks and validator at `0x10f910`, wrapper at `0xa5fd0`. Other semantic field names are recovered interpretations, not original symbols. |
| The local chain reaches the mapped producer/consumer path | Confirmed conditionally | `0x1110b7 -> 0xa5fd0 -> 0xa62e0`; worker's enabled local branch at `0xa6557 -> 0xa5360`. This is a reachable route, not proof of the user's active AIO route. |
| Local producer dependency is GPU ordered after one Flush | Confirmed mapped fence branch | `0xa5448` context Signal; `0xa5454` Flush; `0xa5479` queue Wait. Jump at `0xa547c` skips fallback Flush at `0xa5484`; do not count both branches as two normal Flushes. |
| Local result delivery uses GPU ordering then a D3D11 copy | Confirmed mapped branch | Submit at `0xa57e0`, context Wait method/call at `0xa5800/0xa580d`, regional/full copy at `0xa5852/0xa5899`. Final copy remains. |
| Eight persistent command-work slots are scanned before backpressure | Confirmed | Selector checks completion and `cmp esi,8` at `0x75c33`; begin at `0x75690`; submit stores target by slot at `0x75813`. Eight slots do not prove eight full image sets or eight outstanding frames. |
| Saturation can block the CPU indefinitely | Confirmed | `0x75c72 -> 0x70540`; event registration and `INFINITE` at `0x705f7/0x705fa`; imported `WaitForSingleObject` at `0x70602`; device-removal helper at `0x702e0`. Device checks around an infinite wait do not guarantee a wakeup if the event never arrives. |
| Status querying is separate from chain submission | Confirmed public surface and lock | Status export validates its own packet and uses lock `backend+0x31d0`, snapshots `+0x3220`. This does not prove it measures physical GPU completion or is lock-free. |
| Separate NR and FSR queues explain the gap | Not established | Separate fields/domains can share actual COM objects. Dva's separate DIRECT queue is not itself a defect; queue unification is not a prerequisite. |
| AIO performs no per-frame allocation or CPU waits anywhere | Not established | Negative observations apply only to mapped regions. Vendor calls, cache misses, wrappers, other branches and actual wait frequency require instrumentation. |
| Scheduling is the main cause of the 15 FPS deficit | Hypothesis | Report section 32 is more categorical than its evidence. Static structure cannot assign milliseconds or rank runtime/model/configuration contributions quantitatively. |

## Dva source costs confirmed

- [BeforeUpscale.cpp](../../src/NeuralRendering/BeforeUpscale.cpp): producer signal through interop, additional handoff signal/Flush and CPU completion check; then copy-back reader/completion checks, `Drain()` and `RetireTicket()` before returning.
- [ImagePacket.cpp](../../src/NeuralRendering/ImagePacket.cpp): strict validator requires the producer fence already completed. Simply deleting this guard would remove an ownership guarantee; a separately verified queued-dependency path is required.
- [Stage.cpp](../../src/NeuralRendering/Stage.cpp): one pending ticket, per-record descriptor heap and full-image alpha restoration dispatch. Three interop allocators do not create three retained color/guide/output/parameter sets.
- [PreparedBeforeUpscale.cpp](../../src/NeuralRendering/PreparedBeforeUpscale.cpp): decode to linear FP16, guide preparation, NR bridge, encode back to the source, then D3D11 event query/Flush/poll before return. A single prepared resource set prevents safe overlapping reuse.
- [FSRFrameAdapter.cpp](../../src/Upscaling/FSRFrameAdapter.cpp): subsequently decodes the source to linear and prepares guides again. Current Gamma22 source therefore passes decode → NR → encode → FSR decode, with intermediate quantization where the game target is UNORM8. A direct prepared handoff needs verified resource and reader ownership, not just a changed encoding flag.
- [D3D11D3D12Interop.cpp](../../src/Graphics/D3D11D3D12Interop.cpp): GPU waits coexist with CPU allocator/Drain waits; `Drain()` can add consumer signaling and Flushes. A wait helper first checks completion, so invocation counts are not actual OS-block counts or additive measured stall time.

These source costs exist, but dependent NR → SR GPU work must still execute in order. Removing CPU waits cannot eliminate model work or guarantee concurrency between dependent passes.

## Benchmark evidence and necessary controls

The report's historical `55.98 / 33.48 / 35.39 FPS` arithmetic corresponds to approximately `17.86 / 29.87 / 28.26 ms`, an NR-on increment around `10.4–12.0 ms`. No matching independent measurement receipt was found in the retained `research/` or `out/research/` text/JSON/log records during this audit. The committed first-gameplay [smoke receipt](../nr/runtime-catalog/skyrim-before-smoke-7eb33a7.json) explicitly says measured performance was not established. Keep the reported numbers unverified until the original capture and methodology are available; do not call them false, pure synchronization cost, or an AIO-vs-Dva delta.

The user's approximately 15 FPS deficit is a reported observation. Compare source-frame milliseconds with FG off first, then generated presentation cadence separately with FG on. FPS differences alone depend on the starting rate.

The shipped AIO19 INI has NR disabled, one chain pass, preset/style 0, tone 1 and `mDLSSNRBeforeUpscaling=false`. Its comment explicitly says input resolution scale `0` **or** `>=1` means no downsampling; `0` does not prove automatic reduced-resolution NR. Per-pass settings can supersede legacy shared values. Capture effective placement, chain tuning, resolve mode and actual color/guide/model extents instead of benchmarking default INI text against Dva's active Before native trial.

For the current Dva trial keep local tone 0: the camera-related color issue is mitigated, with its model/color/exposure root cause open. Match that setting on AIO, plus source resolution, SR provider/version/quality, sharpening, reset cadence, HDR, ENB/ReShade/UI path and single-pass settings. Qualify the different model in an isolated same-host probe; do not replace the shipping catalog pin as a shortcut.

## Plan ruling

Prioritize a measured Before performance gate inside milestone 3: timing and matched/runtime baselines, queue-proven producer readiness, retained multi-ticket/resource slots, deferred final readers, then removal of redundant FSR conversions/copies and measured alpha/descriptor costs. Preserve teardown/device/proxy identity safeguards. The detailed [performance plan](../../docs/superpowers/plans/2026-10-03-nr-performance.md) precedes further shipping After work; post-FG feasibility research remains independently useful.

No game settings or plugin code changed during this audit. Overall NR completion remains **1 of 8**. Performance implementation, matched game benchmarking, true After FG and other GPU-family qualification remain unfinished.

## New analysis and gameplay scout

The newly supplied [performance analysis](supplied/DvaKolbas_vs_AIO19_NR_Performance_Analysis_2026-10-03.md) reviews the older `de4662a` revision, which exists locally. Its recommendations are proposals to validate. The shipping path still has the producer handoff, per-source Drain, final query polling, descriptor creation, alpha restoration and NR-to-FSR color roundtrip described. Optional instrumentation added since that revision does not remove those operations.

Fresh reproduction passed all four artifact identities, eight historical source blobs, six exports and 32 instruction witnesses using the existing inspector. The archive without `(1)` has exactly the reported SHA-256. Additional [bounded witnesses](new-performance-review-witnesses.json) confirm the four contiguous method pointers, SubmitResult RTTI strings, indirect calls at the two reported Skyrim RVAs, and `WaitForSingleObject` at PD `0x8bb24`. No direct call to that import appears in the fully decoded main unwind fragment `0x89c0..0x8b95`. This narrow negative does not establish transitive async behavior. The method sequence alone does not establish named vtable offsets; the two indirect calls alone do not prove their live binding to the chain export.

The [standalone baseline](../nr/performance/baseline.json) already fulfills the proposed exact-hash model A/B for the synthetic native Before transaction. Current-model alternating medians were 10.01/10.01/10.15 ms; AIO-model medians 10.00/10.21/10.19 ms. Vendor GPU medians span 6.80–7.15 ms across clean runs, with drift in both models. No material model advantage is isolated in this scene. This does not establish equivalence for all gameplay, histories or presets. Shipping runtime pins remain unchanged.

All four new raw PresentMon CSV hashes/counts match their collection receipts, each has one complete stream, and independently calculated timestamp cadence agrees within 0.05 FPS. After trimming two seconds from each end, 11,423 records remain from 12,234 raw records. The [scout receipt](../nr/performance/skyrim-scout-2026-10-03.json) retains settings, binary/log hashes, state evidence, timing statistics and limitations.

| Host | NR off mean app FPS | NR on mean app FPS | Off → on mean frame time | NR-on p95 / p99 |
| --- | ---: | ---: | ---: | ---: |
| AIO19 | 60.105 | 51.333 | 16.638 → 19.481 ms | 20.274 / 21.233 ms |
| DvaKolbas | 58.124 | 34.641 | 17.205 → 28.867 ms | 30.344 / 31.198 ms |

The NR-on gap is **16.692 FPS / 9.387 ms**. Within-host mean increments are 2.843 ms and 11.663 ms; their difference is 8.820 ms. These are measurements of **unmatched configurations**, not isolated NR inference or host overhead. Saved settings/logs identify AIO DLAA with late NR, tone/style 1 versus Dva FSR 3.1.5 Native AA with Before NR, tone/style 0; saved ReShade ordering also differs. Both are native 2560×1440 and one pass. AIO scale 0 means no downsampling. Dva's saved FG=true is superseded by its logged session-only FG-off transition before capture; AIO saved FG=false and FrameWarp-off log support the source-rate interpretation, but no optical/generated-frame classification was performed.

One 60-second run per state and scene label `river` do not verify identical weather, view, caps or clocks. CSV wall time is three hours ahead of the Moscow collector/game windows; use relative intervals and collector UTC for correlation. Valid display latency on every selected row does not prove absence of optical drops. The new Dva +11.663 ms increment agrees in scale with the historical smoke numbers, but does not authenticate those old source-ID records; their original receipt remains unavailable.

### Updated ruling

Host scheduling remains a credible priority, with its quantitative share open. Do not inherit the report's “very high” causal confidence or equate separate queues with a defect. Synthetic producer waits (~0.68–0.72 ms), final polling (~1.43–1.60 ms), six Flushes and one descriptor heap/source are measured. Consumer blocking overlaps dependent GPU work and cannot be added to vendor time. Alpha (~0.04 ms) and final encode (~0.02 ms) are low priorities there; FSR preparation/dispatch remains unmeasured.

Finish the FSR timing slice and a matched repeated gameplay baseline. Keep the bounded order: queue-proven producer admission → retained resource/ticket slots → deferred final readers → direct prepared FSR handoff. Queue waits must establish real dependencies; slot reuse must follow every genuine reader. Preserve lifecycle drains and alpha until correctness evidence permits an alternative. No game/MO2 settings, installation or plugin behavior changed in this review. Main NR progress remains **1 of 8**; P0 remains partial.
