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
        Require(bool(host.Retire()) && !resources->Runtime(),"AMD asynchronous readers retire before shared SR owner");
        auto unresolved=WithNativeCreator<FsrHostResources>(std::filesystem::absolute(argv[1]),UnresolvedCreator);
        FsrHostPresentation rejected;auto rejectedExtent=rejected.Create(rig.factory.Get(),rig.device11.Get(),unresolved,rig.desc,settings);
        Require(!rejectedExtent && !rejected.SwapChain() && !unresolved->FeatureReady(),"unresolved native ownership rejected before outer publication");
        std::puts("PASS: host sizing, stable D3D11 publication, deferred camera feature, complete UI, live toggle and ordered retirement");return 0;
    }catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}
}
