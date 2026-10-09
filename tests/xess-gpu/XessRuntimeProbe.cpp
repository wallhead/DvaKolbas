#include "Upscaling/XessRuntime.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include <d3d11.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <chrono>
#include <cstdio>
#include <memory>
#include <string>
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
namespace
{
    bool Check(const char* stage,xess_result_t result)
    {
        std::printf("API %s result=%d\n",stage,static_cast<int>(result));
        return result==XESS_RESULT_SUCCESS;
    }
    void Log(const char* message,xess_logging_level_t level)
    { std::printf("SDK level=%d %s\n",static_cast<int>(level),message); }
    std::string Utf8(const wchar_t* text)
    {
        const auto size=WideCharToMultiByte(CP_UTF8,0,text,-1,nullptr,0,nullptr,nullptr);
        if (size<=0) return "unavailable";
        std::string result(static_cast<size_t>(size),0);
        WideCharToMultiByte(CP_UTF8,0,text,-1,result.data(),size,nullptr,nullptr);
        result.pop_back();return result;
    }
}
int wmain(int argc,wchar_t** argv)
{
    if (argc!=3 || std::wstring_view(argv[1])!=L"--plugin-directory") {
        std::puts("Usage: TRPXessRuntimeProbe --plugin-directory <absolute SKSE/Plugins directory>");return 1;
    }
    if (NrRuntimeResearch::GameRunningOrUnknown()) { std::puts("NOT QUALIFIED: Skyrim running or process guard unavailable");return 2; }
    auto runtime=std::make_shared<XessRuntime>();
    const auto loaded=runtime->Load(argv[2]);
    if (!loaded) { std::printf("FAIL: loader kind=%d native=%lld %s\n",static_cast<int>(loaded.error().kind),loaded.error().nativeResult,loaded.error().message.c_str());return 1; }
    const auto& api=runtime->Functions();
    xess_version_t version{};
    if (!Check("GetVersion",api.GetVersion(&version))) return 1;
    std::printf("Dispatcher API=%u.%u.%u SDK bundle pin=3.0.2\n",version.major,version.minor,version.patch);
    ComPtr<ID3D11Device> device11;
    auto hr=D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device11,nullptr,nullptr);
    if (FAILED(hr)) { std::printf("NOT QUALIFIED: D3D11CreateDevice HRESULT=%08lx\n",static_cast<unsigned long>(hr));return 2; }
    ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;DXGI_ADAPTER_DESC description{};
    if (FAILED(device11.As(&dxgi)) || FAILED(dxgi->GetAdapter(&adapter)) || FAILED(adapter->GetDesc(&description))) return 1;
    std::printf("Adapter=%s vendor=%04x device=%04x D3D11 LUID=%08lx:%08lx\n",Utf8(description.Description).c_str(),description.VendorId,description.DeviceId,
        static_cast<unsigned long>(description.AdapterLuid.HighPart),static_cast<unsigned long>(description.AdapterLuid.LowPart));
    ComPtr<IDXGIFactory4> factory;ComPtr<IDXGIAdapter1> matching;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))) || FAILED(factory->EnumAdapterByLuid(description.AdapterLuid,IID_PPV_ARGS(&matching)))) return 1;
    ComPtr<ID3D12Device> device12;
    hr=D3D12CreateDevice(matching.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device12));
    if (FAILED(hr)) { std::printf("NOT QUALIFIED: D3D12CreateDevice HRESULT=%08lx\n",static_cast<unsigned long>(hr));return 2; }
    const auto compute=device12->GetAdapterLuid();
    if (!ValidateXessAdapter(description.AdapterLuid,compute)) return 1;
    std::printf("D3D12 LUID=%08lx:%08lx MATCH\n",static_cast<unsigned long>(compute.HighPart),static_cast<unsigned long>(compute.LowPart));
    const xess_2d_t output{1280,720};
    struct Mode { const char* name;xess_quality_settings_t quality; };
    for (const auto mode : {Mode{"Native",XESS_QUALITY_SETTING_AA},Mode{"Quality",XESS_QUALITY_SETTING_QUALITY},Mode{"Performance",XESS_QUALITY_SETTING_PERFORMANCE}}) {
        xess_context_handle_t context{};
        const auto create=api.CreateContext(device12.Get(),&context);
        if (!Check("D3D12CreateContext",create) || !context) { std::puts("NOT QUALIFIED: actual SDK context unavailable");return 2; }
        bool ok=Check("SetLoggingCallback",api.SetLoggingCallback(context,XESS_LOGGING_LEVEL_WARNING,Log));
        xess_version_t implementation{};
        ok=Check("GetIntelXeFXVersion",api.GetIntelXeFXVersion(context,&implementation)) && ok;
        std::printf("Intel implementation=%u.%u.%u (0.0.0 means non-Intel; not an invented implementation version)\n",implementation.major,implementation.minor,implementation.patch);
        const auto driver=api.IsOptimalDriver(context);
        std::printf("API IsOptimalDriver result=%d\n",static_cast<int>(driver));
        ok=(driver==XESS_RESULT_SUCCESS || driver==XESS_RESULT_WARNING_OLD_DRIVER) && ok;
        xess_2d_t optimal{},minimum{},maximum{};
        ok=Check("GetOptimalInputResolution",api.GetOptimalInputResolution(context,&output,mode.quality,&optimal,&minimum,&maximum)) && ok;
        std::printf("MODE %s output=%ux%u optimal=%ux%u min=%ux%u max=%ux%u\n",mode.name,output.x,output.y,optimal.x,optimal.y,minimum.x,minimum.y,maximum.x,maximum.y);
        ok=optimal.x>0 && optimal.y>0 && optimal.x<=output.x && optimal.y<=output.y && minimum.x<=optimal.x && minimum.y<=optimal.y && optimal.x<=maximum.x && optimal.y<=maximum.y && ok;
        if (mode.quality==XESS_QUALITY_SETTING_AA) ok=optimal.x==output.x && optimal.y==output.y && ok;
        const auto start=std::chrono::steady_clock::now();
        constexpr uint32_t flags=XESS_INIT_FLAG_LDR_INPUT_COLOR;
        if (ok) ok=Check("D3D12BuildPipelines",api.BuildPipelines(context,nullptr,true,flags));
        xess_d3d12_init_params_t parameters{};
        parameters.outputResolution=output;parameters.qualitySetting=mode.quality;parameters.initFlags=flags;
        if (ok) ok=Check("D3D12Init",api.Init(context,&parameters));
        std::printf("Initialization duration_ms=%.2f flags=%u\n",std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count(),flags);
        if (ok) {
            xess_d3d12_init_params_t actual{};
            ok=Check("D3D12GetInitParams",api.GetInitParams(context,&actual));
            std::printf("Init readback output=%ux%u quality=%d flags=%u requested quality=%d flags=%u\n",actual.outputResolution.x,actual.outputResolution.y,static_cast<int>(actual.qualitySetting),actual.initFlags,static_cast<int>(mode.quality),flags);
            // Pinned 2.0.2 dispatcher clears bit 0x40 before forwarding Init (RVA 0x1a6a30).
            // This probe qualifies creation/sizing, never the still-untested colour behavior.
            const bool knownLdrRemoval=version.major==2 && version.minor==0 && version.patch==2 && actual.initFlags==(flags & ~XESS_INIT_FLAG_LDR_INPUT_COLOR);
            if (knownLdrRemoval) std::puts("WARNING: dispatcher clears requested LDR flag; colour/tonemap behavior NOT YET QUALIFIED");
            ok=ok && actual.outputResolution.x==output.x && actual.outputResolution.y==output.y && actual.qualitySetting==mode.quality && (actual.initFlags==flags || knownLdrRemoval);
        }
        // No execute/submission in this probe. SDK initialization retires its own upload work.
        const auto destroy=api.DestroyContext(context);
        if (!Check("DestroyContext",destroy)) { std::puts("FAIL: SDK destruction failed; retaining module until process exit");new std::shared_ptr<XessRuntime>(runtime);return 1; }
        if (!ok) { std::puts("NOT QUALIFIED: initialization/sizing contract failed");return 2; }
    }
    if (GetModuleHandleW(L"sl.interposer.dll") || GetModuleHandleW(L"nvngx.dll")) { std::puts("FAIL: unexpected NVIDIA SDK loader dependency");return 1; }
    runtime.reset();
    std::puts("PASS: real same-adapter Native/Quality/Performance initialization, sizing and clean destruction; temporal execution NOT YET QUALIFIED");
    return 0;
}
