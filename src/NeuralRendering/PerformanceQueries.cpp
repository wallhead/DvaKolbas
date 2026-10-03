#include "PerformanceQueries.h"
namespace TheosRenderPipeline::NeuralRendering {
namespace {constexpr size_t slotCount=16,phaseCount=size_t(GpuPhase::Count),queryStride=phaseCount*2;using Microsoft::WRL::ComPtr;}
struct PerformanceQueries::State {
    PerformanceMetrics* metrics{};
    struct Frame12 {ComPtr<ID3D12Fence> fence;uint64_t source{},completion{};
        uint32_t beginMask{},endMask{};bool recording{},pending{},resolved{};};
    struct Frame11 {ComPtr<ID3D11Query> disjoint;std::array<ComPtr<ID3D11Query>,queryStride> stamps;
        uint64_t source{};uint32_t beginMask{},endMask{};bool recording{},pending{};};
    std::array<Frame12,slotCount> frames12;
    std::array<Frame11,slotCount> frames11;
    ComPtr<ID3D12QueryHeap> heap;ComPtr<ID3D12Resource> readback;
    uint64_t frequency{},dropped{};int active12{-1},active11{-1};bool available12{},available11{};
    bool Enabled()const{return metrics&&metrics->Enabled();}
};
PerformanceQueries::PerformanceQueries(PerformanceMetrics* p):state_(std::make_unique<State>()){state_->metrics=p;}
PerformanceQueries::~PerformanceQueries(){
    // Never release an unsubmitted timestamp recording or an uncertain reader.
    for(const auto& frame:state_->frames12){
        if(frame.recording){state_.release();return;}
        if(frame.pending){const auto done=frame.fence?frame.fence->GetCompletedValue():0;
            if(done==UINT64_MAX||done<frame.completion){state_.release();return;}}}
    for(const auto& frame:state_->frames11)if(frame.recording||frame.pending){state_.release();return;}
}
HRESULT PerformanceQueries::Initialize12(ID3D12Device* device,ID3D12CommandQueue* queue){
    auto& s=*state_;if(!s.Enabled()||!device||!queue)return S_FALSE;
    auto hr=queue->GetTimestampFrequency(&s.frequency);if(FAILED(hr)||!s.frequency)return FAILED(hr)?hr:E_FAIL;
    D3D12_QUERY_HEAP_DESC query{};query.Type=D3D12_QUERY_HEAP_TYPE_TIMESTAMP;query.Count=UINT(slotCount*queryStride);
    hr=device->CreateQueryHeap(&query,IID_PPV_ARGS(&s.heap));if(FAILED(hr))return hr;
    D3D12_RESOURCE_DESC desc{};desc.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;desc.Width=slotCount*queryStride*sizeof(uint64_t);
    desc.Height=desc.DepthOrArraySize=desc.MipLevels=desc.SampleDesc.Count=1;desc.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_READBACK;
    hr=device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&s.readback));
    if(SUCCEEDED(hr))s.available12=true;return hr;
}
HRESULT PerformanceQueries::Initialize11(ID3D11Device* device){
    auto& s=*state_;if(!s.Enabled()||!device)return S_FALSE;
    for(auto& frame:s.frames11){D3D11_QUERY_DESC desc{D3D11_QUERY_TIMESTAMP_DISJOINT,0};
        auto hr=device->CreateQuery(&desc,&frame.disjoint);if(FAILED(hr))return hr;desc.Query=D3D11_QUERY_TIMESTAMP;
        for(auto& stamp:frame.stamps){hr=device->CreateQuery(&desc,&stamp);if(FAILED(hr))return hr;}}
    s.available11=true;return S_OK;
}
void PerformanceQueries::Collect11(ID3D11DeviceContext* context){
    auto& s=*state_;if(!s.Enabled()||!s.available11||!context)return;
    for(auto& frame:s.frames11)if(frame.pending){D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint{};
        const auto hr=context->GetData(frame.disjoint.Get(),&disjoint,sizeof(disjoint),D3D11_ASYNC_GETDATA_DONOTFLUSH);
        if(hr==S_FALSE)continue;if(FAILED(hr)){s.available11=false;return;}
        bool ready=true;const auto mask=frame.beginMask&frame.endMask;
        for(size_t phase=0;phase<phaseCount;++phase)if(mask&(1u<<phase)){
            uint64_t begin{},end{};
            const auto first=context->GetData(frame.stamps[phase*2].Get(),&begin,sizeof(begin),D3D11_ASYNC_GETDATA_DONOTFLUSH);
            const auto last=context->GetData(frame.stamps[phase*2+1].Get(),&end,sizeof(end),D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if(FAILED(first)||FAILED(last)){s.available11=false;return;}
            if(first==S_OK&&last==S_OK)s.metrics->RecordGpuFor(frame.source,GpuPhase(phase),begin,end,disjoint.Frequency,true,bool(disjoint.Disjoint));else ready=false;
        }if(ready)frame.pending=false;
    }
}
void PerformanceQueries::Collect12(){
    auto& s=*state_;if(!s.available12||!s.Enabled())return;
    for(size_t slot=0;slot<slotCount;++slot){auto& frame=s.frames12[slot];if(!frame.pending)continue;
        const auto done=frame.fence->GetCompletedValue();if(done==UINT64_MAX){s.available12=false;return;}if(done<frame.completion)continue;
        const SIZE_T offset=slot*queryStride*sizeof(uint64_t);D3D12_RANGE range{offset,offset+queryStride*sizeof(uint64_t)};
        void* data{};if(FAILED(s.readback->Map(0,&range,&data))){s.available12=false;return;}
        const auto* values=reinterpret_cast<const uint64_t*>(static_cast<const unsigned char*>(data)+offset);
        const auto mask=frame.beginMask&frame.endMask;
        for(size_t phase=0;phase<phaseCount;++phase)if(mask&(1u<<phase))
            s.metrics->RecordGpuFor(frame.source,GpuPhase(phase),values[phase*2],values[phase*2+1],s.frequency,true,false);
        D3D12_RANGE writes{0,0};s.readback->Unmap(0,&writes);frame.pending=false;frame.fence.Reset();
    }
}
void PerformanceQueries::Begin11(ID3D11DeviceContext* context,uint64_t source){
    auto& s=*state_;if(!s.Enabled()||!s.available11)return;Collect11(context);s.active11=-1;
    for(size_t slot=0;slot<slotCount;++slot){auto& frame=s.frames11[slot];if(frame.pending||frame.recording)continue;
        frame.source=source;frame.beginMask=frame.endMask=0;frame.recording=true;s.active11=int(slot);context->Begin(frame.disjoint.Get());return;}++s.dropped;
}
void PerformanceQueries::Stamp11(ID3D11DeviceContext* context,GpuPhase phase,bool begin){
    auto& s=*state_;if(s.active11<0||phase>=GpuPhase::Count)return;auto& frame=s.frames11[size_t(s.active11)];
    context->End(frame.stamps[size_t(phase)*2+(begin?0:1)].Get());(begin?frame.beginMask:frame.endMask)|=1u<<size_t(phase);
}
void PerformanceQueries::End11(ID3D11DeviceContext* context){
    auto& s=*state_;if(s.active11<0)return;auto& frame=s.frames11[size_t(s.active11)];
    context->End(frame.disjoint.Get());frame.pending=true;frame.recording=false;s.active11=-1;
}
void PerformanceQueries::Begin12(ID3D12GraphicsCommandList*,uint64_t source){
    auto& s=*state_;if(!s.available12||!s.Enabled())return;Collect12();s.active12=-1;
    for(size_t slot=0;slot<slotCount;++slot){auto& frame=s.frames12[slot];if(frame.pending||frame.recording)continue;
        frame.source=source;frame.beginMask=frame.endMask=0;frame.resolved=false;frame.recording=true;s.active12=int(slot);return;}++s.dropped;
}
void PerformanceQueries::Stamp12(ID3D12GraphicsCommandList* list,GpuPhase phase,bool begin){
    auto& s=*state_;if(s.active12<0||phase>=GpuPhase::Count)return;auto& frame=s.frames12[size_t(s.active12)];
    const UINT index=UINT(size_t(s.active12)*queryStride+size_t(phase)*2+(begin?0:1));
    list->EndQuery(s.heap.Get(),D3D12_QUERY_TYPE_TIMESTAMP,index);(begin?frame.beginMask:frame.endMask)|=1u<<size_t(phase);
}
void PerformanceQueries::Resolve12(ID3D12GraphicsCommandList* list){
    auto& s=*state_;if(s.active12<0)return;auto& frame=s.frames12[size_t(s.active12)];
    const auto mask=frame.beginMask&frame.endMask;
    for(size_t phase=0;phase<phaseCount;++phase)if(mask&(1u<<phase)){
        const UINT index=UINT(size_t(s.active12)*queryStride+phase*2);
        list->ResolveQueryData(s.heap.Get(),D3D12_QUERY_TYPE_TIMESTAMP,index,2,s.readback.Get(),index*sizeof(uint64_t));}
    frame.resolved=true;
}
void PerformanceQueries::Submitted12(ID3D12Fence* fence,uint64_t value){
    auto& s=*state_;if(s.active12<0)return;auto& frame=s.frames12[size_t(s.active12)];
    if(!frame.resolved||!fence||!value){++s.dropped;return;}
    frame.fence=fence;frame.completion=value;frame.pending=true;frame.recording=false;s.active12=-1;
}
uint64_t PerformanceQueries::Dropped()const{return state_->dropped;}
bool PerformanceQueries::Available11()const{return state_->available11;}
bool PerformanceQueries::Available12()const{return state_->available12;}
}
