# P1: queue-ordered producer admission

`Stage::RecordQueued` validates the complete packet and history, retains its resources/fence, and calls the retained contract queue's actual `Wait` before recording NR. The internal pending validator is private to Stage. Public `ValidateImagePacket` and `Stage::Record` still reject pending producers. Wait failure makes the stage terminal before any vendor recording; retained owners are not released through uncertain work.

Before now obtains the actual submitted D3D11 producer fence/value from `D3D11D3D12Interop::ProducerDependency`. It removes its second producer signal, Flush and CPU wait. The bridge's existing queue wait remains, and Stage independently enforces the dependency on its retained queue. One pending ticket, descriptor ownership, final D3D11 reader signals, completed consumer retirement and Drain remain in this checkpoint.

The new gated runtime mode submits input uploads on a separate real producer queue blocked by an external gate. Strict recording rejects that pending producer. Queued recording returns while the gate remains closed; genuine NR completion cannot retire until release. Only the gate is signaled externally. Producer and output completion fences are signaled by their actual queues. Three gated/reset images match three strict serial references exactly across every RGBA16F component; all 1,382,400 source-alpha pixels are preserved. The existing delayed final-reader gate is also exercised.

The semantic RED used the old strict implementation through a test-only fallback and failed with `NR producer ownership has not retired: target=1 completed=0`. Final tests directly invoke the new API. A test-only forwarding queue observes real device identity and injects Wait failure; invalid/foreign/disabled inputs enqueue no wait, and a failed wait records no vendor evaluation and permits no retry. This wrapper is not part of production code.

Standalone regression: 34/34 passed, including the actual ReShade native/proxy fence regression, live off/on and reset handling. Main Standard NR-enabled regression: 168/168 passed with only the three existing Graphics Tools exclusions (`NativeUIComposition`, `NativeUIBlendState`, `NeuralPeripheralPixels`). NR-disabled FSR regression: 17/17 focused checks passed. Neither run skipped a selected test. Both Standard builds compile and have no mandatory NGX/FidelityFX normal or delay imports. [Qualification](p1-qualification.json) retains identities and hashes for the tests, product imports, semantic RED, clean gated reference and combined NR/FSR correctness.

Clean timing source: `90ddeaed686fe54474e1c21626bd281741111b65`. Three 300-source repeats after 120 warmup sources each use the unchanged RTX40 model, pinned FSR 3.1.5 NativeAA, 2560×1440 synthetic scene and timer-period setting. The [comparison](p1-comparison.json) validates all six receipts against the three historical P0 repeats and admits nine same-model host comparisons, with no incomplete timing cases.

| Median per repeat | Historical P0 | P1 |
| --- | --- | --- |
| Producer CPU blocking | 1.046–1.063 ms | 0 ms |
| Final consumer CPU blocking | 7.093–7.111 ms | 7.904–8.069 ms |
| Whole NR/FSR enqueue transaction | 10.494–10.594 ms | 10.400–10.503 ms |
| Vendor NR GPU | 6.818–6.828 ms | 6.828–7.045 ms |
| Wait calls / actually blocking calls | 7 / 3 | 6 / 2 |
| Flushes / descriptor heaps | 7 / 1 | 6 / 1 |

Producer blocking and one Flush are removed, but most waiting moves to the final consumer; these overlapping intervals must not be added as independent costs. Repeat variation and the observed vendor-GPU drift limit any throughput conclusion. This is a modest standalone transaction change, not a demonstrated Skyrim FPS improvement or a solution to the unmatched AIO19 gameplay gap. P2/P3 still need to remove ownership/retirement bottlenecks safely.

The additional clean combined correctness capture executes 31 NR passes and 32 actual FSR passes, including one live NR-off source and re-enable. All 1,843,200 source-alpha pixels match, all 32 FSR output hashes differ, timing drops are zero, final owners retire and the caller shim restores. Clean gated producer verification again matches three serial references and retires the delayed final reader. Raw local captures remain under `out/research/nr/performance/clean-p1`; committed receipts preserve their SHA256 identities and summaries.

This checkpoint does not establish Skyrim FPS gains, model-family qualification, motion/color stability or literal After-FG support. The installed mod, INI and MO2 settings are unchanged. P0 matched gameplay/FG acceptance is still open; original NR milestone count remains 1 of 8. Next is P2 retained slots, before removing final reader waits in P3.
