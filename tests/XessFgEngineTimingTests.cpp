#include "FrameGen/XessGenerationEngineTiming.h"
#include "FrameGen/XessGenerationEngineHooks.h"
#include "fixtures/xess-fg/Control.h"
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <cstdio>
#include <cstdlib>
#include <thread>
using namespace TheosRenderPipeline;
using Microsoft::WRL::ComPtr;
static void Require(bool value,const char* message)
{ if (!value) { std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1); } }
static void Complete(XessGenerationEngineTiming& timing,uint64_t source,uint32_t id)
{
    Require(bool(timing.InputSampled(source)),"input observed after sleep");
    Require(bool(timing.EndSimulation(source)),"real simulation end");
    Require(bool(timing.BeginRender(source)),"real render start");
    Require(bool(timing.EndRender(source)),"real render end");
    Require(bool(timing.BeforePresent(id)),"same-ID present start");
    Require(bool(timing.AfterPresent(id)),"same-ID present end");
}
int wmain(int argc,wchar_t** argv)
{
    using namespace XessEngineHooks;
    Require(Qualified({1,6,1170,0},inputBytes,renderBytes,gameplayInputBytes),"identified instructions qualify hook sites");
    Require(!Qualified({1,6,640,0},inputBytes,renderBytes,gameplayInputBytes),"uninspected runtime cannot borrow 1170 witnesses");
    auto changed=inputBytes;changed[15]=0xE9;
    Require(!Qualified({1,6,1170,0},changed,renderBytes,gameplayInputBytes),"changed input call rejected before publishing hooks");
    auto wrongArguments=renderBytes;wrongArguments[1]=0xC9;
    Require(!Qualified({1,6,1170,0},inputBytes,wrongArguments,gameplayInputBytes),"changed render arguments rejected");
    auto foreignTarget=inputBytes;foreignTarget[16]^=1;
    Require(!Qualified({1,6,1170,0},foreignTarget,renderBytes,gameplayInputBytes),"unexpected input callee rejected");
    unsigned observed{};Observer sink{&observed,[](void* context,Boundary) noexcept { ++*static_cast<unsigned*>(context); }};
    Observe(Boundary::BeforeInput);Require(!observed && !Bind(nullptr),"ordinary owners dispatch no Intel markers");
    Require(Bind(&sink) && !Bind(&sink),"only one retained Intel observer");
    Observe(Boundary::BeforeInput);Observe(Boundary::InputSampled);Observe(Boundary::BeforeRender);
    Require(observed==3 && Unbind(&sink),"verified wrappers dispatch genuine boundaries");
    Observe(Boundary::BeforeInput);Require(observed==3,"unbound owner receives no callbacks");
    // A detoured outer update caller can be skipped while the actual main input
    // and render calls still run. The input adapter must start timing itself.
    struct PollOrder { unsigned step{}, begin{}, poll{}, sampled{}; } order;
    Observer inputSink{&order,[](void* context,Boundary boundary) noexcept {
        auto& state=*static_cast<PollOrder*>(context);
        if(boundary==Boundary::BeforeInput)state.begin=++state.step;
        if(boundary==Boundary::InputSampled)state.sampled=++state.step;
    }};
    Require(Bind(&inputSink),"main input adapter observer bound");
    PollMainInput([&] { order.poll=++order.step; });
    Require(Unbind(&inputSink),"main input adapter observer unbound");
    Require(order.begin==1 && order.poll==2 && order.sampled==3,
        "source begins before preserved input poll without an outer update callback");
    Require(argc==2,"fixture path");auto loaded=XessGenerationRuntime::Load(argv[1]);Require(bool(loaded),"fixture loader");
    auto module=GetModuleHandleW(L"libxell.dll");
    auto count=reinterpret_cast<XellFixtureCount>(GetProcAddress(module,"FixtureCount"));
    auto read=reinterpret_cast<XellFixtureRead>(GetProcAddress(module,"FixtureRead"));
    Require(count && read,"fixture controls");
    ComPtr<IDXGIFactory4> factory;ComPtr<IDXGIAdapter> warp;ComPtr<ID3D12Device> device;
    Require(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))) && SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp))) &&
        SUCCEEDED(D3D12CreateDevice(warp.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device))),"WARP COM lifetime only");
    XellSession latency;Require(bool(latency.Create(device.Get(),*loaded)),"latency fixture context");
    const auto before=count();XessGenerationEngineTiming unqualified;
    Require(!unqualified.Bind(&latency,false) && !unqualified.BeginSourceLoop(1,7) && count()==before,"unverified engine boundaries inactive before any SDK sleep");
    XessGenerationEngineTiming timing;Require(bool(timing.Bind(&latency,true)),"qualified contract bind");
    Require(!timing.InputSampled(1),"input cannot precede genuine source begin");
    auto first=timing.BeginSourceLoop(1,7);Require(first && *first==1,"one logical source reserves one SDK ID");
    Require(read(count()-2).kind==3 && read(count()-2).id==1 && read(count()-1).kind==4 && read(count()-1).marker==0,"sleep before first simulation marker");
    Require(!timing.EndSimulation(1) && !timing.BeginRender(1),"simulation/render ordering requires observed input after sleep");
    const auto current=timing.CurrentId(1,7);
    Require(!timing.CurrentId(2,7) && current && *current==*first,"host source ID must map exactly");
    Complete(timing,1,*first);const auto completed=count();
    Require(!timing.AfterPresent(1) && !timing.BeforePresent(1) && !timing.BeginSourceLoop(1,7) && count()==completed,"extra/loading Presents and duplicate sources create no simulation or sleep");
    auto second=timing.BeginSourceLoop(2,7);Require(second && *second==2,"next genuine source gets next SDK ID");
    Require(!timing.BeforePresent(3),"mismatched present ID rejected");Complete(timing,2,*second);
    Require(!timing.BeginSourceLoop(10,8) && !timing.ResetAfterDrain(false),"epoch transition/wrap requires proven GPU-drained reset");
    Require(bool(timing.ResetAfterDrain(true)),"drained session epoch reset");
    auto next=timing.BeginSourceLoop(10,8);Require(next && *next==1,"new checked SDK epoch starts consistently");
    bool foreignThreadAccepted=true;std::thread foreign([&] { foreignThreadAccepted=bool(timing.InputSampled(10)); });foreign.join();
    Require(!foreignThreadAccepted,"unqualified engine-thread boundary rejected");Complete(timing,10,*next);
    auto skipped=timing.BeginSourceLoop(11,8);Require(skipped && *skipped==2,"skipped loop reserves an SDK ID once");
    const auto beforeAbandon=count();
    Require(!timing.AbandonUnsubmitted(false),"submitted work cannot use CPU-only abandonment");
    Require(bool(timing.AbandonUnsubmitted(true)) && count()==beforeAbandon,"untagged interruption never fabricates missing end markers");
    auto recovered=timing.BeginSourceLoop(12,8);
    Require(recovered && *recovered==3,"untagged recovery retains monotonic SDK IDs");Complete(timing,12,*recovered);
    Require(bool(timing.ResetAfterDrain(true)),"input-adapter regression starts a drained SDK epoch");
    struct InputLoop { XessGenerationEngineTiming* timing; std::uint32_t id{}; bool sampled{},rendering{}; } loop{&timing};
    Observer loopSink{&loop,[](void* context,Boundary boundary) noexcept {
        auto& state=*static_cast<InputLoop*>(context);
        if(boundary==Boundary::BeforeInput) {
            auto begin=state.timing->BeginSourceLoop(13,8);
            if(begin)state.id=*begin;
        } else if(boundary==Boundary::InputSampled)state.sampled=bool(state.timing->InputSampled(13));
        else if(boundary==Boundary::BeforeRender)state.rendering=
            bool(state.timing->EndSimulation(13)) && bool(state.timing->BeginRender(13));
    }};
    Require(Bind(&loopSink),"input adapter drives real timing owner");
    PollMainInput([&] {
        Require(loop.id==1 && !loop.sampled,"source ID reserved before actual main input poll");
        Require(read(count()-2).kind==3 && read(count()-2).id==1 &&
            read(count()-1).kind==4 && read(count()-1).marker==0,
            "main input observes pre-input XeLL sleep and simulation start");
        Require(!timing.CurrentRenderId(13,8),"no render admission during preserved input poll");
    });
    Observe(Boundary::BeforeRender);
    Require(loop.sampled && loop.rendering && timing.CurrentRenderId(13,8)==1,
        "input and render callbacks alone qualify the same source for FG tagging");
    Require(bool(timing.EndRender(13)) && bool(timing.BeforePresent(1)) && bool(timing.AfterPresent(1)),
        "input-started source completes through real presentation timing");
    Require(Unbind(&loopSink),"input loop observer unbound");
    XessGenerationInputHandoff handoff;
    const auto nextSource=timing.BeginSourceLoop(14,8);
    Require(nextSource && *nextSource==2 && handoff.Arm(14,8,100),
        "post-Present owner reserves next ID and publishes only after sleep returns");
    const auto beforeWorker=count();
    std::thread inputWorker([&] {
        auto ticket=handoff.Begin(101,17);Require(bool(ticket),"native worker retains next source ticket");
        Require(handoff.Complete(ticket,102,17),"native input completion recorded after its actual return");
    });inputWorker.join();
    Require(count()==beforeWorker,"worker handoff never issues foreign-thread XeLL calls");
    Require(handoff.Consume(14,8) && bool(timing.InputSampled(14)) && bool(timing.EndSimulation(14)) && bool(timing.BeginRender(14)),
        "owner translates only exact completed native input into simulation/render markers");
    Require(handoff.Seal(14,8) && bool(timing.EndRender(14)) && bool(timing.BeforePresent(*nextSource)) && bool(timing.AfterPresent(*nextSource)),
        "sealed completed source retains one SDK ID through real publication");
    Require(bool(timing.ResetAfterDrain(true)),"paced regression starts a drained SDK epoch");
    XessGenerationEngineTiming paced;
    Require(bool(paced.Bind(&latency,true,XessGenerationEngineTiming::Mode::PresentationPacing)),"explicit presentation pacing bind");
    auto pacedId=paced.BeginSourceLoop(30,9);
    Require(pacedId && *pacedId==1,"paced source reserves one SDK ID");
    Require(!paced.InputSampled(30),"presentation pacing cannot claim verified input timing");
    Require(bool(paced.EndSimulation(30)) && bool(paced.BeginRender(30)),
        "presentation pacing admits render without a worker input proof");
    Require(!paced.CurrentRenderId(31,9) && !paced.CurrentRenderId(30,10),"pacing still rejects mismatched source or epoch");
    Require(paced.CurrentRenderId(30,9)==1,"exact paced render maps to its SDK ID");
    Require(bool(paced.EndRender(30)) && bool(paced.BeforePresent(1)) && bool(paced.AfterPresent(1)),"paced source completes ordered SDK markers");
    auto gap=paced.BeginSourceLoop(31,9);
    Require(gap && *gap==2 && bool(paced.AbandonUnsubmitted(true)),"interrupted pacing emits no invented end markers");
    auto recovery=paced.BeginSourceLoop(32,9);
    Require(recovery && *recovery==3 && bool(paced.EndSimulation(32)) && bool(paced.BeginRender(32)),"paced recovery keeps exact monotonic IDs without consecutive input warmup");
    bool foreignPacing=true;
    std::thread pacedForeign([&]{foreignPacing=bool(paced.EndRender(32));});pacedForeign.join();
    Require(!foreignPacing,"pacing keeps owner thread enforcement");
    Require(bool(paced.EndRender(32)) && bool(paced.BeforePresent(3)) && bool(paced.AfterPresent(3)),"recovered pacing completes normally");
    Require(bool(latency.Retire(true,true)),"latency retires after proven owner cleanup");
    std::puts("PASS: source/input/simulation/render/Present mapping; Skyrim instruction/ordering qualification remains separate");
}
