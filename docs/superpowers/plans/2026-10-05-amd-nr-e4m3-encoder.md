# AMD NR Portable E4M3 Encoder Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement our recovered nearest-even E4M3 encoding policy in the standalone CPU and portable D3D12 format tools.

**Architecture:** Extend the existing numerical-format API and byte-addressed shader with a fourth operation. Both implementations quantize FP32 words using integer operations, with independent nearest-distance expected results. Preserve the established job ownership, explicit adapter selection and file publication paths.

**Tech Stack:** C++23, existing HLSL shader toolchain, D3D12, CTest, Python/NumPy.

**Spec:** [Approved engine design](../specs/2026-10-05-amd-nr-engine-design.md), especially the numerical-reference, compute-kernel and validation sections; [recovered residual/output contract](../../amd-nr-c512-residual-output.md).

Status: implemented and independently reviewed on 2026-10-05; 12 tests passed, 1 missing-debug-layer test skipped, zero failures. Use the previously selected **Native** execution method. Branch: `codex/amd-nr-engine`.

## Global Constraints

- "FP8 is encoded storage with explicit software decode/encode; FP16 storage and required rounding boundaries are explicit."
- "Avoid `min16float` as a promise of fixed precision."
- "The product consists of source-owned C++, HLSL, logical schemas, and tests."
- "The local RE project, all supplied or extracted binaries, trained parameter data, captured reference activations, disassembly, and decompilation remain under ignored `local/amd-nr-re/`."
- "Software conversion must cover every FP32 bit pattern without out-of-width shifts."
- Use the existing standalone C++23 build; no HIP runtime, copied vendor converter, native FP8 requirement or new compiler dependency.
- Numerical format success retains `KnownArchiveIncompleteSchema` and `inference=unavailable`. No projection/graph execution, TRP integration, game installation, merge or push belongs to this plan.
- E4M3: bias 7, subnormal unit 1/512, maximum 448, nearest-even, signed zero, signed finite saturation including infinities, all NaNs -> `0x7f`.

## Review Focus

1. Extremely small finite FP32 inputs must produce signed zero without undefined or masked large shifts; Task 1 `E4m3DeepUnderflow`, Task 2 independent GPU fixtures.
2. Negative and signalling NaNs must canonicalize to `0x7f`; Task 1 `E4m3SpecialValues`, Task 2 exact GPU output comparisons.
3. Midpoints crossing zero, subnormal/normal and exponent boundaries must choose even codes; Task 1 `E4m3Midpoints`, Task 2 independent GPU fixtures.
4. Adding operation 3 must preserve operations 0/1/2 and reject invalid operations before writes/submission; Tasks 1/2 batch and probe validation tests.
5. New CLI mode must preserve complete four-byte output words, alias rejection, empty-file handling and failure publication behavior; Task 2 CPU/GPU CLI fixtures and oracle length checks.

## File structure

Modify existing focused files only:

- `src/NeuralRendering/Amd/NumericFormats.h/.cpp`: CPU encoder and batch dispatch.
- `tests/amd-nr/NumericFormatsTests.cpp`: generated mathematical CPU fixtures.
- `tests/amd-nr/NumericOracle.py`: independent FP8 distance oracle and shared CPU/GPU fixtures.
- `src/NeuralRendering/Amd/Shaders/FormatCodec.hlsl`: portable integer shader encoder.
- `src/NeuralRendering/Amd/FormatProbe.cpp`: fourth-operation validation.
- `tools/AmdNrFormatFiles.h`, `tools/AmdNrInspect.cpp`, `tools/AmdNrFormatProbe.cpp`: shared parsing and CLI help.
- `tests/amd-nr/FormatProbeTests.cpp`, `InspectorTests.py`, `FormatProbeCliTests.py`: GPU dispatch and file boundaries.
- `tests/amd-nr/README.md`: encoding contract, reproduction, measured results and limits.

Existing CTest registrations cover these changes; no new archive schema or test target is needed.

### Task 1: Source-owned CPU encoder and independent oracle

**Files:** Modify `NumericFormats.h/.cpp`, `NumericFormatsTests.cpp`, `NumericOracle.py`.

**Interfaces:**

- Produces: `std::uint8_t EncodeFloatToE4m3Rne(float value) noexcept;` in `TheosRenderPipeline::NeuralRendering::Amd`.
- Produces: append `FormatOperation::Float32ToE4m3` with numeric value 3; preserve existing values 0/1/2.
- Consumes/extends: `ConvertFormatWords(FormatOperation,std::span<const std::uint32_t>,std::span<std::uint32_t>) noexcept`. FP32 words encode to zero-extended low-byte codes in full `uint32_t` output words.
- Produces: independent Python `encode_e4m3_distance(words)` returning `np.uint32` output codes. Construct finite positive values mathematically from exponent/mantissa; find nearest by FP64 distance, break ties by even code, preserve sign and override NaNs. Do not reuse production bit quantization or the public AMD helper.
- Produces: `e4m3_encode_fixtures()` returning input/expected arrays. The existing `fixtures()` yields `f32-to-e4m3` only after Task 2 exposes that CLI mode.

- [x] **Step 1: Write failing CPU tests.** `E4m3SpecialValues` asserts ±0 -> `0x00/0x80`, ±Inf -> `0x7e/0xfe`, all sampled NaN signs/payloads -> `0x7f`, and ±448/overflow -> signed max. `E4m3DeepUnderflow` checks FP32 words `0x00000001`, `0x007fffff`, `0x00800000`, `0x1a996262` and their signed partners -> signed zero. `E4m3Midpoints` checks all 126 adjacent positive finite-code midpoints and neighboring FP32 values, both signs, against independently constructed values and tie parity; explicitly pin `1.0625 -> 0x38`, `1.1875 -> 0x3a`, `2^-10 -> 0`, and `432 -> 0x7e`. `E4m3Roundtrip` checks all 256 decoded FP8 codes, with only `0xff` canonicalizing to `0x7f`.
- [x] **Step 2: Add failing batch/oracle tests.** Assert enum values 0/1/2/3; check zero extension of outputs, in-place conversion, empty spans, and mismatched counts/invalid operations returning the existing error without writes. In Python construct all 65,536 mathematical half values promoted to FP32, 756 midpoint-neighbor words, literal deep-underflow/special words, and 100,000 random `uint32` words with seed 20261005. Expected codes use the distance oracle. Self-tests pin the literal anchors, negative signalling NaN canonicalization and deep-underflow signed zero.
- [x] **Step 3: Run red checks.** Build `TRPAmdNrNumericTests`: require the missing encoder/API compilation failure. Run `NumericOracle.py --self-test` against the new oracle tests before adding its implementation: require their intended failure. Retain outputs in the ignored build directory.
- [x] **Step 4: Implement the CPU encoder and distance oracle.** Bit-cast the FP32 input, handle NaNs first, saturate magnitude at `0x43e00000`, and return signed zero for magnitude at most `0x3a800000` (the nearest-even zero midpoint). Quantize remaining normal/subnormal E4M3 values with integer ties-to-even and carry handling. Every executed shift must have a valid count; the early underflow guard permits bounded shifts on the remaining finite domain. Extend the batch validator/switch. Implement the independently specified Python distance oracle without adding a vendor dependency.
- [x] **Step 5: Run green checks.** Build `TRPAmdNrNumericTests`; run CTest `-C Release -R '^(AmdNrNumericFormats|AmdNrOracleSelfTest|AmdNrOracleCpu)$' --output-on-failure`. Require all three passing. The independent encoder fixture is not dispatched through CLI until Task 2; existing CPU conversions must continue passing.
- [x] **Step 6: Commit only Task 1 source/tests.** Message: `feat: encode E4M3 with portable nearest-even rounding`.

### Task 2: Portable shader encoding and complete standalone comparisons

**Files:** Modify `FormatCodec.hlsl`, `FormatProbe.cpp`, shared tool parsing/help, GPU/CLI tests, `NumericOracle.py`, `tests/amd-nr/README.md` as listed above.

**Interfaces:**

- Consumes: Task 1 `FormatOperation::Float32ToE4m3`, CPU encoder/batch API and independent oracle fixtures.
- Produces: HLSL `uint FloatE4m3(uint bits)` with the same full-FP32 contract and a zero-extended byte result. Operation 3 selects it; operations 0/1/2 retain their established behavior.
- Produces: `--format f32-to-e4m3` on both existing tools via shared `ParseFormat`; input and output remain little-endian four-byte words. GPU mode continues requiring `--warp` or explicit `--adapter-luid HIGH:LOW`.
- Extends: `FormatProbe::Submit` accepts operation 3 before resource allocation; invalid values remain `ProbeError::InvalidOperation`.

- [x] **Step 1: Write failing GPU and CLI tests.** Add operation 3 to GPU comparisons, including counts 0/1/63/64/65, literal signed-zero/NaN/overflow/deep-underflow inputs, and existing fixed random words. Extend both CLI suites with `f32-to-e4m3`: source words `[0,0x80000000,0x3f800000,0x7f800000,0xff800001,0x9a996262]` must yield full words `[0,0x80,0x38,0x7e,0x7f,0x80]`. Repeat same-path/hardlink alias checks, empty input, a three-byte malformed input and a missing output directory; failures must preserve an existing destination's bytes.
- [x] **Step 2: Wire independent fixtures and run red checks.** Add `f32-to-e4m3` to `NumericOracle.fixtures()` using Task 1 input/expected arrays. Run the CPU/GPU CLI tests and `AmdNrOracleCpu` / `AmdNrOracleWarp`: expect rejection of the new mode. Direct GPU dispatch must also fail on the unaccepted operation before implementation.
- [x] **Step 3: Implement shader and tool support.** Use bounded integer quantization and the explicit special-value policy in HLSL; do not route through half conversion or native FP8. Accept enum value 3 in the host validator and add an explicit shader case while preserving 0/1/2. Extend shared parsing and both help strings. Reuse existing job resources/fences and checked file publication without redesign.
- [x] **Step 4: Run green checks.** Build the complete standalone suite; run CTest `-C Release --output-on-failure`. Require zero failures, exact output lengths and zero bit mismatches for the new independent CPU/WARP comparisons as well as existing operations. Report the unavailable Windows debug-layer test as a skip, without claiming resource/state validation. If a selected physical adapter is available, run the same independent oracle with its enumerated LUID and report its identity; this is portability evidence, not RDNA2/3/4 acceptance.
- [x] **Step 5: Record measured scope.** Update the README with the new mode, numerical policy, actual fixture counts/results and reproduction commands. Keep the public host-helper deep-underflow discrepancy as a research limitation; it is neither a product dependency nor a required test oracle. Preserve incomplete model/inference status and remaining matrix/spatial/graph/hardware gates.
- [x] **Step 6: Commit Task 2 files.** Message: `feat: verify portable GPU E4M3 output encoding`.

## Verification commands

Use the existing build; stop on a failed command and retain red/green outputs.

```powershell
$amdNrCmake = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
$amdNrCtest = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe'
& $amdNrCmake --build out/amd-nr-foundations --config Release
& $amdNrCtest --test-dir out/amd-nr-foundations -C Release --output-on-failure
```

For the Task 1 red build add `--target TRPAmdNrNumericTests`; run the oracle
self-tests directly with `C:\Python314\python.exe tests/amd-nr/NumericOracle.py --self-test`.
No test loads supplied binaries or trained data. Do not claim success from an
old output file or a command whose error was masked by a later shell command.

## Self-review and remaining work

Both tasks implement a bounded numerical-format subcomponent of the approved
baseline. Interfaces and enum values match between tasks; all five review-focus
conditions have explicit tests. Mathematical nearest-distance expectations are
independent of integer CPU/HLSL rounding. The plan preserves existing tool
selection, word layout, limits and lifecycle behavior.

After both tasks pass, use the preserved Native method's whole-branch independent
review gate before declaring implementation complete. This plan leaves residual
FMA execution, matrix reduction equivalence, spatial mappings, other operations,
whole inference, TRP integration and representative Radeon validation open.

## Completion evidence

Task 1 commit: `7c4b80c`. Task 2 commit: `c4dddd9`. Independent whole-branch
review of `246d152..c4dddd9`: no Critical or Important findings; one deferred
Minor concerns earlier README/design status paragraphs that still call the
implemented encoder pending. Current implementation/results are described in
the test README encoder section and research log.

All 166,314 independent FP8 encoding words match CPU, WARP and RTX 4080 SUPER;
all prior format fixtures also match. Radeon acceptance and D3D12 debug-layer
validation remain open. Full inference and TRP integration are not enabled.
