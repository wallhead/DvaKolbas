#pragma once

#include "UpscalerBackend.h"
#include <ffx_api.h>
#include <filesystem>
#include <vector>
#include <windows.h>

struct ID3D12Device;

namespace TheosRenderPipeline::Upscaling
{
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
        Result<void> Load(const std::filesystem::path& pluginDirectory);
        Result<std::vector<ProviderInfo>> Enumerate(ID3D12Device*);
        Result<Extent> QueryRenderExtent(ID3D12Device*, const ProviderInfo&, Quality, Extent);
        Result<ProviderInfo> QueryActualProvider(ffxContext&);
        Result<void> VerifyActualProvider(ffxContext&, const ProviderInfo&);
        const FsrFunctions& Functions() const { return functions_; }
    private:
        void Unload();
        HMODULE loader_{}, upscaler_{};
        FsrFunctions functions_{};
    };
}
