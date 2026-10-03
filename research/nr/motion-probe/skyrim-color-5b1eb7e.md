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

The next one-variable game check sets only `NRLocalTone` from 1 to 0 in the
trial INI. The original INI is backed up locally. This is a visual hypothesis
about the model's scene-dependent tone adjustment, not a production default
change; intensity, structure, sharpening, model, SR and FG settings remain as
before. The user will compare the same building and report whether the shift
is gone, reduced or unchanged. If unchanged, restore 1 and investigate other
color/guide inputs rather than changing multiple controls together.
