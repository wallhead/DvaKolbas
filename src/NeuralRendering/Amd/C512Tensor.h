#pragma once
#include "ModelGeometry.h"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>

namespace TheosRenderPipeline::NeuralRendering::Amd {
enum class C512TensorOrder { Canonical, Packed, Blocked16 };
enum class TensorError { Extent, Overflow, Budget, Order, Coordinate, CountMismatch, Overlap };

// Internal C512 coordinates; game-frame channel semantics remain unestablished.
class C512TensorLayout {
public:
    Extent Dimensions() const noexcept { return extent_; }
    std::size_t ByteCount() const noexcept { return bytes_; }
    std::expected<std::size_t,TensorError> Offset(C512TensorOrder,
        std::uint32_t x,std::uint32_t y,std::uint32_t channel) const noexcept;
private:
    C512TensorLayout(Extent e,std::size_t bytes) noexcept : extent_(e),bytes_(bytes) {}
    Extent extent_;
    std::size_t bytes_;
    friend std::expected<C512TensorLayout,TensorError> MakeC512TensorLayout(Extent,std::size_t) noexcept;
};

std::expected<C512TensorLayout,TensorError> MakeC512TensorLayout(Extent,
    std::size_t budget=256u*1024u*1024u) noexcept;

// Exact counts required. Only identity conversion permits exact in-place use.
// Every error leaves output untouched; storage conversion preserves all FP8 bits.
std::expected<void,TensorError> ReorderC512Tensor(const C512TensorLayout&,
    C512TensorOrder from,C512TensorOrder to,
    std::span<const std::uint8_t> input,std::span<std::uint8_t> output) noexcept;
}
