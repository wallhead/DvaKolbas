#include "NeuralRendering/PerformanceMetrics.h"
#include <cstdio>
#include <cmath>
using namespace TheosRenderPipeline::NeuralRendering;
template<class T>void LateCompletion(T& metrics){if constexpr(requires{metrics.RecordCompletedFor(1);})metrics.RecordCompletedFor(1);else metrics.RecordCompleted();}
int main() {
    int failures{};
    const auto check=[&](bool value,const char* name){std::printf("%s %s\n",value?"PASS":"FAIL",name);failures+=!value;};
    PerformanceMetrics metrics(4);
    metrics.BeginFrame(1,true);
    check(metrics.Snapshot().frames.empty(),"DisabledMetricsAllocateNoFrameSamples");
    metrics.Enable(true);
    metrics.BeginFrame(1,true);
    metrics.RecordWait(CpuPhase::ProducerWait,false,500);
    metrics.RecordCpu(CpuPhase::Total,100,300);
    metrics.RecordCpu(CpuPhase::Record,150,250);
    metrics.RecordCpu(CpuPhase::Delivery,280,350);
    metrics.RecordFlush();
    metrics.RecordDescriptorCreation();
    auto snapshot=metrics.Snapshot();
    check(snapshot.frames.size()==1&&snapshot.frames[0].sourceId==1,"SourceIdentityRetained");
    const auto& first=snapshot.frames[0];
    check(first.waitCalls==1&&first.blockingCalls==0&&first.blockNanoseconds==0,"CompletedFenceCheckIsNotBlockingTime");
    check(first.cpuUnionNanoseconds==250,"NestedCpuIntervalsNotDoubleCounted");
    check(first.flushes==1&&first.descriptorCreations==1,"FlushAndCreationCountSeparateFromStalls");
    metrics.RecordWait(CpuPhase::ConsumerWait,true,70);
    metrics.RecordGpu(GpuPhase::Vendor,100,200,1000,true,false);
    snapshot=metrics.Snapshot();
    check(snapshot.frames[0].blockNanoseconds==70&&snapshot.frames[0].blockingCalls==1,"ActualBlockingDurationRecorded");
    check(snapshot.frames[0].gpuMilliseconds[size_t(GpuPhase::Vendor)]==100.0,"GpuFrequencyConvertsTicksToMilliseconds");
    metrics.BeginFrame(2,false);
    metrics.RecordGpu(GpuPhase::Vendor,100,200,1000,false,false);
    metrics.RecordGpu(GpuPhase::Alpha,100,200,1000,true,true);
    metrics.RecordGpu(GpuPhase::Encode,200,100,1000,true,false);
    metrics.RecordGpu(GpuPhase::InputCopy,100,200,0,true,false);
    snapshot=metrics.Snapshot();
    const auto& second=snapshot.frames[1];
    check(!second.nrEnabled&&!second.gpuMilliseconds[size_t(GpuPhase::Vendor)]&&
        !second.gpuMilliseconds[size_t(GpuPhase::Alpha)]&&!second.gpuMilliseconds[size_t(GpuPhase::Encode)]&&
        !second.gpuMilliseconds[size_t(GpuPhase::InputCopy)],"PendingDisjointInvalidQueriesRemainUnavailable");
    // Late GPU collection must update the original source, not the current one.
    metrics.RecordGpuFor(1,GpuPhase::Delivery,10,15,1000,true,false);
    snapshot=metrics.Snapshot();
    check(snapshot.frames[0].gpuMilliseconds[size_t(GpuPhase::Delivery)]==5.0&&
        !snapshot.frames[1].gpuMilliseconds[size_t(GpuPhase::Delivery)],"LateQueriesKeepSourceIdentity");
    LateCompletion(metrics);snapshot=metrics.Snapshot();
    check(snapshot.frames[0].completed==1&&snapshot.frames[1].completed==0,"LateRetirementKeepsSourceIdentity");
    metrics.EndFrame();metrics.RecordFlush();
    check(metrics.Snapshot().frames[1].flushes==0,"LifecycleCollectionDoesNotContaminateLastFrame");
    PerformanceWorkload left{2560,1440,"Before","e67dee",1,0.0f},right=left;
    check(EquivalentWorkload(left,right),"IdenticalWorkloadsMatch");
    right.width=1920;check(!EquivalentWorkload(left,right),"DifferentExtentIsUnmatched");right=left;
    right.placement="After";check(!EquivalentWorkload(left,right),"DifferentPlacementIsUnmatched");right=left;
    right.passes=2;check(!EquivalentWorkload(left,right),"DifferentPassCountIsUnmatched");right=left;
    right.localTone=1;check(!EquivalentWorkload(left,right),"DifferentToneIsUnmatched");right=left;
    right.stableColors=false;check(!EquivalentWorkload(left,right),"DifferentColorResolveIsUnmatched");right=left;
    right.runtimeHash="8270";check(!EquivalentWorkload(left,right),"DifferentRuntimeIsConfounded");
    metrics.BeginFrame(3,true);metrics.BeginFrame(4,true);metrics.BeginFrame(5,true);
    check(metrics.Snapshot().droppedFrames==1,"BoundedCapacityReportsDroppedSamples");
    return failures?1:0;
}
