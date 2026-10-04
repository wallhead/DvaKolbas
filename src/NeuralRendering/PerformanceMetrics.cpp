#include "PerformanceMetrics.h"
#include <algorithm>
#include <chrono>
#include <cmath>
namespace TheosRenderPipeline::NeuralRendering {
PerformanceMetrics::PerformanceMetrics(size_t capacity):capacity_(capacity){}
void PerformanceMetrics::Enable(bool value){enabled_=value;if(value)data_.frames.reserve(capacity_);else current_=nullptr;}
void PerformanceMetrics::BeginFrame(uint64_t id,bool enabled){current_=nullptr;if(!enabled_)return;
    if(data_.frames.size()==capacity_){++data_.droppedFrames;return;}data_.frames.emplace_back();current_=&data_.frames.back();current_->sourceId=id;current_->nrEnabled=enabled;}
void PerformanceMetrics::RecordCpu(CpuPhase phase,uint64_t begin,uint64_t end){
    if(!current_||phase>=CpuPhase::Count||end<begin)return;
    current_->cpuNanoseconds[size_t(phase)]+=end-begin;
    if(current_->intervalCount==current_->intervals.size()){++data_.droppedIntervals;return;}
    current_->intervals[current_->intervalCount++]={begin,end};
}
void PerformanceMetrics::RecordWait(CpuPhase phase,bool blocked,uint64_t elapsed){
    if(!current_||phase>=CpuPhase::Count)return;++current_->waitCalls;
    if(blocked){++current_->blockingCalls;current_->blockNanoseconds+=elapsed;current_->waitNanoseconds[size_t(phase)]+=elapsed;}
}
void PerformanceMetrics::RecordGpu(GpuPhase phase,uint64_t begin,uint64_t end,uint64_t frequency,bool retired,bool disjoint){
    if(current_)RecordGpuFor(current_->sourceId,phase,begin,end,frequency,retired,disjoint);
}
void PerformanceMetrics::RecordGpuFor(uint64_t source,GpuPhase phase,uint64_t begin,uint64_t end,uint64_t frequency,bool retired,bool disjoint){
    if(!enabled_||phase>=GpuPhase::Count||!retired||disjoint||!frequency||end<begin)return;
    for(auto it=data_.frames.rbegin();it!=data_.frames.rend();++it)if(it->sourceId==source){
        it->gpuMilliseconds[size_t(phase)]=double(end-begin)*1000.0/double(frequency);return;}
}
void PerformanceMetrics::RecordFlush(){if(current_)++current_->flushes;}
void PerformanceMetrics::RecordDescriptorCreation(){if(current_)++current_->descriptorCreations;}
void PerformanceMetrics::RecordResourceCreation(){if(current_)++current_->resourceCreations;}
void PerformanceMetrics::RecordSubmitted(){if(current_)++current_->submitted;}
void PerformanceMetrics::RecordCompleted(){if(current_)++current_->completed;}
void PerformanceMetrics::RecordCompletedFor(uint64_t source){if(!enabled_)return;for(auto it=data_.frames.rbegin();it!=data_.frames.rend();++it)if(it->sourceId==source){++it->completed;return;}}
void PerformanceMetrics::RecordSlotPressure(){if(current_)++current_->slotPressure;}
PerformanceSnapshot PerformanceMetrics::Snapshot()const{
    auto copy=data_;
    for(auto& frame:copy.frames){
        auto intervals=frame.intervals;
        std::sort(intervals.begin(),intervals.begin()+frame.intervalCount,[](auto a,auto b){return a.begin<b.begin;});
        uint64_t begin{},end{};bool active=false;
        for(size_t i=0;i<frame.intervalCount;++i){const auto v=intervals[i];
            if(!active){begin=v.begin;end=v.end;active=true;}
            else if(v.begin<=end)end=std::max(end,v.end);
            else{frame.cpuUnionNanoseconds+=end-begin;begin=v.begin;end=v.end;}}
        if(active)frame.cpuUnionNanoseconds+=end-begin;
    }return copy;
}
bool EquivalentWorkload(const PerformanceWorkload& a,const PerformanceWorkload& b)noexcept{
    return a.width==b.width&&a.height==b.height&&a.placement==b.placement&&a.runtimeHash==b.runtimeHash&&
        a.passes==b.passes&&a.stableColors==b.stableColors&&std::isfinite(a.localTone)&&std::isfinite(b.localTone)&&a.localTone==b.localTone;
}
uint64_t PerformanceNow()noexcept{return uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());}
PerformanceScope::PerformanceScope(PerformanceMetrics* metrics,CpuPhase phase)noexcept:
    metrics_(metrics&&metrics->Enabled()?metrics:nullptr),phase_(phase){if(metrics_)begin_=PerformanceNow();}
PerformanceScope::~PerformanceScope(){if(metrics_)metrics_->RecordCpu(phase_,begin_,PerformanceNow());}
std::string_view PhaseName(CpuPhase phase)noexcept{
    constexpr std::array names{"total","bridge","prepareColor","prepareGuides","inputCopy","producerSignal","producerWait","begin","record","submit","delivery","consumerWait","completionWait","drain","encode","preparedWait","interopWait",
        "fsrTotal","fsrPrepareColor","fsrPrepareGuides","fsrProducerSignal","fsrBegin","fsrRecord","fsrSubmit","fsrDelivery"};
    static_assert(names.size()==size_t(CpuPhase::Count));return phase<CpuPhase::Count?names[size_t(phase)]:"invalid";
}
std::string_view PhaseName(GpuPhase phase)noexcept{
    constexpr std::array names{"prepareColor","prepareGuides","inputCopy","vendor","alpha","delivery","encode","fsrPrepare","fsrDispatch","fsrDelivery"};
    static_assert(names.size()==size_t(GpuPhase::Count));return phase<GpuPhase::Count?names[size_t(phase)]:"invalid";
}
}
