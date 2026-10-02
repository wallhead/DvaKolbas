#pragma once
#include "Upscaling/FSRFrameGeneration.h"
#include "Upscaling/FSRGenerationPolicy.h"
#include "FSRPresentationTransport.h"
namespace TheosRenderPipeline
{
    struct FsrPresentationStatus
    {
        std::uint64_t sourceId{};Upscaling::FsrGenerationDecision decision{};
        Upscaling::FsrGenerationCallbackOutcome callback{};
        HRESULT result{S_OK};bool submitted{};
    };
    class FsrPresentation final
    {
    public:
        FsrPresentation();~FsrPresentation();
        FsrPresentation(const FsrPresentation&)=delete;FsrPresentation& operator=(const FsrPresentation&)=delete;
        static Upscaling::Result<DXGI_SWAP_CHAIN_DESC> TranslateDescriptor(const DXGI_SWAP_CHAIN_DESC&);
        static bool InternalFactoryCreation();
        Upscaling::Result<void> Create(IDXGIFactory*,std::shared_ptr<Upscaling::FsrRuntime>,
            std::shared_ptr<Graphics::D3D11D3D12Interop>,const DXGI_SWAP_CHAIN_DESC&,const Upscaling::FsrEffectProvider&);
        Upscaling::Result<void> CompleteStartup(const Upscaling::FsrGenerationLimits&,const Upscaling::FsrEffectProvider&);
        HRESULT Present(const Upscaling::UpscaleFrame&,Upscaling::UpscaleOutcome,const Upscaling::GpuFrameResources&,
            ID3D11Texture2D* scene,Upscaling::ColorEncoding,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,
            bool uiComplete,bool menu,bool requested,UINT syncInterval,UINT flags);
        HRESULT WaitBeforeProducer();
        Upscaling::Result<void> Retire();
        Upscaling::Result<void> BeforeResize();Upscaling::Result<void> AfterResize(HRESULT);
        // Borrowed interfaces are render-thread-only. A resized host replaces
        // the retired presenter with a new owner and the new fixed descriptor.
        IDXGISwapChain4* SwapChain() const;ID3D11Texture2D* SceneTarget11() const;
        std::shared_ptr<Upscaling::FsrSdkSession> Session()const;
        FsrPresentationStatus Status()const;
    private:
        struct State;std::unique_ptr<State> state_;
    };
}
