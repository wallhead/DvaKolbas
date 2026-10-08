# ENB HDR baseline preparation

User approved starting the ENB baseline from the investigation plan. This first step uses the existing renderer-owned SDR-to-HDR10 code, rather than adding a new HDR implementation. It does not port live HDR activation or enable HDR on the FSR/community NR paths.

## Prepared trial

Archive: `C:/Users/user/Downloads/RaZKolbaS DLSS FSR FG NR - ENB HDR baseline.zip`.
SHA-256: `b04313d4b874e5131299840b39111b9e40dd38e37dbe35476f5dca8251927afc`.

- Plugin: existing clean Universal v1.3.0.0, source `70edb4c1fabc`, identical to the release archive.
- Configuration starts from the installed V5.4 NO-LORE v1.2 mod's INI, preserving unrelated preferences. Trial overrides select DLSS Native (DLAA), NVIDIA presentation, FG off, NR off, HDR output on, dedicated native UI and fixed resolution.
- SDR decoding: Gamma22, consistent with the existing ENB comparison choice. Verify image appearance in the game; this is not a new measured producer-encoding claim.
- Windows SDR brightness matching on; peak 400 nits is a conservative **provisional** trial value. It is not display calibration or certification. Recheck display reports with Windows HDR on and adjust peak to the monitor's actual calibrated HDR capability.
- Portable NR discovery overrides blank. The archive replaces only INI and metadata; all plugin/runtime payloads are unchanged. Release defaults and the v1.3 release archive remain untouched.

The packer verified archive CRC and all 19 entry hashes. Independent Python ZIP/INI verification confirmed actual defaults, CRC, receipt SHA and portable paths. Tracked receipt: `baseline/package-verification.json`. A fresh review identified two small hardening changes: force/verify the bundled relative Streamline directory, and escape control characters in display-probe JSON. Both were applied before final delivery.

## Display inventory

Read-only `tools/hdr/HDRDisplayProbe.cpp` is compiled separately from the plugin. It uses production `QueryDisplayHDR`/`QuerySDRWhiteNits` and DisplayConfig capability queries. It creates hidden positioning windows for each attached output, without changing Windows settings or creating a swapchain.

Observed on RTX 4080 SUPER:

| Display | Name | Windows HDR | Current desktop | DXGI maximum luminance report |
| --- | --- | --- | --- | --- |
| DISPLAY1 | Mi Monitor | off, known | 8-bit SDR | 417.712 nits |
| DISPLAY2 | S24R35x | off, known | 8-bit SDR | 270 nits |

The newer capability query returns Windows error 87 on this OS. The legacy query reports advanced-color support, which may include WCG. Consequently, **HDR support remains unknown**, not proven by these reports. The Windows HDR switch on the intended game display is the practical prerequisite for a live HDR baseline. Display values reported while HDR is off are not sufficient for final peak calibration.
The probe represents unknown HDR support as JSON null. Tracked read-only inventory: `baseline/display-inventory.json`.

## Checks already run

- Standalone display probe configured and built successfully with MSVC/Windows SDK 10.0.26100.0; live read-only enumeration completed and its output parsed as JSON.
- Existing Release checks: PublicIni, IniAudit, CommunityShaderHDR, HDROutput and HDROutputPass: **5/5 passed**. Shader checks use WARP; they do not establish physical HDR display appearance.
- No production renderer code was changed, and the test archive retains the already-built release DLL. No new plugin build or hardware FG qualification is claimed.

## Installation and game acceptance

Confirmed current portable MO2 setup: `D:/TESV54BETA/BETA_TRUEAE_V54`, selected profile `V5.4 NO-LORE`; modlist enables `RaZKolbaS DLSS FSR FG NR v1.2`. MO2 was running when preparation began. After the user replied closed, both MO2 and Skyrim were confirmed stopped. The baseline DLL/INI were installed in that existing active mod folder. Its folder name and metadata remain unchanged for this temporary trial; the installed binary is v1.3.0.0.

The active DLL, INI and metadata were backed up to `out/backups/enb-hdr-baseline-2026-10-08`. Before replacing files, the current INI hash still matched the staged source and all installed runtime assets matched the verified package. Installed DLL/INI hashes were verified against the archive receipt; all three rollback backup hashes were checked. MO2 INI and modlist hashes were unchanged; metadata and runtime assets were preserved. Tracked installation receipt: `baseline/installation-receipt.json`.

User checks once Windows HDR is enabled and the baseline is installed:

1. Start Skyrim through the normal MO2 SKSE entry; load the same ENB save. Keep FG and NR off.
2. Inspect a bright exterior and dark interior, camera movement, HUD and End menu. Look for excessive UI brightness, raised blacks, washed-out colors, clipping or banding.
3. Open inventory/map/dialogue, save/reload and alt-tab/minimize/restore. HDR should resume on the same display without a black screen.
4. Compare with Windows HDR off (SDR fallback), then on again on the game display. Current HDR allocation preference needs restart; calibration changes are live. Do not claim our live HDR on/off menu port is already implemented.
5. Inspect a fresh log: `[HDROutput] requested=true native=true`, display `known=true hdr=true` and `PQ BT.2020`, successful composed/encoded frames. Confirm the source is Native/DLAA and that NR/FG remain disabled. If Windows HDR stays off, this tests fallback only.

Milestone status: **0 of 6 complete; milestone 1 prepared for live acceptance.** Display/visual/installed-route checks remain open; later milestones are not implied by the preparation tests.
