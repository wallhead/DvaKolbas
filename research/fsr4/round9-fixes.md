# Round 9 FSR review fixes — 2026-10-07

Baseline: `4d88ec8b053c270a15f173333b0b3f548864ba2a`, branch `codex/nr`.
Review input: `COMMIT_REVIEW_2026-10-07_ROUND9.md`. Findings were checked
against source, existing receipts and the official SDK manuals.

## Changes

- Disable explicit ML choices known to be unavailable, with a visible reason.
  SR uses the actual adapter's catalog and SM6.6 result; NVIDIA alternate SR
  files are checked against the same size/SHA pins without loading a second
  runtime. The menu caches file-only preflight before FSR is running.
- Unknown support remains labelled unknown, rather than being equated with
  known absence or successful context creation. Startup repeats validation.
- Keep SR and FG availability independent. FG uses its own qualified catalog;
  SR4 does not establish FG4 availability. Analytical and Auto remain selectable.
- After a successfully retired, same-size Auto recovery from observed ML
  creation failure, disable explicit ML SR and retain the actual failure reason.
- Explicit requests remain strict; no new implicit runtime/context fallback.
  Startup errors preserve their cause and name the INI section, key and
  Analytical recovery value even when the game menu cannot open.
- Report actual shader-model results or the failed query HRESULT. Keep SM6.2
  and provider resource-requirements checks, which match the pinned SDK contract.
- Keep qualified FG versions pinned. Do not admit future 4.x revisions without
  updating the runtime and qualifying their ABI, call order and rendering.
- Document measured bounded INT8 GPU-memory retention and AMD ML FG limits.
  A static legal classification is not an established finding from this review.

## Verification

New regressions failed before the fixes: missing SR/FG recovery keys, missing
owned availability snapshot, selectable unavailable ML in actual ImGui input,
and absence of observed ML creation failure in fallback availability state.
They pass after the changes.

- Universal plugin Release build: passed.
- Selected regression: 32 tests, 31 passed, 1 skipped, 0 failed. Covers runtime
  discovery/pins, strict selection, real ImGui interaction, fallback ordering,
  SR/FG contexts, host ownership, actual analytical generated frames, sharpness,
  automatic provider selection, package validation and resize ownership.
- The skipped official ML SR test requires AMD hardware; it establishes no
  AMD ML pixel qualification on this NVIDIA machine.
- Fresh independent FSR-on / FG-off configuration: Startup target builds and
  its GPU startup test passes. FG assertions are conditionally compiled.
- Final plugin-only diagnostic changes: rebuilt successfully and repeated
  NVIDIA-free FSR startup plus package tests, both passed.
- Independent read-only review identified two issues (availability after
  successful Auto recovery and the SR-only test guard). Both were corrected;
  follow-up found no additional actionable issue in those changes.

GPU checks ran with Skyrim closed. Graphics debug layers were unavailable.
No installed mod, MO2 settings, stable release or AMD validation archive was
changed by these fixes. AMD ML FG remains experimental; RX9070 catalog, actual
creation/Agility requirements, generated pixels and Skyrim acceptance remain
outstanding.

## References

- [FSR 3.1.5 shader requirements](https://gpuopen.com/manuals/fsr_sdk/techniques/super-resolution-upscaler/)
- [Official FSR4 SR hardware requirements](https://gpuopen.com/manuals/fsr_sdk/techniques/super-resolution-ml/)
- [ML FG requirements](https://gpuopen.com/manuals/fsr_sdk/techniques/frame-interpolation-ml/)

The review's blanket exclusion of AMD cards older than RX9000 applies too
broadly to SR: the current SR manual includes RX7000. The device/runtime catalog
remains authoritative for this binary. ML FG has separate RX9000+ requirements.
