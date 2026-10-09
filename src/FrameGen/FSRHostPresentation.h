#pragma once
#include "FSRPresentation.h"
#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRSettings.h"
#include "FrameTelemetry.h"
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
        Upscaling::Result<Upscaling::Extent> CreateExternal(IDXGIFactory*,ID3D11Device*,
            std::shared_ptr<Upscaling::FsrHostResources>,const DXGI_SWAP_CHAIN_DESC&,const Upscaling::FsrSettings&,
            Upscaling::Extent render,Upscaling::FsrInputPolicy);
        template<class Query> static Upscaling::Result<Upscaling::Extent> ResolveExternalRenderExtent(const DXGI_SWAP_CHAIN_DESC& descriptor,Query&& query)
        {
            auto normalized=FsrPresentation::TranslateDescriptor(descriptor);
            if(!normalized)return std::unexpected(normalized.error());
            int width{},height{};
            if(!query(normalized->BufferDesc.Width,normalized->BufferDesc.Height,&width,&height) || width<=0 || height<=0)
                return std::unexpected(Upscaling::RuntimeError{Upscaling::ErrorKind::InvalidInput,E_INVALIDARG,"External NGX render-size query failed for normalized client extent"});
            return Upscaling::Extent{UINT(width),UINT(height)};
        }
        HRESULT StartupPresent(UINT interval,UINT flags);
        HRESULT WaitBeforeProducer();
        Upscaling::Result<void> BeforeResize();
        Upscaling::Result<void> Suspend();Upscaling::Result<void> Resume();
        Upscaling::Result<FsrHostResize> Resize(const DXGI_SWAP_CHAIN_DESC&);
        Upscaling::Result<FsrHostResize> ResizeExternal(const DXGI_SWAP_CHAIN_DESC&,Upscaling::Extent render);
        bool Suspended()const;
        HRESULT Present(const Upscaling::UpscaleFrame&,Upscaling::UpscaleOutcome,
            ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool uiComplete,bool menu,bool requested,UINT interval,UINT flags);
        Upscaling::Result<void> Retire();
        IDXGISwapChain4* SwapChain()const;ID3D11Texture2D* SceneTarget11()const;
        bool FeatureReady()const;FsrPresentationStatus Status()const;
        Telemetry::OutputCounter OutputCounter(std::uint64_t observations)const;
        const Upscaling::FsrEffectProvider& GenerationProvider()const;
    private:
        Upscaling::Result<Upscaling::Extent> CreateInternal(IDXGIFactory*,ID3D11Device*,
            std::shared_ptr<Upscaling::FsrHostResources>,const DXGI_SWAP_CHAIN_DESC&,const Upscaling::FsrSettings&,
            bool external,Upscaling::Extent render,Upscaling::FsrInputPolicy);
        Upscaling::Result<FsrHostResize> ResizeInternal(const DXGI_SWAP_CHAIN_DESC&,Upscaling::Extent render);
        struct State;std::unique_ptr<State> state_;
    };
}
