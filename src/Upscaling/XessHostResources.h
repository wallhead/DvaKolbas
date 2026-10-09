#pragma once
#include "XessUpscaler.h"
#include <filesystem>
namespace TheosRenderPipeline::Upscaling
{
    class XessHostResources final
    {
    public:
        explicit XessHostResources(std::filesystem::path);~XessHostResources();
        XessHostResources(const XessHostResources&)=delete;
        XessHostResources& operator=(const XessHostResources&)=delete;
        Result<Extent> Initialize(ID3D11Device*,Quality,Extent,ColorEncoding,bool depthInverted=false);
        // Scene-thread producer; previous dispatch must have queued WaitConsumer first.
        Result<void> PrepareInput(ID3D11Texture2D*,ID3D11Texture2D*,ID3D11Texture2D*);
        Result<void> Retire();
        Extent RenderExtent() const;
        XessGpuResources Resources() const;
        std::shared_ptr<Graphics::D3D11D3D12Interop> Bridge() const;
        XessUpscaler* Upscaler() const;
        ID3D11Texture2D* Output11() const;
    private:
        struct State;std::unique_ptr<State> state_;std::filesystem::path pluginDirectory_;
    };
}
