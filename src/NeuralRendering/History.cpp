#include "History.h"
#include <cmath>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
std::unexpected<Error> Invalid(const char* text){return std::unexpected(Error{ErrorKind::InvalidInput,0,text});}
}
Result<HistoryDecision> History::Check(const ImagePacket& p,const SettingsSnapshot& s)const{
    if(exhausted_ || generation_==UINT64_MAX)return Invalid("NR history generation exhausted");
    if(!s.enabled || !s.revision || s.revision<settingsRevision_ ||
        (s.placement!=Placement::Before && s.placement!=Placement::After))return Invalid("NR recording settings disabled/invalid/stale");
    if(p.kind==ImageKind::Generated)return std::unexpected(Error{ErrorKind::Unsupported,0,"NR generated temporal schedule is not qualified"});
    if(p.kind!=ImageKind::Real || !p.epoch || !p.sourceId || !p.imageId || !p.batchId ||
        p.previousSourceId>=p.sourceId || !std::isfinite(p.presentationTime))return Invalid("NR temporal image identity invalid");
    if(p.epoch<epoch_)return Invalid("NR temporal image belongs to old epoch");
    if(p.epoch==epoch_ && (p.sourceId<=source_ || p.imageId<=image_ || p.presentationTime<=time_))
        return Invalid("NR temporal image is duplicate/stale/backward");
    HistoryDecision d;d.owner_=this;d.generation_=generation_;d.epoch_=p.epoch;d.source_=p.sourceId;
    d.image_=p.imageId;d.settingsRevision_=s.revision;d.time_=p.presentationTime;
    d.reset_=resetNext_ || p.reset || p.epoch!=epoch_ || source_==UINT64_MAX ||
        p.sourceId!=source_+1 || p.previousSourceId!=source_;
    return d;
}
Result<void> History::CommitRecorded(const HistoryDecision& d){
    if(exhausted_ || d.owner_!=this || d.generation_!=generation_ || generation_==UINT64_MAX)
        return Invalid("NR history decision is stale/foreign");
    epoch_=d.epoch_;source_=d.source_;image_=d.image_;settingsRevision_=d.settingsRevision_;time_=d.time_;
    resetNext_=false;++generation_;return {};
}
void History::ResetNext()noexcept{
    resetNext_=true;
    if(generation_==UINT64_MAX)exhausted_=true;else ++generation_;
}
}
