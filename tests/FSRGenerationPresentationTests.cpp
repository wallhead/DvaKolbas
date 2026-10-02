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
    rig.frame.sourceId=16;
    Require(FAILED(rig.Source(p)),"RejectedSourceCannotReusePreviousEnabledGenerationConfig");
    Require(rig.stat(2)==16 && rig.stat(4)==2 && rig.stat(5)==16,"rejected source never configures, dispatches or republishes UI");
    Check(rig.Source(p,false),"disabled generation source");Require(rig.stat(4)==2 && rig.stat(5)==17,"DisabledPresentDoesNotInvokeGenerationCallback and DisabledFgStillComposesUi");
    Require(bool(p.Retire()),"normal shutdown");Require(rig.stat(8)>0 && rig.stat(9)>0 && rig.stat(10)==0,"UiUnregisteredBeforeRelease and ShutdownWaitsForPresents");
    FsrPresentation failed;rig.mode(21);Require(!failed.Create(rig.factory.Get(),rig.runtime,rig.bridge,rig.desc,rig.swapchainProvider),"injected creation failure");
    Require(rig.stat(0)==2 && rig.stat(11)==0,"NoCreationApiFallback");rig.mode(0);Require(bool(failed.Retire()),"failed creation cleanup");
    FsrPresentation startupOnly;rig.Create(startupOnly);Require(bool(startupOnly.CompleteStartup(rig.limits,rig.fgProvider)),"unconfigured FG context created");
    Require(bool(startupOnly.Retire()),"UnconfiguredContextDoesNotConfigureNullSwapchain");
    FsrPresentation delayedFeature;rig.Create(delayedFeature);rig.frame.sourceId=1;
    const auto uiBefore=rig.stat(5),contextsBefore=rig.stat(1);
    Check(rig.Source(delayedFeature,false,true),"completed menu source before camera conventions are known");
    Require(delayedFeature.Status().sourceId==1 && delayedFeature.Status().submitted && rig.stat(5)==uiBefore+1 && rig.stat(1)==contextsBefore,
        "CompletedMenuStillPublishesSceneAndUiBeforeGenerationFeature");
    Require(bool(delayedFeature.CompleteStartup(rig.limits,rig.fgProvider)),"generation feature can start after completed menu sources");
    Check(rig.Source(delayedFeature),"first temporal source after menu-only handoff");Require(bool(delayedFeature.Retire()),"deferred menu presenter retirement");
    FsrPresentation resized;rig.desc.Flags=DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;rig.Create(resized);
    auto* sameChain=resized.SwapChain();Check(sameChain->SetMaximumFrameLatency(1),"configured latency");
    Check(sameChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709),"configured SDR");
    const auto handle=sameChain->GetFrameLatencyWaitableObject();Require(handle!=nullptr,"cached latency handle");
    Require(bool(resized.BeforeResize()),"quiesce extent resources");
    Require(resized.SwapChain()==sameChain,"ResizeKeepsAmdSwapchainIdentity");
    Check(sameChain->ResizeBuffers(2,144,96,DXGI_FORMAT_UNKNOWN,rig.desc.Flags),"same inner proxy resize");
    Require(bool(resized.AfterResize(S_OK)),"rebuild extent transport on retained chain");
    UINT latency{};Check(sameChain->GetMaximumFrameLatency(&latency),"latency after resize");
    Require(latency==1,"MaximumFrameLatencySurvivesResize");
    DWORD handleFlags{};Require(GetHandleInformation(handle,&handleFlags)!=FALSE,"WaitableHandleTracksSameSwapchainAfterResize");
    D3D11_TEXTURE2D_DESC scene{};resized.SceneTarget11()->GetDesc(&scene);Require(scene.Width==144 && scene.Height==96,"new extent producer rebuilt");
    Check(resized.Present({},UpscaleOutcome::SkippedInvalidInput,{},nullptr,ColorEncoding::Unknown,nullptr,nullptr,false,false,false,0,0),"resized startup Present");
    Require(bool(resized.BeforeResize()),"failed resize quiesce");
    Microsoft::WRL::ComPtr<ID3D12Resource> held;Check(sameChain->GetBuffer(0,IID_PPV_ARGS(&held)),"hold a buffer to force genuine DXGI resize rejection");
    const auto failedResize=sameChain->ResizeBuffers(2,160,112,DXGI_FORMAT_UNKNOWN,rig.desc.Flags);
    Require(FAILED(failedResize),"native resize fails with outstanding buffer reference");held.Reset();
    Require(bool(resized.AfterResize(failedResize)) && resized.SwapChain()==sameChain,"FailedResizeKeepsOriginalSwapchainAlive");
    resized.SceneTarget11()->GetDesc(&scene);Require(scene.Width==144 && scene.Height==96,"failed resize restores actual previous extent");
    Check(resized.Present({},UpscaleOutcome::SkippedInvalidInput,{},nullptr,ColorEncoding::Unknown,nullptr,nullptr,false,false,false,0,0),"failed resize remains presentable");
    Require(bool(resized.Retire()),"resized presenter retirement");CloseHandle(handle);
    Check(SetWindowPos(rig.window,nullptr,0,0,0,0,SWP_NOMOVE|SWP_NOZORDER)?S_OK:E_FAIL,"zero client fixture");
    auto zeroDesc=rig.desc;zeroDesc.BufferDesc.Width=zeroDesc.BufferDesc.Height=0;
    auto suspended=FsrPresentation::TranslateResizeDescriptor(zeroDesc);
    Require(suspended && !*suspended,"MinimizedZeroExtentIsRecoverable");
    zeroDesc.BufferDesc.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;
    Require(!FsrPresentation::TranslateResizeDescriptor(zeroDesc),"zero extent cannot bypass SDR descriptor admission");
    Check(SetWindowPos(rig.window,nullptr,0,0,160,160,SWP_NOMOVE|SWP_NOZORDER)?S_OK:E_FAIL,"restored client fixture");
    zeroDesc.BufferDesc.Format=rig.desc.BufferDesc.Format;suspended=FsrPresentation::TranslateResizeDescriptor(zeroDesc);
    Require(suspended && *suspended && (*suspended)->BufferDesc.Width && (*suspended)->BufferDesc.Height,"RestoreResolvesNonzeroClientExtent");
    rig.ValidateDebug();std::puts("PASS: NewDX12 descriptor/admission, deferred startup, per-source callback transaction and separate UI registration (vendor double)");
}
