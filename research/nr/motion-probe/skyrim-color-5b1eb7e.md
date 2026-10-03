# Skyrim NR color-motion capture, 2026-10-03

V5.4 NO-LORE used clean Standard plugin `5b1eb7e7df2b` (installed DLL SHA-256
`7b78c5366ca0a66a5076752801f044165983d28fce627779297fa6a77886d0ad`),
RTX 40 NR Before and FSR NativeAA. FG generation was active in this captured
run; the user had separately reproduced the shift with FG off. The user
reported that the same color shift remained and had previously confirmed that
it disappears when NR is off. This is **not** visual acceptance or an
established root cause.

With temporary `[Debug] LogFrameDiagnostics=true`, the plugin took 16 bounded
read-only staging samples of its actual RGBA8 source color immediately before
and after NR, plus the RG16F motion texture. Normal logging was restored after
Skyrim and MO2 closed. The complete local game log is retained outside Git in
`out/research/nr/install-backup-motion-probe-5b1eb7e/`.

- First world sample had reset=true and 41,867 of 57,600 sampled motion vectors
  outside normalized UV range. It occurred during the transition into the
  world and is excluded from settled color interpretation.
- The next 15 samples had finite motion values with zero outside-UV counts.
  NR stayed active when requested. User off/on toggles at source 8774 and 9250
  changed the settings revision as expected; there was no unrequested toggle.
- At revision 3 and reset=false, near-still samples 8 and 13 had mean motion
  magnitudes 0.14 and 0.10 pixels, yet NR's mean signed blue correction was
  respectively +5.68 and -1.87 8-bit code values. These are different views,
  so they establish a content-dependent change **within NR output**, not a
  same-pixel temporal color jump or the cause of the perceived building effect.
- Input and output were sampled in the same source frame, before FSR upscaling
  and FG. This excludes the weather preset controller and FG from the measured
  RGB difference. Depth was logged as format 44 (`R24G8_TYPELESS`); depth
  pixels were not read back. Global RGB means may hide localized changes.

The one-variable game check changed only `NRLocalTone` from 1 to 0 in the
trial INI. Intensity, structure, sharpening, model, SR and FG settings remained
as before. The original INI is backed up locally. The user reported **"Effect
gone"** in the same building comparison. Retain Local tone 0 in this trial;
this resolves the reported color-shifting effect in the tested setup without
changing the global product default or removing the live Local tone control.

The following live log confirms active RTX 40 NR Before, successful off/on
recovery at sources 32821/32909, temporal FSR and actual FG generation. No
rendering error was present in the inspected log. The successful visual report
therefore covers the captured FG-on session; an FG-off setting was requested
but was not observed in the log. The installed INI hash is
`d72e2a1fa972a15b5b8fdc5e86b63b2a367d94ac06f8d7ec18f384c5bff81ae4`,
with `NRLocalTone=0` and normal frame logging disabled.

This is bounded user acceptance of the previously reported color effect,
not complete NR image-quality, performance, lifecycle, other-GPU or true-After
qualification. Full NR milestone count remains 1 of 8.
