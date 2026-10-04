# RazKolbas NR comparison — 2026-10-04

The owner reports stable NR in RazKolbas. Read-only inspection confirms a
different color contract from Dva; it does not yet establish a Style-0 fix.
The owner is unsure whether Raz's stable check used Style 0 / Tone 1. Its
currently saved V5.4 INI has NR off, **Style 2 / Tone 1**, intensity about 0.98,
Default preset and the experimental `ada-fastfp16` profile.

Reference checkout: `C:/Users/user/Documents/ChatGPT/RazKolbas`, HEAD
`c89f799`. Its existing untracked `docs/NEXT_SESSION_PROMPT.md` was preserved.
[The receipt](razkolbas-comparison.json) pins the exact inspected source files
and installed RTX40 runtime. No Raz/Dva rendering source or installed setting
was changed by this comparison.

| Contract | RazKolbas | Dva native Before |
| --- | --- | --- |
| RTX40 runtime | `ada-fastfp16`, SHA256 `e67dee…c989a` | `rtx40`, same exact SHA256 |
| NR color/output | `R8G8B8A8_UNORM`; shared textures allocated in `NrStage::rebuild` | `R16G16B16A16_FLOAT`; shared native bridge |
| Color preparation | Color bytes copied directly; no color transfer shader in `NrStage`/SDR preparation | Explicit Gamma22/SRGB decode into linear FP16 |
| Delivery | `CopyResource` from RGBA8 NR output into prepared SR input | FP16 NR output, then encode or lease linear input to FSR |
| Guides | R32 depth conversion; copied RG16 motion; scale equals NR width/height | R32 depth conversion; copied RG16 motion; scale equals NR width/height |
| Depth inversion | Explicit integer 0 | Measured camera depth convention |
| UI correction | Off; UI composed later | Off; native UI composed later |
| Tone/style controls | Full scalar controls; no Stable colors correction | Same scalar keys; Stable colors remains rejected/off for comparison |
| Ordering | NR processor installed before DLSS/DLAA evaluation | NR before selected DLSS/FSR provider; ReShade/FSR delivery routes differ |

Creation contracts agree on direct feature `0x12`, app ID `0x0876232c`, API
`0x15`, create flags `0x42`, native scaling and quality 2. Both provide resource
allocation/release and scaling callbacks. Raz's Auto/Default maps to preset 0;
Shipping maps to 1. Both use integer bool controls and unsigned Style. Raz's
exposed color-encoding/resolve-strength settings are not consumed by the
inspected direct NR stage; their UI presence is not a color-stabilization path.

The historical Raz 0.1.107 receipt in `docs/IMPLEMENTATION_STATUS.md` records
successful Skyrim DLAA NR submissions with the same RTX40 runtime through
5,400 frames. This establishes processing history, not a retained matched
Style-0 camera-stability measurement. Current source admission/placement also
needs to be distinguished from the exact installed historical build.

## Investigation decision

Use Raz as a source-level positive reference alongside AIO19. Prioritize the
SDR byte-preserving input/output contract and the actual capture/guide stage.
Do not change Dva to RGBA8 solely because it differs: the earlier
[format probe](FORMAT_COMPARISON.md) already found Style-0 drift of 8.839 code
values with encoded RGBA8 versus 22.440 with decoded FP16 in its synthetic
context scene. RGBA8 reduced that response but did not eliminate it. That
fixture is not a matched Skyrim camera rotation.

The next gameplay comparison must enable **Raz NR Style 0 / Tone 1**, verify a
visible NR off/on effect, and inspect the same building/camera movement, with
FG/sharpening off and matched native resolution. A stable result would support
an isolated Dva trial of Raz's color contract; a shifting result would explain
why Style 2 looked stable without establishing integration parity. Actual AIO
texture capture remains ready and useful. Retain full Tone 1 and distinct
styles; no drift-fix claim or milestone advance follows this source comparison.
