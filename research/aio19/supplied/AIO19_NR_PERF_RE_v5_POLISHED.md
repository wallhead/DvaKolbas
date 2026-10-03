# PureDark Skyrim Upscaler AIO19 Hotfix 1 — NR Performance RE, Passes 1–5 Polished

**Target archive:** `SkyrimUpscalerAIOBuild19-Hotfix1(1).7z`  
**Archive SHA-256:** `49e7f7dabf426937915d1aeed664fc40a7cc7d89f42092a69c205b22c4687439`  
**Analysis type:** static reverse engineering and cross-pass consistency audit. Binaries were not executed in this pass.  
**Purpose:** combine AIO19 NR performance RE Passes 1–4 with a fifth performance-focused polish pass against current DvaKolbas `codex/nr`, separating proven scheduling costs from runtime-only hypotheses and turning the comparison into an implementation priority list.

## 1. Exact build identities

All RVAs and architectural conclusions in this report apply to this exact AIO19 Hotfix 1 build:

```text
SkyrimUpscaler.dll
ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a
size 15975424

PDPerfPlugin.dll
ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435
size 20332032

nvngx_dlssnr.dll
8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206
size 165840496
```

The NR runtime is byte-identical to AIO18. The performance-relevant AIO19 changes therefore live in the host/backend integration rather than in a newer NVIDIA NR model DLL.

## 2. Final combined architecture

The highest-confidence steady-state AIO19 local NR path is:

```text
Skyrim / D3D11 producer
    |
    | ID3D11DeviceContext4::Signal(shared fence, producerValue)
    | ID3D11DeviceContext::Flush()
    v
shared producer fence
    |
    | ID3D12CommandQueue::Wait(producerValue)
    v
persistent NR D3D12 command-ring slot
    |
    +-- cached resource mapping / staging
    +-- NR chain evaluation
    +-- Close command list
    +-- ExecuteCommandLists
    +-- ID3D12CommandQueue::Signal(completionValue)
    v
shared completion fence
    |
    | ID3D11DeviceContext4::Wait(completionValue)
    v
D3D11 output CopyResource / CopySubresourceRegion is queued
    |
    v
host continues to later SR/presentation work
```

This path still crosses D3D11↔D3D12 and still performs a final result copy. The key difference from DvaKolbas's current proof-oriented Before-NR bridge is that the normal dependencies are kept GPU-ordered rather than forcing the CPU to retire the complete NR transaction before FSR proceeds.

## 3. AIO19 chain ABI and public NR surface

AIO19 PD exposes:

```text
InitDLSSNR                  +0x1177E0
EvaluateDLSSNR              +0x117810
EvaluateDLSSNRChain         +0x1178D0
QueryDLSSNRChainStatus      +0x117910
QueryDLSSNRMaskCapabilities +0x1179F0
ReleaseDLSSNR               +0x117A50
```

The D3D11 backend vtable includes:

```text
+0xF0  -> +0x110990 InitDLSSNR
+0xF8  -> +0x110B10 EvaluateDLSSNR
+0x100 -> +0x110E60 EvaluateDLSSNRChain
+0x108 -> +0x111100 ReleaseDLSSNR
```

The chain entry requires:

```text
frameData != null
frameData->size    == 0x598
frameData->version == 1
```

The deeper validator establishes:

```text
PassCount offset = +0x174
PassCount range  = 1..10
pass table start = +0x17C
pass stride      = 0x68
```

Per-pass IDs must be nonzero and unique. Four per-pass floats are range checked to `[0,1]`; a resolve-method-like field is limited to `0..2`; an embedded list count is bounded to 4. The shipped INI uses `ChainVersion=1` and `PassCount=1`.

The important scheduling change is therefore present even for the default one-pass configuration: submission, slot lifetime, and completion/status ownership are backend managed.

## 4. Submission and status are decoupled

`EvaluateDLSSNRChain` returns a submit result while `QueryDLSSNRChainStatus` is a separate API. The status API validates a version-1, 0x20-byte status structure, takes the backend lock at approximately `backend+0x31D0`, and copies the live status block at approximately `backend+0x3220`.

This supports a submit-now / observe-status-later architecture. The static evidence does **not** show the caller using status polling as a mandatory CPU completion barrier in the mapped local hot path.

The numeric submit results observed at the API boundary are consistent with:

```text
0 = accepted/success path
1 = invalid chain contract
2 = backend unavailable
3 = submission/HRESULT failure path
```

The exact enum names are not embedded and should not be treated as recovered symbols.

## 5. Persistent 8-slot D3D12 command ring

The NR backend maintains eight command-work slots. Slot selection at approximately `PD +0x75BB0` works as follows:

1. Start from the current slot index.
2. Scan up to eight slots.
3. For a slot with a prior completion value, query `ID3D12Fence::GetCompletedValue()`.
4. Reuse the first slot whose fence value is complete.
5. Only when every candidate remains busy does the backend enter its backpressure wait path.

The loop bound is statically visible as:

```asm
inc esi
cmp esi, 8
jl  scan_next_slot
```

This is stronger than merely observing an 8-element allocation: the runtime actively searches the ring for a reusable completed slot.

### Important wording correction

An 8-slot command ring does **not** prove eight full frames of NR are always in flight. It proves eight persistent command/work slots are available for reuse and overlap. The exact number of simultaneously outstanding frames is runtime-dependent.

## 6. Pass 4 — saturation/backpressure path

Pass 4 concentrated on hidden waits, failure tails, and whether Passes 1–3 overstated the steady-state nonblocking behavior.

### 6.1 The backpressure wait is real and is infinite

When all eight slots are busy, the selector calls helper `PD +0x70540`. That helper:

1. reads the fence completed value;
2. returns immediately if the target value is already complete;
3. checks device-removal state;
4. calls the fence event-registration method;
5. calls `WaitForSingleObject(event, INFINITE)`.

The exact wait setup contains:

```asm
1800705f7  call qword ptr [rax+0x48]   ; fence event registration
1800705fa  mov edx, 0xffffffff         ; INFINITE
1800705ff  mov rcx, rbp                ; event handle
180070602  call [WaitForSingleObject]
```

Therefore the combined report must **not** say AIO19 is globally CPU-wait-free.

The accurate statement is:

> The mapped normal local dependency chain is GPU-ordered. A CPU wait appears when the persistent command ring is saturated or in exceptional/recovery paths.

### 6.2 This wait is not performed on every frame

The selector first tests all eight slots with `GetCompletedValue()`. Only after the complete eight-slot scan fails does it wait on the current slot's target fence value.

This distinction is crucial when comparing against DvaKolbas. Dva's current Before-NR path explicitly waits and drains as part of normal transaction retirement, while AIO19 reserves its direct OS wait for pressure/failure conditions in the mapped ring path.

### 6.3 Device-loss checks guard the wait

Before and after the blocking event wait, the helper checks both relevant D3D12 device objects through a virtual call at device vtable offset `+0x128`, consistent with `GetDeviceRemovedReason()` behavior. A nonzero result diverts into recovery/cleanup rather than blindly continuing.

This materially reduces the risk of interpreting the infinite wait as an unconditional deadlock design. It is still an infinite OS wait if the event never arrives and the device checks do not report failure, so runtime instrumentation should measure whether this path is ever hit in normal play.

## 7. Command-list begin/submit behavior

The begin helper around `PD +0x75690`:

- requires valid local device/queue state;
- selects a reusable ring slot;
- resets the slot's command allocator;
- resets the slot's command list;
- stores the active slot index;
- returns the persistent command-list pointer.

If selection/reset fails, it returns null and the local NR route follows a diagnostic/recovery path instead of issuing malformed work.

The submit helper around `PD +0x75750`:

1. closes the active command list;
2. aborts on `Close()` failure;
3. calls `ExecuteCommandLists` on the NR queue;
4. increments a persistent fence counter;
5. signals the queue fence;
6. stores the signaled value in the slot;
7. advances the ring index modulo 8.

There is no direct CPU wait in this submit helper.

## 8. Local D3D11 producer handoff is GPU-side after one Flush

The mapped local path around `PD +0xA5417` obtains the shared fence and performs:

```asm
call [D3D11 context vtable +0x498]   ; Signal
call [D3D11 context vtable +0x378]   ; Flush
...
call [D3D12 queue vtable +0x78]      ; Queue::Wait
```

The `Flush()` is real and should not be hidden in the comparison. The important point is what happens next: AIO19 queues the D3D12 dependency with `Queue::Wait` instead of immediately blocking the CPU on the producer fence.

## 9. Local D3D12 -> D3D11 consumer handoff is also GPU-ordered

After NR work is submitted and the D3D12 completion fence is signaled, the mapped local route queues the corresponding D3D11-side fence wait and then queues the output copy back to the D3D11-visible destination.

This means AIO19 can return host control after scheduling the dependency chain; it does not need to synchronously wait for NR completion before merely recording the dependent D3D11 copy.

The final local result copy is therefore **not eliminated**. The optimization is scheduling/ownership, not magical zero-copy execution.

## 10. NR and FSR queue domains

Pass 2 correctly left final queue identity unresolved. Pass 3 recovered enough backend layout to refine this:

### FSR backend

The FidelityFX/D3D12 backend has its own device/queue fields, including approximately:

```text
device +0x98
queue  +0xB0
```

### NR backend

The NR/local backend has distinct queue-bearing fields, including approximately:

```text
primary device       +0x98
primary queue        +0xA8
local-secondary dev  +0xB98
local-secondary queue+0xBA0
```

A compatible configuration can alias the secondary NR pair to the primary pair. Otherwise a separate queue is created.

### Correct final claim

**Proven:** FSR and NR are owned by separate backend domains with separate queue fields.  
**Not proven:** their final COM queue pointers can never alias in any runtime configuration.

Thus sharing one physical queue is not a prerequisite for reproducing AIO19's performance model. GPU-side dependencies matter more than forced queue unification.

## 11. Persistent resources and descriptor ownership

The mapped normal NR route repeatedly references cached backend resources/work surfaces around fields such as:

```text
+0x390 / +0x398
+0x480 / +0x488
+0x4A0 / +0x4A8
+0x4B0 / +0x4B8
+0x8B0 ...
```

No normal-frame `CreateDescriptorHeap` or direct `CreateCommittedResource` call was recovered in the mapped hot region. Resource creation is pushed into cache/init/miss helpers instead.

The resource clone helper at approximately `PD +0x6F060` obtains the source `ID3D12Resource::GetDesc()` and constructs a matching committed resource on a miss.

For DvaKolbas this supports moving per-frame NR descriptors/heaps into persistent per-slot ownership rather than creating/destroying them for each active frame.

## 12. Resource copies: corrected interpretation

AIO19 does **not** eliminate staging or copies.

### Proven

- D3D11 producer synchronization exists.
- D3D12 queue waiting exists.
- backend-local mapping/staging exists.
- a D3D11-visible final NR result copy exists.
- full-resource and subrect output paths exist.

### Not proven

- zero-copy NR input;
- a single shared D3D12 resource for every NR/FSR semantic;
- one universal NR+FSR command list;
- one universal NR+FSR queue.

The performance advantage is therefore best explained by reduced CPU serialization plus persistent resource lifetime, not by the absence of memory movement.

## 13. Color-domain round trip

Dva's current first Skyrim Before path explicitly performs a costly pattern equivalent to:

```text
Gamma22 source
 -> decode / prepare Linear FP16
 -> NR
 -> encode Linear -> Gamma22
 -> FSR decodes Gamma22 -> Linear again
```

The mapped AIO19 resource path preserves source resource descriptions and does not expose an equivalent explicit transfer-function shader pair around NR.

This is **not sufficient to prove** the exact AIO19 live color domain for every ENB/HDR/ReShade combination.

Correct classification:

- **Observed:** no explicit Dva-like decode/encode round trip was recovered in the mapped AIO19 local staging path.
- **Likely/inferred:** AIO19 avoids at least part of Dva's redundant transfer-function round trip.
- **Runtime needed:** exact color domain and format at the live NR→FSR boundary.

## 14. Depth and motion-vector reuse

The Skyrim-facing depth and motion semantics share host-side provenance, but Pass 3 does not prove that NR and FSR use the exact same D3D12 resource objects internally.

Best-supported model:

```text
same host D3D11 depth/motion provenance
       |
       +--> NR backend cached D3D12 mapping/staging
       |
       +--> FSR backend cached D3D12 mapping/staging
```

Therefore guide-resource unification remains a valid Dva optimization target, but it should follow the higher-value synchronization rewrite rather than block it.

## 15. Failure behavior and fail-soft chain processing

The chain backend contains explicit diagnostics equivalent to:

```text
DLSSNR chain: passId=%u failed, retaining %u successful passes
DLSSNR: unavailable passId=%u
```

That means multipass failure can retain already-successful work instead of forcing the host to rebuild the entire chain transaction.

The local command path similarly checks begin/reset/close/device state and routes failures into logging/cleanup/recovery. These failure branches should not be mistaken for steady-state costs, but a Dva rewrite should preserve equivalent safe cleanup semantics before removing blocking waits.

## 16. What was wrong or overstated in Passes 1–3

Pass 4 found no identity/hash contradiction and no evidence invalidating the main scheduling conclusion. It did find several statements that needed tighter wording.

### Correction A — “no CPU waits” was too broad

Incorrect broad interpretation:

```text
AIO19 NR has no CPU waits.
```

Correct:

```text
The mapped normal local dependency chain is GPU-ordered.
The 8-slot ring has an explicit WaitForSingleObject(INFINITE) backpressure path when all slots are busy, plus exceptional/controller waits elsewhere.
```

### Correction B — eight slots are not automatically eight frames

Use “eight persistent command/work slots,” not “eight frames always in flight.”

### Correction C — NR and FSR are not proven to be one queue

Pass 2 left this unresolved. Pass 3 resolved ownership to separate backend queue domains, with possible pointer aliasing in compatible configurations. The combined report adopts the Pass-3 wording.

### Correction D — color-domain savings remain inference

The absence of an explicit Dva-like transfer round trip in the captured AIO19 path is useful evidence, but it does not prove the exact live transfer function across every renderer stack.

### Correction E — shared guide provenance is not shared object identity

Depth/MV originate from the same host-side semantics; internal NR and FSR D3D12 resources may still be distinct cached mappings.

### Correction F — the nearby CreateThread/controller loop is not a proven NR chain worker

A nearby thread entry previously looked relevant to NR scheduling. Pass 3 correctly reclassified it as an ETW/controller-style subsystem. The combined report makes no claim that this thread is the NR chain worker.


## 17. Pass 5 — performance-focused polish: AIO19 vs current DvaKolbas

Pass 5 does not change the AIO19 RE architecture recovered in Passes 1–4. Its purpose is to make the performance comparison against **current `wallhead/DvaKolbas` `codex/nr`** more precise and to remove two earlier ambiguities:

1. a separate NR queue is not, by itself, the performance defect;
2. Dva's 3-slot interop allocator ring does not currently create meaningful NR overlap because the higher-level Stage contract permits only one pending ticket and the caller retires it in the same source frame.

### 17.1 Current Dva branch checkpoint

The earlier Dva comparison used:

```text
codex/nr @ de4662a651ca2a6f5b5ef0f6f1f1f69ad45fcc6f
```

At this Pass-5 check, `codex/nr` is four commits ahead of that revision. The four-commit diff does not touch the core Before-NR retirement files that control the performance diagnosis. Current fetched blob identities are:

```text
src/NeuralRendering/BeforeUpscale.cpp
432da9b6be167278f2ba2780de2e00384a16b175

src/NeuralRendering/PreparedBeforeUpscale.cpp
013dfc146b121ca0664d84fdc10eafeea79925a6

src/Graphics/D3D11D3D12Interop.cpp
5cb677fe4788ef3e7aea5eb450bcea7aaf326764

src/Graphics/D3D11D3D12Interop.h
57be88c6e1b9075f0db4077baaf2497b7be9820d

src/NeuralRendering/Stage.cpp
8472af23269fc9b44ab8637168fbf28e43bd69ea

src/NeuralRendering/ImagePacket.cpp
b8262fc28fe7cc0346857a345afef1592e0cd38c

src/NeuralRendering/BeforeHost.cpp
414b89e8701befa70d6ae171e7c2ad1db52282d2

src/NeuralRendering/PreparedBeforeUpscale.cpp
013dfc146b121ca0664d84fdc10eafeea79925a6

tools/nr/runtime-pin.json
765f9e36fc2c54b1448ac4971c0cebd8f26aa7a9
```

The RTX40 runtime remains pinned to:

```text
nvngx_dlssnr.dll
size   165840496
sha256 e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a
```

AIO19 uses the different 165,840,496-byte runtime:

```text
sha256 8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206
```

Therefore runtime/device-code remains a valid secondary variable until an exact Dva host-code A/B is run.

## 18. Exact steady-state scheduling comparison

### AIO19 mapped local NR path

```text
D3D11 input work
  -> D3D11 Signal(shared producer fence)
  -> one explicit Flush
  -> D3D12 Queue::Wait(producer value)
  -> acquire reusable slot from persistent 8-slot ring
  -> record/submit NR
  -> D3D12 queue Signal(completion value)
  -> D3D11 Context::Wait(completion value)
  -> queue output copy
  -> host can continue

CPU wait only if:
  - all 8 ring slots are still busy, or
  - exceptional/recovery code requires it.
```

### Current Dva Before-NR path

```text
D3D11 source
  -> decode source to linear FP16
  -> prepare depth + motion
  -> copy color/depth/motion to NR shared resources
  -> SignalProducer() + Flush
  -> Handoff(): second D3D11 signal + Flush
  -> CPU wait for handoff fence completion
  -> Begin(): D3D12 Queue::Wait on producer fence
  -> optional CPU allocator-slot reuse wait
  -> record NR
  -> per-frame descriptor heap creation
  -> full-resolution alpha-preservation dispatch
  -> submit NR
  -> D3D11 Context::Wait for NR completion
  -> queue output copy back to source path
  -> D3D11 signal + Flush
  -> CPU wait for D3D11 output reader
  -> CPU wait for NR completion fence
  -> Drain()
       -> D3D11 signal/Flush where applicable
       -> another Flush
       -> CPU fence retirement
  -> RetireTicket() in the same source frame
  -> encode linear back to source Gamma22/SRGB domain
  -> End(query) + Flush
  -> CPU GetData/Sleep polling until reader completes
  -> FSR starts and decodes source back to linear again
```

The key difference is not simply API count, queue count, or copy count. It is **where ownership is retired**.

AIO19 keeps normal dependencies in queue order and keeps resources alive until later reuse. Dva turns both the producer side and the consumer side into same-frame CPU retirement boundaries.

## 19. New high-confidence finding: Dva double-synchronizes the producer handoff

This is the clearest Pass-5 performance finding.

Current Dva already has a valid GPU-side producer ordering mechanism:

```text
SignalProducer()
  -> SignalD3D11(InteropWork::Upscaling)
  -> D3D11 Signal
  -> Flush

Begin()
  -> WaitD3D12(InteropWork::Upscaling)
  -> ID3D12CommandQueue::Wait(producer fence/value)
```

That is structurally similar to the high-level AIO19 producer handoff.

But Dva then adds a second fence path before recording NR:

```text
Handoff()
  -> D3D11 Signal(second shared fence)
  -> Flush
  -> WaitForSingleObject(...) on CPU
```

The second wait is not only a caller choice. `ValidateImagePacket()` currently requires:

```cpp
const auto completed = p.producerFence->GetCompletedValue();
if (completed < p.producerFenceValue)
    return Error("NR producer ownership has not retired");
```

Therefore Dva's Stage contract **requires the producer fence to be already completed on the CPU before `Stage::Record()` is accepted**.

This is the opposite of AIO19's mapped model, where a pending producer fence is legal because the D3D12 queue waits on it before executing the dependent command list.

### Performance consequence

The render thread cannot record and submit NR while the producer work is merely queued. It must first wait for D3D11 input copies/signals to physically retire.

That destroys D3D11 -> D3D12 overlap and can create a GPU bubble even though Dva already has the queue-side primitive needed to avoid it.

### First architectural correction

Do **not** merely delete `Handoff()`.

Change the ownership contract so a producer fence/value may be **pending but valid**, then require the NR queue to have a matching `Queue::Wait` ordered before the NR list executes.

A safe target contract is:

```text
producer fence identity valid
producer fence value nonzero
producer fence not device-removed
queue Wait(producer fence,value) has been enqueued
resource lifetime extends through that dependency
```

instead of:

```text
producer fence must already be completed before command recording
```

This change needs a focused runtime probe because the vendor Evaluate call must tolerate command recording while producer resources are still pending. The static command-queue model supports it; runtime validation must confirm the exact NR runtime does not perform an unexpected CPU-side resource-content dependency.

## 20. New high-confidence finding: Dva's NR pipeline depth is structurally one

`D3D11D3D12Interop` owns:

```text
kCommandSlots = 3
```

but the higher-level `Stage` owns exactly one:

```cpp
std::unique_ptr<Pending> pending;
```

and `Stage::Record()` explicitly rejects a new recording when `pending` is non-null:

```text
"NR prior recording/readers have not retired"
```

Current `BeforeUpscale::Evaluate()` then waits for the output reader, waits for completion, calls `Drain()`, and calls `RetireTicket()` before it returns.

Therefore the three allocator slots do **not** currently mean three NR frames can remain outstanding. In the present Before path the effective Stage-level outstanding depth is one.

### AIO19 contrast

AIO19 has eight persistent command/work slots and scans all eight for a completed reusable slot before blocking. It only enters the `WaitForSingleObject(INFINITE)` backpressure path when all candidates remain busy.

The correct comparison is therefore:

```text
Dva current:
    one Stage pending ticket
    same-frame retirement
    3 allocator slots mostly unable to create NR overlap

AIO19:
    8 persistent work slots
    deferred slot retirement
    CPU wait only after slot-pool exhaustion in the mapped normal backend
```

Do not infer that Dva needs exactly eight slots. A 3- or 4-slot retained NR ring may be sufficient. The important property is **deferred retirement**, not copying the number eight.

## 21. Post-NR serialization is also harder than necessary

Dva correctly queues the D3D11 consumer dependency with:

```text
WaitConsumer()
  -> ID3D11DeviceContext4::Wait(...)
```

and then queues the output copy.

That part is conceptually good: the copy can be ordered on the D3D11 GPU timeline.

However, immediately afterward Dva performs:

```text
D3D11 signal + Flush
CPU Wait(output reader)
CPU Wait(NR completion)
Drain()
RetireTicket()
```

and `PreparedBeforeUpscale` later performs:

```text
End(D3D11_QUERY_EVENT)
Flush
GetData(DONOTFLUSH) loop
Sleep(1)
```

before the frame continues.

### Performance consequence

AIO19 can leave the D3D11 wait/copy queued and return host control while resources remain owned by the backend slot.

Dva instead proves completion immediately so it can free/reuse the single pending ticket and single prepared resource set. This makes correctness easy but prevents pipelining.

### Correct target

Keep per-slot ownership alive until a later slot-reuse check:

```text
slot N owns:
  color/depth/motion/output shared textures
  command allocator/list
  descriptor heap/descriptors
  NR ticket
  completion fence target
  D3D11 reader fence target
```

Then:

```text
frame N:
  queue producer dependency
  submit NR N
  queue D3D11 consumer wait/copy
  record reader fence value
  return without CPU retirement

frame N+1:
  use another free slot

when slot N is considered for reuse:
  GetCompletedValue()
  if both producer/NR/reader ownership is retired -> reset/reuse
  otherwise scan another slot
  CPU wait only if no slot can be reused
```

This is the closest architectural lesson from AIO19.

## 22. Flush-count interpretation

Current Dva Before execution can reach roughly six explicit D3D11 Flush calls around one active NR source frame:

```text
1. SignalProducer -> SignalD3D11 -> Flush
2. Handoff -> Flush
3. output-reader signal -> Flush
4. Drain consumer signal -> Flush
5. Drain unconditional Flush
6. PreparedBefore FinishReaders -> Flush
```

This count is useful, but it must not be misinterpreted as six independent full GPU stalls. A Flush submits queued D3D11 work; the actual stall is caused by the CPU waits/query polling tied to these submission points.

The real optimization objective is therefore:

```text
reduce forced CPU completion boundaries
```

not simply:

```text
delete every Flush
```

AIO19 itself performs a producer Flush before its D3D12 queue wait.

## 23. Queue topology — corrected priority

Earlier analysis put "independent NR and FSR D3D12 queues" too high in the ranking.

Current Dva `BeforeHost` does create a dedicated DIRECT queue for NR even when it reuses the presenter's D3D12 device:

```cpp
s.contract.device = presenter;
...
s.contract.device->CreateCommandQueue(&q, &s.contract.queue);
```

But AIO19 RE shows that NR and FSR are owned by separate backend queue domains and their final COM queue pointers are not proven to be identical.

Therefore:

> Separate queues are not inherently the performance bug. The expensive design is forcing CPU/D3D11 retirement between queue domains.

A fast architecture can use separate queues if all cross-queue/API dependencies are represented by GPU-side fences and resource lifetime is retained asynchronously.

### Revised priority

```text
1. remove same-frame CPU retirement
2. retain per-slot resources/tickets
3. remove redundant preparation/conversion
4. only then consider queue unification if profiling still shows queue-boundary cost
```

## 24. Resource and bandwidth overhead

Even after synchronization is fixed, Dva still performs more explicit steady-state work than the mapped AIO19 path.

### Dva currently performs

```text
source color -> decode to linear FP16
source depth -> prepared R32
source motion -> prepared RG16F

prepared color -> NR shared color copy
prepared depth -> NR shared depth copy
prepared motion -> NR shared motion copy

NR output -> D3D11 source copy-back

linear -> original Gamma22/SRGB encode

FSR then decodes original source domain -> linear again
```

At 2560x1440, the nominal texture sizes are approximately:

```text
RGBA16F : 29.5 MB
R32     : 14.7 MB
RG16F   : 14.7 MB
```

The raw bandwidth is unlikely to explain the entire observed gap on an RTX 4080 SUPER. The larger issue is that the copies/conversions sit on synchronization boundaries and break overlap.

### Optimization after synchronization

Use one prepared-domain pipeline where possible:

```text
Skyrim source
  -> one decode / guide preparation
  -> linear color + depth + motion
  -> NR
  -> FSR
  -> final delivery conversion only when actually required
```

Do not force:

```text
Linear NR output -> Gamma22 -> Linear FSR input
```

when the next stage can consume the linear result directly.

## 25. Per-frame descriptor allocation and alpha pass

Current `Stage::Record()` creates a shader-visible descriptor heap for every NR record:

```cpp
CreateDescriptorHeap(...)
```

and then dispatches a full-resolution custom compute shader to copy the original alpha channel into the enhanced output.

These are real steady-state costs, but they are lower-priority than synchronization because neither explains the loss of CPU/GPU overlap.

### Target

Move descriptor ownership into each persistent slot:

```text
slot.descriptorHeap
slot.SRV
slot.UAV
```

and investigate whether alpha restoration can be:

- merged into an already-required delivery/conversion pass;
- preserved through the NR path without a separate full-image dispatch; or
- performed only on routes that actually require source alpha preservation.

Do not remove alpha preservation without HUD/transparency validation.

## 26. Runtime DLL remains an unresolved performance variable

AIO19 and Dva use different exact RTX40 NR runtimes:

```text
AIO19: 8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206
Dva:   e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a
```

The host-side architecture now provides enough evidence to explain a large performance gap, but it is still incorrect to attribute 100% of the difference to host integration without a runtime A/B.

### Highest-value discriminating test

Run the same Dva executable and scene with only the exact NR runtime profile changed:

```text
A: current Dva e67dee...
B: AIO19 8270...
```

Measure:

```text
CPU frame time
GPU frame time
NR GPU duration
source-frame FPS
PresentMon cadence
GPU utilization
```

Interpretation:

```text
B ~= A
  -> host scheduling/copy architecture dominates

B materially faster
  -> runtime device code contributes substantially

B somewhat faster but still behind AIO19
  -> both runtime and host architecture contribute
```

This test should be performed before attributing an exact millisecond number to any single Dva code path.

## 27. What the existing Dva smoke data actually proves

The earlier same-session Dva smoke sequence measured approximately:

```text
NR OFF:
55.98 FPS
17.87 ms/source frame

NR ON window 1:
33.48 FPS
29.86 ms/source frame

NR ON window 2:
35.39 FPS
28.26 ms/source frame
```

So enabling the current Dva NR path corresponded to roughly:

```text
+10.4 to +12.0 ms/source frame
```

in that smoke sequence.

This number includes the NR model plus Dva's integration overhead. It is **not** a measurement of pure synchronization overhead and is **not** an AIO19-vs-Dva delta.

A user-observed 10–15 FPS difference also does not map to one fixed millisecond value. Examples:

```text
60 -> 45 FPS = +5.56 ms/frame
55 -> 45 FPS = +4.04 ms/frame
50 -> 35 FPS = +8.57 ms/frame
```

Therefore the correct statement is:

> Dva's measured NR-on frame cost is large enough that recovering several milliseconds through overlap/scheduling changes is plausible, but the exact recoverable share requires instrumented A/B testing.

## 28. Revised performance cause ranking

### Tier 1 — structural throughput limiters

1. **Producer fence must be CPU-complete before Stage recording.**  
   Very high confidence. This directly prevents D3D11 producer / D3D12 NR overlap.

2. **One Stage pending ticket plus same-frame retirement.**  
   Very high confidence. This makes the effective NR pipeline depth one even though the interop layer owns three allocator slots.

3. **Post-NR CPU retirement before continuing.**  
   Very high confidence. Output-reader wait, completion wait, Drain/RetireTicket, and later D3D11 query polling prevent deferred ownership.

### Tier 2 — significant extra work

4. **Duplicate prepared color/depth/motion copies and copy-back.**  
   High confidence. The copies are real; their exact GPU cost requires timestamps.

5. **Linear -> source transfer function -> linear round trip before FSR.**  
   High confidence for current Gamma22/SRGB prepared path; medium confidence for its exact contribution.

6. **Different NR runtime device code (`e67dee` vs `8270`).**  
   Performance contribution unknown until A/B; potentially significant.

### Tier 3 — secondary steady-state overhead

7. **Per-record descriptor heap allocation.**  
   Proven. Expected to be smaller than synchronization losses.

8. **Full-resolution alpha-restoration compute pass.**  
   Proven. Correctness-motivated; exact GPU duration needs timestamps.

9. **Separate NR and FSR queue objects.**  
   Lower priority than previously stated. Separate queues can perform well if dependencies remain GPU-side.

## 29. Recommended Dva rewrite order

### Phase 1 — change the producer contract

Goal:

```text
no CPU completion requirement before NR command recording/submission
```

Work:

- replace `ValidateImagePacket()`'s completed-value requirement with identity/lifetime validation suitable for a pending producer fence;
- ensure the NR queue receives the producer `Queue::Wait` before dependent execution;
- remove the redundant second producer handoff fence if it no longer serves another lifetime contract;
- preserve device-removal/fence-value validation.

Acceptance:

```text
steady-state Before NR has no producer WaitForSingleObject
```

### Phase 2 — make Stage multi-pending

Replace:

```text
std::unique_ptr<Pending> pending
```

with a small retained slot ring.

Each slot owns:

```text
packet/resources
NR ticket id
command allocator/list ownership
completion values
D3D11 reader value
descriptor heap/views
```

Start with 3 or 4 slots; make saturation telemetry visible before deciding whether 8 is useful.

Acceptance:

```text
frame N can remain owned while frame N+1 is recorded/submitted
```

### Phase 3 — remove steady-state Drain/RetireTicket waits

Do not call `Drain()` on every evaluated source frame.

Use it only for true lifetime barriers:

```text
resize
shutdown
runtime recreation
device transition/error recovery
```

Retire tickets lazily when their slot is reused and all reader/completion fences are complete.

Acceptance:

```text
steady-state Evaluate returns with an outstanding retained ticket
```

### Phase 4 — remove the prepared-reader query stall

`PreparedBeforeUpscale::FinishReaders()` should not block the CPU each active frame merely to prove that the source delivery write has retired.

Either:

- make the following FSR path consume the retained prepared/NR output directly; or
- track the final D3D11 reader fence in the same slot lifecycle and retire it later.

Acceptance:

```text
no per-frame GetData/Sleep completion loop before FSR
```

### Phase 5 — keep NR -> FSR in one prepared color domain

Target:

```text
source -> Linear -> NR -> FSR -> final encode
```

instead of:

```text
source -> Linear -> NR -> source encoding -> Linear -> FSR
```

### Phase 6 — persist descriptors and optimize alpha

- allocate descriptor heaps per retained slot;
- reuse SRV/UAV descriptors;
- profile/merge the alpha restoration pass where safe.

### Phase 7 — only then test queue unification

If GPU timestamps still show an expensive NR->FSR queue boundary after CPU retirement is removed, experiment with sharing a queue. Do not make this a prerequisite for the asynchronous rewrite.

## 30. Instrumentation required for a valid AIO19-vs-Dva comparison

For the same save, camera position, resolution, SR quality, FG state, NR preset and runtime DLL, record at least 300 source frames after warmup.

### CPU events

```text
BeforeHost::Evaluate total
Prepared decode
Depth preparation
Motion preparation
CopyInput x3
SignalProducer
Handoff wait
Stage::Record
Submit
output-reader wait
completion wait
Drain
RetireTicket
encode
FinishReaders query wait
FSR host submission
```

### GPU events

```text
D3D11 producer completion
D3D12 NR queue start/end
NGX NR evaluate GPU duration
alpha restoration duration
D3D11 output copy
FSR preparation
FSR dispatch start/end
Present
```

### Lifetime/ring telemetry

```text
slot index
slot target values
GetCompletedValue at reuse
number of free slots at submit
saturation count
CPU backpressure wait count/duration
oldest outstanding ticket age
```

### Report

Use:

```text
p50
p95
p99
max
```

for CPU waits and frame time. Average FPS alone can hide rare ring saturation and long retirement spikes.

## 31. Performance acceptance criteria for the rewrite

A successful Dva optimization should show all of the following before image-quality work is considered complete:

```text
steady-state producer CPU fence waits:       0
steady-state Drain() calls per NR frame:     0
steady-state RetireTicket() same-frame use:  0
steady-state FinishReaders polling loops:    0
steady-state descriptor heap creation:       0
normal ring saturation events:               near zero
```

Expected remaining synchronization is primarily:

```text
D3D11 producer Signal + required submission Flush
D3D12 queue Wait
D3D12 completion Signal
D3D11 Context Wait
lazy slot-reuse completion checks
```

This is much closer to the AIO19 ownership model while preserving Dva's explicit correctness tracking.

## 32. Final performance conclusion

Pass 5 strengthens the original conclusion and narrows the primary cause:

> **AIO19 is faster mainly because it treats NR as queued GPU work whose resources remain owned after submission. Current Dva treats NR as a transaction that must be proven complete on the CPU before the frame can continue.**

The most performance-critical Dva differences are now directly visible in current source:

```text
producer fence must already be complete before Stage::Record
+
one pending Stage ticket only
+
same-frame output/completion waits
+
Drain + RetireTicket before return
+
prepared-reader query polling before FSR
```

The Dva path already contains the GPU-side building blocks needed for a better design (`D3D12 Queue::Wait`, D3D11 `Context::Wait`, multiple allocator slots). The optimization is therefore less about inventing new primitives and more about changing lifetime rules so those primitives are allowed to work asynchronously.

The most important implementation target is:

```text
QUEUE dependency
-> SUBMIT NR
-> QUEUE consumer dependency/copy
-> RETURN CPU control
-> KEEP slot/resources alive
-> RETIRE only on later reuse or real lifecycle boundary
```

Do **not** optimize in this order:

```text
force one NR+FSR queue
remove all copies
micro-optimize descriptor calls
```

until the hard CPU retirement boundaries are gone.

Finally, the different RTX40 NR runtime remains a mandatory A/B variable. The static host architecture can plausibly explain a large share of the AIO19 performance advantage, but only a same-host runtime A/B plus GPU timestamps can determine how much of the remaining gap belongs to `8270...` versus `e67dee...` device code.
