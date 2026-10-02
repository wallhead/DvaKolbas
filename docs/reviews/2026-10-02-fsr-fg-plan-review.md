# DvaKolbas FSR Frame Generation Plan Review

**Repository:** `wallhead/DvaKolbas`  
**Plan commit:** `533aa5b86eec41f99c6b603256810e2a9e849851`  
**Plan:** `docs/superpowers/plans/2026-10-02-fsr-fg.md`  
**Reviewed:** 2026-10-02

## Assessment

The plan has a strong overall architecture: preserve Skyrim's D3D11 producer identity, use the official AMD D3D12 interpolation swapchain, keep FSR FG optional, retain a dedicated native-resolution UI path, and validate in a standalone fixture before touching the installed Skyrim profile.

I would **not execute the plan unchanged**. Four design points should be corrected first because they directly affect correctness, synchronization, and race safety.

## P1 — Use the AMD frame-generation callback in production

The current plan makes manual/direct frame-generation dispatch the primary route: it proposes querying the interpolation command list/output and calling a `Generate(...)` method itself.

AMD's pinned SDK says the recommended path is to configure `frameGenerationCallback`; the swapchain invokes this callback during `Present`, supplying the correct `ffxDispatchDescFrameGeneration`, and the callback calls `ffxDispatch`. AMD describes manual interpolation dispatch as **highly discouraged**, intended for engines where dispatch from Present is unsafe.

AIO18 also follows the Present-driven callback architecture.

**Plan change:** make the callback route the production design.

```text
AMD Present
  -> TRP frameGenerationCallback
       -> ffxDispatch(frameGenerationContext, supplied descriptor)
```

Do not query interpolation command-list/output on the normal production path. Keep a manual route only as an experimental fallback if a real fixture proves the callback route cannot work in TRP.

Add tests:
- `PresentInvokesGenerationCallbackExactlyOnce`
- `DisabledPresentDoesNotInvokeGenerationCallback`
- `CallbackDescriptorFrameIdMatchesConfiguredId`
- `NoInterpolationCommandListQueryOnProductionPath`

## P1 — Make Configure -> PrepareV2 -> Present ordering explicit

The plan currently exposes `Prepare()` before `Configure()` and describes frame 41 as mapping to "prepare/configure/dispatch 41". That is too ambiguous.

The AMD contract is:

```text
ffxConfigure(FrameGenContext), frameID=N
    ->
ffxDispatch(PrepareV2), frameID=N
    ->
AMD SwapChain::Present()
    ->
frameGenerationCallback -> ffxDispatch(generation), frameID=N
```

`ffxConfigure` must occur once per source frame and the frame ID must advance exactly by one.

The separate UI-resource registration is a configure call on the **swapchain context**, not the same operation as the per-frame frame-generation configuration.

**Plan change:** separate these operations explicitly:
1. Configure FG context.
2. Dispatch PrepareV2.
3. Finish native UI.
4. Register completed UI with swapchain context.
5. Copy HUD-less scene to AMD application backbuffer.
6. Present; callback performs generation dispatch.

When FG is suppressed for a menu/loading/spatial frame, configure the frame-generation context with `frameGenerationEnabled=false` rather than silently leaving an undefined per-frame configuration gap. Define frame-ID advancement deliberately.

Add tests:
- `ConfigurePrecedesPrepare`
- `ConfigureExactlyOncePerSourceFrame`
- `SuppressedFrameConfiguresDisabled`
- `PrepareAndCallbackUseConfiguredFrameId`
- `UiRegistrationIsSeparateFromFrameConfigure`

## P1 — Define the SDK lock protocol before implementation

AMD documents the frame-generation and swapchain contexts as not guaranteed thread-safe. It explicitly identifies races between Present, PrepareV2, and destruction.

The plan says to follow the API thread-safety rules but does not define an actual locking protocol.

A naive mutex can deadlock if TRP holds it around `Present()` and the generation callback tries to acquire the same mutex.

**Plan change:** add one explicit FG SDK serialization lock and a call ownership table.

Recommended first-stage rule:

```text
Acquire FG SDK lock around:
- FG context create/destroy
- swapchain context create/destroy
- per-frame FG Configure
- PrepareV2 dispatch
- swapchain UI-resource Configure
- relevant swapchain queries/configuration
- Present while a generation callback is installed

Generation callback:
- executes while Present owns the SDK lock
- MUST NOT reacquire the lock
- calls only ffxDispatch with the supplied descriptor
- touches no Skyrim, ImGui, or D3D11 immediate-context state
```

Never enter a blocking AMD Present/Configure/WaitForPresents path while holding unrelated host/UI/settings locks that an AMD worker or callback might need.

Add stress tests for Present-vs-Prepare, Present-vs-disable, and Present-vs-destroy.

## P1 — Do not tie depth/motion lifetime to Present

The proposed `RecordGuideReaderAfterPresent()` conflates two lifetimes.

Depth and motion are read by **PrepareV2**. They are not inputs to the final generation dispatch. They should become reusable when the GPU submission containing PrepareV2 has retired.

Scene and UI have different lifetimes:
- the HUD-less scene is copied to the AMD application backbuffer for Present;
- with `ENABLE_INTERNAL_UI_DOUBLE_BUFFERING`, the registered UI can be reused after `Present()` returns;
- registration alone is not the UI ownership boundary.

**Plan change:** replace `RecordGuideReaderAfterPresent()` with a dedicated Prepare-retirement fence/value:

```text
D3D11 writes depth/MV
 -> producer fence
D3D12 waits
 -> Configure + PrepareV2
 -> submit main-queue work
 -> signal prepare-retirement fence
 -> next D3D11 source frame waits only on that value before rewriting guides
```

Do not use a main-queue fence to claim that the SDK's asynchronous UI/present queue has retired.

Add tests:
- `DelayedPrepareReaderBlocksDepthOverwrite`
- `PrepareRetirementDoesNotWaitForPresent`
- `UiRegistrationAloneDoesNotPermitReuse`
- `UiMayReuseAfterPresentReturnsWithInternalBuffering`
- `MainQueueFenceDoesNotClaimAsyncPresentRetirement`

## P2 — Make FG provider selection deterministic

The plan discovers separate SR, FG, and swapchain providers, but uses the same untagged `ProviderInfo` and does not state which FG provider is selected.

Do not let enumeration order become policy.

Use an effect-tagged provider identity or distinct types, and define explicit policy such as:
- prefer FSR FG 4.0.1 when supported;
- optionally fall back to analytical FG 3.1.6;
- always query/report the actual created provider;
- never reuse an SR provider ID for FG or swapchain creation.

## P2 — Add low-input-FPS suppression

Positive finite frame time is not enough for FG quality.

AMD documents FSR FG 4.0.1 as designed for roughly 30 FPS or higher input and analytical 3.1.6 around 60 FPS; low source rates can produce severe artifacts.

Add provider-aware suppression/hysteresis:
- very long/stalled frame -> disable generation and reset re-entry;
- low input FPS -> suppress FG but keep SR active;
- resume only after several consecutive valid source frames.

## P2 — Explicitly unregister UI during teardown

Before releasing the UI transport resource:
1. stop new source submissions;
2. configure FG disabled and clear callback/HUD-less references as appropriate;
3. configure the swapchain UI resource to null / flags 0;
4. wait for outstanding interpolation/UI/present work;
5. retire TRP's Prepare/main-queue work;
6. destroy FG context;
7. destroy swapchain context;
8. release final proxy-swapchain COM reference;
9. release transport/runtime ownership.

If TRP itself calls `GetFrameLatencyWaitableObject()`, close every duplicated handle only after the swapchain context has been destroyed.

## P2 — Specify AMD-path IDXGISwapChain4 semantics

The outer `GameSwapChain` remains D3D11-facing while its AMD inner swapchain is D3D12. The plan should explicitly define:
- `GetDevice`: return/query the retained D3D11 producer, never forward D3D12 identity to Skyrim.
- `Present` and `Present1`: one shared FG source-frame boundary.
- `ResizeBuffers1`: do not blindly forward D3D11 `pPresentQueue` objects as D3D12 queues.
- normalize application-visible backbuffer count consistently (two is sufficient for the AMD FG swapchain).

## P2 — Choose one swapchain creation path

`NewDX12` and `ForHwndDX12` are separate creation APIs. Pick one authoritative translation path for the first milestone and test every DXGI field.

Do not "try NewDX12, then ForHwnd" after partial creation: DX12 permits only one swapchain per HWND and retrying around a partially-owned context complicates failure recovery.

## P2 — Enable AMD debug checking in the GPU fixture

Task 6 should create the FG context with the SDK's debug checking flag in validation builds and capture the public debug callback. Unexpected warnings about frame IDs, camera data, dimensions, resources, or state should fail the fixture unless explicitly waived.

Do not enable it by default in release packages.

## What should stay unchanged

The plan already gets many important things right:
- FSR FG is compile-time optional and SR-only remains usable.
- Skyrim keeps a real D3D11 game-facing producer.
- No D3D12 backbuffer is cast to D3D11.
- AMD owns final pacing/presentation.
- Dedicated UI, premultiplied alpha, and SDK internal UI buffering are explicit.
- Gamma 2.2 is converted instead of mislabeled as sRGB.
- camera position/orientation is planned explicitly.
- source IDs and time units are explicit.
- loading/spatial/invalid frames suppress generation.
- async compute is deferred.
- standalone 1000-frame / 25-recreation testing happens before Skyrim installation.
- callback instrumentation is not confused with production automatic UI mode.
- hidden-window timing is not claimed as physical scanout.
- device-loss/timeout paths preserve ownership rather than fabricating completion.
- ReShade ownership remains an explicit gate.
- FG is staged as a separate package with the working SR build retained for rollback.

## Recommended production frame sequence

```text
Source frame N
  Skyrim D3D11 reduced scene
       |
       +-> depth / motion / camera / jitter / frameID N
       |
       +-> one ReShade source-effects pass
       |
       +-> FSR SR -> native HUD-less image
       |
       +-> lock SDK
       |     Configure FG context(frameID=N, enabled/suppressed)
       |     Dispatch PrepareV2(frameID=N)
       |   unlock
       |
       +-> submit PrepareV2 queue work
       +-> signal PREPARE-retirement fence
       |
       +-> native UI completes
       +-> freeze/convert premultiplied UI
       +-> copy HUD-less native image to AMD application backbuffer
       |
       +-> lock SDK
       |     RegisterUiResource(PREMUL | INTERNAL_DOUBLE_BUFFER)
       |   unlock
       |
       `-> lock SDK
             AMD Present()
               -> generation callback
                    -> ffxDispatch(callback descriptor)
               -> private UI capture/composition
               -> generated-frame pacing
               -> real-frame pacing
           unlock
```

## Codex gate before implementation

Update the plan before Task 1 begins. The revised plan should state unambiguously:

1. Production generation uses `frameGenerationCallback`.
2. Ordering is Configure -> PrepareV2 -> Present/callback.
3. The per-source Configure/frame-ID policy is explicit even during suppression.
4. There is an exact SDK locking table and the callback never re-locks it.
5. Depth/motion reuse is tied to PrepareV2 GPU retirement, not Present.
6. UI is unregistered before its transport resource is destroyed.
7. FG provider selection is deterministic and effect-tagged.
8. Low-input-FPS suppression/re-entry is defined.
9. `Present1`, `ResizeBuffers1`, and game-facing `GetDevice` behavior are specified.

After those edits, perform one more **plan-only review** before writing production FG code.
