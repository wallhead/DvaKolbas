#pragma once
#include "WeightArchive.h"
#include <array>
namespace TheosRenderPipeline::NeuralRendering::Amd {
using Sha256Digest=std::array<std::byte,32>;
enum class IdentityError { CryptoProvider, Memory };
enum class ModelStatus { UnsupportedArchive, KnownArchiveIncompleteSchema };
struct ModelIdentification { ModelStatus status; Sha256Digest digest; std::string diagnostic; };
std::expected<Sha256Digest,IdentityError> Sha256(std::span<const std::byte>);
std::expected<ModelIdentification,IdentityError> IdentifyModel(const ArchiveFile&);
}
