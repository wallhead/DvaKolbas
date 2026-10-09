# AIO19 XeSS FG implementation: static call-site evidence

Date: 2026-10-09. Scope: Build19-Hotfix1, read-only PE inspection with
Capstone and pefile. No AIO DLL was loaded, patched or executed. These findings
describe code present in the binary, not a captured gameplay execution.

## Result

AIO19 has a concrete Intel D3D12 FG wrapper, not just SDK names in a loader.
It creates XeLL and FG contexts on the same supplied D3D12 device, links them,
creates or wraps a DXGI swap chain, tags guides/constants with matching frame
IDs, surrounds Present with XeLL markers, and reads actual SDK presentation
status. Its main SDK calls use **PE delay imports**. An earlier GetProcAddress
witness alone did not establish this implementation.

The useful model for RaZkolbaS is the public Intel proxy/context contract and
frame-ID ordering. Resource retirement, engine timing and HUD composition
still need independent qualification. We should not infer those contracts
from the presence of a tag or copy AIO's provider-specific patch policy.

## Reproduce and identify

From the repository root, with pefile and Capstone available:

```powershell
python tools/xess/Inspect-Aio19Generation.py --extracted out/research/aio19/extracted --output out/research/xess-fg-reference/reproduced-calls.json
```

The tool rejects callers that do not match these identities:

| Caller | Bytes | SHA-256 |
|---|---:|---|
| `UpscalerBasePlugin/PDPerfPlugin.dll` | 20,332,032 | `ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435` |
| `SKSE/Plugins/SkyrimUpscaler.dll` | 15,975,424 | `ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a` |

Archive/member provenance and byte-identical Intel SDK 3.0.2 runtime identities
are in [the earlier SDK witness](../xess/aio19-sdk-witnesses-2026-10-09.json).
The new [call witness JSON](aio19-fg-call-witnesses-2026-10-09.json) records 33
FG/XeLL call sites, 15 supporting instruction windows, two vtables and the
Skyrim plugin's FG dispatch. Each instruction includes its actual bytes.
No new runtime binaries are checked in.

All addresses below are RVAs in PDPerfPlugin unless labelled SkyrimUpscaler.
Unwind ranges bound disassembly fragments; they are not necessarily complete
C++ functions. Struct interpretations use the public headers at Intel commit
`8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0`.

## Initialization and presentation ownership

| Operation | Call RVA | Evidence |
|---|---|---|
| Query FG version | `0x84f86` | `xefgSwapChainGetVersion` |
| Create XeLL | `0x850d3` | Native D3D12 device at wrapper `+0x148`; context at `+0x198` |
| Set sleep mode | `0x8512a` | `minimumIntervalUs=0`, low-latency bit set |
| Create FG | `0x8513e` | Same device; context at wrapper `+0x180` |
| Connect latency context | `0x851a8` | `xefgSwapChainSetLatencyReduction(FG, XeLL)` |
| Wrap existing chain | `0x86442` | `xefgSwapChainD3D12InitFromSwapChain` |
| Create chain from translated legacy descriptor | `0x86802` | `xefgSwapChainD3D12InitFromSwapChainDesc` |
| Create chain from newer descriptor | `0x86a01` | Same public initialization API |
| Obtain actual proxy | `0x86561`, `0x86835`, `0x86a40` | `xefgSwapChainD3D12GetSwapChainPtr` |
| Query properties | `0x86c30` | `xefgSwapChainGetProperties` |

The legacy descriptor path selects flip-discard (`4`) and builds FG flags
from wrapper booleans: inverted depth (`1`), high-resolution motion (`4`),
jittered motion (`16`). This is conditional configuration, not proof that
all three flags are enabled in Skyrim. The init struct's `uiMode` is zero
(`AUTO`) in this path. The optional external heaps and pipeline pointer are
zero-initialized.

The ordinary maximum interpolated-frame request starts at one. A separate
provider helper at `0xb5050`, called from `0x85089`, changes a branch to a
literal five at `0x850b5`; the later properties path bounds the effective
count. This is **not evidence of official five-frame generation on non-Intel
hardware**, nor a runtime proof of any requested count. Our first backend
should use the official supported one-generated-frame path.

## Frame resources and constants

The FG evaluation fragment `0x85920..0x8631a` calls:

| Call RVA | SDK resource type | AIO generic input | Validity |
|---|---|---|---|
| `0x85e30` | UI (`3`) | Pointer at input `+0x88`, optional | Until next Present (`0`) |
| `0x85ed3` | Motion vector (`2`) | Pointer at input `+0x10` | Until next Present (`0`) |
| `0x85f77` | Depth (`1`) | Pointer at input `+0x18`, possible wrapper override | Until next Present (`0`) |

Each description writes type and validity together as a 64-bit value `3`,
`2` or `1`. The high 32 bits are zero, which is the public
`XEFG_SWAPCHAIN_RV_UNTIL_NEXT_PRESENT` value. Sizes come from the selected
D3D12 resource description. Motion and depth resource bases come from input
offsets `+0x80/+0x84` and `+0x78/+0x7c`; the UI base is zero. Incoming resource
states come from helper `0x76670`, rather than a hard-coded state. These calls
use the command list at input `+0x68` and frame ID at wrapper `+0x1d8`.

No HUD-less colour tag (`type 0`) occurs in this inspected evaluation fragment.
The presentation back buffer supplies colour implicitly. We did not find an
explicit UI-composition enable call in this wrapper or its FG delay imports.
`AUTO` alone does not enable composition: Intel's SDK defaults to UI
interpolation. This limits what we can conclude about AIO's HUD policy; it
does not establish a global absence of composition in every helper/path.

`xefgSwapChainTagFrameConstants` at `0x86191` uses the same ID. The constants
are a 152-byte public struct. A contiguous 16-byte copy at `0x86144` transfers
jitter X/Y and motion scale X/Y from input `+0x44`. Reset is set from the input
reset byte or a wrapper reset request. Render time is the difference between
successive helper-clock readings, converted to float. Helper `0x836a0` uses
QPC times **1000 / frequency**, or GetTickCount, so the value is milliseconds.
The conditional camera-matrix helpers were located, but their complete
matrix-space/unjittering contract has not been proven here.

## Frame identity, timing and output statistics

Before Present, `0x86fd0..0x87335` sets enabled state (`0x870bf`) and calls
`xefgSwapChainSetPresentId` at `0x871f5` with wrapper ID `+0x1d8`. It emits
RenderSubmitEnd (`3`) and PresentStart (`4`) at `0x8728b` and `0x872a4`.

After Present, `0x87340..0x87520` emits PresentEnd (`5`) at `0x8736e`, then
calls `xefgSwapChainGetLastPresentStatus` at `0x873ec`. The status lives at
wrapper `+0x188`; the pre-Present statistics callback uses its actual
`framesPresented` value, clamped to at least one for that callback.

At `0x874cd` the wrapper increments the frame ID. It calls XeLL Sleep for
that **next ID** at `0x874e7`, then emits SimulationStart (`0`) at `0x874fd`.
This places normal next-frame sleep after the preceding Present. A fallback
helper `0x85670` emits missing Sleep/SimulationStart, SimulationEnd (`1`) and
RenderSubmitStart (`2`) behind per-frame guards. Clustering fallback markers
before Present is not proof of actual simulation timing.

For RaZkolbaS, post-Present sleep is a candidate boundary only after proving
it precedes the next Skyrim input sample and occurs once per real source
frame. Extra/loading Presents must not create invented simulation cycles.
The existing renderer jitter hook does not by itself prove this timing.

## Dispatch and teardown

The SkyrimUpscaler delay-import call to `EvaluateFrameGeneration` is at
`0x2e92dc`. PDPerf's exported function at `0x116410` copies a generic frame
and dispatches through active controller vtable offset `+0x40`. The D3D11
controller vtable at `0x12facf0` resolves that slot to `0x112760`; it dispatches
to its selected method's slot `+8` at `0x112820`. The XeFG constructor
`0x84d50` installs vtable `0x12eef10`, whose `+8` slot is evaluation `0x85920`,
and whose `+0x68/+0x70` slots are the pre/post-Present methods above.
Transport selector `0x107480` constructs this XeFG class for method value
`3`. The complete controller-to-transport assignment and game Present
hook chain remain untraced; the witnesses do not assert a full execution
path or establish AIO's NR/SR ordering.

Release fragment `0x853a0` calls FG Destroy at `0x85452` and XeLL Destroy at
`0x85576`, in that order. It also clears context fields after attempts. We
have not proven a complete GPU/proxy reader drain around those calls.
In particular, helper `0x70410` polls device-loss/recovery state; its presence
is **not** a GPU-fence drain witness. Our owner must retain pending resources
on timeout/destroy failure under the public SDK's teardown requirements.

## Design consequences

1. Use an independent public Intel DXGI proxy owner and same-device XeLL.
2. Preserve `SR -> NR -> FG -> UI`, explicitly enable and qualify dedicated
   HUD composition instead of inferring it from AIO's UI tag.
3. Keep our initial `RV_ONLY_NOW` copy/tag plan. AIO's retained-resource
   mode requires lifetime proof we do not yet have for mutable Skyrim input.
4. Track one consistent SDK ID across sleep, markers, constants, tags and
   Present; investigate post-Present as a next-input timing candidate.
5. Publish actual `GetLastPresentStatus` counts and errors, not nominal 2x.
6. Destroy FG before XeLL after proven quiescence. Do not port private
   provider patches or infer cross-vendor MFG from AIO's request literal.

This is implementation research. Intel FG is not yet added to RaZkolbaS,
and no Skyrim/MO2 files or installed runtimes were changed for this inspection.
