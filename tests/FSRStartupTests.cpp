#include "FrameGen/OrdinaryPresentation.h"
#include "FrameGen/PresentationPolicy.h"
#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRColorContract.h"
#include "Upscaling/FSRPreparedResources.h"
#include "FrameGen/GameFacingTargets.h"
#include "FrameGen/SourceHostBoundary.h"
#include "InteropTestRig.h"
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
struct Spy
{
    OrdinaryPresentation& ordinary;IDXGIFactory* factory;ID3D11Device* device;
    DXGI_SWAP_CHAIN_DESC desc;IDXGISwapChain** output;unsigned nvidiaCalls{};
    HRESULT CreateOrdinary(){return ordinary.CreateSwapChain(factory,device,desc,output,&IDXGIFactory::CreateSwapChain);}
    HRESULT CreateNvidia(){++nvidiaCalls;return E_FAIL;}
    bool OrdinaryReady(){return ordinary.Ready();}bool NvidiaReady(){++nvidiaCalls;return false;}
    HRESULT RetireOrdinary(){return ordinary.Retire();}HRESULT RetireNvidia(){++nvidiaCalls;return E_FAIL;}
};
int main(int argc,char** argv)
{
    Require(argc==2,"fixture root supplied");Rig rig;
    HWND window=CreateWindowExW(0,L"STATIC",L"FSR startup fixture",WS_OVERLAPPEDWINDOW,0,0,800,600,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    Require(window!=nullptr,"hidden test window");
    DXGI_SWAP_CHAIN_DESC desc{};desc.BufferDesc.Width=1921;desc.BufferDesc.Height=1081;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=1;desc.OutputWindow=window;desc.Windowed=TRUE;desc.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;
    OrdinaryPresentation ordinary;ComPtr<IDXGISwapChain> chain;Spy spy{ordinary,rig.factory.Get(),rig.device11.Get(),desc,chain.GetAddressOf()};
    BackendDecision backend{BackendKind::Fsr,PresentationKind::Ordinary,true,false,{}};
    Check(CreatePresentation(backend,spy),"ordinary startup");Require(PresentationReady(backend,spy),"ordinary readiness");
    DXGI_SWAP_CHAIN_DESC effective{};Check(chain->GetDesc(&effective),"effective ordinary descriptor");
    Require(effective.BufferCount==2 && effective.SwapEffect==DXGI_SWAP_EFFECT_FLIP_DISCARD,"ordinary presenter supports indexed two-buffer native UI contract");
    FsrHostResources fsr(std::filesystem::absolute(argv[1]));BackendConfiguration config;config.backend=BackendKind::Fsr;config.generationEnabled=false;config.generationBackend=0;config.quality=Quality::Performance;
    for(auto format:{DXGI_FORMAT_R10G10B10A2_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,DXGI_FORMAT_R8G8B8A8_TYPELESS,DXGI_FORMAT_UNKNOWN}) {
        auto rejected=fsr.PrepareSizing(rig.device11.Get(),config,{1921,1081},format,ColorEncoding::Gamma22);
        Require(!rejected && rejected.error().message.find(std::to_string(static_cast<unsigned>(format)))!=std::string::npos,
            "unsupported handoff format rejected with actual DXGI value");
        Require(!fsr.Bridge() && !fsr.ContextOwned() && !fsr.FeatureReady(),"format rejection happens before GPU ownership and readiness");
    }
    Require(!fsr.PrepareSizing(rig.device11.Get(),config,{1921,1081},DXGI_FORMAT_R8G8B8A8_UNORM,ColorEncoding::Unknown) && !fsr.Bridge(),
        "unknown transfer rejected before reduced resource preparation");
    for(auto format:{DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_B8G8R8A8_UNORM,DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT})
        Require(bool(ValidateFsrHandoff(format,ColorEncoding::Linear)),"converter-supported handoff format admitted");
    auto motionDesc=FsrPreparedTextureDesc(FsrResourceRole::Motion,{4,4});
    Require((motionDesc.BindFlags&D3D11_BIND_RENDER_TARGET) && !(motionDesc.BindFlags&D3D11_BIND_UNORDERED_ACCESS),
        "motion retains proven sharing RTV capability without an unused UAV");
    D3D12_FEATURE_DATA_FORMAT_SUPPORT readOnlyMotion{DXGI_FORMAT_R16G16_FLOAT,D3D12_FORMAT_SUPPORT1_SHADER_LOAD,D3D12_FORMAT_SUPPORT2_NONE};
    Require(FsrPreparedFormatSupported(FsrResourceRole::Motion,readOnlyMotion),"motion shader load without typed UAV store is sufficient");
    auto readOnlyOutput=readOnlyMotion;readOnlyOutput.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;
    Require(!FsrPreparedFormatSupported(FsrResourceRole::Output,readOnlyOutput),"native output still requires typed UAV store");
    auto render=fsr.PrepareSizing(rig.device11.Get(),config,{1921,1081},desc.BufferDesc.Format,ColorEncoding::Gamma22);Require(render && *render==Extent{960,540},"RenderExtentBeforePublication: selected provider supplies odd sizing");
    Require(!fsr.FeatureReady(),"no temporal feature during factory interception");
    GameFacingTargets targets;ComPtr<ID3D11Texture2D> native;Check(chain->GetBuffer(0,IID_PPV_ARGS(&native)),"native buffer");D3D11_TEXTURE2D_DESC actual{};native->GetDesc(&actual);
    Check(targets.CreateGameFacingAfterRetirement(rig.device11.Get(),actual,render->width,render->height),"stable reduced target");
    D3D11_TEXTURE2D_DESC published{};targets.GameFacing()->GetDesc(&published);Require(published.Width==960 && published.Height==540,"stable target published before original return");
    Check(chain->Present(0,0),"StartupPresentWithoutFeature");Require(!fsr.FeatureReady(),"startup Present does not create a temporal feature");
    Require(bool(fsr.CompleteStartup()) && fsr.FeatureReady(),"DeferredFeatureCreation: context created after original return");
    auto* fixedColor=fsr.Resources().color;
    D3D11_TEXTURE2D_DESC actualMotion{};fsr.Motion11()->GetDesc(&actualMotion);
    Require(!(actualMotion.BindFlags&D3D11_BIND_UNORDERED_ACCESS),"real shared motion allocation has no UAV");
    Require(bool(fsr.EnsureInputPolicy({true,false,false,true})) && fsr.FeatureReady() && fsr.Resources().color==fixedColor && fsr.Upscaler()->Limits().input.depthInverted,"measured reversed depth recreates context after retirement without changing published allocations");
    Require(bool(fsr.EnsureInputPolicy({true,true,false,true})) && fsr.Upscaler()->Limits().input.depthInfinite && fsr.Resources().color==fixedColor,
        "infinite depth recreates context after retirement without replacing fixed textures");
    Require(spy.nvidiaCalls==0,"FsrDoesNotRequireNvidia: no NVIDIA creation/readiness/NR/Reflex path");
    const auto retainedRuntime=fsr.Runtime();const auto retainedBridge=fsr.Bridge();
    const auto retainedDevice=retainedBridge->Device12();
    for(const Extent extent : {Extent{641,361},Extent{1279,719},Extent{1921,1081}}){
        Require(bool(fsr.ReleaseSizedAfterRetirement()),"actual shared readers retire before resized allocations");
        const auto resized=fsr.ResizeSizingAfterRetirement(extent,DXGI_FORMAT_R8G8B8A8_UNORM);
        Require(resized && fsr.Runtime()==retainedRuntime && fsr.Bridge()==retainedBridge &&
                fsr.Bridge()->Device12()==retainedDevice,"resizing preserves loaded runtime and actual device/bridge identity");
        Require(bool(fsr.CompleteStartup()) && fsr.FeatureReady(),"retained runtime recreates ready context at resized extent");
        D3D11_TEXTURE2D_DESC output{};fsr.Output11()->GetDesc(&output);
        Require(output.Width==extent.width && output.Height==extent.height,"resized shared output has requested extent");
    }
    Require(bool(fsr.Retire()),"retire feature");Check(RetirePresentation(backend,spy),"retire ordinary presenter");
    Require(spy.nvidiaCalls==0 && !fsr.FeatureReady(),"retirement does not call NVIDIA");
    Check(ordinary.BeforeResize(),"retire before ordinary resize");
    Require(FAILED(ordinary.AfterResize(DXGI_ERROR_INVALID_CALL)) && !ordinary.Ready(),"failed ordinary resize latches unavailable without freeing reader ownership");
    config.generationEnabled=true;Require(!fsr.PrepareSizing(rig.device11.Get(),config,{1921,1081},desc.BufferDesc.Format,ColorEncoding::Gamma22),"startup rejects contradictory generation settings");
    chain.Reset();DestroyWindow(window);rig.ValidateDebug();std::puts("PASS: ordinary startup, provider sizing, stable targets and deferred FSR feature with zero NVIDIA operations");
}
