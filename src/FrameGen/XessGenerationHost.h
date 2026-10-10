#pragma once
#include "XessGenerationPresentation.h"
#include "XessGenerationEngineTiming.h"
#include "XessGenerationPolicy.h"
#include "FrameTelemetry.h"
#include <filesystem>
#include <mutex>
namespace TheosRenderPipeline
{
    class XessGenerationHost final
    {
    public:
        struct PresentTiming
        {
            std::uint32_t sdkId{};
            double prepareMs{-1},proxyPresentMs{-1};
        };
        explicit XessGenerationHost(std::filesystem::path pluginDirectory);
        ~XessGenerationHost();
        XessGenerationHost(const XessGenerationHost&)=delete;
        XessGenerationHost& operator=(const XessGenerationHost&)=delete;
        Upscaling::Result<void> Create(IDXGIFactory*,ID3D11Device*,const DXGI_SWAP_CHAIN_DESC&,
            std::shared_ptr<Graphics::D3D11D3D12Interop>,std::uint32_t initFlags=0);
        Upscaling::Result<void> BindTiming(bool verifiedInstructionProfile,
            XessGenerationEngineTiming::Mode mode=XessGenerationEngineTiming::Mode::VerifiedInput);
        void RequireOrderedSources(unsigned count);
        unsigned OrderedSources()const;
        std::uint64_t DrainSuspends()const;
        std::uint64_t SkippedCycles()const;
        Upscaling::Result<std::uint32_t> BeforeSourceLoop(std::uint64_t source,std::uint64_t epoch,double* sleepMs=nullptr,std::uint32_t* sleepSdkId=nullptr);
        Upscaling::Result<void> InputSampled(std::uint64_t source);
        Upscaling::Result<void> BeforeRender(std::uint64_t source);
        HRESULT Present(const Upscaling::UpscaleFrame&,Upscaling::UpscaleOutcome,ID3D11Texture2D* ui,
            ID3D11ShaderResourceView* overlay,bool complete,bool menu,bool requested,UINT interval,UINT flags,
            bool sourceProof=true,PresentTiming* timing=nullptr);
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
        std::string Reason() const;
        bool Suspended() const;
        std::shared_ptr<Graphics::D3D11D3D12Interop> Bridge() const;
    private:
        struct State;
        // Skyrim loading and world rendering can use different threads. Native
        // publication is serialized; only engine timing is pinned to its loop.
        mutable std::recursive_mutex mutex_;
        std::unique_ptr<State> state_;
    };
}
