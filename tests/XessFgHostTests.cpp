#include "FrameGen/XessGenerationHost.h"
#include "InteropTestRig.h"
#include "XessFgFrameFixture.h"
#include "fixtures/xess-fg/Control.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <thread>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
static ComPtr<ID3D11Texture2D> Texture(Rig& rig,DXGI_FORMAT format,UINT bind)
{
    auto desc=rig.Description();desc.Format=format;desc.BindFlags=bind;
    ComPtr<ID3D11Texture2D> texture;Check(rig.device11->CreateTexture2D(&desc,nullptr,&texture),"producer texture");return texture;
}
int wmain(int argc,wchar_t** argv)
{
    if(NrRuntimeResearch::GameRunningOrUnknown()){std::puts("SKIPPED: Skyrim running or unknown");return 77;}
    Require(argc==2,"fixture plugin directory");
    WNDCLASSW wc{};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"Intel host boundary fixture";
    Require(RegisterClassW(&wc)!=0,"window class");
    auto window=CreateWindowW(wc.lpszClassName,L"Intel host",WS_OVERLAPPEDWINDOW,0,0,64,48,nullptr,nullptr,wc.hInstance,nullptr);
    Require(window!=nullptr,"test window");
    Rig rig;auto bridge=std::make_shared<Interop>();rig.Initialize(*bridge);
    DXGI_SWAP_CHAIN_DESC desc{};desc.OutputWindow=window;desc.Windowed=TRUE;desc.BufferCount=2;desc.BufferDesc.Width=3;desc.BufferDesc.Height=2;
    desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
    XessGenerationHost owner(argv[1]);Require(bool(owner.Create(rig.factory.Get(),rig.device11.Get(),desc,bridge)),"Intel host creates on the retained producer/native bridge");
    ComPtr<ID3D11Device> producer;Check(owner.GetProducerDevice(IID_PPV_ARGS(&producer)),"game-facing device query");
    Require(producer.Get()==rig.device11.Get(),"game-facing identity is the retained D3D11 producer");
    ComPtr<ID3D12Device> native;Check(owner.SwapChain()->GetDevice(IID_PPV_ARGS(&native)),"native inner device query");
    Require(native.Get()==rig.device12.Get(),"only inner chain exposes native D3D12 device");
    auto module=GetModuleHandleW(L"libxess_fg.dll");
    auto count=reinterpret_cast<XellFixtureCount>(GetProcAddress(module,"FixtureFgCount"));
    auto read=reinterpret_cast<XessFgFixtureRead>(GetProcAddress(module,"FixtureFgRead"));
    auto latencyModule=GetModuleHandleW(L"libxell.dll");
    auto latencyCount=reinterpret_cast<XellFixtureCount>(GetProcAddress(latencyModule,"FixtureCount"));
    Require(count && read && latencyCount,"runtime observation controls");
    auto scene=Texture(rig,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
    auto ui=Texture(rig,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
    auto depth=Texture(rig,DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS);
    auto motion=Texture(rig,DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
    auto frame=XessFgFrame();frame.render=frame.display=frame.subrect=frame.depthExtent=frame.motionExtent={3,2};
    frame.output=scene.Get();frame.depth=depth.Get();frame.motion=motion.Get();frame.outputEncoding=frame.uiEncoding=ColorEncoding::SRGB;
    unsigned startupFlushes=0;Graphics::InteropPerformanceSink startupSink{};startupSink.owner=&startupFlushes;
    startupSink.flush=+[](void* value){++*static_cast<unsigned*>(value);};bridge->SetPerformanceSink(startupSink);
    Check(owner.StartupPresent(0,0),"real startup publication");
    Require(startupFlushes==0,"startup buffer clear retains RTV ownership without draining every Present");
    bridge->SetPerformanceSink({});
    const auto firstEvents=count(),firstLatency=latencyCount();
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,true,true,0,0),"loading/main-menu real frame presents before timing bind");
    Require(latencyCount()==firstLatency,"first/loading Present creates no source loop");
    for(auto i=firstEvents;i<count();++i)Require(read(i).kind!=8 && read(i).kind!=9 && read(i).kind!=10,"loading real image has no fresh guide tags or SDK ID");
    Require(!owner.BindTiming(false),"unqualified engine hooks cannot begin latency cycles");
    Require(bool(owner.BindTiming(true)),"fixture models inspected profile contract; no gameplay qualification claim");
    HRESULT foreignWait=E_FAIL,foreignPresent=E_FAIL,foreignStartup=E_FAIL;
    const auto beforeForeignLatency=latencyCount();
    std::thread loading([&] {
        foreignWait=owner.WaitBeforeProducer();
        foreignStartup=owner.StartupPresent(0,DXGI_PRESENT_TEST);
        foreignPresent=owner.Present(frame,UpscaleOutcome::SpatialRecovery,ui.Get(),nullptr,true,true,false,0,0);
    });
    loading.join();
    Require(SUCCEEDED(foreignWait) && SUCCEEDED(foreignStartup) && SUCCEEDED(foreignPresent),"loading thread keeps real output without a fatal ownership failure");
    Require(latencyCount()==beforeForeignLatency,"foreign-thread publication never fabricates main-loop XeLL markers");
    frame.sourceId=10;frame.sourceEpoch=3;frame.reset=true;
    auto begin=owner.BeforeSourceLoop(10,3);Require(begin && *begin==1,"genuine source reserves checked SDK ID");
    Require(bool(owner.InputSampled(10)) && bool(owner.BeforeRender(10)),"verified source boundary order");
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"warm timed source presents");
    const auto duplicateLatency=latencyCount(),duplicateSdk=count();
    Check(owner.Present(frame,UpscaleOutcome::RepeatedOutput,ui.Get(),nullptr,true,false,true,0,0),"duplicate source presents real image without invented timing");
    Require(latencyCount()==duplicateLatency,"extra Present creates no latency markers");
    for(auto i=duplicateSdk;i<count();++i)Require(read(i).kind!=8 && read(i).kind!=9 && read(i).kind!=10,"extra Present has no tags or Present ID");
    auto* chain=owner.SwapChain();auto changed=desc;changed.BufferDesc.Width=4;
    Require(!owner.Resize(changed) && owner.SwapChain()==chain,"changed extent rejected before owner release");
    auto changedWindow=desc;changedWindow.OutputWindow=nullptr;
    Require(!owner.Resize(changedWindow),"fixed owner rejects replacement window");
    auto changedSamples=desc;changedSamples.SampleDesc.Count=4;
    Require(!owner.Resize(changedSamples),"fixed owner rejects changed sample contract");
    auto abandoned=owner.BeforeSourceLoop(11,3);Require(abandoned && *abandoned==2 && bool(owner.InputSampled(11)),"interrupted source begins before minimize");
    const auto interruptedLatency=latencyCount();
    Require(bool(owner.Suspend()) && owner.Suspended() && bool(owner.Resume()) && owner.SwapChain()==chain,"same-size minimize/restore retains owner");
    Require(latencyCount()==interruptedLatency,"drained interruption creates no fabricated end markers");
    frame.sourceId=12;frame.reset=false;
    auto resumed=owner.BeforeSourceLoop(12,3);Require(resumed && *resumed==3 && bool(owner.InputSampled(12)) && bool(owner.BeforeRender(12)),"restored genuine source keeps SDK IDs monotonic");
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"restored frame resets generation history");
    frame.sourceId=13;Require(bool(owner.BeforeSourceLoop(13,3)),"source begins before unqualified render ordering check");
    const auto beforeBadRender=count(),beforeBadRenderLatency=latencyCount();
    // The untagged cycle can be discarded locally without GPU retirement or
    // invented simulation/render end markers.
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"missing input/render boundary suppresses interpolation and keeps real output");
    Require(latencyCount()==beforeBadRenderLatency,"unqualified render Present creates no repair markers");
    for(auto i=beforeBadRender;i<count();++i)Require(read(i).kind!=8 && read(i).kind!=9 && read(i).kind!=10,"unqualified render ordering prevents SDK tags before publication");
    auto next=owner.BeforeSourceLoop(14,3);
    Require(next && *next==5 && bool(owner.InputSampled(14)) && bool(owner.BeforeRender(14)),"next genuine loop discards untagged cycle and preserves monotonic IDs");
    frame.sourceId=14;frame.camera.depthInverted=true;
    const auto incompatible=count();
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"incompatible immutable guide flags keep real output without killing owner");
    for(auto i=incompatible;i<count();++i)Require(read(i).kind!=8 && read(i).kind!=9,"incompatible flags never reach SDK tags/constants");
    owner.RequireOrderedSources(2);frame.camera.depthInverted=false;
    for(std::uint64_t source=15;source<=16;++source) {
        frame.sourceId=source;
        Require(bool(owner.BeforeSourceLoop(source,3)) && bool(owner.InputSampled(source)) && bool(owner.BeforeRender(source)),"qualification source follows genuine timing boundaries");
        const auto unqualified=count();
        Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"qualification retains actual real image");
        Require(owner.OrderedSources()==source-14,"only compatible completed timed temporal sources advance qualification");
        for(auto i=unqualified;i<count();++i)Require(read(i).kind!=8 && read(i).kind!=9,"qualification gate suppresses tags and interpolation");
    }
    Require(bool(owner.Resize(desc)) && owner.SwapChain()==chain,"same-size resize preserves native owner");
    Require(owner.Bridge()->Queue()==rig.queue.Get(),"native capture queue is the actual retained Intel queue");
    unsigned flushes=0;
    Graphics::InteropPerformanceSink sink{};sink.owner=&flushes;
    sink.flush=+[](void* value){++*static_cast<unsigned*>(value);};bridge->SetPerformanceSink(sink);
    for(std::uint64_t source=17;source<=20;++source) {
        Require(bool(owner.BeforeSourceLoop(source,3)) && bool(owner.InputSampled(source)) && bool(owner.BeforeRender(source)),"mismatch reproduction has genuine input/render boundaries");
        frame.sourceId=source;frame.sourceEpoch=99;
        Check(owner.Present(frame,UpscaleOutcome::SpatialRecovery,ui.Get(),nullptr,true,true,false,0,0),"epoch mismatch retains real image");
        const auto priorFlushes=flushes;
        Require(bool(owner.BeforeSourceLoop(source+1,3)),"next loop recovers an untagged cycle");
        Require(flushes==priorFlushes,"untagged mismatch recovery must not flush/drain the GPU each loop");
        // The new cycle has no render boundary or tagged resources yet.
        frame.sourceId=source+1;
        Check(owner.Present(frame,UpscaleOutcome::SpatialRecovery,ui.Get(),nullptr,true,true,false,0,0),"unrendered cycle remains real-only");
        ++source;
    }
    bridge->SetPerformanceSink({});
    const auto drainCount=owner.DrainSuspends();
    Require(bool(owner.BeforeSourceLoop(21,3)) && bool(owner.InputSampled(21)) && bool(owner.BeforeRender(21)),"main loop ready before foreign loading interruption");
    bool loadingSuspend=false,loadingResume=false;
    std::thread pause([&]{loadingSuspend=bool(owner.Suspend());loadingResume=bool(owner.Resume());});pause.join();
    Require(loadingSuspend && loadingResume && owner.DrainSuspends()==drainCount+1,"foreign-thread minimize drains once without calling main-loop timing");
    frame.sourceId=21;frame.sourceEpoch=3;frame.reset=false;
    const auto beforeInterruptedPresent=latencyCount(),beforeInterruptedTags=count();
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"interrupted owner cycle keeps real output after foreign resume");
    Require(latencyCount()==beforeInterruptedPresent,"pending reset forbids continuation of interrupted markers");
    for(auto i=beforeInterruptedTags;i<count();++i)Require(read(i).kind!=8 && read(i).kind!=9 && read(i).kind!=10,"pending reset forbids tags and IDs for the interrupted cycle");
    Require(bool(owner.BeforeSourceLoop(22,3)) && bool(owner.InputSampled(22)) && bool(owner.BeforeRender(22)),"main thread safely resets timing after foreign drained interruption");
    owner.RequireOrderedSources(0);
    frame.sourceId=22;frame.sourceEpoch=3;frame.reset=false;
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"world rendering survives loading-thread suspend/resume");
    Require(owner.DrainSuspends()==drainCount+1 && owner.SkippedCycles()>=3,"mismatch recovery does not add drain suspends");
    Require(bool(owner.BeforeSourceLoop(23,3)) && bool(owner.InputSampled(23)) && bool(owner.BeforeRender(23)),"accepted source before disabling a repeated image");
    frame.sourceId=23;
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"accepted history is ready before disabling");
    Check(owner.Present(frame,UpscaleOutcome::RepeatedOutput,ui.Get(),nullptr,true,false,false,0,0),"disable during repeated image retains real output");
    Require(bool(owner.BeforeSourceLoop(24,3)) && bool(owner.InputSampled(24)) && bool(owner.BeforeRender(24)),"new source after re-enable");
    frame.sourceId=24;const auto afterDisable=count();
    Check(owner.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0),"re-enable warms accepted history");
    bool resetAfterDisable=false;
    for(auto i=afterDisable;i<count();++i)if(read(i).kind==9)resetAfterDisable=read(i).value==1;
    Require(resetAfterDisable,"disabled duplicate invalidates history before re-enabled temporal tags");
    Require(bool(owner.Retire()),"host retires generation then XeLL after drain");
    Rig foreign;XessGenerationHost wrong(argv[1]);
    Require(!wrong.Create(rig.factory.Get(),foreign.device11.Get(),desc,bridge),"foreign D3D11 identity rejected even on same adapter");
    Require(bool(wrong.Retire()),"rejected creation cleanup");
    rig.ValidateDebug();std::puts("PASS: producer identity, no-source/duplicate real output, checked engine timing and fixed owner lifecycle");
    DestroyWindow(window);
}
