# DvaKolbas `codex/nr` Branch Review

**Repository:** `wallhead/DvaKolbas`  
**Branch:** `codex/nr`  
**Reviewed head:** `f20db264c9e508e853759166514674be8d40abbc`  
**Base:** `codex/fsr-sr` at `90e8693572c1a688c805538fbf205b3faa72792e`  
**Relationship:** 12 commits ahead, 0 behind  
**Review date:** 2026-10-03

## Assessment

The branch is a careful **research / bounded-trial implementation**, not yet the completed NR architecture in the design. The currently runnable path is one native-SDR NR pass before SR/FG. True After-FG processing and generated-guide provenance remain intentionally unimplemented. The branch's `1 / 8` milestone status is accurate.

I did not find a Critical use-after-free or an obvious unsafe retirement race in the current Before path. Its resource/ticket/shim ownership is conservative and generally stronger than AIO18's statically recovered lifecycle. The main blockers are portability, synchronization/performance, and the future After-stage contract.

## Strong parts

- Exact held-file hashing and FileId verification.
- Narrow IAT caller-identity shim with compare/exchange restoration and no blind chaining.
- Same-adapter/LUID validation.
- Sealed history/ticket identities.
- Pending packet ownership retained through submission/readers.
- Feature/parameter teardown only after reader retirement.
- Uncertain ownership is retained instead of guessed safe.
- AIO18-style NGX contract now uses `UI=null`, `UIAlpha=null`, `UICorrection=0`.
- Camera/gap/off-on reset handling is stronger than AIO18's common `Reset=0` path.
- Documentation correctly keeps RTX20/30/50 unqualified and AMD unsupported.

## P1 — General NVIDIA support is tied to one exact driver core

`src/NeuralRendering/RuntimeOwner.cpp` hard-pins `_nvngx.dll` to:

```text
SHA-256 66767018c36b3bab46398dade3adf173daa3730fda75965689ea848c9bc4e79b
size    1,428,200
driver  32.0.16.1714
```

`RuntimeOwner::Open()` requires that exact core. The trial package does not contain it; `Stage-BeforeTrial.ps1` writes the staging machine's absolute DriverCore path into the INI.

This is safe for the current controlled RTX4080 trial, but it is not deployable four-family support. Any different NVIDIA driver/core is rejected before NR initialization.

**Recommendation:** keep the current pin for the trial, but introduce a separately versioned `DriverCoreProfile` catalog before general release. Do not weaken this to basename-only loading.

## P1 — Before NR fully serializes CPU/GPU work every source frame

The current proof bridge performs a synchronous chain:

```text
D3D11 copies
 -> Signal + Flush
 -> CPU wait
 -> D3D12 NR
 -> D3D11 wait/copy-back
 -> Signal + Flush
 -> CPU wait
 -> completion-fence wait
 -> interop Drain
 -> ticket retire
 -> source-domain encode
 -> D3D11 event-query wait
```

Relevant code:
- `src/NeuralRendering/BeforeUpscale.cpp:31-45`
- `src/NeuralRendering/BeforeUpscale.cpp:110-141`
- `src/NeuralRendering/PreparedBeforeUpscale.cpp:20-25`
- `src/NeuralRendering/PreparedBeforeUpscale.cpp:65-75`

This is excellent for proving lifetime correctness, but not production-ready scheduling. It destroys normal overlap and can make NR look much more expensive than the model itself.

**Recommendation:** measure the first Skyrim trial, then move toward a ring of per-frame resources/tickets and queue-side waits. CPU waits should normally be reserved for resize/shutdown/retirement.

## P1 — The shared Stage is not yet suitable for true After FG

Current `ValidateImagePacket()` deliberately:
- rejects `ImageKind::Generated`;
- requires `GuideOrigin::RealSource`;
- requires interpolation fraction `1`;
- requires matching source/guide identity;
- requires the producer fence already CPU-completed.

`Stage::Initialize()` also requires `colorExtent == guideExtent`, and the Stage allows only one pending ticket.

That is correct for the Before proof path. It conflicts with the future approved After design, where a provider callback may expose an in-flight generated image whose correctness is established by queue ordering/fences rather than CPU completion.

**Recommendation:** do not relax this validator. Add an After-specific provider packet/ownership contract only after Task-1 research establishes real/generated image identity, matching guide provenance, queue ownership and retirement.

## P2 — First activation duplicates hashing and performs expensive lazy setup on the world frame

`BeforeHost::Inspect()` hashes and holds the large NR runtime and driver core. On first eligible world frame, `RuntimeOwner::Open()` independently opens and hashes both files again.

Runtime sizes are roughly:
- RTX40/50: 165.8 MB
- RTX20/30: 309.7 MB

The same first-use path then loads DLLs, installs the shim, initializes NR, compiles the alpha shader with `D3DCompile`, creates the feature, submits creation work and CPU-waits for completion.

**Recommendation:** transfer/move the already-verified leases from inspection into the runtime owner instead of rehashing them. Precompile the tiny alpha-preservation shader. Log initialization time separately from normal NR inference time.

## P2 — AIO18 Backbuffer behavior remains an unresolved parity difference

AIO18's internal NGX block has a distinct `DLSSNR.Backbuffer`. The immediate route chooses a route backbuffer when available and otherwise falls back to Output.

The new Stage sets:

```text
Color
MVec
Depth
Output
UI = null
UIAlpha = null
```

but does not explicitly set ControlMask, Backbuffer or BidirectionalDistortionField.

Standalone RTX40 output proves this is not automatically a bug.

**Recommendation:** A/B current behavior against `DLSSNR.Backbuffer = Output` on the qualified RTX40 runtime, including moving-scene temporal tests. Also explicitly null unsupported optional resources if the runtime contract accepts it.

## P2 — ReShade ordering is now a product choice but is not yet qualified

The new common ordering is:

```text
CopyInput
 -> NR Before
 -> ReShade "before upscaling"
 -> SR
 -> ReShade "after upscaling"
```

So ReShade-before receives the NR-processed image. AIO18 RE confirms `NR -> SR` in Before mode but did not resolve exact ReShade/ENB interposition in every configuration.

**Recommendation:** explicitly qualify ReShade absent/before/after and ENB on/off. Verify one effects execution, depth-dependent effects, color and HUD integrity.

## P2 — Source color remains provisional

The first trial uses Gamma22, inherited from the accepted FSR trial. The branch correctly converts:

```text
explicit source encoding -> Linear FP16 -> NR -> source encoding
```

A wrong source-transfer assumption directly changes the model input. This is a qualification blocker, not a code defect. ENB-off/on need separate calibration.

## P3 — Every live tuning change currently requests a temporal reset

`NvidiaHostNeural.cpp` includes tuning in the `changed` comparison and then sets:

```cpp
input.reset = reset || changed;
```

So Intensity, Local Tone, Local Structure, Skin Structure, Style, etc. all reset NR history.

Safe, but potentially unnecessary for live look changes.

**Recommendation:** later split settings into `requires recreation`, `requires temporal reset`, and `safe live parameter update`.

## P3 — No independent CI

At `f20db264...`, GitHub reports zero check runs and zero status contexts. The branch does contain useful local receipts: 240-frame GPU/bridge tests, delayed-reader retirement, 158 product checks, 23 standalone runtime tests and 30 targeted integration tests. Treat those as local evidence rather than CI.

## AIO18 parity summary

| Topic | `codex/nr` | AIO18 | Review |
|---|---|---|---|
| NGX UI | null | null | match |
| NGX UIAlpha | null | null | match |
| NGX UICorrection | 0 | 0 | match |
| reset/history | explicit camera/gap/off-on | weak common primary reset | Dva is safer |
| MV scale | render-pixel dimensions | mapped MV scale fields | plausible |
| native direct NR | yes | yes | good |
| Residual/Ratio in this new Stage | rejected; adapter required | PD private reconstruction | correct trial separation |
| Backbuffer | not explicitly set | route-selected/fallback Output | A/B needed |
| Before order | NR -> ReShade-before -> SR | NR -> SR; exact ReShade relation unresolved | runtime qualification needed |
| retirement | explicit tickets/fences/readers | partially mapped static lifecycle | Dva is easier to reason about |

## Recommended Codex order

1. Keep the branch at **1/8**, not "complete NR".
2. Run the bounded Skyrim Before trial and measure initialization hitch, NR GPU time, CPU frame time and wait durations.
3. If normal-frame synchronization is costly, pipeline the Before bridge before adding more feature scope.
4. Refactor driver-core qualification before claiming broad RTX20/30/40/50 support.
5. Run the Backbuffer=Output A/B.
6. Qualify ReShade/ENB ordering and source transfer.
7. Only then extend the stage for true After. Do not weaken real-frame safety checks to force generated frames through it.
8. True After remains blocked until generated-image guides and provider output ownership are actually observed.

## Merge assessment

**Good for a separate reversible RTX4080 native-SDR Before trial:** yes.

**Ready as the final four-family, Before+After NR implementation:** no.

The largest risk in the currently runnable code is not memory safety; it is **per-frame synchronization and first-use activation cost**. The largest architectural risk ahead is stretching the proof-oriented real-frame Stage into generated-frame processing without a provider-specific output/guide contract.
