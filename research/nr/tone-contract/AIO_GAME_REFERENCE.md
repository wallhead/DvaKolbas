# AIO19 stable gameplay reference — 2026-10-04

The user reports **no color shifts in AIO19 with its actual installed INI**. Treat that as a stable gameplay reference. Prior standalone fixtures show context sensitivity in our host; they do not override this observation or prove AIO's game host drifts. The Dva enabled-tone camera defect remains open. Stable colors is rejected as the solution and remains off; Tone remains 1.

**Latest result:** AIO19's active chain at **Style 0 / Tone 1** also has no shifting, and the user confirms that NR off/on visibly changes the image while camera rotation remains stable. This is now the relevant positive visual control for Dva's reported Style-0 shift. The prepared startup INI, post-session INI, log prefix, profile selectors and three installed binary identities are retained in [the Style-0 game result](aio-style0-game-result.json). The post-session INI differs from startup only in `mEnableDLSSNR=true -> false`, consistent with the toggle comparison. It was not silently re-enabled. Neither its feature-creation log nor this qualitative observation measures every-frame effective controls or pixel values.

## Subsequent matched game check

The user started the matched Dva configuration and reports **“seems only style 0 shifted.”** The captured log confirms FSR 3.1.5 NativeAA/Analytical, FG off, ReShade before upscaling, Tone/Structure 1 and Stable colors off throughout the logged revisions. It records live styles 0/1/2/3/4/5/7, including repeated returns to 0, and 11,400 cumulative evaluated sources by 11:40:40 Moscow time, with no error/critical records. Style 6 is not present in this captured prefix. Sparse progress records during revision 12 (Style 0) show frames 6,000 -> 7,800 with resets fixed at 11, so that interval does not support a repeated-reset explanation. These are processing/control observations; color stability comes from the user's tentative visual report, not pixel measurement. No complete per-style visual qualification is claimed.

At that earlier checkpoint, **AIO Style 0 had not been tested yet**; its active saved chain was Style 1. The subsequent Style-0 comparison and positive NR off/on control above answer that missing test. Do not describe Style-0 camera shifts as unavoidable or intentional vendor behavior. AIO's stable Style-0 result makes the actual integration/runtime-boundary difference the priority. [Captured Dva log evidence](style-game-20261004.json) preserves the earlier prefix; installed settings/assets and MO2 were untouched during that check.

## Current controls and installed comparison

Fresh snapshots of both installed INIs found these remaining differences:

| Choice | AIO19 reference | Dva before this change | Dva comparison now installed |
| --- | --- | --- | --- |
| Upscaler | FSR 3.1 DX11 Native | DLAA | FSR Native AA, Analytical provider |
| Active chain style | `[NR PASS 1] Style=1` | `NRStyle=0` | `NRStyle=1` |
| Sharpening | Off | On, 0.672 | Off |
| ReShade ordering setting | Before upscaling | After upscaling | Before upscaling |
| FG | Off | Off | Off |
| Tone / intensity / structure / skin | All 1 | All 1 | All 1 |
| Placement / preset / pass count | Before / 0 / 1 | Before / 0 / 1 | Before / 0 / 1 |
| Auto exposure | Off | Off | Off |

AIO's legacy global style 0 does not replace the chain's Style 1. Its native input-scale sentinel 0 canonicalizes to 1. Dva already uses scale 1, Auto resolve, transfer/color strengths 1, max ratio 2, white point 1, SDR and auto skin/UI correction off. AIO's groups are all disabled. Dva's appearance presets, peripheral compression, fused preparation and Stable colors remain disabled.

After user-confirmed closure and process verification, only five Dva INI keys changed: `Settings/UpscaleType`, `Settings/Sharpening`, `Compatibility/ReShadeBeforeUpscaling`, `Experimental/FrameGenerationBackend`, and `SourceDLSSG/NRStyle`. The AMD presenter selector 2 is required for Dva's existing FSR route even with FG disabled. All other 98 parsed settings were preserved. This is **a comparison configuration, not a drift fix**. FSR integration/provider, source transfer and exact runtime identities are not asserted identical between hosts.

The installed clean Standard DLL remains source `92955b794d5f`, SHA-256 `f829ad80fe92d195c7c707368909f077aae6fbec87b94ac0687be79eb8904058`, with FSR/FG/NR compiled. All 22 manifest non-INI assets were hash checked. AIO's INI, MO2 INI, profile modlist, both existing logs and the trial manifest were unchanged by installation. The prior complete Dva INI is retained in the local snapshot for rollback. The first atomic replacement call rejected an empty backup-path argument before modifying the INI; the corrected null argument succeeded, and its verified leftover temporary file was removed.

## Further bounded host tracing

The hash-pinned host has two distinct observed static NR call sites. `Host+281580` contains the **Community Shaders** preparation route, identified by its embedded `CSUpscaleInput.cpp`/`PrepareCSUpscaleInput` strings; do not treat that as the active ENB path just because it also calls the chain export. `Host+2FA520..2FAC2F` is the general chain packet builder. It resolves `EvaluateDLSSNRChain` by name and calls the retained pointer at `+2FABB7` with the packet at `RBP+D0`. The selected Before caller at `+2E967B` passes the color holder at host `+15A8` when its Before flag is set. Packet builder entries retain incoming color/output identity and the two guide pointers from host `+1600/+1658`; actual guide pixels remain unmeasured.

The log's “Native frame read-only capture implementations ready” string belongs to **Map/Unmap hook installation**, confirmed by the failure string and `InstallNativeFrameCapture(ID3D11Device*, ID3D11DeviceContext*)`. It is not evidence of a public NR image-dump API or an already available Color/Output capture. No debugger was attached to Skyrim.

PD native preparation previously recovered at `+A1E00` can return without transfer processing. Additional resolve tracing at `+A2740` tests prepared state byte `+3D`: if false, it takes the compatible CopyResource path at `+A2800` or alias no-op, rather than the later resolve shader. AIO's format-preserving route remains a concrete difference from Dva's Gamma22-decode/linear-FP16/encode path, but its actual active view, input values and branch still need measurement.

Additional chain tracing initializes the pass mask pointer to zero (`+A3850/+A3853`), tests the per-pass `+20` field at backend `+33DC` (`+A3D84`), and bypasses mask allocation/strength overrides when zero. In that conditional path, null is forwarded as the builder's mask argument (`+A4069/+A40A8`), while scalar Tone is loaded from `+33CC` (`+A3802/+A380B`). This strengthens the scalar-only hypothesis for disabled groups; it is not a live-dispatch observation. There is no basis to synthesize an all-default mask as a fix.

The retained [host probe](host-probe/TraceAioHost.py) checks the exact host hash and decodes contiguous, fully covered PE unwind fragments. The selected run has 719 instructions in four ranges. Cross references cover unwind-described code only, not every code byte. Proprietary binaries stay local. The compact [checkpoint receipt](aio-game-reference.json) records exact identities and installation scope; older experiment receipts remain immutable.

## Next gate

The qualitative same-scene comparison is now complete for the reported defect: apparently only Dva Style 0 shifts, while AIO Style 0 stays stable and visibly processes NR. Do not ask for the same generic stability/on-off comparison again.

Proceed to bounded actual Color/Output, guide, view/format, effective-control/mask and reset captures. Include active viewport dimensions: AIO logs forcing DynamicResolution on, even though its created NR feature and FSR recommendation are native 2560x1440. Do not infer a complete source-viewport match from creation dimensions. Compare the NR-induced correction on persistent surfaces, not unrelated full-screen mean colors. AIO's stable game result makes integration parity the priority before any new temporal tone architecture. The original and RTX40 model captures match only within the retained fixtures; actual moving-game input parity is unproven. Raw encoded input, output initialization, new mask semantics or reduced tone remain unproven fixes. Completed NR milestone count remains **1 of 8**.

The first bounded [runtime-control observer](runtime-observer/README.md) is built
and qualified on owned CPU/GPU fixtures. It reads the exact vendor network-entry
frame using a hardware breakpoint, including effective Style/Tone, masks,
subrects, resets and the indexed coefficient block. Both model fixtures retain
identical baseline/observed CSVs and RGB captures. **No Skyrim observation has
been captured yet**, and this first tool does not measure resource/view formats,
pixel values or active viewport dimensions. Those remain subsequent evidence
requirements. No installed settings or product code changed.
