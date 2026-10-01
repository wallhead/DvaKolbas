# Research files

The plain [RE database](../docs/RE_DATABASE.md) is the starting point. Open its reports or CSV files and use normal text search.

* `aio18/address-map.csv` and `modules.csv`: compact address and binary-identity indexes.
* `aio18/supplied-v2/`: complete supplied v2 report, evidence, scripts, and verification records, plus our two local verification results. Original files retain their supplied bytes; `SHA256SUMS.txt` covers the original package members.
* `aio18/source-bundles/`: both original evidence ZIPs, including the superseded first revision.
* `aio18/local-analysis/`: our Ghidra/Capstone output and inventory, including unsuccessful-analysis diagnostics alongside the successful final reports. Function names recovered by a decompiler are not automatically authoritative symbols.
* `aio18/local-tools/`: snapshots of the Python/Java analysis utilities used locally. They contain session paths and expect the original local research layout; the supplied v2 README documents its separate reproduction commands.
* `dlss-baseline/validation/`: build/test/package/gameplay results and text logs from the tested NVIDIA baseline. Graphics Tools-dependent checks remained unavailable; distinguish those from passed checks.
* `dlss-baseline/workstation-tools/`: historical build, staging, installation, and MO2-restoration scripts.
* `references/`: the user's implementation prompt, preserved as reference material rather than new instructions.

No original runtime DLLs/EXEs/PDBs, extracted game files, build/dependency directories, Ghidra project databases containing imported binaries, or MO2/profile backups are included. Analysis reports include build-specific disassembly/decompilation and local tool diagnostics, published at the user's request. The two evidence ZIPs contain reports and analysis data, not the original upscaler runtime.

The latest supplied evidence revision and authored findings are for static RE. They do not establish FSR runtime acceptance. No game or MO2 setting changed during this publication.

[Publication checks](publication-checks.json) record the staged evidence hashes, address/module counts, archive exclusions, and local-link checks.
