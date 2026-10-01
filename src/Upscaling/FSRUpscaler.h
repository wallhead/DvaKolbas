#pragma once
#include "FSRRuntime.h"
#include "FSRParameters.h"
#include "Graphics/D3D11D3D12Interop.h"
#include <dx12/ffx_api_dx12.h>
#include <memory>

namespace TheosRenderPipeline::Upscaling
{
    class FsrUpscaler final
    {
    public:
        FsrUpscaler();
        ~FsrUpscaler();
        FsrUpscaler(const FsrUpscaler&)=delete;
        FsrUpscaler& operator=(const FsrUpscaler&)=delete;
        // Configure before Initialize; input conventions are immutable per context.
        Result<void> SetInputPolicy(FsrInputPolicy);
        Result<void> SetAllocationCallbacks(const ffxCreateBackendDX12AllocationCallbacksDesc&);
        Result<void> SetRetirementBridge(std::shared_ptr<Graphics::D3D11D3D12Interop>);
        Result<void> Initialize(std::shared_ptr<FsrRuntime>,ID3D12Device*,const ProviderInfo&,Quality,Extent render,Extent output);
        Result<Extent> RenderExtent() const;
        Result<ProviderInfo> ActualProvider() const;
        Result<std::array<float,2>> QueryJitter(std::uint64_t sourceId);
        Result<void> Dispatch(ID3D12GraphicsCommandList*,const GpuFrameResources&,const UpscaleFrame&);
        Result<void> DestroyAfterRetirement();
        const FsrContextLimits& Limits() const;
    private:
        struct State;
        std::unique_ptr<State> state_;
    };
}
