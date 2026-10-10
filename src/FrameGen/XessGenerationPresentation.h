#pragma once
#include "XessGenerationTransport.h"
#include "XellSession.h"
namespace TheosRenderPipeline
{
    class XessGenerationPresentation final
    {
    public:
        XessGenerationPresentation();
        ~XessGenerationPresentation();
        XessGenerationPresentation(const XessGenerationPresentation&)=delete;
        XessGenerationPresentation& operator=(const XessGenerationPresentation&)=delete;
        Upscaling::Result<void> Create(IDXGIFactory*,std::shared_ptr<XessGenerationRuntime>,std::shared_ptr<Graphics::D3D11D3D12Interop>,const DXGI_SWAP_CHAIN_DESC&,std::uint32_t initFlags=0,bool waitTaggedPublication=false);
        Upscaling::Result<XessGenerationFrame> Prepare(const Upscaling::UpscaleFrame&,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool hudComplete,std::uint32_t sdkId,bool tag);
        // Real image/HUD publication with no new engine source or SDK ID.
        Upscaling::Result<XessGenerationFrame> PrepareReal(const Upscaling::UpscaleFrame&,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool hudComplete);
        HRESULT Present(const XessGenerationFrame&,bool generate,UINT interval,UINT flags,double* proxyPresentMs=nullptr);
        HRESULT StartupPresent(UINT interval,UINT flags);
        Upscaling::Result<void> Suspend();
        Upscaling::Result<void> Resume();
        Upscaling::Result<void> Retire();
        IDXGISwapChain4* SwapChain() const;
        xefg_swapchain_present_status_t Status() const;
        XellSession* Latency() const;
        static bool InternalFactoryCreation();
    private:
        struct State;
        std::unique_ptr<State> state_;
    };
}
