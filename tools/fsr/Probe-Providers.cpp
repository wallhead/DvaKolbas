#include "Upscaling/FSRRuntime.h"
#include "Upscaling/FSRProviderPolicy.h"
#include <ffx_upscale.h>
#include <dx12/ffx_api_dx12.h>
#include <d3d11.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <cstdio>
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
static int Fail(const RuntimeError& error)
{ std::fprintf(stderr,"FSR error kind=%u native=%lld: %s\n",unsigned(error.kind),static_cast<long long>(error.nativeResult),error.message.c_str()); return 1; }
int main(int argc, char** argv)
{
    if(argc != 2) { std::fprintf(stderr,"Usage: TRPFsrProviderProbe <absolute plugin directory containing FSR DLL subdirectory>\n"); return 2; }
    FsrRuntime runtime; auto loaded=runtime.Load(std::filesystem::path(argv[1])); if(!loaded) return Fail(loaded.error());
    ComPtr<ID3D11Device> device11;
    auto hr=D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device11,nullptr,nullptr);
    if(FAILED(hr)) { std::fprintf(stderr,"D3D11 device failed: %08lx\n",hr);return 1; }
    ComPtr<IDXGIDevice> dxgiDevice; ComPtr<IDXGIAdapter> adapter; DXGI_ADAPTER_DESC desc{};
    if(FAILED(device11.As(&dxgiDevice)) || FAILED(dxgiDevice->GetAdapter(&adapter)) || FAILED(adapter->GetDesc(&desc))) return 1;
    ComPtr<ID3D12Device> device12;
    if(FAILED(hr=D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device12)))) { std::fprintf(stderr,"D3D12 device failed: %08lx\n",hr);return 1; }
    const auto luid=device12->GetAdapterLuid();
    if(luid.HighPart != desc.AdapterLuid.HighPart || luid.LowPart != desc.AdapterLuid.LowPart) return 1;
    std::printf("Same adapter LUID=%08lx:%08lx\n",static_cast<unsigned long>(luid.HighPart),luid.LowPart);
    auto providers=runtime.Enumerate(device12.Get()); if(!providers) return Fail(providers.error());
    for(const auto& provider:*providers) std::printf("Discovered provider id=%llu name=%s\n",static_cast<unsigned long long>(provider.id),provider.name.c_str());
    auto selected=SelectProvider(*providers,ProviderPolicy::Analytical); if(!selected) return Fail(selected.error());
    Extent output{1921,1081};auto render=runtime.QueryRenderExtent(device12.Get(),*selected,Quality::Performance,output); if(!render) return Fail(render.error());
    std::printf("Selected id=%llu render=%ux%u output=%ux%u\n",static_cast<unsigned long long>(selected->id),render->width,render->height,output.width,output.height);
    ffxCreateContextDescUpscale create{};create.header.type=FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE;
    create.flags=FFX_UPSCALE_ENABLE_AUTO_EXPOSURE;create.maxRenderSize={render->width,render->height};create.maxUpscaleSize={output.width,output.height};
    ffxCreateBackendDX12Desc backend{{FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12,nullptr},device12.Get()};
    ffxCreateContextDescUpscaleVersion version{{FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE_VERSION,nullptr},FFX_UPSCALER_VERSION};
    ffxOverrideVersion override{{FFX_API_DESC_TYPE_OVERRIDE_VERSION,nullptr},selected->id};
    create.header.pNext=&backend.header;backend.header.pNext=&version.header;version.header.pNext=&override.header;
    ffxContext context{};const auto created=runtime.Functions().CreateContext(&context,&create.header,nullptr);
    if(created != FFX_API_RETURN_OK) return Fail({ErrorKind::ContextFailure,created,"Official FSR context creation failed"});
    const auto verified=runtime.VerifyActualProvider(context,*selected);
    const auto actual=runtime.QueryActualProvider(context);
    // No commands were recorded/dispatched: no GPU readers reference this context.
    const auto destroyed=runtime.Functions().DestroyContext(&context,nullptr);
    if(!verified) return Fail(verified.error());if(!actual) return Fail(actual.error());
    if(destroyed != FFX_API_RETURN_OK) return Fail({ErrorKind::ContextFailure,destroyed,"Official FSR context destruction failed"});
    std::printf("Actual provider id=%llu name=%s\nPASS: official provider discovery, sizing, context creation/destruction; no image dispatch tested\n",static_cast<unsigned long long>(actual->id),actual->name.c_str());
}
