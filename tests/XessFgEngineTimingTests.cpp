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
    Require(Qualified({1,6,1170,0},updateBytes,inputBytes,renderBytes),"identified instructions qualify hook sites");
    Require(!Qualified({1,6,640,0},updateBytes,inputBytes,renderBytes),"uninspected runtime cannot borrow 1170 witnesses");
    auto changed=inputBytes;changed[15]=0xE9;
    Require(!Qualified({1,6,1170,0},updateBytes,changed,renderBytes),"changed input call rejected before publishing hooks");
    auto wrongArguments=renderBytes;wrongArguments[1]=0xC9;
    Require(!Qualified({1,6,1170,0},updateBytes,inputBytes,wrongArguments),"changed render arguments rejected");
    auto foreignTarget=updateBytes;foreignTarget[8]^=1;
    Require(!Qualified({1,6,1170,0},foreignTarget,inputBytes,renderBytes),"unexpected update callee rejected");
    unsigned observed{};Observer sink{&observed,[](void* context,Boundary) noexcept { ++*static_cast<unsigned*>(context); }};
    Observe(Boundary::BeforeUpdate);Require(!observed && !Bind(nullptr),"ordinary owners dispatch no Intel markers");
    Require(Bind(&sink) && !Bind(&sink),"only one retained Intel observer");
    Observe(Boundary::BeforeUpdate);Observe(Boundary::InputSampled);Observe(Boundary::BeforeRender);Observe(Boundary::AfterUpdate);
    Require(observed==4 && Unbind(&sink),"verified wrappers dispatch genuine boundaries");
    Observe(Boundary::BeforeUpdate);Require(observed==4,"unbound owner receives no callbacks");
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
    Require(bool(latency.Retire(true,true)),"latency retires after proven owner cleanup");
    std::puts("PASS: source/input/simulation/render/Present mapping; Skyrim instruction/ordering qualification remains separate");
}
