#include "fsr-fg/PresentationTestRig.h"
using namespace PresentationFixture;
int main(int argc,char** argv)
{
    Require(argc==2,"fixture runtime root");PresentationFixture::Rig rig(argv[1]);FsrPresentation p;
    auto translated=FsrPresentation::TranslateDescriptor(rig.desc);
    Require(translated && translated->BufferCount==2 && translated->SwapEffect==DXGI_SWAP_EFFECT_FLIP_DISCARD &&
        translated->BufferDesc.RefreshRate.Numerator==120 && translated->BufferDesc.RefreshRate.Denominator==1 &&
        translated->BufferDesc.Scaling==rig.desc.BufferDesc.Scaling && translated->BufferDesc.ScanlineOrdering==rig.desc.BufferDesc.ScanlineOrdering &&
        translated->BufferUsage==rig.desc.BufferUsage && translated->OutputWindow==rig.window,"AllLegacyDescFieldsTranslatedOrRejected");
    for(int field=0;field<7;++field){auto bad=rig.desc;switch(field){case 0:bad.Windowed=FALSE;break;case 1:bad.SampleDesc.Count=4;break;case 2:bad.SampleDesc.Quality=1;break;case 3:bad.BufferDesc.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;break;case 4:bad.Flags=DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;break;case 5:bad.BufferUsage=DXGI_USAGE_READ_ONLY;break;case 6:bad.BufferDesc.RefreshRate={120,0};break;}
        auto result=FsrPresentation::TranslateDescriptor(bad);Require(!result && !result.error().message.empty(),"unsupported descriptor has field diagnostic");}
    rig.Create(p);Require(!FsrPresentation::InternalFactoryCreation(),"internal factory guard restored");
    FsrPresentation second;Require(!second.Create(rig.factory.Get(),rig.runtime,rig.bridge,rig.desc,rig.swapchainProvider),"SingleHwndPresenter");
    Check(p.Present({},UpscaleOutcome::SkippedInvalidInput,{},nullptr,ColorEncoding::Unknown,nullptr,nullptr,false,false,false,0,0),"StartupPresentBeforeFgContext");
    Require(rig.stat(1)==0,"startup has no FG context");Require(bool(p.CompleteStartup(rig.limits,rig.fgProvider)),"deferred FG context");
    Check(p.Present({},UpscaleOutcome::SkippedInvalidInput,{},nullptr,ColorEncoding::Unknown,nullptr,nullptr,false,false,false,0,DXGI_PRESENT_TEST),"test Present");
    Require(rig.stat(2)==0 && rig.stat(3)==0 && rig.stat(4)==0,"PresentTestDoesNotDispatch");
    for(int i=0;i<16;++i)Check(rig.Source(p),"source handoff");
    Require(rig.stat(2)==16 && rig.stat(3)==2 && rig.stat(4)==2,"PresentInvokesGenerationCallbackExactlyOnce");
    Require(rig.stat(5)==16 && rig.stat(6)==0,"UiRegistrationIsSeparateFromFrameConfigure and NoInterpolationCommandListQueryOnProductionPath");
    Require(rig.stat(7)==16,"PrepareAndCallbackUseConfiguredFrameId");
    Check(rig.Source(p,false),"disabled generation source");Require(rig.stat(4)==2 && rig.stat(5)==17,"DisabledPresentDoesNotInvokeGenerationCallback and DisabledFgStillComposesUi");
    Require(bool(p.Retire()),"normal shutdown");Require(rig.stat(8)>0 && rig.stat(9)>0 && rig.stat(10)==0,"UiUnregisteredBeforeRelease and ShutdownWaitsForPresents");
    FsrPresentation failed;rig.mode(21);Require(!failed.Create(rig.factory.Get(),rig.runtime,rig.bridge,rig.desc,rig.swapchainProvider),"injected creation failure");
    Require(rig.stat(0)==2 && rig.stat(11)==0,"NoCreationApiFallback");rig.mode(0);Require(bool(failed.Retire()),"failed creation cleanup");
    FsrPresentation startupOnly;rig.Create(startupOnly);Require(bool(startupOnly.CompleteStartup(rig.limits,rig.fgProvider)),"unconfigured FG context created");
    Require(bool(startupOnly.Retire()),"UnconfiguredContextDoesNotConfigureNullSwapchain");
    rig.ValidateDebug();std::puts("PASS: NewDX12 descriptor/admission, deferred startup, per-source callback transaction and separate UI registration (vendor double)");
}
