# AIO19 Local Tone / Backbuffer RE: validation and follow-up

2026-10-04. The new report's main route findings are supported by independent decoding of the exact supplied binaries. Its conditional output initialization was a meaningful gap in the previous isolated Backbuffer experiment. **Testing that gap did not fix the standalone drift.** Further observations inside the runtime also reproduce drift with constant effective controls. No installed DLL, game INI, MO2 setting or product source was changed. Stable colors remains rejected as the requested solution.

## Static findings

The report's archive, host, PD, original NR, shipped INI and two Dva source blob identities match the retained inputs. The archive filename lacks `(1)` but its bytes/hash match. Only the Markdown report was supplied for this review; its separately described evidence ZIP/verifier is unavailable. **The claimed 144/144 and 7,289 instruction checks were not replayed.** Our independent receipt contains 823 instructions in 24 selected observation intervals, with original bytes and hashes. Decoding starts from actual PE unwind-fragment boundaries; the two leaf entries are separately checked against their caller/entry instructions. A short report window is not treated as a safe hook prologue.

| Finding | Independent result |
| --- | --- |
| First-pass Tone is packet `+0x18C`, forwarded from stored `+0x33CC` | Confirmed selected load/store instructions; matches the two inspected Dva source blobs. Full validator non-finite semantics are not claimed from one classification call. |
| Conditional mask route overwrites scalar Structure/Tone/Intensity with 1 | Confirmed stores at `A3F97/A3FA1/A3FAB`. Activation and mask-channel meaning in actual AIO gameplay remain unknown. |
| Local Backbuffer starts as Output and can select mapped Color | Confirmed `A56F0..A5709` pointer tests/selection. This is not an independent luminance-reference guarantee. |
| Builder can copy Color into Output before evaluation | Confirmed pointer/compatibility checks, state arguments and CopyResource dispatch at `A2480`. Dva's stage **and surrounding Before adapter** do not perform this initialization. Causality remained a hypothesis until the new probe. |
| Builder falls back to Output for Backbuffer | Confirmed `A2642..A265D`. |
| Runtime callback can overwrite Tone | Confirmed getter/null branch, callback-result test and conditional double→float overwrite of parsed `+0xE4`. Existence alone does not establish activation. |
| Configuration helper scales 14 coefficients with Tone | Confirmed clamp/interpolation/store sequence, including coefficient 1's baseline 1. No exposure/gamma/color-matrix names assigned to unnamed coefficients. |
| Tone reaches network argument preparation | Confirmed load at `22563`, stack store at `225CB`, and call at `22636`. Learned GPU equations are not reconstructed. |
| Runtime can set Reset on control changes | Confirmed selected comparison and conditional store at `17B98`; state/threshold depend on the surrounding path. Do not remove product lifecycle resets. |
| Generic AutoExposure/ExposureTexture/GlobalToneStrength keys are not literal NR parser evidence | Original NR image contains none of those three literals; PD contains them. This is a narrow literal/parser observation, not proof that no indirect adaptation exists. |
| Output allocation preserves the supplied format | Selected dimension/format forwarding to allocation is confirmed. The active game's resource/view formats and transfer remain unmeasured. |

The selected NR code intervals match the installed RTX40 model's bytes independently. That permits checking the **specific observation instructions** in both pinned images; it does not imply that every RVA or all model data is interchangeable.

## Output-initialization experiment: 32 runs

The retained native 640×360 fixed-surface context fixture was repeated with Tone/intensity/structure/skin 1, preset 0, Stable colors off, zero motion, depth 0.5, and one initial reset. Only the surrounding image changes. Each run has 160 NR evaluations, all-pixel alpha checks and confirmed retirement.

Four configurations separate the operations:

1. Existing baseline: no initialization, null Backbuffer.
2. Copy Color→Output, null Backbuffer.
3. Copy Color→Output, Backbuffer=Output and matching subrect.
4. No initialization, Backbuffer=Output.

The copy follows the real queued producer dependency, transitions input/output to COPY_SOURCE/COPY_DEST, then restores shader-read/UAV states. Color and Output remain distinct. Backbuffer uses the already-retained Output resource; alias admission, slot lifetime and final readers are unchanged. No CPU wait was added. This is an ignored throwaway stage derivative, not a shipping option or full debug-layer/lifecycle qualification.

Both pinned models were tested with Gamma22→linear FP16 for Styles 0/1/4. Style 1 additionally used the deliberately encoded-input diagnostic. All four configurations have **identical 160-row ROI CSVs and all four saved full RGB images** within every group, and the new baselines match previous captures exactly. Settled maximum channel drift remains 22.5525 / 21.8355 / 29.9268 code values for Gamma22 Styles 0/1/4, and 6.9151 for encoded-input Style 1. These are RGB-code statistics, not a Skyrim camera-motion quality metric. The separate original-scene-reference experiment remains a different semantic test.

## Inside-runtime observations: 8 runs / 3,840 snapshots

An owned standalone child was debugged at the exact verified instruction points `1886B`, `18E69`, `21BB0` and `17B98`. No attachment to Skyrim, permanent DLL patch or product instrumentation occurred. Instruction prefixes were checked before placing temporary child-memory breakpoints. The post-callback/configuration frame is `RBP+0x20`; network-entry frame is `R8`. Float observations use round-trip precision. Output hashes match non-debugged baselines, so no captured image change was introduced by observation. Debugged runs are **not timing measurements**.

Six runs cover both models and Styles 0/1/4 with Tone 1. Two RTX40 Style-1 controls cover Tone 0 and the encoded-input diagnostic. Each records 160 post-callback, 160 post-configuration and 160 network-entry snapshots:

- Callback slot is null and callback routine returns false on all 1,280 sources.
- Effective Tone is exactly the requested 1, or 0 in the explicit control, at all three observation points.
- ControlMask and Backbuffer are null in these scalar-only baseline runs.
- Reset is 1 only on source 1, then 0; the conditional internal control-reset write breakpoint is never reached.
- The 14 coefficients are constant within each run. Post-configuration and network-entry vectors match, and vectors agree across the two models for the same style.
- The scene still drifts with these controls/configuration/reset observations stable.

Observed non-neutral coefficients at Tone 1 are indices 2/4/5 for Style 1 (`-0.1/-0.25/-0.1`) and index 5 for Style 4 (`-0.15`); Style 0 is neutral in this block. Coefficient 1 is 1; remaining entries are 0. These are indexed numbers, **not assigned semantic color controls**. Tone 0 makes the inspected block neutral but leaves about 11.55 code values of drift in this fixture with structure still enabled. The network also receives Tone separately; this does not isolate the block as the entire tone operation or justify patching it.

This establishes the report's case C **for our fixture**: stable requested/effective Tone, coefficient block and observed resets, changing same-surface NR correction. It does not establish actual AIO19 callback/mask/configuration behavior in the user's scene, or rule out unobserved model/guide processing.

## Revised next step

Follow-up: [native format comparison](FORMAT_COMPARISON.md) traced the conditional native preparation passthrough and tested encoded UNORM8 against encoded/linear FP16. Twelve standalone runs completed, but UNORM8 still drifts; both models match captured UNORM8 output. A format swap is not a demonstrated fix. Actual AIO host scene evidence remains the next discriminator.

Stop repeating generic flag and isolated Backbuffer/initialization experiments without new game-specific evidence. Do not ship a speculative initialization copy, clear coefficients, reduce Tone, or restore the rejected broad-delta subtraction.

The next comparison must establish actual AIO19 vs Dva producer/evaluation boundaries: Color/Output resource and view formats, source transfer and pixel values, working dimensions, active mask/resolve branch, effective Tone/configuration/reset, and same-surface input→NR correction. AIO19's output-format preservation is now a concrete difference to trace against Dva's forced linear FP16 preparation. A format or transfer experiment remains research until it matches the actual branch. Controls must be matched by effective dispatch values, not only INI text.

If the actual same-input AIO route also drifts with stable controls, a style-preserving correction needs separate design and acceptance for lighting changes, disocclusions, style strength and ghosting. Neither a non-temporal shader nor delayed smoothing automatically guarantees camera-invariant materials.

No NR milestone advances from these reproductions. The enabled-tone Skyrim fix remains open. `backbuffer-static-witnesses.json`, `output-parity-validation.json` and `runtime-observation-validation.json` retain identities, raw/compiled-source hashes, capture hashes, configurations and scope limits. Follow-up sources/executables are separately snapshotted under ignored `out/research/nr/tone-preserve-investigation/backbuffer-report/`; the earlier audit's source hashes describe its earlier build. Proprietary binaries and captures remain local.
