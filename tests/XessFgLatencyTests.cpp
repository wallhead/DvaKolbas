#include "FrameGen/XellSession.h"
#include "fixtures/xess-fg/Control.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdio>
#include <cstdlib>
#include <limits>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
static void Require(bool value,const char* reason) { if (!value) { std::fprintf(stderr,"FAIL: %s\n",reason);std::exit(1); } }
static void Finish(XellSession& session,uint32_t id)
{ for (int marker=1;marker<=5;++marker) Require(bool(session.Marker(id,static_cast<xell_latency_marker_type_t>(marker))),"ordered marker accepted"); }
int wmain(int argc,wchar_t** argv)
{
    Require(argc==2,"fixture directory supplied");auto loaded=XessGenerationRuntime::Load(argv[1]);Require(bool(loaded),"fixture loader");
    const auto module=GetModuleHandleW(L"libxell.dll");
    const auto reset=reinterpret_cast<XellFixtureReset>(GetProcAddress(module,"FixtureReset"));
    const auto count=reinterpret_cast<XellFixtureCount>(GetProcAddress(module,"FixtureCount"));
    const auto read=reinterpret_cast<XellFixtureRead>(GetProcAddress(module,"FixtureRead"));
    const auto fail=reinterpret_cast<XellFixtureFailNext>(GetProcAddress(module,"FixtureFailNext"));
    Require(reset && count && read && fail,"test-only control exports");
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    Microsoft::WRL::ComPtr<IDXGIAdapter> warp;
    Require(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))) && SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp))) &&
        SUCCEEDED(D3D12CreateDevice(warp.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device))),"software device ownership for unit lifetime test");
    reset();XellSession session;
    Require(!session.BeginFrame(0),"uncreated session rejects frame");
    Require(bool(session.Create(device.Get(),*loaded)) && session.Context(),"created context retained");
    Require(count()==2 && read(0).kind==1 && read(1).kind==2 && read(1).enabled==1 && read(1).minimumIntervalUs==0,"creation enables latency without FPS cap");
    Require(bool(session.BeginFrame(40)),"first frame arbitrary ID");
    Require(count()==4 && read(2).kind==3 && read(3).kind==4 && read(2).id==40 && read(3).id==40 && read(3).marker==0,"one sleep precedes simulation start with same ID");
    Require(!session.BeginFrame(40) && !session.BeginFrame(41),"duplicate or overlapping simulation cannot sleep");
    Require(!session.Marker(41,XELL_SIMULATION_END) && !session.Marker(40,XELL_PRESENT_START),"mismatched and out-of-order markers rejected before SDK");
    Require(count()==4,"invalid sequence never calls SDK");Finish(session,40);
    Require(!session.Marker(40,XELL_PRESENT_END) && !session.BeginFrame(40),"extra Present cannot invent another simulation");
    Require(!session.SetEnabled(false,false),"pending work blocks sleep-mode change");
    Require(bool(session.SetEnabled(false,true)),"quiescent disable accepted");
    Require(!session.Retire(false,true) && !session.Retire(true,false),"FG destruction and GPU quiescence both required");
    Require(bool(session.BeginFrame(41)),"off retains valid frame instrumentation");Finish(session,41);
    Require(!session.BeginFrame(43),"skipped SDK identity rejected");
    Require(bool(session.ResetAfterDrain(true)),"drained epoch reset");
    Require(bool(session.BeginFrame(std::numeric_limits<uint32_t>::max())),"maximum ID valid before wrap");Finish(session,std::numeric_limits<uint32_t>::max());
    Require(!session.BeginFrame(0) && !session.ResetAfterDrain(false),"wrap cannot reuse an undrained epoch");
    Require(bool(session.ResetAfterDrain(true)) && bool(session.BeginFrame(0)),"drained wrap accepted");
    fail(XELL_RESULT_ERROR_INVALID_ARGUMENT);
    const auto recoverable=session.Marker(0,XELL_SIMULATION_END);
    Require(!recoverable && recoverable.error().kind==ErrorKind::DispatchFailure,"recoverable marker failure distinguished");
    Require(bool(session.Marker(0,XELL_SIMULATION_END)),"failed marker did not advance phase");
    for (int marker=2;marker<=5;++marker) Require(bool(session.Marker(0,static_cast<xell_latency_marker_type_t>(marker))),"resume valid phase");
    fail(XELL_RESULT_ERROR_DEVICE);
    const auto lost=session.BeginFrame(1);
    Require(!lost && lost.error().kind==ErrorKind::DeviceLost && !session.BeginFrame(2),"device loss stops admissions");
    Require(bool(session.Retire(true,true)) && !session.Context(),"explicit proven retirement destroys context");
    Require(read(count()-1).kind==5,"destroy is final SDK call");
    XellSession failingDestroy;Require(bool(failingDestroy.Create(device.Get(),*loaded)),"new context");fail(XELL_RESULT_ERROR_UNKNOWN);
    Require(!failingDestroy.Retire(true,true) && failingDestroy.Context(),"failed destroy retains context and modules");
    Require(bool(failingDestroy.Retire(true,true)),"proven quiescent destroy retry");
    std::puts("PASS: XeLL sleep/marker order, checked IDs, quiescence and retained failure ownership; engine timing unqualified");
}
