#pragma once
#include "UpscalerBackend.h"
#include <ffx_upscale.h>
#include <d3d12.h>

namespace TheosRenderPipeline::Upscaling
{
    struct GpuFrameResources
    {
        ID3D12Resource *color{}, *depth{}, *motion{}, *output{}, *exposure{}, *reactive{}, *transparencyComposition{};
        std::array<ID3D12Resource*,7> All() const { return {color,depth,motion,output,exposure,reactive,transparencyComposition}; }
    };
    struct FsrInputPolicy
    {
        bool depthInverted{}, depthInfinite{}, motionIncludesJitter{}, colorIsLinear{true};
        bool operator==(const FsrInputPolicy&)const=default;
    };
    struct FsrContextLimits
    {
        Extent render{}, output{};
        DXGI_FORMAT colorFormat{DXGI_FORMAT_R16G16B16A16_FLOAT};
        DXGI_FORMAT depthFormat{DXGI_FORMAT_R32_FLOAT}, motionFormat{DXGI_FORMAT_R16G16_FLOAT};
        FsrInputPolicy input{};
    };
    // Descriptors describe COMPUTE_READ inputs and a UAV output. The owner must
    // transition COMMON -> those states before dispatch and restore COMMON.
    Result<ffxDispatchDescUpscale> BuildFsrDispatch(const GpuFrameResources&, const UpscaleFrame&, const FsrContextLimits&);
}
