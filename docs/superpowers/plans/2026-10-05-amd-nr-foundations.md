# AMD NR Foundations Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a source-owned model inspector and verified CPU/D3D12 numerical formats that form the first working subsystem of TRP's AMD NR engine.

**Architecture:** A standalone C++23 core owns archive parsing, model identification, floating-point formats, and geometry. Small CPU and D3D12 executables exercise those contracts without loading the supplied DLL or initializing Skyrim. Full inference and TRP host integration follow as separate plans after their numerical contracts are recovered; passing this plan does not establish working NR.

**Tech Stack:** C++23, CMake 3.21+, MSVC 2022 x64, Windows SDK BCrypt/D3D12/DXGI/D3DCompiler, Shader Model 5.1. Python 3.14 and NumPy are optional differential-test tools, not product dependencies.

**Spec:** `docs/superpowers/specs/2026-10-05-amd-nr-engine-design.md`, approved 2026-10-05. See `docs/amd-nr-research.md` and the ignored local evidence directory for recovered contracts.

## Global Constraints

- Branch: `codex/amd-nr-engine`; preserve the user's request for a separate branch.
- Target generations: RDNA2, RDNA3, and RDNA4; no compatibility claim follows from a compile or WARP test.
- The product executes our C++ and compute shaders and reads user-selected weights.
- Do not include the supplied weights, DLL, disassembly, decompiled source, or GPU objects in the package.
- Unknown identities, unexpected record lengths, and incomplete schemas return an explicit unsupported-model result.
- Require exact integer/layout results and exact storage bits where rounding is fully specified.
- Do not invent operations to fill gaps in the recovered model contract.
- Existing NVIDIA rendering and frame generation continue through their existing route.
- The first game route remains one NR pass after the active upscaler, with native input resolution and HUD composition; this foundations plan does not yet enable that route.
- No new product dependency, proxy DLL loading, game launch, package, installation, or push is needed for this stage.

## Review Focus

- Truncated, overlapping, duplicate-name, and overflowing archives must fail before payload interpretation or GPU upload — Task 1.
- A structurally valid modified or unknown archive must not acquire a known model identity or become inference-ready — Task 4.
- NaN payloads/signs, negative zero, halfway values, and subnormals must retain the documented storage-bit behavior — Tasks 2 and 5.
- Non-square and overflow-prone extents must preserve the height adjustment and reject arithmetic overflow — Task 3.
- File/CLI/GPU failures must produce an error rather than successful-looking output, and GPU-visible allocations must survive until fence retirement — Tasks 4–6.

## Stage boundaries

1. **This plan:** archive reader, numerical formats, geometry, known-archive identification, inspector CLI, and D3D12 format probe. Independently usable and testable.
2. **Network plan:** recover and implement all internal record views, operation formulas, FP8 encoding, noise/history/exposure, layouts, CPU graph, GPU kernels, memory planner, and standalone image evaluator. A complete 71-block graph cannot be planned as exact implementation steps until those mathematical contracts are closed. This work remains required for the user's original objective.
3. **Integration plan:** extract the vendor-independent presentation/session boundary, connect the validated engine, preserve the current upscaler and UI, validate failure/retirement/reset behavior, and test representative RDNA2/3/4 hardware. This work also remains required.

The foundations API must not expose `EvaluateFrame`, fake block outputs, or a ready/available inference status. A known archive is explicitly labelled **incomplete schema** until the network plan supplies a validated schema.

## File structure

All core declarations live in `TheosRenderPipeline::NeuralRendering::Amd` and include their own standard headers rather than the renderer PCH.

| File | Responsibility |
|---|---|
| `src/NeuralRendering/Amd/WeightArchive.h/.cpp` | Bounded byte parser, directory records, owned file snapshot |
| `src/NeuralRendering/Amd/NumericFormats.h/.cpp` | FP16 conversion and E4M3 decode |
| `src/NeuralRendering/Amd/ModelGeometry.h/.cpp` | Checked extent policy and six spatial levels |
| `src/NeuralRendering/Amd/ModelIdentity.h/.cpp` | SHA-256, exact known archive metadata, incomplete-schema status |
| `src/NeuralRendering/Amd/KnownModelRecords.inc` | Only the 153 record names and lengths, generated from local metadata |
| `src/NeuralRendering/Amd/FormatCodec.hlsl` | Explicit integer numerical conversion kernels |
| `src/NeuralRendering/Amd/FormatProbe.h/.cpp` | Standalone D3D12 format dispatch and fence-owned jobs |
| `tools/AmdNrInspect.cpp` | Model/extent inspector CLI and CPU numerical batch mode |
| `tools/AmdNrFormatProbe.cpp` | Manual adapter-selectable GPU numerical probe |
| `tools/CompileAmdNrShaders.cpp` | Build-time compilation of only the AMD format kernels |
| `cmake/AmdNrFoundations.cmake` | Reusable core target and generated bytecode dependency |
| `tests/amd-nr/CMakeLists.txt` | Independent test/tool build, no SKSE/NGX/Streamline |
| `tests/amd-nr/TestSupport.h` | Test assertions and synthetic archive fixture writer |
| `tests/amd-nr/WeightArchiveTests.cpp` | Parsing and immutable-view failures |
| `tests/amd-nr/NumericFormatsTests.cpp` | Independent numerical anchors and rounding boundaries |
| `tests/amd-nr/ModelGeometryTests.cpp` | Reference geometry policies and overflow failures |
| `tests/amd-nr/ModelIdentityTests.cpp` | Digest, identity, and incomplete-schema tests |
| `tests/amd-nr/InspectorTests.py` | CLI results, malformed files, and batch-I/O failures |
| `tests/amd-nr/NumericOracle.py` | Independent NumPy differential checks; optional manual validation |
| `tests/amd-nr/FormatProbeTests.cpp` | WARP storage-bit comparisons and D3D12 validation |
| `tests/amd-nr/README.md` | Reproduction commands and precise stage limitations |

The top-level plugin build and runtime sources remain untouched in this plan. A standalone test CMake project includes `cmake/AmdNrFoundations.cmake`; the network/integration plans decide when to link the core into the plugin.

## Shared build commands

Use the installed CMake/CTest binaries:

```powershell
$amdNrCmake = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
$amdNrCTest = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe'
& $amdNrCmake -S tests/amd-nr -B out/amd-nr-foundations -G 'Visual Studio 17 2022' -A x64
& $amdNrCmake --build out/amd-nr-foundations --config Release --parallel 2
& $amdNrCTest --test-dir out/amd-nr-foundations -C Release --output-on-failure --no-tests=error
```

Each task adds its executable/CTest registration to the same test project. Use the test names below with `-R` for the focused red/green cycle. A first red result may be a compile failure caused by the missing declared interface; preserve the diagnostic before adding the implementation. Any test must actually execute in Release builds: use explicit failure functions, not disabled `assert` calls.

### Task 1: Checked archive reader

**Files:** Create `WeightArchive.h/.cpp`, `TestSupport.h`, `WeightArchiveTests.cpp`, `tests/amd-nr/CMakeLists.txt`, and `cmake/AmdNrFoundations.cmake`.

**Interfaces:**
- Produce `ArchiveRecord { std::string name; std::uint64_t offset, size; }` and `ArchiveIndex { std::uint32_t payloadStart; std::uint64_t fileSize; std::vector<ArchiveRecord> records; }`.
- Produce `ArchiveLimits { std::uint64_t maxBytes = 268435456; std::uint32_t maxRecords = 4096; }`, explicit parser/file error codes, and `std::expected<ArchiveIndex, ArchiveError> ParseArchive(std::span<const std::byte>, ArchiveLimits = {})`.
- Produce move-only `ArchiveFile`, owning immutable bytes and their index; `std::expected<ArchiveFile, ArchiveError> ReadArchive(const std::filesystem::path&, ArchiveLimits = {})`; `std::expected<std::span<const std::byte>, ArchiveError> RecordBytes(const ArchiveFile&, std::size_t recordIndex)`.
- `ArchiveFile::Bytes() const noexcept -> std::span<const std::byte>` and `ArchiveFile::Index() const noexcept -> const ArchiveIndex&` provide immutable access for identification/inspection. No public operation replaces the bytes while record spans exist.
- Synthetic fixture helper: `MakeArchive(std::span<const TestRecord>) -> std::vector<std::byte>`, with independently written little-endian directory fields. It never reads proprietary data.

- [ ] **Step 1:** Write `ValidTwoRecordArchive`, asserting names `first/second`, lengths `3/2`, offsets `0/3`, payload start `61`, and EOF `66`. Add `TruncationAtEveryByte`, `DuplicateName`, `EmptyName`, `InvalidNameBytes`, `ZeroRecordLength`, `Overlap`, `Gap`, `TrailingBytes`, `PayloadOffsetBeforeDirectory`, `RecordRangeOverflow`, `CountBudget`, `ByteBudget`, and `RecordIndexOutOfRange`. Invalid names include NUL/control/non-ASCII bytes; do not interpret names as filesystem paths. Check owned record spans still point into the retained file after moving its owner.

  Core assertions for the valid fixture:
  ```cpp
  Require(index.payloadStart == 61 && index.fileSize == 66, "absolute payload start and exact EOF");
  Require(index.records.size() == 2 && index.records[1].offset == 3, "record directory accounting");
  ```
- [ ] **Step 2:** Configure/build and run `AmdNrArchive`; verify the missing parser/interface causes the red result.
- [ ] **Step 3:** Implement bounded parsing of `DLSSNRW1`, the count and absolute payload start, and all directory entries. Use subtraction-based range checks before additions/casts, reject zero counts/lengths, require exact directory closure and contiguous payload coverage regardless of directory ordering, and verify exact EOF. Read one bounded file snapshot and parse/hash that snapshot later; never retain a span into an input temporary. Catch allocation/I/O failure at the owning boundary and return an error.
- [ ] **Step 4:** Run `AmdNrArchive`; require all malformed fixtures to fail and valid/move-lifetime fixtures to pass. Add a deterministic directory mutation sweep without allocating from untrusted lengths.
- [ ] **Step 5:** Commit this tested reader and its standalone build target: `feat: add checked AMD NR weights archive reader`.

### Task 2: Source-owned numerical formats

**Files:** Create `NumericFormats.h/.cpp` and `NumericFormatsTests.cpp`; extend the test project.

**Interfaces:**
- Produce `std::uint16_t FloatToHalfRne(float) noexcept`, `float HalfToFloat(std::uint16_t) noexcept`, and `std::uint16_t DecodeE4m3ToHalf(std::uint8_t) noexcept`.
- Produce `std::expected<void, FormatError> ConvertFormatWords(FormatOperation, std::span<const std::uint32_t>, std::span<std::uint32_t>)`, with operations `Float32ToHalf`, `HalfToFloat32`, and `E4m3ToHalf`. Inputs/outputs are raw storage bits, one result per 32-bit word; invalid operation or unequal counts returns an explicit error instead of writing partial output. Unused high input bits are ignored for byte/half operations; unused high output bits are zero. Empty equal-length spans are valid and perform no writes.

- [ ] **Step 1:** Write `E4m3Anchors`, asserting `00→0000`, `01→1800`, `07→2300`, `08→2400`, `38→3c00`, `7e→5f00`, `7f→7e00`, `80→8000`, `b8→bc00`, `fe→df00`, and `ff→7e00`. Compare all 256 codes with an independent double-precision mathematical decoder; test sign symmetry excluding NaNs. Write `HalfBoundaryRounding` for both signs, normal/subnormal boundary, zero/subnormal midpoint, mantissa carry, overflow, infinities, and signalling/quiet NaN payloads.

  ```cpp
  Require(DecodeE4m3ToHalf(0x7f) == 0x7e00 && DecodeE4m3ToHalf(0xff) == 0x7e00, "positive quiet NaN");
  Require(FloatToHalfRne(-0.0f) == 0x8000 && DecodeE4m3ToHalf(0x01) == 0x1800, "signed zero and subnormal");
  ```
- [ ] **Step 2:** Build/run `AmdNrNumericFormats`; record the red failure.
- [ ] **Step 3:** Implement using `std::bit_cast` and integer operations for storage conversions. Preserve signed zero; round to nearest ties-to-even; quiet FP32 NaNs while preserving sign and retained payload for FP16 conversion. Half-to-float expansion preserves NaN sign/payload and signalling status, as recovered at RVA `0x4f4c0`; it does not quiet signalling half NaNs. E4M3's two NaN codes both return positive `0x7e00`. Do not add an FP8 encoder before its clipping/rounding contract is recovered.
- [ ] **Step 4:** Run `AmdNrNumericFormats`. Independently compute values for all 65,536 half encodings using exponent/significand arithmetic and compare `HalfToFloat`; test finite round-trips and adjacent-half midpoint/tie neighborhoods. Verify batch errors cannot alter the destination. Add no lookup table copied from the DLL.
- [ ] **Step 5:** Commit: `feat: implement AMD NR numerical storage formats`.

### Task 3: Checked processing geometry

**Files:** Create `ModelGeometry.h/.cpp` and `ModelGeometryTests.cpp`; extend the test project.

**Interfaces:**
- Produce `Extent { std::uint32_t width, height; }`, `ExtentMode { Default, Step8, Step128 }`, `LevelDimensions { Extent extent; std::uint32_t channels; }`, and `ModelGeometry { Extent input, processing; std::array<LevelDimensions, 6> levels; }`.
- Produce `std::expected<ModelGeometry, GeometryError> MakeGeometry(Extent input, ExtentMode = ExtentMode::Default, bool disableExtraHeight = false)`.

- [ ] **Step 1:** Write `DefaultExtentContracts` for `1280×720→1280×832/deep20×16`, `1600×900→1600×960/deep28×16`, `1920×1080→1920×1088/deep32×20`, `2560×1440→2560×1472/deep40×24`, and `3840×2160→3840×2176/deep60×36`. Assert `1024×1024→1024×1088`, disabling the extra yields `1024×1024`, small input reaches the 320 minimum, and the channel sequence is exactly `32,64,128,256,512,1024`.

  ```cpp
  const auto g = MakeGeometry({1920, 1080}).value();
  Require(g.processing.width == 1920 && g.processing.height == 1088, "reference padding");
  Require(g.levels[5].extent.width == 32 && g.levels[5].extent.height == 20, "deepest grid");
  ```
- [ ] **Step 2:** Add `Step8And128`, `NonSquareLevels`, `ZeroDimension`, `InvalidMode`, and `DimensionOverflow` (`UINT32_MAX`, values overflowing a signed reference extent, and rounding carry). Build/run `AmdNrGeometry` and verify red.
- [ ] **Step 3:** Implement checked 64-bit rounding before narrowing. Default: ceil64/min320 and +64 to height when both padded dimensions are multiples of 256. Other modes: ceil8/ceil128 without minimum/extra. First level halves the processing extent; later default/Step8 levels ceil-half then ceil4, while Step128 levels halve exactly. Reject invalid enums or extents whose processing dimensions cannot be represented as positive signed 32-bit reference dimensions.
- [ ] **Step 4:** Run `AmdNrGeometry`; compare every field against the local `numeric-contract.json` geometry examples without adding that private artifact as a build/runtime dependency.
- [ ] **Step 5:** Commit: `feat: add checked AMD NR processing geometry`.

### Task 4: Known archive identity and useful inspector

**Files:** Create `ModelIdentity.h/.cpp`, `KnownModelRecords.inc`, `tools/AmdNrInspect.cpp`, `ModelIdentityTests.cpp`, and `InspectorTests.py`; extend the reusable/test CMake files.

**Interfaces:**
- Consume Task 1's immutable `ArchiveFile` and Task 3's `MakeGeometry`.
- Produce `using Sha256Digest = std::array<std::byte, 32>` and `std::expected<Sha256Digest, IdentityError> Sha256(std::span<const std::byte>)` using BCrypt with RAII handles.
- Produce `ModelStatus { UnsupportedArchive, KnownArchiveIncompleteSchema }`, `ModelIdentification { ModelStatus status; Sha256Digest digest; std::string diagnostic; }`, and `std::expected<ModelIdentification, IdentityError> IdentifyModel(const ArchiveFile&)`. It has no inference-ready value.
- Build `TRPAmdNrInspect`. Inspection syntax: `--weights <path> --width <u32> --height <u32> [--mode default|8|128] [--no-extra-height] [--list]`. Numerical syntax: `--format <f32-to-f16|f16-to-f32|e4m3-to-f16> --input <path> --output <path>`; input and output are little-endian 32-bit storage words.

- [ ] **Step 1:** Write digest fixtures for empty input and ASCII `abc` against standard SHA-256 values. Write identity tests that a valid synthetic archive remains unsupported and a record-name/length-only match cannot bypass the digest. Inspector tests cover missing/unreadable files, paths with spaces, missing/duplicate/unknown CLI options, negative/zero/overflowing dimensions, invalid mode, malformed archive, non-multiple-of-four format input, output write failure, and input/output aliases.

  ```cpp
  Require(IdentifyModel(synthetic).value().status == ModelStatus::UnsupportedArchive,
      "structural validity does not identify a model");
  ```
- [ ] **Step 2:** Build/run `AmdNrIdentity|AmdNrInspector`; verify red before implementing identity and CLI behavior.
- [ ] **Step 3:** Bind the known identity to size `147689451`, count `153`, payload start `5673`, SHA-256 `6bf8dc931ef3ccffe18c82de26ab374156e7f19539ffcf8eabaa25dca5cf15ab`, and all 153 exact names/lengths from the local directory metadata. Commit only names/lengths, not payload bytes or trained coefficients. Hash the owned snapshot already parsed; use chunked BCrypt input and checked cleanup/error paths.
- [ ] **Step 4:** Implement the CLI with checked `from_chars` parsing, no renderer initialization, and no DLL loading. Known inspection returns `0` and prints `status=KnownArchiveIncompleteSchema`, `records=153`, processing/deep dimensions, and `inference=unavailable`. Unknown models return `3`; invalid arguments/I/O/malformed files return `2`; usage success returns `0`. Format mode bounds input to 256 MiB before reading and returns `0` only after a complete conversion/output; refuse identical/equivalent input/output paths and publish a complete output file using a temporary sibling. Never truncate the input or original weights.
- [ ] **Step 5:** Run identity/CLI tests. Manually run the inspector on the user-supplied weights at `1920×1080` and verify the exact size/digest/count, processing `1920×1088`, deepest `32×20`, and incomplete-schema status. Confirm original input hashes are unchanged.
- [ ] **Step 6:** Commit: `feat: add AMD NR model inspector and exact archive identity`.

### Task 5: Portable D3D12 numerical probe

**Files:** Create `FormatCodec.hlsl`, `FormatProbe.h/.cpp`, `tools/AmdNrFormatProbe.cpp`, `tools/CompileAmdNrShaders.cpp`, and `FormatProbeTests.cpp`; extend the CMake files.

**Interfaces:**
- Consume Task 2's `FormatOperation` and CPU word-conversion contract.
- Produce move-only `FormatProbe`, initialized with an explicit D3D12 device and DIRECT queue belonging to that device; return an explicit error for mismatched devices/capability failures.
- Produce `std::expected<FormatProbeJob, ProbeError> Submit(FormatOperation, std::span<const std::uint32_t>)` and `std::expected<std::vector<std::uint32_t>, ProbeError> Readback(FormatProbeJob&, std::chrono::milliseconds timeout)`. Jobs own upload/default/readback buffers, allocator, command list, and completion fence. Timeout does not release in-flight resources.
- Produce `std::expected<void, ProbeError> FormatProbe::Drain(std::chrono::milliseconds timeout)`. Failed shutdown transfers the complete device/queue/job state to a process-lifetime retained owner instead of destructing in-flight allocations. A zero-word submit produces a completed empty job without zero-sized buffers or GPU dispatch; counts beyond `65535 * 64` words return an explicit count error for this probe's one-dimensional dispatch.
- Build `TRPAmdNrFormatProbe --warp|--adapter-luid <high-hex>:<low-hex> --format <operation> --input <path> --output <path>`. No implicit first-adapter choice. WARP is the automatic CTest route; physical adapters are manual probes.

- [ ] **Step 1:** Write `GpuFormatStorageBits` comparing shader outputs for all 256 E4M3 codes, all 65,536 half bit patterns, the CPU boundary fixtures, and non-multiple-of-threadgroup counts. Add empty-submit, invalid operation/count, missing adapter LUID, wrong-device queue, and repeated submitted-job cases. Inspect D3D12 validation messages and fail on resource/state/lifetime warnings or errors.

  ```cpp
  const std::array<std::uint32_t, 4> input{0x00, 0x01, 0x80, 0xff};
  auto job = probe.Submit(FormatOperation::E4m3ToHalf, input).value();
  const auto got = probe.Readback(job, std::chrono::seconds(5)).value();
  Require(got == std::vector<std::uint32_t>{0, 0x1800, 0x8000, 0x7e00}, "GPU storage bits");
  ```
- [ ] **Step 2:** Configure/build and run `AmdNrGpuFormats` with WARP. Record the missing probe/shader red result; absence of WARP/debug tools is an unavailable test, not a pass.
- [ ] **Step 3:** Implement byte-addressed input/output words and explicit integer format conversions in `cs_5_1`, with 64 threads per group. Compile at build time with strictness/IEEE settings and publish only complete generated bytecode. HLSL does not use `min16float` to enforce precision or architecture-specific instructions. Bounds-check the dispatch tail and define NaN/sign handling in storage bits. No per-frame shader compilation.
- [ ] **Step 4:** Implement the probe with checked allocation/dispatch sizes, explicit COPY/UAV barriers, one submit, completion signal, and resource-owning jobs. Keep timed-out/unretired jobs in probe-owned storage; use checked shutdown drain and retain allocations rather than release GPU-visible storage on failed retirement. Fence failure injection tests must exercise this ownership policy without leaving actual GPU work hung. The CLI shares Task 4's format-file validation rules.
- [ ] **Step 5:** Run `AmdNrGpuFormats` and manual format probes on each available local adapter by LUID. Record vendor/device/LUID/driver, result count, storage-bit mismatches, and debug-layer status. Label local RTX/integrated-AMD results separately from outstanding RDNA2/3/4 acceptance.
- [ ] **Step 6:** Commit: `feat: add portable D3D12 AMD NR numerical probe`.

### Task 6: Differential verification and handoff to network recovery

**Files:** Create `NumericOracle.py` and `tests/amd-nr/README.md`; update the research prose and this plan's checkboxes as tasks pass.

**Interfaces:**
- Consume the CPU/GPU tools' numerical batch modes; Python generates inputs and expected outputs independently using NumPy, then compares actual output files. It never imports implementation formulas or a proprietary runtime.
- Produce a local comparison report including device identity, counts, mismatches, and unavailable hardware; reports and binary input/output fixtures stay in `out/amd-nr-foundations`.
- Produce `compare_words(actual: bytes, expected: bytes) -> None`, raising `ValueError` on malformed byte counts, unequal lengths, or any storage-word mismatch; no partial prefix can count as a match.
- Oracle CLI: `--self-test` runs malformed-output tests only; `--cpu <inspector-exe> [--gpu <probe-exe> (--warp|--adapter-luid <high>:<low>)] --work-dir <directory>` generates fixtures, invokes the tools, and validates complete outputs.

- [ ] **Step 1:** Write `NumericOracle.py` checks that truncated or missing output fails, so a partial conversion cannot count as success. Use RNG seed `12345`, 50,000 random FP32 storage words, the host-probe exponent/mantissa boundary neighborhoods, all E4M3/half codes, and explicit NaN quieting normalization. Compare signed zero and finite values bitwise rather than by float equality.

  ```python
  with self.assertRaises(ValueError):
      compare_words(b"", b"\x00\x00\x00\x00")
  with self.assertRaises(ValueError):
      compare_words(b"\x00\x80\x00\x00", b"\x00\x00\x00\x00")
  ```
- [ ] **Step 2:** Run focused failure fixtures against a deliberately truncated/missing output and confirm failure, then run the real CPU and available GPU conversions and require zero unexplained storage-bit mismatches. Retain failures as diagnostics; do not loosen precision to make them pass.

  ```powershell
  C:\Python314\python.exe tests/amd-nr/NumericOracle.py --self-test
  C:\Python314\python.exe tests/amd-nr/NumericOracle.py --cpu out/amd-nr-foundations/Release/TRPAmdNrInspect.exe --gpu out/amd-nr-foundations/Release/TRPAmdNrFormatProbe.exe --warp --work-dir out/amd-nr-foundations/oracle
  ```
- [ ] **Step 3:** Run the complete standalone CTest suite once after the final code changes and `git diff --check`. Recheck the supplied input hashes and the branch identity. Confirm no supplied/extracted binaries, payload data, local decompilation, or generated shader blobs are staged.
- [ ] **Step 4:** Document exact build/manual commands, known archive status, numerical coverage, and hardware gaps. Mark this stage complete only if the inspector and real compute-format probe work; do not state that the full NR graph is implemented.
- [ ] **Step 5:** Prepare the next written network plan only after internal schemas, FP8 encode, window/skip layout, and input/noise/history/output contracts have explicit evidence. Continue static RE in parallel with foundation execution, but do not make unverified operations callable in production. The next plan must account for all 153 records and all 71 blocks.
- [ ] **Step 6:** Commit the verified handoff: `test: verify AMD NR foundations against independent numerical oracles`.

## Self-review and execution handoff

This plan implements the archive, identity, explicit-format, geometry, and standalone numerical pieces of the approved design. Full inference, resident graph planning, frame codecs, presentation/session extraction, UI/reset/failure integration, and physical RDNA2/3/4 acceptance are deliberately assigned to the subsequent network/integration plans; they remain necessary before the original task is complete.

Every consumed interface above is produced by an earlier task. The five review-focus classes map to named tests. The known archive never becomes inference-ready simply because its digest matches. Default geometry preserves the recovered height adjustment, and no FP8 encoder/noise formula is assumed. Tests use synthetic fixtures or local inputs and cannot distribute the supplied model.

Recommended execution: **Native**, implementing tasks in this chat with focused red/green cycles and one independent review at the end. The tasks share sequential numerical and archive interfaces, so fresh implementation agents would repeatedly need the same evidence. The user must review this written plan and select the execution method before product implementation begins, as required by the installed planning skills.
