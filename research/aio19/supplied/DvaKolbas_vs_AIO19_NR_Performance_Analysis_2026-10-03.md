# DvaKolbas vs PureDark AIO19 — NR Performance Investigation

**Date:** 2026-10-03  
**Goal:** explain why DvaKolbas loses roughly 10–15 FPS versus the newly supplied PureDark AIO19 build when Neural Rendering is enabled.

## Exact targets

### PureDark AIO19 Hotfix 1

Archive:

```text
SkyrimUpscalerAIOBuild19-Hotfix1(1).7z
SHA-256:
49e7f7dabf426937915d1aeed664fc40a7cc7d89f42092a69c205b22c4687439
```

Principal binaries:

```text
SkyrimUpscaler.dll
ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a

PDPerfPlugin.dll
ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435

nvngx_dlssnr.dll
8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206
```

The AIO19 NR runtime is **byte-identical to the AIO18 runtime** previously reverse-engineered.

### DvaKolbas

Reviewed branch:

```text
branch: codex/nr
head: de4662a651ca2a6f5b5ef0f6f1f1f69ad45fcc6f
```

RTX40 NR runtime:

```text
e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a
```

This is the openNR compat-fp16 runtime, not the AIO19 `8270...` runtime.

---

# Executive conclusion

The performance gap is most likely **mostly integration overhead in DvaKolbas**, with the different NR device-code runtime as a secondary variable that still needs a clean A/B.

Current Dva native-Before architecture effectively does:

```text
Skyrim D3D11
  ↓
decode Gamma22 -> linear FP16
prepare depth + motion
  ↓
copy color/depth/motion AGAIN to NR shared resources
  ↓
D3D11 signal + Flush
CPU fence wait
  ↓
independent NR D3D12 DIRECT queue
  ↓
NGX NR
extra full-resolution alpha-restoration compute
  ↓
D3D11 wait
copy NR output back
signal + Flush
CPU wait
completion CPU wait
interop Drain + additional Flush/waits
  ↓
encode linear -> Gamma22
D3D11 query Flush + CPU polling
  ↓
FSR starts
  ↓
decode Gamma22 -> linear AGAIN
copy depth + motion AGAIN
  ↓
independent FSR D3D12 DIRECT queue
  ↓
FSR SR
```

This is a proof-oriented implementation with many synchronization boundaries.

AIO19 instead owns NR inside the persistent PD D3D11/D3D12 backend, and adds a new **chain submission API**:

```text
EvaluateDLSSNRChain
QueryDLSSNRChainStatus
```

The chain call submits a `0x598` `DLSSNRChainFrameData` structure and returns a `DLSSNRSubmitResult`.

The top-level chain submission path does **not** contain a direct `WaitForSingleObject` call.

That is not proof that every callee is fully asynchronous, but it is a strong architectural contrast with Dva, which explicitly CPU-blocks multiple times in its normal source-frame path.

---

# 1. Dva's own Skyrim log already shows a very large NR cost

The current branch's Skyrim smoke log contains live NR off/on transitions in one session.

Measured from source IDs and timestamps:

```text
NR ON
source 8622 @ 11:53:12.279
source 8703 @ 11:53:14.698

81 frames / 2.419 s
= 33.48 FPS
= 29.86 ms/source frame
```

Then:

```text
NR OFF
source 8703 @ 11:53:14.698
source 8829 @ 11:53:16.949

126 frames / 2.251 s
= 55.98 FPS
= 17.87 ms/source frame
```

Then NR on again:

```text
NR ON
source 8829 @ 11:53:16.949
source 9160 @ 11:53:26.302

331 frames / 9.353 s
= 35.39 FPS
= 28.26 ms/source frame
```

So the observed Dva NR path adds approximately:

```text
~10.4 to ~12.0 ms per source frame
```

in this smoke sequence.

This is not a calibrated GPU timestamp and should not be reported as pure NGX inference cost. But it is very strong same-session evidence that Dva's current NR integration introduces a large frame-path stall.

---

# 2. Highest-confidence cause: multiple CPU synchronization points per frame

## Dva `BeforeUpscale`

`src/NeuralRendering/BeforeUpscale.cpp`

The normal active frame executes:

```text
CopyInput(color)
CopyInput(depth)
CopyInput(motion)

SignalProducer()
Handoff()

Begin()
Stage::Record()
Submit()
Stage::MarkSubmitted()

WaitConsumer()
copy output to source

Signal output reader
Flush()

CPU Wait(output reader)
CPU Wait(completion fence)

interop.Drain()
Stage::RetireTicket()
```

### `Handoff()` explicitly CPU-blocks

It performs:

```cpp
Context11()->Signal(...)
Context11()->Flush();
Wait(handoffFence, value);
```

`Wait()` uses:

```cpp
SetEventOnCompletion(...)
WaitForSingleObject(...)
```

So every NR source frame can explicitly wait for the D3D11 producer before continuing.

## `Drain()` explicitly retires the frame every frame

`D3D11D3D12Interop::Drain()`:

```text
may Signal D3D11 consumer
Flush
unconditional Flush
WaitCPU on D3D12 work fences
```

A drain is normally appropriate for destruction/resize/lifecycle boundaries.

Dva currently invokes it in the normal NR evaluation path:

```cpp
interop.Drain();
```

before retiring each NR ticket.

This prevents the intended allocator-slot/ring design from providing much inter-frame overlap.

## Prepared source adapter adds another wait

`PreparedBeforeUpscale::FinishReaders()`:

```cpp
context->End(query);
context->Flush();

while (GetData(...DONOTFLUSH) != S_OK) {
    Sleep(1);
}
```

This is another full source-frame completion boundary after NR output is encoded back.

### Estimated explicit Flush count

The active Dva Before path can execute roughly these D3D11 flushes:

1. `SignalProducer()` -> `SignalD3D11()` -> `Flush`
2. `Handoff()` -> `Flush`
3. output-reader signal -> `Flush`
4. `Drain()` consumer signal -> `Flush`
5. `Drain()` unconditional -> `Flush`
6. `PreparedBefore::FinishReaders()` -> `Flush`

So Dva can issue around **six explicit D3D11 Flush calls per active NR frame** before normal FSR processing completes.

Not every Flush necessarily stalls by itself, but combined with explicit CPU fence waits they destroy driver batching and CPU/GPU overlap.

### Confidence

**VERY HIGH.**

This mechanism alone is sufficient to explain a several-millisecond frame-time penalty.

---

# 3. High-confidence cause: two independent D3D12 DIRECT queues with a D3D11 round trip between NR and FSR

A recent Dva fix correctly reuses the FSR presenter's D3D12 **device**:

```cpp
if (fsrResources_ && fsrResources_->Bridge())
    presenter = fsrResources_->Bridge()->Device12();
```

But `BeforeHost` still creates a new D3D12 queue:

```cpp
D3D12_COMMAND_QUEUE_DESC q{};
contract.device->CreateCommandQueue(&q, &contract.queue);
```

FSR has its own separately-created DIRECT queue.

Therefore the NativeAA + NR path is approximately:

```text
Skyrim D3D11
     ↓
NR D3D12 queue
     ↓
D3D11 copy-back / waits
     ↓
FSR D3D12 queue
     ↓
D3D11 / presentation
```

This forces explicit synchronization between API/queue domains that could otherwise remain backend-local.

### AIO19 contrast

PD owns the D3D11/D3D12 routing, NR feature and SR/FG integrations inside the same backend architecture.

AIO19 also adds:

```text
EvaluateDLSSNRChain
QueryDLSSNRChainStatus
```

This strongly suggests PureDark is moving more NR scheduling/retirement into its persistent backend rather than forcing Skyrim's caller to synchronously retire each NR operation.

### Confidence

**HIGH.**

---

# 4. High-confidence cause: duplicate full-resolution resource preparation

Dva prepares its input once in `PreparedBeforeUpscale`:

```text
source color -> linear FP16 color
source depth -> R32 prepared depth
source motion -> RG16F prepared motion
```

Then `BeforeUpscale` copies all three again into another D3D11/D3D12 shared set:

```text
prepared linear color -> NR shared color
prepared depth        -> NR shared depth
prepared motion       -> NR shared motion
```

After NR, the result is copied back.

Then FSR separately prepares color/depth/motion again.

At 2560×1440 these are not tiny resources:

```text
RGBA16F  ≈ 29.5 MB per image
R32      ≈ 14.7 MB
RG16F    ≈ 14.7 MB
```

The bandwidth itself is not likely the entire 10–12 ms because an RTX 4080 SUPER has enormous memory bandwidth.

The expensive part is that these copies occur across synchronization boundaries and separate queue ownership domains.

### Confidence

**HIGH.**

---

# 5. High-confidence cause: linear -> Gamma22 -> linear round trip between NR and FSR

Dva's first Skyrim trial uses:

```text
SourceColorEncoding = Gamma22
```

The NR path:

```text
Gamma22 source
    ↓
decode
    ↓
linear FP16
    ↓
NR
    ↓
encode
    ↓
Gamma22 source
```

Immediately afterward FSR receives that texture and does:

```text
Gamma22
    ↓
decode
    ↓
linear FP16 FSR input
```

Therefore:

```text
NR linear output
 -> Gamma22
 -> linear again
```

is performed every temporal frame.

That means two avoidable full-resolution conversion passes sit between NR and FSR.

### Better architecture

When NR Before feeds TRP-owned FSR:

```text
Skyrim source
   ↓ one decode
shared linear prepared color
   ↓
NR
   ↓
FSR
   ↓
one final output encode
```

Do not convert back into Skyrim's original source domain merely so FSR can immediately convert it again.

### Confidence

**HIGH.**

---

# 6. Medium-confidence cause: extra full-resolution alpha-restoration pass

After NGX evaluation, `Stage::Record()` always dispatches a custom compute shader:

```hlsl
float4 p = enhanced[id.xy];
p.a = original.Load(...).a;
enhanced[id.xy] = p;
```

This is a complete full-resolution dispatch over the NR image.

It exists for a valid correctness reason: preserve source alpha.

But AIO19 owns its private output/UI pipeline and may not need an equivalent extra standalone full-image pass on the same route.

This likely contributes a smaller fraction of the total cost than the waits/round trips.

### Confidence

**MEDIUM.**

---

# 7. Medium-confidence cause: D3D12 descriptor heap allocated every NR frame

`Stage::Record()` creates a new shader-visible descriptor heap for every evaluation:

```cpp
CreateDescriptorHeap(...)
CreateShaderResourceView(...)
CreateUnorderedAccessView(...)
```

The heap is retained in the per-frame `Pending` object and discarded after ticket retirement.

This is unnecessary steady-state CPU overhead.

A persistent small descriptor ring would be cheaper.

### Confidence

**MEDIUM**, probably sub-millisecond alone.

---

# 8. New AIO19 optimization: `EvaluateDLSSNRChain`

AIO19 PD exports now include:

```text
EvaluateDLSSNR
EvaluateDLSSNRChain
QueryDLSSNRChainStatus
QueryDLSSNRMaskCapabilities
InitDLSSNR
ReleaseDLSSNR
```

Important AIO19 PD RVAs:

```text
InitDLSSNR             +0x1177E0
EvaluateDLSSNR         +0x117810
EvaluateDLSSNRChain    +0x1178D0
QueryDLSSNRChainStatus +0x117910
ReleaseDLSSNR          +0x117A50
```

Inside `UpscaleAPI_D3D11`:

```text
+0xF0  -> +0x110990  Init
+0xF8  -> +0x110B10  Evaluate
+0x100 -> +0x110E60  EvaluateDLSSNRChain
+0x108 -> +0x111100  Release
```

The chain API validates/copies exactly:

```text
0x598 bytes
```

of `pd::DLSSNRChainFrameData`.

RTTI identifies the return type as:

```text
pd::DLSSNRSubmitResult
```

The host resolves `EvaluateDLSSNRChain` dynamically and invokes it from at least:

```text
SkyrimUpscaler +0x281F3F
SkyrimUpscaler +0x2FABB7
```

The top-level chain submission helper reached from this path (`PD +0x89C0`) has **no direct `WaitForSingleObject` call**.

The helper immediately below this region (`+0x8BB24`) does contain a wait, so it would be incorrect to claim the entire backend is asynchronous from static evidence alone.

But the API design itself is important:

```text
submit chain
       ↓
DLSSNRSubmitResult
       ↓
query chain status separately
```

is qualitatively different from Dva's:

```text
record
submit
CPU wait
CPU wait
Drain
retire
return
```

on every normal frame.

### AIO19 chain config

The shipped INI uses:

```text
ChainVersion = 1
PassCount    = 1
```

and warns that additional passes increase GPU cost.

The new chain therefore matters even in the current one-pass configuration because it centralizes submission/state ownership, not only because it can execute multiple passes.

### Important qualification

Current static evidence does **not** prove:

```text
SR + NR
```

are one single AIO19 command list.

`EvaluateDLSSNRChain` is an **NR pass-chain API**.

The safe conclusion is:

> AIO19 has moved NR pass submission and status ownership deeper into its backend and can avoid some caller-side per-pass plumbing.

Do not claim it literally fuses FSR SR and NR until a command-list trace proves that.

---

# 9. Runtime DLL difference may also matter

AIO19 uses:

```text
8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206
```

Dva RTX40 uses:

```text
e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a
```

Both are 165,840,496-byte 310.8-derived runtimes, but they are **not the same GPU device code**.

Prior openNR research establishes that this family of patched runtimes can have identical host ABI/code while differing substantially inside embedded CUDA/device-code fatbin regions.

Therefore an RTX4080 performance difference is possible even with the same host integration.

### Critical A/B

Before major refactoring, run the exact same Dva executable/settings twice:

```text
A: Dva + current e67dee runtime
B: Dva + AIO19 8270 runtime
```

using an explicit temporary pinned profile.

Measure:

```text
source FPS
PresentMon cadence
GPU busy
NGX evaluation GPU duration
CPU frame time
```

Interpretation:

```text
8270 recovers most FPS
    -> device-code/runtime kernels are a major cause

8270 changes little
    -> Dva host synchronization/copy architecture is dominant

8270 recovers some but not all
    -> both contribute
```

This is the **fastest and most useful discriminating experiment**.

Do not simply overwrite the existing runtime file: add a separate exact-hash profile so the result is reproducible.

---

# 10. Why the reported 10–15 FPS difference is plausible

At 60 FPS:

```text
16.67 ms/frame
```

At 45 FPS:

```text
22.22 ms/frame
```

A drop of 60 -> 45 FPS corresponds to only:

```text
+5.56 ms/frame
```

A drop of 50 -> 35 FPS corresponds to:

```text
20.0 -> 28.6 ms
= +8.6 ms/frame
```

Dva's own smoke log suggests roughly **+10–12 ms** while NR is active.

Therefore it is entirely plausible for the current synchronization architecture to explain the reported 10–15 FPS deficit.

This does **not** require the NR model itself to be dramatically slower.

---

# 11. Recommended optimization order

## Experiment 1 — runtime-only A/B

Add exact AIO19 `8270...` as a temporary Dva runtime profile.

No host-code changes.

This separates runtime-device-code cost from integration cost.

## Experiment 2 — instrument the current frame

Add CPU + GPU timing around:

```text
source decode
depth preparation
motion preparation

NR color copy
NR depth copy
NR motion copy

SignalProducer
Handoff wait

NGX Evaluate command recording
NR queue execution

WaitConsumer
copy-back

output-reader wait
completion wait
Drain

alpha restoration
source encode
FinishReaders query wait

FSR color decode
FSR depth copy
FSR motion copy
FSR dispatch
```

Report p50/p95 rather than one frame.

## Experiment 3 — remove normal-frame `Drain()`

Do not just delete synchronization.

Build a 2- or 3-slot retained ticket/resource ring:

```text
frame N:
 submit NR N
 queue D3D11 consumer dependency

frame N+1:
 use another slot

retire N only when its real reader fences complete
```

`Drain()` should remain for:

```text
resize
shutdown
feature recreation
device/presentation transition
```

not normal steady-state frames.

## Experiment 4 — remove CPU handoff waits

Convert:

```text
D3D11 signal
CPU WaitForSingleObject
D3D12 submit
```

into:

```text
D3D11 signal
D3D12 queue Wait
D3D12 submit
```

where the existing resource ownership contract permits it.

Likewise, let D3D11 queue a wait for NR completion instead of waiting on the CPU.

## Experiment 5 — share NR/FSR preparation

For the FSR Before route, create one host preparation domain:

```text
source
  ↓
linear color
prepared depth
prepared motion
  ↓
NR
  ↓
FSR
```

Reuse:

```text
linear color
R32 depth
RG16F motion
```

instead of copying them through NR and then preparing them again for FSR.

Ideally NR and FSR should also share the same D3D12 DIRECT queue, or use a queue-side fence without returning through D3D11 between them.

## Experiment 6 — remove the Gamma22 round trip

Current:

```text
Gamma22 -> Linear -> NR -> Gamma22 -> Linear -> FSR
```

Target:

```text
Gamma22 -> Linear -> NR -> FSR -> final encode
```

## Experiment 7 — persistent descriptor ring

Replace per-frame `CreateDescriptorHeap()` with preallocated per-slot descriptors.

## Experiment 8 — alpha preservation

Determine whether alpha must be restored with a standalone compute pass for the FSR Before route.

Possible alternatives:

- route alpha separately without a full-image post-NR pass;
- merge alpha preservation into an already-required color conversion;
- preserve/copy alpha in the final delivery shader.

Do not remove alpha restoration without HUD/transparency validation.

---

# 12. Highest-value target architecture

For the current FSR + NativeAA + NR Before mode:

```text
Skyrim D3D11 source
        ↓
one D3D11 producer signal
        ↓
shared/persistent D3D12 queue
        ↓
one preparation:
    linear color
    prepared depth
    prepared motion
        ↓
NR
        ↓
queue barrier/fence only
        ↓
FSR SR
        ↓
FG preparation / presentation
        ↓
native UI
```

Avoid this steady-state path:

```text
D3D11
 -> queue A
 -> CPU
 -> D3D11
 -> queue B
 -> CPU
 -> D3D11
```

That is the central performance lesson from the AIO19 comparison.

---

# Final ranking

| Suspect | Confidence | Likely importance |
|---|---|---|
| Per-frame CPU waits / `Drain()` / repeated `Flush()` | **Very high** | **Very high** |
| Independent NR and FSR D3D12 queues with D3D11 round trip | **High** | **Very high** |
| Duplicate full-res color/depth/MV copies | **High** | High |
| Gamma22 -> Linear -> Gamma22 -> Linear | **High** | Medium-high |
| `e67dee` runtime vs AIO19 `8270` device code | **Needs A/B** | potentially high |
| Extra alpha restoration compute | Medium | low-medium |
| Per-frame descriptor heap creation | Medium | low-medium |
| Generic validation/COM queries/allocations | High that they exist | low |

## Working conclusion

The most likely explanation for DvaKolbas being 10–15 FPS behind AIO19 is:

```text
Dva correctness-harness synchronization
+
duplicated interop/resource preparation
+
separate NR and FSR queues
+
avoidable color-domain round trip
```

with a **possible additional runtime-kernel difference** between `e67dee...` and AIO19's `8270...`.

The next test should be the runtime-only A/B first because it is cheap and clean. After that, instrument and pipeline the host path rather than trying to optimize the NGX parameter writer.
