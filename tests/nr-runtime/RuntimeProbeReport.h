#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace NrRuntimeResearch {
struct ProbeReport {
    std::string profile, runtimeSha256, coreSha256, failure;
    uint32_t init{}, create{}, evaluate{}, release{}, destroyParameters{}, shutdown{};
    uint32_t requestedFrames{}, readbackFrames{}, distinctOutputHashes{}, spatiallyVariedFrames{};
    uint64_t outputPixels{}, finitePixels{}, overwrittenPixels{}, changedFromInputPixels{};
    uint32_t allocations{}, releases{};
    bool runtimeHeldAndMatched{}, coreHeldAndMatched{}, shimRequested{}, shimRestored{}, outputReadersRetired{};
};
std::vector<std::string> Validate(const ProbeReport&);
}
