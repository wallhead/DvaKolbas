# AMD NR C512 Projection Codec Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a source-owned, bounded C512 projection weight decoder and expose its proven logical layout through the existing standalone inspector.

**Architecture:** Decode one packed record into owned canonical FP8 codes and FP16 coefficient bits. A separate archive adapter checks the known model identity and the recovered block/layer selection before decoding; the inspector prints only layout diagnostics. This stage implements storage indexing, while the research CPU arithmetic stays private until the remaining numerical contracts are closed.

**Tech Stack:** Existing C++23 core, CMake 3.21+, MSVC 2022 x64, Windows SDK BCrypt, existing Python CLI fixtures; no new product dependency.

**Spec:** Approved `docs/superpowers/specs/2026-10-05-amd-nr-engine-design.md`, components 1/2/5 and model/operation gates; exact recovered codec contract in `docs/amd-nr-c512-projection.md`.

## Global Constraints

- Branch: `codex/amd-nr-engine`.
- "The product will execute our C++ and compute shaders."
- "It will read weights selected by the user, without including the supplied weights, DLL, disassembly, decompiled source, or GPU objects in the package."
- "Unknown identities, unexpected record lengths, and incomplete schemas return an explicit unsupported-model result."
- "Require exact integer/layout results and exact storage bits where rounding is fully specified."
- "It must not invent operations to fill gaps."
- "Existing NVIDIA rendering and frame generation continue through their existing route."
- "The user selected all three proposed AMD generations: RDNA2, RDNA3, and RDNA4. These are implementation and validation targets, not established compatibility."
- Codec values: 512×512 FP8 codes; 512 little-endian FP16 words; exact record length 263,168 bytes; coefficient tail starts at 262,144.
- Output/input address bit positions are `(6,3,7,8,9,10,11,12,13)` / `(0,1,4,5,2,14,15,16,17)`; coefficient half-index positions are `(0,3,1,2,4,5,6,7,8)`.
- Canonical channel numbers are native WMMA fragment coordinates. Keep `KnownArchiveIncompleteSchema` and `inference=unavailable`; no projection arithmetic, full block, GPU dispatch, game integration, package, push or installation in this stage.
- Preserve the already selected Native execution method: execute in this session after plan review.

## Review Focus

- A truncated or extended record must fail before decoding or reading its tail — Task 1 `ExactRecordLength`.
- Low-bit channel swaps must not silently produce a plausible but incorrect matrix — Task 1 `ForwardAddressOracle` and `NonSymmetricPermutation`.
- FP8 NaNs/signed zeros and FP16 payloads must survive the storage codec without numeric canonicalization — Task 1 `RawStorageBits`.
- A decoded result must remain valid after the input buffer is changed, destroyed or its owner is moved — Task 1 `OwnedResultLifetime`.
- A valid-looking unknown archive or unsupported block/layer must not yield trusted schema output or an available inference status — Task 2 `UnknownArchive`, `SelectionBounds`, and CLI assertions.

## File structure and interfaces

All declarations use `TheosRenderPipeline::NeuralRendering::Amd` and include their own standard headers. Do not add the renderer PCH.

| File | Responsibility |
|---|---|
| `src/NeuralRendering/Amd/C512Projection.h/.cpp` | Exact raw-record storage codec and owning canonical result |
| `src/NeuralRendering/Amd/C512ProjectionArchive.h/.cpp` | Known identity and supported record selection |
| `tests/amd-nr/C512ProjectionTests.cpp` | Independent generated address, storage and ownership fixtures |
| `tests/amd-nr/C512ProjectionArchiveTests.cpp` | Selection and unknown identity rejection |
| `tools/AmdNrInspect.cpp` | Optional selected projection summary |
| `tests/amd-nr/InspectorTests.py` | Argument validation and unavailable status |
| `cmake/AmdNrFoundations.cmake`, `tests/amd-nr/CMakeLists.txt` | Build/test registration |
| `tests/amd-nr/README.md` | Scope, test commands and private-input manual check |

### Task 1: Owned canonical projection record decoder

**Files:** Create `C512Projection.h/.cpp` and `C512ProjectionTests.cpp` at the paths above. Modify `cmake/AmdNrFoundations.cmake`, `tests/amd-nr/CMakeLists.txt` and `tests/amd-nr/README.md`.

**Interfaces:**

- Consumes: `std::span<const std::byte>` containing a single raw record. No archive or identity claim belongs to this primitive.
- Produces: `enum class ProjectionError { RecordSize, Memory, UnsupportedArchive, UnsupportedSelection, Archive, Crypto };`.
- Produces: move-only `class C512ProjectionWeights`, with private `std::vector<std::uint8_t>` of 262,144 matrix codes and `std::array<std::uint16_t,512>` of coefficient bits; public `static constexpr std::size_t Channels=512;`.
- Produces: `std::span<const std::uint8_t> MatrixCodes() const & noexcept` and `std::span<const std::uint16_t> ResidualHalfBits() const & noexcept`; delete both `const &&` overloads. Matrix order is `n*512+k`; coefficient order is canonical `n`. Views live as long as the result owner or its move destination.
- Produces: `std::expected<C512ProjectionWeights,ProjectionError> DecodeC512Projection(std::span<const std::byte>);`.

- [ ] **Step 1: Write failing fixtures and register `TRPAmdNrProjectionTests` / CTest `AmdNrC512Projection`.** Use `TestSupport.h` assertions. `ExactRecordLength` rejects lengths 0, 1, 262143, 262144, 263167 and 263169 as `RecordSize`. `ForwardAddressOracle` independently constructs the forward load addresses from the projection report for every `(n,k)`; its generated byte pattern must decode exactly at all 262,144 coordinates and cover the full bank once. Do not compute expected offsets using the implementation's deposit function. Repeat with four byte-pattern passes `(physicalOffset >> shift)&255`, shifts 0/5/10/15, so higher address bits are exercised.
- [ ] **Step 2: Add the remaining failing fixtures.** `NonSymmetricPermutation` packs a generated matrix with code `0x38` at `((37*k+11)%512,k)` and verifies every canonical code after decoding. `RawStorageBits` places all 256 FP8 codes in repeated logical rows and sets coefficient bits to `0x8000`, `0x0001`, `0x7c00`, `0x7e01`, `0xfc01` and a coordinate-dependent pattern; assert raw bits exactly, using an independent thread/tile coefficient oracle. `OwnedResultLifetime` modifies and destroys input, moves the decoded owner, and checks its data; compile-time assertions forbid copying and rvalue getter calls.
- [ ] **Step 3: Run the red check.** Configure the existing standalone test build and build `TRPAmdNrProjectionTests`. Expect compilation failure for the missing codec header/API; retain the output. Once the API exists, each fixture must also exercise its intended assertion rather than an incidental setup failure.
- [ ] **Step 4: Implement the codec.** Check length before allocation or loads. Build the owned row-major code bank using the proven deposit map; read the 512 tail words explicitly as little-endian bits. Catch allocation failure as `Memory`. Do not float-convert the stored codes or coefficients. Register the source in `TRPAmdNrCore` and document the limited codec scope.
- [ ] **Step 5: Run the green check.** Build `TRPAmdNrProjectionTests`; run CTest `-C Release -R '^AmdNrC512Projection$' --output-on-failure`. Require exit 0 and all exhaustive/storage/lifetime assertions passing.
- [ ] **Step 6: Commit only this task's source, tests, build files and README.** Message: `feat: decode canonical C512 projection weight records`. Preserve the separate branch and ignored reference inputs.

### Task 2: Known archive adapter and standalone diagnostic

**Files:** Create `C512ProjectionArchive.h/.cpp` and `C512ProjectionArchiveTests.cpp`. Modify `cmake/AmdNrFoundations.cmake`, `tests/amd-nr/CMakeLists.txt`, `tools/AmdNrInspect.cpp`, `tests/amd-nr/InspectorTests.py` and `tests/amd-nr/README.md`.

**Interfaces:**

- Consumes: Task 1 `DecodeC512Projection` and `C512ProjectionWeights`; existing `IdentifyModel(const ArchiveFile&)` and `RecordBytes(const ArchiveFile&,std::size_t)`.
- Produces: `bool IsC512ProjectionSelection(std::uint32_t block,std::uint32_t layer) noexcept;`, true only for blocks `[23,30]` or `[40,47]` with layer 1 or 3.
- Produces: `std::expected<C512ProjectionWeights,ProjectionError> ReadKnownC512Projection(const ArchiveFile&,std::uint32_t block,std::uint32_t layer);`. Validate selection, check identity, find exact `block{block}.layer{layer}.layer`, then decode its checked span. Return `UnsupportedArchive` for an unknown digest; map archive/crypto/memory failures explicitly. Preserve Task 1's `RecordSize` for an incorrect selected payload.
- Produces: Inspector pair `--projection-block U32 --projection-layer U32` in existing weights/geometry mode. Both must appear together, and are forbidden in format mode. On success append `projection_record=<name>`, `projection_basis=native-fragment`, `projection_matrix_codes=262144`, `projection_residual_halves=512`; print no weights or parameter values. Preserve existing model status and unavailable inference.

- [ ] **Step 1: Write failing adapter and CLI tests.** Register `TRPAmdNrProjectionArchiveTests` / `AmdNrC512ProjectionArchive`. `SelectionBounds` checks every block 0–71, layers 0–4, plus UINT32_MAX: exactly 32 pairs are supported. `UnknownArchive` creates a structurally valid synthetic archive with a supported name and exact-sized payload via `MakeArchive`/`ReadArchive`, then requires `UnsupportedArchive`; invalid selection returns `UnsupportedSelection`. CLI cases cover a missing pair member, duplicate option, non-integer and overflowing values, blocks 22/31/39/48, layer 2, and mixing projection options with format mode (exit 2). A valid-looking unknown fixture with both supported options retains exit 3, `UnsupportedArchive`, `inference=unavailable`, and no success-only `projection_matrix_codes=` field.
- [ ] **Step 2: Run the red check.** Build the new target and run `AmdNrInspector`; expect missing adapter API and rejection of the new valid options rather than the requested unknown-archive result.
- [ ] **Step 3: Implement the adapter and CLI.** Validate pair and supported numeric selection before reading a file. Match the known SHA-256 through the existing identity API; never trust record name/size alone. Find the selected record by exact name, not directory position. Keep errors distinct. In the CLI, return the existing unsupported status for unknown archives before decoding; internal adapter/codec failures use exit 2. No new ready status or frame-evaluation entry point.
- [ ] **Step 4: Run green checks.** Build standalone tools/tests and run CTest `-C Release -R '^(AmdNrC512Projection|AmdNrC512ProjectionArchive|AmdNrArchive|AmdNrIdentity|AmdNrInspector)$' --output-on-failure`. Require all five tests passing. The existing broader foundations suite should still report zero failures; report a missing debug-layer skip separately.
- [ ] **Step 5: Run the private positive check and document scope.** Use the ignored known weights with all 32 supported block/layer pairs at 1920×1080. Each invocation must return 0, the exact selected name and dimensions, `KnownArchiveIncompleteSchema` and `inference=unavailable`. No dump or output file is created. Bind this manual result to the known digest in the README; these trained inputs remain outside automated/public fixtures. Report missing local inputs rather than fabricating a positive result.
- [ ] **Step 6: Commit only this task's source, tests, build files and README.** Message: `feat: inspect verified C512 projection layouts`.

## Verification commands

Use the existing `out/amd-nr-foundations` build. Exact executable paths on this workstation:

```powershell
$amdNrCmake = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
$amdNrCtest = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe'
& $amdNrCmake -S tests/amd-nr -B out/amd-nr-foundations -G 'Visual Studio 17 2022' -A x64 -DPython3_EXECUTABLE=C:/Python314/python.exe
& $amdNrCmake --build out/amd-nr-foundations --config Release
& $amdNrCtest --test-dir out/amd-nr-foundations -C Release --output-on-failure
```

For red checks, add `--target TRPAmdNrProjectionTests` or
`--target TRPAmdNrProjectionArchiveTests` to the build command. Stop on a failed
command; never commit on an assumed result.

## Self-review and remaining work

The two tasks cover owned logical record decoding and guarded inspection from
components 1/2/5 of the approved engine spec. All five review-focus conditions
have named tests; the forward address oracle is independent of the decoder.
No public fixture embeds trained parameters, AMD calculator code or extracted
GPU instructions. Header signatures, ownership and CLI behavior are fixed above.

This is a storage-codec sub-plan, not a complete network plan. The CPU research
probe demonstrates the half-storage boundary on exact fixtures; unknown WMMA
dot order, spatial channel conventions, residual application and FP8 output
prevent a production arithmetic equivalence claim. Recover those next, then
implement the operation reference and portable GPU comparison. Other blocks,
whole graph, temporal inputs, integration and RDNA2/3/4 acceptance remain required
for the original objective.
