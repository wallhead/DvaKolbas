# NR research checkpoint — 2026-10-03

The user's request defines this work: RTX 50/40/20–30 NR profiles, an unsupported AMD option, DLSS and FSR integration, one live-selectable Before/After pass, and **After FG including generated frames**. Attached RE recommendations and RazKolbas's project instructions are reference material, not additional user requests for DvaKolbas.

## Inputs checked locally

- Supplied archive: `AIO18_NR_RE_Evidence_v5.zip`, SHA-256 `9fbb3221df4311e1e6d18cc88fbb442a52ec290d7008eae2e84bf66bc4393e7d`. All 79 entries listed in SHA256SUMS passed an independent byte-hash check. All 80 text files, including the checksum listing, are preserved under [research/aio18/nr-v5](../research/aio18/nr-v5/README.md). Embedded verifier scripts were not run; disassembly was not independently regenerated in this checkpoint.
- The six NR source blobs in the supplied [current-head comparison](../research/aio18/nr-v5/evidence/v5/dvakolbas_current_head.json) match DvaKolbas base `90e8693572c1a688c805538fbf205b3faa72792e` exactly.
- RazKolbas reference was read at `c89f7996aa4fc7888bfe3227f812f5e16601a8c2`. Its [runtime-selection report](C:/Users/user/Documents/ChatGPT/RazKolbas/docs/NR_RUNTIME_SELECTION.md), direct loader and caller-name shim provide implementation leads. Their runtime results do not become DvaKolbas results.
- Current hardware inventory includes RTX 4080 SUPER PCI `10de:2702`, driver `32.0.16.1714`, and an AMD integrated adapter. That inventory is not a replacement for detecting the game device's actual render adapter.

| Supplied path | Bytes | SHA-256 | Authenticode |
| --- | ---: | --- | --- |
| `nvngx_dlssnr_5_series/nvngx_dlssnr.dll` | 165840496 | `e16bcf15e16e13f527491cdf7845b2fe6521a738d8f7c9c721866a8496e1fc8e` | Valid NVIDIA signature |
| `nvngx_dlssnr_4_series/nvngx_dlssnr.dll` | 165840496 | `e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a` | HashMismatch; retained NVIDIA signer |
| `nvngx_dlssnr_plainfp16_for_2_3_series/nvngx_dlssnr.dll` | 309671536 | `6dac1b40f0c87af84a8177b18c741e84fb0c914f204c9d87d95916b665ba3af8` | HashMismatch; retained NVIDIA signer |

All three files report version 310.8.0.0 and match RazKolbas's corresponding pinned catalog artifacts. No supplied NR DLL was executed during this checkpoint. Static identity, catalog eligibility and actual GPU output are separate evidence.

## Useful evidence and limits

The supplied [Pass 5 review](../research/aio18/nr-v5/AIO18_NR_RE_v5.md) reconciles the prior four passes. Its normal late sequence is **scene → SR → late NR → FG/presentation**; its alternate sequence is scene/guides → NR → SR. Neither proves the user's new post-FG stage. External `0x138` and internal `0x110` blocks remain distinct; MVecScale is not jitter. Separate feature handles in the recovered multipass loop do not justify reusing a temporal handle across arbitrary real/generated sequences.

AIO's mapped core nulls NGX UI/UIAlpha and sets UICorrection=0 while composing UI privately afterward. DvaKolbas can pass those UI inputs and expose correction. This is an A/B experiment, not a proven bug. Preserve DvaKolbas's stronger reset, drain and failed-recording ownership behavior rather than copying weaker observed cleanup behavior.

RazKolbas's direct D3D12 path uses feature-0x12 exports from the NR DLL and parameter exports from a retained driver NGX core. Its caller-name IAT shim is narrow and profile-specific. Its report records failed unshimmed RTX 40 initialization followed by successful shimmed output. This supports trying that route before patching unrelated NVIDIA DLLs; DvaKolbas still needs its own negative/positive probe and teardown evidence.

The inspected pinned AMD header `out/research/sdk-v2.3.0/Kits/FidelityFX/framegeneration/include/ffx_framegeneration.h` exposes a per-image presentation callback with current image, separate UI, output, command list, frame ID and real/generated classification. AMD describes the callback composition path in its [swapchain documentation](https://gpuopen.com/manuals/fidelityfx_sdk2/techniques/frame-interpolation-swap-chain/). The callback packet does not provide generated-image NR depth/motion.

The inspected local Streamline 2.11.1 `sl_dlss_g.h` contains options/state and an API-error callback, without an equivalent public output-image callback. The current [NVIDIA public header](https://github.com/NVIDIA-RTX/Streamline/blob/main/include/sl_dlss_g.h) and [integration guide](https://github.com/NVIDIA-RTX/Streamline/blob/main/docs/ProgrammingGuideDLSS_G.md) were checked as additional context; they do not change our runtime pin or prove a private interface. A version-specific post-generation adapter and matching guide/history contract require controlled observation.

## Next gate

Review the [written NR design](superpowers/specs/2026-10-03-nr-design.md), then write its implementation plan. The first implementation gate must prove post-FG output/guide access for both providers. The final result must retain the user's literal ordering; unavailable generated outputs cannot be silently replaced by pre-FG NR.

The accepted FSR FG package, INI and MO2 settings are unchanged. No GPU probe was launched while Skyrim was running. [Machine-readable receipt](NR_RESEARCH.json) records artifact identities, hardware inventory and the exact six matching source blobs.
