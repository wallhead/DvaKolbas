#pragma once
#include "FSRParameters.h"
#include "Graphics/D3D11D3D12Interop.h"
#include <memory>
namespace TheosRenderPipeline::Upscaling
{
    // Serialized render-thread producer. Its bridge retains all published guide readers.
    class FsrGenerationGuideAdapter final
    {
    public:
        FsrGenerationGuideAdapter(std::shared_ptr<Graphics::D3D11D3D12Interop>,GpuFrameResources,
            ID3D11Texture2D* depth,ID3D11Texture2D* motion);
        ~FsrGenerationGuideAdapter();
        FsrGenerationGuideAdapter(const FsrGenerationGuideAdapter&)=delete;
        FsrGenerationGuideAdapter& operator=(const FsrGenerationGuideAdapter&)=delete;
        Result<void> Prepare(const UpscaleFrame&);
    private:
        struct State;std::unique_ptr<State> state_;
    };
}
