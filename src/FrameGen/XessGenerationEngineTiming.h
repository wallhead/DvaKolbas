#pragma once
#include "XellSession.h"
namespace TheosRenderPipeline
{
    // Contract only. Both modes require inspected render/owner boundaries.
    // Presentation pacing does not prove sleep occurred before engine input.
    class XessGenerationEngineTiming final
    {
    public:
        enum class Mode { VerifiedInput, PresentationPacing };
        Upscaling::Result<void> Bind(XellSession*,bool verifiedBoundaries,Mode mode=Mode::VerifiedInput);
        Upscaling::Result<std::uint32_t> BeginSourceLoop(std::uint64_t sourceId,std::uint64_t epoch,double* sleepMs=nullptr,std::uint32_t* sleepSdkId=nullptr);
        Upscaling::Result<void> InputSampled(std::uint64_t sourceId);
        Upscaling::Result<void> EndSimulation(std::uint64_t sourceId);
        Upscaling::Result<void> BeginRender(std::uint64_t sourceId);
        Upscaling::Result<void> EndRender(std::uint64_t sourceId);
        Upscaling::Result<void> BeforePresent(std::uint32_t sdkId);
        Upscaling::Result<void> AfterPresent(std::uint32_t sdkId);
        Upscaling::Result<void> ResetAfterDrain(bool gpuQuiescent);
        // Discard an interrupted cycle after quiescence, without inventing end
        // markers or resetting the next monotonic SDK ID.
        Upscaling::Result<void> AbandonAfterDrain(bool gpuQuiescent);
        Upscaling::Result<void> AbandonUnsubmitted(bool noTaggedWork);
        bool OnOwnerThread() const { return thread_ && thread_==GetCurrentThreadId(); }
        Upscaling::Result<std::uint32_t> CurrentId(std::uint64_t sourceId,std::uint64_t epoch) const;
        Upscaling::Result<std::uint32_t> CurrentRenderId(std::uint64_t sourceId,std::uint64_t epoch) const;
    private:
        Upscaling::Result<void> Check() const;
        Upscaling::Result<void> SourceMarker(std::uint64_t sourceId,unsigned phase,xell_latency_marker_type_t);
        XellSession* latency_{};
        DWORD thread_{};
        std::uint64_t source_{},epoch_{},next_{1};
        std::uint32_t id_{};
        unsigned phase_{};
        Mode mode_{Mode::VerifiedInput};
        bool verified_{},sampled_{},fault_{};
    };
}
