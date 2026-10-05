#include "NeuralRendering/PerformanceQueries.h"
#include "Graphics/D3D11D3D12Interop.h"
#include "nr-runtime/GpuProbeGuard.h"
#include "D3D11QueryReadinessProbe.h"
#include <cstdio>
#include <d3d11_4.h>
#include <dxgi1_6.h>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {void Need(HRESULT hr){if(FAILED(hr)){std::printf("GPU failure %08x\n",unsigned(hr));ExitProcess(1);}}}
int main(){
    if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
    int failures{};const auto check=[&](bool v,const char* name){std::printf("%s %s\n",v?"PASS":"FAIL",name);failures+=!v;};
    ComPtr<ID3D12Device> device;Need(D3D12CreateDevice(nullptr,D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device)));
    ComPtr<ID3D12CommandQueue> queue;D3D12_COMMAND_QUEUE_DESC desc{};Need(device->CreateCommandQueue(&desc,IID_PPV_ARGS(&queue)));
    ComPtr<ID3D12CommandAllocator> allocator;Need(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator)));
    ComPtr<ID3D12GraphicsCommandList> list;Need(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list)));
    ComPtr<ID3D12Fence> gate,done;Need(device->CreateFence(0,D3D12_FENCE_FLAG_SHARED,IID_PPV_ARGS(&gate)));Need(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&done)));
    PerformanceMetrics metrics;metrics.Enable(true);metrics.BeginFrame(1,true);
    PerformanceQueries queries(&metrics);Need(queries.Initialize12(device.Get(),queue.Get()));
    check(queries.Available12(),"RealQueueTimestampFrequencyAvailable");
    queries.Begin12(list.Get(),1);queries.Stamp12(list.Get(),GpuPhase::Vendor,true);queries.Stamp12(list.Get(),GpuPhase::Vendor,false);queries.Resolve12(list.Get());Need(list->Close());
    Need(queue->Wait(gate.Get(),1));ID3D12CommandList* lists[]={list.Get()};queue->ExecuteCommandLists(1,lists);Need(queue->Signal(done.Get(),1));queries.Submitted12(done.Get(),1);
    const auto before=PerformanceNow();queries.Collect12();const auto elapsed=PerformanceNow()-before;
    check(!metrics.Snapshot().frames[0].gpuMilliseconds[size_t(GpuPhase::Vendor)],"PendingGpuTimestampDoesNotReadUnretiredData");
    check(elapsed<100000000,"PendingTimestampCollectionReturnsWithoutWaiting");
    Need(gate->Signal(1));HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!event)ExitProcess(1);
    Need(done->SetEventOnCompletion(1,event));if(WaitForSingleObject(event,20000)!=WAIT_OBJECT_0)ExitProcess(1);CloseHandle(event);
    queries.Collect12();
    const auto result=metrics.Snapshot().frames[0].gpuMilliseconds[size_t(GpuPhase::Vendor)];
    check(result.has_value()&&*result>=0,"ActualRetiredGpuTimestampCollected");
    ComPtr<IDXGIFactory4> factory;Need(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));ComPtr<IDXGIAdapter1> adapter;Need(factory->EnumAdapterByLuid(device->GetAdapterLuid(),IID_PPV_ARGS(&adapter)));
    ComPtr<ID3D11Device> device11;ComPtr<ID3D11DeviceContext> context;Need(D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device11,nullptr,&context));
    ComPtr<ID3D11Device5> device5;ComPtr<ID3D11DeviceContext4> context4;Need(device11.As(&device5));Need(context.As(&context4));
    HANDLE shared{};Need(device->CreateSharedHandle(gate.Get(),nullptr,GENERIC_ALL,nullptr,&shared));ComPtr<ID3D11Fence> gate11;Need(device5->OpenSharedFence(shared,IID_PPV_ARGS(&gate11)));CloseHandle(shared);
    metrics.BeginFrame(2,true);Need(queries.Initialize11(device11.Get()));check(queries.Available11(),"RealD3D11DisjointQueriesAvailable");
    Need(context4->Wait(gate11.Get(),2));queries.Begin11(context.Get(),2);queries.Stamp11(context.Get(),GpuPhase::PrepareColor,true);queries.Stamp11(context.Get(),GpuPhase::PrepareColor,false);queries.End11(context.Get());Need(context4->Signal(gate11.Get(),3));context->Flush();
    queries.Collect11(context.Get());check(!metrics.Snapshot().frames[1].gpuMilliseconds[size_t(GpuPhase::PrepareColor)],"PendingD3D11QueriesDoNotForceFlushOrWait");
    Need(gate->Signal(2));event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Need(gate->SetEventOnCompletion(3,event));if(WaitForSingleObject(event,20000)!=WAIT_OBJECT_0)ExitProcess(1);CloseHandle(event);
    queries.Collect11(context.Get());check(metrics.Snapshot().frames[1].gpuMilliseconds[size_t(GpuPhase::PrepareColor)].has_value(),"RetiredD3D11TimestampAndFrequencyCollected");
    metrics.BeginFrame(3,true);
    for(unsigned i=0;i<20;++i){
        queries.Begin12(list.Get(),3);queries.DiscardUnsubmitted12();
    }
    check(queries.Dropped()==0,"SuccessfullyDiscardedUnsubmittedQueriesReuseBoundedSlots");
    check(!metrics.Snapshot().frames[2].gpuMilliseconds[size_t(GpuPhase::Vendor)],"DiscardedRecordingNeverInventsGpuTiming");
    metrics.BeginFrame(4,false);
    TheosRenderPipeline::Graphics::D3D11D3D12Interop interop;Need(interop.Initialize(device11.Get(),device.Get(),queue.Get()));
    struct Submission {PerformanceQueries* queries;ID3D12Fence* fence{};uint64_t value{};unsigned calls{};} submission{&queries};
    interop.SetPerformanceSink({&submission,nullptr,nullptr,[](void* owner,TheosRenderPipeline::Graphics::InteropWork work,ID3D12Fence* fence,uint64_t value){
        auto& p=*static_cast<Submission*>(owner);if(work==TheosRenderPipeline::Graphics::InteropWork::Upscaling){p.fence=fence;p.value=value;++p.calls;p.queries->Submitted12(fence,value);}}});
    Need(queue->Wait(gate.Get(),5));Need(interop.SignalProducer());ID3D12GraphicsCommandList* observed{};Need(interop.Begin(&observed));
    queries.Begin12(observed,4);queries.Stamp12(observed,GpuPhase::FsrDispatch,true);queries.Stamp12(observed,GpuPhase::FsrDispatch,false);queries.Resolve12(observed);Need(interop.Submit());
    check(submission.calls==1&&submission.fence&&submission.value==interop.LastValue(TheosRenderPipeline::Graphics::InteropWork::Upscaling),"SubmissionObserverUsesActualInteropQueueFenceAndValue");
    queries.Collect12();check(!metrics.Snapshot().frames[3].gpuMilliseconds[size_t(GpuPhase::FsrDispatch)],"ObservedFsrSubmissionCannotCollectBeforeRealGpuGate");
    Need(gate->Signal(5));Need(interop.WaitConsumer());Need(interop.Drain());queries.Collect12();interop.SetPerformanceSink({});
    check(metrics.Snapshot().frames[3].gpuMilliseconds[size_t(GpuPhase::FsrDispatch)].has_value(),"ObservedFsrTimestampCollectsAfterGenuineRetirement");
    metrics.BeginFrame(5,true);
    queries.Begin11(context.Get(),5);
    for(const auto phase:{GpuPhase::PrepareColor,GpuPhase::PrepareGuides}){
        queries.Stamp11(context.Get(),phase,true);queries.Stamp11(context.Get(),phase,false);
    }
    queries.End11(context.Get());Need(context4->Signal(gate11.Get(),6));context->Flush();
    event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Need(gate->SetEventOnCompletion(6,event));
    if(WaitForSingleObject(event,20000)!=WAIT_OBJECT_0)ExitProcess(1);CloseHandle(event);
    {
        D3D11QueryReadinessProbe readiness(context.Get());
        check(readiness.Installed(),"PartialReadinessFixtureInstalledOnRealContext");
        if(!readiness.Installed())return 1;
        queries.Collect11(context.Get());
        const auto partial=metrics.Snapshot().frames[4].gpuMilliseconds;
        check(readiness.DelayedOneRead()&&partial[size_t(GpuPhase::PrepareColor)].has_value()&&
            !partial[size_t(GpuPhase::PrepareGuides)],"OneCompletedPhaseCollectedWhileAnotherRemainsPending");
        queries.Collect11(context.Get());
        check(readiness.CompletedPhaseReads()==2,"CompletedPhaseNotQueriedAgainWhenPendingPhaseResumes");
        check(metrics.Snapshot().frames[4].gpuMilliseconds[size_t(GpuPhase::PrepareGuides)].has_value(),"PreviouslyPendingPhaseCollectedOnRetry");
    }
    PerformanceMetrics off;PerformanceQueries disabled(&off);
    check(disabled.Initialize12(device.Get(),queue.Get())==S_FALSE&&!disabled.Available12(),"DisabledQueriesCreateNoGpuInstrumentation");
    return failures?1:0;
}
