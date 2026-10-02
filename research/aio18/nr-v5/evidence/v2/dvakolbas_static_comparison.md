# DvaKolbas static comparison — NR Pass 2

Reviewed DvaKolbas head: `7f4ac08539b9cf655bfd2d0e6fc0f8c344daeb34`.

## Confirmed matches

- NGX key vocabulary.
- 310.8 reconstruction runtime identity.
- `SetI` for AutoMask/Reset/DepthInverted/Enabled/UICorrection and `SetUI` for Style.
- Float tuning/motion scales.
- Separate private reconstruction/resolve logic.

## Static differences requiring tests, not automatic fixes

1. AIO18 core NGX call forces `UI` and `UIAlpha` null; Dva forwards them when supplied.
2. AIO18 core NGX call forces `UICorrection=0`; Dva forwards requested UI correction to NGX.
3. AIO18 starts common subrects at zero and selectively transforms them; Dva writes explicit full rectangles.
4. AIO18 can route/fallback Backbuffer when the external value is null; Dva requires a non-null Backbuffer.

These differences are not declared bugs without Pass-3/Pass-4 or runtime evidence.
