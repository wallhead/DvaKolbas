#include "fsr-fg/PresentationTestRig.h"
#include "FrameGen/FSRHostPresentation.h"
#include "FrameGen/GameFacingTargets.h"
using namespace PresentationFixture;
using TheosRenderPipeline::FsrHostPresentation;
static unsigned nativeCreations{};
static HRESULT NativeCreator(IUnknown* adapter,D3D_FEATURE_LEVEL level,ID3D12Device** output)
{++nativeCreations;return D3D12CreateDevice(adapter,level,__uuidof(ID3D12Device),reinterpret_cast<void**>(output));}
static HRESULT UnresolvedCreator(IUnknown*,D3D_FEATURE_LEVEL,ID3D12Device** output)
{*output=nullptr;return E_NOINTERFACE;}
template<class Resources> auto WithNativeCreator(const std::filesystem::path& root,decltype(&NativeCreator) creator)
{
 if constexpr(std::is_constructible_v<Resources,std::filesystem::path,decltype(creator)>)return std::make_shared<Resources>(root,creator);
 else {Require(false,"FSR pre-query requires native device creator ownership boundary");return std::shared_ptr<Resources>{};}
}
int main(int argc,char** argv)
{
    try {
        Require(argc==2,"fixture runtime root");PresentationFixture::Rig rig(argv[1]);
        auto resources=WithNativeCreator<FsrHostResources>(std::filesystem::absolute(argv[1]),NativeCreator);
        FsrSettings settings;settings.quality=Quality::Performance;settings.sourceColorEncoding=ColorEncoding::SRGB;
        FsrHostPresentation host;auto extent=host.Create(rig.factory.Get(),rig.device11.Get(),resources,rig.desc,settings);
        Require(nativeCreations==1,"pre-query device supplied through native ownership boundary");
        Require(extent && *extent==Extent{64,64},"AMD sizing before outer stable-buffer publication");
        Require(!resources->FeatureReady(),"no SR or FG temporal feature during factory interception");
        DXGI_SWAP_CHAIN_DESC effective{};Check(host.SwapChain()->GetDesc(&effective),"AMD descriptor");Require(effective.BufferCount==2,"TwoVisibleApplicationBuffers");
        TheosRenderPipeline::GameFacingTargets game;D3D11_TEXTURE2D_DESC native{};host.SceneTarget11()->GetDesc(&native);
        Check(game.CreateGameFacingAfterRetirement(rig.device11.Get(),native,extent->width,extent->height),"stable reduced buffer before first GetBuffer");
        D3D11_TEXTURE2D_DESC reduced{};game.GameFacing()->GetDesc(&reduced);Require(reduced.Width==64 && reduced.Height==64,"StableReducedBufferAtFirstGetBuffer");
        Check(host.StartupPresent(0,DXGI_PRESENT_TEST),"test startup present");Require(bool(resources->CompleteStartup()),"deferred SR startup");
        auto frame=rig.frame;frame.sourceId=1;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
        Check(host.Present(frame,UpscaleOutcome::SpatialRecovery,rig.ui.Get(),nullptr,true,true,true,0,0),"menu handoff before FG feature");
        Require(!host.FeatureReady() && host.Status().submitted && !host.Status().decision.generate,"SpatialFramesDisableGeneration and keep UI");
        for(unsigned i=2;i<=18;++i){frame.sourceId=i;Check(host.WaitBeforeProducer(),"guide producer waits before SR copy");
            Check(host.Present(frame,UpscaleOutcome::Temporal,rig.ui.Get(),nullptr,true,false,true,0,0),"temporal source with completed HUD");}
        Require(host.FeatureReady() && host.Status().decision.generate && host.Status().callback.invocations==1,"measured source enables one generation callback");
        frame.sourceId=19;Check(host.Present(frame,UpscaleOutcome::Temporal,rig.ui.Get(),nullptr,true,false,false,0,0),"live FG off on same AMD chain");
        Require(!host.Status().decision.generate && host.Status().callback.invocations==0,"off source does not generate");
        auto* retainedChain=host.SwapChain();const auto retainedBridge=resources->Bridge();const auto retainedRuntime=resources->Runtime();
        auto nextDesc=rig.desc;nextDesc.BufferDesc.Width=144;nextDesc.BufferDesc.Height=96;
        auto resized=host.Resize(nextDesc);
        Require(resized && SUCCEEDED(resized->result) && resized->render==Extent{72,48},"host queries new render size after proxy resize");
        Require(host.SwapChain()==retainedChain && resources->Bridge()==retainedBridge && resources->Runtime()==retainedRuntime && nativeCreations==1,
            "ResizePreservesAmdChainNativeDeviceAndRuntime");
        Require(!host.FeatureReady() && !resources->FeatureReady(),"fixed-size features deferred after resize");
        Require(bool(resources->CompleteStartup()),"resized SR feature startup");
        D3D11_TEXTURE2D_DESC resizedUi{};rig.ui->GetDesc(&resizedUi);resizedUi.Width=144;resizedUi.Height=96;rig.ui.Reset();
        Check(rig.device11->CreateTexture2D(&resizedUi,nullptr,&rig.ui),"resized native UI");
        frame.render=frame.subrect={72,48};frame.display={144,96};
        bool sawReset{};
        for(unsigned id=20;id<=37;++id){frame.sourceId=id;Check(host.WaitBeforeProducer(),"resized guide wait");
            Check(host.Present(frame,UpscaleOutcome::Temporal,rig.ui.Get(),nullptr,true,false,true,0,0),"resized temporal source");
            if(host.Status().decision.generate)sawReset|=host.Status().decision.reset;}
        Require(sawReset && host.Status().decision.generate,"RestoreFromResizeRecreatesWithReset");
        auto* unchangedScene=host.SceneTarget11();auto* unchangedUpscaler=resources->Upscaler();
        Require(bool(host.Suspend()),"window suspension quiesces readers without replacing producer resources");
        Require(host.StartupPresent(0,DXGI_PRESENT_TEST)==DXGI_STATUS_OCCLUDED,"suspended test Present cannot reuse enabled generation");
        Require(bool(host.Resume()) && host.SceneTarget11()==unchangedScene && resources->Upscaler()==unchangedUpscaler,
            "ClientRestoreWithoutResizeKeepsGameBuffersAndResumes");
        frame.sourceId=38;Check(host.Present(frame,UpscaleOutcome::Temporal,rig.ui.Get(),nullptr,true,false,true,0,0),"source reentry without resource recreation");
        Require(host.Status().decision.prepare && host.Status().decision.generate && host.Status().decision.reset && host.Status().callback.invocations==1,
            "suspension rearms reset and resumed eligible source immediately generates once");
        Require(bool(host.BeforeResize()),"temporary suspension retires FG safely");
        Require(host.StartupPresent(0,0)==DXGI_STATUS_OCCLUDED && host.SwapChain()==retainedChain,"SuspendedHostNeverPresentsStaleConfiguredSource");
        resized=host.Resize(nextDesc);Require(resized && SUCCEEDED(resized->result),"suspended host can restore nonzero extent");
        Microsoft::WRL::ComPtr<ID3D12Resource> held;Check(retainedChain->GetBuffer(0,IID_PPV_ARGS(&held)),"force native resize rejection");
        nextDesc.BufferDesc.Width=160;nextDesc.BufferDesc.Height=112;
        resized=host.Resize(nextDesc);held.Reset();
        Require(resized && FAILED(resized->result) && resized->render==Extent{72,48} && host.SwapChain()==retainedChain,
            "FailedResizeRebuildsOriginalExtentWithoutReplacingChain");
        Require(bool(resources->CompleteStartup()),"failed resize SR recovery");
        frame.sourceId=39;Check(host.Present(frame,UpscaleOutcome::Temporal,rig.ui.Get(),nullptr,true,false,true,0,0),"failed resize host remains usable");
        Require(bool(host.Retire()) && !resources->Runtime(),"AMD asynchronous readers retire before shared SR owner");
        auto unresolved=WithNativeCreator<FsrHostResources>(std::filesystem::absolute(argv[1]),UnresolvedCreator);
        FsrHostPresentation rejected;auto rejectedExtent=rejected.Create(rig.factory.Get(),rig.device11.Get(),unresolved,rig.desc,settings);
        Require(!rejectedExtent && !rejected.SwapChain() && !unresolved->FeatureReady(),"unresolved native ownership rejected before outer publication");
        rig.mode(0); // QueryRenderExtent deliberately overwrites the fixture's shared SDK name.
        auto retryResources=WithNativeCreator<FsrHostResources>(std::filesystem::absolute(argv[1]),NativeCreator);
        FsrHostPresentation retryHost;auto retryCreated=retryHost.Create(rig.factory.Get(),rig.device11.Get(),retryResources,rig.desc,settings);
        if(!retryCreated)std::fprintf(stderr,"retry creation: %s native=%lld\n",retryCreated.error().message.c_str(),retryCreated.error().nativeResult);
        Require(bool(retryCreated),"retirement retry host");
        auto* retryChain=retryHost.SwapChain();rig.mode(23);
        Require(!retryHost.BeforeResize() && retryHost.SwapChain()==retryChain && retryResources->Runtime(),"failed resize retirement retains all owners");
        rig.mode(0);Require(bool(retryHost.BeforeResize()),"failed retirement must actually retry quiescence");
        Require(bool(retryHost.Resize(rig.desc)),"successful retirement retry permits resize");Require(bool(retryHost.Retire()),"retry host retirement");
        std::puts("PASS: host sizing, stable D3D11 publication, deferred camera feature, complete UI, live toggle and ordered retirement");return 0;
    }catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}
}
