# RE database

This is a plain folder of reports, evidence, and address maps. Read it directly in GitHub or a text editor. There is no database server or special search tool to set up.

## Start here

NR workstream (2026-10-03): [research checkpoint](NR_RESEARCH.md), [artifact/verification receipt](NR_RESEARCH.json), [supplied NR v5 report](../research/aio18/nr-v5/AIO18_NR_RE_v5.md), and [NR design draft](superpowers/specs/2026-10-03-nr-design.md). The NR archive is a separate evidence package from the FSR packages below. Its 79 checksummed members were verified; its scripts and disassembly were not rerun. The requested post-FG NR ordering is a new requirement, beyond the recovered AIO late-before-FG path.

| Item | Contents |
| --- | --- |
| [Our RE findings](FSR_AIO_RE_NOTES.md) | Ghidra/Capstone findings, verified v2–v5 additions, and implications for TRP |
| [Address map](../research/aio18/address-map.csv) | 363 instruction records at 350 distinct module/RVA pairs, with hashes and evidence paths |
| [Host routing map](../research/aio18/host-routing-map.csv) | 16 additional locally checked V4 landmarks for ReShade, UI transitions, scissor calls, and ENB detection |
| [Module identities](../research/aio18/modules.csv) | 21 DLL identities, hashes, sizes, preferred image bases, and versions |
| [Supplied v5 report](../research/aio18/supplied-v5/AIO18_FSR_RE.md) | Current cumulative SR, FG, UI, synchronization, resize, jitter, depth, and reset analysis |
| [Proposed breakpoint map](../research/aio18/supplied-v5/RUNTIME_BREAKPOINTS_v5.md) | Follow-up runtime observations; these traces have not been collected |
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
| ReShade brackets | Tracked-runtime effects save/clear/restore routing flags; target substitution is gated during effects | Host `0x284ec0`, `0x284e70`, `0x2a97e0`; section 28 |
| Nested menu scene | UI phase clears around the StatsMenu scene call; the thunk invokes post-processing afterward | Host `0x1a0827`, `0x1a0839`; section 29 |
| Scissor scope | No slot-45 replacement in the inspected installer; two identified ImGui calls confirmed locally | Host `0x19ec61`, `0x21350e`, `0x2135ac`; section 28; global search is supplied evidence |
| ENB | The checked helper probes `ENBGetSDKVersion` presence; stage ordering remains unresolved | Host `0x299750`; section 30 |
| CS temporal guides | Dedicated capture converges into prepared depth/MV consumed by both SR and FG; exact CS version/hook boundary remains untested | Host `0x2656f5`, `0x2a15b0`, `0x294cf5`, `0x2944d9`; sections 32–34 |
| Masks/exposure | Normal SR reactive input is null; DX11 external exposure/T&C are null with auto-exposure context flags | Host `0x294f09`; PD `0xfe3bc`, `0xff096`, `0xff2dd`; section 33 |
| V5 evidence qualification | Four excerpts start off instruction boundaries; the named transform-helper excerpt covers capture code instead. Local callee decode confirms a draw, with broader restoration unresolved | [Our V5 notes](FSR_AIO_RE_NOTES.md#revision-5-temporal-input-provenance-and-evidence-qualifications) |

Addresses are RVAs in the named module, not Skyrim executable addresses. Runtime address = loaded module base + RVA. Match the module SHA-256 before using an address. Duplicate address-map rows retain separate evidence contexts.

The original AIO archive SHA-256 is `136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8`. The latest evidence ZIP (v5) is `5505f04660d66aca9d78aff7b7e08fb9af52ef118a624b6542edf4b638c86290`.

## What has been checked

Local static checks passed: 25 original file hashes, 325 instruction records, 49 v3 code spans/6,107 excerpt instruction boundaries, 11 constants, 34 PD vtable entries, and reruns of the earlier 40 spans/22 AMD vtable entries/PDB layouts and identities. All 16 supplied pure-Python reference-model tests passed. Capstone performed the local decoding/boundary checks; the supplied GNU/LLVM verifier was not rerun here. [Local byte checks](../research/aio18/supplied-v3/independent-verification.json), [v3 range/boundary checks](../research/aio18/supplied-v3/local-v3-verification.json), [layout checks](../research/aio18/supplied-v3/local-layout-verification.json), [PDB identities](../research/aio18/supplied-v3/local-pdb-verification.json), [reference-model results](../research/aio18/supplied-v3/local-reference-model-tests.json).

V4 checks passed for all 190 checksummed ZIP members, the same 25 original files and 325 retained instruction records, plus mnemonic/operand comparisons of 720 instruction records in eight new excerpts. [V4 byte checks](../research/aio18/supplied-v4/independent-verification.json), [new excerpt checks](../research/aio18/supplied-v4/local-v4-verification.json). The 16 host-routing CSV labels are analyst interpretations attached to checked bytes. The archive’s global scissor search and whole-program ENB string inventory were not independently rerun. Earlier V3 range/layout/model results above remain historical checks.

V5 checks passed for 211 checksummed package members and 363 cumulative selected instruction records. All 38 new records also align with decoding from PE unwind-function starts, and all 11 embedded strings match their supplied file offsets. Across 13 excerpts, 5,615 literal GNU byte chunks (22,768 bytes) and 724 PD instruction comparisons passed; edge rows are not all complete instructions. The reviewed supplied verifier and eight simple reference-model tests were rerun successfully. [Local V5 checks](../research/aio18/supplied-v5/local-v5-verification.json), [verifier rerun](../research/aio18/supplied-v5/local-supplied-verifier-v5.json). These checks support the qualified findings above, not complete shader equations or caller-wide state restoration.

Static reproducibility does not prove every interpretation, active GPU provider, complete game UI ordering, or end-to-end resource lifetime. FSR gameplay and performance remain untested. The existing DLSS gameplay evidence is separate. The [FSR implementation plan](superpowers/plans/2026-10-01-fsr-sr.md) has not been executed.
