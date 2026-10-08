# Automatic NR selection and startup-settings audit

An RTX 5060 tester's v1.3 log and menu showed the Legacy NR path missing.
The previous public schema defaulted to Legacy, so selecting a GPU profile
never reached the bundled model catalog. User approved automatic model
selection and requested the same audit for other PC-dependent settings.

Normal startup now uses the bundled catalog without a Legacy/Community INI
selector. The actual rendering device's PCI vendor/device IDs and LUID remain
authoritative: RTX 20/30 use the compatibility model, RTX 40/50 share the
RTX 40 model, and AMD NR is unavailable. Missing or unverified model files,
unknown GPU IDs and mismatched family overrides retain their existing errors;
there is no automatic Legacy fallback or relaxed integrity check.

The old `[NeuralRendering Advanced] Runtime` key is ignored and removed on
save. `[Debug] NRLegacyRuntime=true` is an explicit diagnostic escape hatch
for the separately installed Legacy implementation; its optional runtime path
defaults blank. `[NeuralRendering Advanced] Profile=Auto` is the normal GPU
choice; other profile/root/core selections are documented diagnostic overrides.
Startup logs record automatic/diagnostic/AMD policy and discovery origins.

The audit also found and fixed:

- Schema defaults differed from the shipped INI: FSR/NR encodings were Unknown,
  the SDR byte path was off and FG was on. Schema, generated examples and release
  packing now agree on ENB Gamma22, SDR bytes, FG off and NR off.
- AMD normalization inserted the internal `Settings/EnableUpscaler` flag.
  Saving it could make the next launch reject its own INI as obsolete. Encoding
  now removes it and the effective CommunityRuntime flag, preserving a configured
  diagnostic override separately from effective GPU policy.
- The Before-trial stager rejected every normal automatic reference as already
  using Community NR. It now stages from that valid reference and explicitly
  disables the Legacy diagnostic switch.
- Review found the offline converter still required the retired Legacy selector
  to survive unchanged. A regression reproduced its rejection; conversion now
  drops retired effective flags while preserving actual user settings.

Other startup choices checked:

| Setting | Normal behavior |
| --- | --- |
| Rendering GPU | Actual D3D rendering adapter, not a guessed primary card |
| NR profile | Auto; wrong-family/missing-model errors retained |
| NR catalog / driver core | Blank overrides use packaged root / active rendering driver |
| Streamline | Portable path relative to virtual SKSE/Plugins |
| FG Backend | Auto derives its presenter; AMD normalizes to available FSR presentation |
| FSR SR / FG provider | Conservative FSR3 default; explicit FSR4 keeps strict availability checks |
| Dynamic MFG target | Zero uses the actual monitor refresh; no fixed PC-specific refresh |
| HDR | Off by default; capability/Windows HDR checks and calibration remain explicit |
| Source color encoding | ENB Gamma22 preset; other producers must select their actual encoding |
| Dynamic resolution / appearance / traces | Off by default |

Verification before clean plugin build: the new Legacy-selector regression failed
against the old implementation. After the changes, 11 focused tests passed:
BackendSelection, StartupPreferences, PublicIni, IniAudit, IniLayout, IniPackage,
PublicIniPackage, PublicIniSchema, NrPortablePackage, NrRuntimeCatalog and
NrStartupSettings. Tests cover old Legacy/Community values, automatic startup,
diagnostic opt-in, AMD save/reload, the exact RTX 5060 PCI ID and missing-model
rejection. This is CPU/configuration validation, not new RTX 5060 image quality
or GPU execution qualification.

The installed ENB HDR trial and MO2 settings are not updated by packaging.
Final build and archive identity are recorded in the separate delivery receipt.

## Delivered package

Clean Universal plugin build succeeded: v1.3.0.0, source `bade88e7787d`,
FSR/FG/NR compiled. The final 11 focused checks passed after review fixes.
The updated trial stager parsed successfully; its full historical GPU/payload
staging workflow was not rerun. Independent archive verification checked CRC,
19-entry inventory, 23 default settings and every payload hash against the
original qualified v1.3 archive. Only plugin DLL, INI and metadata differ;
all vendor runtime payloads are unchanged. The production INI decoder test
also accepted the actual ZIP-readback INI and the previously generated tester
INI containing the retired selector.

Delivery: `C:/Users/user/Downloads/RaZKolbaS DLSS FSR FG NR v1.3.zip`.
Size: **367,377,445 bytes**.
SHA256: `5178e492eb7526c7fb1a6411aa9c5fa3acb4f9d4c926335dddbc2e8f0087826d`.
[Full package receipt](release-1.3/automatic-nr-verification.json).

The new DLL is required for this automatic-selection behavior. Updating only
the INI while retaining the previous DLL leaves the old implementation chooser
in place. Actual RTX 5060 gameplay with the new package remains a tester check.
