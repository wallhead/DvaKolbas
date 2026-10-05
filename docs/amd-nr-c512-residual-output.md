# C512 residual initialization and output encoding

Date: 2026-10-05. Branch: `codex/amd-nr-engine`.

This stage closes two numerical boundaries around the previously recovered
[C512 projection storage](amd-nr-c512-projection.md): initialization of the half
accumulator and conversion of its output to E4M3 bytes. An optional half average
is also identified. These are static contracts and generated numerical probes;
the supplied DLL and embedded GPU kernels were not executed. Product arithmetic
and complete inference remain unavailable.

## Evidence and scope

The private probe rechecks both original SHA-256 identities, the 182 host
registrations, and extracted objects. It compares four projection variants
between gfx1200 and gfx1201; corresponding bodies are byte identical:

| Variant | Clamp / packed FP8 conversion / NaN override sites |
|---|---:|
| `k_conv_res_views<false>` | 9 / 9 / 9 |
| `k_conv_res_views<true>` | 9 / 9 / 9 |
| `k_conv_res2<false>` | 32 / 32 / 32 |
| `k_conv_res2<true>` | 4 / 4 / 4 |

All eight kernel descriptors specify nearest-even rounding for FP32 and
FP16/64, permit input/output denormals, and have FP16 overflow mode bit zero.
No instruction in these four bodies changes those modes. Descriptor entry
offsets are checked against the actual function symbols. AMD's
[LLVM descriptor documentation](https://rocm.docs.amd.com/projects/llvm-project/en/docs-7.2.1/LLVM/llvm/html/AMDGPUUsage.html)
defines the mode fields; the pinned objects establish their values here.

Detailed data flow below is traced in `k_conv_res_views<false>`, ELF entry
`0x2b8000`, body SHA-256
`3a2817c83b980a1399efe1e4d81e6e3b53f23cd2b0c0a2b25224c54634a97c2c`.
Conversion-site counts in other variants corroborate the output policy; they
do not establish all of their spatial coordinates or launch routes.

## Residual initializes the accumulator

The weight pointer is loaded from launch-packet offset `0x28`. The coefficient
bank starts at weight base plus `0x40000` (`0x2b8c0c`); its load and byte
reassembly preserve the stored FP16 bits. Residual bytes are decoded to FP32.
Mixed FP16 FMAs at `0x2b92e8` through `0x2b9328` initialize the packed half
accumulators using the decoded residual, a half coefficient, and additive +0.

For output channel `n`, the recovered boundary is:

```text
h0[n] = roundHalfRNE(decodeE4M3(residual[n]) * halfCoefficient[n] + (+0))
```

This happens before the projection's sixteen 32-channel reduction chunks.
Each chunk contributes a FP32 matrix product and combines it with the previous
half accumulator, followed by another half rounding. Scaling the complete
projection, or adding an unrounded residual product after it, changes results.
The matrix instruction's internal reduction order remains unresolved.

The additive +0 matters: under nearest-even rounding, a negative zero product
added to positive zero produces positive zero. A multiplication-only shortcut
would preserve a different sign. Non-finite mixed-FMA half payload behavior is
not closed by the finite-product probe.

A generated example uses residual `1.125`, half coefficient `1.0009765625`,
and a projection contribution of `-1.125`:

| Calculation | Result |
|---|---:|
| Initial half residual product | 1.1259765625 |
| Contribution added after that boundary | 0.0009765625 |
| Product and contribution combined before one half conversion | 0.0010986328125 |

The residual route also selects between an external blocked pointer at packet
offset `0x08` and a Tin fallback pointer at `0x10`. The non-null test precedes
reuse of the scalar registers: at `0x2b88e0`, registers previously holding
`0x08` are assigned an address derived from `0x10`. Later loads must be traced
through that assignment. The subsequent [tensor-layout audit](amd-nr-c512-tensor-layout.md)
closes its external 16-channel-block and packed Tin coordinates for this selected
variant; game-frame input/output conventions remain open.

## E4M3 output policy

At `0x2b943c` the accumulator half is promoted to FP32. The code records an
ordered self-comparison, clamps finite/infinite values to `[-448,+448]`, converts
with the nearest-even packed FP8 instruction, then overrides unordered inputs
with byte `0x7f`. The first override/store sites are `0x2b9474` / `0x2b94a8`.
The clamp is explicit; it must not be inferred from a hardware overflow flag.
The conversion instruction's fixed nearest-even behavior is documented in the
[AMD RDNA4 ISA](https://www.amd.com/content/dam/amd/en/documents/radeon-tech-docs/instruction-set-architectures/rdna4-instruction-set-architecture.pdf).

The resulting source-owned encoder contract is:

- E4M3 exponent bias 7, subnormal unit `1/512`, finite maximum 448.
- Round to nearest, ties to even, including the zero/subnormal boundary.
- Preserve positive and negative zero when encoding them.
- Saturate values beyond the finite range, including infinities, to `0x7e`
  or `0xfe` by sign.
- Map every NaN sign/payload to positive canonical `0x7f`.

Anchors include `1.0625 -> 0x38`, `1.1875 -> 0x3a`, `2^-10 -> 0x00`, the
next FP32 value above `2^-10 -> 0x01`, `432 -> 0x7e`, and `-0 -> 0x80`.
For a general FP32 software API, deep underflow must also produce signed zero
without an out-of-width integer shift.

## Optional half average

The pre-quantization half accumulator is stored to LDS at `0x2b9460`. A non-null
packet pointer at `0x38` enables a later output path. Four LDS half values at
relative offsets 0, 1024, 4096, and 5120 are combined as:

```text
s0 = roundHalfRNE(a + b)
s1 = roundHalfRNE(c + d)
s2 = roundHalfRNE(s0 + s1)
average = roundHalfRNE(s2 * 0.25)
output = encodeE4M3(halfToFloat(average))
```

The three half adds occur at `0x2b9b28`, `0x2b9b34`, and `0x2b9b48`; the half
quarter multiply occurs at `0x2b9b6c`. The subsequent
[tensor-layout audit](amd-nr-c512-tensor-layout.md#optional-reduction-is-spatially-2x2)
identifies these samples as one non-overlapping 2x2 pixel block and recovers its
packed destination address. Padded destination tails remain untouched, and their
initialization is still open. Averaging previously quantized FP8 bytes, or performing
one FP32 average followed by a single half conversion, is incorrect. Generated
values `[1024,0.5,-1024,0.5]` give `0.125` through these half boundaries and
`0.25` through a single final rounding.

## Generated probes and independent oracle

Our private integer encoder matches an independent nearest-distance oracle
for 166,306 FP32 words: every promoted half encoding, 756 adjacent FP8 midpoint
neighbors across both signs, 14 literal edge words, and 100,000 random words
with seed 20261005. There are zero mismatches. All 256 FP8 codes round-trip,
with negative NaN `0xff` canonicalized to `0x7f`.

The finite residual probe checks 16,125,952 pairs: 254 finite FP8 values times
63,488 finite half values. FP32 product plus +0 followed by half conversion
matches an independent FP64-product route with zero half-bit mismatches.
These products fit within FP32 precision. This result does not validate general
WMMA accumulation or non-finite FMA payloads.

An additional pure host check uses the installed AMD ROCm 7.2 public FP8 header
with our explicit clamp/NaN wrapper. All 126,098 sampled words with zero magnitude
or magnitude at least `2^-24` match; this includes all promoted half inputs and
the midpoint neighbors. The executable imports only `KERNEL32.dll`, initializes
no HIP runtime, and executes no supplied binary code.

The same header disagrees on 1,950 smaller random FP32 inputs outside that
half-output domain. For example, word `0x9a996262` should encode as negative zero
`0x80`, but the host helper returns `0x81`. Its deep-underflow path contains
64-bit shifts with counts 88 and 68 for this input, outside the operand width.
Therefore it is not an authoritative unrestricted FP32 oracle. This is a public
host-helper limitation, not an observed defect in the original GPU converter.

The public header SHA-256 is
`6416f6265d9a7ec11d1cbfe2f3ba9935e8fed47b658f43e4f3149dc5475c9c45`.
The private host check required `-Wno-c++11-narrowing` for unrelated unused
half-raw wrappers in that header; no SDK file or product compiler option was
changed. The product will use our integer implementation and independent tests.

## Reproduction and remaining gates

```powershell
& C:\Python314\python.exe local/amd-nr-re/probe_c512_residual_output.py
```

The ignored script requires the stable local originals, LLVM disassembler,
previous research helpers, and already-built public host oracle. It writes
`local/amd-nr-re/c512-residual-output-contract.json` with mode checks, variant
hashes/counts, numerical totals, and the host-helper limitation. All extracted
objects, instructions, source exports, trained weights and private probes remain
ignored. Only this prose contract is a repository deliverable at this stage.

The portable E4M3 encoder is now implemented. The selected false-view kernel's
spatial coordinates are closed by the tensor-layout audit. Production projection
still requires WMMA numerical comparisons, padding initialization and auditing
the other variants. Attention/FFN, other families, the full graph, temporal/noise/input
contracts, TRP integration, debug validation and representative RDNA2/3/4 tests
remain necessary. Model status stays `KnownArchiveIncompleteSchema` and
`inference=unavailable`.
