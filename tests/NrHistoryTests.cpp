#include "NeuralRendering/History.h"
#include <cstdio>
#include <limits>
#include <memory>
using namespace TheosRenderPipeline::NeuralRendering;
int main(){
    int failed{};const auto check=[&](bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);if(!ok)++failed;};
    SettingsSnapshot settings;settings.enabled=true;settings.revision=1;
    ImagePacket p;p.epoch=1;p.sourceId=7;p.previousSourceId=6;p.imageId=10;p.batchId=7;p.presentationTime=1;
    History h;auto first=h.Check(p,settings);
    check(first&&first->Reset(),"FirstEligibleImageResetsHistory");
    auto retry=h.Check(p,settings);check(bool(retry),"ValidationWithoutVendorRecordingDoesNotConsumeImage");
    if(first)check(bool(h.CommitRecorded(*first)),"RecordedIdentityCommitted");
    check(!h.Check(p,settings),"DuplicateRecordedImageRejected");
    if(retry)check(!h.CommitRecorded(*retry),"StaleDecisionCannotCommitAfterAnotherRecording");
    auto next=p;next.sourceId=8;next.previousSourceId=7;next.imageId=11;next.batchId=8;next.presentationTime=2;
    auto d=h.Check(next,settings);check(d&&!d->Reset(),"ConsecutiveRealSourceKeepsHistory");
    auto gap=next;gap.sourceId=10;gap.previousSourceId=9;gap.imageId=12;auto g=h.Check(gap,settings);
    check(g&&g->Reset(),"SkippedSourceResetsTemporalHistory");
    auto old=next;old.presentationTime=.5;check(!h.Check(old,settings),"BackwardTimeRejectedInSameEpoch");
    old=next;old.presentationTime=std::numeric_limits<double>::quiet_NaN();check(!h.Check(old,settings),"NanTemporalTimeRejected");
    auto generated=next;generated.kind=ImageKind::Generated;check(!h.Check(generated,settings),"GeneratedTemporalScheduleStillUnqualified");
    auto disabled=settings;disabled.enabled=false;check(!h.Check(next,disabled),"DisabledNrCannotAdmitRecording");
    disabled=settings;disabled.revision=0;check(!h.Check(next,disabled),"MissingSettingsRevisionRejected");
    disabled=settings;disabled.placement=static_cast<Placement>(99);check(!h.Check(next,disabled),"UnknownPlacementRejected");
    h.ResetNext();d=h.Check(next,settings);check(d&&d->Reset(),"ReenableOrCameraResetAffectsNextEligibleImage");
    auto epoch=next;epoch.epoch=2;epoch.sourceId=1;epoch.previousSourceId=0;epoch.imageId=1;epoch.batchId=1;epoch.presentationTime=0;
    auto e=h.Check(epoch,settings);check(e&&e->Reset(),"NewEpochAllowsRestartedSourceAndTimeWithReset");
    History other;if(e)check(!other.CommitRecorded(*e),"DecisionFromAnotherHistoryOwnerRejected");
    if(e)check(bool(h.CommitRecorded(*e)),"NewEpochRecordedIdentityCommitted");check(!h.Check(next,settings),"OldEpochRejectedAfterNewEpochRecording");
    alignas(History) unsigned char storage[sizeof(History)];
    auto* recycled=std::construct_at(reinterpret_cast<History*>(storage));auto staleOwner=recycled->Check(p,settings);
    std::destroy_at(recycled);recycled=std::construct_at(reinterpret_cast<History*>(storage));
    if(staleOwner)check(!recycled->CommitRecorded(*staleOwner),"DecisionCannotCommitToSameAddressReplacementHistory");
    std::destroy_at(recycled);
    return failed?1:0;
}
