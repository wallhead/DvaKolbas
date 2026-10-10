#include "FrameGen/XessGenerationRuntime.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <cstdio>
#include <string>
using namespace TheosRenderPipeline;
using Microsoft::WRL::ComPtr;
namespace
{
    struct ProbeOwners
    {
        std::shared_ptr<XessGenerationRuntime> runtime;
        ComPtr<ID3D11Device> renderDevice;
        ComPtr<ID3D12Device> device;
        xefg_swapchain_handle_t fg{};
        xell_context_handle_t latency{};
    };
    void FgLog(const char* text, xefg_swapchain_logging_level_t level, void*)
    { std::printf("FG SDK level=%d %s\n",static_cast<int>(level),text); }
    void LatencyLog(const char* text, xell_logging_level_t level)
    { std::printf("XeLL SDK level=%d %s\n",static_cast<int>(level),text); }
    template<class T> bool Check(const char* stage,T result)
    { std::printf("API %s result=%d\n",stage,static_cast<int>(result));return static_cast<int>(result)==0; }
    std::string Utf8(const wchar_t* text)
    {
        const auto size=WideCharToMultiByte(CP_UTF8,0,text,-1,nullptr,0,nullptr,nullptr);
        if (size<=0) return "unavailable";
        std::string result(size,0);
        WideCharToMultiByte(CP_UTF8,0,text,-1,result.data(),size,nullptr,nullptr);
        result.pop_back();return result;
    }
}
int wmain(int argc,wchar_t** argv)
{
    if (argc!=3 || std::wstring_view(argv[1])!=L"--plugin-directory") {
        std::puts("Usage: TRPXessFgRuntimeProbe --plugin-directory <absolute SKSE/Plugins directory>");return 1;
    }
    if (NrRuntimeResearch::GameRunningOrUnknown()) { std::puts("NOT QUALIFIED: Skyrim running or guard unavailable");return 2; }
    auto owner=std::make_unique<ProbeOwners>();
    auto loaded=XessGenerationRuntime::Load(argv[2]);
    if (!loaded) { std::printf("FAIL: loader kind=%d native=%lld %s\n",static_cast<int>(loaded.error().kind),loaded.error().nativeResult,loaded.error().message.c_str());return 1; }
    owner->runtime=std::move(*loaded);
    const auto& fg=owner->runtime->Generation();
    const auto& latency=owner->runtime->Latency();
    xefg_swapchain_version_t version{};xell_version_t latencyVersion{};
    if (!Check("FG.GetVersion",fg.GetVersion(&version)) || !Check("XeLL.GetVersion",latency.GetVersion(&latencyVersion))) return 1;
    std::printf("VERSIONS FG=%u.%u.%u XeLL=%u.%u.%u SDK_bundle=3.0.2\n",version.major,version.minor,version.patch,latencyVersion.major,latencyVersion.minor,latencyVersion.patch);
    const auto hr=D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&owner->renderDevice,nullptr,nullptr);
    if (FAILED(hr)) { std::printf("NOT QUALIFIED: D3D11 device HRESULT=%08lx\n",static_cast<unsigned long>(hr));return 2; }
    ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;DXGI_ADAPTER_DESC desc{};
    if (FAILED(owner->renderDevice.As(&dxgi)) || FAILED(dxgi->GetAdapter(&adapter)) || FAILED(adapter->GetDesc(&desc))) return 1;
    std::printf("ADAPTER %s vendor=%04x device=%04x renderLUID=%08lx:%08lx\n",Utf8(desc.Description).c_str(),desc.VendorId,desc.DeviceId,
        static_cast<unsigned long>(desc.AdapterLuid.HighPart),static_cast<unsigned long>(desc.AdapterLuid.LowPart));
    ComPtr<IDXGIFactory4> factory;ComPtr<IDXGIAdapter1> matching;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))) || FAILED(factory->EnumAdapterByLuid(desc.AdapterLuid,IID_PPV_ARGS(&matching)))) return 1;
    const auto nativeHr=D3D12CreateDevice(matching.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&owner->device));
    if (FAILED(nativeHr)) { std::printf("NOT QUALIFIED: same-adapter D3D12 HRESULT=%08lx\n",static_cast<unsigned long>(nativeHr));return 2; }
    const auto luid=owner->device->GetAdapterLuid();
    if (luid.LowPart!=desc.AdapterLuid.LowPart || luid.HighPart!=desc.AdapterLuid.HighPart) { std::puts("FAIL: native/render adapter mismatch");return 1; }
    D3D12_FEATURE_DATA_SHADER_MODEL shader{D3D_SHADER_MODEL_6_4};
    const auto smHr=owner->device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&shader,sizeof(shader));
    std::printf("CAPABILITY same_adapter=1 shader_model=%x HRESULT=%08lx\n",shader.HighestShaderModel,static_cast<unsigned long>(smHr));
    if (FAILED(smHr) || (desc.VendorId!=0x8086 && shader.HighestShaderModel<D3D_SHADER_MODEL_6_4)) { std::puts("NOT QUALIFIED: required shader model unavailable");return 2; }
    auto cleanup=[&](int result) {
        if (owner->fg && !Check("FG.Destroy",fg.Destroy(owner->fg))) {
            owner.release();std::puts("FAIL: retaining contexts/devices/modules until process exit after FG destruction failure");return 1;
        }
        owner->fg=nullptr;
        if (owner->latency && !Check("XeLL.DestroyContext",latency.DestroyContext(owner->latency))) {
            owner.release();std::puts("FAIL: retaining XeLL/device/module until process exit after destruction failure");return 1;
        }
        owner->latency=nullptr;return result;
    };
    if (!Check("XeLL.CreateContext",latency.CreateContext(owner->device.Get(),&owner->latency)) || !owner->latency) return cleanup(2);
    if (!Check("XeLL.SetLoggingCallback",latency.SetLoggingCallback(owner->latency,XELL_LOGGING_LEVEL_WARNING,LatencyLog))) return cleanup(2);
    if (!Check("FG.CreateContext",fg.CreateContext(owner->device.Get(),&owner->fg)) || !owner->fg) return cleanup(2);
    if (!Check("FG.SetLoggingCallback",fg.SetLoggingCallback(owner->fg,XEFG_SWAPCHAIN_LOGGING_LEVEL_WARNING,FgLog,nullptr)) ||
        !Check("FG.SetLatencyReduction",fg.SetLatencyReduction(owner->fg,owner->latency))) return cleanup(2);
    xefg_swapchain_properties_t properties{};
    if (!Check("FG.GetProperties",fg.GetProperties(owner->fg,&properties))) return cleanup(2);
    std::printf("PROPERTIES max_interpolated=%u requested=1\n",properties.maxSupportedInterpolations);
    if (properties.maxSupportedInterpolations<1) return cleanup(2);
    xefg_swapchain_d3d12_init_params_t init{};
    init.maxInterpolatedFrames=1;init.uiMode=XEFG_SWAPCHAIN_UI_MODE_HUDLESS_UITEXTURE;
    if (!Check("FG.GetD3D12Properties",fg.GetD3D12Properties(owner->fg,&init,1280,720,DXGI_FORMAT_R8G8B8A8_UNORM,&properties))) return cleanup(2);
    std::printf("MEMORY descriptors=%u buffer=%llu texture=%llu constants=%llu\n",properties.requiredDescriptorCount,
        properties.tempBufferHeapSize,properties.tempTextureHeapSize,properties.constantBufferSize);
    if (cleanup(0)!=0) return 1;
    owner.reset();
    if (GetModuleHandleW(L"libxess_fg.dll") || GetModuleHandleW(L"libxell.dll")) { std::puts("FAIL: modules remain after context destruction");return 1; }
    std::puts("PASS: physical same-adapter Intel FG/XeLL contexts, properties and ordered destruction; frame production NOT YET QUALIFIED");
    return 0;
}
