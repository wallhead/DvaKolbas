# AIO18 FSR reverse engineering — revision 3

Start with **AIO18_FSR_RE.md**, sections **22–27**. The report is cumulative: v1 and v2 remain, with the old SR-payload table explicitly corrected. **RUNTIME_BREAKPOINTS_v3.md** contains proposed live experiments, not recorded game tests.

## New evidence

Revision 3 adds **49 disassembly excerpts**, **6,107 excerpt instruction lines**, and **183 selected instruction records**. The cumulative package has **128 excerpts and 325 instruction records at 316 distinct addresses**. Some excerpts overlap earlier ranges because new producer/consumer relationships needed fuller context; these are not 128 distinct functions.

New findings cover the normal UI viewport resize, depth-SRV substitution, an unbalanced GetResource reference, six-stage sampler substitution, camera jitter signs, prepared motion/depth resources, depth-convention recreation, distinct SR/FG reset logic, and corrected FSR mask/output fields.

**No original binaries, shader bytecode, PDBs, game executable, or font files are redistributed. No Windows DLL, game, or GPU workload was executed.** PASS means evidence reproducibility and reference-model checks, not a runtime compatibility result.

## Reproduce

Python standard library; system libarchive for extraction; GNU objdump and LLVM llvm-objdump for verification. Linux/WSL:

```bash
mkdir -p work
python scripts/extract_archive.py /path/to/SkyrimUpscalerAIOBuild18-Hotfix1.7z work/extracted > work/manifest.json
python scripts/verify_v3.py work/extracted \
  --archive /path/to/SkyrimUpscalerAIOBuild18-Hotfix1.7z \
  --llvm-objdump /path/to/llvm-objdump \
  --output work/verification_v3.json
sha256sum -c SHA256SUMS.txt
```

Use a fresh extraction directory. The archive filename is not identity: verification checks size/hash and the 25 extracted file hashes. `verify_v3.py` reruns v2/v1 and then checks new landmark bytes, GNU instruction text, LLVM instruction boundaries/bytes, excerpt files/code-range hashes, constants, and 34 PD vtable pointers. It deliberately corrupts an expected instruction in memory to confirm rejection.

`reference_models_v3.py` has **16 pure-Python tests** for arithmetic/conditional reconstructions. They do not execute or emulate the original Windows binary and do not establish GPU correctness.

These purpose-built read-only PE/PDB readers are intended for the matching inputs, not as hardened parsers for arbitrary hostile data. Unsupported CodeView field types fail explicitly. No analysis helper changes the original files.

## Inspectable new records

- `evidence/v3/landmarks_v3.json`: 183 address/byte/instruction records with module SHA-256.
- `evidence/v3/excerpt_manifest_v3.json`: 49 file and byte-range hashes, exact decoder-aligned bounds.
- `evidence/v3/pipeline_map_v3.json`: corrected resource layout, hook slots, host fields, reset contract and unresolved items.
- `evidence/v3/constants_and_imports.json`: 11 pinned numeric/sign-mask/export-name byte records.
- `evidence/v3/pd_api_dimension_vtable.json`: concrete RTTI-backed API vtable.
- `verification_v3.json`: fresh full verification including rerun earlier checks.
- `scripts/reference_models_v3.py` and `scripts/verify_v3.py`: runnable verification companions.

Prior reports' “39/79 excerpts” and “50/142 records” are historical per-pass counts, not current totals. No original evidence excerpt has been silently rewritten to fit a new conclusion; the live report explicitly records corrections.

## Regenerate excerpts

```bash
objdump -d -j .text -M intel --no-show-raw-insn \
  work/extracted/SKSE/Plugins/SkyrimUpscaler.dll > work/host.asm
python scripts/disasm_index.py \
  --dll work/extracted/SKSE/Plugins/SkyrimUpscaler.dll \
  --asm work/host.asm \
  --strings evidence/SkyrimUpscaler_selected_strings.json \
  dump 0x2A9740 > work/ui_depth_hook.asm
```

Left columns are RVAs; instruction operands can be preferred VAs. The original image base is 0x180000000. Live addresses require the module's actual loaded base. String comments and analyst names are not automatically original function symbols.

## Main open boundary

The exact final Skyrim Scaleform submission, late-mod-widget attribution, all scissor/transform paths, actual GPU provider and dimensions, live reference growth, and transition stability still need a matching Skyrim executable and runtime captures. Recovered calls are not proof that their branches execute in every renderer mode.
