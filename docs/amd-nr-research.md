# AMD NR reverse-engineering findings

2026-10-05. These are static findings for a source-owned TRP engine. No AMD inference, game acceptance, output-quality, or performance result is claimed.

## Local inputs

| File | Bytes | SHA-256 |
|---|---:|---|
| `winmm.dll` | 56,677,888 | `195c4a891b6eac4c1cb7671e10ff62bbbe2b17f1dfae1344dc5a6714e4775721` |
| `dlssnr_on_amd_weights.bin` | 147,689,451 | `6bf8dc931ef3ccffe18c82de26ab374156e7f19539ffcf8eabaa25dca5cf15ab` |

Both remain in the user-supplied `D:\TESV54BETA\BETA_TRUEAE_V54\Stock Game` directory. The DLL identifies AMDNR v0.6.0 and embeds HIP code objects; it is a WinMM proxy with 857 exports, not an NGX feature DLL.

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

The local Ghidra project, 25 selected host decompilations, archive directory, kernel registration/metadata, top-level graph inventory, numerical constants/probes, and disassembly are in ignored `local/amd-nr-re/`. None of the supplied or extracted binaries, model parameters, or external source files are part of the branch's committed documentation.

Remaining work: close internal schemas and all numerical operations, implement the independent CPU/GPU graph, build the AMD host/session route, compare intermediate and final results, and run actual tests on RDNA2, RDNA3, and RDNA4. The local workstation currently has an RTX 4080 SUPER and AMD integrated graphics; physical acceptance on those three Radeon generations remains outstanding.

See [the source-owned engine design](superpowers/specs/2026-10-05-amd-nr-engine-design.md) for the proposed implementation boundary and validation gates.
