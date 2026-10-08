#pragma once
#include "HDROutput.h"
#include <cstdint>
#include <span>

namespace TheosRenderPipeline::HDROutput
{
    // A sparse grid is evidence about those pixels, not the frame's true peak.
    struct SampleMetric
    {
        float mean{}, p95{}, maximum{};
    };

    inline SampleMetric Summarize(std::span<const float> values)
    {
        if (values.empty()) { return {}; }
        std::array<float, 144> sorted{};
        const auto size = (std::min)(values.size(), sorted.size());
        float sum = 0;
        for (std::size_t i = 0; i < size; ++i) { sorted[i] = values[i]; sum += values[i]; }
        std::sort(sorted.begin(), sorted.begin() + size);
        return {sum / static_cast<float>(size), sorted[(95 * (size - 1)) / 100], sorted[size - 1]};
    }

    inline float DecodePQ(float code)
    {
        constexpr double m1 = 2610.0 / 16384.0, m2 = 2523.0 / 32.0;
        constexpr double c1 = 3424.0 / 4096.0, c2 = 2413.0 / 128.0, c3 = 2392.0 / 128.0;
        const auto p = std::pow(std::clamp(static_cast<double>(code), 0.0, 1.0), 1.0 / m2);
        return static_cast<float>(10000.0 * std::pow((std::max)(p - c1, 0.0) / (c2 - c3 * p), 1.0 / m1));
    }

    inline float Luminance709(RGB rgb)
    {
        return 0.2126f * rgb[0] + 0.7152f * rgb[1] + 0.0722f * rgb[2];
    }

    inline float LuminancePQ2020(RGB codes)
    {
        const auto& y = SourceDLSSG::HDRColorimetry::k2020ToXYZ[1];
        return static_cast<float>(y[0] * DecodePQ(codes[0]) + y[1] * DecodePQ(codes[1]) + y[2] * DecodePQ(codes[2]));
    }

    struct SampleReport
    {
        std::uint64_t frame{};
        unsigned samples{};
        bool composed{};
        bool generationRequested{};
        ShaderConstants constants{}; // Calibration at recording time, not harvest time.
        SampleMetric scene, composite, expectedWorld, hudless, output;
        SampleMetric inputUICoverage, uiCoverage; // Alpha, not nits.
    };
}
