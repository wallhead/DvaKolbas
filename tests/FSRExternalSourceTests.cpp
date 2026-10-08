#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRGenerationGuideAdapter.h"
#include "InteropTestRig.h"
#include "fsr-fg/GenerationTestRig.h"
#include "FrameGen/FSRHostPresentation.h"
#include <filesystem>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;

static void GenerationOnlyRuntime(const std::filesystem::path& root)
{
    FsrRuntime runtime;
    Require(bool(runtime.LoadGenerationOnly(root)), "generation-only runtime loads without SR ownership");
    Require(!GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll"), "ExternalSourceCreatesNoSrContext: SR module was not loaded");
    Require(runtime.Profile()==FsrRuntimeProfile::Official, "InactiveMlSrDoesNotSelectInt8: FG uses official runtime");
    Require(!runtime.Enumerate(reinterpret_cast<ID3D12Device*>(1)), "generation-only runtime rejects SR queries");
    Require(!runtime.EnumerateForEffect(reinterpret_cast<ID3D12Device*>(1),FsrEffect::Upscale), "generation-only runtime also rejects effect-tagged SR queries");
    Require(!runtime.LoadGenerationOnly(root), "generation-only owner cannot replace a live loader");
}
static void Guides(const std::filesystem::path& root)
{
    Rig rig;
    FsrHostResources resources(root);
    FsrInputPolicy policy{true,false,true,true};
    Require(bool(resources.PrepareExternalSizing(rig.device11.Get(),{8,4},{16,8},DXGI_FORMAT_R8G8B8A8_UNORM,ColorEncoding::Gamma22,policy)), "external sizing uses actual producer adapter");
    Require(resources.ExternalSource() && !resources.Upscaler() && !resources.ContextOwned(), "external mode has no SR owner");
    Require(bool(resources.CompleteExternalStartup()) && resources.GenerationInputsReady(), "shared generation guides allocate without an SR context");
    Require(resources.GenerationInputPolicy()==policy && resources.RenderExtent()==Extent{8,4}, "external guide convention and extent retained");
    ComPtr<ID3D11Texture2D> depth,motion;
    D3D11_TEXTURE2D_DESC desc{};resources.Depth11()->GetDesc(&desc);
    Check(rig.device11->CreateTexture2D(&desc,nullptr,&depth),"producer depth");
    resources.Motion11()->GetDesc(&desc);Check(rig.device11->CreateTexture2D(&desc,nullptr,&motion),"producer motion");
    float depths[32];for(auto& v:depths)v=0.375f;
    uint16_t motions[64];for(unsigned i=0;i<64;++i)motions[i]=i%2?0xbc00:0x3800; // -1, +0.5 half-float
    rig.context11->UpdateSubresource(depth.Get(),0,nullptr,depths,8*sizeof(float),0);
    rig.context11->UpdateSubresource(motion.Get(),0,nullptr,motions,8*4,0);
    FsrGenerationGuideAdapter adapter(resources.Bridge(),resources.Resources(),resources.Depth11(),resources.Motion11());
    UpscaleFrame frame{};frame.sourceId=1;frame.sourceEpoch=3;frame.render=frame.subrect={8,4};frame.display={16,8};
    frame.depth=depth.Get();frame.motion=motion.Get();frame.jitterX=0.25f;frame.jitterY=-0.375f;
    frame.motionConvention={8,4,true,true};frame.camera.depthInverted=true;
    Require(bool(adapter.Prepare(frame)), "valid current external guides copied");
    frame.sourceId=2;Require(bool(adapter.Prepare(frame)),"next guide producer does not require a fictitious SR dispatch");
    Check(resources.Bridge()->Drain(),"external producer/consumer retirement");
    auto read=[&](ID3D11Texture2D* texture,const void* expected,size_t pixelBytes){
        D3D11_TEXTURE2D_DESC staging{};texture->GetDesc(&staging);staging.BindFlags=0;staging.MiscFlags=0;staging.Usage=D3D11_USAGE_STAGING;staging.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
        ComPtr<ID3D11Texture2D> copy;Check(rig.device11->CreateTexture2D(&staging,nullptr,&copy),"guide readback resource");
        rig.context11->CopyResource(copy.Get(),texture);D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(copy.Get(),0,D3D11_MAP_READ,0,&mapped),"guide readback");
        for(unsigned y=0;y<4;++y)Require(!std::memcmp(static_cast<char*>(mapped.pData)+mapped.RowPitch*y,static_cast<const char*>(expected)+8*pixelBytes*y,8*pixelBytes),"actual shared guide pixels retained");
        rig.context11->Unmap(copy.Get(),0);
    };
    read(resources.Depth11(),depths,4);read(resources.Motion11(),motions,4);
    Require(frame.jitterX==0.25f && frame.jitterY==-0.375f && frame.motionConvention.scaleX==8, "ExternalGuidesPreserveJitterAndMotionConvention: caller snapshot immutable");
    Require(!adapter.Prepare(frame), "GuidesRequireCurrentMatchingSource: duplicate identity rejected");
    frame.sourceId=3;frame.depth=nullptr;Require(!adapter.Prepare(frame),"missing depth rejects current source");
    frame.depth=depth.Get();frame.render.width=9;Require(!adapter.Prepare(frame),"mismatched render extent rejected");frame.render.width=8;
    ComPtr<ID3D11Device> foreign;ComPtr<ID3D11DeviceContext> foreignContext;
    Check(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&foreign,nullptr,&foreignContext),"foreign guide device");
    depth->GetDesc(&desc);ComPtr<ID3D11Texture2D> foreignDepth;Check(foreign->CreateTexture2D(&desc,nullptr,&foreignDepth),"foreign guide texture");
    frame.depth=foreignDepth.Get();Require(!adapter.Prepare(frame),"foreign producer guide rejected");
    frame.depth=depth.Get();Require(bool(adapter.Prepare(frame)),"invalid input does not consume a valid source identity");
    Require(bool(resources.ReleaseSizedAfterRetirement()) && !resources.GenerationInputsReady(),"external guides released only after retirement");
    Require(bool(resources.ResizeExternalSizingAfterRetirement({4,2},{8,4},DXGI_FORMAT_R8G8B8A8_UNORM)) && bool(resources.CompleteExternalStartup()),"external resize retains official runtime and creates no SR context");
    Require(!resources.Upscaler() && !GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll"),"no hidden second upscaler after resize");
    Require(bool(resources.Retire()),"external owner fully retired");rig.ValidateDebug();
}
static void ExternalPresentation(const std::filesystem::path& root,bool failPolicyRetirement=false)
{
    GenerationFixture::Rig rig;
    HWND window=CreateWindowExW(0,L"STATIC",L"External FG fixture",WS_OVERLAPPEDWINDOW,0,0,160,160,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    Require(window!=nullptr,"external presenter hidden HWND");
    DXGI_SWAP_CHAIN_DESC desc{};desc.BufferDesc.Width=desc.BufferDesc.Height=128;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=1;desc.OutputWindow=window;desc.Windowed=TRUE;desc.SwapEffect=DXGI_SWAP_EFFECT_DISCARD;
    auto windowSized=desc;windowSized.BufferDesc.Width=windowSized.BufferDesc.Height=0;
    auto resolved=FsrHostPresentation::ResolveExternalRenderExtent(windowSized,[](UINT width,UINT height,int* renderWidth,int* renderHeight){
        *renderWidth=int(width/2);*renderHeight=int(height/2);return width>0 && height>0;
    });
    Require(resolved && resolved->width && resolved->height,"ExternalQueryResolvesClientSizeBeforeNgxQuery");
    auto resources=std::make_shared<FsrHostResources>(root);FsrHostPresentation host;FsrSettings settings;
    settings.providerPolicy=ProviderPolicy::MachineLearning;settings.sourceColorEncoding=ColorEncoding::Gamma22;
    auto created=host.CreateExternal(rig.factory.Get(),rig.device11.Get(),resources,desc,settings,{64,64},{});
    Require(created && *created==Extent{64,64},"ExternalPresenterRequiresNoSrFeature: explicit DLSS sizing used");
    Require(!resources->Upscaler() && !GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll"),"inactive FSR4 SR preferences do not load an SR module");
    auto module=GetModuleHandleW(L"amd_fidelityfx_loader_dx12.dll");
    auto mode=reinterpret_cast<void(*)(unsigned)>(GetProcAddress(module,"FixtureMode"));Require(mode!=nullptr,"external callback fixture mode");
    ComPtr<ID3D11Texture2D> depth,motion,ui;
    auto tex=rig.Description();tex.Width=tex.Height=64;tex.Format=DXGI_FORMAT_R32_FLOAT;
    Check(rig.device11->CreateTexture2D(&tex,nullptr,&depth),"external presenter depth");tex.Format=DXGI_FORMAT_R16G16_FLOAT;
    Check(rig.device11->CreateTexture2D(&tex,nullptr,&motion),"external presenter motion");tex.Width=tex.Height=128;tex.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    Check(rig.device11->CreateTexture2D(&tex,nullptr,&ui),"external presenter HUD");
    auto frame=rig.frame;frame.backend=BackendKind::Dlaa;frame.depth=depth.Get();frame.motion=motion.Get();frame.sourceId=0;
    auto present=[&](UpscaleOutcome outcome=UpscaleOutcome::Temporal,bool requested=true,bool menu=false){
        ++frame.sourceId;Check(host.WaitBeforeProducer(),"external presenter producer wait");
        Check(host.Present(frame,outcome,ui.Get(),nullptr,true,menu,requested,0,0),"external presenter source");
    };
    for(unsigned i=0;i<18;++i)present();
    Require(host.FeatureReady() && host.Status().decision.generate && host.Status().callback.invocations==1,"external NGX-source classification actually generates a callback");
    present(UpscaleOutcome::SkippedInvalidInput);Require(!host.Status().decision.generate,"failed DLSS suppresses generation");
    present();Require(host.Status().decision.generate && host.Status().decision.reset,"valid source resets after failed DLSS");
    auto* validDepth=frame.depth;frame.depth=nullptr;present();Require(!host.Status().decision.generate,"missing guides suppress instead of reusing prior guides");frame.depth=validDepth;
    present();Require(host.Status().decision.generate && host.Status().decision.reset,"guide recovery resets temporal history");
    present(UpscaleOutcome::Temporal,false);Require(!host.Status().decision.generate,"live interpolation off");
    present();Require(host.Status().decision.generate && host.Status().decision.reset,"live interpolation on resets");
    present(UpscaleOutcome::Temporal,true,true);Require(!host.Status().decision.generate,"menu suppresses external generation");
    Require(bool(host.Suspend()),"external owner suspension");Require(bool(host.Resume()),"external owner restoration");present();
    Require(host.Status().decision.generate && host.Status().decision.reset,"ResumeResetsGeneration");
    auto* policyChain=host.SwapChain();auto* policyDepth=resources->Depth11();auto* policyScene=host.SceneTarget11();
    auto policy=resources->GenerationInputPolicy();policy.depthInverted=!policy.depthInverted;
    Require(bool(resources->EnsureInputPolicy(policy)),"measured external policy update");frame.camera.depthInverted=policy.depthInverted;
    present();
    Require(host.Status().decision.generate && host.Status().decision.reset,"changed convention recreates FG and resets only FG history");
    Require(host.SwapChain()==policyChain && resources->Depth11()==policyDepth && host.SceneTarget11()==policyScene && !resources->Upscaler(),
        "input policy change preserves chain, guides and scene without creating SR");
    policy.depthInverted=!policy.depthInverted;Require(bool(resources->EnsureInputPolicy(policy)),"restored external policy");
    frame.camera.depthInverted=policy.depthInverted;present();
    Require(host.Status().decision.generate && host.Status().decision.reset,"restored convention resumes generation");
    if(failPolicyRetirement) {
        policy.depthInfinite=!policy.depthInfinite;
        Require(bool(resources->EnsureInputPolicy(policy)),"policy retirement failure setup");frame.camera.depthInfinite=policy.depthInfinite;
        mode(23);++frame.sourceId;Check(host.WaitBeforeProducer(),"policy failure producer wait");
        Require(FAILED(host.Present(frame,UpscaleOutcome::Temporal,ui.Get(),nullptr,true,false,true,0,0)),
            "failed SDK reader retirement rejects policy reentry");
        Require(host.SwapChain()==policyChain && resources->Depth11()==policyDepth && host.SceneTarget11()==policyScene,
            "failed policy retirement retains all reader-owned buffers");
        Require(FAILED(host.WaitBeforeProducer()),"failed policy reentry stops new source admissions");
        mode(0);Require(bool(host.Retire()),"policy retirement failure can retry final cleanup");
        DestroyWindow(window);rig.ValidateDebug();return;
    }
    auto* chain=host.SwapChain();auto* retainedDepth=resources->Depth11();mode(23);
    Require(!host.BeforeResize() && host.SwapChain()==chain && resources->Depth11()==retainedDepth,"ExternalResizeRetainsAllReaders: failed retirement preserves resource owner");mode(0);
    Require(bool(host.BeforeResize()),"external retirement retry");
    auto next=desc;next.BufferDesc.Width=144;next.BufferDesc.Height=96;
    auto resized=host.ResizeExternal(next,{72,48});Require(resized && SUCCEEDED(resized->result) && resized->render==Extent{72,48},"external resize uses measured DLSS render extent");
    Require(host.SwapChain()==chain && !resources->Upscaler(),"resize preserves presenter and does not introduce SR");
    Require(bool(host.Retire()),"external SDK and guide owners retire in order");DestroyWindow(window);rig.ValidateDebug();
}
int main(int argc,char** argv)
{
    Require(argc>=2,"runtime root required");const auto root=std::filesystem::absolute(argv[1]);
    if(argc==3 && std::string_view(argv[2])=="--gpu")Guides(root);
    else if(argc==3 && std::string_view(argv[2])=="--host"){ExternalPresentation(root);ExternalPresentation(root,true);}
    else GenerationOnlyRuntime(root);
    std::puts("PASS: generation-only official runtime and external guide ownership");
}
