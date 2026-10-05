# AMD NR C512 Tensor Primitives Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans with the preserved Native method. Steps use checkbox syntax for tracking.

**Goal:** Add checked, source-owned C512 storage conversion and a CPU reference for the recovered 2x2 half reduction.

**Architecture:** A validated immutable layout describes positive aligned extents and checked byte counts. Caller-provided spans avoid hidden allocation. Conversion preserves FP8 codes; reduction consumes canonical raw FP16 values and writes only the active region of a packed destination, preserving padding.

**Tech Stack:** C++23, CMake/CTest, Python/NumPy independent numerical oracle.

**Spec:** [Approved engine design](../specs/2026-10-05-amd-nr-engine-design.md), numerical reference and operation gates; [recovered tensor coordinates](../../amd-nr-c512-tensor-layout.md); [half reduction sequence](../../amd-nr-c512-residual-output.md#optional-half-average).

Branch: `codex/amd-nr-engine`. Execution: Native, preserving the user's earlier selection and instruction to implement what is needed for AMD NR. This bounded follow-on stays within the approved numerical-reference component.

Status: implemented and independently reviewed on 2026-10-05. Whole-branch review through `f73dee0`: no Critical/Important findings or new Minors. Full standalone suite: 14 passes, one missing-debug-layer skip, zero failures; independent reduction oracle: 131,072 exact encoded outputs. Scope limitations and the previously deferred historical status wording remain explicit.

## Global Constraints

- "The product consists of source-owned C++, HLSL, logical schemas, and tests."
- "FP8 is encoded storage with explicit software decode/encode; FP16 storage and required rounding boundaries are explicit."
- "A block is enabled only after its schema and CPU/GPU comparison pass."
- "It must not invent operations to fill gaps."
- Keep original/extracted code, trained weights and private probes ignored.
- Both tensor axes are positive multiples of 4; channel count is exactly 512. Default storage budget is 256 MiB. Reject size overflow before accepting metadata.
- No GPU projection, full inference, renderer connection, installation, push or merge. Incomplete model/inference status remains unchanged.

## Review Focus

1. Non-square extents and upper channel bits must not transpose pixels or alias addresses: complete forward-address checks in Task 1.
2. Zero/unaligned/overflowing extents and insufficient budgets must fail before storage use: Task 1 invalid-layout tests.
3. Invalid order/count/coordinates and overlapping buffers must fail before writes; exact in-place identity conversion is permitted: Task 1 validation sentinels.
4. Reduced output may contain padding or crop source-derived pixels; untouched bytes must retain their prior contents: Task 2 sentinel and cropped-output tests.
5. Half overflow, cancellation, NaNs, signed zero and subnormal rounding must follow three half adds and one half multiply, not one final rounding: Task 2 literal fixtures and independent oracle.

## File structure and interfaces

- Create `src/NeuralRendering/Amd/C512Tensor.h/.cpp`: immutable validated layout and byte-preserving reorder only.
- Create `src/NeuralRendering/Amd/C512Reduction.h/.cpp`: CPU half reduction only.
- Create `tests/amd-nr/C512TensorTests.cpp`: layout validation, forward address/bit tests and reduction fixtures; test-only oracle file driver.
- Create `tests/amd-nr/C512ReductionOracle.py`: independent NumPy half rounding and mathematical nearest-distance E4M3 comparison.
- Modify `cmake/AmdNrFoundations.cmake`, `tests/amd-nr/CMakeLists.txt`: core sources, test executable and conditional NumPy oracle registration.
- Update `tests/amd-nr/README.md`, research/layout reports: actual scope, reproduction, allocator initialization evidence and measured results.

Shared types: `enum class C512TensorOrder { Canonical, Packed, Blocked16 }; enum class TensorError { Extent, Overflow, Budget, Order, Coordinate, CountMismatch, Overlap };`

`class C512TensorLayout` has private construction, `Extent Dimensions() const noexcept`, `size_t ByteCount() const noexcept`, and `expected<size_t,TensorError> Offset(C512TensorOrder,uint32_t x,uint32_t y,uint32_t channel) const noexcept`.

`MakeC512TensorLayout(Extent,size_t budget=256u*1024u*1024u) noexcept -> expected<C512TensorLayout,TensorError>`.

Canonical byte/half index is `(x*height+y)*512+channel`. Packed and Blocked16 offsets follow the recovered contract. `ReorderC512Tensor(const C512TensorLayout&,C512TensorOrder from,C512TensorOrder to,span<const uint8_t>,span<uint8_t>) noexcept -> expected<void,TensorError>`. Exact input/output counts are required. Reject partial overlap and every overlapping non-identity permutation before writing; exact in-place identity succeeds.

`ReduceC512Half2x2(const C512TensorLayout& source,const C512TensorLayout& destination,span<const uint16_t> canonicalHalfInput,span<uint8_t> packedOutput) noexcept -> expected<void,TensorError>`. Exact source/destination counts required; reject any byte overlap before writing. Active dimensions are `min(source.width/2,destination.width)` and likewise height. Each output uses `(2*x,2*y),(2*x,2*y+1),(2*x+1,2*y),(2*x+1,2*y+1)`, three explicit nearest-even half adds and one half quarter multiply, then the existing E4M3 encoder. Leave destination padding untouched.

### Task 1: Validated C512 layout and storage conversion

- [x] Write `LayoutsAndForwardOracle`: enumerate 4x4, 8x12, 12x8, 32x20 and 60x36; compare every packed offset to independent forward gather integer definitions; independently check Blocked16 strides and Canonical index. Require exact single-visit coverage. Reorder address-bit patterns both directions among all three orders; preserve all 256 FP8 codes.
- [x] Write `InvalidLayoutsAndBuffers`: reject zero/unaligned extents, 32-bit maximum aligned product overflow, budget underflow; test exact budget acceptance, out-of-range coordinates and invalid orders; count errors, alias and partial-overlap rejection leave sentinels unchanged; identity in-place succeeds.
- [x] Wire target/core source only when implementation exists. First configure/build target with test includes naming the absent API: expected failure identifies missing `C512Tensor.h`. Keep the red output.
- [x] Implement checked metadata, recovered bit-deposit indexing and conversion. No allocation or numeric decode. Use integer address comparisons for byte overlap without relational comparisons between unrelated C++ pointers.
- [x] Build and run `AmdNrC512Tensor`; expected exit 0 and exact address/storage assertions passing. Commit Task 1 source/tests/build changes.

### Task 2: Half reduction and independent comparison

- [x] Add `HalfReductionBoundaries`: literals `[1024,0.5,-1024,0.5] -> 0x20`, all-negative zero -> `0x80`, opposite infinite sums -> `0x7f`, all large positives -> `0x7e`; source pixels/channels differ. Check 60x36 ->32x20 preserves exactly 51,200 sentinel padding bytes, cropped 8x12 ->4x4, count and overlap failures before writes.
- [x] Add test-only `--oracle input output` driver using fixed source64x16/destination32x8 and exact input length (524,288 raw half words). Python oracle covers all 65,536 half encodings in one input position, 65,536 fixed-seed random four-half tuples, and literal special/boundary tuples. NumPy rounds each half arithmetic step; E4M3 expected values use the existing independent nearest-distance oracle. Compare all 131,072 canonical encoded bytes exactly.
- [x] Build before implementing reduction: expected missing header/API failure. Retain red output.
- [x] Implement reduction with the existing explicit half conversions and E4M3 encoder; verify counts and any byte overlap before touching output. Preserve padding.
- [x] Build/run full suite and independent oracle: expected 14 passes, one missing-debug-layer skip, zero failures and zero bit mismatches. Update actual results and allocator initialization limits; commit Task 2 changes.
- [x] Run the Native whole-branch fresh review against branch merge-base, resolve Critical/Important findings with red/green tests, ledger deferred Minors and scope rulings. Keep branch local.

## Self-review

Scope covers a bounded portion of the approved numerical reference. Product signatures and source/destination ordering agree across both tasks. All five review focus classes have explicit tests. Floating matrix equivalence, GPU reduction, persistent padding lifecycle and whole-network integration remain later gates; no incomplete block is enabled by these primitives.
