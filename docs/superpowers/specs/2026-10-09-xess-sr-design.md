# XeSS SR design — 2026-10-09

Status: proposed written design, awaiting review. Investigation complete; product implementation has not started.

## Intent and scope

Add XeSS upscaling to RaZkolbaS alongside DLSS and FSR. The user selected SR first, Intel FG afterward. Preserve the existing Native render-scale choice, three main tabs, live NR/FG controls and the standard processing order:

```text
Skyrim / ENB scene → XeSS SR → NR After → selected FG provider → UI → output
```

An explicitly selected existing NR Before placement stays at the current pre-upscale hook. No pass runs on generated frames. NR remains limited to its existing supported NVIDIA routes; adding XeSS does not enable AMD/Intel NR.

Initial validation uses SDR and the current ENB source. HDR, dynamic resolution, Intel FG and XeLL are outside this SR implementation. Existing startup defaults and automatic GPU provider choices remain unchanged. XeSS becomes an explicit choice only when the actual SDK/device combination is eligible.

## Approach

Use the official Intel SDK 3.0.2 D3D12 SR dispatcher through RaZkolbaS's existing interop owner. This provides one input/lifetime contract for NVIDIA, AMD and Intel. An Intel-only D3D11 backend would duplicate that integration without covering the local NVIDIA test GPU. Copying AIO19's private wrapper would add unknown assumptions; its bundled runtime is already the public SDK.

The [investigation](../../../research/xess/XESS_INVESTIGATION_2026-10-09.md) records the source pins, SDK requirements and exact AIO19 witness scope. SDK version is not automatically the effective Intel driver implementation version: diagnostics must distinguish dispatcher/API version from any implementation version the runtime can report.

## Ownership and boundaries

Introduce a XeSS runtime loader, SR context owner, frame-contract adapter and host-resource owner under `src/Upscaling`. Keep those responsibilities distinct:

- Runtime loader resolves the required official API exports from the packaged `RaZkolbaS/XeSS/libxess.dll` path. It is optional and loaded only for a requested XeSS mode or deliberate capability probe. No process-global DLL search changes or NR caller-identity shim.
- Context owner creates, builds pipelines, initializes, executes and destroys a single render-thread-owned XeSS context. It reports exact API/result/stage failures. It retains the runtime module through context destruction.
- Frame adapter converts our measured camera, motion, jitter, extents and reset identity into the SDK contract. It contains no presentation ownership.
- Host resources prepare shared linear input/output and guide resources using `Graphics/D3D11D3D12Interop`. They retain all submitted resources until the owner's GPU fence confirms completion.

Use the D3D12 device matching the actual D3D11 rendering adapter LUID. Reuse interop submission and retirement rather than creating an independent device/fence island. Do not conflate frame-generation availability with SR capability, or require NVIDIA Streamline to initialize XeSS on AMD/Intel.

The current `BackendKind`, `UpscaleType`, backend decision and source evaluator need explicit XeSS routing. XeSS must not fall through the non-FSR branch into DLSS/DLAA. Existing `External` classification is insufficient without a real adapter.

## Colour and temporal inputs

Prepare HUD-less input as linear `R16G16B16A16_FLOAT`. Decode the declared SDR source using the existing tested conversion math, then re-encode the reconstructed image into the established downstream SDR handoff. Separate shared colour conversion from FSR-specific validation/messages where necessary; do not rewrite the FSR dispatch or expose FSR masks as XeSS inputs.

For this SDR path set the SDK LDR input flag and use exposure scale 1 without automatic exposure. Treat alpha as opaque scene output and retain the independent HUD layer. Both NR placements must receive the colour encoding they already expect, which is essential to preserve the prior NR colour-drift fix.

Start with low-resolution, undilated `R16G16_FLOAT` motion and corresponding depth. Convert combined camera/object motion to current-to-previous input-pixel units. An extent/convention mismatch is a diagnostic failure, not permission to guess a motion scale. Set inverted-depth and jittered-motion flags only when the producer contract warrants them. Validate whether the currently captured source motion is raw or already dilated before binding it; produce the required undilated guide if it is not raw.

Generate and apply a XeSS-specific temporal jitter contract. Verify X/Y direction, reported sample offset versus camera offset, viewport origin and history reset with synthetic tests. Use the SDK sizing queries and its documented jitter-sequence rule rather than reusing a fixed FSR ratio/sequence length. Returning jitter to Skyrim and submitting it to XeSS must describe the same frame.

AIO19's pinned game-side RE now provides a comparison convention: raw jitter `hx/hy` is stored as `-hx/-hy` for the generic SR frame, alongside resolution-normalized hook values `-2*hx/W` and `+2*hy/H`. Its motion-scale setters receive the dimensions used for the SR input. Use these as reference cases for adapter tests; do not copy the signs into RaZkolbaS without checking our own projection and motion producers. The traced AIO19 path is static evidence and does not resolve its optional colour-processing branches.

Reset history on source-epoch changes, camera cuts, loading transitions, extent changes and context replacement. A missing guide is not a successful temporal frame. Spatial copies/recovery, if invoked by an existing recovery policy, must be labeled separately from XeSS active processing.

## Sizing, settings and menu

Add `Mode = XeSS` to the existing common startup provider selection. Add an organized `[XeSS]` section with its own render-scale choice and source encoding. Initial render-scale choices are `Native`, `Quality`, `Balanced`, and `Performance`, mapping to SDK AA/Quality/Balanced/Performance constants. All sizing comes from SDK queries; Native remains the visible name for output-resolution AA.

Mode and render-scale changes use the current save-and-restart flow. No expensive pipeline build or context reinitialization occurs inside a live menu Apply path. NR and FG off/on remain live controls.

Use the existing upscaler tab and Mode control; do not add a fourth XeSS tab. Report XeSS dispatcher version, active execution state and unsupported reason. Do not reuse the FSR Sharpness slider for XeSS unless a real shared sharpening pass is wired and verified. In the first trial, show only controls that affect XeSS.

A failed early XeSS capability/context check uses the established safe startup fallback only before reduced render targets and jitter are committed, with requested/effective provider logged and shown. If no compatible fallback is available, give a clear startup diagnostic. A failure after committing resources goes through coordinated renderer retirement; never relabel an unprocessed frame as another active upscaler.

## Existing NR and FG combinations

Qualify NR Before and After separately on NVIDIA. Scaled After needs correctly aligned display-space guides and the existing guide validation, including the source frame's jitter convention. Changing SR provider must not bypass those checks.

FG provider selection remains independent of XeSS SR. Initially bring up SR with NR and FG off. Then prove XeSS → existing FSR FG and XeSS → NVIDIA FG on eligible hardware with the same HUD-less output, source-frame identity, camera data and reader-hold contracts used by the other upscalers. Those combinations are targets, not established support from static RE.

No new swap-chain owner is required for SR. Intel FG will later add a separate, exclusive presentation owner with XeLL markers and explicit UI composition. Never stack XeSS FG over another FG proxy. Intel FG has its own generated-frame capability limits and latency ownership; it does not inherit NVIDIA MFG options or Reflex settings.

## Diagnostics and failure recovery

Log requested/effective SR and FG independently; adapter description/LUID/vendor; runtime location/API and reported implementation version; output/input sizes; source encoding; motion/depth/jitter flags; initialization duration; first successful temporal execute; reset reasons; and SDK result codes. Per-frame detail stays behind focused diagnostics.

Pipeline construction occurs during coordinated startup after device availability and before gameplay use. Context/resource destruction requires confirmed retirement of both SR work and downstream readers. A timeout retains ownership and reports recovery state rather than releasing in-use resources. Resize, disable, shutdown, failed initialization and allocation failure need distinct tested cleanup paths.

## Eight SR milestones and acceptance evidence

1. **Investigation and written design:** pinned SDK/upstream/AIO19 evidence and scope; evidence complete, written design pending approval.
2. **Runtime and capability probe:** official API loading and initialization on the actual local adapter, unsupported/missing-runtime diagnostics, no dependency on NVIDIA initialization.
3. **Frame-contract adapter:** sizing, colour roundtrip, motion/depth, jitter and history-reset tests, including non-square and mismatched extents.
4. **Standalone temporal GPU scene:** Native/Quality/Performance reconstruct successfully; static edges converge, moving edges track, no whole-image wobble, GPU debug validation and retirement checks pass.
5. **Skyrim SR integration:** current INI/menu routing, Native selection, source sizing and colour handoff; trial with NR/FG off, clearly identified active XeSS frames.
6. **NR combinations:** Before/After, scaled After guides, live pass/style changes and colour checks on the local supported NVIDIA route.
7. **Existing FG combinations:** FSR and NVIDIA FG independently; source identity/reader holds, HUD composition, off/on, menus and suspend/resume qualification.
8. **Lifecycle and trial packaging:** save/reload, fast travel, resize/minimize/alt-tab, startup failures and cleanup; immutable MO2 test archive, tester instructions and real cross-vendor qualification status.

Milestone 1 does not imply 12.5% of engineering effort. These are checkpoints with unequal workload. Intel FG/XeLL receives a separate design and milestone count after SR; completing the eight SR checkpoints does not claim Intel FG support.

Success requires actual SDK execution and temporal image checks. Mock GPU names can test routing, not hardware support. The local RTX 4080 SUPER is the first GPU witness; Intel and AMD execution stay unqualified until real device tests. Existing DLSS/FSR defaults and active mod files are preserved while preparing isolated probes and trial packages.

## Implementation review boundaries

The implementation plan should give precise interfaces and tests for the loader/context, frame adapter, host resources, source evaluator routing, INI/menu, NR and FG adapters. Avoid restructuring unrelated HDR work. Final packaging must include the official runtime and applicable distribution notices, without retaining duplicate research binaries in the repository.

Main unresolved engineering risks are guide provenance (raw versus dilated motion), jitter-sign alignment, same-adapter shared depth/resource formats and downstream asynchronous readers. These are explicit probe/test gates, not undocumented assumptions to resolve during a tester's launch.
