#pragma once
#include "XessGenerationFrameAdapter.h"
#include "XessGenerationRuntime.h"
#include "Graphics/D3D11D3D12Interop.h"
namespace TheosRenderPipeline
{
    class XessGenerationTransport final
    {
    public:
        XessGenerationTransport();
        ~XessGenerationTransport();
        XessGenerationTransport(const XessGenerationTransport&)=delete;
        XessGenerationTransport& operator=(const XessGenerationTransport&)=delete;
        Upscaling::Result<void> Initialize(std::shared_ptr<Graphics::D3D11D3D12Interop>,Upscaling::Extent display);
        Upscaling::Result<void> WaitBeforeProducer();
        Upscaling::Result<void> Upload(const Upscaling::UpscaleFrame&,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool hudComplete);
        Upscaling::Result<void> UploadReal(const Upscaling::UpscaleFrame&,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool hudComplete);
        Upscaling::Result<ID3D12GraphicsCommandList*> BeginTag();
        // Only the list returned by BeginTag is accepted; Tag submits it and
        // seals the actual SDK ONLY_NOW copies with the bridge's FG fence.
        Upscaling::Result<void> Tag(ID3D12GraphicsCommandList*,xefg_swapchain_handle_t,const XessGenerationFunctions&,const XessGenerationFrame&);
        Upscaling::Result<void> PublishTo(ID3D12Resource* backbuffer);
        Upscaling::Result<void> Retire();
        ID3D12Resource* Scene() const;
        ID3D12Resource* Ui() const;
    private:
        Upscaling::Result<void> UploadInternal(const Upscaling::UpscaleFrame&,ID3D11Texture2D*,ID3D11ShaderResourceView*,bool,bool realOnly);
        struct State;
        std::unique_ptr<State> state_;
    };
}
