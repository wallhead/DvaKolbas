# NR static review validation — 2026-10-05

The supplied review was checked against `1464d909cd25` and the current sources.
The four priority items were reproduced or verified before changing production
code. This update preserves DLSS/FSR → NR → FG → UI and existing hardware,
native-resolution, runtime-hash and GPU-retirement requirements.

## Fixed

| Finding | Verification and change |
| --- | --- |
| Main plugin misses IPO | The actual generated plugin policy had no IPO property. Both IPO defaults now precede `add_library`; Release plugin output includes whole-program optimization and LTCG, while Debug keeps its disabled override. |
| Compiler flags depend on Visual Studio | Both generator conditions were present. They now use `MSVC`, including Ninja with `cl` or `clang-cl`. `/MP` is restricted to the MSVC compiler itself; clang-cl receives the applicable language, warning and architecture options. |
| Legacy module-path hook spoofs unrelated modules | A real fixture DLL querying kernel32 reproduced substitution in both A/W imports. Only the module containing the proxy is now substituted; feature, unrelated and executable/null queries retain their actual paths as appropriate. |
| Hook restoration loses ownership/state | A later IAT owner reproduced destructive restoration and loss of callback data. Compare-and-swap restoration now leaves later owners alone. Failed imports retain their callback state and target/caller module references; completed restorations are removed from the pending list. No new owners join during failed restoration. A separate fixture denies `VirtualProtect` for one import and proves its remaining proxy stays usable until restoration succeeds. |
| ANSI path conversion loses characters | The old hook accepted an unrepresentable Unicode loader path. Conversion now checks default-character substitution and disables best-fit mappings, with the appropriate UTF-8 branch. Inexact conversion rejects installation before any import changes and logs the cause. |
| Timing phases collected repeatedly | A real retired D3D11 query fixture reports one phase pending once. The old collector re-read the completed phase on retry. It now consumes that phase's masks once. The claim of accumulated duplicate statistics was **not confirmed**: `PerformanceMetrics::RecordGpuFor` overwrites the same frame/phase value rather than adding another sample. |

The [Windows conversion documentation](https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte)
describes default-character detection, best-fit behavior and the distinct UTF-8
parameter restrictions used by the conversion fix.

## Remaining policies and follow-up work

- **Timeouts:** confirmed. Before drain/reconfiguration uses a shared 20-second
  deadline; Stage creation uses 15 seconds and legacy creation 30 seconds.
  An unproven GPU retirement retains resources and becomes terminal. These waits
  can block rendering. They are NR ownership policy, not a promise of the FG
  backend's recoverable behavior. Automatic retries or progress-relative
  deadlines require separate lifecycle design and fault-injection validation.
- **Legacy FeatureSession hardening:** confirmed gaps in share-denied file
  leases and DLL dependency search policy. Its public NGX Init call has no
  paired session-owned Shutdown. Adding shutdown needs explicit ownership
  across DLSS and both legacy NR sessions; calling global shutdown from one
  session could affect another. This patch does not change that lifecycle.
- **Drain enforcement:** the `NeuralPass` type itself does not require a
  retirement token, but actual backend resets call `Interop::Drain` first.
  `Backend::Quiesce` also stops presentation and returns before destruction on
  failures. The swapchain destructor retains its buffers when quiescing fails;
  the backend itself is a process-lifetime owner. Thus “comment
  only” is accurate at the type boundary, not for these current call sites.
- **Filesystem identity:** `RuntimeFileLease::Matches` requires successful
  `FileIdInfo` queries and otherwise returns false, losing the detailed reason.
  The local C: and D: volumes are NTFS and passed lease checks. exFAT was not
  tested, so this update does not claim support or confirmed failure there.
  Improve the typed diagnostic before considering any fallback; do not replace
  held-file identity with a path-string or same-hash comparison.
- **Allocator ownership:** confirmed and now documented on `Stage`. NGX
  allocation callbacks have no per-client context, so one initialized Stage
  owns them process-wide until its features, allocations and readers retire.
- **Pinned caller shim:** unchanged. New runtime/driver binaries still require
  identity and compatibility qualification. This review adds no hardware or
  runtime support claim.
- **Formatting:** broad formatting of Stage/Before is deferred to a separate
  change so this ownership fix remains reviewable. The added hook handling and
  regressions use separate statements around the changed paths.

## Validation

- New plugin-policy, hook and partial-query tests failed against the original
  behavior before fixes, then passed.
- Full Visual Studio Release build passed with plugin IPO enabled.
- All **220 selected noninteractive checks passed**, including 57 GPU and six
  ReShade checks. Interactive tests and the three existing Graphics Tools
  exclusions were not counted as passes.
- The additional denied-protection restoration fixture passed after the broad
  suite. Independent review found no remaining Critical or Important issues;
  its single-config Ninja test finding was corrected and verified.
- Actual Ninja Release configuration/policy checks passed for both `cl` and
  `clang-cl`; a separate Ninja/cl Debug configuration verified the IPO override.
  These are configuration checks, not complete Ninja plugin builds.
- A new Skyrim launch is still needed to validate the updated plugin in game.

Detailed build/test logs are local ignored artifacts under
`out/research/nr-static-review/`; no vendor runtime binaries were added to Git.
