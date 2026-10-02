#include "nr-runtime/RuntimeProbeReport.h"
#include <cstdio>
using namespace NrRuntimeResearch;
int main() {
    int failed{};
    const auto check=[&](bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);if(!ok)++failed;};
    ProbeReport good;
    good.profile="fixture";good.runtimeSha256=std::string(64,'a');good.coreSha256=std::string(64,'b');
    good.init=good.create=good.evaluate=good.release=good.destroyParameters=good.shutdown=1;
    good.requestedFrames=good.readbackFrames=30;good.distinctOutputHashes=30;
    good.spatiallyVariedFrames=30;
    good.outputPixels=good.finitePixels=good.overwrittenPixels=1000;good.changedFromInputPixels=100;
    good.allocations=good.releases=4;good.runtimeHeldAndMatched=good.coreHeldAndMatched=good.outputReadersRetired=true;
    check(Validate(good).empty(),"CompleteSchemaFixture_NotMeasuredGpuResult");
    auto r=good;r.readbackFrames=0;check(!Validate(r).empty(),"InitSuccessAloneIsNotOutput");
    r=good;r.distinctOutputHashes=1;check(!Validate(r).empty(),"ConstantOutputNotQualified");
    r=good;r.spatiallyVariedFrames=0;check(!Validate(r).empty(),"ChangingUniformFramesAreNotSceneOutput");
    r=good;r.spatiallyVariedFrames=29;check(!Validate(r).empty(),"EveryReadbackMustHaveSpatialVariation");
    r=good;r.overwrittenPixels=0;check(!Validate(r).empty(),"UntouchedSentinelIsNotOutput");
    r=good;r.finitePixels=999;check(!Validate(r).empty(),"NanOutputRejected");
    r=good;r.changedFromInputPixels=0;check(!Validate(r).empty(),"PassthroughIsNotNrOutput");
    r=good;r.outputReadersRetired=false;check(!Validate(r).empty(),"RecordingIsNotGpuRetirement");
    r=good;r.releases=3;check(!Validate(r).empty(),"UnbalancedResourceCallbacksRejected");
    r=good;r.shutdown=0xbad00002;check(!Validate(r).empty(),"FailedShutdownCannotPass");
    r=good;r.shimRequested=true;r.shimRestored=false;check(!Validate(r).empty(),"UnrestoredShimRetainsFailure");
    r=good;r.coreHeldAndMatched=false;check(!Validate(r).empty(),"UnretainedDriverCoreRejected");
    r=good;r.init=0xbad00002;check(!Validate(r).empty(),"RejectedInitCannotPass");
    r=good;r.readbackFrames=29;check(!Validate(r).empty(),"ShortRunCannotPass");
    return failed?1:0;
}
