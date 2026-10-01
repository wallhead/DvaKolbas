#include "FrameGen/OrdinaryPresentation.h"
#include "Upscaling/FSRHostResources.h"
#include "InteropTestRig.h"
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
int main(int argc,char** argv)
{
    Require(argc==2,"fixture root supplied");Rig rig;BackendConfiguration config;
    config.backend=BackendKind::Fsr;config.generationEnabled=false;config.generationBackend=0;config.quality=Quality::Performance;
    FsrHostResources fsr(std::filesystem::absolute(argv[1]));Require(bool(fsr.PrepareSizing(rig.device11.Get(),config,{321,181})),"prepare lifecycle sizing");
    Require(bool(fsr.CompleteStartup()),"create lifecycle feature");auto bridge=fsr.Bridge();bridge->SetRetirementWaitPolicy({1,10});
    ComPtr<ID3D12Fence> gate12;ComPtr<ID3D11Fence> gate11;rig.SharedGate(gate12,gate11);
    Check(rig.context4->Wait(gate11.Get(),1),"delay native reader");auto* resource=fsr.Resources().output;
    Require(!fsr.Retire(),"RetirementBeforeDestroy: gated D3D11 reader prevents retirement");
    Require(fsr.Resources().output==resource && fsr.ContextOwned() && !fsr.FeatureReady(),"FailedResizeKeepsSafeOwnership: retain ownership without claiming readiness");
    Check(gate12->Signal(1),"release independent test reader gate");Check(bridge->Drain(),"retire controlled GPU work");
    // The latched bridge fault keeps the host terminal; destructor must abandon
    // the complete context/runtime/resources instead of releasing them.
    Require(!fsr.Retire() && fsr.Resources().output==resource,"terminal retirement fault cannot relabel ownership safe");
    HWND window=CreateWindowExW(0,L"STATIC",L"FSR retirement fixture",WS_OVERLAPPEDWINDOW,0,0,400,300,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    Require(window!=nullptr,"hidden lifecycle window");
    DXGI_SWAP_CHAIN_DESC desc{};desc.BufferDesc.Width=321;desc.BufferDesc.Height=181;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=1;desc.OutputWindow=window;desc.Windowed=TRUE;
    OrdinaryPresentation ordinary;ComPtr<IDXGISwapChain> chain;
    Check(ordinary.CreateSwapChain(rig.factory.Get(),rig.device11.Get(),desc,chain.GetAddressOf(),&IDXGIFactory::CreateSwapChain),"ordinary lifecycle creation");
    ordinary.SetRetirementWaitPolicy({1,10});
    Check(rig.context4->Wait(gate11.Get(),2),"delay ordinary output reader");
    Require(FAILED(ordinary.BeforeResize()) && !ordinary.Ready(),"ordinary retirement stalls before resize and latches unavailable");
    Check(gate12->Signal(2),"release ordinary reader gate");Check(ordinary.Retire(),"prove ordinary reader retirement after gate release");
    Require(FAILED(ordinary.AfterResize(S_OK)) && !ordinary.Ready(),"successful later fence cannot clear terminal resize fault");
    chain.Reset();DestroyWindow(window);
    rig.ValidateDebug();std::puts("PASS: host context/resources retained on reader stall and failed lifecycle");
}
