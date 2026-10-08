#include "NeuralRendering/SourcePolicy.h"
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
int main(){int failures{};auto check=[&](bool v,const char* n){std::printf("%s %s\n",v?"PASS":"FAIL",n);failures+=!v;};
    check(!SourceWorldEligible(true,false,true,false),"FallbackWithoutUiHandoffCannotRunWorldNr");
    check(SourceWorldEligible(true,true,true,false),"CleanNativeWorldAdmitted");
    check(!SourceWorldEligible(true,true,true,true),"ExternalWorldUnsupportedInTrial");
    check(!SourceResetAfterNr(false,false,false,true),"UnavailableNrCameraResetCannotResetSr");
    check(SourceResetAfterNr(false,true,false,true),"EvaluatedNrDiscontinuityReachesSr");
    check(SourceResetAfterNr(false,false,true,true),"LeavingActiveNrResetsSrOnce");
    check(SourceResetAfterNr(true,false,false,false),"CallerResetSurvivesNrBypass");
    check(!SourceResetForNrSettings(false,true,false,false),"AfterToneChangeDoesNotResetSr");
    check(SourceResetForNrSettings(false,true,true,false),"BeforeToneChangeResetsSr");
    check(SourceResetForNrSettings(false,true,false,true),"LeavingBeforePlacementResetsSr");
    check(SourceResetForNrSettings(true,false,false,false),"AfterSettingsKeepCallerReset");
    int nr{},present{};
    const auto blocked=RetireBeforeSourceResize([&]{++nr;return E_FAIL;},[&]{++present;return S_OK;});
    check(FAILED(blocked)&&nr==1&&present==0,"FailedNrRetirementCannotTouchPresenterOrBuffers");
    nr=present=0;const auto okay=RetireBeforeSourceResize([&]{++nr;return S_OK;},[&]{++present;return nr==1?S_OK:E_FAIL;});
    check(SUCCEEDED(okay)&&nr==1&&present==1,"NrRetiresBeforePresenter");
    SourceCameraHistory history;TheosRenderPipeline::Upscaling::CameraMeasurements c;c.identity=1;c.nearDistance=.1f;c.farDistance=100;c.verticalFovRadians=1;
    c.view={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};c.projection=c.view;
    check(history.Accept(1,c,{320,180}).reset,"FirstCameraReset");
    check(!history.Accept(2,c,{320,180}).reset,"ContinuousCameraDoesNotReset");
    c.position[0]=100;check(history.Accept(3,c,{320,180}).reset,"SameCameraTeleportResetsBeforeNr");
    c.verticalFovRadians=1.2f;check(history.Accept(4,c,{320,180}).reset,"SameCameraFovChangeResetsBeforeNr");
    c.view[0]=-1;check(history.Accept(5,c,{320,180}).reset,"SameCameraLargeRotationResetsBeforeNr");
    history.Invalidate();check(history.Accept(6,c,{320,180}).reset,"InvalidWorldReentryResetsBeforeNr");
    check(!history.Accept(7,c,{320,180},false).valid,"ValidCameraCannotOverrideSourceExclusion");
    check(history.Accept(8,c,{320,180},true).reset,"RejectedSourceInvalidatesCameraHistoryForReentry");
    return failures?1:0;
}
