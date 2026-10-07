#pragma once
#include "FSRUpscaler.h"
#include "FSRAvailability.h"
namespace TheosRenderPipeline::Upscaling
{
    // Owns the pre-query device and deferred temporal feature. It contains no
    // swapchain wrapper, scene/UI routing or vendor presentation implementation.
    class FsrHostResources final
    {
    public:
        using DeviceCreator = HRESULT (*)(IUnknown*, D3D_FEATURE_LEVEL, ID3D12Device**);
        explicit FsrHostResources(std::filesystem::path pluginDirectory, DeviceCreator = nullptr);~FsrHostResources();
        FsrHostResources(const FsrHostResources&)=delete;FsrHostResources& operator=(const FsrHostResources&)=delete;
        Result<Extent> PrepareSizing(ID3D11Device*,const BackendConfiguration&,Extent output,DXGI_FORMAT handoffFormat,ColorEncoding handoffEncoding);
        Result<void> PrepareExternalSizing(ID3D11Device*,Extent render,Extent display,DXGI_FORMAT,ColorEncoding,FsrInputPolicy);
        Result<void> CompleteExternalStartup();
        Result<void> ResizeExternalSizingAfterRetirement(Extent render,Extent display,DXGI_FORMAT);
        bool ExternalSource()const;
        bool GenerationInputsReady()const;
        FsrInputPolicy GenerationInputPolicy()const;
        Extent RenderExtent()const;
        Result<void> CompleteStartup();Result<void> Retire();
        Result<void> ReleaseSizedAfterRetirement();
        Result<Extent> ResizeSizingAfterRetirement(Extent,DXGI_FORMAT);
        Result<void> EnsureInputPolicy(FsrInputPolicy);
        bool FeatureReady()const;bool ContextOwned()const;
        std::shared_ptr<Graphics::D3D11D3D12Interop> Bridge()const;
        std::shared_ptr<FsrRuntime> Runtime()const;
#if defined(TRP_ENABLE_FSR_FG)
        Result<void> LoadFrameGeneration();
#endif
        GpuFrameResources Resources()const;
        FsrUpscaler* Upscaler()const;
        ID3D11Texture2D* Color11()const;ID3D11Texture2D* Depth11()const;ID3D11Texture2D* Motion11()const;ID3D11Texture2D* Output11()const;
        const ProviderInfo& Provider()const;
        const std::string& ProviderDiagnostic()const;
        const FsrMlAvailability& MlAvailability()const;
        ColorEncoding HandoffEncoding()const;
    private:
        Result<void> InitializeDevice(ID3D11Device*);
        struct State;std::unique_ptr<State> state_;std::filesystem::path pluginDirectory_;DeviceCreator deviceCreator_{};
    };
}
