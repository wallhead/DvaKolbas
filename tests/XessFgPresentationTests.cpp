#include "FrameGen/XessGenerationPresentation.h"
#include "InteropTestRig.h"
#include "XessFgFrameFixture.h"
#include "fixtures/xess-fg/Control.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <atomic>
#include <thread>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
static XellFixtureCount latencyCount;
static XellFixtureRead latencyRead;
static void Observe(uint32_t stage)
{
    if (stage==1) Require(latencyCount()>=2 && latencyRead(0).kind==1,"XeLL created/enabled before FG");
    if (stage==4) Require(XessGenerationPresentation::InternalFactoryCreation(),"recursive factory creation guard active inside Intel call");
    if (stage==12) Require(latencyRead(latencyCount()-1).kind!=5,"FG destroyed before latency context");
}
static void Success(const Result<void>& result,const char* message)
{ if (!result) std::fprintf(stderr,"ERROR %s native=%lld %s\n",message,result.error().nativeResult,result.error().message.c_str());Require(bool(result),message); }
static ComPtr<ID3D11Texture2D> Texture(Rig& rig,DXGI_FORMAT format,UINT bind)
{ auto desc=rig.Description();desc.Format=format;desc.BindFlags=bind;ComPtr<ID3D11Texture2D> texture;Check(rig.device11->CreateTexture2D(&desc,nullptr,&texture),"frame texture");return texture; }
int wmain(int argc,wchar_t** argv)
{
    if (NrRuntimeResearch::GameRunningOrUnknown()) { std::puts("NOT QUALIFIED: Skyrim running or process guard unavailable");return 2; }
    Require(argc==2,"fixture path");auto loaded=XessGenerationRuntime::Load(argv[1]);Require(bool(loaded),"loader");
    const auto fgModule=GetModuleHandleW(L"libxess_fg.dll"),latencyModule=GetModuleHandleW(L"libxell.dll");
    const auto reset=reinterpret_cast<XellFixtureReset>(GetProcAddress(fgModule,"FixtureFgReset"));
    const auto count=reinterpret_cast<XellFixtureCount>(GetProcAddress(fgModule,"FixtureFgCount"));
    const auto read=reinterpret_cast<XessFgFixtureRead>(GetProcAddress(fgModule,"FixtureFgRead"));
    const auto fail=reinterpret_cast<XessFgFixtureFailAt>(GetProcAddress(fgModule,"FixtureFgFailAt"));
    const auto observe=reinterpret_cast<XessFgFixtureObserve>(GetProcAddress(fgModule,"FixtureFgObserve"));
    const auto resetLatency=reinterpret_cast<XellFixtureReset>(GetProcAddress(latencyModule,"FixtureReset"));
    latencyCount=reinterpret_cast<XellFixtureCount>(GetProcAddress(latencyModule,"FixtureCount"));
    latencyRead=reinterpret_cast<XellFixtureRead>(GetProcAddress(latencyModule,"FixtureRead"));
    Require(reset && count && read && fail && observe && resetLatency && latencyCount && latencyRead,"test controls");
    WNDCLASSW wc{};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"RaZkolbaS XeSS FG ownership test";
    Require(RegisterClassW(&wc)!=0,"hidden test window class");
    const auto window=CreateWindowW(wc.lpszClassName,L"ownership test",WS_OVERLAPPEDWINDOW,0,0,64,48,nullptr,nullptr,wc.hInstance,nullptr);
    Require(window!=nullptr,"hidden test window");
    Rig rig;auto bridge=std::make_shared<Interop>();rig.Initialize(*bridge);
    DXGI_SWAP_CHAIN_DESC desc{};desc.OutputWindow=window;desc.Windowed=TRUE;desc.BufferCount=2;
    desc.BufferDesc.Width=3;desc.BufferDesc.Height=2;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
    reset();resetLatency();observe(Observe);
    XessGenerationPresentation owner;Success(owner.Create(rig.factory.Get(),*loaded,bridge,desc),"Intel presentation owner initializes");
    Require(owner.SwapChain() && owner.Latency() && !XessGenerationPresentation::InternalFactoryCreation(),"published owner and restored guard");
    const uint32_t expected[]{1,2,3,4,5,6,7};
    Require(count()==7,"bounded public creation sequence");
    for (uint32_t i=0;i<7;++i) Require(read(i).kind==expected[i],"public API order");
    Require(read(3).id==1 && read(3).value==XEFG_SWAPCHAIN_UI_MODE_HUDLESS_UITEXTURE &&
        read(5).value==XEFG_SWAPCHAIN_UI_COMPOSITION_STATE_ENABLED && read(6).value==0,"2x cap, explicit HUD composition and initial FG off");
    Check(owner.StartupPresent(0,0),"cleared startup real present");
    Require(read(count()-1).kind==11,"startup queries SDK status without fabricated simulation/present ID");
    auto scene=Texture(rig,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
    auto ui=Texture(rig,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
    auto depth=Texture(rig,DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS);
    auto motion=Texture(rig,DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
    auto frame=XessFgFrame();frame.render=frame.subrect=frame.display=frame.depthExtent=frame.motionExtent={3,2};
    frame.output=scene.Get();frame.depth=depth.Get();frame.motion=motion.Get();frame.outputEncoding=frame.uiEncoding=ColorEncoding::SRGB;frame.reset=true;
    const auto beforeInvalid=count();Require(!owner.Prepare(frame,ui.Get(),nullptr,false,17,true) && count()==beforeInvalid,"incomplete HUD never enables SDK generation");
    auto prepared=owner.Prepare(frame,ui.Get(),nullptr,true,17,true);Require(bool(prepared),"warm accepted inputs");
    Check(owner.Present(*prepared,false,0,0),"reset frame presents real");
    Require(read(count()-2).kind==10 && read(count()-2).id==17 && read(count()-1).kind==11,"accepted SDK ID then actual status query");
    Require(owner.Status().framesPresented==7,"raw fixture SDK status preserved without nominal multiplier");
    frame.sourceId=2;frame.reset=false;prepared=owner.Prepare(frame,ui.Get(),nullptr,true,18,true);Require(bool(prepared),"next accepted source");
    Check(owner.Present(*prepared,true,0,0),"accepted interpolating source");
    Require(owner.Status().isFrameGenEnabled==1,"enabled after validated inputs");
    const auto beforeReal=count(),beforeRealLatency=latencyCount();
    auto real=owner.PrepareReal(frame,ui.Get(),nullptr,true);Require(bool(real),"repeated/loading real scene and HUD publish without a fresh source");
    const auto beforeUntimedGenerate=count();Require(owner.Present(*real,true,0,0)==E_INVALIDARG && count()==beforeUntimedGenerate,"no-source real image cannot be promoted to generated output");
    Check(owner.Present(*real,false,0,0),"untimed real-only presentation");
    Require(!owner.Status().isFrameGenEnabled && latencyCount()==beforeRealLatency,"no-source real presentation disables generation without invented latency markers");
    for(auto i=beforeReal;i<count();++i)Require(read(i).kind!=8 && read(i).kind!=9 && read(i).kind!=10,"no-source real publication tags no resources/constants or SDK Present ID");
    real=owner.PrepareReal(frame,ui.Get(),nullptr,true);Require(bool(real),"same source can be displayed repeatedly without new guides");
    Check(owner.Present(*real,false,0,0),"second untimed real-only presentation");
    auto* proxy=owner.SwapChain();Success(owner.Suspend(),"suspend");Success(owner.Resume(),"resume without rebuild");Require(owner.SwapChain()==proxy,"fixed-size restore keeps owner");
    auto resized=frame;resized.display.width=4;Require(!owner.Prepare(resized,ui.Get(),nullptr,true,19,true) && owner.SwapChain()==proxy,"changed extent rejected before owner retirement");
    fail(12);Require(!owner.Retire() && owner.Latency() && owner.Latency()->Context(),"failed FG destroy keeps latency and runtime owners");
    Success(owner.Retire(),"quiescent FG destroy retry then XeLL cleanup");Require(!owner.SwapChain() && !owner.Latency(),"clean owner released");
    for (bool tagged:{true,false}) {
        reset();resetLatency();observe(Observe);
        auto pacedBridge=std::make_shared<Interop>();rig.Initialize(*pacedBridge);
        XessGenerationPresentation paced;Success(paced.Create(rig.factory.Get(),*loaded,pacedBridge,desc,0,true),"source-ready pacing owner");
        ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"source-ready pacing gate");
        Check(rig.queue->Wait(gate.Get(),1),"hold source publication");
        HANDLE returned=CreateEventW(nullptr,FALSE,FALSE,nullptr);Require(returned!=nullptr,"prepare return event");
        std::atomic<bool> early{false};
        std::thread release([&]{early=WaitForSingleObject(returned,75)==WAIT_OBJECT_0;Check(gate->Signal(1),"release source publication");});
        frame.sourceId=1;frame.reset=true;
        auto result=tagged?paced.Prepare(frame,ui.Get(),nullptr,true,17,true):paced.PrepareReal(frame,ui.Get(),nullptr,true);
        SetEvent(returned);release.join();CloseHandle(returned);
        Require(bool(result),"source-ready preparation succeeds after actual progress");
        Check(paced.Present(*result,false,0,0),"source-ready real/reset present");Success(paced.Retire(),"source-ready owner retirement");
        Require(early!=tagged,"only tagged FG preparation waits for current source publication; real-only publication stays asynchronous");
    }
    {
        reset();resetLatency();observe(Observe);
        auto stalledBridge=std::make_shared<Interop>();rig.Initialize(*stalledBridge);stalledBridge->SetRetirementWaitPolicy({10,40});
        // Timeout intentionally retains the proxy. Its HWND must be separate
        // from subsequent fixtures; DXGI allows only one flip chain per HWND.
        const auto timeoutWindow=CreateWindowW(wc.lpszClassName,L"readiness timeout",WS_OVERLAPPEDWINDOW,0,0,64,48,nullptr,nullptr,wc.hInstance,nullptr);
        Require(timeoutWindow!=nullptr,"timeout fixture window");auto timeoutDesc=desc;timeoutDesc.OutputWindow=timeoutWindow;
        XessGenerationPresentation stalled;Success(stalled.Create(rig.factory.Get(),*loaded,stalledBridge,timeoutDesc,0,true),"readiness timeout owner");
        ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"readiness timeout publication gate");
        Check(rig.queue->Wait(gate.Get(),1),"hold current publication beyond deadline");
        frame.sourceId=1;frame.reset=true;
        const auto result=stalled.Prepare(frame,ui.Get(),nullptr,true,17,true);
        Check(gate->Signal(1),"release timed out source before assertions");Check(stalledBridge->Drain(),"retire actual copies without granting owner reuse");
        Require(!result && result.error().nativeResult==HRESULT_FROM_WIN32(WAIT_TIMEOUT),"publication timeout is reported from preparation");
        const auto calls=count();
        auto rejected=XessGenerationFrame{};rejected.sourceId=1;rejected.sourceEpoch=frame.sourceEpoch;rejected.sdkId=17;
        Require(stalled.Present(rejected,true,0,0)==E_UNEXPECTED && count()==calls,"failed readiness never admits SDK Present/status");
        Require(!stalled.PrepareReal(frame,ui.Get(),nullptr,true) && !stalled.Retire() && stalled.Latency()->Context(),"timeout keeps native, SDK and latency owners and rejects reuse");
        for(auto i=calls;i<count();++i)Require(read(i).kind!=12,"failed source readiness cannot authorize SDK destruction");
        DestroyWindow(timeoutWindow);
    }
    for (const uint32_t stage:{1u,2u,3u,4u,5u,6u,7u}) {
        reset();resetLatency();observe(Observe);fail(stage);
        XessGenerationPresentation partial;Require(!partial.Create(rig.factory.Get(),*loaded,bridge,desc),"public creation failure returned");
        Success(partial.Retire(),"partial initialization unwinds in safe order");
        Require(latencyRead(latencyCount()-1).kind==5,"partial init latency cleanup");
    }
    reset();resetLatency();observe(Observe);
    auto pendingBridge=std::make_shared<Interop>();rig.Initialize(*pendingBridge);pendingBridge->SetRetirementWaitPolicy({10,40});
    XessGenerationPresentation pending;Success(pending.Create(rig.factory.Get(),*loaded,pendingBridge,desc),"pending presentation owner");
    ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"presentation retirement gate");
    Check(rig.queue->Wait(gate.Get(),1),"hold native presentation queue");
    ID3D12GraphicsCommandList* list{};Check(pendingBridge->Begin(Work::SwapChain,&list),"pending native list");
    Check(pendingBridge->Submit(Work::SwapChain),"pending native submission");
    const auto pendingCalls=count();const auto retireAttempt=pending.Retire();
    Check(gate->Signal(1),"release presentation queue before assertion");
    Require(!retireAttempt && pending.Latency() && pending.Latency()->Context(),"unretired work retains FG/XeLL/runtime owners");
    for (auto i=pendingCalls;i<count();++i) Require(read(i).kind!=12,"unretired work cannot authorize FG destruction");
    // A timed-out bridge stops admissions permanently; queue progress alone
    // cannot manufacture permission to reuse it or release its producer state.
    Require(!pending.Resume() && !pending.Retire() && pending.Latency()->Context(),"poisoned bridge keeps producer/latency owners after progress resumes");
    observe(nullptr);DestroyWindow(window);UnregisterClassW(wc.lpszClassName,wc.hInstance);
    rig.ValidateDebug();std::puts("PASS: public Intel proxy ownership and failure traces; real Intel interpolation unqualified");
}
