# Split-C512 projection: canonical indexing and CPU research probe

Date: 2026-10-05. Branch: `codex/amd-nr-engine`.

This stage recovers a canonical matrix view for `block23`–`block30` and
`block40`–`block47`, layers 1 and 3: 32 projection records. Each record contains
262,144 FP8 bytes and 512 FP16 residual coefficients, totaling 263,168 bytes.
It extends [physical-region recovery](amd-nr-packed-regions.md). Other records
and full inference remain incomplete.

## Coordinate basis and storage contract

Define `W[n,k]`, with output channel `n` and reduction channel `k` in `[0,512)`.
These channel numbers use native WMMA fragment coordinates. They are **not yet
the model's spatial/HWC channel convention**. In particular, a permutation of
the low channel bits cannot be dismissed as an equivalent image layout until
the input and output codecs are traced.

`deposit(value, positions)` places bit `i` of `value` into physical bit
`positions[i]`, with positions listed from the least significant logical bit.
The physical matrix byte offset is:

```text
output positions = (6, 3, 7, 8, 9, 10, 11, 12, 13)
input positions  = (0, 1, 4, 5, 2, 14, 15, 16, 17)
offset(n,k) = deposit(n, output positions) | deposit(k, input positions)
```

These disjoint bit sets partition all 18 address bits. Exhaustive enumeration
maps all 512×512 coordinates bijectively onto bytes `[0,262144)`.

The residual coefficient for canonical output `n` uses FP16 half index:

```text
coefficient positions = (0, 3, 1, 2, 4, 5, 6, 7, 8)
halfIndex(n) = deposit(n, coefficient positions)
byteOffset(n) = 0x40000 + 2 * halfIndex(n)
```

Half words are little endian. Decoding the storage view must preserve all FP8
codes and FP16 bits, including signed zeros and NaN payloads. It does not apply
the residual coefficient or perform an arithmetic operation.

## Independent address derivation

The examined projection-view kernel is registered at token RVA `0x8cc30`.
Its selected gfx1200 body starts at ELF virtual address `0x2b8000` and has
SHA-256 `3a2817c83b980a1399efe1e4d81e6e3b53f23cd2b0c0a2b25224c54634a97c2c`.
The corresponding gfx1201 body is byte identical. The exact supplied DLL hash
and byte-preserving weight upload are documented in the earlier reports.

The external register-coordinate check uses AMD's
[matrix instruction calculator, pinned at 2ef91896](https://github.com/ROCm/amd_matrix_instruction_calculator/tree/2ef91896bcdc4d26624f952e5c905c787cd9bc9e).
For `v_wmma_f32_16x16x16_fp8_fp8`, wave32, the B element `B[k,j]` is in lane
`16*floor(k/8)+j`, VGPR `floor(k/4)%2`, byte `k%4`. D element `D[i,j]` is in
lane `16*floor(i/8)+j`, VGPR `i%8`. All 256 B coordinates and 256 D coordinates
were checked against that calculator. The public calculator is a local research
dependency; its source and dependencies are not added to the product.

An independent forward simulation of the kernel's load-address definitions
combines those B coordinates with the two 64-bit loads per thread. Let
`(L,r,b)` be the lane, VGPR, and byte for `B[k%16,n%16]`. Then:

```text
v2   = L >> 1
v6   = L & 1
v25  = v2 & 8
v31  = (L << 2) & 8
v2   = (v2 & 6) | v6
root = ((v2 << 6) | (v25 << 2)) + v31
offset = root + floor(n/16)*512 + floor(k/32)*0x4000
              + r*16 + (floor(k/16)%2)*4 + b
```

This forward formula agrees with the separate bit-deposit formula at every
coordinate. It does not derive its expected answers from the deposit map.

For residual coefficients, thread `t` in `[0,256)` visits output tile `g`,
starting at `t>>5` and advancing by 8 while `g<32`. With
`n = 16*g | (t&15)`, the observed byte offset within the coefficient tail is:

```text
((((n & 0x1f1) | ((t << 2) & 8)) << 1) | (t & 12))
```

All 1,024 thread/tile reads agree with `2*halfIndex(n)`. They cover all 512
coefficients exactly twice, matching the replicated wave lanes.

Selected address and arithmetic sites, without committing extracted code:

| ELF address | Role |
|---|---|
| `0x2b800c` | Weight pointer comes from launch packet offset `0x28` |
| `0x2b86f8`–`0x2b885c` | Lane-bit rearrangement and weight-address root |
| `0x2b8bf0` | Output tile stride of 512 bytes |
| `0x2b9338`, `0x2b9350` | Reduction chunk address and `0x4000` advance |
| `0x2b9360`, `0x2b936c` | 64-bit loads at relative offsets 0 and 16 |
| `0x2b9394`, `0x2b93ac` | Low/high load words enter the two B fragments |
| `0x2b93bc`, `0x2b93c8` | Two consecutive 16-term FP8 WMMA operations |
| `0x2b93dc`, `0x2b9420` | Add previous half accumulator and store rounded halves |

## Arithmetic boundary and research prototype

Each reduction step consumes 32 channels: two FP8 WMMA instructions accumulate
into FP32, then the result is combined with the prior FP16 accumulator and
rounded back to FP16. There are 16 such steps for 512 channels. Rounding only
once after all 512 terms changes the result.

The source-owned, ignored CPU probe implements this boundary with NumPy FP32
matrix products and FP16 storage. It accepts canonical FP8 matrix/input codes
and an explicit initial FP16 accumulator. It does not infer the spatial residual
source or automatically multiply the residual coefficient. It stops before
final FP8 encoding.

NumPy dot is **not a bit-exact emulation of AMD WMMA's internal accumulation**.
The exact arithmetic assertions below use synthetic dyadic inputs whose relevant
FP32 products and sums are exact. Real activation tolerances and WMMA reduction
order remain unestablished; no production projection arithmetic is enabled.

The fresh probe reports:

- All 262,144 matrix addresses agree between forward tracing and bit deposition.
- All 32 real projection records round-trip through the canonical view to their
  original matrix bytes and coefficient bits, with no mismatch.
- A non-symmetric synthetic matrix maps input channel `k` to output
  `(37*k+11)%512`. All 512 basis inputs produce exact expected FP16 output bits,
  with zero mismatches. This catches channel-bit swaps that an identity matrix
  would miss.
- A zero matrix preserves an explicit initial accumulator of FP16 `-0.5`.
- A rounding fixture uses input code `0x01` in all channels and a matrix row with
  first code `0x7e`, remaining codes `0x01`. Per-32-channel half rounding gives
  `0.875`; rounding the full sum once gives `0.876953125`.

## Reproduction and scope

```powershell
& C:\Python314\python.exe local/amd-nr-re/verify_packed_regions.py
& C:\Python314\python.exe local/amd-nr-re/probe_c512_projection.py
```

The probe checks the copied original inputs' hashes, exact selected instruction
sites and gfx1200/1201 body equality. It writes the ignored report
`local/amd-nr-re/c512-projection-contract.json`. The original DLL and its GPU
kernels are not executed. Only our probe and AMD's reviewed public coordinate
calculator run.

The recovered codec can now become an owned, bounded C++ logical view, tested
with generated data. Remaining gates include projection input/output channel
and spatial mappings, residual application, WMMA dot order, FP8 output encoding,
attention/FFN math, other record families, whole graph, TRP integration, and real
RDNA2/3/4 validation. `KnownArchiveIncompleteSchema` and `inference=unavailable`
remain the correct product status.
