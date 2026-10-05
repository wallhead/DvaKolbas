# C512 tensor coordinates and optional 2x2 reduction

Date: 2026-10-05. Branch: `codex/amd-nr-engine`.

This audit connects the canonical channels in the [projection record codec](amd-nr-c512-projection.md)
to spatial pixels in the selected `k_conv_res_views<false>` kernel. It recovers
packed tensor addresses, the external view with 16-channel blocks, and the four
pixels used by the optional half reduction. The supplied runtime was not executed.
This is a static contract, not a completed neural inference implementation.

## Evidence and domain

The selected registration token is RVA `0x8cc30`; its gfx1200 ELF entry is
`0x2b8000`, with body SHA-256
`3a2817c83b980a1399efe1e4d81e6e3b53f23cd2b0c0a2b25224c54634a97c2c`.
The corresponding gfx1201 body is byte identical. The private probe checks both
supplied file hashes, the host registration, fresh disassembly sites and host
packet writes before checking coordinates. Other projection variants have not
received this spatial audit.

The formulas below cover positive width `W` and height `H`, both divisible by 4,
512 channels, and a 256x1x1 workgroup of eight wave32 waves. The audited host route
sets those workgroup dimensions and a one-dimensional tile grid. No claim is made
for unaligned extents, other workgroup dimensions, integer overflow at arbitrary
sizes, or game-frame channel semantics.

The 72-byte explicit packet has these roles in the selected kernel:

| Offset | Role |
|---|---|
| `0x00` | Packed projection input |
| `0x08` | Optional external residual view, channels blocked by 16 |
| `0x10` | Packed residual fallback |
| `0x18` | Packed output |
| `0x20` | Optional external output view, channels blocked by 16 |
| `0x28` | Projection record pointer |
| `0x30`, `0x34` | Width and height, 32-bit integers |
| `0x38` | Optional packed reduced output |
| `0x40`, `0x44` | Reduced-output width and height |

The non-null residual test occurs before scalar registers are reused for the
fallback address. Therefore the later value of the register originally holding
packet `0x08` cannot be interpreted as that pointer. A non-null external view uses
its blocked strides; otherwise the packed fallback is used when present.

The load at packet-relative `0x54`, masked to its low 16 bits, supplies the gather
loop stride. This agrees with the implicit argument starting after 72 bytes:
LLVM's [hidden argument layout](https://www.llvm.org/doxygen/AMDGPUPreloadKernelArguments_8cpp_source.html)
places the two-byte group-size X field at implicit offset 12. The host's packed
dimension value `0x100000100`, followed by 32-bit 1, establishes 256x1x1 here.
This identification combines public ABI information with the supplied object's
address use; the public source does not establish the object's layout by itself.

## Pixel and packed tensor mapping

Each workgroup processes one 4x4 tile, containing 8,192 encoded bytes. Let `b`
be the one-dimensional tile index and `i` the WMMA row in `[0,16)`:

```text
tileX = b / (H/4)
tileY = b % (H/4)
x = 4*tileX + i/4
y = 4*tileY + i%4
```

Division is integer division. Tile order and within-tile pixel order both advance
Y first. This is an internal tensor coordinate basis; the path from game inputs
to these coordinates remains to be recovered.

For logical channel `n` in `[0,512)`, `deposit(v, positions)` places logical bit
`j` into physical bit `positions[j]`. The packed tensor, called Tin in prior
reports, has:

```text
channel positions = (0, 1, 4, 5, 3, 9, 10, 11, 12)
pixel positions   = (6, 7, 8, 2)

i = 4*(x%4) + y%4
b = (x/4)*(H/4) + y/4
packedOffset(x,y,n) = 8192*b
                    + (deposit(n, channel positions) | deposit(i, pixel positions))
```

These disjoint sets partition all 13 bits in the tile. The total byte count is
`W*H*512`. The external blocked view uses the same logical channel numbers:

```text
externalOffset(x,y,n) = 16*((n/16*W + x)*H + y) + n%16
```

Its order is channel block, X, Y, then channel within the block. This closes the
connection between the selected kernel's canonical matrix/coefficients and
its two tensor storage layouts. It does not identify the complete model's
image/input/output convention.

## Independent gather and WMMA operand check

The normal gather uses `q = thread + 256*j`, with `thread` in `[0,256)` and
`j` in `[0,32)`. It loads one source byte and stores it at LDS byte `16384+q`.
Forward simulation of its integer address definitions, including its signed
`-508` correction, agrees with:

```text
i = q/512
k = q%512
sourceByteInTile = deposit(k, channel positions) | deposit(i, pixel positions)
```

There is also a four-byte gather prologue gated by group-size X equal to 1.
The audited 256-thread route skips it. Treating those prologue stores as active
for this route would incorrectly suggest overlapping writes.

AMD's [pinned matrix calculator](https://github.com/ROCm/amd_matrix_instruction_calculator/tree/2ef91896bcdc4d26624f952e5c905c787cd9bc9e)
independently locates `A[i,k]` for a wave32 FP8 16x16x16 WMMA instruction:
lane `i+16*(k/8)`, VGPR `(k/4)%2`, byte `k%4`, for local `k` in `[0,16)`.
Every one of these 256 coordinates is checked. Applying the kernel's two LDS
load pairs across all sixteen 32-channel chunks reaches exactly `i*512+k`
relative to LDS byte 16384, for every full input coordinate.

The output traversal visits group `g = thread/32 + 8*j` while `g<32`.
Its channel is `n = 16*g + thread%16`; its eight accumulator registers represent
rows `i = r + 8*((thread/16)%2)`, for `r` in `[0,8)`. Independent forward output
addresses agree with both packed and external formulas. Each of the 8,192
pixel/channel coordinates is stored exactly once. Before FP8 encoding, the
half accumulator is stored to LDS at half index `512*i+n`.

## Optional reduction is spatially 2x2

The numerical sequence is the [previously recovered half average](amd-nr-c512-residual-output.md#optional-half-average):
round two pair sums to half, round their sum to half, then multiply by one quarter
and round to half before E4M3 encoding. Its input is the pre-quantization half
accumulator, not the encoded main output.

For reduction index `u` in `[0,2048)`, let `n = u%512` and `p = u/512`.
The first source row is `i0 = 8*(p/2) + 2*(p%2)`. LDS half indices are:

```text
512*i0 + n + (0, 512, 2048, 2560)
```

The four samples are `(x,y)`, `(x,y+1)`, `(x+1,y)`, `(x+1,y+1)` for one
non-overlapping 2x2 block. They map to reduced coordinates:

```text
x2 = 2*tileX + p/2
y2 = 2*tileY + p%2
```

The destination uses the same packed layout, with reduced dimensions `W2,H2`,
and a bounds mask `x2<W2 && y2<H2`. For the tested aligned reduced extents, its
tile stride is `H2/4`. The kernel computes a ceiling tile stride for H2; this
audit does not establish a general unaligned destination allocation contract.

Only pixels derived from the source are written. In particular, 60x36 source
into a padded 32x20 destination writes a 30x18 region:

| Source | Destination allocation | Written bytes | Padding bytes untouched |
|---|---|---:|---:|
| 4x4 | 4x4 | 2,048 | 6,144 |
| 8x12 | 4x8 | 12,288 | 4,096 |
| 60x36 | 32x20 | 276,480 | 51,200 |

The subsequent allocator audit establishes zero initialization at allocation;
persistent frame lifetime remains open. An implementation must not assume this
kernel writes or clears the padded tail.

### Allocator initialization follow-up

Fresh Capstone checks fifteen allocator/producer instruction sites and resolves
both delayed HIP import thunks from the supplied PE's delay-import directory.
At RVA `0x42c88` the allocator reads the tile count from manager offset `0x41c`;
it multiplies by 8,192 bytes, calls `hipMalloc` at `0x42c9b`, then `hipMemset`
with value zero and the same size at `0x42caa`. The resulting pointer is stored
at manager offset `0x340` at `0x42ce3`.

In the orchestrator, RVA `0x3c9c3` selects block 30; `0x3c9c8` reads that same
pointer, and `0x3c9f6` supplies it as the sixth argument to the C512 runner called
at `0x3ca0c`. Combined with the recovered runner packet, this connects the cleared
allocation to the optional reduction output. The cached geometry/allocator
exports corroborate its tile count and allocation route.

This establishes constructor initialization and selected producer routing, not
all writers or padding contents across every alternative execution route and
frame lifecycle. Our CPU primitive therefore preserves padding and leaves its
initialization to the caller. A caller can initialize fresh encoded storage with
zero bytes to represent positive E4M3 zero.

```powershell
& C:\Python314\python.exe local/amd-nr-re/probe_c512_padding_initialization.py
```

The ignored report is `local/amd-nr-re/c512-padding-initialization-contract.json`.

## Verification and remaining work

The fresh probe passes 70 exact GPU instruction sites and 21 host instruction
sites. Complete packed/external coverage is checked for 4x4, 8x12, 12x8, 32x20
and 60x36 extents: 1,540,096 byte coordinates in total, each visited once.
The three reduction fixtures check every source half coordinate and destination
address, reporting the untouched tails explicitly.

A generated asymmetric projection maps input channel `k` to output
`(37*k+11)%512` with coefficient 1. Distinct dyadic values depend on both pixel
and channel. All 8,192 values pass packed gathering, the CPU research projection,
E4M3 encoding and forward physical output storage with zero mismatches.
These exact dyadic fixtures do not establish general WMMA accumulation order
or numerical equivalence on trained activations.

```powershell
& C:\Python314\python.exe local/amd-nr-re/probe_c512_tensor_layout.py
```

The ignored report is `local/amd-nr-re/c512-tensor-layout-contract.json`.
Original binaries, trained data, extracted code and private probes remain ignored;
this prose contains the independently derived storage contract.

Checked C++ layout conversion and a CPU half-reduction reference are now
implemented; [standalone tests](../tests/amd-nr/README.md#c512-tensor-primitives)
cover generated storage and independent numerical fixtures. Next gates are
persistent padding lifetime, other projection variants and a portable projection
implementation with justified numerical comparisons. Attention/FFN,
other tensor families, complete graph/input/output contracts, renderer integration
and RDNA2/3/4 validation remain necessary. Product model status stays
`KnownArchiveIncompleteSchema`, `inference=unavailable`.
