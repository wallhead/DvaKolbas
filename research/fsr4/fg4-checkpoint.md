# Experimental FSR4 frame generation — 2026-10-07

The FG path now selects its algorithm independently from upscaling through
`[FrameGeneration] FsrProviderPolicy`. Default `Analytical` retains 3.1.6.
`Compatible` selects catalog ML 4.0.1 when present, otherwise 3.1.6; explicit
`MachineLearning` reports unavailable without creating an analytical context.
The discovered opaque ID passes unchanged through creation and actual-provider
verification. Only the pinned SDK's documented 4.0.1 revision is admitted.
Provider changes are restart-only; live sharpening and FG off/on preserve the
pending provider. The rendering order and retirement ownership are unchanged.

## Local evidence

- RTX 4080 SUPER, matching D3D11/D3D12 adapter LUID `00000000:00010318`.
- SDK 2.3.0: FG catalog ID `17726168133342859270`, algorithm `3.1.6`.
- Original AIO19: the same FG ID/name; swapchain 3.1.6 rather than the SDK's 3.1.7.
- AIO's upscaling catalog exposes 4.0.2b, which does not establish FG4 support.
- Real Auto probe creates/verifies FG3.1.6, queries memory and retires it.
- Real explicit ML probe rejects absent ML before context creation (exit 1).
- Synthetic ML fixture (ID 42, not a hardware ID) exercises exact actual-provider
  verification, context Configure → PrepareV2 → Generate, host routing and
  retirement. This verifies orchestration, not ML rendering correctness.
- Selected regression: 31 tests, 30 passed, one official AMD ML SR hardware skip,
  no failures. Real analytical FG generated pixels/UI/lifecycle are included.
- Independent reviewer found no actionable defects in the provider/settings
  implementation. Subsequent packaging/portable-source changes receive a final review.

## Remaining qualification

AMD documents ML FG4.0.1 for Windows 11, RX9000 or later and DirectX 12 Agility
SDK1.4.9+. This plugin currently uses its existing native D3D12 device creation;
no Agility factory/deployment change is claimed. Device-catalog availability and
successful real creation must be checked on the RX9070 before a Skyrim trial.
If its native D3D12 runtime fails ML creation, Agility deployment must be handled
before claiming support. No ML FG generated pixels have been tested locally.

The separate validation kit checks exact ML identity/create/presenter, then
actual changing generated pixels, opaque and alpha UI sentinels, menu/off/on
and repeated retirement. It rejects analytical fallback. Portable fixture
source hashes must match the hashes compiled into the executable. Visual cadence
and Skyrim NR/UI/lifecycle acceptance remain separate requirements.

The AMD trial uses Native AA, Auto SR, explicit ML FG, FG initially off and NR off.
It retains both official SR and NVIDIA INT8 files but is not a NVIDIA ML FG trial.
Stable v1.1 and the installed NVIDIA SR trial are not updated by these scripts.

Source: https://gpuopen.com/manuals/fsr_sdk/techniques/frame-interpolation-ml/
