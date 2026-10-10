#pragma once
#include "XellSession.h"
namespace TheosRenderPipeline
{
    // Contract only. Bind(true) is permitted only after the host has verified
    // real engine boundaries; CPU tests cannot qualify a Skyrim hook address.
    class XessGenerationEngineTiming final
    {
    public:
        Upscaling::Result<void> Bind(XellSession*,bool verifiedBoundaries);
        Upscaling::Result<std::uint32_t> BeginSourceLoop(std::uint64_t sourceId,std::uint64_t epoch);
        Upscaling::Result<void> InputSampled(std::uint64_t sourceId);
        Upscaling::Result<void> EndSimulation(std::uint64_t sourceId);
        Upscaling::Result<void> BeginRender(std::uint64_t sourceId);
        Upscaling::Result<void> EndRender(std::uint64_t sourceId);
        Upscaling::Result<void> BeforePresent(std::uint32_t sdkId);
        Upscaling::Result<void> AfterPresent(std::uint32_t sdkId);
        Upscaling::Result<void> ResetAfterDrain(bool gpuQuiescent);
        Upscaling::Result<std::uint32_t> CurrentId(std::uint64_t sourceId,std::uint64_t epoch) const;
    private:
        Upscaling::Result<void> Check() const;
        Upscaling::Result<void> SourceMarker(std::uint64_t sourceId,unsigned phase,xell_latency_marker_type_t);
        XellSession* latency_{};
        DWORD thread_{};
        std::uint64_t source_{},epoch_{},next_{1};
        std::uint32_t id_{};
        unsigned phase_{};
        bool verified_{},sampled_{},fault_{};
    };
}
