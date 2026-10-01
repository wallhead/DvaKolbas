# RE database

This is a plain folder of reports, evidence, and address maps. Read it directly in GitHub or a text editor. There is no database server or special search tool to set up.

## Start here

| Item | Contents |
| --- | --- |
| [Our RE findings](FSR_AIO_RE_NOTES.md) | Ghidra/Capstone findings, verified v2 additions, and implications for TRP |
| [Address map](../research/aio18/address-map.csv) | 325 instruction records at 316 distinct module/RVA pairs, with hashes and evidence paths |
| [Module identities](../research/aio18/modules.csv) | 21 DLL identities, hashes, sizes, preferred image bases, and versions |
| [Supplied v3 report](../research/aio18/supplied-v3/AIO18_FSR_RE.md) | Current cumulative SR, FG, UI, synchronization, resize, jitter, depth, and reset analysis |
| [Proposed breakpoint map](../research/aio18/supplied-v3/RUNTIME_BREAKPOINTS_v3.md) | Follow-up runtime observations; these traces have not been collected |
| [Our Ghidra/Capstone output](../research/aio18/local-analysis) | Targeted decompilation, disassembly, references, vtables, and analysis logs |
| [Evidence and tools](../research/README.md) | Source archives, analysis scripts, verification records, and DLSS baseline results |

## Useful findings

| Topic | Finding | Where to look |
| --- | --- | --- |
| SR | Separate native D3D11 FSR 3.1.2 and D3D12-interoperated FFX paths | PD `0xfedc0`, `0xd0cb0`, `0x100b70`; report sections 5–6 |
| FG | Preparation and actual interpolation are separate; AMD Present invokes the generation callback | PD `0xef230`, `0xef880`; AMD `0xbab0`, `0x94a0`; section 7 |
| Frame identity | Prepare and Configure receive the same source-frame ID | PD `0xef581`, `0xef719`; sections 7 and 10 |
| Camera/order | The inspected AIO path leaves camera vectors/world scale zero and prepares before its same-frame Configure | Section 11; a static concern, not a reproduced failure |
| Scene/UI | An eligible pre-UI branch runs SR/effects before switching UI targets; later bindings can be redirected | Host `0x19f420`, `0x2a9b30`; section 16 |
| UI lifetime | AMD registers the UI resource, then copies it during Present; registration is not completion | AMD `0xadc0`, `0x9ea0`; section 18 |
| Retirement | Transfer slots, source IDs, game buffers, generated outputs, and presentation progress have different lifetimes | Sections 18–20 |
| Resize | The AMD swapchain drains/releases replacement resources before its underlying resize | AMD `0xa360`, `0xbec0`; section 20 |
| SR resources | Reactive is payload `+0x20`; output is `+0x30` with fallback `+0x28`. The DX11 intermediate bundle has a different layout | Sections 22 and 26 |
| UI depth ownership | A qualifying depth-SRV hook GetResource call has no balancing Release in the inspected function | Host `0x2a9740..0x2a97d6`; section 23; runtime impact unmeasured |
| Jitter/depth | Engine-hook jitter signs, prepared guide dimensions, and depth-convention recreation are traced for specific branches | Sections 24–25; private conventions require TRP validation |
| FG resets | Backend resetPending survives failed Prepare/Configure and clears after successful enabled work | PD `0xf0320`, `0xef230`; section 25 |

Addresses are RVAs in the named module, not Skyrim executable addresses. Runtime address = loaded module base + RVA. Match the module SHA-256 before using an address. Duplicate address-map rows retain separate evidence contexts.

The original AIO archive SHA-256 is `136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8`. The latest evidence ZIP (v3) is `ca9e0b41ffd58dbfdae5f97cf0c3296479f43a79daa6b2e7f2fd25b172b1c62b`.

## What has been checked

Local static checks passed: 25 original file hashes, 325 instruction records, 49 v3 code spans/6,107 excerpt instruction boundaries, 11 constants, 34 PD vtable entries, and reruns of the earlier 40 spans/22 AMD vtable entries/PDB layouts and identities. All 16 supplied pure-Python reference-model tests passed. Capstone performed the local decoding/boundary checks; the supplied GNU/LLVM verifier was not rerun here. [Local byte checks](../research/aio18/supplied-v3/independent-verification.json), [v3 range/boundary checks](../research/aio18/supplied-v3/local-v3-verification.json), [layout checks](../research/aio18/supplied-v3/local-layout-verification.json), [PDB identities](../research/aio18/supplied-v3/local-pdb-verification.json), [reference-model results](../research/aio18/supplied-v3/local-reference-model-tests.json).

Static reproducibility does not prove every interpretation, active GPU provider, complete game UI ordering, or end-to-end resource lifetime. FSR gameplay and performance remain untested. The existing DLSS gameplay evidence is separate. The [FSR implementation plan](superpowers/plans/2026-10-01-fsr-sr.md) has not been executed.
