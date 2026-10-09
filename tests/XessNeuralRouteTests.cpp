#include "RendererBackendPolicy.h"
#include "RendererSettingsEdits.h"
#include "NeuralRenderingMode.h"
#include "NeuralRendering/BeforeSettings.h"
#include "NeuralRendering/PostSrContract.h"
#include "NeuralRendering/SourcePolicy.h"
#include <cstdio>
using namespace TheosRenderPipeline;
using namespace NeuralRendering;
int main() {
    int failures{};
    const auto check=[&](bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);failures+=!ok;};
    Upscaling::BackendConfiguration config;
    config.backend=Upscaling::BackendKind::Xess;config.adapterVendorId=0x10de;
    config.enabled=true;config.generationBackend=0;config.generationEnabled=false;
    config.neuralRendering=config.communityNeural=true;
    check(ResolveBackend(config,true,true).valid,"NvidiaXeSSCommunityNrOrdinaryRouteAccepted");
    config.generationBackend=2;config.generationEnabled=true;
    auto generated=ResolveBackend(config,true,true);
    check(generated.valid && generated.presentation==Upscaling::PresentationKind::Fsr,"XeSSCommunityNrFsrPresenterAccepted");
    check(!ResolveBackend(config,true,false).valid,"XeSSFsrPresenterRequiresBuiltTransport");
    config.generationEnabled=false;
    check(ResolveBackend(config,true,true).valid,"XeSSFgOffRetainsFsrPresenterForLiveReentry");
    config.generationBackend=1;
    check(!ResolveBackend(config,true,true).valid,"XeSSNvidiaPresenterStillAwaitingQualification");
    config.generationBackend=0;
    check(SupportsNeuralRenderingMode(Xess,false,true) && !SupportsNeuralRenderingMode(Xess,false,false),"XeSSUsesCommunityNrOnly");
    RendererSettingsDraft draft;draft.upscaleType=Xess;draft.sourceDLSSG.neuralEnabled=true;
    check(PrepareRendererStartupDraft(draft,true).sourceDLSSG.neuralEnabled,"StartupXeSSPreservesCommunityNrRequest");
    draft.generationBackendPreference=GenerationBackendPreference::Fsr;draft.generationBackend=2;draft.generationEnabled=true;
    const auto fgDraft=PrepareRendererStartupDraft(draft,true);
    check(fgDraft.generationBackend==2 && fgDraft.generationEnabled,"XeSSStartupPreservesExplicitFsrFgRequest");
    draft.generationBackendPreference=GenerationBackendPreference::Auto;draft.generationBackend=0;draft.generationEnabled=false;
    draft.upscaleType=DLAA;draft.xessMode={true,false,true,false,false,0,false};
    SetRendererUpscaleMode(draft,Xess);
    check(draft.sourceDLSSG.neuralEnabled,"ReturningToXeSSRestoresNrPreference");
    draft.valid=true;draft.sourceDLSSG.neuralBeforeUpscaling=false;draft.xess.quality=Upscaling::Quality::Performance;
    draft.fsr.quality=static_cast<Upscaling::Quality>(99);
    RendererSettingsCapabilities caps{true,true,true,false};caps.communityNeural=true;caps.adapterVendorId=0x10de;
    check(!ValidateRendererSettings(draft,caps),"XeSSAfterUsesXeSSScaleRatherThanInactiveFsrPreference");
    config.communityNeural=false;
    check(!ResolveBackend(config,true,true).valid,"XeSSLegacyNrRejected");
    config.communityNeural=true;
    for(auto vendor:{0x1002u,0x8086u}) {
        config.adapterVendorId=vendor;config.fsrOnlyRenderer=false;
        check(!ResolveBackend(config,true,true).valid,"CrossVendorXeSSNrRejected");
        config.neuralRendering=false;
        check(ResolveBackend(config,true,true).valid,"CrossVendorXeSSWithoutNrAccepted");
        config.neuralRendering=true;
    }
    SourceDLSSG::Preferences prefs; prefs.neuralBeforeUpscaling=false;
    check(!NativeAfterUnavailable(prefs,Xess,Upscaling::Quality::Performance,false),"ScaledXeSSAfterPlacementAccepted");
    prefs.neuralBeforeUpscaling=true;
    check(!NativeAfterUnavailable(prefs,Xess,Upscaling::Quality::NativeAA,false),"NativeXeSSBeforePlacementAccepted");
    PostSrSourceContract source;
    source.backend=Upscaling::BackendKind::Xess;source.outcome=Upscaling::UpscaleOutcome::Temporal;
    source.epoch=source.guideEpoch=1;source.sourceId=source.guideSourceId=2;source.previousSourceId=1;
    source.sourceTime=source.guideTime=1.;source.display=source.color={1920,1080};
    source.colorDomain=ColorDomain::SdrBytes;source.encoding=Upscaling::ColorEncoding::Gamma22;
    source.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;source.depthFormat=DXGI_FORMAT_R32_FLOAT;
    source.motionFormat=DXGI_FORMAT_R16G16_FLOAT;source.guideOrigin=GuideOrigin::RealSource;
    for(auto extent:{ImageExtent{1920,1080},ImageExtent{960,540}}) {
        source.render=source.guides=extent;source.motion={float(extent.width),float(extent.height),true,false};
        auto accepted=ValidatePostSrSourceContract(source);
        check(accepted && accepted->motionScaleX==1920 && accepted->extent==extent,"XeSSNativeOrScaledRealGuidesAccepted");
    }
    source.guides.width--;
    float x=1920,y=1080;auto rejected=BindPostSrMotionScales(source,x,y);
    check(!rejected && rejected.error().message.find("extent")!=std::string::npos && x==0 && y==0,"StaleXeSSGuidesDiagnosedAndNoFallbackScale");
    source.guides=source.render;source.outcome=Upscaling::UpscaleOutcome::SpatialRecovery;
    check(!ValidatePostSrSourceContract(source),"XeSSSpatialRecoveryCannotRunAfterNr");
    check(!SourceWorldEligible(false,true,true,false),"LoadingRecoveryCannotRunBeforeNr");
    check(SourceResetForNrSettings(false,true,true,false) && SourceResetForNrSettings(false,true,false,true),"BeforeEditAndPlacementChangeResetSrHistory");
    return failures?1:0;
}
