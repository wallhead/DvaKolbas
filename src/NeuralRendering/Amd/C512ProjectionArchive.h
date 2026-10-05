#pragma once
#include "C512Projection.h"
#include "WeightArchive.h"

namespace TheosRenderPipeline::NeuralRendering::Amd {
bool IsC512ProjectionSelection(std::uint32_t block,std::uint32_t layer) noexcept;
// Exact known archive identity is mandatory. A successful storage decode never
// promotes the model's incomplete schema to an inference-ready status.
std::expected<C512ProjectionWeights,ProjectionError> ReadKnownC512Projection(
    const ArchiveFile&,std::uint32_t block,std::uint32_t layer);
}
