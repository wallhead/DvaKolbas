#pragma once
#include "C512Tensor.h"

namespace TheosRenderPipeline::NeuralRendering::Amd {
// CPU numerical reference. Input is canonical raw FP16, output packed E4M3.
// Writes only min(source/2,destination) on each axis; padding remains untouched.
// Exact counts required, any input/output byte overlap rejected before writes.
std::expected<void,TensorError> ReduceC512Half2x2(
    const C512TensorLayout& source,const C512TensorLayout& destination,
    std::span<const std::uint16_t> canonicalHalfInput,
    std::span<std::uint8_t> packedOutput) noexcept;
}
