# Native color route and direct format comparison

The enabled-tone camera-drift fix remains open. No installed DLL, INI, MO2 profile or product source was changed by this investigation. Stable colors remains disabled. The original AIO19 DLL is a model comparison here, not execution of AIO19's game host.

## Recovered native route

Read-only Capstone disassembly of the hash-pinned AIO19 PD binary followed the local mapping, chain evaluator, preparation and evaluation builder. Resource mapping/copy routines `PD+74F10`, `+75080` and `+752B0` use retained resource mappings and `ID3D12GraphicsCommandList::CopyResource`; the inspected copy branches do not run a transfer shader. Output allocation at `+A06D0` forwards the input description's dimensions and format to the allocator at `+75960`.

Preparation `+A1E00` calls the scale canonicalizer `+A0A70` and reconstruction selector `+A0AA0`. The scale canonicalizer maps values <=0 or >=1 to 1. The selector returns 2 for explicit method 2, otherwise returns 1 when the canonical scale is below 1, and 0 at native scale. On the selected method-0 branch, `+A1F7D..A1F84` keeps the input pointer as both working color references; `+A1F9F` jumps directly to the return path at `+A1FFE`. That branch does not reach the later shader preparation. This is conditional static flow, not proof that the user's game enters it or that its producer supplied encoded color.

The active chain, optional mask, incoming resource format and upstream view/pixel transfer still require actual AIO host evidence. The existence of a no-conversion path does not justify relabeling Dva's production linear contract.

## Bounded standalone experiment

A private direct D3D12 Stage fixture compares one format/transfer variable, retaining the 640x360 fixed-surface scene: 160 sources in four 40-source phases, unchanged textured surface and changing dark/bright/blue/dark surroundings. The settled ROI is x220..419/y120..239, measured over the last 20 sources in each phase. Motion is zero, depth 0.5, preset 0, tone/intensity/structure/skin 1, AutoSkin/UI correction off, no mask/UI/Backbuffer, no Output preseed, and one initial reset. Styles 0, 1 and 4 are separate processes; every source completes and retires before allocator/resource reuse.

The private runtime target changes only packet color-format admission and the alpha-restoration view format. It accepts RGBA8_UNORM in addition to FP16; the real product validator is unchanged. Encoded modes intentionally violate the production declared-Linear input-domain contract and are research experiments. They are not approved NR modes.

| Input/output route | Style 0 | Style 1 | Style 4 |
|---|---:|---:|---:|
| Gamma22 decode -> FP16 NR -> Gamma22 encode | 22.440 | 21.747 | 29.967 |
| Encoded values in FP16, no transfer conversion | 9.022 | 6.941 | 7.616 |
| Encoded RGBA8_UNORM, no transfer conversion | 8.839 | 7.049 | 6.588 |

Values are maximum settled ROI RGB-channel changes in 8-bit code values. All exceed the existing one-code-value context threshold. Direct UNORM8 changes the model response but does not eliminate drift. Both original AIO19 and supplied RTX40 models produce byte-identical captured UNORM8 CSVs, four RGB images and four raw outputs for each style.

Twelve processes completed **1,920 real vendor evaluations**, with all-pixel source alpha checked on every source (442,368,000 checks), successful vendor create/evaluate/release/shutdown and confirmed retirement. An independent audit recomputes drift from all CSV rows, checks the fixed ROI source means, compares model captures and checks source alpha in all four saved raw images per process (11,059,200 additional checks). The receipt pins models, driver core, executable, actual runtime translation units and captures.

CPU decode/encode and rounding in this fixture differ from the earlier prepared GPU converter. Small cross-fixture numeric differences are not performance/quality conclusions; comparisons within this fixture isolate the intended format/transfer variable. Saved image parity does not prove parity of every unsaved pixel. Zero-motion synthetic context changes are not camera-motion gameplay qualification.

The portable checked-in CMake target also clean-built. Its separate 160-source Style-1 UNORM8 smoke matches all nine retained CSV/RGB/raw output files from the qualifying run, and its probe/private Stage/private packet sources match the original snapshot hashes. This smoke is separately identified in the receipt; it does not relabel the original 12 runs with a later build identity.

## Next game evidence

Update: the user confirms that AIO19 has no color shifts with its installed INI. [The stable game reference checkpoint](AIO_GAME_REFERENCE.md) records its exact snapshot and the newly installed matched Dva settings. This is qualitative gameplay evidence; active AIO boundary captures remain missing. The conditional discussion below describes the earlier investigation decision, not a request to repeat the already answered AIO stability check.

The remaining useful comparison is AIO19's actual host in the same building/camera scene, with one Before pass, preset 0, Tone 1, FG off and masks off. Use the same style in both implementations: the currently saved Dva style is 0, while AIO's authoritative `[NR PASS 1] Style` is 1. Its legacy global style 0 does not override the chain pass. Also match upscaling and ReShade order before claiming a one-to-one image comparison.

If AIO also drifts, restoring its integration alone cannot meet the requested camera-stable behavior; a style-preserving temporal approach needs separate design and lighting/disocclusion/ghosting acceptance. If AIO stays stable, measure its actual Color/Output formats and pixels, working dimensions, mask, effective controls/configuration and resets before changing Dva's contract. No milestone advances from these negative experiments.

[Format validation receipt](format-validation.json) records the qualifying snapshot under ignored `out/research/nr/tone-preserve-investigation/native-route/format-captures/experiment`. [Probe source](format-probe/FormatContextProbe.cpp) and [private CMake target](format-probe/CMakeLists.txt) retain the research fixture without changing the production runtime target. Proprietary binaries remain local.
