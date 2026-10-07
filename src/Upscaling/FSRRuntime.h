#pragma once

#include "UpscalerBackend.h"
#include "FSRRuntimeProfile.h"
#include <ffx_api.h>
#include <filesystem>
#include <vector>
#include <windows.h>

struct ID3D12Device;

namespace TheosRenderPipeline::Upscaling
{
    enum class FsrEffect { Upscale, FrameGeneration, FrameGenerationSwapChain };
    struct FsrEffectProvider { FsrEffect effect; ProviderInfo identity; };
    Result<FsrEffectProvider> SelectFsrEffectProvider(const std::vector<FsrEffectProvider>&, FsrEffect);
    struct FsrFunctions
    {
        PfnFfxCreateContext CreateContext{};
        PfnFfxDestroyContext DestroyContext{};
        PfnFfxConfigure Configure{};
        PfnFfxQuery Query{};
        PfnFfxDispatch Dispatch{};
    };

    // A context owner must retain this runtime until GPU retirement and context
    // destruction. No import library or global DLL search-path changes are used.
    class FsrRuntime final
    {
    public:
        FsrRuntime() = default;
        ~FsrRuntime();
        FsrRuntime(const FsrRuntime&) = delete;
        FsrRuntime& operator=(const FsrRuntime&) = delete;
        Result<void> Load(const std::filesystem::path& pluginDirectory, FsrRuntimeProfile profile = FsrRuntimeProfile::Official);
        Result<void> LoadFrameGeneration(const std::filesystem::path& pluginDirectory);
        Result<std::vector<ProviderInfo>> Enumerate(ID3D12Device*);
        Result<std::vector<FsrEffectProvider>> EnumerateForEffect(ID3D12Device*, FsrEffect);
        Result<Extent> QueryRenderExtent(ID3D12Device*, const ProviderInfo&, Quality, Extent);
        Result<ProviderInfo> QueryActualProvider(ffxContext&);
        Result<void> VerifyActualProvider(ffxContext&, const ProviderInfo&);
        Result<void> VerifyActualProvider(ffxContext&, const FsrEffectProvider&);
        const FsrFunctions& Functions() const { return functions_; }
        FsrRuntimeProfile Profile() const { return profile_; }
        uint32_t UpscaleApiVersion() const { return FsrUpscaleApiVersion(profile_); }
    private:
        void Unload();
        Result<std::vector<ProviderInfo>> EnumerateType(ID3D12Device*, uint64_t createDescType);
        HMODULE loader_{}, upscaler_{}, frameGeneration_{};
        std::array<HANDLE,2> pinnedFiles_{INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE};
        FsrRuntimeProfile profile_{FsrRuntimeProfile::Official};
        FsrFunctions functions_{};
    };
}
