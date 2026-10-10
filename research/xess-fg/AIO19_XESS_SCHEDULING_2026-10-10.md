# AIO19 XeSS FG: Present callback and next-frame timing follow-up

2026-10-10. Read-only Capstone/pefile inspection of the pinned Build19-Hotfix1
PDPerfPlugin.dll, SHA-256
`ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435`.
The DLL was not loaded, patched or executed. All addresses are PE RVAs.

## Result

The formerly untraced assignment and Present callback path is now connected
through concrete stores, vtable entries and decoded calls. AIO19 installs
callbacks into its outer D3D11 swap-chain wrapper, forwards them to the selected
XeSS FG object, and advances/sleeps the next SDK frame after the inner Present.
This strengthens the static evidence for a post-Present reservation schedule.
It does **not** establish when Skyrim samples input relative to that sleep.

The two decoded direct callers of its fallback simulation-marker helper are
FG evaluation and pre-Present. Their late placement is not a genuine input
boundary. Our gameplay handoff must retain its independent input proof.

## Dispatch chain

| Boundary | Witness |
|---|---|
| Select method 3 | `0x1076e6` compares 3; `0x107704` constructs XeFG at `0x84d50`; `0x107714` stores selected object at controller `+0x148` |
| Attach selected transport | `0x10782f` selects transport; `0x107858` calls its slot `+0x20` |
| Construct outer chain | `0x1078aa` calls `0xfe620`; constructor installs vtable `0x12f8110` at `0xfe673/0xfe67a` |
| Bind selected object | `0x1078c1` stores controller `+0x148` into global `0x1375df0` |
| Install callbacks | `0x1078c8/0x1078cf` set outer `+0x18` to `0x107750`; `0x1078d3/0x1078da` set `+0x20` to `0x1077f0` |
| Ordinary Present route | Outer slot `+0x40` is `0x100ad0`; `0x100cf8` calls `0xff240` |
| Synchronous route | `0xff55e` calls `0xff5f0`, which calls preparation `0x101470` at `0xff893`, then presentation `0x101870` at `0xff8c3` |
| Invoke pre callback | `0x1017cf` loads outer `+0x18`; `0x1017e3` invokes it with command list, buffer index and buffer resource |
| Forward to XeFG pre | `0x1077b0` loads selected global; `0x1077d7` tail-dispatches slot `+0x68`, verified as `0x86fd0` |
| Inner Present | `0x101901` calls inner swap-chain slot `+0x40` |
| Invoke post callback | `0x1019aa` loads outer `+0x20`; `0x1019ba` invokes it after inner Present with buffer index/resource |
| Forward to XeFG post | Leaf `0x1077f0` loads selected global; `0x107808` tail-dispatches slot `+0x70`, verified as `0x87340` |

The post callback receives buffer index/resource, **not the Present HRESULT**.
The inspected XeFG post method uses its own current ID and SDK status. The
outer routine also calls this callback on some paths that skip inner Present
because of recovery conditions. Therefore, copying its unconditional next-ID
advance into our lifecycle would lose our actual-success and source guards.

Outer Present1 slot `+0xb0` resolves to leaf `0xa9a70`, which forwards to its
own vtable slot `+0x40`. The explicit seven-byte witness is
`488b0148ff6040`; this leaf has no unwind entry.

The optional worker branch in `0xff240` packages work and signals a condition
variable instead of taking the synchronous branch above. This follow-up does
not qualify its executing thread, cross-adapter paths, or every error branch.
Helper `0xed2d0` updates a TLS-associated completion flag; it is not itself the
XeFG post callback. Helper `0x107f60` checks a keyboard chord; it is not itself
the XeFG pre callback. These distinctions avoid conflating adjacent helpers
with the timing boundary.

## Marker schedule

The existing SDK call witnesses establish the following within the selected
XeFG class:

- `0x85c39` (evaluation) and `0x87273` (pre-Present) directly call `0x85670`.
  This helper emits missing Sleep/SimulationStart behind flag `+0x1e6`, then
  SimulationEnd/RenderSubmitStart behind flag `+0x1e7`.
- Pre-Present sets SDK Present ID from `+0x1d8` at `0x871f5`, then emits
  RenderSubmitEnd and PresentStart at `0x8728b/0x872a4`.
- Post-Present emits PresentEnd at `0x8736e` and queries actual SDK status at
  `0x873ec`. At `0x874cd` it increments `+0x1d8`, clears the second guard,
  sleeps the next ID at `0x874e7`, starts that ID's simulation at `0x874fd`,
  and sets the first guard.

The E8 scan verifies both direct callers against decoded unwind instruction
boundaries. It does not establish that no indirect caller exists elsewhere.
These static results do not prove engine-thread affinity, physical input
sampling, or successful generation during an AIO gameplay run.

## Consequence for the installed RaZkolbaS trial

Candidate `000ccf57b3c7` already reserves the next SDK ID after successful
owner Present, then requires a completed native Skyrim input job for that
exact reservation before render and final source sealing. This research
supports trying that schedule; it supplies no reason to remove its proof,
128-frame qualification, repeated-output, menu or recovery guards.

The Skyrim log currently available is still source `4c6efe59a725`, so it is
not evidence for the installed candidate. In-game validation of the new
candidate remains pending. No runtime code, installed DLL, INI, MO2 settings
or published archive was changed during this follow-up.

## Reproduce

```powershell
python tools/xess/Inspect-Aio19Scheduling.py --extracted out/research/aio19/extracted --output out/research/xess-fg-reference/reproduced-scheduling.json
```

The inspector uses the existing pinned Evidence reader, verifies 15 decoded
windows, two explicit leaf byte ranges, four vtable slots, and both fallback
direct callers. The [saved JSON](aio19-scheduling-witnesses-2026-10-10.json)
contains actual instruction bytes. For SDK call semantics and initialization,
see the [earlier implementation report](AIO19_XESS_FG_IMPLEMENTATION_2026-10-09.md)
and its [call witnesses](aio19-fg-call-witnesses-2026-10-09.json).
