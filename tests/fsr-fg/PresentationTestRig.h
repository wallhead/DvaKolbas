#pragma once
#include "GenerationTestRig.h"
#include "FrameGen/FSRPresentation.h"
#include <filesystem>
namespace PresentationFixture
{
    using namespace GenerationFixture;
    using TheosRenderPipeline::FsrPresentation;
    struct Rig : GenerationFixture::Rig
    {
        HWND window{};DXGI_SWAP_CHAIN_DESC desc{};
        std::shared_ptr<FsrRuntime> runtime=std::make_shared<FsrRuntime>();
        std::shared_ptr<InteropFixture::Interop> bridge=std::make_shared<InteropFixture::Interop>();
        ComPtr<ID3D11Texture2D> ui,depth11,motion11;
        GpuFrameResources guides{};
        FsrEffectProvider swapchainProvider{FsrEffect::FrameGenerationSwapChain,{17752306900579389447ull,"3.1.7"}};
        FsrEffectProvider fgProvider{FsrEffect::FrameGeneration,{17726168133342859270ull,"3.1.6"}};
        void (*mode)(unsigned){};unsigned (*stat)(unsigned){};
        Rig(const char* root)
        {
            Require(bool(runtime->Load(std::filesystem::absolute(root))) && bool(runtime->LoadFrameGeneration(std::filesystem::absolute(root))),"presentation runtime");
            auto dll=GetModuleHandleW(L"amd_fidelityfx_loader_dx12.dll");mode=reinterpret_cast<decltype(mode)>(GetProcAddress(dll,"FixtureMode"));stat=reinterpret_cast<decltype(stat)>(GetProcAddress(dll,"FixturePresentationStat"));
            Require(mode && stat,"presentation fixture instrumentation");mode(0);
            window=CreateWindowExW(0,L"STATIC",L"TRP FG presenter fixture",WS_OVERLAPPEDWINDOW,0,0,160,160,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);Require(window!=nullptr,"hidden window");
            desc.BufferDesc.Width=desc.BufferDesc.Height=128;desc.BufferDesc.Format=limits.format;
            desc.BufferDesc.RefreshRate={120,1};desc.BufferDesc.ScanlineOrdering=DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;desc.BufferDesc.Scaling=DXGI_MODE_SCALING_STRETCHED;
            desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT|DXGI_USAGE_SHADER_INPUT;desc.BufferCount=1;desc.OutputWindow=window;desc.Windowed=TRUE;
            desc.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;Initialize(*bridge);
            auto d=Description();d.Width=d.Height=128;Check(device11->CreateTexture2D(&d,nullptr,&ui),"completed UI");
            d.Width=d.Height=64;d.Format=DXGI_FORMAT_R32_FLOAT;Check(device11->CreateTexture2D(&d,nullptr,&depth11),"D3D11 depth identity");
            d.Format=DXGI_FORMAT_R16G16_FLOAT;Check(device11->CreateTexture2D(&d,nullptr,&motion11),"D3D11 motion identity");
            frame.depth=depth11.Get();frame.motion=motion11.Get();frame.deltaMilliseconds=1000.0f/72;frame.sourceId=1;
            guides.depth=resources.depth;guides.motion=resources.motion;
        }
        ~Rig(){DestroyWindow(window);}
        void Create(FsrPresentation& p){Require(bool(p.Create(factory.Get(),runtime,bridge,desc,swapchainProvider)),"NewDX12 presenter create");}
        HRESULT Source(FsrPresentation& p,bool requested=true,bool menu=false)
        {
            const auto result=p.Present(frame,UpscaleOutcome::Temporal,guides,p.SceneTarget11(),ColorEncoding::SRGB,ui.Get(),nullptr,true,menu,requested,0,0);
            ++frame.sourceId;return result;
        }
    };
}
