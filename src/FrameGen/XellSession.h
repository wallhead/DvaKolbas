#pragma once
#include "XessGenerationRuntime.h"
namespace TheosRenderPipeline
{
    class XellSession final
    {
    public:
        XellSession();
        ~XellSession();
        XellSession(const XellSession&)=delete;
        XellSession& operator=(const XellSession&)=delete;
        Upscaling::Result<void> Create(ID3D12Device*, std::shared_ptr<XessGenerationRuntime>);
        Upscaling::Result<void> BeginFrame(std::uint32_t sdkId);
        Upscaling::Result<void> Marker(std::uint32_t sdkId,xell_latency_marker_type_t);
        Upscaling::Result<void> SetEnabled(bool enabled,bool gpuQuiescent);
        Upscaling::Result<void> ResetAfterDrain(bool gpuQuiescent);
        Upscaling::Result<void> Retire(bool fgDestroyed,bool gpuQuiescent);
        xell_context_handle_t Context() const;
    private:
        struct State;
        std::unique_ptr<State> state_;
    };
}
