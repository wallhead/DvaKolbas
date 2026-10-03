# DvaKolbas current `codex/nr` performance checkpoint — 2026-10-03

Purpose: source checkpoint supporting AIO19 Pass-5 performance comparison.

## Branch relation

Compared with the earlier reviewed revision:

```text
base: de4662a651ca2a6f5b5ef0f6f1f1f69ad45fcc6f
head: codex/nr
status: ahead
commits: 4
```

The four-commit diff did not modify the core files below that drive the Before-NR synchronization diagnosis.

## Current source blob identities

```text
src/NeuralRendering/BeforeUpscale.cpp
blob 432da9b6be167278f2ba2780de2e00384a16b175

src/NeuralRendering/PreparedBeforeUpscale.cpp
blob 013dfc146b121ca0664d84fdc10eafeea79925a6

src/Graphics/D3D11D3D12Interop.cpp
blob 5cb677fe4788ef3e7aea5eb450bcea7aaf326764

src/Graphics/D3D11D3D12Interop.h
blob 57be88c6e1b9075f0db4077baaf2497b7be9820d

src/NeuralRendering/Stage.cpp
blob 8472af23269fc9b44ab8637168fbf28e43bd69ea

src/NeuralRendering/ImagePacket.cpp
blob b8262fc28fe7cc0346857a345afef1592e0cd38c

src/NeuralRendering/BeforeHost.cpp
blob 414b89e8701befa70d6ae171e7c2ad1db52282d2

tools/nr/runtime-pin.json
blob 765f9e36fc2c54b1448ac4971c0cebd8f26aa7a9
```

## Evidence A — producer is GPU-wait capable but Stage still requires CPU completion

`D3D11D3D12Interop::SignalD3D11` performs a D3D11 signal and Flush. `Begin()` calls `WaitD3D12()`, which enqueues `queue_->Wait(...)` before command-list reuse/recording.

At the same time `BeforeUpscale::Handoff()` performs a second D3D11 signal/Flush and then calls the CPU `Wait()` helper.

`ValidateImagePacket()` rejects a pending producer fence:

```cpp
const auto completed=p.producerFence->GetCompletedValue();
if(completed<p.producerFenceValue){
    ...
    return Error("NR producer ownership has not retired");
}
```

Result: the producer must physically retire before Stage recording, despite a queue-side wait mechanism already existing.

## Evidence B — Stage permits one pending NR ticket

`Stage::State` contains:

```cpp
std::unique_ptr<Pending> pending;
```

`Stage::Record()` checks:

```cpp
if(s.pending)
    return Fail(ErrorKind::Retirement,
                "NR prior recording/readers have not retired");
```

Result: Stage-level outstanding NR depth is one.

## Evidence C — interop has three allocator slots, but current Before path retires same-frame

`D3D11D3D12Interop.h`:

```cpp
inline constexpr std::size_t kCommandSlots = 3;
```

`BeforeUpscale::Evaluate()` after submission performs:

```text
WaitConsumer
copy back
D3D11 signal + Flush
CPU Wait(output reader)
CPU Wait(completion fence)
interop.Drain()
stage.RetireTicket()
return
```

Therefore the lower-level three-slot ring cannot provide meaningful multi-frame NR overlap in the current Before path.

## Evidence D — Drain is a real CPU retirement barrier

`D3D11D3D12Interop::Drain()`:

```text
may signal final D3D11 consumer + Flush
unconditional D3D11 Flush
WaitCPU() for active work fence values
```

It is called in steady-state `BeforeUpscale::Evaluate()`, not only on resize/shutdown.

## Evidence E — prepared adapter adds a query/poll barrier

`PreparedBeforeUpscale::FinishReaders()`:

```cpp
context->End(reader.Get());
context->Flush();
for(;;){
    auto hr=context->GetData(...D3D11_ASYNC_GETDATA_DONOTFLUSH);
    if(hr==S_OK&&done) ...;
    ...
    Sleep(1);
}
```

This is another hard CPU completion point before the source frame proceeds into the normal upscaler path.

## Evidence F — per-record descriptor heap + full-frame alpha dispatch

`Stage::Record()` creates a new shader-visible descriptor heap per recording, creates SRV/UAV descriptors, then dispatches an 8x8-thread compute pass across the full image to restore original alpha.

These are proven steady-state costs, but are lower priority than the serialization boundaries above.

## Evidence G — NR uses a dedicated DIRECT queue

`BeforeHost` reuses the presenter D3D12 device when available, then creates a new DIRECT queue for NR:

```cpp
s.contract.device=presenter;
...
s.contract.device->CreateCommandQueue(&q,IID_PPV_ARGS(&s.contract.queue));
```

This does not by itself prove a performance defect. AIO19 also has separate backend queue domains. The key difference is whether cross-domain dependencies are CPU-retired or GPU-ordered.

## Runtime pin

Current RTX40 Dva runtime:

```text
size   165840496
sha256 e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a
```

AIO19 runtime:

```text
size   165840496
sha256 8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206
```

A same-host runtime A/B is still required before assigning the entire AIO19-vs-Dva gap to host scheduling.
