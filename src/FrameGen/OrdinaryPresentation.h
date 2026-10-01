#pragma once
#include "Graphics/D3D11D3D12Interop.h"
#include <dxgi.h>
#include <memory>
namespace TheosRenderPipeline
{
    using OriginalCreateSwapChain=decltype(&IDXGIFactory::CreateSwapChain);
    class OrdinaryPresentation final
    {
    public:
        OrdinaryPresentation();~OrdinaryPresentation();
        OrdinaryPresentation(const OrdinaryPresentation&)=delete;
        OrdinaryPresentation& operator=(const OrdinaryPresentation&)=delete;
        HRESULT CreateSwapChain(IDXGIFactory*,ID3D11Device*,const DXGI_SWAP_CHAIN_DESC&,IDXGISwapChain**,OriginalCreateSwapChain);
        bool Ready() const;
        HRESULT Retire();HRESULT BeforeResize();HRESULT AfterResize(HRESULT);
        void ResetAfterRetirement();
        void SetRetirementWaitPolicy(Graphics::RetirementWaitPolicy);
    private:
        struct State;std::unique_ptr<State> state_;
    };
}
