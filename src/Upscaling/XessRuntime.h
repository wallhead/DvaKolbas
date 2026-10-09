#pragma once
#include "UpscalerBackend.h"
#include <filesystem>
#include <Windows.h>
#include <xess/xess_d3d12.h>

namespace TheosRenderPipeline::Upscaling
{
    inline Result<void> ValidateXessAdapter(LUID rendering,LUID compute)
    {
        if (rendering.LowPart!=compute.LowPart || rendering.HighPart!=compute.HighPart)
            return std::unexpected(RuntimeError{ErrorKind::UnsupportedDevice,0,"XeSS D3D12 device must match the D3D11 rendering adapter LUID"});
        return {};
    }
    struct XessFunctions
    {
        decltype(&xessGetVersion) GetVersion{};
        decltype(&xessGetIntelXeFXVersion) GetIntelXeFXVersion{};
        decltype(&xessD3D12CreateContext) CreateContext{};
        decltype(&xessGetOptimalInputResolution) GetOptimalInputResolution{};
        decltype(&xessGetProperties) GetProperties{};
        decltype(&xessD3D12BuildPipelines) BuildPipelines{};
        decltype(&xessD3D12Init) Init{};
        decltype(&xessD3D12GetInitParams) GetInitParams{};
        decltype(&xessD3D12Execute) Execute{};
        decltype(&xessSetJitterScale) SetJitterScale{};
        decltype(&xessSetVelocityScale) SetVelocityScale{};
        decltype(&xessSetLoggingCallback) SetLoggingCallback{};
        decltype(&xessIsOptimalDriver) IsOptimalDriver{};
        decltype(&xessDestroyContext) DestroyContext{};
    };
    // Every context owner retains a shared_ptr to this module until destruction.
    class XessRuntime final
    {
    public:
        XessRuntime() = default;
        XessRuntime(const XessRuntime&) = delete;
        XessRuntime& operator=(const XessRuntime&) = delete;
        ~XessRuntime();
        Result<void> Load(const std::filesystem::path& pluginDirectory);
        const XessFunctions& Functions() const { return functions_; }
    private:
        HMODULE module_{};
        HANDLE file_{INVALID_HANDLE_VALUE};
        std::filesystem::path path_;
        XessFunctions functions_{};
    };
}
