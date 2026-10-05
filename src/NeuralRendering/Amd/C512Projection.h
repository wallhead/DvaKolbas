#pragma once
#include <cstddef>
#include <cstdint>
#include <expected>
#include <memory>
#include <span>

namespace TheosRenderPipeline::NeuralRendering::Amd {
enum class ProjectionError { RecordSize, Memory, UnsupportedArchive, UnsupportedSelection, Archive, Crypto };

// Canonical native-fragment storage, not yet the model's spatial/HWC convention.
// Both views survive owner moves; they expire when the final owner is destroyed.
class C512ProjectionWeights {
public:
    static constexpr std::size_t Channels=512;
    ~C512ProjectionWeights();
    C512ProjectionWeights(C512ProjectionWeights&&) noexcept;
    C512ProjectionWeights(const C512ProjectionWeights&)=delete;
    C512ProjectionWeights& operator=(const C512ProjectionWeights&)=delete;
    C512ProjectionWeights& operator=(C512ProjectionWeights&&)=delete;
    // Row-major [output channel][reduction channel], preserving encoded FP8 bytes.
    std::span<const std::uint8_t> MatrixCodes() const & noexcept;
    std::span<const std::uint8_t> MatrixCodes() const &&=delete;
    // Canonical output-channel order, preserving raw FP16 bits.
    std::span<const std::uint16_t> ResidualHalfBits() const & noexcept;
    std::span<const std::uint16_t> ResidualHalfBits() const &&=delete;
private:
    struct Storage;
    explicit C512ProjectionWeights(std::unique_ptr<Storage>) noexcept;
    std::unique_ptr<Storage> storage_;
    friend std::expected<C512ProjectionWeights,ProjectionError> DecodeC512Projection(std::span<const std::byte>);
};

// Raw-record primitive only. Model identity is checked by the archive adapter.
std::expected<C512ProjectionWeights,ProjectionError> DecodeC512Projection(std::span<const std::byte>);
}
