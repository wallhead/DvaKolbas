# AMD NR reverse-engineering findings

2026-10-05. These are static findings for a source-owned TRP engine. No AMD inference, game acceptance, output-quality, or performance result is claimed.

## Local inputs

| File | Bytes | SHA-256 |
|---|---:|---|
| `winmm.dll` | 56,677,888 | `195c4a891b6eac4c1cb7671e10ff62bbbe2b17f1dfae1344dc5a6714e4775721` |
| `dlssnr_on_amd_weights.bin` | 147,689,451 | `6bf8dc931ef3ccffe18c82de26ab374156e7f19539ffcf8eabaa25dca5cf15ab` |

The initial paths were in `Stock Game`. During implementation both files were found in `D:\TESV54BETA\BETA_TRUEAE_V54\overwrite\Root`; their hashes match the original analysis. The DLL identifies AMDNR v0.6.0 and embeds HIP code objects; it is a WinMM proxy with 857 exports, not an NGX feature DLL.

## Recovered model structure

The little-endian archive starts with `DLSSNRW1`, count 153 at +8, and absolute payload start 5673 at +12. Directory entries are an 8-bit name length, ASCII name, 64-bit relative payload offset, and 64-bit record length. Every record is unique and contiguous and the payload closes exactly at EOF. Internal packed tensor schemas remain incomplete.

The top-level grouping below is recovered from the orchestrator and allocation metadata. It is an inventory, not a runnable mathematical graph.

| Blocks | Family | Channel group |
|---|---|---:|
| 0 | Pre processing | 32 |
| 1–4 / 5–8 / 9–14 / 15–22 | Encoder Swin groups | 32 / 64 / 128 / 256 |
| 23–30 | Split Swin group and bottleneck entry | 512 |
| 31–38 | ViT | 1024 |
| 39 | Decoder entry | 512 |
| 40–47 | Split Swin decoder | 512 |
| 48–55 / 56–61 / 62–65 / 66–69 | Decoder up/skip and Swin groups | 256 / 128 / 64 / 32 |
| 70 | Post processing | 32 |

Key host RVAs: loader `0x2cfb0`, orchestrator `0x3b090`, allocation `0x41c00`, working extent `0x41a30`, float-to-half conversion `0x4f360`, generic dispatch `0x22af0`, Swin dispatch `0x45de0`, split-Swin dispatch `0x487f0`. The function at `0x43a00` regenerates noise; an earlier exploratory label calling it weight preparation was incorrect.

## GPU registration and parameter contracts

Capstone recovered all 182 HIP registration calls. Their symbols match all 182 ELF function symbols on each of nine embedded targets. AMDGPU MessagePack metadata was decoded directly from ELF notes. Five selected explicit argument contracts agree across all nine targets; this is ABI evidence, not evidence that all targets execute correctly.

The default pre and post variants use **192-byte `VarParams`**, through host tokens `0x8ca48/0x8ca50` and `0x8c960/0x8c968`. The older `_Z21k_pre_block_1h_32_fp89PreParams` and `_Z22k_post_block_1h_32_fp810PostParams` have **80-byte** arguments. The noise variants at `0x8c970/0x8c978` accept a global FP16 pointer, two 32-bit dimensions, and a 32-bit seed. Registration establishes that `0x43a00` launches these noise kernels.

LLVM disassembly covers the older pre/post/repack/head/exposure functions and both registered RDNA4 noise variants. Noise ISA includes a 32-bit integer hash, logarithm/square-root work, and FP16 stores. A complete portable numerical reproduction of these kernels remains to be established.

## Numerical contracts

The FP8 decode table uses E4M3, bias 7. For exponent zero, magnitude is `mantissa / 512`; otherwise it is `(1 + mantissa / 8) * 2^(exponent - 7)`. Codes `0x7f` and `0xff` both map to positive quiet FP16 NaN `0x7e00`. Negative zero is preserved. Finite maximum magnitude is 448. This does not establish the encoder's clipping, tie-breaking, or possible noise contribution.

The host float-to-half integer branches implement round-to-nearest, ties-to-even, including subnormal boundaries, signed zero, overflow, and quieting NaNs. A research translation passed **51,230** comparisons against NumPy's independent conversion, normalizing NumPy's signalling-NaN results to the binary's explicit quieting policy. All **256** FP8 decode entries were generated, with explicit anchor and sign-symmetry checks.

Further planning-stage analysis recovered half-to-float expansion at RVA `0x4f4c0`. It moves NaN sign/payload bits directly and **preserves signalling status** rather than quieting it. An independent-library comparison covered all **65,536** half encodings and matched NumPy exactly, including NaNs. This asymmetry must be retained in CPU and shader storage conversions.

Default extents round up to 64 with a 320 minimum. If both rounded axes are divisible by 256, add 64 to **height**, unless the disabling environment variable is present. Modes 1 and 2 instead round to 8 and 128. Deeper default grids halve with ceiling and round to four; the first grid is half the processing extent.

| Input | Default processing | Deepest grid |
|---|---|---|
| 1280×720 | 1280×832 | 20×16 |
| 1600×900 | 1600×960 | 28×16 |
| 1920×1080 | 1920×1088 | 32×20 |
| 2560×1440 | 2560×1472 | 40×24 |
| 3840×2160 | 3840×2176 | 60×36 |

These results reconstruct specific binary branches for positive valid dimensions; they are not actual runtime captures. A public port uses different geometry, so comparing its output requires accounting for that distinction.

## Reproduction and artifacts

Tools: Ghidra 12.1.3, Capstone, pefile, NumPy, and ROCm LLVM objdump. The original DLL was not loaded or executed, the game was not launched, and the supplied files were not modified.

From the repository, the ignored local probes run as:

```powershell
C:\Python314\python.exe local\amd-nr-re\verify_analysis.py
C:\Python314\python.exe local\amd-nr-re\extract_numeric_contract.py
C:\Python314\python.exe local\amd-nr-re\extract_kernel_metadata.py
C:\Python314\python.exe local\amd-nr-re\map_kernel_registration.py
C:\Python314\python.exe local\amd-nr-re\extract_graph_manifest.py
```

The local Ghidra project, 30 selected host decompilations, archive directory, kernel registration/metadata, top-level graph inventory, numerical constants/probes, and disassembly are in ignored `local/amd-nr-re/`. None of the supplied or extracted binaries, model parameters, or external source files are part of the branch's committed documentation.

Remaining work: close internal schemas and all numerical operations, implement the independent CPU/GPU graph, build the AMD host/session route, compare intermediate and final results, and run actual tests on RDNA2, RDNA3, and RDNA4. The local workstation currently has an RTX 4080 SUPER and AMD integrated graphics; physical acceptance on those three Radeon generations remains outstanding.

See [the source-owned engine design](superpowers/specs/2026-10-05-amd-nr-engine-design.md) for the proposed implementation boundary and validation gates.

## Source-owned foundations implemented

The separate `codex/amd-nr-engine` branch now contains a checked archive snapshot,
exact known-archive identification, processing geometry, integer CPU storage
conversions, model inspector, and build-time-compiled D3D12 format probe.
The known model status is `KnownArchiveIncompleteSchema`; it never enables
inference. These targets are built independently of the plugin.

An independent NumPy oracle matched 51,950 FP32 words, all 65,536 half encodings,
and all 256 E4M3 encodings on CPU, WARP, and the RTX 4080 SUPER with zero storage-bit
mismatches. Timeout and failed-retirement fixtures verify retained GPU ownership.
Windows Graphics Tools is missing: debug validation is explicitly skipped.
The integrated AMD adapter is reported by WMI but absent from DXGI enumeration,
and discrete RDNA2/3/4 acceptance is outstanding. These are format results,
not model inference or quality results. See [reproduction commands and stage
limits](../tests/amd-nr/README.md).

## Further group-dispatch evidence

Read-only Ghidra exports recovered the record-name helper at `0x43c60`, grouped
Swin run assembly at `0x45660`, and C256 chain assembly at `0x45160`. The helper
constructs `block<block>.layer<layer>.layer` and calls the named lookup at
`0x4a4e0`. The run and chain helpers construct different parameter packets;
they cannot be treated as one generic per-block dispatch.

Capstone confirms the relevant token references and copy sizes, and AMDGPU
metadata agrees across all nine embedded targets: C64 run tokens
`0x8c930/0x8c938` use a 392-byte `RunParams`; C256 chain tokens
`0x8ca68/0x8c928` use a 1680-byte `ChainParams`. Local stack allocation sizes
are not the ABI packet lengths. The signed shift table at RVA `0x814c0` contains
`(0,0), (-4,-4), (-4,0), (0,-4)`. Both helpers use these values when constructing
window grids. Axis ordering and full indexing still require recovery; replacing
these signed values with positive offsets would change the grid calculation.
The ignored `verify_network_dispatch.py` and `network-dispatch-contracts.json`
reproduce this cross-check without executing the DLL. This evidence does not
close the internal tensor views or establish runnable grouped operations.

## Foundation review and decisions

A fresh independent reviewer assessed `246d152..26fc428`, found no actionable
Critical/Important/Minor defect, and reran CTest: 10 passed, 1 debug validation
test skipped. The stage is reviewable independently of full NR inference.
The implementation remains on the user's separately requested branch.

Execution decisions, in order:

1. Reuse the selected checkout on `codex/amd-nr-engine`; simultaneous editing
   would compromise isolation, so no other implementer edited the checkout.
2. Continue independent foundations despite the pre-existing Graphics Tools
   failure; GPU debug validation remains unavailable.
3. Explicitly skip the debug fixture and run separate storage/CLI/manual probes;
   this leaves resource/state warnings unobserved.
4. Keep inference, internal schemas, FP8 encoding, history/noise, and TRP
   integration behind the next evidence gates; the full original objective is
   incomplete until those are implemented.
5. Keep discrete RDNA2/3/4 compatibility unvalidated; generation-specific
   defects may remain undiscovered.
6. Preserve the debug-validation gate after review; storage/fence fixtures
   supply narrower evidence and cannot eliminate resource/state risks.
7. Treat the review as verification against recovered contracts, with the
   original DLL unexecuted; a mistaken static contract can still differ from
   the original runtime.
8. Preserve the separate branch without merging or pushing; integration waits
   for a later explicit request.

There are no deferred minor findings. Build commands and the exact scope of
numerical/hardware verification are in `tests/amd-nr/README.md`.

## Packed-region recovery after foundations

The user requested stable project copies of the supplied inputs. They now reside
under ignored `local/amd-nr-re/input/`; both SHA-256 identities match the original
files. RE scripts use these copies instead of paths that move during game setup.

The next static audit recovered physical partitions for the four core records of
C512 blocks 23–30 and 40–47: 64 records, totaling 31,491,072 bytes. These combine
FP8 matrix banks, FP16 residual coefficients and attention bias, and FP32 scale
tails. The loader's record relocation preserves bytes. Treating an entire record
as linear FP16 is therefore incorrect. Exact boundaries, launch-packet fields,
GPU access sites, and the remaining logical-indexing gaps are documented in
[the packed-region report](amd-nr-packed-regions.md).

The audit also distinguishes registered symbols from implemented entry paths:
51 of 182 entries start with `s_trap 2` in each of the gfx10-3/gfx1030 and gfx11
targets, zero in gfx1200/gfx1201, and 158 in gfx9-generic. All eight examined
split-C512 variants start with traps outside gfx1200/gfx1201. Other families have
non-trap bodies on older targets; this is neither a whole-DLL incompatibility
finding nor proof that those alternate routes work. Matching ABI metadata across
targets does not establish functional support.

Fresh checks passed for the original hashes, archive boundaries, 182 Capstone
registration mappings, nine ELF entry audits, eight byte-identical gfx1200/1201
variant bodies, 64 closing physical partitions, and exhaustive coefficient
address coverage. Logical schemas and operation math remain incomplete;
`KnownArchiveIncompleteSchema` and unavailable inference remain correct.

## Canonical C512 projection indexing

The next audit recovered an exact canonical matrix and residual-coefficient
indexing contract for the 32 split-C512 projection records (layers 1 and 3 of
blocks 23–30 and 40–47). All 262,144 matrix coordinates agree between independently
constructed kernel load addresses and a bit-deposit map. AMD's pinned public
matrix calculator checks the underlying wave32 B/D register coordinates.
Every selected record round-trips byte exactly through the canonical view.

An own CPU research prototype passes all 512 non-symmetric basis fixtures with
zero FP16 bit mismatches. A boundary fixture confirms that rounding after each
32 reduction channels gives a different result from one final half conversion.
These fixtures do not establish bit-exact WMMA dot accumulation or a complete
C512 block. Channel numbers are native fragment coordinates; spatial/HWC
compatibility remains open. Exact evidence, limitations and reproduction are in
[the projection report](amd-nr-c512-projection.md). Full inference stays unavailable.

## C512 storage codec implementation

The canonical projection storage contract is now implemented in our C++23 core.
`C512Projection.h/.cpp` decodes exactly 263,168 record bytes into an owned
512×512 FP8 code matrix and 512 raw FP16 coefficient words. It preserves
all encoded bits and uses no supplied executable code. Both result views survive
owner moves; getters on temporaries are disabled. Heap-owned coefficient storage
is necessary to honor the same move-lifetime contract as the matrix bank.

`C512ProjectionArchive.h/.cpp` validates the selection and exact known archive
identity before reading a checked span by record name. The inspector accepts
paired `--projection-block` and `--projection-layer` options and prints only
record, basis and dimension metadata. Unknown archives retain their unsupported
status, including synthetic archives with the right record name and length.

All four address-pattern passes cover all 262,144 matrix coordinates against
an independent forward load oracle. Other generated tests cover the asymmetric
channel permutation, all FP8 codes, special FP16 payloads, truncated/extended
records, ownership, the 32 selection pairs, and CLI validation. The standalone
suite passed 12 tests with no failures; one debug-layer GPU test was skipped.
All 32 supported records of the ignored original archive also passed manual
CLI checks with the exact known digest, names and dimensions.

This completes the [storage codec plan](superpowers/plans/2026-10-05-amd-nr-c512-projection-codec.md).
Projection arithmetic and complete C512 block execution are not implemented.
The model remains `KnownArchiveIncompleteSchema`, `inference=unavailable`;
spatial channel conventions, WMMA dot order, residual application, FP8 output,
other model records, the full network, TRP integration and Radeon acceptance
remain required.

## C512 residual and output numerical boundaries

The next static audit establishes that a decoded residual FP8 value times its
FP16 coefficient initializes the half accumulator with additive +0 and an early
half rounding. It precedes the sixteen projection reduction chunks. Output half
values are promoted, clamped to [-448,+448], converted to nearest-even E4M3,
and explicitly canonicalized to `0x7f` for NaNs. An optional output uses three
half adds and a half quarter multiply on four pre-quantization samples.

Four projection variants have matching conversion/clamp/NaN-override counts and
byte-identical corresponding gfx1200/1201 bodies. All eight descriptors have
nearest-even rounding and input/output denormals enabled. Our encoder matches
an independent distance oracle on 166,306 generated FP32 words with zero
mismatches. All 16,125,952 finite FP8/half residual pairs also match an independent
FP64-product route at the half boundary. This does not establish WMMA dot order.

AMD's public host FP8 helper agrees on all 126,098 sampled words within the
promoted-half output range. It disagrees on 1,950 deeper-underflow random FP32
inputs; its out-of-width shifts make it unsuitable as an unrestricted oracle.
Our independent encoder/oracle agree on those inputs as well. Exact evidence,
fixtures and limitations are in [the residual/output report](amd-nr-c512-residual-output.md).

No supplied executable was run and no product arithmetic was enabled. A portable
source-owned E4M3 encoder is the next bounded implementation. Spatial mappings,
WMMA accumulation, complete blocks/network, integration and Radeon acceptance
remain outstanding; inference is still unavailable.

## Portable E4M3 encoding implementation

The recovered output policy is implemented in our C++ numerical core and
portable byte-addressed HLSL shader. Both standalone tools accept
`--format f32-to-e4m3`, preserving the four-byte storage-word convention and
explicit GPU selection. Quantization uses bounded integer shifts, handles deep
underflow correctly and has no HIP/native FP8 dependency.

An independent nearest-distance oracle matches CPU, WARP and RTX 4080 SUPER on
166,314 words, with zero bit mismatches: all promoted half encodings, every FP8
midpoint neighbor across both signs, explicit special/underflow words, and a
100,000-word random fixture. All previous format modes match their complete
independent fixtures. The standalone suite has 12 passes, one missing-debug-layer
skip and zero failures. Exact scope is in [the test README](../tests/amd-nr/README.md).

The whole-branch independent review of `246d152..c4dddd9` found no Critical or
Important issue. One deferred Minor notes that earlier README/design paragraphs
still describe encoding as pending; the implemented APIs and current encoder
results above supersede those historical status statements.

This closes product storage encoding. Residual FMA execution, WMMA equivalence,
spatial layouts and complete inference remain pending. No supplied executable
code is loaded by the product.

## Additional user-provided OptiScaler reference

The [pinned reference audit](amd-nr-optiscaler-reference.md) examines the supplied
OptiScaler repository at `f0c0232a2384f1fc9ee0167dcabf2dbe34fd7708`. It supplies
host integration source and an external-runtime API, not the complete neural
runtime implementation. Its device policy rejects RDNA1/2, and its RDNA3 notice
describes FP8 work on FP16 matrix units. These findings support retaining our
portable baseline and investigating optimized gfx11 math after numerical
closure. Its full-network flag and producer/consumer lifecycle provide useful
comparison and integration conditions. No external source was copied into the
product and no runtime package was executed.

## C512 tensor spatial mapping and reduction

The selected false-view projection now has a recovered internal spatial contract.
A workgroup processes one 4x4 tile in Y-first order. The canonical matrix channels
connect to both packed Tin storage and the external view with 16-channel blocks.
Forward gather/output tracing and AMD's pinned A-operand coordinate calculator
agree on every tile byte. Complete coverage passes for five aligned extents,
totaling 1,540,096 bytes, with no duplicate or missing addresses.

The optional half reduction uses one non-overlapping 2x2 pixel block. Its four
source half coordinates and packed destination addresses pass exhaustive generated
fixtures. Padded tails are not written: the 60x36 to 32x20 route leaves 51,200
bytes untouched beyond the active 30x18 region. Padding initialization remains
a separate dependency to recover.

An asymmetric dyadic projection fixture passes all 8,192 pixel/channel values
through packed gathering, CPU research arithmetic, encoding and forward storage
with zero mismatches. The fresh audit checks both original identities, 70 GPU
instruction sites, 21 host sites and gfx1200/1201 body equality. It executes no
supplied runtime and adds no product arithmetic. Formulas, reproduction and limits
are in [the tensor-layout report](amd-nr-c512-tensor-layout.md).

This closes selected internal spatial addressing. General WMMA equivalence,
other variants, padding initialization, complete graph/game-frame conventions,
renderer integration and representative RDNA2/3/4 acceptance remain open.
`KnownArchiveIncompleteSchema` and `inference=unavailable` remain accurate.
