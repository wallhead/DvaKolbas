# Commit review fixes — 2026-10-05

This change follows validation of the six new findings in the supplied
`COMMIT_REVIEW_2026-10-05.md`, against baseline `34c1702`. The attachment's
recommendations were evaluated individually.

## NR initialization recovery (N1)

A Stage can report `InitializationRolledBackBeforeCreate()` only after parameter
destruction, runtime-client release and allocator-callback release succeed, with
a healthy retained D3D12 device and no vendor feature-creation attempt. Device
health is checked before and after parameter cleanup. Any creation call crosses
the retention boundary, including failed calls and successful calls returning a
null handle.

BeforeUpscale, PreparedBeforeUpscale and PostUpscale propagate this proof.
Prepared-state self-retention is released only on the confirmed safe path.
BeforeHost additionally verifies both devices, a ready runtime with no clients,
shim ownership and successful runtime retirement before disabling NR for the
session. Source rendering continues, with `wasActive || input.reset` preserving
the correct transition reset. Later frames do not retry NR or force more resets.
The status keeps the original failure reason and native error code.

Failed cleanup, partial three-pass creation, failed shutdown, removed devices
and failures without explicit rollback proof remain terminal with uncertain
ownership retained. Wrapper failures before Stage initialization are not newly
classified as recoverable. Existing partial Init_Ext quarantine checks (N2)
remain active; explicit live FG checkbox persistence (N3) is unchanged.

## Legacy NR pass count (N4)

A saved community three-pass preference no longer blocks legacy startup or
Apply. The existing legacy execution sanitizer still caps actual execution at
two passes. The menu shows Pass 2 enabled for a saved three-pass preference and
explains the cap; configuration logs also report it. Unrelated Save operations
keep PassCount=3 and independent Pass 3 overrides for a later return to the
community runtime. Explicit pass-count edits still change the saved choice.

FSR ownership, build availability, native UI and other compatibility validation
remain enforced. This does not extend legacy execution to three passes.

## AMD documentation (N6)

The separate experimental AMD bridge documentation was corrected in commit
`06f542b`, pushed to `amd-nr-bridge`. It distinguishes loading, hooks, staging
and actual NR execution, and qualifies the D3D11 diagnostic rather than treating
the string as proof of unconditional rejection. Published requirements checked
on 2026-10-05 list RX 9000 tested, RX 7000 expected and older GPUs unsupported;
unqualified RX 6000/HIP SDK support claims were removed.

No AMD bridge code is merged into this branch and no AMD execution is qualified.
The temporary documentation checkout was archived after its committed changes
were pushed. N5's exact total-memory multiplier and parameter-rotation drift
claim remain unproven; the historical StableColorResolve shader test is retained.

## Verification

- BackendSelection and RendererSettingsActions reproduced the legacy rejection
  before their fixes; both and NeuralPassSettings passed afterwards.
- NR preparation/host regressions reproduced the terminal safe-failure behavior
  before implementation. Additional regressions first failed for lost native
  status codes and premature cleanup after device removal.
- The final focused suite passed 31 checks, including 30 serial GPU modes across
  Before/After placement, parameter allocation/ABI rejection, partial parameter
  ownership, failed destruction, null features, partial creation and failed
  shutdown. Source bytes remain unchanged and failed-preparation weak references
  expire on the safe path. First-launch and restart reset behavior are distinct.
- GPU fault fixtures use real devices, pinned runtime initialization, shared
  resources and shutdown, while replacing low-level parameter/create exports.
  Restart fixtures simulate a previously retired preparation; existing lifecycle
  tests cover genuine successful creation, reader retirement and menu changes.
- Release plugin and all configured test targets built successfully.
- All **259 selected noninteractive checks passed**, including **93 GPU** and
  **seven ReShade** checks, with zero failures/skips in 226.27 seconds. The
  three debug-component and three foreground-window exclusions were not run.
- Independent source review found no confirmed P1/P2 issues in the final diff.

Local build/test logs and JUnit results are under
`out/research/commit-review-*`. Three Graphics Tools checks and three interactive
NVIDIA foreground-window checks are excluded from the integrated run. These
exclusions are not counted as passes. No additional GPU/driver/model combination
is qualified. A new Skyrim launch remains required for the final trial DLL.
