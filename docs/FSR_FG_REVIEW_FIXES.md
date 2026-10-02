# FSR FG implementation-review fixes

Reviewed baseline: `7f4ac08539b9cf655bfd2d0e6fc0f8c344daeb34`. Input: the user's `DvaKolbas_FSR_FG_Implementation_Review_2026-10-02.md`. Recommendations were checked against the source and pinned AMD implementation before applying the user's authorization to fix the useful findings.

## Findings and decisions

| Finding | Verification and action |
| --- | --- |
| P1 swapchain replacement during resize | Confirmed. Retain the AMD swapchain context/object, native D3D11/D3D12 devices, bridge, runtime and HWND ownership. Retire readers, rebuild fixed-size SR/FG resources and invoke ResizeBuffers on that same chain. |
| P1 runtime SDR contract bypass | Confirmed. Reject PQ/scRGB, nonempty HDR metadata and exclusive fullscreen before forwarding. CheckColorSpaceSupport reports no support for unavailable spaces. Ordinary and SR-only presenters retain their existing routing. |
| P2 rejected-source submission | Not present at baseline: `decision.admit == false` already returned before transport Upload, Configure or Present. Added a regression asserting no SDK/native submission and intact prior source state. |
| P2 source identity | An acceptance risk, without a reproduced divergence. Keep the shared Present-based SR/jitter/FG identity and add bounded engine count, capture count, camera, guide generation and configured/prepared ID correlation. Log failed submission snapshots as well. |
| P2 empty/minimized extent | The old translator rejected it before retiring the presenter; terminal reconstruction was overstated. Add explicit suspension with SDK/UI/producer retirement. Resume retained SR/scene/game buffers when the client becomes nonempty, reset history, and let an explicit subsequent ResizeBuffers handle a changed resolution. No zero-sized feature is created. |
| P3 repeated waits | Safe conservative synchronization. Defer splitting the API: the first wait protects guide overwrite and the later wait protects scene/UI publication. Existing lifetime tests remain in force. |
| P3 duplicate generation callback | Add fail-closed handling before a second SDK dispatch within a Present transaction. |
| Future FSR4 profiles | Defer to the separate FSR4 milestone. This change retains the pinned analytical provider identities and rate policy. |

## Lifetime and failure behavior

Resize stops admission, disables/detaches generation, unregisters UI, waits for AMD presents, and seals/drains producer/Prepare/copy work before destroying fixed-size feature resources. The AMD chain is never replaced during this operation. After ResizeBuffers, resources are built from its actual GetDesc, including on a rejected native resize; the caller still receives the native failure HRESULT. A failed retirement or reconstruction retains owners and closes admission rather than inventing completion.

The real runtime exposed an additional assumption: successful DestroyContext does not necessarily clear the caller's context variable. TRP now clears the handle explicitly after successful destruction. A runtime-double regression preserves the stale variable deliberately and verifies safe reentry.

Immutable waitable-object and tearing flags are checked before forwarding. The pinned AMD implementation updates its reported flags before the native resize result, so rejecting these unsupported changes locally prevents descriptor mutation on a failed request.

Suspension retains the scene, SR feature, guide transport and game buffer identities. Reentry requires proven SDK retirement, reopens the existing session, clears temporal eligibility, and requires reset preparation before generated frames resume. Production polling runs at background/source evaluation and both outer Present entries. Invalid Present1 dirty/scroll requests are rejected before restoration or source consumption.

## Verification

Build/test receipts are in [FSR_FG_REVIEW_VALIDATION.json](FSR_FG_REVIEW_VALIDATION.json), including the reviewed base, hashes of all changed source/test files, exact commands, binary identities and raw-report/log hashes. All results are local evidence; this record makes no independent CI claim. Three Graphics Tools-dependent tests remain excluded under the user's earlier instruction: NativeUIComposition, NativeUIBlendState and NeuralPeripheralPixels. AMD/Intel hardware, Skyrim minimize/restore, color calibration and game source-ID correlation remain gameplay acceptance items.

A targeted read-only reviewer checked the final diff, including restoration without a second resize, failed-resize states, callback/UI/Prepare retirement and production restoration polling. No confirmed issues remained. The initial review's larger-extent coverage gap was closed by the additional real SR/FG transactions below. This review does not substitute for the pending whole-plan Task 8 review or Skyrim gameplay acceptance.

The expanded real AMD/ReShade probe passed: 96 baseline sources/SR passes, 24 baseline generation callbacks, 96 completed HUD checks, two larger/restored resize cycles, 80 additional temporal sources at 640x360, 22 larger-resolution callbacks and two suspension/restoration cycles. A cached pre-resize waitable handle was exhausted and subsequently signaled by the retained AMD presenter; maximum latency and chain/device identity survived. The larger source transactions check configured/prepared IDs and generation reset after restoration. Physical cadence is outside this hidden probe's scope.

The final compatibility matrix passed 498 runnable checks: Standard FG/NR off 133, Universal FG/NR on 133, Standard SR-only/NR off 121, and Standard FSR-disabled/NR on 111. The GPU smoke submitted 2,000 sources across 50 retired contexts with 550 callbacks, 250 generated-image readbacks, 1,225 UI sentinel checks and zero SDK warnings/errors. After commit `aac2814ad2307ed90096d923cb6caaef76f3fe84`, the Standard NR-off delivery DLL was rebuilt with a clean embedded source marker. Its separate staged FG package and extracted ZIP passed independent pins, settings, license, normal/delayed imports and `-RequireCleanSource` validation. [FSR_FG_REVIEW_PACKAGE.json](FSR_FG_REVIEW_PACKAGE.json) records the final binary/ZIP identities. All 23 changed source/test blobs match the code that passed the matrix; the later delivery rebuild changes the embedded source revision rather than product behavior.

Task 8 remains incomplete. The installed V5.4 NO-LORE trial and MO2 settings are unchanged during these fixes; its installed receipt still identifies the older DLL. A newly staged patched package must be installed separately with MO2 and Skyrim closed before new gameplay evidence can apply to this revision. Seven of eight milestones are complete.
