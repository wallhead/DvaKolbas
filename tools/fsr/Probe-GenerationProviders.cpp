#include "Upscaling/FSRRuntime.h"
#include "Upscaling/FSRFrameGeneration.h"
#include "FrameGen/FSRPresentation.h"
#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <cstdio>
#include <cstring>
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
static int Fail(const RuntimeError& error)
{ std::fprintf(stderr, "FAIL kind=%u native=%lld: %s\n", unsigned(error.kind), static_cast<long long>(error.nativeResult), error.message.c_str()); return 1; }
int main(int argc, char** argv)
{
    if(argc<2){std::fprintf(stderr,"Usage: TRPFsrGenerationProviderProbe <absolute plugin directory> [--provider Analytical|Compatible|MachineLearning] [--create-context|--create-presenter]\n");return 2;}
    ProviderPolicy policy=ProviderPolicy::Analytical;bool createContext{},createPresenter{},checkPolicy{};
    for(int i=2;i<argc;++i){
        if(!std::strcmp(argv[i],"--create-context"))createContext=true;
        else if(!std::strcmp(argv[i],"--create-presenter"))createPresenter=true;
        else if(!std::strcmp(argv[i],"--provider") && i+1<argc){
            const std::string_view value=argv[++i];checkPolicy=true;
            if(value=="Analytical")policy=ProviderPolicy::Analytical;
            else if(value=="Compatible")policy=ProviderPolicy::Compatible;
            else if(value=="MachineLearning")policy=ProviderPolicy::MachineLearning;
            else return 2;
        }else return 2;
    }
    if(createContext && createPresenter)return 2;
    auto runtime = std::make_shared<FsrRuntime>();
    if (auto loaded = runtime->Load(std::filesystem::path(argv[1])); !loaded) return Fail(loaded.error());
    if (auto loaded = runtime->LoadFrameGeneration(std::filesystem::path(argv[1])); !loaded) return Fail(loaded.error());
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
        auto providers = runtime->EnumerateForEffect(device.Get(), effect); if (!providers) return Fail(providers.error());
        for (const auto& provider : *providers)
            std::printf("Effect=%u id=%llu name=%s\n", unsigned(provider.effect), static_cast<unsigned long long>(provider.identity.id), provider.identity.name.c_str());
    }
    if(checkPolicy){
        auto catalog=runtime->EnumerateForEffect(device.Get(),FsrEffect::FrameGeneration);if(!catalog)return Fail(catalog.error());
        auto selected=SelectFsrEffectProvider(*catalog,FsrEffect::FrameGeneration,policy);if(!selected)return Fail(selected.error());
        std::printf("Selected FG provider id=%llu name=%s (algorithm version, independent of SR)\n",
            static_cast<unsigned long long>(selected->identity.id),selected->identity.name.c_str());
    }
    if(createPresenter){
        ComPtr<ID3D12CommandQueue> queue;D3D12_COMMAND_QUEUE_DESC queueDesc{};queueDesc.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
        if(FAILED(device->CreateCommandQueue(&queueDesc,IID_PPV_ARGS(&queue))))return 1;
        auto bridge=std::make_shared<TheosRenderPipeline::Graphics::D3D11D3D12Interop>();
        if(FAILED(bridge->Initialize(producer.Get(),device.Get(),queue.Get())))return 1;
        ComPtr<IDXGIFactory4> factory;if(FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))return 1;
        HWND window=CreateWindowExW(0,L"STATIC",L"TRP real AMD presenter probe",WS_OVERLAPPEDWINDOW,0,0,1280,720,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
        if(!window)return 1;
        int failure{};
        {
            TheosRenderPipeline::FsrPresentation presenter;DXGI_SWAP_CHAIN_DESC desc{};
            desc.OutputWindow=window;desc.Windowed=TRUE;desc.BufferDesc.Width=1280;desc.BufferDesc.Height=720;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=1;
            auto catalog=runtime->EnumerateForEffect(device.Get(),FsrEffect::FrameGenerationSwapChain);if(!catalog)return Fail(catalog.error());
            auto selected=SelectFsrEffectProvider(*catalog,FsrEffect::FrameGenerationSwapChain);if(!selected)return Fail(selected.error());
            if(auto result=presenter.Create(factory.Get(),runtime,bridge,desc,*selected);!result)failure=Fail(result.error());
            if(!failure && FAILED(presenter.Present({},UpscaleOutcome::SkippedInvalidInput,{},nullptr,ColorEncoding::Unknown,nullptr,nullptr,false,false,false,0,0)))failure=1;
            auto fgCatalog=runtime->EnumerateForEffect(device.Get(),FsrEffect::FrameGeneration);if(!fgCatalog)return Fail(fgCatalog.error());
            auto fg=SelectFsrEffectProvider(*fgCatalog,FsrEffect::FrameGeneration,policy);if(!fg)return Fail(fg.error());
            FsrGenerationLimits limits;limits.render={640,360};limits.display={1280,720};
            if(!failure){if(auto result=presenter.CompleteStartup(limits,*fg);!result)failure=Fail(result.error());}
            if(auto result=presenter.Retire();!result)failure=Fail(result.error());
        }
        DestroyWindow(window);if(failure)return failure;
        std::puts("PASS: real AMD NewDX12 create/identity, feature-less startup Present, deferred FG create, unregister/WaitForPresents and ordered destruction; generated pixels not tested");return 0;
    }
    if (createContext) {
        auto session = std::make_shared<FsrSdkSession>(); FsrFrameGeneration generation(session); auto lock = session->Lock();
        auto catalog = runtime->EnumerateForEffect(device.Get(), FsrEffect::FrameGeneration); if (!catalog) return Fail(catalog.error());
        auto selected = SelectFsrEffectProvider(*catalog, FsrEffect::FrameGeneration,policy); if (!selected) return Fail(selected.error());
        FsrGenerationLimits limits; limits.render = {640,360}; limits.display = {1280,720};
        if (auto created = generation.Create(lock, runtime, device.Get(), *selected, limits); !created) return Fail(created.error());
        auto memory = generation.QueryMemoryUsage(lock);
        // Creation has no queue or recorded work. No callback was installed;
        // there are no Prepare/Present readers to retire in this narrow probe.
        auto destroyed = generation.DestroyAfterRetirement(lock);
        if (!memory) return Fail(memory.error()); if (!destroyed) return Fail(destroyed.error());
        std::printf("Actual FG provider id=%llu name=%s ABI=%u.%u.%u swapchainABI=3.1.7 GPU memory bytes=%llu\n",
            static_cast<unsigned long long>(selected->identity.id), selected->identity.name.c_str(),
            FFX_FRAMEGENERATION_VERSION_MAJOR, FFX_FRAMEGENERATION_VERSION_MINOR, FFX_FRAMEGENERATION_VERSION_PATCH,
            static_cast<unsigned long long>(memory->totalUsageInBytes));
        std::puts("PASS: real selected FG context create, identity, memory query and destruction; no Prepare/generation/presentation tested");
    }
    std::puts("PASS: real effect catalogs queried; no generation or presentation tested");
}
