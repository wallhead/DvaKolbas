#pragma once
#include "NeuralRendering/PerformanceQueries.h"
#include "Upscaling/FSRFrameAdapter.h"
namespace NrRuntimeResearch {
class FsrPerformanceRecorder final:public TheosRenderPipeline::Upscaling::FsrPerformanceObserver {
    using CpuPhase=TheosRenderPipeline::Upscaling::FsrCpuPhase;
    using Gpu=TheosRenderPipeline::Upscaling::FsrGpuPhase;
    using Metrics=TheosRenderPipeline::NeuralRendering::PerformanceMetrics;
    using Phase=TheosRenderPipeline::NeuralRendering::CpuPhase;
    using GpuPhase=TheosRenderPipeline::NeuralRendering::GpuPhase;
    Metrics& metrics_;TheosRenderPipeline::NeuralRendering::PerformanceQueries queries_;
    std::array<uint64_t,size_t(CpuPhase::Count)> begins_{};
    static constexpr std::array phases_{Phase::FsrTotal,Phase::FsrPrepareColor,Phase::FsrPrepareGuides,Phase::FsrProducerSignal,Phase::FsrBegin,Phase::FsrRecord,Phase::FsrSubmit,Phase::FsrDelivery};
    static GpuPhase Map(Gpu p){return p==Gpu::Prepare?GpuPhase::FsrPrepare:p==Gpu::Dispatch?GpuPhase::FsrDispatch:GpuPhase::FsrDelivery;}
public:
    explicit FsrPerformanceRecorder(Metrics& m):metrics_(m),queries_(&m){}
    HRESULT Initialize(ID3D11Device* d11,ID3D12Device* d12,ID3D12CommandQueue* queue){
        auto hr=queries_.Initialize11(d11);return FAILED(hr)?hr:queries_.Initialize12(d12,queue);
    }
    void Cpu(TheosRenderPipeline::Upscaling::FsrCpuPhase p,bool begin)override{
        if(begin)begins_[size_t(p)]=TheosRenderPipeline::NeuralRendering::PerformanceNow();
        else metrics_.RecordCpu(phases_[size_t(p)],begins_[size_t(p)],TheosRenderPipeline::NeuralRendering::PerformanceNow());
    }
    void Begin11(ID3D11DeviceContext* c,uint64_t source)override{queries_.Begin11(c,source);}
    void Stamp11(ID3D11DeviceContext* c,Gpu p,bool begin)override{queries_.Stamp11(c,Map(p),begin);}
    void End11(ID3D11DeviceContext* c)override{queries_.End11(c);}
    void Begin12(ID3D12GraphicsCommandList* c,uint64_t source)override{queries_.Begin12(c,source);}
    void Stamp12(ID3D12GraphicsCommandList* c,Gpu p,bool begin)override{queries_.Stamp12(c,Map(p),begin);}
    void Resolve12(ID3D12GraphicsCommandList* c)override{queries_.Resolve12(c);}
    void DiscardUnsubmitted12()override{queries_.DiscardUnsubmitted12();}
    void Collect(ID3D11DeviceContext* c){queries_.Collect11(c);queries_.Collect12();}
    uint64_t Dropped()const{return queries_.Dropped();}
    bool Available11()const{return queries_.Available11();}bool Available12()const{return queries_.Available12();}
    TheosRenderPipeline::Graphics::InteropPerformanceSink InteropSink(){return {this,
        [](void* p,bool blocked,uint64_t ns){static_cast<FsrPerformanceRecorder*>(p)->metrics_.RecordWait(Phase::InteropWait,blocked,ns);},
        [](void* p){static_cast<FsrPerformanceRecorder*>(p)->metrics_.RecordFlush();},
        [](void* p,TheosRenderPipeline::Graphics::InteropWork work,ID3D12Fence* fence,uint64_t value){
            if(work==TheosRenderPipeline::Graphics::InteropWork::Upscaling)static_cast<FsrPerformanceRecorder*>(p)->queries_.Submitted12(fence,value);}};
    }
};
}
