# Opt-in SDR-byte NR comparison — 2026-10-04

The user confirms no camera drift in RazKolbas at Style 0 / Tone 1. Raz copies
SDR RGBA8 through the same exact RTX40 NR model that Dva uses. This trial
isolates that color-contract difference while retaining full tone and styles.
It is not a confirmed Skyrim drift fix. Earlier synthetic RGBA8 context tests
still drifted; these historical receipts are retained unchanged.

## Contract and controls

`[NeuralRendering] SdrBytesTrial=true` is a restart-scoped opt-in. Absent/false
retains Dva's default linear-FP16 path. The trial requires encoded Gamma22 or
SRGB source metadata and exact `R8G8B8A8_UNORM` Color/Output. It byte-copies color
before/after NR, admits an explicit `ColorDomain::SdrBytes` packet and queries
RGBA8 typed UAV support before retaining/recording the first SDR packet.
Depth/motion preparation, source provenance, resets, aliases, fences, alpha
restoration and uncertain-ownership retention keep their existing checks.

Use one Before pass, preset 0, native input scale 1, Auto resolve, Tone 1,
Style 0, intensity/structure/skin 1, masks and UI correction off. Disable
Stable colors: it requires linear color and is unavailable in this route.
NR enabled/tuning remain live; changing the color contract requires restart.

DLSS/DLAA receive encoded NR color through their existing native source route.
FSR receives encoded NR color and uses its ordinary source decode; this route
never exports encoded bytes as a prepared linear FSR lease. Keep FSR Native AA,
sharpening/FG off, Gamma22 and the existing ReShade-before-upscaling choice for
the first camera comparison. No new source-encoding calibration is claimed.

## Verification scope

The initial packet regression failed before admission was implemented. The
standalone suite passes **63/63**, including five SDR cases:
`NrPreparedBefore-sdr-bytes`, `NrBeforeHostFrames-sdr-bytes`,
`NrReShadeSdrBefore`, `NrPreparedFsr-sdr-bytes`, `NrReShadeSdrFsr`.
Prepared/host tests check exact source alpha, unchanged disabled-source bytes,
NR RGB changes, off/on, host resize and retirement. The FSR fixture executes
18 real temporal FSR evaluations, including one disabled NR source; empty
linear leases and measured FSR color preparation prove ordinary decode runs.
It checks 1,036,800 source alpha pixels, not FSR output-alpha readbacks.
The supplied ReShade wrapper must expose wrapped interfaces in its cases.

The NR-enabled product suite passes **188/188**, with the same three explicit
Graphics Tools exclusions (the layer DLLs are absent). [The receipt](sdr-byte-trial-validation.json)
pins test logs and source hashes. The updated format probe
[smoke](sdr-format-smoke.json) matches all nine retained historical Style-0
CSV/RGB/raw captures and still measures 8.83886 code values of synthetic drift.
This verifies contract parity, not a drift fix.

These are synthetic sources on RTX 4080 SUPER. ReShade effect edits, Skyrim
camera stability, generated-frame NR, RTX20/30/50 output and performance parity
are not qualified by these checks. AMD NR remains unsupported. No standalone
result advances the **1 of 8** milestone count.

## Skyrim acceptance

After installing the validated trial, start using the existing MO2 SKSE entry,
load the same save and press End. Confirm `NR active ... SDR RGBA8 byte trial
pass`, full Tone 1, Style 0 and Stable colors off. Compare NR off/on to confirm
visible processing, then rotate around the same building. A stable image must
retain the requested style effect. Test Style 1 as a control and Apply/off/on
without freezing. Leave FG off for this diagnosis. Report whether the same
surface changes color; logs alone cannot qualify visual camera stability.


## Deployment checkpoint

Clean source `5d9a3b873b72` is built and installed in the existing V5.4 NO-LORE
Dva trial, with only DLL/INI/manifest updated. The profile switches Raz off and
Dva on; AIO stays off. Only Style 0 and the new startup trial selector change
in the Dva INI. Tone 1, Stable colors/FG off, FSR Native AA and launch settings
are preserved. [The installation receipt](sdr-byte-trial-installation.json)
records exact hashes, rollback and protected settings. NR-disabled compilation
also succeeds; its receipt distinguishes the earlier precommit build identity.
Skyrim camera acceptance is **PENDING**; the completion count stays **1 of 8**.


## First gameplay result

The owner reports **“seems drifting fixed”** in the installed SDR-byte trial.
The [running-game receipt](sdr-byte-trial-game-result.json) confirms active
Style 0 / Tone 1 processing, Stable colors/FG off and a successful live NR
off/on cycle. No error/critical line appears in its captured prefix. This is
provisional qualitative camera acceptance in the reported scene. The old
synthetic context fixture still drifts; other scenes/styles, FG and DLSS/DLAA
remain unqualified. No settings changed during this log check, and **1 of 8**
remains complete.
