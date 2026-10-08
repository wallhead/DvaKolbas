# HDR investigation — ENB first

Scope: investigate upstream TRP, AIO19 and the current RaZkolbaS branch. The user selected the current ENB setup first. No rendering code, installed mod, INI or MO2 settings were changed. This is a source/static-RE assessment, not an HDR gameplay qualification.

## Conclusion

Use upstream's existing SDR-to-HDR10 output as the foundation. Much of that implementation is already present in RaZkolbaS. Start with DLSS Native/DLAA, NVIDIA presentation, FG off and NR off; then validate SDR NR before HDR conversion and matching HDR scene/UI inputs to FG. Extend the FSR presenter separately. Do not enable HDR by removing its SDR guards.

ENB continues producing its finished SDR image. Inverse tone mapping expands its highlights and applies display paper white/peak brightness; it cannot recover highlights that ENB already clipped. Community Shaders' HDR Display is a separate producer path and is outside the first ENB milestone.

## Upstream evidence

Pinned upstream main: `b169e29006b94aaab43aeeb22bfbed729a4b9bea`, dated 2026-10-03, merge of PR #86 (`feature/hdr-live-toggle`). Read-only shallow reference in `out/research/hdr-2026-10-08/upstream`. Normal fetch encountered an existing invalid local turn-diff ref; no refs were deleted or repaired.

[Pinned README HDR section](https://github.com/theosw/theosrenderpipeline/blob/b169e29006b94aaab43aeeb22bfbed729a4b9bea/README.md#hdr-experimental) describes both CS HDR Display and ENB/non-CS inverse tone mapping. For ENB it documents Windows HDR, borderless presentation, independent UI brightness, live calibration, and SDR NR/ReShade before HDR output. Its reported ENB evidence is one Universal RTX 4080 SUPER setup; it also reports unresolved vendor flip-queue errors. This does not establish support across all cards or presenters.

Source comparison in `source-comparison-2026-10-08.json` confirms identical current/upstream colorimetry, HDR settings/math, CS HDR conversion implementation and SDR-to-HDR shader implementation. The primary missing HDR behavior is live swapchain activation:

- Upstream `SourceDLSSGSwapChain::StartHDROutput` runs after a successful Present. It quiesces readers, reallocates native buffers as RGB10A2, preserves game-facing buffer identities, rebuilds native handles and resumes. A failed HDR resize attempts to restore the previous format.
- Backend request/display state distinguishes requested HDR, allocated native HDR buffers and the current Windows display state. HDR off keeps the 10-bit native buffers until restart and signals SDR.
- RaZkolbaS currently allocates HDR output at startup. The End menu and INI document a restart for HDR on/off; calibration is live. Porting needs corresponding settings-action/UI changes, not just copying the swapchain method.
- Current output conversion executes independently of FG enablement. HDR does not inherently require FG to be on.

## AIO19 static RE

Archive: `SkyrimUpscalerAIOBuild19-Hotfix1.7z`, 204,042,400 bytes, SHA-256 `49e7f7dabf426937915d1aeed664fc40a7cc7d89f42092a69c205b22c4687439`.

`tools/hdr/Inspect-Aio19Hdr.py` verifies the archive and three extracted artifact identities, seven literal strings and eleven instructions. It disassembles from PE unwind-fragment boundaries and verifies RIP-relative string targets. Full hashes, bytes and bounded instruction windows are in `aio19-hdr-witnesses-2026-10-08.json`. No AIO DLL was loaded or executed.

| Evidence | What it establishes | Limit |
| --- | --- | --- |
| Shipped INI `mDLSSNRColorIsHdr=false`; UI description at SkyrimUpscaler RVA `0x45aa58` says it treats NR input as HDR for Ratio resolve | This is an NR input/reconstruction setting | Does not enable Windows HDR or prove HDR monitor output |
| INI read references key at `0x2f3596`, stores returned boolean at `0x2f35ae` into settings `+0x2e4` | The setting is actually parsed | Full subsequent NR parameter dataflow is not established here |
| Named `HDR::UiTexture` / `HDR::HdrTexture` lookups and conditional stores at `0x2ecd18..0x2ecd48` | Distinct CS HDR UI and scene resource capture | Live texture formats/encodings and visual correctness still require capture |
| Labelled `SetColorSpace1` proxy tail at `0x282f10` forwards to retained receiver vtable `+0x130` | Color-space requests can be forwarded; DXGI method identification follows the labelled function/interface layout | Does not prove an ENB HDR converter creates those requests |
| PDPerfPlugin fake-HDR-buffer labels and descriptor value 10 at `0xfe49d` / `0x101e0a` | Conditional FP16 surrogate-buffer preparation exists at creation and resize | FP16 storage alone does not prove wide-range HDR content or HDR10 output |

No ENB SDR-to-HDR10 converter, live HDR calibration or output toggle has been established in AIO19. Its exposed NR input-HDR setting must not be substituted for display output controls. `mDLSSNRWhitePoint` is documented as the Ratio encode white point, not display paper-white nits. Static evidence is sufficient to identify useful CS resource separation, but not to copy an asserted ENB HDR pipeline.

## RaZkolbaS gaps

1. **Community NR is blocked when HDR output is enabled.** `NeuralRendering/BeforeSettings.h::NativeBeforeUnavailable` rejects `p.hdrOutput.enabled`; the check is used for both NR placements. The compatibility repair button explicitly disables HDR output. This conflates SDR NR inputs with later HDR display conversion. Keep true HDR NR input unsupported, while validating the ENB SDR NR → HDR output route before relaxing that separate display restriction.
2. **FSR presentation is SDR-only.** `FSRPresentation::TranslateDescriptor` accepts only RGBA8. Transport publishes sRGB scene/UI; `BuildFsrGenerationPrepare` enforces RGBA8 and sRGB; the FG callback enforces sRGB transfer. `FSRSwapChainPolicy` rejects PQ color space and HDR metadata. Startup/menu policy rejects HDR even with FG off when using this presenter; AMD startup also clears HDR output.
3. **Output ownership must remain explicit.** Keep upscaler/NR/ReShade processing in their existing SDR domains for ENB. Convert the final HUDless scene and UI consistently before FG consumes HDR resources. Preserve the design's upscaler → NR → FG → UI ordering; no NR over generated frames or display-PQ bytes.
4. **FSR SDK capability is broader than our integration.** The checked local SDK exposes sRGB, PQ and scRGB transfer-function enums. [AMD's integration documentation](https://gpuopen.com/manuals/fidelityfx_sdk/techniques/frame-interpolation/) requires the backbuffer transfer function and HDR min/max luminance. The FSR3 and FSR4 FG providers must each be qualified; SDK enums alone do not establish driver/provider correctness.

## Proposed implementation milestones

1. Establish an ENB HDR baseline on NVIDIA presentation: Windows HDR/display identity, RGB10A2 + PQ/BT.2020 signaling, SDR scene input, independent HUD/menu brightness; DLAA with NR/FG off. Preserve Windows-HDR-off SDR fallback and portable defaults with HDR off.
2. Port upstream live on/off and calibration controls into our settings controller/End menu. Retain ownership, reader retirement, resize recovery and warm-up handling; test failed allocation, repeated toggles, display-state changes, menus/loading and alt-tab.
3. Qualify community NR Before/After feeding SDR into the HDR conversion; remove only the display-output incompatibility once tests demonstrate the NR model still receives the expected SDR format/encoding. Test all supported passes and style switches without color-domain changes.
4. Qualify NVIDIA FG with consistent HDR HUDless/UI/composite resources, including FG off/on, resize, loading and UI lifecycle.
5. Extend FSR presentation and transport to HDR10, including separate game/native formats, HDR scene/UI resources, PQ transfer and luminance dispatch values, metadata/display fallback and callback format validation. Cover DLSS → FSR FG, FSR3/FSR4 SR and both FG providers, including AMD. Relax policy guards only for validated combinations.
6. Finish visual/display tests and portable packaging. Add exact output diagnostics (requested/active/fallback, producer/native format, encoding, color space, luminance and provider) with bounded transition logging. HDR remains off by default until explicitly selected and qualified.

Milestones above are a proposed implementation sequence; none is marked complete by this investigation. No Skyrim start is required until the baseline test package is prepared.

## Verification performed

- Fresh hash-gated AIO19 probe: PASS, archive + three artifacts + seven strings + eleven instruction witnesses.
- Existing Release test executables: `CommunityShaderHDR`, `HDROutput`, `HDROutputPass`, **3/3 passed**. Includes WARP resource/composition checks and CPU colorimetry/settings tests. No fresh plugin build was needed for read-only research; these are not physical-monitor or live-game checks.
- Source comparison pinned to the local revision recorded in JSON and upstream commit above.

Reproduce the AIO probe from repository root:

```powershell
& C:/Python314/python.exe tools/hdr/Inspect-Aio19Hdr.py `
  --archive 'C:/Users/user/Downloads/SkyrimUpscalerAIOBuild19-Hotfix1.7z' `
  --extracted out/research/aio19/extracted `
  --output research/hdr/aio19-hdr-witnesses-2026-10-08.json
```

Run existing HDR checks:

```powershell
ctest --test-dir out/build/fsr-fg-universal -C Release `
  -R '^(HDROutput|HDROutputPass|CommunityShaderHDR)$' --output-on-failure
```
