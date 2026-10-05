# AMD NR packed-region recovery

Date: 2026-10-05. Branch: `codex/amd-nr-engine`.

This stage recovers physical storage boundaries for the four core records of
the C512 split blocks. It does **not** establish logical matrix permutations,
complete block math, runnable inference, or Radeon compatibility.

## Stable local inputs

At the user's request, the supplied files are copied into the ignored directory
`local/amd-nr-re/input/`. Both copies match the original identities:

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| `winmm.dll` | 56,677,888 | `195c4a891b6eac4c1cb7671e10ff62bbbe2b17f1dfae1344dc5a6714e4775721` |
| `dlssnr_on_amd_weights.bin` | 147,689,451 | `6bf8dc931ef3ccffe18c82de26ab374156e7f19539ffcf8eabaa25dca5cf15ab` |

The originals moved between `Stock Game` and `overwrite/Root` during research.
Ghidra's original-byte cache also preserved the exact DLL, independently checked
against its SHA-256. Local RE scripts now use the stable copies. Neither the DLL
nor any extracted GPU object was executed. The copies, parameter data,
decompilation, and disassembly remain excluded from Git.

## Upload semantics

The loader at host RVA `0x2cfb0` relocates whole records into an aligned host arena.
At `0x2d650–0x2d66a`, Capstone confirms that the destination is the arena plus the
new offset, the source is the old payload plus the record's old offset, and the
copy length is the record's stored byte length. It then replaces the offset.
Ghidra's export of the called routine at `0x7e850` shows a byte-copy implementation,
including scalar and vector copy paths, with no numeric conversion. The loader
uploads the resulting arena through `hipMemcpy`.

Consequently, this relocation does not unpack archive matrices from linear FP16
into FP8. Record lengths alone cannot establish a half-precision tensor format.
The matrix encodings below come from GPU reads and their arithmetic consumers.
The private arena alignment is not a logical model-format requirement.

## C512 physical partitions

Offsets are relative to the start of the named record. Ends are exclusive.
The four partitions apply to blocks **23–30 and 40–47**. Each of the 64 record
lengths was reread from the copied archive; every partition closes exactly at its
record boundary, with no gaps or overlap. Together they cover 31,491,072 bytes.
Block 30's additional `layer4` record is excluded and remains unresolved.

| Record suffix | Start | End | Bytes | Observed storage / role |
| --- | ---: | ---: | ---: | --- |
| `layer0.layer` | `0x00000` | `0x40000` | 262,144 | FP8 matrix bank before the grouped feed-forward stages |
| `layer0.layer` | `0x40000` | `0x60000` | 131,072 | FP8 expansion bank; group stride `0x4000` |
| `layer0.layer` | `0x60000` | `0x80000` | 131,072 | FP8 contraction bank; group stride `0x4000` |
| `layer1.layer`, `layer3.layer` | `0x00000` | `0x40000` | 262,144 | FP8 projection matrix bank |
| `layer1.layer`, `layer3.layer` | `0x40000` | `0x40400` | 1,024 | FP16 residual coefficient bank: 512 half words |
| `layer2.layer` | `0x00000` | `0xc0000` | 786,432 | FP8 QKV matrix bank |
| `layer2.layer` | `0xc0000` | `0xe0000` | 131,072 | FP16 attention-bias bank; selector stride `0x2000` |
| `layer2.layer` | `0xe0000` | `0xe0040` | 64 | FP32 attention-scale bank: 16 words |

These are packed banks, not row-major tensors. Their byte counts and use do not
yet prove a complete mapping from each byte to a logical row, column, head,
window position, or Q/K/V component. No decoder is enabled by this document.

## Host-to-kernel binding

Ghidra's split dispatcher at `0x487f0` calls the record-name helper at `0x43c60`.
The helper constructs the block/layer name and looks up the uploaded record.
The following weight-pointer fields can be followed from those return values
into the launch packets and scalar GPU loads:

| Kernel family | Core record | Explicit packet bytes | Weight-pointer byte offset |
| --- | --- | ---: | ---: |
| `k_ffwd2` / `Ffwd2Params` | `layer0` | 48 | `0x18` |
| `k_conv_res_views` / `ConvPlParams` | `layer1` or `layer3` | 72 | `0x28` |
| `k_conv_res2` / `Conv2Params` | `layer1` | 64 | `0x28` |
| `k_qkv_attn2` / `AttnParams` | `layer2` | 40 | `0x10` |

Both boolean variants of each family were checked. AMDGPU metadata agrees on
each explicit packet size across all nine embedded targets. Compiler/runtime
hidden arguments are separate; loads beyond the explicit packet must not be
misidentified as model fields.

For all eight selected variants, the complete function bodies in `gfx1200` and
`gfx1201` are byte-identical. The fixed bank-boundary observations agree between
the variants. The richer register-source tracing below is for the `Lb0` bodies;
it is not a claim of identical arithmetic across variants or architectures.

## Arithmetic and indexing evidence

The first feed-forward matrix's pointer can be followed from the scalar root
through vector address construction into the loads at `0x2b68e0/0x2b68ec` and
their FP8 WMMA consumers. Subsequent address construction adds `0x40000` at
`0x2b6b18` and `0x60000` at `0x2b772c`; each grouped root includes the group
selector shifted by 14. These observations distinguish the three FP8 banks.

For the projection-view kernel, `0x2b8c0c` adds `0x40000` to the record pointer;
`0x2b8c94` reads a half word from the resulting residual bank. Its byte-address
expression, recovered through register definitions rather than inferred from
record size, is:

```text
offset(t,g) = ((((16*g | (t&15)) & 0x1f1) | ((t<<2)&8)) << 1) | (t&12)
t = 0..255
g starts at t>>5 and advances by 8 while g < 32
```

Exhaustive enumeration produces all 512 even offsets in `[0,1024)`, each read
twice. The register-source audit includes the definition of the `t<<2` term;
checking only range coverage would fail to distinguish several wrong
permutations. This is a physical-address contract. Mapping these reads to
logical output channels still requires following the accumulator and store
layout.

For QKV attention, the scale load at `0x2c6ddc` uses record offset `0xe0000` plus
four times the selector. At `0x2c6e3c`, the `Lb0` body rounds the loaded FP32
scale to FP16 before half-precision products such as `0x2c7058`. Keeping that
scale and all products in FP32 would change the operation's rounding boundary.
The bias root adds `selector<<13` at `0x2c75c8`, then `0xc0000` at `0x2c75d4`;
half-word bias reads start at `0x2c7624`. Full within-bank bias and matrix
permutations remain open.

## Target entry audit

All 182 registered host tokens were freshly cross-checked using Capstone against
the DLL's registration calls and strings, then matched to ELF function symbols.
The following counts describe functions whose **first instruction is `s_trap 2`**:

| Embedded target | Registered functions | Entries starting with a trap |
| --- | ---: | ---: |
| `gfx10-3-generic` | 182 | 51 |
| `gfx1030` | 182 | 51 |
| `gfx11-generic` | 182 | 51 |
| `gfx1100` | 182 | 51 |
| `gfx1101` | 182 | 51 |
| `gfx1102` | 182 | 51 |
| `gfx1200` | 182 | 0 |
| `gfx1201` | 182 | 0 |
| `gfx9-generic` | 182 | 158 |

All eight selected split-C512 variants start with traps in the seven targets
other than `gfx1200/gfx1201`. Other registered families have non-trap bodies on
older targets. Therefore this finding does not establish that the whole DLL is
RDNA4-only, nor that older-target routes work. Host route selection and their
math still require recovery. Symbol registration, matching argument metadata,
and a non-trap entry are each insufficient to prove functional GPU support.

The source-owned D3D12 baseline remains the chosen route for the requested
RDNA2/RDNA3/RDNA4 coverage; it cannot depend on these RDNA4 FP8 WMMA bodies being
available on every generation.

## Reproduction and remaining gate

The ignored local audit writes `packed-region-contracts.json`. It independently
checks the copied inputs' hashes, parses the archive directory again, verifies
Capstone registration sites, reads all nine ELF symbol tables, compares the eight
selected function bodies, regenerates their disassembly, checks the boundary
observations, verifies packet metadata, and exhaustively checks residual
coefficient addresses.

```powershell
& C:\Python314\python.exe local/amd-nr-re/extract_kernel_metadata.py
& C:\Python314\python.exe local/amd-nr-re/verify_packed_regions.py
& .\out\amd-nr-foundations\Release\TRPAmdNrInspect.exe `
  --weights .\local\amd-nr-re\input\dlssnr_on_amd_weights.bin `
  --width 1920 --height 1080
```

These commands passed against the stable copies. The inspector reports 153
records, the expected identity and geometry, `KnownArchiveIncompleteSchema`, and
`inference=unavailable`.

The subsequent [projection audit](amd-nr-c512-projection.md) closes canonical
matrix and residual-coefficient indexing for layers 1 and 3, and verifies the
per-32-channel FP16 storage boundary on synthetic fixtures. It does not establish
the spatial/HWC convention, WMMA internal dot order, final FP8 encoding or a full
C512 CPU block. The rest of the C512 operation order and activation rules still
require recovery before a complete block can be implemented and compared.
The remaining record families, whole graph, temporal/input/output contracts,
GPU execution, TRP integration, and real RDNA2/3/4 acceptance remain unfinished.
