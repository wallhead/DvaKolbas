# Independent FSR frame generation — proposed design

Status: proposal for user review; no renderer changes made.

## Intended result

The user reports having used both DLSS/DLAA + FSR FG and FSR + FSR FG.
Restore those choices in RaZkolbaS while retaining the working NVIDIA FG route.
The order remains **DLSS/FSR → NR → FG → UI** for After NR. NR Before retains
its existing position before the selected upscaler. NR must not process the HUD.
FG on/off remains live. Selecting a different presentation backend requires
Save as default and restart; live swapchain replacement is outside this change.

## Findings from the current checkout

- `PublicIni::Decode` derives `FrameGeneration/Backend` from the upscaler.
- `OverlayFrameGenerationPanel.cpp` displays FSR FG only when `view.fsrActive`.
- `ResolveBackend` and the settings validators reject NVIDIA upscaling with
  the FSR presenter.
- `NvidiaHost::FsrFgActive` additionally requires FSR upscaling.
- `FsrHostPresentation` obtains render sizing, prepared guides and feature
  readiness from `FsrHostResources`, which currently creates an FSR SR owner.
- The direct DLSS evaluator also prepares Streamline generation; its NR,
  camera and retirement dependencies must be separated from that preparation.

These restrictions exist before the v1.2 metadata bump. This investigation has
not located a historical implementation of the user's mixed route in this
checkout; it does not identify which earlier binary or checkout they used.
Restoring a dropdown alone would expose a configuration that startup rejects.

## Approaches considered

1. **Reuse the existing FSR presenter with generation inputs independent of SR
   (recommended).** Keep its callback, native UI transport and retirement
   machinery. Supply final colour and current render-sized depth/motion from
   either upscaler. DLSS does not run through an extra FSR SR context.
2. **Build a separate hybrid presentation host.** This could isolate mixed-route
   code, but would duplicate swapchain, callback, resize and UI lifetime rules.
   Prefer the shared presenter to avoid maintaining two FSR implementations.

## Settings and menu

Add `[FrameGeneration] Backend = Auto` to the single organized named INI.
Allowed values are `Auto`, `NVIDIA`, `FSR`, on a separate comment line.
`Auto` preserves existing defaults: NVIDIA for DLSS/DLAA, FSR for FSR upscaling.
Store this preference separately from the resolved runtime backend, so saving
does not replace Auto with the backend selected on the current machine.

Expose the same choice in the Frame generation tab. FSR-specific provider
choices (FSR3/Auto/FSR4) appear when FSR FG is selected, independently of SR.
Keep controls for the current live owner distinct from a pending restart:
editing a next-launch backend must not disable or misconfigure current FG.
Show both active and pending selections, and state when restart is required.

Ordinary/backend 0 stays available only through the existing diagnostic option.
AMD supports FSR SR + FSR FG; NVIDIA FG and DLSS/DLAA remain unavailable there.
This task restores DLSS/DLAA + FSR FG and retains FSR + FSR FG and DLSS/DLAA +
NVIDIA FG. FSR SR + NVIDIA FG is a separate route and remains unavailable with
an explicit explanation rather than a selectable broken configuration.

## Rendering and ownership

- Determine the upscaler and presentation owner independently at startup.
- DLSS/DLAA sizing and evaluation continue through the existing D3D11 NGX
  backend. Never initialize a second SR feature for the mixed route.
- Prepare FG-only shared depth/motion resources on the actual adapter's D3D12
  device, using existing interop and format conversion rules. Snapshot each
  source once, with actual render/display extents, jitter, motion convention,
  camera planes/FOV, timing, reset and identity.
- Pass the post-upscale/post-NR world image to the existing FSR presentation
  transport. Its UI registration composes native HUD after interpolation.
- The FG-only path loads the qualified official FG runtime. Inactive FSR SR
  policy/INT8 settings must not create an SR feature or select its loader.
- Configure NR independently of Streamline swapchain creation; the mixed
  route retains NR passes, styles, placement, device sharing and reader holds.
- Use the actual presenter's retirement proof when replacing DLSS or NR
  features. Never invoke a Streamline retirement/resume path without that owner.
- Suppress generation for menus, loading, missing/stale guides, failed SR,
  suspension and transitions. Resume with a temporal reset after valid sources.
- Keep one presentation owner. No stacked NVIDIA and FSR interpolation.
- Resize, alt-tab/minimize and teardown retain source, guide, scene, native UI
  and SDK callback readers until their respective fences prove retirement.
- Retain the existing SDR-only contract for the FSR presenter. Enabling FSR FG
  with HDR or unsupported dynamic sizing must give an actionable message.

## Verification before installation

1. Failing configuration/menu regressions: independent choice, Auto round-trip,
   AMD restrictions, backend 0 hidden, inactive SR options, restart staging and
   live off/on addressed to the current owner.
2. Real D3D11/D3D12 interop tests for external-upscaler colour and guide inputs;
   no FSR SR dispatch, current-frame identity, same-device validation and reader
   retirement. Faults must not generate frames from stale or invalid inputs.
3. Standalone actual DLAA and scaled DLSS → NR → FSR FG tests on this PC,
   checking real generation callbacks, HUD integrity, colors, toggles, resize,
   suspension and retirement. A synthetic FSR input is not proof of the mixed
   route. No performance comparison is required.
4. Build regression checks and review. Package a matched DLL/INI, back up the
   installed v1.2, preserve the user's settings, and install only with Skyrim
   and MO2 closed. User then performs the Skyrim check.

## Scope limits

No new AMD NR, MFG algorithm, global driver setting changes, DLL patches,
fullscreen support or automatic old-INI migration. Existing FSR3/FSR4 SR/FG
hardware restrictions remain enforced. Changing the backend remains a restart
operation; enabling/disabling interpolation remains live.
