#pragma once
#include <cstdint>
#include <expected>
#include <span>

namespace TheosRenderPipeline::NeuralRendering::Amd {
std::uint16_t FloatToHalfRne(float value) noexcept;
float HalfToFloat(std::uint16_t bits) noexcept;
std::uint16_t DecodeE4m3ToHalf(std::uint8_t bits) noexcept;
enum class FormatOperation { Float32ToHalf, HalfToFloat32, E4m3ToHalf };
enum class FormatError { InvalidOperation, CountMismatch };
std::expected<void, FormatError> ConvertFormatWords(FormatOperation operation,
    std::span<const std::uint32_t> input, std::span<std::uint32_t> output) noexcept;
}
