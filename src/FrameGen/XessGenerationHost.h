#pragma once
#include "XessGenerationPresentation.h"
#include "XessGenerationEngineTiming.h"
#include "XessGenerationPolicy.h"
#include "FrameTelemetry.h"
#include <filesystem>
namespace TheosRenderPipeline
{
    class XessGenerationHost final
    {
    public:
        explicit XessGenerationHost(std::filesystem::path pluginDirectory);
        ~XessGenerationHost();
        XessGenerationHost(const XessGenerationHost&)=delete;
        XessGenerationHost& operator=(const XessGenerationHost&)=delete;
        Upscaling::Result<void> Create(IDXGIFactory*,ID3D11Device*,const DXGI_SWAP_CHAIN_DESC&,
            std::shared_ptr<Graphics::D3D11D3D12Interop>,std::uint32_t initFlags=0);
        Upscaling::Result<void> BindTiming(bool verifiedInstructionProfile);
        void RequireOrderedSources(unsigned count);
        unsigned OrderedSources()const;
        Upscaling::Result<std::uint32_t> BeforeSourceLoop(std::uint64_t source,std::uint64_t epoch);
        Upscaling::Result<void> InputSampled(std::uint64_t source);
        Upscaling::Result<void> BeforeRender(std::uint64_t source);
        HRESULT Present(const Upscaling::UpscaleFrame&,Upscaling::UpscaleOutcome,ID3D11Texture2D* ui,
            ID3D11ShaderResourceView* overlay,bool complete,bool menu,bool requested,UINT interval,UINT flags);
        HRESULT WaitBeforeProducer();
        HRESULT StartupPresent(UINT interval,UINT flags);
        Upscaling::Result<void> Suspend();
        Upscaling::Result<void> Resume();
        Upscaling::Result<void> Resize(const DXGI_SWAP_CHAIN_DESC&);
        Upscaling::Result<void> Retire();
        IDXGISwapChain4* SwapChain() const;
        HRESULT GetProducerDevice(REFIID,void**) const;
        xefg_swapchain_present_status_t Status() const;
        Telemetry::OutputCounter OutputCounter() const;
        const std::string& Reason() const;
        bool Suspended() const;
        std::shared_ptr<Graphics::D3D11D3D12Interop> Bridge() const;
    private:
        struct State;
        std::unique_ptr<State> state_;
    };
}
