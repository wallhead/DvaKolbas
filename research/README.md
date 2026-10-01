# Research files

The plain [RE database](../docs/RE_DATABASE.md) is the starting point. Open its reports or CSV files and use normal text search.

* `aio18/address-map.csv` and `modules.csv`: compact address and binary-identity indexes.
* `aio18/supplied-v2/`: complete supplied v2 report, evidence, scripts, and verification records, plus our two local verification results. Original files retain their supplied bytes; `SHA256SUMS.txt` covers the original package members.
* `aio18/supplied-v3/`: V3 report/evidence, including corrected SR fields and jitter/depth/reset/UI-state traces, plus fresh local byte, layout, PDB, and Python reference-model checks.
* `aio18/supplied-v4/`: V4 cumulative report/evidence, eight host-routing excerpts, and fresh local archive/byte/instruction checks. The 325 earlier address records remain unchanged; `host-routing-map.csv` adds 16 checked host landmarks.
* `aio18/supplied-v5/`: latest cumulative report and 13 temporal-input excerpts, 38 new landmarks, 11 strings, fresh local checks, and the rerun supplied verifier/eight reference-model tests. Local notes qualify misaligned excerpt edges and the incorrectly named transform-helper excerpt; the actual callee is decoded separately.
* `aio18/source-bundles/`: all five original evidence ZIPs. Earlier revisions remain available for comparison.
* `aio18/local-analysis/`: our Ghidra/Capstone output and inventory, including unsuccessful-analysis diagnostics alongside the successful final reports. Function names recovered by a decompiler are not automatically authoritative symbols.
* `aio18/local-tools/`: snapshots of the Python/Java analysis utilities used locally. They contain session paths and expect the original local research layout; the supplied v2 README documents its separate reproduction commands.
* `dlss-baseline/validation/`: build/test/package/gameplay results and text logs from the tested NVIDIA baseline. Graphics Tools-dependent checks remained unavailable; distinguish those from passed checks.
* `dlss-baseline/workstation-tools/`: historical build, staging, installation, and MO2-restoration scripts.
* `references/`: the user's implementation prompt, preserved as reference material rather than new instructions.

No original runtime DLLs/EXEs/PDBs, extracted game files, build/dependency directories, Ghidra project databases containing imported binaries, or MO2/profile backups are included. Analysis reports include build-specific disassembly/decompilation and local tool diagnostics, published at the user's request. The evidence ZIPs contain reports and analysis data, not the original upscaler runtime.

V4 and V5 original ZIPs have top-level `AIO18_FSR_RE_v4/` and `AIO18_FSR_RE_v5/` folders and seven checksummed Python cache files each. The unpacked publication removes those prefixes and omits the caches; supplied text remains byte-identical. Their unchanged `SHA256SUMS.txt` files retain all 190/211 original entries, so use the original ZIPs for complete archive verification. Publication checks validate the 14 omitted cache entries from those ZIPs; no cache was executed.

The latest supplied evidence revision and authored findings are for static RE. They do not establish FSR runtime acceptance. No game or MO2 setting changed during this publication.

[Publication checks](publication-checks.json) record the staged evidence hashes, address/module counts, archive exclusions, and local-link checks.
