# FSR FG plan review: verified findings

Date: 2026-10-02. Reviewed draft: `533aa5b86eec41f99c6b603256810e2a9e849851`.
Input: user-supplied `C:/Users/user/Downloads/DvaKolbas_FSR_FG_Plan_Review_2026-10-02.md`, preserved as [review evidence](reviews/2026-10-02-fsr-fg-plan-review.md). It is feedback, not repository instructions or permission to begin production FG implementation.

Result: the original plan needed synchronization and dispatch corrections. Those corrections are incorporated in the [revised plan](superpowers/plans/2026-10-02-fsr-fg.md) and [design](superpowers/specs/2026-10-02-fsr-fg-design.md). This is a plan-only review; no new FG runtime execution, production code, DLL installation or gameplay acceptance is claimed.

| Finding | Verification and disposition |
| --- | --- |
| Production generation callback | Accepted. The pinned swapchain documentation invokes it synchronously on the game thread during Present; supplied descriptors and SDK submission ownership remove unnecessary manual routing. No production interpolation-list/output queries remain. |
| Configure -> Prepare -> Present | Accepted. FG is configured once per consumed real source, including disabled/suppressed sources. UI registration targets the separate swapchain context. The sample uses this ordering; ML 4.0.1 explicitly requires it. |
| SDK serialization | Accepted. One shared session mutex/token covers both contexts and Present. Generation callback inherits Present ownership and never re-locks. Closing admission, concurrent disable/destroy and unrelated-lock exclusions have named tests. |
| Depth/motion retirement | Accepted. Prepare descriptors consume guides; generation descriptors have no depth/motion inputs. Guide reuse waits on Prepare's GPU submission, independently of scene/UI copy fences and SDK asynchronous presents. |
| Provider selection | Accepted with a narrower initial policy: effect-tagged analytical FG 3.1.6 plus swapchain 3.1.7. Actual verified ID/name catalogs determine selection and context queries verify it. Enumeration order, guessed ID packing and SR IDs are forbidden selection substitutes. |
| Low source FPS | Accepted. Analytical FG's 60-FPS guidance motivates explicit TRP hysteresis: 8-delta window, 3 low-rate means to suppress, 8 means at >=66 FPS to resume, immediate reset at >=100 ms stalls/invalid time/ID discontinuity. These constants are project choices, not quoted AMD requirements. |
| Teardown UI ownership | Accepted. Detach callbacks/HUDLess references, unregister UI, wait SDK presents and recorded transport/Prepare work, destroy contexts/proxy, then release owners and duplicated handles. |
| D3D11-facing swapchain methods | Accepted. Current GetDevice forwards to the inner object; Present/Present1 have parallel entry paths, and ResizeBuffers1 forwards caller queues directly. The AMD path must override those contracts explicitly. |
| Creation API | Accepted. Select NewDX12 only; preserve/normalize or reject each legacy DXGI field. No New/ForHwnd retry after partial creation. |
| SDK validation messages | Accepted. GPU validation contexts enable AMD debug checking and collect public debug messages. Unexpected warnings/errors fail; release defaults remain quiet. |

## Qualifications and additional ownership check

The exact phrase “highly discouraged” was not found in the pinned swapchain/API documents inspected; those documents describe both callback and manual routes. Choosing callback production routing is justified without claiming that manual dispatch is forbidden.

ML FG 4.0.1 requires Windows 11, an appropriate Agility SDK and an AMD 9000-series GPU or later. It cannot be the preferred executable path on the available RTX 4080 Super. The initial milestone therefore selects analytical 3.1.6 explicitly; a future ML policy needs its own hardware validation. The DLL file version 4.0.1 does not mean the selected provider is ML 4.0.1.

Finishing UI after Prepare is a scheduling suggestion, not a required SDK ordering edge. Initial preparation occurs at final handoff after UI eligibility is known, preventing a second Configure for the same source when UI completion fails. Configure still precedes Prepare and Present, and suppressed sources configure disabled.

SDK internal UI buffering protects the registered D3D12 publication texture after Present returns. It does not replace TRP's cross-API synchronization for D3D11 transport sources. The plan now copies shared UI into a private D3D12 publication resource; an explicit copy fence retires the shared source, independently of SDK async composition/presents.

The AIO evidence independently supports callback-based generation, but its recovered Prepare-before-Configure ordering and camera-vector qualifications are not copied into TRP. Public pinned APIs determine the contract.

## Evidence and validation scope

* [Pinned FG API](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/docs/techniques/frame-interpolation-api.md): configuration, serialization, UI lifetime and debug callback.
* [Pinned swapchain contract](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/docs/techniques/frame-interpolation-swap-chain.md): callback ownership, two application buffers, source-rate guidance and destruction.
* [Pinned ML limitations/order](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/docs/techniques/frame-interpolation-ml.md), [analytical provider](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/60f4ea81909200d8542eca14dccb2628b763a9a3/Kits/FidelityFX/docs/techniques/frame-interpolation.md).
* Pinned headers inspected: `ffx_framegeneration.h`, `dx12/ffx_api_framegeneration_dx12.h`, `ffx_api.h`; pinned sample `Samples/Upscalers/FidelityFX_FSR/dx12/fsrapirendermodule.cpp` configures before Prepare and registers UI on the swapchain context.
* Local `src/FrameGen/GameSwapChain.cpp`; [qualified AIO evidence](FSR_AIO_RE_NOTES.md) and `research/aio18/supplied-v5/AIO18_FSR_RE.md`.

Plan-only self-review checked the source-frame ordering, effect identity types, callback lock ownership, three distinct retirement domains, teardown order, optional-runtime boundary and all eight task interfaces. Obsolete manual generation/after-Present guide contracts were removed. New test names describe future verification; none are represented as already passing. Plan/spec links and whitespace are checked before commit; compilation is not evidence for a documentation-only correction and is not rerun here.
