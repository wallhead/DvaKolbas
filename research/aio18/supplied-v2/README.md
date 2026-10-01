# AIO18 FSR reverse engineering — revision 2

Start with **AIO18_FSR_RE.md**, sections **16–21** for this update. The report retains the earlier findings and evidence. **CHANGELOG.md** summarizes what changed; **RUNTIME_BREAKPOINTS_v2.md** is a proposed trace map, not an executed game test.

## What is included

There are **79 annotated disassembly excerpts**: 39 from the first pass and 40 additional excerpts under `evidence/v2/disassembly/`. The two landmark files contain **142 instruction records at 140 distinct addresses**. The original 20 structure layouts are retained; nine additional class/inheritance layouts contain 91 more direct data fields. Input hashes and the two matched AMD PDB identities are unchanged.

The update maps AIO18's pre-UI SR/ReShade/HUD-target sequence; conditional final-overlay submission; OMSetRenderTargets redirection; shared D3D11/D3D12 fences; AMD UI copy timing, ring indices, fence roles, and resize/drain behavior. It explicitly distinguishes the generic FGFence publisher's observed DLSS-G caller from FSR-specific evidence.

**No original DLLs, PDBs, shaders, fonts, or game executables are redistributed.** No Windows DLL or GPU workload was executed. The files are build-specific evidence and analysis helpers, not a working plugin or a compatibility certificate.

## Reproduce

The scripts require Python's standard library. Extraction additionally uses system **libarchive**; independent decoding uses **GNU objdump**. Linux/WSL commands:

```bash
mkdir -p work
python scripts/extract_archive.py /path/to/SkyrimUpscalerAIOBuild18-Hotfix1.7z work/extracted > work/manifest.json
python scripts/verify_v2.py work/extracted \
  --archive /path/to/SkyrimUpscalerAIOBuild18-Hotfix1.7z \
  --output work/verification_v2.json
sha256sum -c SHA256SUMS.txt
```

Use a fresh extraction directory. Filename spelling is not the identity check: size and SHA-256 are. `verify_v2.py` runs the original verifier, checks the new instruction bytes, independently decodes all 92 new records with objdump, checks all 40 new excerpt code ranges, reparses the nine additional PDB layouts, checks 22 AMD vtable entries, and runs parser/evidence rejection controls. Its result is stored in **verification_v2.json**.

`verification.json` is the retained first-pass record. A fresh rerun of the first-pass checks is embedded under `v1` in the second-pass verification output. **PASS applies to static reproducibility only.** It is not proof that every descriptive interpretation is correct or that any GPU/runtime scenario has been tested.

These are purpose-built, read-only tools for this matching build, not hardened parsers for arbitrary corrupt archives, PE images, or PDBs. Unknown CodeView field leaves in the extended class reader fail explicitly; inherited fields are not silently flattened.

## New machine-readable evidence

- `evidence/v2/landmarks_v2.json`: module identities, addresses, instruction bytes, mnemonics, operands and direct/RIP references.
- `evidence/v2/excerpt_manifest_v2.json`: source code-span and excerpt-file hashes for all 40 additions.
- `evidence/v2/amd_internal_class_layouts.json`: internal layouts, base records, methods, nested types and data fields from the matched PDB.
- `evidence/v2/amd_swapchain_vtable.json`: 22 method mappings for the concrete selected AMD swapchain implementation.
- `evidence/v2/pipeline_map_v2.json`: host/AMD boundaries, field offsets, conditions, attribution control, and unresolved areas.

## Regenerate a disassembly excerpt

```bash
objdump -d -j .text -M intel --no-show-raw-insn \
  work/extracted/SKSE/Plugins/SkyrimUpscaler.dll > work/host.asm
python scripts/disasm_index.py \
  --dll work/extracted/SKSE/Plugins/SkyrimUpscaler.dll \
  --asm work/host.asm \
  --strings evidence/SkyrimUpscaler_selected_strings.json \
  dump 0x19F420 > work/preui.asm
```

The left column uses RVAs; instruction operands retain preferred VAs. Optional string comments are scanner annotations, not a guarantee of original function names. AMD function names can be added with the matching-PDB symbol index. For leaf functions or narrowly scoped regions, use `dump START END`; unwind-based whole-function bounds are not available for every leaf.

## Remaining runtime work

No SkyrimSE.exe addresses, shared Scaleform flush chain, per-mod late-widget attribution, live provider choice, GPU resource dimensions, complete viewport/scissor/transform contract, or end-to-end failure/resize/backend-switch result has been verified in a running game. The report now narrows host ordering statically but does not claim every ENB, ReShade or Community Shaders branch is solved.
