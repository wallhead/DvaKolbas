#include "FrameGen/XessGenerationHost.h"
#include "InteropTestRig.h"
#include "XessFgFrameFixture.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <chrono>
#include <thread>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
int wmain(int argc,wchar_t** argv) {
    if(NrRuntimeResearch::GameRunningOrUnknown())return 77;Require(argc==2,"fixture directory");
    WNDCLASSW wc{};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"Intel lifecycle fixture";
    RegisterClassW(&wc);auto window=CreateWindowW(wc.lpszClassName,L"Intel lifecycle",WS_OVERLAPPEDWINDOW,0,0,64,48,nullptr,nullptr,wc.hInstance,nullptr);Require(window!=nullptr,"test window");
    Rig rig;auto bridge=std::make_shared<Interop>();rig.Initialize(*bridge);bridge->SetRetirementWaitPolicy({10,1000});
    DXGI_SWAP_CHAIN_DESC desc{};desc.OutputWindow=window;desc.Windowed=TRUE;desc.BufferCount=2;desc.BufferDesc.Width=3;desc.BufferDesc.Height=2;desc.SampleDesc.Count=1;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    XessGenerationHost owner(argv[1]);Require(bool(owner.Create(rig.factory.Get(),rig.device11.Get(),desc,bridge)),"host create");
    auto module=GetModuleHandleW(L"libxess_fg.dll");auto frames=reinterpret_cast<void(*)(uint32_t)>(GetProcAddress(module,"FixtureFrames"));Require(frames!=nullptr,"fixture frame control");frames(0);
    auto texture=[&](DXGI_FORMAT format){auto d=rig.Description();d.Format=format;ComPtr<ID3D11Texture2D> result;Check(rig.device11->CreateTexture2D(&d,nullptr,&result),"actual source resource");return result;};
    auto scene=texture(DXGI_FORMAT_R8G8B8A8_UNORM),ui=texture(DXGI_FORMAT_R8G8B8A8_UNORM),depth=texture(DXGI_FORMAT_R32_FLOAT),motion=texture(DXGI_FORMAT_R16G16_FLOAT);
    auto frame=XessFgFrame();frame.render=frame.subrect=frame.display=frame.depthExtent=frame.motionExtent={3,2};frame.camera.projection[0]=2.f/3;
    frame.output=scene.Get();frame.depth=depth.Get();frame.motion=motion.Get();frame.outputEncoding=frame.uiEncoding=ColorEncoding::SRGB;frame.sourceEpoch=1;
    Check(owner.Present(frame,UpscaleOutcome::RepeatedOutput,ui.Get(),nullptr,true,true,false,0,0),"real-only startup");
    Require(owner.OutputCounter().available && owner.OutputCounter().frames==1,"host publishes actual no-source SDK count");
    Require(bool(owner.BindTiming(true)),"fixture verified boundaries");auto* chain=owner.SwapChain();unsigned source{};
    auto present=[&](bool request,uint64_t epoch){frame.sourceId=++source;frame.sourceEpoch=epoch;
        Require(bool(owner.BeforeSourceLoop(source,epoch)) && bool(owner.InputSampled(source)) && bool(owner.BeforeRender(source)),"genuine ordered source");
        Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,request,0,0),"live present");};
    present(true,1);present(true,1);Require(owner.Status().isFrameGenEnabled,"live FG on");
    present(false,1);Require(!owner.Status().isFrameGenEnabled && owner.OutputCounter().available,"FG off keeps measured SR/NR real output");
    present(true,2);present(true,2);Require(owner.Status().isFrameGenEnabled,"changed NR/source epoch warms then resumes");
    ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"actual reader gate");
    Check(rig.queue->Wait(gate.Get(),1),"hold actual native queue");ID3D12GraphicsCommandList* list{};
    Check(bridge->Begin(Work::SwapChain,&list),"pending reader work");Check(bridge->Submit(Work::SwapChain),"pending reader submission");
    std::thread progress([gate]{std::this_thread::sleep_for(std::chrono::milliseconds(100));gate->Signal(1);});
    const auto start=std::chrono::steady_clock::now();auto paused=owner.Suspend();progress.join();
    Require(bool(paused) && owner.Suspended() && !owner.OutputCounter().available,"suspend waits genuine reader and invalidates cached output");
    Require(std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()>=70,"reader wait really occurred");
    Require(bool(owner.Resume()) && owner.SwapChain()==chain,"same-size resume retains owner");present(true,2);present(true,2);
    Require(owner.Status().isFrameGenEnabled && owner.OutputCounter().available,"resume reacquires actual output after history reset");
    Require(bool(owner.Retire()) && !owner.OutputCounter().available,"clean retirement clears output");
    auto blocked=std::make_shared<Interop>();rig.Initialize(*blocked);blocked->SetRetirementWaitPolicy({10,40});
    XessGenerationHost timeout(argv[1]);Require(bool(timeout.Create(rig.factory.Get(),rig.device11.Get(),desc,blocked)),"timeout owner create");
    ComPtr<ID3D12Fence> hold;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&hold)),"terminal reader gate");Check(rig.queue->Wait(hold.Get(),1),"terminal queue hold");
    Check(blocked->Begin(Work::SwapChain,&list),"terminal pending list");Check(blocked->Submit(Work::SwapChain),"terminal pending submit");
    auto failed=timeout.Suspend();Check(hold->Signal(1),"release queue before assertions");
    Require(!failed && timeout.SwapChain() && !timeout.Resume() && !timeout.OutputCounter().available && !timeout.Reason().empty(),"timeout retains owner, stops admissions and reports reason");
    frames(7);DestroyWindow(window);rig.ValidateDebug();std::puts("PASS live request/source epochs, genuine pending reader suspend, retained resume and terminal timeout; fixture SDK only");
}
