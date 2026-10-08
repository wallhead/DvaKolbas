// Render hooks derived from PureDark's MIT-licensed Skyrim-Upscaler
// (https://github.com/PureDark/Skyrim-Upscaler).

#include <PCH.h>
#include "UpscalerDeviceHooks.h"
#include "UpscalerHooks.h"
#include "DLSSBackend.h"
#include "RenderPipeline.h"
#include "FrameGen/NvidiaHost.h"
#include "FrameGen/SourceFrameGeneration.h"
#include "FrameGen/SourceHostBoundary.h"
#include "OverlayUI.h"
#include "PerformanceTuning.h"
#include "CommunityShaderIntegration.h"
#include "HookInstallation.h"
#include "SettingsFile.h"
#include "PluginPaths.h"
#include "NvidiaAppSettings.h"
#include "RendererGpuPolicy.h"
#include "RendererGpuSupport.h"
#include "RendererStartupValidation.h"
#include <SimpleIni.h>

decltype(&D3D11CreateDeviceAndSwapChain) ptrD3D11CreateDeviceAndSwapChain;
decltype(&IDXGIFactory::CreateSwapChain) ptrFactoryCreateSwapChain;

void BeforeGameSwapChainPresent(IDXGISwapChain* a_swapChain)
{
    // Alt-tab freeze diagnosis: a long gap between presents tells us whether
    // the game loop wedged (gap here) or only the display path stalled.
    static LARGE_INTEGER lastPresent{};
    static LARGE_INTEGER qpcFreq{};
    if (!qpcFreq.QuadPart)
    {
        ::QueryPerformanceFrequency(&qpcFreq);
    }
    LARGE_INTEGER now{};
    ::QueryPerformanceCounter(&now);
    if (lastPresent.QuadPart)
    {
        const double gap =
            static_cast<double>(now.QuadPart - lastPresent.QuadPart) / static_cast<double>(qpcFreq.QuadPart);
        if (gap > 1.0)
        {
            logger::info("[Present] gap of {:.2f}s between presents", gap);
        }
    }
    lastPresent = now;

    auto* nvidiaHost = NvidiaHost::GetSingleton();
    if (TheosRenderPipeline::CommunityShaders::Active()) {
        nvidiaHost->PrepareCommunityFrameForPresent();
        PerformanceTuning::GetSingleton()->EndD3D11Frame(RenderPipeline::GetSingleton()->mContext);
        return;
    }
    if (nvidiaHost->ProxyActive() && nvidiaHost->StartupConfigured())
    {
        nvidiaHost->PrepareSourceFrameForPresent(a_swapChain);
        OverlayUI::GetSingleton()->OnPresent();
        nvidiaHost->FinishNativeUIPassForPresent();
    }
    else
    {
        OverlayUI::GetSingleton()->OnPresent();
    }
    // End GPU work before the potentially blocking native Present. Leaving this
    // open until the next renderer begin includes presentation/idle time.
    PerformanceTuning::GetSingleton()->EndD3D11Frame(RenderPipeline::GetSingleton()->mContext);
}

HRESULT WINAPI hk_IDXGIFactory_CreateSwapChain(IDXGIFactory* This, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc,
                                               IDXGISwapChain** ppSwapChain)
{
#if defined(TRP_ENABLE_FSR_FG)
    if (TheosRenderPipeline::FsrPresentation::InternalFactoryCreation()) {
        return (This->*ptrFactoryCreateSwapChain)(pDevice,pDesc,ppSwapChain);
    }
#endif
    auto nvidiaHost = NvidiaHost::GetSingleton();

    // The vtable detour is class-wide: later swapchain creations from other
    // components must not re-enter proxy setup over live state.
    if (nvidiaHost->ProxyActive())
    {
        logger::info("[FrameGen] additional CreateSwapChain after proxy setup; passing through");
        return (This->*ptrFactoryCreateSwapChain)(pDevice, pDesc, ppSwapChain);
    }

    ID3D11Device* d3d11Device = nullptr;
    if (!pDesc || !ppSwapChain || !pDevice || FAILED(pDevice->QueryInterface(IID_PPV_ARGS(&d3d11Device))))
    {
        logger::warn("[FrameGen] factory CreateSwapChain without a D3D11 device; passing through");
        return (This->*ptrFactoryCreateSwapChain)(pDevice, pDesc, ppSwapChain);
    }

    // The stable render-sized buffer is allocated inside CreateSwapChain, so
    // load its quality/sharpening contract before that one-way size decision.
    Microsoft::WRL::ComPtr<IDXGIDevice> rendererDxgi;
    Microsoft::WRL::ComPtr<IDXGIAdapter> rendererAdapter;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> rendererAdapter1;
    DXGI_ADAPTER_DESC1 rendererDesc{};
    auto adapterResult = d3d11Device->QueryInterface(IID_PPV_ARGS(&rendererDxgi));
    if (SUCCEEDED(adapterResult)) adapterResult = rendererDxgi->GetAdapter(&rendererAdapter);
    if (SUCCEEDED(adapterResult)) adapterResult = rendererAdapter.As(&rendererAdapter1);
    if (SUCCEEDED(adapterResult)) adapterResult = rendererAdapter1->GetDesc1(&rendererDesc);
    if (FAILED(adapterResult)) {
        logger::critical("[Renderer GPU] actual rendering adapter query failed HRESULT=0x{:08X}", static_cast<unsigned>(adapterResult));
        d3d11Device->Release();
        return adapterResult;
    }
    auto* pipeline = RenderPipeline::GetSingleton();
    pipeline->mAdapterVendorId = rendererDesc.VendorId;
    TheosRenderPipeline::NeuralRendering::AdapterIdentity identity{
        rendererDesc.VendorId, rendererDesc.DeviceId, rendererDesc.SubSysId,
        {rendererDesc.AdapterLuid.LowPart,rendererDesc.AdapterLuid.HighPart},
        (rendererDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)!=0};
    TheosRenderPipeline::NeuralRendering::DiscoverGpuArchitecture(identity);
    // A null output validates device support without creating another device.
    // S_FALSE is success for this probe; both hosts require feature level 12_0.
    const auto d3d12Support = D3D12CreateDevice(rendererAdapter.Get(), D3D_FEATURE_LEVEL_12_0,
        __uuidof(ID3D12Device), nullptr);
    const auto gpuChoice = TheosRenderPipeline::ClassifyRendererGpu(identity,rendererDesc.Description,
        d3d11Device->GetFeatureLevel()>=D3D_FEATURE_LEVEL_11_0, SUCCEEDED(d3d12Support));
    char rendererName[512]{};
    const bool named = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, rendererDesc.Description,
        -1,rendererName,sizeof(rendererName),nullptr,nullptr)>0;
    logger::info("[Renderer GPU] name={} vendor=0x{:04X} device=0x{:04X} LUID={:08X}:{:08X} arch=0x{:X} queried={} RTX={} NR-mapping-ambiguous={} D3D12-FL12_0=0x{:08X} route={}",
        named?rendererName:"<unknown>",
        identity.vendorId,identity.deviceId,static_cast<unsigned>(identity.luid.high),identity.luid.low,
        identity.architecture.id,identity.architecture.queried,identity.architecture.rtxProduct,identity.architecture.mappingAmbiguous,
        static_cast<unsigned>(d3d12Support),
        gpuChoice==TheosRenderPipeline::RendererGpuChoice::NvidiaRtx?"NVIDIA-RTX":
        gpuChoice==TheosRenderPipeline::RendererGpuChoice::FsrOnly?"FSR-only":"unsupported");
    if (gpuChoice==TheosRenderPipeline::RendererGpuChoice::Unsupported) {
        nvidiaHost->FailLifecycle(DXGI_ERROR_UNSUPPORTED,
            "Rendering GPU/driver requires a hardware adapter with Direct3D 11 feature level 11_0 and Direct3D 12 feature level 12_0. Update the GPU driver or select a compatible GPU; vendor backends were not started.");
        d3d11Device->Release();
        return DXGI_ERROR_UNSUPPORTED;
    }
    pipeline->mFsrOnlyRenderer=gpuChoice==TheosRenderPipeline::RendererGpuChoice::FsrOnly;
    CSimpleIniA startup;
    startup.SetUnicode();
    const auto configError = TheosRenderPipeline::SettingsFile::LoadRenderer(startup, L"Data\\SKSE\\Plugins\\RaZkolbaS.ini").second;
    if (!configError.empty()) {
        nvidiaHost->FailLifecycle(E_INVALIDARG, configError.c_str());
        d3d11Device->Release();
        return E_INVALIDARG;
    }
    const auto startupError = TheosRenderPipeline::ValidateRendererStartup(startup, rendererDesc.VendorId,
#if defined(TRP_ENABLE_FSR)
        true,
#else
        false,
#endif
#if defined(TRP_ENABLE_FSR_FG)
        true
#else
        false
#endif
        , pipeline->mFsrOnlyRenderer
    );
    if (!startupError.empty()) {
        logger::critical("[Renderer GPU] normalized startup configuration rejected: {}", startupError);
        nvidiaHost->FailLifecycle(E_INVALIDARG, startupError.c_str());
        d3d11Device->Release();
        return E_INVALIDARG;
    }
    pipeline->LoadINI();
    logger::info("[Renderer GPU] vendor=0x{:04X} device=0x{:04X} LUID={:08X}:{:08X} FSR-only={}",
        rendererDesc.VendorId, rendererDesc.DeviceId, static_cast<unsigned>(rendererDesc.AdapterLuid.HighPart),
        rendererDesc.AdapterLuid.LowPart, pipeline->mFsrOnlyRenderer);
    if (rendererDesc.VendorId==0x10DE) {
        TheosRenderPipeline::NvidiaAppSettings::SetLog([](const char* message){logger::info("[NVIDIA App Settings] {}",message);});
        TheosRenderPipeline::NvidiaAppSettings::ReportDriverSettings();
    }
    if (pipeline->mFsrOnlyRenderer) {
        auto* generation = SourceFrameGeneration::GetSingleton();
        generation->LoadINI(rendererDesc.VendorId, pipeline->mFsrOnlyRenderer);
        generation->ResolveRuntimePaths(TheosRenderPipeline::PluginPaths::Directory());
        logger::info("[Renderer GPU] FSR-only policy: presenter={}, FG requested={}, NR/HDR disabled; non-RTX NVIDIA uses FSR3; INI unchanged",
            generation->settings.generationBackend, generation->settings.enabled);
    }
    const auto result = nvidiaHost->CreateSwapChain(This, d3d11Device, pDesc, ppSwapChain, ptrFactoryCreateSwapChain);
    if (SUCCEEDED(result))
    {
        d3d11Device->Release();
        logger::info("[NvidiaHost] game-facing swapchain ownership acquired");
        return result;
    }

    // Streamline may already own a native swapchain on this HWND. Do not
    // construct a second owner after a partially successful startup.
    logger::critical("[SourceDLSSG] startup failed; refusing mixed-owner fallback: {}", nvidiaHost->Status());
    d3d11Device->Release();
    return result;
}

HRESULT WINAPI hk_D3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
                                                UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
                                                UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
                                                IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice,
                                                D3D_FEATURE_LEVEL* pFeatureLevel,
                                                ID3D11DeviceContext** ppImmediateContext)
{
    static TheosRenderPipeline::HookSafety::DeviceAdmission gameDevice;
    if (!gameDevice.Begin()) {
        util::report_and_fail("RaZkolbaS already owns a game-device creation request. A second or reentrant request cannot replace the active NVIDIA host. Restart Skyrim and check RaZkolbaS.log.");
    }
    logger::info("Calling original D3D11CreateDeviceAndSwapChain");
    TheosRenderPipeline::CommunityShaders::InstallEngineHooks();

    // DLSS and native UI require the host's stable game-facing buffer, including
    // sessions that start with interpolation off. Install before device creation.
    if (!pSwapChainDesc || !ppSwapChain || !ppDevice || !ppImmediateContext)
    {
        util::report_and_fail("RaZkolbaS requires a game device and swapchain creation request.");
    }
    if (!pSwapChainDesc->Windowed)
    {
        util::report_and_fail("RaZkolbaS requires windowed or borderless mode. Disable exclusive fullscreen and restart Skyrim.");
    }
    auto frameGen = SourceFrameGeneration::GetSingleton();
    frameGen->refreshRate = SourceFrameGeneration::GetRefreshRate(pSwapChainDesc->OutputWindow);
    logger::info("[NvidiaHost] display refresh rate: {:.1f} Hz; host required with interpolation {}",
                 frameGen->refreshRate, frameGen->RuntimeInterpolationRequested() ? "on" : "off");
    if (!ptrFactoryCreateSwapChain)
    {
        Microsoft::WRL::ComPtr<IDXGIFactory> factory;
        if (pAdapter)
        {
            pAdapter->GetParent(IID_PPV_ARGS(&factory));
        }
        if (!factory)
        {
            CreateDXGIFactory1(IID_PPV_ARGS(&factory));
        }
        if (factory)
        {
            // Class-level vtable patch: affects the factory instance DXGI uses
            // internally for this same factory type.
            TheosRenderPipeline::InstallVTableHook(factory.Get(), 10, &hk_IDXGIFactory_CreateSwapChain, ptrFactoryCreateSwapChain);
            if (!ptrFactoryCreateSwapChain)
            {
                util::report_and_fail("RaZkolbaS could not install its required NVIDIA swapchain hook.");
            }
            logger::info("[FrameGen] IDXGIFactory::CreateSwapChain detoured for proxying");
        }
        else
        {
            util::report_and_fail("RaZkolbaS could not obtain the DXGI factory required for NVIDIA presentation.");
        }
    }

    if (ppSwapChain)
    {
        *ppSwapChain = nullptr;
    }
    if (ppDevice)
    {
        *ppDevice = nullptr;
    }
    if (ppImmediateContext)
    {
        *ppImmediateContext = nullptr;
    }
    HRESULT hr = (*ptrD3D11CreateDeviceAndSwapChain)(pAdapter, DriverType, Software, Flags, pFeatureLevels,
                                                     FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice,
                                                     pFeatureLevel, ppImmediateContext);

    auto* nvidiaHost = NvidiaHost::GetSingleton();
    hr = TheosRenderPipeline::CompleteDeviceCreation(
        hr, ppSwapChain, ppDevice, ppImmediateContext,
        [&]
        {
            return TheosRenderPipeline::CompleteRequiredHostStartup(*nvidiaHost);
        });
    if (FAILED(hr))
    {
        // The completion boundary has cleared failed outputs. Show the actual
        // startup error once, instead of leaving the game to fail without context.
        const auto* smoothMotionNotice=TheosRenderPipeline::NvidiaAppSettings::CurrentSmoothMotionNotice();
        util::report_and_fail(std::format("RaZkolbaS could not start rendering.\n\n{}\nHRESULT: 0x{:08X}\n\n{}{}{}"
                                          "See RaZkolbaS.log for details. Skyrim will close after this message.",
                                          nvidiaHost->Status(), (uint32_t)hr,
                                          smoothMotionNotice?"Separately, NVIDIA driver settings need attention:\n":"",
                                          smoothMotionNotice?smoothMotionNotice:"",smoothMotionNotice?"\n\n":""));
    }

    auto device = *ppDevice;
    auto deviceContext = *ppImmediateContext;
    auto swapChain = *ppSwapChain;
    // ENB may return a wrapper distinct from the host's inner immediate
    // context. These are the very interfaces whose vtables we hook below.
    nvidiaHost->RegisterSourceGameContext(deviceContext);
    RenderPipeline::GetSingleton()->SetupSwapChain(swapChain);
    RenderPipeline::GetSingleton()->PreInit();
    OverlayUI::GetSingleton()->Init(swapChain, device, deviceContext);
    logger::info("Detouring virtual function tables");
    // D3D11CreateDeviceAndSwapChain can return its own COM wrapper around our
    // stable outer swapchain, so pointer identity is not reliable here. The
    // active source proxy always reaches our outer Present and owns this work.
    logger::info("[NvidiaHost] outer game-facing swapchain owns the Present lifecycle");
    if (TheosRenderPipeline::CommunityShaders::Active()) {
        TheosRenderPipeline::CommunityShaders::InstallDeviceHooks(deviceContext, swapChain);
    } else {
        InstallUpscalerContextHooks(device, deviceContext);
    }

    return hr;
}

namespace TheosRenderPipeline
{
void InstallUpscalerDeviceHooks(std::uintptr_t moduleBase)
{
    // Continue through the previous import target so an earlier renderer's
    // device setup still runs before control returns to our completion boundary.
    InstallImportHook(moduleBase, "d3d11.dll", "D3D11CreateDeviceAndSwapChain",
        &hk_D3D11CreateDeviceAndSwapChain, ptrD3D11CreateDeviceAndSwapChain);
    if (!ptrD3D11CreateDeviceAndSwapChain)
    {
        util::report_and_fail("RaZkolbaS could not hook D3D11 device creation for its required NVIDIA host.");
    }
}
} // namespace TheosRenderPipeline
