#pragma once
#include "Upscaling/UpscalerBackend.h"
#include <filesystem>
#include <memory>
#include <xess_fg/xefg_swapchain_d3d12.h>
#include <xell/xell_d3d12.h>

namespace TheosRenderPipeline
{
    struct XessGenerationFunctions
    {
        decltype(&xefgSwapChainGetVersion) GetVersion{};
        decltype(&xefgSwapChainD3D12CreateContext) CreateContext{};
        decltype(&xefgSwapChainD3D12BuildPipelines) BuildPipelines{};
        decltype(&xefgSwapChainGetProperties) GetProperties{};
        decltype(&xefgSwapChainD3D12GetProperties) GetD3D12Properties{};
        decltype(&xefgSwapChainD3D12InitFromSwapChainDesc) InitFromSwapChainDesc{};
        decltype(&xefgSwapChainD3D12GetSwapChainPtr) GetSwapChainPtr{};
        decltype(&xefgSwapChainD3D12GetInitializationParameters) GetInitializationParameters{};
        decltype(&xefgSwapChainD3D12TagFrameResource) TagFrameResource{};
        decltype(&xefgSwapChainTagFrameConstants) TagFrameConstants{};
        decltype(&xefgSwapChainSetEnabled) SetEnabled{};
        decltype(&xefgSwapChainSetPresentId) SetPresentId{};
        decltype(&xefgSwapChainGetLastPresentStatus) GetLastPresentStatus{};
        decltype(&xefgSwapChainSetLoggingCallback) SetLoggingCallback{};
        decltype(&xefgSwapChainSetLatencyReduction) SetLatencyReduction{};
        decltype(&xefgSwapChainSetUiCompositionState) SetUiCompositionState{};
        decltype(&xefgSwapChainDestroy) Destroy{};
    };
    struct XellFunctions
    {
        decltype(&xellGetVersion) GetVersion{};
        decltype(&xellD3D12CreateContext) CreateContext{};
        decltype(&xellSetSleepMode) SetSleepMode{};
        decltype(&xellSleep) Sleep{};
        decltype(&xellAddMarkerData) AddMarkerData{};
        decltype(&xellSetLoggingCallback) SetLoggingCallback{};
        decltype(&xellDestroyContext) DestroyContext{};
    };
    class XessGenerationRuntime final
    {
    public:
        ~XessGenerationRuntime();
        XessGenerationRuntime(const XessGenerationRuntime&) = delete;
        XessGenerationRuntime& operator=(const XessGenerationRuntime&) = delete;
        static Upscaling::Result<std::shared_ptr<XessGenerationRuntime>> Load(const std::filesystem::path& pluginDirectory);
        const XessGenerationFunctions& Generation() const { return generation_; }
        const XellFunctions& Latency() const { return latency_; }
    private:
        XessGenerationRuntime();
        struct Modules;
        std::unique_ptr<Modules> modules_;
        XessGenerationFunctions generation_{};
        XellFunctions latency_{};
    };
}
