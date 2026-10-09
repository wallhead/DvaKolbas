#pragma once
#include "XessFrameAdapter.h"
#include "Graphics/D3D11D3D12Interop.h"
#include <memory>
namespace TheosRenderPipeline::Upscaling
{
    enum class XessFrameAdmission { Ready, DuplicateSource, OffOwnerThread };
    struct XessGpuResources
    {
        ID3D12Resource *color{},*depth{},*motion{},*output{};
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;
        std::array<ID3D12Resource*,4> All() const { return {color,depth,motion,output}; }
    };
    class XessUpscaler final
    {
    public:
        XessUpscaler();~XessUpscaler();
        XessUpscaler(const XessUpscaler&)=delete;
        XessUpscaler& operator=(const XessUpscaler&)=delete;
        Result<Extent> Initialize(std::shared_ptr<XessRuntime>,ID3D12Device*,Quality,Extent,XessInputPolicy);
        // Resolve measured guide units/extents after SDK sizing, before first execution.
        Result<void> ConfigureGuides(XessInputPolicy);
        Result<std::array<float,2>> QueryJitter(uint64_t sourceId);
        // Check before NR, shared input writes or SDK calls. Deferred frames do
        // not change accepted history; the next owner-thread source may resume.
        XessFrameAdmission AdmitFrame(uint64_t sourceId,uint64_t sourceEpoch) const;
        bool OnOwnerThread() const;
        DWORD OwnerThread() const;
        Result<void> Dispatch(ID3D12GraphicsCommandList*,const XessGpuResources&,const UpscaleFrame&);
        Result<void> TrackReader(ID3D12Fence*,uint64_t value);
        Result<void> DestroyAfterRetirement();
        uint32_t RequestedFlags() const;
        uint32_t EffectiveFlags() const;
    private:
        struct State;std::unique_ptr<State> state_;
    };
}
