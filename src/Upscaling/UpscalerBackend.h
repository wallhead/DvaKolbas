#pragma once

#include <array>
#include <cstdint>
#include <expected>
#include <string>
#include <dxgiformat.h>

struct ID3D11Texture2D;

namespace TheosRenderPipeline::Upscaling
{
    struct Extent { std::uint32_t width{}, height{}; bool operator==(const Extent&) const = default; };
    enum class BackendKind { Dlss, Dlaa, Fsr, External };
    enum class Quality { Quality, Balanced, Performance, NativeAA };
    enum class ProviderPolicy { Analytical, Compatible };
    struct ProviderInfo { std::uint64_t id{}; std::string name; };
    enum class GenerationPreparationStatus { NotRequested, Succeeded, Failed };
    enum class UpscaleOutcome { Temporal, SpatialRecovery, SkippedInvalidInput, Fatal };
    enum class PresentationKind { Ordinary, Nvidia };
    enum class ErrorKind {
        MissingRuntime, WrongArchitecture, MissingExport, IncompatibleAbi,
        NoProvider, UnsupportedDevice, InvalidInput, ContextFailure,
        DispatchFailure, DeviceLost, RetirementFailure
    };
    struct RuntimeError { ErrorKind kind{ErrorKind::InvalidInput}; std::int64_t nativeResult{}; std::string message; };
    template<class T> using Result = std::expected<T, RuntimeError>;

    struct BackendConfiguration
    {
        BackendKind backend{BackendKind::Dlss};
        Quality quality{Quality::Quality};
        ProviderPolicy providerPolicy{ProviderPolicy::Analytical};
        bool enabled{true}, generationEnabled{true};
        long generationBackend{1};
        bool neuralRendering{}, hdr{}, dynamicResolution{};
        float sharpness{};
    };
    struct BackendDecision
    {
        BackendKind backend{BackendKind::Dlss};
        PresentationKind presentation{PresentationKind::Nvidia};
        bool valid{}, generationEnabled{};
        std::string diagnostic;
    };

    // Measurements remain in engine/world coordinates until a backend adapter
    // validates and converts them. No vendor constants appear in this view.
    struct CameraMeasurements
    {
        std::array<float, 16> view{}, projection{};
        std::array<float, 3> position{};
        float nearDistance{}, farDistance{}, verticalFovRadians{}, worldUnitsToMeters{1};
        std::uint64_t identity{};
        bool depthInverted{}, depthInfinite{}, reset{};
    };
    struct MotionConvention
    {
        float scaleX{}, scaleY{};
        bool currentToPrevious{}, includesJitter{};
    };
    struct UpscaleFrame
    {
        BackendKind backend{BackendKind::Dlss};
        ID3D11Texture2D *color{}, *input{}, *depth{}, *motion{}, *output{};
        ID3D11Texture2D *exposure{}, *reactive{}, *transparencyComposition{};
        Extent render{}, display{}, subrect{};
        DXGI_FORMAT colorFormat{DXGI_FORMAT_UNKNOWN}, depthFormat{DXGI_FORMAT_UNKNOWN}, motionFormat{DXGI_FORMAT_UNKNOWN};
        CameraMeasurements camera{};
        MotionConvention motionConvention{};
        std::uint64_t sourceId{};
        float deltaMilliseconds{}, jitterX{}, jitterY{}, preExposure{1}, sharpness{};
        bool reset{}, colorIsLinear{};
    };
}
