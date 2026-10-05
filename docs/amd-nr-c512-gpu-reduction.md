# Portable C512 GPU reduction

On 2026-10-05 the source-owned D3D12 2x2 half reduction matched an independent
NumPy oracle for 1,007,616 packed output bytes on each of CPU, WARP and the
explicitly selected RTX 4080 SUPER. This includes 956,416 active values and
51,200 preserved padding bytes, with zero mismatches on each path.

## Contract and implementation

`C512ReductionProbe` consumes validated source/destination C512 layouts,
canonical raw half input and initial packed FP8 destination bytes. Canonical
order is `[X,Y,channel]`, Y before X. Active width/height are independently
`min(source/2,destination)`. Every inactive byte keeps its caller-supplied value.
The arithmetic matches `ReduceC512Half2x2`: two half-rounded pair sums, a
half-rounded sum of those pairs, and a half-rounded quarter multiplication,
followed by the existing nearest-even E4M3 encoder.

The diagnostic accepts at most 4096 source pixels / 2,097,152 half values, and
1024 destination pixels / 524,288 packed bytes. Existing immutable layouts
require positive axes aligned to four. Exact spans and limits are checked before
payload allocation. The wrapper snapshots a four-word extent header, input half
words and initial output bytes into explicit little-endian uint32 transport.

One shader thread owns a complete packed output word. Each of its four byte
coordinates is mapped back to pixel/channel, reduced when active, or retained
from the initial word. There are no shared-byte UAV writes or atomic operations.
Input/output/work counts are distinct and bounded. The existing format probe
provides same-device DIRECT queue validation, fences, pending-resource ownership,
timeouts/drain and shutdown retention. Readback returns packed bytes; errors
propagate through the wrapper, with allocation failures reported as Memory.

Original intermediate NaN payloads need no claimed equivalence for this FP8
output diagnostic: every NaN output encodes canonically. No supplied code is
executed. This implements the selected recovered pooling contract, and enables
no model block or rendering route.

## Validation

| Generated fixture | Source -> destination | Compared packed bytes | Preserved padding |
| --- | --- | ---: | ---: |
| Exhaustive half/seeded tuples | 64x16 ->32x8 | 131,072 | 0 |
| Padded | 60x36 ->32x20 | 327,680 | 51,200 |
| Cropped | 16x12 ->4x4 | 8,192 | 0 |
| Maximum accepted size | 64x64 ->32x32 | 524,288 | 0 |
| Asymmetric coordinate/channel pattern | 8x16 ->4x8 | 16,384 | 0 |

The first fixture uses every half encoding in the first position, 65,525 seeded
random four-half tuples and eleven literals for pair rounding, negative zero,
NaN/infinity, overflow, subnormals and FP8 midpoint neighbors. The true
zero/first-subnormal midpoint is half `0x1400` (2^-10); `0x1401` lies above it.
An independent assertion caught inherited `0x1000/0x1001` literal values that
were both below this midpoint. Only the new fixture was corrected; product
arithmetic and the older CPU oracle were unchanged.

Expected arithmetic uses explicit NumPy half rounds and the independent
mathematical nearest-distance FP8 encoder. Packed addresses use the forward
gather equations, independent of the shader's inverse mapping. The oracle checks
every packed byte, not just active output. Generated inputs/expected/actual/report
files remain under ignored `out/` paths. No trained data is used.

GPU tests also cover short spans, oversized layouts, maximum accepted sizes,
missing/wrong/foreign device queues, input/destination mutations after Submit,
repeat readback, moved contexts, dropped handles, foreign jobs, and a queue-gate
timeout followed by successful readback/drain. The test-only binary driver rejects
short/trailing/zero/over-limit fixtures before writing any destination.

The strict shader build has no warnings. The full standalone suite reports
21 passes, three missing-debug-layer skips and zero failures; existing format,
projection, archive and tensor tests continue to pass. Physical lifecycle tests
also pass on RTX. Observed adapter identities:

| Execution | Vendor:device | LUID | Driver |
| --- | --- | --- | --- |
| WARP / Microsoft Basic Render Driver | 1414:008c | 00000000:00012200 | 10.0.22621.3672 |
| RTX 4080 SUPER | 10de:2702 | 00000000:00010318 | 32.0.16.1714 |

## Reproduction and limits

```powershell
& local/amd-nr-re/Build-Foundations.ps1 -Configure
& out/amd-nr-foundations/Release/TRPAmdNrFormatProbe.exe --list-adapters
& C:/Python314/python.exe tests/amd-nr/C512ReductionExecutionOracle.py `
  --exe out/amd-nr-foundations/Release/TRPAmdNrReductionGpuTests.exe `
  --adapter-luid 00000000:00010318 --work-dir out/amd-nr-foundations/c512-reduction-execution-rtx
```

The binary fixture driver is test-only. Padding preservation establishes a
single diagnostic submission; original persistent multi-frame padding contents
remain unresolved. Exact original-runtime comparison, WMMA dot order, other
operator variants, full graph, temporal/noise/input contracts, renderer/game
integration, production timing/memory and representative RDNA2/3/4 acceptance
remain open. Model stays `KnownArchiveIncompleteSchema`, `inference=unavailable`.
