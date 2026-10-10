# Standalone Intel FG qualification — 2026-10-10

Qualified locally on the actual RTX 4080 SUPER (`10DE:2702`, render/native
LUID `00000000:00011072`), official SDK bundle 3.0.2, FG API 1.3.1 and
XeLL API 1.3.2. This is not Skyrim integration or cross-vendor qualification.

The [visible receipt](standalone-visible-2026-10-10.json) and
[log](standalone-visible-2026-10-10.log) record 720 source frames:
357 accepted interpolation sources each reported two presented frames;
363 off/menu/reset sources reported one. SDK frame errors: zero. Unexpected
focus losses: zero. FG destruction followed by XeLL destruction succeeded.
The user separately reported **“Smoother; HUD intact.”**

Native backbuffer readbacks checked unchanged scene, opaque red HUD and
premultiplied translucent green HUD pixels while FG was both off and on.
The first HUD regression exposed missing real-frame composition: Intel's
generated-frame composition does not substitute for the application's
composed real backbuffer. Owned scene/UI tags stay separate; the application's
real image now gets `UI + (1-alpha) * scene` before publication.
Readbacks do not expose Intel's private generated buffers; generated-image
HUD stability is user-observed evidence.

The bounded numerical lifecycle run also suppressed interpolation during
foreground loss, delivered real images, reset history on return, and drained
before same-size minimize/restore. Contract/GPU regressions reject missing HUD,
duplicate sources and extra Presents, retain owners on held-queue timeout or
native submission failure, and destroy FG before XeLL. No AMD retirement
acknowledgements are used. Positive TOO_FEW_FRAMES after reset is expected;
it is not counted as successful interpolation.

Graphics Tools/debug layers are unavailable on this machine. Their checks
remain unqualified. AMD, Intel, other NVIDIA GPUs, HDR and resized presentation
remain unqualified. No Skyrim/MO2 files were changed.

Reproduce the visual scene with `tools/xess/Start-Xess-FG-Visible.cmd`.
The launcher verifies official DLL hashes and writes immutable timestamped
logs/receipts. Validate this saved result without another GPU run:

```powershell
C:/Python314/python.exe tools/xess/Validate-FG-Receipt.py --receipt research/xess-fg/standalone-visible-2026-10-10.json
```
