#pragma once
#include "FSRParameters.h"
#include <ffx_framegeneration.h>
namespace TheosRenderPipeline::Upscaling
{
    struct FsrGenerationLimits
    {
        Extent render{}, display{};
        DXGI_FORMAT format{DXGI_FORMAT_R8G8B8A8_UNORM};
        FsrInputPolicy input{};
        bool debugChecking{};
    };
    struct FsrGenerationResources
    {
        ID3D12Resource *scene{}, *depth{}, *motion{}, *ui{};
        ColorEncoding sceneEncoding{ColorEncoding::Unknown};
    };
    Result<ffxDispatchDescFrameGenerationPrepareV2> BuildFsrGenerationPrepare(
        ID3D12GraphicsCommandList*, const UpscaleFrame&, const FsrGenerationResources&, const FsrGenerationLimits&, bool reset);
}
