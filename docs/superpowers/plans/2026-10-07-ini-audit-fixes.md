# INI audit fixes

Execute inline using executing-plans and test-driven-development; obtain one final fresh-context whole-diff review.

Scope: verified A1 public diagnostics, A2 integer-only controls, A3 clear removal guidance for obsolete release switches, A4 fixed-list validation, A5 symmetric hotkeys. Keep numeric sanitizers, independent encoding, pass overrides, runtime paths and rendering order unchanged. Missing-file defaults are unreachable in normal startup and remain unchanged. No installed mod/profile edits while MO2 is open.

1. Add failing runtime audit tests and package parity checks for integer input, fixed choices, hotkeys, obsolete controls and actionable public recovery text.
2. Update the shared schema, decoder/package codec, generated public references and diagnostic messages. Maintain floating-point menu coordinates and existing range sanitizers.
3. Build Universal and relevant tests, review the diff and correct confirmed defects.
4. Commit/push, rebuild a clean DLL and stage a matching INI preserving current values. Record identity and hashes outside the checkout.

Ledger: audit reproduced Style=1.0 -> 0 and generated count 2.0 -> 1. Trailing spaces and fractional menu geometry are valid. No old-INI aliases or layout version will be introduced.

Progress:
- Pre-flight: Tasks 1/2 share the integer/choice/hotkey schema; runtime and package codec must accept the same public values. Tasks 2/3 share generated messages and CMake tests; templates remain unchanged in values. Task 4 consumes the clean build and current-profile INI.
- Task 1 complete: runtime audit failed with 61 regressions; package and message tests also failed on the intended defects.
- Task 2 complete: audit, public package and message regressions pass. The 12 integer fields use signed 32-bit lexical validation; menu coordinates remain floating point.
- Task 3 verification: Universal plugin and relevant targets built; 22/22 selected CTest checks pass. Invalid encoding package fixtures now corrupt files directly because the writer correctly rejects invalid fixed choices. Final read-only review pending.
- Scope decision: retain numeric sanitizers and missing-file fallback defaults. Independent encoding and linked pass overrides preserve existing behavior; no automatic old-layout support.
- Final review: no Critical findings. Integer documentation was regraded from Minor to Important because a value within the advertised range (1.0) now prevents startup; users need the accepted type in the file. Added separate Whole numbers only comments in templates, generated reference and package formatter. New guidance regressions failed first, then passed.
- Review scope decisions: keep numeric clamps and missing-file fallback defaults (unchanged runtime semantics); retain separate FSR/NR encoding and linked pass overrides (different pipeline formats and saved unlink settings); GPU rendering is outside this configuration-only change and has not been retested.
- Task 3 complete: Universal build passes; 22/22 relevant checks pass after the review fix. No deferred findings.
- Task 4: commit/push the verified changes, then rebuild with clean embedded source identity and stage matching DLL/INI. External verification.json records delivery hashes and preserves every current-profile value; installation is separate.
