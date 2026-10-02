#include "Upscaling/FSRRuntime.h"
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <cstdio>
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
static int Fail(const RuntimeError& error)
{ std::fprintf(stderr, "FAIL kind=%u native=%lld: %s\n", unsigned(error.kind), static_cast<long long>(error.nativeResult), error.message.c_str()); return 1; }
int main(int argc, char** argv)
{
    if (argc != 2) { std::fprintf(stderr, "Usage: TRPFsrGenerationProviderProbe <absolute plugin directory with FSR subfolder>\n"); return 2; }
    FsrRuntime runtime;
    if (auto loaded = runtime.Load(std::filesystem::path(argv[1])); !loaded) return Fail(loaded.error());
    if (auto loaded = runtime.LoadFrameGeneration(std::filesystem::path(argv[1])); !loaded) return Fail(loaded.error());
    ComPtr<ID3D11Device> producer;
    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &producer, nullptr, nullptr))) return 1;
    ComPtr<IDXGIDevice> dxgi; ComPtr<IDXGIAdapter> adapter; DXGI_ADAPTER_DESC adapterDesc{};
    if (FAILED(producer.As(&dxgi)) || FAILED(dxgi->GetAdapter(&adapter)) || FAILED(adapter->GetDesc(&adapterDesc))) return 1;
    ComPtr<ID3D12Device> device;
    if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)))) return 1;
    const auto luid = device->GetAdapterLuid();
    if (luid.HighPart != adapterDesc.AdapterLuid.HighPart || luid.LowPart != adapterDesc.AdapterLuid.LowPart) return 1;
    std::printf("Adapter=%ls LUID=%08lx:%08lx\n", adapterDesc.Description, static_cast<unsigned long>(luid.HighPart), luid.LowPart);
    for (const auto effect : {FsrEffect::Upscale, FsrEffect::FrameGeneration, FsrEffect::FrameGenerationSwapChain}) {
        auto providers = runtime.EnumerateForEffect(device.Get(), effect); if (!providers) return Fail(providers.error());
        for (const auto& provider : *providers)
            std::printf("Effect=%u id=%llu name=%s\n", unsigned(provider.effect), static_cast<unsigned long long>(provider.identity.id), provider.identity.name.c_str());
    }
    std::puts("PASS: real effect catalogs queried; no generation or presentation tested");
}
