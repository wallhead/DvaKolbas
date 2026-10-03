# Post-P4 performance report validation — 2026-10-04

Reviewed `DvaKolbas_NR_Performance_Report_for_Codex_2026-10-03.md` from the user's Downloads folder against branch `codex/nr` at `797a770c64c51f292b2e603f8bffd92b7d9caf3c`. Report SHA-256: `6d3a6a61cfd8fb3239d51eb174f76e00fbc5c1c8740ae72f07e0a8f1e04e41dc`. The document is review material; its implementation instructions do not independently authorize code or game-setting changes.

The architectural findings are substantially supported. They identify optimization opportunities, not demonstrated remaining Skyrim FPS losses. The working candidate and P1–P4 correctness contracts are preserved. This review changes documentation only; no binaries, INI, MO2 settings or GPU fixtures were changed/run.

## Numerical verification

Re-ran `tools/nr/Compare-Performance.py --require-clean` over all five raw clean P5 timing receipts. All raw hashes and recomputed summaries exactly match [p5-comparison.json](p5-comparison.json); every case has zero validation issues. The fresh local reconstruction is `out/research/nr/performance/review-2026-10-04/p5-recomputed.json`. Each case has 300 measured sources after 120 warmup sources, native 2560×1440 input and output, tone/style 0, clean source `0a0be58`, and no dropped timing records. Instrumentation-off is a wall-time control, not a source of GPU phase measurements.

| Metric | Verified medians, ms |
|---|---:|
| NR-on enqueue transaction, three repeats | 8.525400 / 8.545550 / 8.560600 |
| NR-off transaction, one control | 1.012700 |
| Instrumentation-off NR-on, one control | 8.563900 |
| Vendor NR GPU interval | 6.920192 / 6.946816 / 6.955008 |
| FSR dispatch GPU interval | 0.635904 / 0.636928 / 0.637952 |
| Alpha restoration GPU interval | 0.040960 in all three |
| Bridge input copies GPU interval | 0.066144 / 0.066176 / 0.066272 |
| FSR prepared-input GPU preparation | 0.086016 in all three |
| Prepared slot-pressure blocking | 7.540950 / 7.567400 / 7.582750 |

NR-on wall p95 is 8.692630–8.743355 ms and p99 is **8.771703–8.838468 ms**. Measured steady counters are four Flushes and zero descriptor creations per source. The synthetic producer saturates retained slots: its blocked CPU interval overlaps dependent GPU work. These phase medians cannot be summed into recoverable time or converted into an expected Skyrim FPS gain. The NR-off and instrumentation controls have only one repeat each.

## Findings against source

| Report finding | Verdict | Evidence and qualification |
|---|---|---|
| A: duplicate producer queue dependency | Confirmed by source trace | `BeforeUpscale::Initialize` passes the same `contract.queue` to interop and Stage. `Evaluate` signals the producer, exports its fence/value, calls interop `Begin`, then `Stage::RecordQueued`. `ProducerDependency` exports `work->fence12` and `work->value`; `Begin` immediately passes those to `WaitD3D12`; no producer value change occurs before Stage waits on that same exported pair. `Stage::Initialize` retains the original contract queue. This proves the two calls on the normal successful Before path; a fresh instrumented whole-path count is still required before changing it. |
| B: duplicated input pools and copies | Confirmed | Prepared owns three private linear-color/R32-depth/RG16-motion groups. Before owns three shared input/output groups, copies all three prepared inputs, then copies enhanced output back into Prepared color. `FsrFrameAdapter` copies the Prepared color/depth/motion into its own retained resources. P4 avoids conversion, not these copies. Cost dominance or safety of eliminating a copy has not been established. |
| C: conservative upstream lifetime | Confirmed, with scope correction | Prepared attaches its reader fence, lease gate and eventual FSR reader to the bridge ticket. The FSR lease references **Prepared** textures, not bridge textures. However, FSR already registers its final input **copy** reader before dispatch, and spatial recovery registers its genuine draw reader. It does not normally retain NR through the entire FSR dispatch. The excess edge is between Prepared/FSR-copy lifetime and the upstream bridge ticket. |
| D: retain three slots instead of mechanically copying AIO's eight | Supported | Explicit arrays remain size three. Bridge payload is 24 bytes/pixel and Prepared payload 16 bytes/pixel. `2560×1440×3×40 / 2^20 = 421.875 MiB`; at 3840×2160 it is 949.21875 MiB. One group of each is 140.625 MiB at 1440p. These estimates exclude alignment, FSR/FG, vendor scratch and game targets; they are not measured total VRAM. |
| E: direct route depends on ReShade order | Confirmed; diagnostics incomplete | `SourceFsrEvaluation` requests a lease only when `mReShadeBeforeUpscaling` is false; evaluation selects `EvaluatePrepared` only for a valid lease. `NvidiaHostNeural` additionally forces encoded delivery on motion-diagnostic capture frames. There is no explicit `PreparedLinear`/`EncodedForReShadeBefore` transition log. ReShade configuration alone does not prove the route on every frame. |
| F: resource getters survive consumption | Confirmed API robustness issue | `PreparedFsrInput::TrackReader` clears callbacks on success; getters still expose COM objects. Contents may be reused after the actual reader completes. Existing precision fixtures deliberately access consumed `lease.Color()` for reference readbacks; an API cleanup must move those readbacks before consumption and cover their real reader lifetime, not simply invalidate getters and weaken/remove the checks. No current product call after consumption was identified in the inspected FSR path. |
| G: deprioritize alpha restoration | Supported by measurement | Its roughly 0.041 ms interval is small beside roughly 6.95 ms vendor NR. Preserve alpha correctness; no standalone alpha removal is justified. |
| H: runtime replacement is not the leading suspect | Supported within measured scope | [baseline.json](baseline.json) contains alternating clean same-host current/AIO-runtime tests. [fsr-baseline.json](fsr-baseline.json) also measures the combined NR/FSR workload. Those runs did not isolate a material AIO-model advantage; they do not establish equivalent workload in the AIO game host. Keep existing runtime pins. |

Relevant source: [interop](../../../src/Graphics/D3D11D3D12Interop.cpp), [Before](../../../src/NeuralRendering/BeforeUpscale.cpp), [Stage](../../../src/NeuralRendering/Stage.cpp), [Prepared](../../../src/NeuralRendering/PreparedBeforeUpscale.cpp), [lease](../../../src/NeuralRendering/PreparedFsrInput.h), [FSR adapter](../../../src/Upscaling/FSRFrameAdapter.cpp), [host selection](../../../src/FrameGen/SourceFsrEvaluation.cpp), [diagnostic override](../../../src/FrameGen/NvidiaHostNeural.cpp), [precision fixtures](../../../tests/NrPreparedFsrTests.cpp).

## Follow-up order

1. Preserve the qualified working package and capture a same-candidate, three-repeat gameplay NR off/on baseline with FG off. Inspect color, HUD and lifecycle separately. The user's good-performance report and five logged NR toggle recoveries are initial smoke evidence, as recorded in [acceptance](../../../docs/NR_PERFORMANCE_ACCEPTANCE.json).
2. Add effective-route observability before interpreting new benchmarks: startup/change-only route logging, ReShade-before value, diagnostic override and reasons for bypass. Include actual model hash, provider, extents and route in the benchmark receipt. The current PresentMon script records user labels and present intervals, not automatic effective settings/route validation, CPU render-thread time or GPU utilization. Keep unavailable fields explicit. Define any 1% low statistic precisely; inverse p99 is not automatically a slowest-1%-mean statistic.
3. Qualify removal of the redundant producer wait as a bounded change. Prefer Stage-owned pending-producer admission and an explicit narrowly scoped interop recording entry; leave normal FSR/other interop Begin behavior intact. Preserve allocator retirement checks, strict `Stage::Record`, exact queue/fence identity, failed-wait-before-evaluation behavior and genuine gated reference output. Count the full Before handoff waits, not only Stage's unit fixture.
4. Split bridge and Prepared lifetimes using exact resource identities. Retain Prepared through its copy/draw readers and abandon/quarantine path. Preserve reader fence identity/device validation formerly performed through Stage. Map selected `WaitDelivery`, stale-ticket handling and collection behavior too: allowing bridge retirement earlier means a later Prepared operation cannot assume its bridge ticket still exists. A test must hold a genuine Prepared-only FSR reader, prove upstream reuse can proceed, and prove the Prepared image remains protected.
5. Use matched residual milliseconds and measured pressure/memory to decide whether to collapse Prepared/Before input pools and eliminate output copy-back. Treat that topology as one coordinated ownership design; after resources are shared, downstream readers can become genuine bridge-resource readers again. Keep FSR's own retained resources initially; broader cross-API zero-copy is optional and requires a separate state/lifetime proof.
6. Invalidate consumed lease resource access with explicit regression coverage and repaired precision fixtures. This is API robustness, not a claimed FPS optimization, and may be handled independently of measured performance priorities.
7. If a substantial matched AIO gap remains, investigate actual AIO work extent, parameters, placement, formats, dispatch/copy/barrier counts and GPU timings. Static RE and evidence archives do not by themselves measure live AIO GPU duration.

The report's 1 ms / 1–3 ms / greater-than-3 ms decision bands are useful proposed heuristics, not measured thresholds or automatic acceptance criteria. Its concluding order puts large topology changes before gameplay measurement; the safer order above keeps the baseline and route qualification first, particularly now that the user reports good performance.

No new implementation milestone is completed by this review. P5 matched performance/visual/lifecycle acceptance remains open; original NR progress remains **1 of 8**. No new implementation, installation or game launch was performed.

## Subsequent authorized fix pass

After the user requested fixes, the duplicate producer wait, actual NR-to-FSR route diagnostics and consumed-lease access were addressed. [Post-P4 qualification](POST_P4_FIXES.md) records 717 fresh passing checks, source `bc2ef9d`, clean synthetic captures and a separately staged package. The installed working candidate and user settings remain unchanged. This later implementation does not complete the matched gameplay gate or the original remaining NR milestones.
