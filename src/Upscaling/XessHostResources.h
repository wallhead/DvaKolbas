#pragma once
#include "XessUpscaler.h"
#include <filesystem>
namespace TheosRenderPipeline::Upscaling
{
    class XessHostResources final
    {
    public:
        using DeviceCreator = HRESULT (*)(IUnknown*, D3D_FEATURE_LEVEL, ID3D12Device**);
        explicit XessHostResources(std::filesystem::path, DeviceCreator = nullptr);~XessHostResources();
        XessHostResources(const XessHostResources&)=delete;
        XessHostResources& operator=(const XessHostResources&)=delete;
        Result<Extent> Initialize(ID3D11Device*,Quality,Extent,ColorEncoding,bool depthInverted=false);
        // Scene-thread producer; previous dispatch must have queued WaitConsumer first.
        Result<void> PrepareInput(ID3D11Texture2D*,ID3D11Texture2D*,ID3D11Texture2D*);
        Result<void> SharpenOutput(ID3D11DeviceContext*,ID3D11Texture2D*,float);
        bool SharpeningAvailable() const;
        float AppliedSharpness() const;
        const std::optional<RuntimeError>& SharpeningError() const;
        std::optional<RuntimeError> TakeSharpeningNotice();
        Result<void> Retire();
        Extent RenderExtent() const;
        XessGpuResources Resources() const;
        std::shared_ptr<Graphics::D3D11D3D12Interop> Bridge() const;
        XessUpscaler* Upscaler() const;
        ID3D11Texture2D* Output11() const;
    private:
        struct State;std::unique_ptr<State> state_;std::filesystem::path pluginDirectory_;DeviceCreator deviceCreator_{};
    };
}
