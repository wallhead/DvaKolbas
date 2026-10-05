# Portable C512 projection diagnostic

On 2026-10-05 our C512 CPU reference and source-owned D3D12 shader matched an
independent NumPy oracle for 49,152 activation values / 98,304 complete diagnostic
words per execution path. CPU, WARP and the explicitly selected RTX 4080 SUPER
each had zero mismatches. This establishes our declared serial arithmetic order.
Original WMMA order, model image quality and Radeon acceptance remain unresolved.

## Interface and arithmetic

`EvaluateC512ProjectionReference` consumes a validated C512 layout, canonical
E4M3 input, optional residual, and owned decoded weights. Canonical order is
`[X,Y,channel]`, Y before X. Positive axes remain aligned to 4. The diagnostic
accepts at most 256 pixels / 131,072 activation values. Output has two uint32
words per activation:

- Word 0: final half bits0..15, E4M3 byte bits16..23, bits24..31 zero.
- Word 1: initial half bits0..15, bits16..31 zero.

Initialization multiplies decoded residual by its raw half coefficient, adds
positive zero and rounds to half. Missing residual means positive-zero FP8 codes,
including multiplication by infinite coefficients. Each of sixteen 32-channel
chunks sums FP8 products in ascending K with serial FP32 additions from positive
zero, adds the prior half accumulator and rounds to half. Arithmetic NaNs become
positive half `0x7e00`; final encoding uses the existing nearest-even E4M3 policy.
Finite residual and FP8 products fit FP32 precision.

Initialization and chunk rounding boundaries come from the audited selected
false-view kernel. Serial additions inside a chunk are our portable baseline:
WMMA internal order and intermediate non-finite payloads remain unresolved.
No extracted shader or supplied DLL code is executed, and no model block is enabled.

`C512ProjectionProbe` snapshots input, residual, row-major matrix codes and
little-endian coefficient words before queue submission. It reuses the format
probe's same-device DIRECT queue validation, ownership, fences, drain and
retention path. Input/output buffers and dispatch counts are independently sized.
The private transport factory keeps the root ABI and all four format operations.
Timeouts and moved/dropped handles preserve pending resources; foreign jobs and
invalid counts/weights fail before packaging.

## Validation

Five generated fixtures cover asymmetric channel permutation on 4x4, seeded
finite signed dot products and coefficients on 4x8, a 32-term half-rounding
boundary, early residual rounding, and NaNs/infinite coefficients/signed zeros.
The oracle constructs FP8 mathematically, uses NumPy serial FP32 adds and explicit
half rounds, and obtains expected bytes from the independent nearest-distance
encoder. Every diagnostic word is compared, including unused bits.

FXC folded `product + 0.0f` despite strict IEEE flags and `precise`, leaving
negative zero in 118 initial words of the finite fixture. A minimal zero-times-
negative-coefficient GPU regression failed before correction. The shader now
normalizes exactly zero product bits to positive zero before half conversion,
preserving negative nonzero products that round to negative half zero. The
regression and complete oracles pass after correction.

GPU tests cover missing/wrong device queues, bad counts/weights, independent
readback sizes, post-submit input mutation, repeated readbacks, moved contexts,
foreign jobs, dropped handles, optional residual special values and gated queue
timeout/readback/drain. Existing conversion/retirement tests continue to pass.
The full standalone suite reports 18 passes, two missing-debug-layer skips and
zero failures. Both shaders compile with strict cs_5_1 flags and warnings as errors.

| Execution | Adapter | Vendor:device | LUID | Driver |
| --- | --- | --- | --- | --- |
| WARP | Microsoft Basic Render Driver | 1414:008c | 00000000:00012200 | 10.0.22621.3672 |
| Physical | NVIDIA GeForce RTX 4080 SUPER | 10de:2702 | 00000000:00010318 | 32.0.16.1714 |

The available AMD integrated adapter is not selectable through DXGI here.
Representative RDNA2/3/4 acceptance and debug-layer validation remain open.

## Reproduction and next gates

```powershell
& local/amd-nr-re/Build-Foundations.ps1 -Configure
& C:/Python314/python.exe tests/amd-nr/C512ProjectionExecutionOracle.py `
  --exe out/amd-nr-foundations/Release/TRPAmdNrProjectionReferenceTests.exe `
  --adapter-luid 00000000:00010318 --work-dir out/amd-nr-foundations/c512-projection-rtx
```

Enumerate with `TRPAmdNrFormatProbe.exe --list-adapters` before using a physical
LUID. The bounded oracle driver is a test-only mode of the reference executable;
its fixtures contain only generated inputs/weights. Short, trailing and invalid-
extent files fail before destination writes. Reports/diagnostics remain in `out/`.

Next gates: GPU pooling, WMMA-order comparison, remaining operator variants/full
graph, temporal/noise/input contracts, renderer integration, performance/image
quality and Radeon hardware. Model status remains `KnownArchiveIncompleteSchema`,
`inference=unavailable`.
