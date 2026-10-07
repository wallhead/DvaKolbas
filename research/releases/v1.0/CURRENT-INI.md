# Current INI layout only — 2026-10-07

The v1.0 archive now contains Universal build `e9bc73fe19f4`. The earlier release
receipts describe historical packages; `current-ini-verification.json` identifies
this update and the installed copies.

Old mapped settings do not configure the renderer, even if their current
replacements are absent. Saving discards recognized obsolete keys and does not
write ConfigVersion. TheosRenderPipeline.ini is never copied to RaZkolbaS.ini;
old resource folder names are not redirected. The duplicate renderer DLL guard
remains in place. Internal writer slots are adapted to the current public layout;
this adapter does not accept old external values.

The current layout preserves explicit zero/false/empty values, independent NR
passes, edited startup paths and menu geometry. Old PureDarkHUDFixMethod and
RasterFPSLimit fallbacks are removed. Package tools discard obsolete values
rather than migrate them.

Validation: the new obsolete-key and filename tests failed before the fix, then
passed. Twelve focused settings checks and 70 selected settings/compatibility
checks pass. The Universal plugin built successfully. All 18 archive entries were
verified, with unchanged runtime payloads and passing ZIP CRC. TESV_EX and V5.4
DLL/INI pairs were backed up and updated; every user setting except ConfigVersion
is preserved. MO2 profiles were untouched. No new Skyrim gameplay test was run.
