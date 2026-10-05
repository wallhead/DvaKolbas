# Round 2 menu edit fixes

Review source: `COMMIT_REVIEW_2026-10-05_ROUND2.md`, supplied by the user.
Its recommendations were checked against the current code before implementation.

## Changes

- R2-1: numeric fields apply on Enter, focus loss or menu close. Typing an FPS
  limit of `144` never submits the intermediate `1` or `14`.
- R2-2: slider drags form one settings transaction, committed on release.
  Checkboxes and live selections remain immediate. Idle frames do not resubmit.
- R2-3: restart-only provider, render scale, presenter and HDR allocation choices
  stay in the menu until Save. Live edits use the actual renderer/presenter owner.
  Browsing a pending provider preserves active NR/FG and legacy NR controls.
  Save prepares compatible startup defaults while preserving the current presenter.
- R2-4: live application projects only changed fields onto the accepted runtime
  settings. An unchanged rejected target cannot block an NR toggle or multiplier.
  Accepted sanitized values merge back into the menu without losing pending choices.
- Moving directly between editors finishes the old interaction separately; it
  cannot bundle an invalid dynamic target with a new FPS limit.
- Pending FSR sharpness survives unrelated NR changes during a NVIDIA session.

Save as default remains the only INI writer. The three standard tabs, frame-time
graph, Native render scale and `DLSS/FSR -> NR -> FG -> UI` ordering are preserved.
This change does not replace source features or presenters while browsing options.

## Validation

- Release plugin and all configured test targets built successfully.
- Integrated selected suite: 260 passed, zero failures or skips; 257.17 seconds.
  Includes 93 GPU and 7 ReShade checks on the existing RTX 40 environment.
- Following the final requested-sharpness and legacy-availability corrections,
  the full build and seven affected checks passed: RendererSettingsActions,
  OverlaySettingsCommit, NeuralWorldContract, RendererSettingsController,
  OverlayCommunityNeuralRepair, SourceUpscalerDeferred, SourceUpscalerDeferredGpu.
- The new ImGui regression exercises real input events, Enter, slider release,
  checkbox release, focus loss, direct editor handoff, close and reverted edits.
- Production-controller regressions cover actual-owner projection, rejected
  inputs, pending presenter defaults, sanitized-value merging and FSR sharpness.
  The pending sharpness regression failed before the correction and passed after it.
- Final independent read-only review reported no remaining P1/P2 findings.
- Diff whitespace verification passed.

The same six environment/foreground checks remain excluded: NativeUIComposition,
NativeUIBlendState, NeuralPeripheralPixels, NrPostDlssVendorPresentationGpu,
NrPostDlssVendorLifecycleGpu and NrPostDlssVendorInterruptionGpu. No new Skyrim
menu observation or additional hardware qualification is claimed.

Local evidence: `out/research/round2-*.log` and `round2-*.xml`.
The trial installer records clean build identity, hashes and rollback location
in `out/packages/round2-menu-edits-<revision>/menu-update.json`. Installation updates
only the DLL and validation manifest, with Skyrim closed. INI, runtime payloads
and MO2 launch/profile files are preserved and verified by hashes.

## Other review items

R2-5 was corrected on the separate `codex/amd-bridge-docs` branch, commit
`1e0b7c8`: the ambiguous binary-inspection sentence was removed without inventing
an observed AMD execution result. AMD compatibility remains unqualified.

N1 wrapper failure recovery remains a separate GPU ownership task. The existing
proved rollback before vendor feature creation is retained; this menu change
does not weaken terminal handling after unsafe or unproved initialization.
N2 quarantine checks and N3 explicit FG preference persistence are retained.
The unproved N5 multi-pass drift claim is not treated as a confirmed bug.
