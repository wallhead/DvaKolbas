# AMD NR C512 GPU Reduction Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans with the preserved Native method. Steps use checkbox syntax for tracking.

**Goal:** Execute the recovered C512 2x2 half reduction in portable D3D12, preserving packed output padding and matching an independent oracle.

**Architecture:** A thin reduction probe reuses the existing private compute transport and conversion helpers. Each shader thread owns one complete packed output word, computes its four active bytes or preserves the snapshotted initial bytes, avoiding shared-byte UAV races. CPU reference and model status stay unchanged.

**Tech Stack:** C++23, strict cs_5_1 FXC, D3D12, CTest, NumPy.

**Spec:** [Approved engine design](../specs/2026-10-05-amd-nr-engine-design.md), operation/compute gates; [tensor contract](../../amd-nr-c512-tensor-layout.md), recovered rounding, crop and padding.

## Global Constraints

- "The product consists of source-owned C++, HLSL, logical schemas, and tests."
- "FP8 is encoded storage with explicit software decode/encode; FP16 storage and required rounding boundaries are explicit."
- "It must not invent operations to fill gaps."
- Keep branch `codex/amd-nr-engine`; preserve Native authorization. No supplied code execution, model enablement, game installation, merge or push.
- Source canonical raw half `[X,Y,channel]`, Y before X. Destination packed 4x4 order. Active extent is min(source/2,destination) independently per axis; preserve every inactive byte.
- Two pair sums, pair-sum addition, and quarter multiply each round to half. Final E4M3 storage uses existing nearest-even encoder. No intermediate NaN-payload equivalence claim is needed for canonical FP8 output.
- Bound source to 4096*512 half values and destination to 1024*512 FP8 bytes; immutable layouts still validate axes/budgets. Exact input/initial destination spans required before allocation.

## Review Focus

1. Packed word ownership and inverse coordinate mapping: all four lanes and non-square/padded/cropped extents must match independent forward packing, with no byte UAV races.
2. NaNs, infinities, negative zeros, subnormals and half-rounding boundaries: exhaustive half inputs and seeded four-half tuples plus literal assertions.
3. Padding must retain the caller's arbitrary initial bytes: 60x36 ->32x20 preserves 51,200 bytes; all output words including inactive regions must be stored.
4. Bad spans, oversized valid layouts and moved-from contexts fail before payload allocation; maximum accepted extents run successfully and existing format/projection tests stay green.
5. Snapshotting, timeout, dropped handles, probe moves and foreign jobs: dedicated GPU tests exercise ownership and successful readback/drain after releasing a queue gate.

## Files and interfaces

- Create `src/NeuralRendering/Amd/C512ReductionProbe.h/.cpp` with constants `C512ReductionMaxSourceValues=4096u*512u`, `C512ReductionMaxDestinationBytes=1024u*512u`.
- Class `C512ReductionProbe`: static `Create(ID3D12Device*,ID3D12CommandQueue*) -> expected<C512ReductionProbe,ProbeError>`; move-only; `Submit(const C512TensorLayout& source,const C512TensorLayout& destination,span<const uint16_t> canonicalInput,span<const uint8_t> initialPackedOutput) -> expected<FormatProbeJob,ProbeError>`; `Readback(FormatProbeJob&,milliseconds) -> expected<vector<uint8_t>,ProbeError>`; delegated `Drain(milliseconds)`.
- Consume existing `FormatProbe::CreateCompute` / `SubmitCompute`; add one friend, keep public format/projection APIs and two-root-constant ABI unchanged.
- Payload: four LE uint32 header words source W/H and destination W/H, then LE half input, then exact initial packed output. Constants output word count and source value count; outputWords/workItems=destinationBytes/4. Snapshot before submit. Use explicit byte packing/unpacking.
- Create `Shaders/C512Reduction.hlsl`, reuse `FormatConversions.hlsli`. One thread per output word, inverse packed coordinates per byte, conditional active computation. Store exactly one complete uint32 output word, preserving inactive bytes from payload.
- Modify `cmake/AmdNrFoundations.cmake`: generate `C512ReductionBytecode.h`, fixed valid symbol `C512Reduction`, same strict compiler flags/dependency tracking; add probe source.
- Create `tests/amd-nr/C512ReductionProbeTests.cpp`: GPU lifecycle/limits/word mapping and bounded test-only binary driver. Register WARP and debug-skip tests in `tests/amd-nr/CMakeLists.txt`.
- Create `tests/amd-nr/C512ReductionExecutionOracle.py`: mathematical forward packed address, NumPy explicit half rounds, nearest-distance encoder. Register WARP oracle when NumPy is present. Existing exhaustive CPU reduction oracle remains.
- Document results in `docs/amd-nr-c512-gpu-reduction.md`, research/standalone README appendices. Supplied/private evidence remains ignored.

### Task 1: Portable reduction and validation

- [x] Write missing-API GPU tests before product code; build and retain expected missing-header RED. Test 4x4 ->4x4 arbitrary initial bytes, non-square channel/pixel patterns, 60x36 ->32x20 preserved padding, crop, max 64x64 ->32x32, over-limit shapes, short input/output, moved-from context, wrong/foreign queues/jobs, post-submit mutations, repeated readbacks, dropped handles and queue-gate timeout/drain.
- [x] Add test-only driver `--oracle --warp INPUT OUTPUT` / `--oracle --adapter-luid LUID INPUT OUTPUT`, plus `--cpu` for oracle cross-check. Input header four LE dimensions then canonical half words and initial packed destination, exact bounded file length. Reject malformed fixtures before destination writes. CPU mode delegates existing reference.
- [x] Add independent oracle fixtures: all 65,536 half encodings plus 65,525 random four-half tuples and eleven literals at source64x16 ->dest32x8; seeded 60x36 ->32x20 padded fixture; crop16x12 ->4x4; asymmetric8x16 ->4x8; maximal64x64 ->32x32. Compare every packed byte, including padding; retain inputs/expected/actual/report in ignored output. Literal averages include negative zero, NaN, overflow and midpoint bytes.
- [x] Implement wrapper/header, word-owning shader and build registration. Invalid counts/limits return Count, moved context InvalidDevice; Readback propagates transport errors and maps allocation failure to Memory. No generic public shader API or second lifetime engine.
- [x] Run strict build and full suite via `local/amd-nr-re/Build-Foundations.ps1 -Configure`; expected no enabled failures, debug skipped77 if absent. Run physical oracle/lifecycle using a freshly enumerated explicit adapter LUID when available; zero byte mismatches, no Radeon claim.
- [x] Record actual totals/adapter identities/limits; commit. Complete task with full suite. Request fresh Native whole-branch review from246d152; fix Critical/Important once with RED/GREEN tests, defer Minors and ledger every declined scope. Preserve branch and ignored scratch (prior cleanup rejection).

## Self-review

One cohesive task, exact existing CPU arithmetic contract. Immutable layouts/caps keep payload, indices and dispatch within transport bounds. Per-word ownership avoids races when channels/tiles are interleaved. Every review focus has a test; model, WMMA, full graph, renderer and Radeon acceptance remain later gates. Native and user authorization are preserved.
