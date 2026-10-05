#pragma once
#include <array>
#include <cstdint>
#include <expected>
namespace TheosRenderPipeline::NeuralRendering::Amd {
struct Extent { std::uint32_t width,height; bool operator==(const Extent&) const = default; };
enum class ExtentMode { Default, Step8, Step128 };
struct LevelDimensions { Extent extent; std::uint32_t channels; };
struct ModelGeometry { Extent input,processing; std::array<LevelDimensions,6> levels; };
enum class GeometryError { InvalidMode, ZeroDimension, DimensionOverflow };
std::expected<ModelGeometry,GeometryError> MakeGeometry(Extent input, ExtentMode mode=ExtentMode::Default, bool disableExtraHeight=false) noexcept;
}
