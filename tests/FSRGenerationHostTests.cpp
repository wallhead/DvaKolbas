#include "FrameGen/PresentationPolicy.h"
#include "FrameGen/PresentationDevice.h"
#include "FrameGen/PresentationTargets.h"
#include "FrameGen/NativeUIComposition.h"
#include "Upscaling/FSRHostResources.h"
#include "InteropTestRig.h"
#include "FrameGen/NativeUICompletion.h"
#include "FrameGen/FSRSwapChainPolicy.h"
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
struct Routes
{
    unsigned ordinary{},nvidia{},fsr{};
    HRESULT CreateOrdinary(){++ordinary;return S_OK;}HRESULT CreateNvidia(){++nvidia;return S_OK;}HRESULT CreateFsr(){++fsr;return S_OK;}
    bool OrdinaryReady(){++ordinary;return true;}bool NvidiaReady(){++nvidia;return true;}bool FsrReady(){++fsr;return true;}
    HRESULT RetireOrdinary(){++ordinary;return S_OK;}HRESULT RetireNvidia(){++nvidia;return S_OK;}HRESULT RetireFsr(){++fsr;return S_OK;}
};
template<class UI> bool CaptureCompleted(UI& ui,ID3D11DeviceContext* context)
{return ui.CaptureDedicated(context);}
template<class Targets> HRESULT CacheNativeScene(Targets& targets,ID3D11Texture2D* scene)
{return targets.CacheSceneAfterRetirement(scene);}
template<class Resources> std::shared_ptr<FsrRuntime> RetainedRuntime(Resources& resources)
{return resources.Runtime();}
void CompletedUiAtPresent(Rig& rig)
{
    auto desc=rig.Description();Microsoft::WRL::ComPtr<ID3D11Texture2D> scene;
    Check(rig.device11->CreateTexture2D(&desc,nullptr,&scene),"native scene allocation");
    const uint32_t scenePixels[6]{0xff40261a,0xff40261a,0xff40261a,0xff40261a,0xff40261a,0xff40261a};
    rig.context11->UpdateSubresource(scene.Get(),0,nullptr,scenePixels,12,0);
    NativeUIComposition ui;Require(ui.Initialize(rig.device11.Get(),rig.context11.Get(),scene.Get(),desc,true) && ui.Dedicated(),"dedicated native UI allocation");
    const uint32_t hud[6]{0x80008000,0xff0000ff,0,0,0,0};
    rig.context11->UpdateSubresource(ui.RenderTexture(),0,nullptr,hud,12,0);
    Require(CaptureCompleted(ui,rig.context11.Get()),"CompletedUiAtPresent: tagged HUD captured without scene composition");
    auto read=[&](ID3D11Texture2D* source){auto stagingDesc=desc;stagingDesc.BindFlags=0;stagingDesc.Usage=D3D11_USAGE_STAGING;stagingDesc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> staging;Check(rig.device11->CreateTexture2D(&stagingDesc,nullptr,&staging),"HUD readback allocation");
        rig.context11->CopyResource(staging.Get(),source);D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"HUD readback");
        const auto pixel=*static_cast<const uint32_t*>(mapped.pData);rig.context11->Unmap(staging.Get(),0);return pixel;};
    Require(read(ui.TaggedTexture())==hud[0] && read(scene.Get())==scenePixels[0],"capture retains premultiplied coverage and HUD-less scene");
    Require(!CaptureCompleted(ui,nullptr),"missing UI completion context rejected");
    Microsoft::WRL::ComPtr<ID3D11Device> foreign;Microsoft::WRL::ComPtr<ID3D11DeviceContext> foreignContext;
    Check(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&foreign,nullptr,&foreignContext),"foreign UI producer fixture");
    Require(!CaptureCompleted(ui,foreignContext.Get()),"foreign producer cannot claim completed HUD capture");
    Require(UseDedicatedPresentUi(true,false) && UseDedicatedPresentUi(true,true) && !UseDedicatedPresentUi(false,false) && UseDedicatedPresentUi(false,true),"AMD foreground/ReShade must stay outside the scene even without game UI draws");
    Require(PrepareFsrPresentUi(rig.context11.Get(),ui.RenderRTV(),true),"completed game UI retained");
    Require(CaptureCompleted(ui,rig.context11.Get()) && read(ui.TaggedTexture())==hud[0],"preparing overlay cannot erase game HUD");
    Require(PrepareFsrPresentUi(rig.context11.Get(),ui.RenderRTV(),false),"empty game HUD explicitly cleared before overlay draws");
    Require(CaptureCompleted(ui,rig.context11.Get()) && read(ui.TaggedTexture())==0,"no stale HUD on UI-less source");
    const uint32_t overlay[6]{0x80008000,0,0,0,0,0};rig.context11->UpdateSubresource(ui.RenderTexture(),0,nullptr,overlay,12,0);
    Require(CaptureCompleted(ui,rig.context11.Get()) && read(ui.TaggedTexture())==overlay[0] && read(scene.Get())==scenePixels[0],"late ImGui/ReShade foreground survives final freeze outside scene");
    Require(!PrepareFsrPresentUi(foreignContext.Get(),ui.RenderRTV(),false),"foreign context cannot clear completed HUD");
    PresentationTargets targets;Check(CacheNativeScene(targets,scene.Get()),"AMD caches producer scene without D3D11 GetBuffer on D3D12 inner chain");
    Require(targets.BufferIndex(1)==0 && targets.Buffers().size()==1 && targets.Buffers()[0].Get()==scene.Get(),"stable producer scene across both AMD buffer indices");
    Check(targets.Select(rig.device11.Get(),0),"native scene RTV and depth attachment");Require(targets.Texture()==scene.Get(),"native scene identity retained");
}
void PresentAndResizeSourceBoundaries()
{
    unsigned effects{},submits{},completions{};unsigned events{};
    auto prepare=[&](){Require(events++%3==0,"one effects boundary before each submit");++effects;return S_OK;};
    auto submit=[&](){Require(events++%3==1,"one submit after effects");++submits;return S_OK;};
    auto completed=[&](HRESULT result){Require(SUCCEEDED(result) && events++%3==2,"one completion after submit");++completions;};
    Check(PresentFsrSourceBoundary(0,nullptr,prepare,submit,completed),"Present shared source boundary");
    DXGI_PRESENT_PARAMETERS empty{};
    Check(PresentFsrSourceBoundary(0,&empty,prepare,submit,completed),"Present1 shared source boundary");
    Require(effects==2 && submits==2 && completions==2,"ExactlyOneUpscaleAndEffectsPass per Present entry");
    RECT dirty{0,0,1,1};POINT offset{};
    for(unsigned kind=0;kind<3;++kind){DXGI_PRESENT_PARAMETERS invalid{};
        if(kind==0){invalid.DirtyRectsCount=1;invalid.pDirtyRects=&dirty;}
        if(kind==1)invalid.pScrollRect=&dirty;if(kind==2)invalid.pScrollOffset=&offset;
        Require(PresentFsrSourceBoundary(0,&invalid,prepare,submit,completed)==E_INVALIDARG,"dirty/scroll rejected before source consumption");}
    Require(effects==2 && submits==2 && completions==2,"rejection consumes no source or completion");
    Check(PresentFsrSourceBoundary(DXGI_PRESENT_TEST,&empty,prepare,[&]{++submits;return S_OK;},completed),"test Present only probes presenter");
    Require(effects==2 && submits==3 && completions==2,"test Present has no effects/source/completion");
    Require(PresentFsrSourceBoundary(0,nullptr,[]{return E_FAIL;},submit,completed)==E_FAIL,"failed source is never submitted");
    unsigned retired{},detached{},created{};
    auto retire=[&]{++retired;return S_OK;};auto detach=[&]{Require(retired==detached+1,"retire before detaching all inner refs");++detached;};
    auto recreate=[&]{Require(detached==created+1,"detach before new HWND owner");++created;return S_OK;};
    IUnknown* queues[2]{reinterpret_cast<IUnknown*>(1),reinterpret_cast<IUnknown*>(2)};
    Require(ResizeFsrSourceBoundary(2,nullptr,queues,retire,detach,recreate)==E_INVALIDARG,"ResizeBuffers1DoesNotForwardD3D11Queues");
    UINT bad[2]{0,1};Require(ResizeFsrSourceBoundary(2,bad,nullptr,retire,detach,recreate)==E_INVALIDARG,"nonzero masks rejected before retirement");
    UINT zero[3]{};Require(ResizeFsrSourceBoundary(0,zero,nullptr,retire,detach,recreate)==E_INVALIDARG,"unknown mask array length rejected before mutation");
    Require(retired==0 && detached==0 && created==0,"invalid resize cannot mutate host");
    for(UINT count:{0u,1u,2u,3u})Check(ResizeFsrSourceBoundary(count,count?zero:nullptr,nullptr,retire,detach,recreate),"single native queue owner replacement");
    Require(created==4 && detached==4 && retired==4,"all accepted buffer counts use replacement protocol");
    Require(ResizeFsrSourceBoundary(2,nullptr,nullptr,[]{return DXGI_ERROR_WAS_STILL_DRAWING;},detach,recreate)==DXGI_ERROR_WAS_STILL_DRAWING,"failed retirement keeps original owner");
    Require(created==4 && detached==4,"failed retirement cannot release HWND owner refs");
}
int main(int argc,char** argv)
{
    try {
        BackendDecision backend{BackendKind::Fsr,PresentationKind::Fsr,true,false,{}};Routes routes;
        Check(CreatePresentation(backend,routes),"AMD host creation");Require(routes.fsr==1 && routes.nvidia==0 && routes.ordinary==0,"AMD selection cannot enter NVIDIA or ordinary creation");
        Require(PresentationReady(backend,routes),"AMD readiness");Check(RetirePresentation(backend,routes),"AMD retirement");
        Require(routes.fsr==3 && routes.nvidia==0 && routes.ordinary==0,"AMD readiness/retirement isolated from NVIDIA");
        unsigned amd{};PresentationCreation creation{[]{return E_FAIL;},[]{return E_FAIL;},[&]{++amd;return S_OK;}};
        Check(CreatePresentation(backend,creation),"production creation adapter has AMD route");Require(amd==1,"one AMD creation boundary");
        PresentationCreation unavailable{[]{return S_OK;},[]{return S_OK;}};
        Require(CreatePresentation(backend,unavailable)==E_NOTIMPL,"unwired AMD creation cannot fall through to another owner");
        backend.valid=false;Require(CreatePresentation(backend,routes)==E_INVALIDARG && routes.fsr==3,"invalid selector does not mutate presenter ownership");
        PresentAndResizeSourceBoundaries();
        Rig rig;Microsoft::WRL::ComPtr<ID3D11Device> producer;
        HWND window=CreateWindowExW(0,L"STATIC",L"AMD host producer fixture",WS_OVERLAPPEDWINDOW,0,0,128,96,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
        Require(window!=nullptr,"producer fixture HWND");struct Window{HWND value;~Window(){DestroyWindow(value);}}windowOwner{window};
        DXGI_SWAP_CHAIN_DESC1 desc{};desc.Width=128;desc.Height=96;desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=2;desc.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> chain;Check(rig.factory->CreateSwapChainForHwnd(rig.queue.Get(),window,&desc,nullptr,nullptr,&chain),"D3D12 inner chain");
        Check(AcquirePresentationDevice(chain.Get(),rig.device11.Get(),true,producer),"AMD creator device retention");
        Require(producer.Get()==rig.device11.Get(),"OriginalProducerReturnedToGame");
        CompletedUiAtPresent(rig);
        Require(argc==2,"fixture runtime root supplied");FsrHostResources resources(std::filesystem::absolute(argv[1]));BackendConfiguration config;
        config.backend=BackendKind::Fsr;config.generationEnabled=false;config.generationBackend=0;
        Require(bool(resources.PrepareSizing(rig.device11.Get(),config,{128,96},DXGI_FORMAT_R8G8B8A8_UNORM,ColorEncoding::SRGB)),"pre-query sizing before FG owner creation");
        auto runtime=RetainedRuntime(resources);Require(runtime && runtime->Functions().CreateContext!=nullptr,"FG owner receives the retained SR runtime without duplicate loading");
        Require(!resources.FeatureReady(),"SR feature remains deferred during factory interception");Require(bool(resources.Retire()),"pre-query retirement");
        Require(!resources.Runtime() && runtime->Functions().CreateContext!=nullptr,"retained runtime survives SR owner's retirement");
        std::puts("PASS: AMD route creation/readiness/retirement and retained producer");return 0;
    }catch(const std::exception& error){std::fprintf(stderr,"FAIL: %s\n",error.what());return 1;}
}
