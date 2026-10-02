#pragma once
#include "FSRPresentation.h"
#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRSettings.h"
namespace TheosRenderPipeline
{
    struct FsrHostResize { Upscaling::Extent render{};HRESULT result{S_OK}; };
    // Render-thread host boundary. SDK callbacks belong to FsrPresentation;
    // this owner never exposes game calls to an AMD worker.
    class FsrHostPresentation final
    {
    public:
        FsrHostPresentation();~FsrHostPresentation();
        FsrHostPresentation(const FsrHostPresentation&)=delete;
        FsrHostPresentation& operator=(const FsrHostPresentation&)=delete;
        Upscaling::Result<Upscaling::Extent> Create(IDXGIFactory*,ID3D11Device*,
            std::shared_ptr<Upscaling::FsrHostResources>,const DXGI_SWAP_CHAIN_DESC&,const Upscaling::FsrSettings&);
        HRESULT StartupPresent(UINT interval,UINT flags);
        HRESULT WaitBeforeProducer();
        Upscaling::Result<void> BeforeResize();
        Upscaling::Result<void> Suspend();Upscaling::Result<void> Resume();
        Upscaling::Result<FsrHostResize> Resize(const DXGI_SWAP_CHAIN_DESC&);
        bool Suspended()const;
        HRESULT Present(const Upscaling::UpscaleFrame&,Upscaling::UpscaleOutcome,
            ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool uiComplete,bool menu,bool requested,UINT interval,UINT flags);
        Upscaling::Result<void> Retire();
        IDXGISwapChain4* SwapChain()const;ID3D11Texture2D* SceneTarget11()const;
        bool FeatureReady()const;FsrPresentationStatus Status()const;
    private:
        struct State;std::unique_ptr<State> state_;
    };
}
