#pragma once
#include "C512Projection.h"
#include "C512Tensor.h"

namespace TheosRenderPipeline::NeuralRendering::Amd {
inline constexpr std::size_t C512ProjectionMaxValues=256u*512u;
inline constexpr std::size_t C512ProjectionWordsPerValue=2;
enum class ProjectionEvaluationError { Extent, Count, Weights, Overlap };

std::expected<void,ProjectionEvaluationError> ValidateC512ProjectionInputs(
    const C512TensorLayout&,std::span<const std::uint8_t> canonicalInput,
    std::span<const std::uint8_t> canonicalResidual,const C512ProjectionWeights&) noexcept;

// Declared portable order: ascending FP32 K within each 32-term chunk, then half
// rounding; not claimed equivalent to WMMA's unresolved internal dot order.
// Word0 = final half | encoded E4M3 << 16; word1 = initial half, zero-extended.
// Arithmetic NaNs are canonical half 0x7e00. Every validation error precedes writes.
std::expected<void,ProjectionEvaluationError> EvaluateC512ProjectionReference(
    const C512TensorLayout&,std::span<const std::uint8_t> canonicalInput,
    std::span<const std::uint8_t> canonicalResidual,const C512ProjectionWeights&,
    std::span<std::uint32_t> output) noexcept;
}
