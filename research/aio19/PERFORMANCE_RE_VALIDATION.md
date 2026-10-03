# AIO19 NR performance: independent validation, 2026-10-03

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
