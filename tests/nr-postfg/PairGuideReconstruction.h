#pragma once
#include <cstdint>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

namespace NrPostFgResearch {
// Research-only: orthographic, constant velocity/depth, same-resolution guides.
// No geometry, color, object IDs or independent reference enter this interface.
struct PairGuidePixel {
    float depthMetres{}, currentToHistoryPixelsX{}, currentToHistoryPixelsY{};
    bool operator==(const PairGuidePixel&) const = default;
};
struct PairGuideFrame {
    uint64_t epoch{}, sourceId{};
    unsigned width{}, height{};
    double time{}, historyTime{};
    std::vector<PairGuidePixel> pixels;
};
struct GuideTarget {
    uint64_t epoch{}, previousSourceId{}, currentSourceId{};
    double fraction{}, historyTime{};
};
struct ReconstructedGuides {
    unsigned width{}, height{};
    double time{}, historyTime{};
    std::vector<PairGuidePixel> pixels;
    // Coverage only means an endpoint splatted here, not that it is correct.
    std::vector<bool> covered;
};
inline ReconstructedGuides ReconstructPairGuides(
    const PairGuideFrame& previous, const PairGuideFrame& current, const GuideTarget& target) {
    const auto invalid = [] { throw std::invalid_argument("invalid source-pair guide contract"); };
    if (!previous.epoch || previous.epoch != current.epoch || previous.epoch != target.epoch ||
        !previous.sourceId || previous.sourceId == std::numeric_limits<uint64_t>::max() ||
        current.sourceId != previous.sourceId + 1 || target.previousSourceId != previous.sourceId ||
        target.currentSourceId != current.sourceId || !previous.width || !previous.height ||
        uint64_t(previous.width) * previous.height > 16 * 1024 * 1024 ||
        previous.width != current.width || previous.height != current.height ||
        previous.pixels.size() != size_t(previous.width) * previous.height ||
        previous.pixels.size() != current.pixels.size() || !std::isfinite(previous.time) ||
        !std::isfinite(current.time) || current.time <= previous.time ||
        !std::isfinite(target.fraction) || target.fraction <= 0 || target.fraction >= 1)
        invalid();
    const double pairInterval = current.time - previous.time;
    const double outputTime = previous.time + pairInterval * target.fraction;
    if (!std::isfinite(pairInterval) || !std::isfinite(outputTime) ||
        !std::isfinite(target.historyTime) || target.historyTime < previous.time ||
        target.historyTime >= outputTime) invalid();
    const double outputInterval = outputTime - target.historyTime;
    for (const auto* frame : {&previous, &current}) {
        const double interval = frame->time - frame->historyTime;
        if (!std::isfinite(frame->historyTime) || !std::isfinite(interval) || interval <= 0) invalid();
        for (const auto& p : frame->pixels) {
            if (!std::isfinite(p.depthMetres) || p.depthMetres <= 0 ||
                !std::isfinite(p.currentToHistoryPixelsX) || !std::isfinite(p.currentToHistoryPixelsY)) invalid();
            for (const auto motion : {p.currentToHistoryPixelsX, p.currentToHistoryPixelsY}) {
                const double outputMotion = double(motion) / interval * outputInterval;
                if (!std::isfinite(outputMotion) || std::abs(outputMotion) > std::numeric_limits<float>::max()) invalid();
            }
        }
    }
    ReconstructedGuides result{previous.width, previous.height, outputTime, target.historyTime,
        std::vector<PairGuidePixel>(previous.pixels.size()), std::vector<bool>(previous.pixels.size(), false)};
    for (const auto* frame : {&previous, &current}) {
        const double interval = frame->time - frame->historyTime;
        for (unsigned y = 0; y < frame->height; ++y) for (unsigned x = 0; x < frame->width; ++x) {
            const auto& p = frame->pixels[size_t(y) * frame->width + x];
            // The sign is current -> history: positive camera motion moves the
            // surface left in the future. Backward splatting reverses that shift.
            const double vx = double(p.currentToHistoryPixelsX) / interval;
            const double vy = double(p.currentToHistoryPixelsY) / interval;
            const double px = x + .5 - vx * (outputTime - frame->time);
            const double py = y + .5 - vy * (outputTime - frame->time);
            if (!std::isfinite(px) || !std::isfinite(py) || px < 0 || py < 0 ||
                px >= frame->width || py >= frame->height) continue;
            const auto index = size_t(std::floor(py)) * frame->width + size_t(std::floor(px));
            if (result.covered[index] && result.pixels[index].depthMetres <= p.depthMetres) continue;
            result.covered[index] = true;
            result.pixels[index] = {p.depthMetres, float(vx * outputInterval), float(vy * outputInterval)};
        }
    }
    return result;
}
}
