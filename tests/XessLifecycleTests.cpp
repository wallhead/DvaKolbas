#include "Upscaling/XessUpscaler.h"
#include <dxgi1_4.h>
#include <cstdio>
#include <cstdlib>
#include <thread>
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
static void Require(bool ok,const char* message) { if(!ok){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);} }
int wmain(int argc,wchar_t** argv)
{
    Require(argc==2,"fixture root required");
    auto runtime=std::make_shared<XessRuntime>();Require(bool(runtime->Load(std::filesystem::absolute(argv[1]))),"fixture runtime loaded");
    const auto module=GetModuleHandleW(L"libxess.dll");
    const auto mode=reinterpret_cast<void(*)(unsigned)>(GetProcAddress(module,"FixtureMode"));
    const auto created=reinterpret_cast<unsigned(*)()>(GetProcAddress(module,"FixtureCreated"));
    const auto destroyed=reinterpret_cast<unsigned(*)()>(GetProcAddress(module,"FixtureDestroyed"));
    ComPtr<IDXGIFactory4> factory;ComPtr<IDXGIAdapter> warp;ComPtr<ID3D12Device> device;
    Require(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))) && SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp))) && SUCCEEDED(D3D12CreateDevice(warp.Get(),D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&device))),"real software D3D12 fence/device for ownership tests");
    XessInputPolicy policy{};policy.sourceEncoding=ColorEncoding::Gamma22;policy.motionGuide=XessMotionGuide::Undilated;
    {
        XessUpscaler failed;mode(1);
        Require(!failed.Initialize(runtime,device.Get(),Quality::NativeAA,{640,360},policy),"SDK Init failure reported");
        Require(created()==1 && destroyed()==1,"partial initialization unwinds its created context");
        Require(bool(failed.DestroyAfterRetirement()) && destroyed()==1,"partial-init cleanup idempotent");
    }
    mode(0);
    {
        XessUpscaler owner;
        const auto initialized=owner.Initialize(runtime,device.Get(),Quality::NativeAA,{640,360},policy);
        Require(initialized && *initialized==Extent{640,360},"context uses SDK-queried Native size");
        bool deferred{},destroyRefused{};
        std::thread worker([&]{
            deferred=owner.AdmitFrame(1,1)==XessFrameAdmission::OffOwnerThread;
            destroyRefused=!owner.DestroyAfterRetirement();
        });
        worker.join();
        Require(deferred && destroyRefused && destroyed()==1,"off-thread frame defers without weakening destruction affinity");
        Require(owner.AdmitFrame(1,1)==XessFrameAdmission::Ready,"owner thread can resume after deferred worker frame");
        Require(!owner.Initialize(runtime,device.Get(),Quality::NativeAA,{800,450},policy),"extent replacement refused until context retired");
        ComPtr<ID3D12Fence> reader;Require(SUCCEEDED(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&reader))),"pending-reader fence created");
        Require(bool(owner.TrackReader(reader.Get(),1)),"actual fence object retained for pending reader");
        Require(!owner.DestroyAfterRetirement() && destroyed()==1,"pending reader retains context instead of destroying it on timeout");
        Require(GetModuleHandleW(L"libxess.dll")!=nullptr,"pending context retains runtime module");
        Require(SUCCEEDED(reader->Signal(1)),"synthetic pending reader completed");
        Require(bool(owner.DestroyAfterRetirement()) && destroyed()==2,"retirement retry destroys context exactly once");
        Require(bool(owner.DestroyAfterRetirement()) && destroyed()==2,"completed cleanup remains idempotent");
    }
    {
        XessUpscaler owner;Require(bool(owner.Initialize(runtime,device.Get(),Quality::NativeAA,{640,360},policy)),"next context initializes");
        mode(3);Require(!owner.DestroyAfterRetirement() && destroyed()==2,"SDK destroy failure retains ownership");
        auto guides=policy;guides.motionExtent=guides.depthExtent={640,360};guides.motion={640,360,true,false};
        Require(!owner.ConfigureGuides(guides),"poisoned retained context cannot accept guide changes");
        mode(0);Require(bool(owner.DestroyAfterRetirement()) && destroyed()==3,"SDK destroy can retry after failure");
    }
    for(unsigned selected:{4u,5u,6u}) {
        XessUpscaler owner;mode(selected);
        const auto initialized=owner.Initialize(runtime,device.Get(),Quality::NativeAA,{640,360},policy);
        if(selected==5)Require(bool(initialized),"qualified 2.0.2 LDR stripping remains accepted");
        else Require(!initialized && initialized.error().kind==ErrorKind::IncompatibleAbi &&
            initialized.error().message.find(selected==4?"2.0.1":"2.0.3")!=std::string::npos &&
            initialized.error().message.find("flags")!=std::string::npos,
            "unqualified effective flags report actual dispatcher version and ABI mismatch");
        Require(bool(owner.DestroyAfterRetirement()),"version qualification cleanup succeeds");
    }
    runtime.reset();Require(!GetModuleHandleW(L"libxess.dll"),"last context releases module after successful destruction");
    std::puts("PASS: XeSS partial-init, retained readers, destruction retry and replacement ownership");
}
