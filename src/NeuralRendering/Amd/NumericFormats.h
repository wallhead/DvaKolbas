#pragma once
#include <cstdint>
#include <expected>
#include <span>

namespace TheosRenderPipeline::NeuralRendering::Amd {
std::uint16_t FloatToHalfRne(float value) noexcept;
float HalfToFloat(std::uint16_t bits) noexcept;
std::uint16_t DecodeE4m3ToHalf(std::uint8_t bits) noexcept;
// Nearest-even, signed finite saturation at 448, signed zero, canonical NaN 0x7f.
std::uint8_t EncodeFloatToE4m3Rne(float value) noexcept;
enum class FormatOperation { Float32ToHalf, HalfToFloat32, E4m3ToHalf, Float32ToE4m3 };
enum class FormatError { InvalidOperation, CountMismatch };
std::expected<void, FormatError> ConvertFormatWords(FormatOperation operation,
    std::span<const std::uint32_t> input, std::span<std::uint32_t> output) noexcept;
}
