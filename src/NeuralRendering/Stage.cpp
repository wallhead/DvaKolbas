#include "Stage.h"
#include "PerformanceQueries.h"
#include "History.h"
#include "RetirementEvent.h"
#include "RuntimeParameters.h"
#include "FrameGen/NeuralRenderingRuntimeContract.h"
#include <nvsdk_ngx_params.h>
#include <d3dcompiler.h>
#include <mutex>
#include <set>
#include <atomic>
#include <vector>

namespace TheosRenderPipeline::NeuralRendering {
namespace {
using Microsoft::WRL::ComPtr;
std::unexpected<Error> Fail(ErrorKind kind,const char* text,int64_t code=0){return std::unexpected(Error{kind,code,text});}
struct AllocationContext {
    ComPtr<ID3D12Device> device;
    std::set<ID3D12Resource*> resources;
    std::atomic_uint allocations{},releases{};
    std::atomic_bool failed{};
};
std::mutex allocationMutex;
AllocationContext* activeAllocation{};
void __cdecl Allocate(const D3D12_RESOURCE_DESC* desc,D3D12_RESOURCE_STATES state,const D3D12_HEAP_PROPERTIES* heap,ID3D12Resource** out)noexcept{
    if(!out)return;*out=nullptr;std::scoped_lock lock(allocationMutex);auto* c=activeAllocation;if(!c)return;
    if(!desc||!heap||FAILED(c->device->CreateCommittedResource(heap,D3D12_HEAP_FLAG_NONE,desc,state,nullptr,IID_PPV_ARGS(out)))){c->failed=true;return;}
    try{c->resources.insert(*out);++c->allocations;}catch(...){(*out)->Release();*out=nullptr;c->failed=true;}
}
void __cdecl ReleaseResource(ID3D12Resource* resource)noexcept{
    if(!resource)return;std::scoped_lock lock(allocationMutex);auto* c=activeAllocation;
    if(!c)return;if(!c->resources.erase(resource)){c->failed=true;return;}++c->releases;resource->Release();
}
uint32_t __cdecl Scaling(NVSDK_NGX_Parameter* p){
    if(!p)return 0xbad00005;int upscaling{};auto r=p->Get("DLSSNR.Upscaling",&upscaling);
    if(static_cast<uint32_t>(r)!=1)return static_cast<uint32_t>(r);p->Set("DLSSNR.ScalingRatio",1.f);return 1;
}
template<class T>bool OnDevice(T* object,ID3D12Device* d){
    ComPtr<ID3D12Device> actual;ComPtr<IUnknown> a,b;
    return object&&SUCCEEDED(object->GetDevice(IID_PPV_ARGS(&actual)))&&SUCCEEDED(actual.As(&a))&&
        SUCCEEDED(d->QueryInterface(IID_PPV_ARGS(&b)))&&a.Get()==b.Get();
}
void UavBarrier(ID3D12GraphicsCommandList* list,ID3D12Resource* resource){
    D3D12_RESOURCE_BARRIER b{};b.Type=D3D12_RESOURCE_BARRIER_TYPE_UAV;b.UAV.pResource=resource;list->ResourceBarrier(1,&b);
}
void Transition(ID3D12GraphicsCommandList* list,ID3D12Resource* resource,D3D12_RESOURCE_STATES from,D3D12_RESOURCE_STATES to){
    D3D12_RESOURCE_BARRIER b{};b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    b.Transition={resource,D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,from,to};list->ResourceBarrier(1,&b);
}
}
struct Stage::State {
    PerformanceMetrics* metrics{};std::unique_ptr<PerformanceQueries> timing;
    Detail::TicketOwnership ticketOwner;
    std::shared_ptr<RuntimeOwner> owner;
    StageContract contract;
    FenceDeviceIdentity fences;
    AllocationContext allocation;
    struct Feature {NVSDK_NGX_Parameter* parameters{};void* handle{};std::optional<int> recordedStyle;};
    std::array<Feature,3> features;
    unsigned passes{1};
    bool attempted{},ready{},client{},terminal{},sdrViewsQualified{};
    unsigned preset{};
    uint64_t serial{},recorded{},evaluatedPasses{},submissionValue{1};
    uint32_t create{},evaluate{},release{},destroy{};
    History history;
    ComPtr<ID3D12RootSignature> alphaRoot;
    ComPtr<ID3D12PipelineState> alphaPipeline;
    ComPtr<ID3D12CommandAllocator> creationAllocator;
    ComPtr<ID3D12GraphicsCommandList> creationList;
    ComPtr<ID3D12Fence> creationFence;
    struct Reader {ComPtr<ID3D12Fence> fence;uint64_t value{};};
    struct Pending {ImagePacket packet;ComPtr<ID3D12GraphicsCommandList> list;uint64_t id{};bool submitted{};std::vector<Reader> readers;};
    struct Pass {ComPtr<ID3D12DescriptorHeap> views;NVSDK_NGX_Parameter* parameters{};};
    struct Slot {std::unique_ptr<Pending> pending;std::array<Pass,3> passes;std::array<ComPtr<ID3D12Resource>,2> intermediate;};
    std::array<Slot,3> slots;
    RetirementEvent retirement;
    size_t nextSlot{};
    bool HasPending()const{for(const auto& slot:slots)if(slot.pending)return true;return false;}
    Slot* Find(const EvaluationTicket& ticket){if(!ticketOwner.Owns(ticket.owner_))return nullptr;for(auto& slot:slots)if(slot.pending&&slot.pending->id==ticket.id_)return &slot;return nullptr;}
    Result<bool> Complete(const Pending& pending){
        if(!pending.submitted)return false;
        for(const auto& reader:pending.readers){auto value=reader.fence->GetCompletedValue();if(value==UINT64_MAX){terminal=true;return Fail(ErrorKind::Retirement,"NR reader fence reports device removal");}if(value<reader.value)return false;}
        return true;
    }
    Result<void> Gpu(HRESULT code,const char* text){if(FAILED(code)){terminal=true;return Fail(ErrorKind::Runtime,text,code);}return {};}
    Result<void> Native(uint32_t code,const char* text){if(code!=1 || allocation.failed){terminal=true;return Fail(ErrorKind::Runtime,text,code);}return {};}
    Result<void> FailBeforeFeatureCreate(Result<void> failure){
        // Called only before entering vendor CreateFeature. No feature or
        // command recording can own these parameters/allocator callbacks yet.
        // Failed cleanup still retains the client and process allocator claim.
        {std::scoped_lock lock(allocationMutex);
            if(activeAllocation!=&allocation || !allocation.resources.empty() || allocation.failed){terminal=true;return Fail(ErrorKind::Retirement,"NR early initialization allocator ownership is uncertain");}}
        for(auto& feature:features)if(feature.parameters){
            destroy=owner->Exports().destroy(feature.parameters);
            auto destroyed=Native(destroy,"NR early initialization parameter destruction failed; ownership retained");
            if(!destroyed)return destroyed;feature.parameters=nullptr;
        }
        {std::scoped_lock lock(allocationMutex);
            if(activeAllocation!=&allocation || !allocation.resources.empty() || allocation.failed){terminal=true;return Fail(ErrorKind::Retirement,"NR early initialization callback cleanup is uncertain");}
            auto released=owner->ReleaseClientAfterRetirement();if(!released){terminal=true;return released;}
            client=false;activeAllocation=nullptr;
        }
        creationFence.Reset();creationList.Reset();creationAllocator.Reset();
        alphaPipeline.Reset();alphaRoot.Reset();allocation.device.Reset();
        contract={};timing.reset();owner.reset();
        return failure;
    }
    Result<void> BuildAlpha(){
        D3D12_FEATURE_DATA_FORMAT_SUPPORT format{DXGI_FORMAT_R16G16B16A16_FLOAT};
        auto r=Gpu(contract.device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT,&format,sizeof(format)),"NR alpha format query failed");if(!r)return r;
        constexpr auto needed=D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD|D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE;
        if((format.Support2&needed)!=needed){terminal=true;return Fail(ErrorKind::Unsupported,"NR FP16 alpha preservation lacks typed UAV support");}
        D3D12_DESCRIPTOR_RANGE ranges[2]{};ranges[0].RangeType=D3D12_DESCRIPTOR_RANGE_TYPE_SRV;ranges[1].RangeType=D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        for(auto& range:ranges){range.NumDescriptors=1;range.OffsetInDescriptorsFromTableStart=D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;}
        D3D12_ROOT_PARAMETER parameter{};parameter.ParameterType=D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;parameter.DescriptorTable={2,ranges};
        D3D12_ROOT_SIGNATURE_DESC root{};root.NumParameters=1;root.pParameters=&parameter;
        ComPtr<ID3DBlob> blob;r=Gpu(D3D12SerializeRootSignature(&root,D3D_ROOT_SIGNATURE_VERSION_1,&blob,nullptr),"NR alpha root serialization failed");if(!r)return r;
        r=Gpu(contract.device->CreateRootSignature(0,blob->GetBufferPointer(),blob->GetBufferSize(),IID_PPV_ARGS(&alphaRoot)),"NR alpha root creation failed");if(!r)return r;
        constexpr char program[]=R"(Texture2D<float4> original:register(t0);RWTexture2D<float4> enhanced:register(u0);
[numthreads(8,8,1)]void main(uint3 id:SV_DispatchThreadID){uint w,h;enhanced.GetDimensions(w,h);if(id.x>=w||id.y>=h)return;float4 p=enhanced[id.xy];p.a=original.Load(int3(id.xy,0)).a;enhanced[id.xy]=p;})";
        ComPtr<ID3DBlob> shader;r=Gpu(D3DCompile(program,sizeof(program)-1,"NrSourceAlpha",nullptr,nullptr,"main","cs_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&shader,nullptr),"NR alpha shader compilation failed");if(!r)return r;
        D3D12_COMPUTE_PIPELINE_STATE_DESC pipeline{};pipeline.pRootSignature=alphaRoot.Get();pipeline.CS={shader->GetBufferPointer(),shader->GetBufferSize()};
        return Gpu(contract.device->CreateComputePipelineState(&pipeline,IID_PPV_ARGS(&alphaPipeline)),"NR alpha pipeline creation failed");
    }
};
Stage::Stage():state_(std::make_unique<State>()){}
Stage::~Stage(){
    // Never call vendor cleanup, wait queues or release uncertain recordings.
    bool owned=state_->client||state_->HasPending();for(const auto& feature:state_->features)owned|=feature.handle||feature.parameters;
    if(owned)state_.release();
}
Result<void> Stage::Initialize(std::shared_ptr<RuntimeOwner> owner,const StageContract& c,unsigned preset,PerformanceMetrics* metrics,unsigned passes){
    auto& s=*state_;if(s.attempted)return Fail(ErrorKind::Conflict,"NR stage initialization already attempted");s.attempted=true;
    if(!owner||!owner->Ready()||!c.device||!c.queue||!c.adapterLuid.Valid()||preset>1||passes<1||passes>3||
        c.colorExtent!=c.guideExtent||!c.colorExtent.width||!c.colorExtent.height||c.colorExtent.width>16384||c.colorExtent.height>16384)
        return Fail(ErrorKind::InvalidInput,"NR stage runtime/native extent/device contract invalid");
    auto r=owner->CheckClientDevice(c.device.Get());if(!r)return r;const auto luid=c.device->GetAdapterLuid();
    if(luid.LowPart!=c.adapterLuid.low||luid.HighPart!=c.adapterLuid.high||!OnDevice(c.queue.Get(),c.device.Get())||c.queue->GetDesc().Type!=D3D12_COMMAND_LIST_TYPE_DIRECT)
        return Fail(ErrorKind::IdentityMismatch,"NR stage queue/device identity invalid");
    r=s.fences.Initialize(c.device.Get());if(!r)return r;
    s.metrics=metrics;
    if(metrics&&metrics->Enabled()){s.timing=std::make_unique<PerformanceQueries>(metrics);s.timing->Initialize12(c.device.Get(),c.queue.Get());}
    r=owner->AcquireClient();if(!r)return r;s.client=true;s.owner=std::move(owner);s.contract=c;s.preset=preset;s.passes=passes;s.allocation.device=c.device;
    {std::scoped_lock lock(allocationMutex);if(activeAllocation){r=s.owner->ReleaseClientAfterRetirement();if(!r){s.terminal=true;return r;}s.client=false;return Fail(ErrorKind::Conflict,"Another NR shared stage owns allocator callbacks");}activeAllocation=&s.allocation;}
    r=s.BuildAlpha();if(!r)return s.FailBeforeFeatureCreate(r);const auto& e=s.owner->Exports();
    // Allocate all creation parameters before any vendor creation. After the
    // first CreateFeature call, every partial failure quarantines all owners.
    for(unsigned i=0;i<passes;++i){auto& feature=s.features[i];r=s.Native(e.allocate(&feature.parameters),"NR shared parameters allocation failed");if(!r)return s.FailBeforeFeatureCreate(r);
    if(!feature.parameters){s.terminal=true;return s.FailBeforeFeatureCreate(Fail(ErrorKind::Runtime,"NR allocator returned no parameters"));}
    auto& p=*feature.parameters;r=WriteDirectCreationParameters(p,{s.owner->ProfileId(),c.colorExtent,c.guideExtent,preset});if(!r){s.terminal=true;return s.FailBeforeFeatureCreate(r);}
    int roundtrip=-1;r=s.Native(static_cast<uint32_t>(p.Get("DLSSNR.Upscaling",&roundtrip)),"NR direct signed parameter ABI read failed");if(!r)return s.FailBeforeFeatureCreate(r);
    if(roundtrip!=0){s.terminal=true;return s.FailBeforeFeatureCreate(Fail(ErrorKind::Runtime,"NR direct signed parameter ABI differs"));}
    p.Set("ResourceAllocCallback",reinterpret_cast<void*>(&Allocate));p.Set("ResourceReleaseCallback",reinterpret_cast<void*>(&ReleaseResource));p.Set("DLSSNRComputeScalingRatioCallback",reinterpret_cast<void*>(&Scaling));
    }
    r=s.Gpu(c.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&s.creationAllocator)),"NR creation allocator failed");if(!r)return s.FailBeforeFeatureCreate(r);
    r=s.Gpu(c.device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,s.creationAllocator.Get(),nullptr,IID_PPV_ARGS(&s.creationList)),"NR creation list failed");if(!r)return s.FailBeforeFeatureCreate(r);
    r=s.Gpu(c.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&s.creationFence)),"NR creation fence failed");if(!r)return s.FailBeforeFeatureCreate(r);
    for(unsigned i=0;i<passes;++i){auto& feature=s.features[i];s.create=e.create(s.creationList.Get(),kDirectNrFeatureId,feature.parameters,&feature.handle);r=s.Native(s.create,"NR shared feature creation failed");if(!r)return r;
    if(!feature.handle){s.terminal=true;return Fail(ErrorKind::Runtime,"NR create returned no feature");}}
    r=s.Gpu(s.creationList->Close(),"NR creation list close failed");if(!r)return r;
    ID3D12CommandList* lists[]={s.creationList.Get()};c.queue->ExecuteCommandLists(1,lists);r=s.Gpu(c.queue->Signal(s.creationFence.Get(),1),"NR creation signal failed");if(!r)return r;
    const HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!event){s.terminal=true;return Fail(ErrorKind::Runtime,"NR creation completion event failed",GetLastError());}
    r=s.Gpu(s.creationFence->SetEventOnCompletion(1,event),"NR creation completion arm failed");
    const bool completed=r&&WaitForSingleObject(event,15000)==WAIT_OBJECT_0&&s.creationFence->GetCompletedValue()==1;CloseHandle(event);if(!r)return r;
    if(!completed){s.terminal=true;return Fail(ErrorKind::Retirement,"NR feature creation completion unconfirmed");}
    r=s.Gpu(c.device->GetDeviceRemovedReason(),"NR device removed during creation");if(!r)return r;s.ready=true;return {};
}
Result<EvaluationTicket> Stage::Record(ID3D12GraphicsCommandList* list,const ImagePacket& packet,const SettingsSnapshot& settings){
    return RecordInternal(list,packet,settings,false);
}
Result<EvaluationTicket> Stage::RecordQueued(ID3D12GraphicsCommandList* list,const ImagePacket& packet,const SettingsSnapshot& settings){
    return RecordInternal(list,packet,settings,true);
}
Result<EvaluationTicket> Stage::RecordInternal(ID3D12GraphicsCommandList* list,const ImagePacket& packet,const SettingsSnapshot& settings,bool queued){
    PerformanceScope performance(state_->metrics,CpuPhase::Record);
    auto& s=*state_;if(!s.ready||s.terminal)return Fail(ErrorKind::Runtime,"NR stage unavailable/terminal");
    auto healthy=s.Gpu(s.contract.device->GetDeviceRemovedReason(),"NR device removed during admission");if(!healthy)return std::unexpected(healthy.error());
    State::Slot* available{};
    for(size_t i=0;i<s.slots.size();++i){auto& slot=s.slots[(s.nextSlot+i)%s.slots.size()];if(!slot.pending){if(!available)available=&slot;continue;}if(!slot.pending->submitted)return Fail(ErrorKind::Retirement,"NR prior recording has not been submitted");}
    if(!available)return Fail(ErrorKind::Retirement,"NR three image slots remain occupied; backpressure");
    if(settings.passes!=int(s.passes))return Fail(ErrorKind::InvalidInput,"NR pass count differs from initialized chain");
    if(settings.reconstruction.preset!=s.preset || settings.reconstruction.inputScale!=1 || settings.reconstruction.peripheralCompression || settings.reconstruction.fusedPreparation || settings.reconstruction.producerColor || settings.reconstruction.colorIsHDR ||
        settings.reconstruction.method>ResolveMethod::Ratio || EffectiveResolve(settings.reconstruction)!=ResolveMethod::Auto)
        return Fail(ErrorKind::Unsupported,"NR shared native stage requires adapter-owned reconstruction before/after it");
    auto validated=queued?QueuedImageAdmission::Validate(list,packet,s.contract,s.fences):ValidateImagePacket(list,packet,s.contract,&s.fences);
    if(!validated)return std::unexpected(validated.error());
    if(packet.colorDomain==ColorDomain::SdrBytes&&!s.sdrViewsQualified){
        D3D12_FEATURE_DATA_FORMAT_SUPPORT format{DXGI_FORMAT_R8G8B8A8_UNORM};
        const auto code=s.contract.device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT,&format,sizeof(format));
        constexpr auto needed=D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD|D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE;
        if(FAILED(code)||(format.Support2&needed)!=needed)return Fail(ErrorKind::Unsupported,"NR SDR alpha preservation lacks typed UAV support",code);
        s.sdrViewsQualified=true;
    }
    auto history=s.history.Check(packet,settings);if(!history)return std::unexpected(history.error());
    for(unsigned i=0;i<s.passes;++i)if(s.features[i].recordedStyle&&*s.features[i].recordedStyle!=SanitizeBuild14Tuning(PassTuning(settings,i)).style&&s.HasPending())
        return Fail(ErrorKind::Retirement,"NR style change requires retirement of every chain reader");
    for(const auto& slot:s.slots)if(slot.pending){
        const auto& held=slot.pending->packet;
        for(auto* incoming:{packet.color.Get(),packet.output.Get(),packet.depth.Get(),packet.motion.Get()})
            for(auto* occupied:{held.color.Get(),held.output.Get(),held.depth.Get(),held.motion.Get()}){
                ComPtr<IUnknown> a,b;if(SUCCEEDED(incoming->QueryInterface(IID_PPV_ARGS(&a)))&&SUCCEEDED(occupied->QueryInterface(IID_PPV_ARGS(&b)))&&a==b)return Fail(ErrorKind::Retirement,"NR image resource aliases an occupied slot");
            }
    }
    if(s.serial==UINT64_MAX)return Fail(ErrorKind::Runtime,"NR ticket sequence exhausted");
    auto& slot=*available;
    s.nextSlot=(size_t(available-s.slots.data())+1)%s.slots.size();
    // Retain the whole packet before touching the queue, including when a
    // subsequent descriptor allocation or vendor recording fails terminally.
    slot.pending=std::make_unique<State::Pending>();slot.pending->packet=packet;slot.pending->list=list;slot.pending->id=++s.serial;
    // Validate every input/history identity first. A failed Wait records no NR
    // commands, and only this retained queue may admit the exact dependency.
    if(queued&&packet.producerFence){
        auto wait=s.Gpu(s.contract.queue->Wait(packet.producerFence.Get(),packet.producerFenceValue),"NR producer queue wait failed; no evaluation recorded");
        if(!wait)return std::unexpected(wait.error());
    }
    if(s.timing)s.timing->Begin12(list,packet.sourceId);
    // Intermediates remain private to this retained image slot. They are never
    // reused until all vendor, bridge and downstream readers of its ticket retire.
    for(unsigned i=0;i+1<s.passes;++i){auto& intermediate=slot.intermediate[i];
        const auto desc=packet.output->GetDesc();
        if(intermediate&&intermediate->GetDesc().Format!=desc.Format)intermediate.Reset();
        if(!intermediate){D3D12_HEAP_PROPERTIES properties{};properties.Type=D3D12_HEAP_TYPE_DEFAULT;
            auto allocated=s.Gpu(s.contract.device->CreateCommittedResource(&properties,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_UNORDERED_ACCESS,nullptr,IID_PPV_ARGS(&intermediate)),"NR pass intermediate allocation failed; recording ownership retained");
            if(!allocated)return std::unexpected(allocated.error());}
    }
    D3D12_DESCRIPTOR_HEAP_DESC heap{};heap.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;heap.NumDescriptors=2;heap.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    Result<void> r;
    if(s.timing)s.timing->Stamp12(list,GpuPhase::Vendor,true);
    for(unsigned i=0;i<s.passes;++i){auto& pass=slot.passes[i];auto& feature=s.features[i];
    auto* color=i?slot.intermediate[i-1].Get():packet.color.Get();
    auto* output=i+1==s.passes?packet.output.Get():slot.intermediate[i].Get();
    if(!pass.views){r=s.Gpu(s.contract.device->CreateDescriptorHeap(&heap,IID_PPV_ARGS(&pass.views)),"NR retained slot descriptor creation failed");if(!r)return std::unexpected(r.error());if(s.metrics)s.metrics->RecordDescriptorCreation();}
    if(!pass.parameters){r=s.Native(s.owner->Exports().allocate(&pass.parameters),"NR retained slot parameter allocation failed");if(!r)return std::unexpected(r.error());if(!pass.parameters){s.terminal=true;return Fail(ErrorKind::Runtime,"NR slot allocator returned null parameters");}
        r=WriteDirectCreationParameters(*pass.parameters,{s.owner->ProfileId(),s.contract.colorExtent,s.contract.guideExtent,s.preset});if(!r){s.terminal=true;return std::unexpected(r.error());}
        pass.parameters->Set("ResourceAllocCallback",reinterpret_cast<void*>(&Allocate));pass.parameters->Set("ResourceReleaseCallback",reinterpret_cast<void*>(&ReleaseResource));pass.parameters->Set("DLSSNRComputeScalingRatioCallback",reinterpret_cast<void*>(&Scaling));}
    auto cpu=pass.views->GetCPUDescriptorHandleForHeapStart();D3D12_SHADER_RESOURCE_VIEW_DESC srv{};srv.Format=NrColorFormat(packet.colorDomain);srv.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D;srv.Shader4ComponentMapping=D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;srv.Texture2D.MipLevels=1;
    s.contract.device->CreateShaderResourceView(color,&srv,cpu);cpu.ptr+=s.contract.device->GetDescriptorHandleIncrementSize(heap.Type);
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav{};uav.Format=srv.Format;uav.ViewDimension=D3D12_UAV_DIMENSION_TEXTURE2D;s.contract.device->CreateUnorderedAccessView(output,nullptr,&uav,cpu);
    auto& p=*pass.parameters;p.Set("DLSSNR.Color",color);p.Set("DLSSNR.MVec",packet.motion.Get());p.Set("DLSSNR.Depth",packet.depth.Get());p.Set("DLSSNR.Output",output);
    p.Set("DLSSNR.UI",static_cast<ID3D12Resource*>(nullptr));p.Set("DLSSNR.UIAlpha",static_cast<ID3D12Resource*>(nullptr));
    for(const char* plane:{"Color","MVec","Depth","Output"}){const auto prefix=std::string("DLSSNR.")+plane+"Subrect";p.Set((prefix+"BaseX").c_str(),0u);p.Set((prefix+"BaseY").c_str(),0u);p.Set((prefix+"Width").c_str(),packet.colorExtent.width);p.Set((prefix+"Height").c_str(),packet.colorExtent.height);}
    p.Set("DLSSNR.MVecScaleX",packet.motionScaleX);p.Set("DLSSNR.MVecScaleY",packet.motionScaleY);auto tuning=SanitizeBuild14Tuning(PassTuning(settings,i));tuning.uiCorrection=false;
    const bool reset=history->Reset()||!feature.recordedStyle||*feature.recordedStyle!=tuning.style;
    WriteTuningParameters(p,tuning,reset,packet.depthInverted,RuntimeBuild::Build14);
    s.evaluate=s.owner->Exports().evaluate(list,feature.handle,pass.parameters,nullptr);r=s.Native(s.evaluate,"NR shared evaluation failed; recording ownership retained");if(!r)return std::unexpected(r.error());
    ++s.evaluatedPasses;feature.recordedStyle=tuning.style;
    // The existing two timestamps cover the whole chain through its final
    // vendor evaluation; Alpha isolates the final output preservation dispatch.
    if(s.timing&&i+1==s.passes){s.timing->Stamp12(list,GpuPhase::Vendor,false);s.timing->Stamp12(list,GpuPhase::Alpha,true);}
    UavBarrier(list,output);ID3D12DescriptorHeap* heaps[]={pass.views.Get()};list->SetDescriptorHeaps(1,heaps);list->SetComputeRootSignature(s.alphaRoot.Get());list->SetPipelineState(s.alphaPipeline.Get());
    list->SetComputeRootDescriptorTable(0,pass.views->GetGPUDescriptorHandleForHeapStart());list->Dispatch((packet.colorExtent.width+7)/8,(packet.colorExtent.height+7)/8,1);UavBarrier(list,output);
    if(i+1<s.passes)Transition(list,output,D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    }
    for(unsigned i=0;i+1<s.passes;++i)Transition(list,slot.intermediate[i].Get(),D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    if(s.timing){s.timing->Stamp12(list,GpuPhase::Alpha,false);s.timing->Resolve12(list);}
    r=s.history.CommitRecorded(*history);if(!r){s.terminal=true;return std::unexpected(r.error());}++s.recorded;EvaluationTicket ticket;ticket.owner_=s.ticketOwner.Seal();ticket.id_=slot.pending->id;ticket.image_=packet.imageId;ticket.output_=packet.output;return ticket;
}
Result<void> Stage::MarkSubmitted(const EvaluationTicket& ticket,ID3D12Fence* fence,uint64_t value){
    auto& s=*state_;auto* slot=s.Find(ticket);if(s.terminal||!slot||slot->pending->submitted||!value)return Fail(ErrorKind::InvalidInput,"NR submission ticket/fence invalid");
    auto identity=s.fences.Validate(fence,s.contract.device.Get());if(!identity)return identity;
    if(fence->GetCompletedValue()>=value || s.submissionValue==UINT64_MAX)
        return Fail(ErrorKind::InvalidInput,"NR submission value already completed/exhausted");
    // An external fence can be signalled by another owner. Our private fence
    // always also covers this stage's actual queue order and is never exported.
    slot->pending->readers.push_back({s.creationFence,++s.submissionValue});slot->pending->readers.push_back({fence,value});
    auto r=s.Gpu(s.contract.queue->Signal(s.creationFence.Get(),s.submissionValue),"NR private completion signal failed; ownership retained");if(!r)return r;
    r=s.Gpu(s.contract.queue->Signal(fence,value),"NR recording completion signal failed; ownership retained");if(!r)return r;slot->pending->submitted=true;
    if(s.timing)s.timing->Submitted12(s.creationFence.Get(),s.submissionValue);if(s.metrics)s.metrics->RecordSubmitted();return {};
}
Result<void> Stage::TrackReader(const EvaluationTicket& ticket,ID3D12Fence* fence,uint64_t value){
    auto& s=*state_;auto* slot=s.Find(ticket);if(s.terminal||!slot||!slot->pending->submitted||!value)return Fail(ErrorKind::InvalidInput,"NR output reader ticket/fence invalid");
    auto identity=s.fences.Validate(fence,s.contract.device.Get());if(!identity)return identity;slot->pending->readers.push_back({fence,value});return {};
}
Result<void> Stage::RetireTicket(const EvaluationTicket& ticket){
    auto& s=*state_;auto* slot=s.Find(ticket);if(s.terminal||!slot||!slot->pending->submitted)return Fail(ErrorKind::Retirement,"NR ticket unsubmitted/foreign/terminal");auto r=s.Gpu(s.contract.device->GetDeviceRemovedReason(),"NR device removed during ticket retirement");if(!r)return r;
    auto completed=s.Complete(*slot->pending);if(!completed)return std::unexpected(completed.error());if(!*completed)return Fail(ErrorKind::Retirement,"NR output readers remain pending");if(s.metrics)s.metrics->RecordCompletedFor(slot->pending->packet.sourceId);slot->pending.reset();return {};
}
Result<void> Stage::WaitForRetirement(const EvaluationTicket& ticket,uint32_t timeoutMilliseconds){
    auto& s=*state_;auto* slot=s.Find(ticket);if(s.terminal||!slot||!slot->pending->submitted||!timeoutMilliseconds)return Fail(ErrorKind::Retirement,"NR retirement wait ticket/deadline invalid");
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::milliseconds(timeoutMilliseconds);
    for(const auto& reader:slot->pending->readers){auto waited=s.retirement.Wait(reader.fence.Get(),reader.value,s.contract.device.Get(),deadline);if(!waited){s.terminal=true;return waited;}}
    return {};
}
Result<void> Stage::WaitForProgress(uint32_t timeoutMilliseconds){
    auto& s=*state_;if(s.terminal||!s.ready||!timeoutMilliseconds)return Fail(ErrorKind::Retirement,"NR capacity wait state/deadline invalid");
    std::vector<RetirementEvent::Dependency> readers;
    for(const auto& slot:s.slots)if(slot.pending&&slot.pending->submitted)for(const auto& reader:slot.pending->readers)readers.push_back({reader.fence.Get(),reader.value});
    auto result=s.retirement.WaitAny(readers,s.contract.device.Get(),std::chrono::steady_clock::now()+std::chrono::milliseconds(timeoutMilliseconds));if(!result)s.terminal=true;return result;
}
Result<uint32_t> Stage::CollectCompleted(){
    auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,"NR stage terminal; ownership retained");if(!s.ready)return uint32_t{0};auto r=s.Gpu(s.contract.device->GetDeviceRemovedReason(),"NR device removed during collection");if(!r)return std::unexpected(r.error());uint32_t count{};
    for(auto& slot:s.slots)if(slot.pending){auto complete=s.Complete(*slot.pending);if(!complete)return std::unexpected(complete.error());if(*complete){if(s.metrics)s.metrics->RecordCompletedFor(slot.pending->packet.sourceId);slot.pending.reset();++count;}}return count;
}
Result<void> Stage::Retire(){
    auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,"NR stage terminal; no teardown retry");if(s.HasPending())return Fail(ErrorKind::Retirement,"NR recorded output/reader owners remain pending");if(!s.client)return {};
    if(s.timing)s.timing->Collect12();
    Result<void> r;
    for(auto& feature:s.features)if(feature.handle){s.release=s.owner->Exports().release(feature.handle);r=s.Native(s.release,"NR feature release failed; ownership retained");if(!r)return r;feature.handle=nullptr;}
    for(auto& slot:s.slots){for(auto& pass:slot.passes)if(pass.parameters){s.destroy=s.owner->Exports().destroy(pass.parameters);r=s.Native(s.destroy,"NR slot parameter destruction failed; ownership retained");if(!r)return r;pass.parameters=nullptr;pass.views.Reset();}for(auto& intermediate:slot.intermediate)intermediate.Reset();}
    for(auto& feature:s.features)if(feature.parameters){s.destroy=s.owner->Exports().destroy(feature.parameters);r=s.Native(s.destroy,"NR parameter destruction failed; ownership retained");if(!r)return r;feature.parameters=nullptr;}
    {std::scoped_lock lock(allocationMutex);if(activeAllocation!=&s.allocation || !s.allocation.resources.empty() || s.allocation.failed){s.terminal=true;return Fail(ErrorKind::Retirement,"NR callback allocation ownership unbalanced");}activeAllocation=nullptr;}
    r=s.owner->ReleaseClientAfterRetirement();if(!r){s.terminal=true;return r;}s.client=false;s.ready=false;return {};
}
StageDiagnostics Stage::Diagnostics()const{const auto& s=*state_;StageDiagnostics d{s.create,s.evaluate,s.release,s.destroy,s.allocation.allocations,s.allocation.releases,s.recorded,s.terminal};
    d.activePasses=s.passes;d.evaluatedPasses=s.evaluatedPasses;
    d.slotCount=uint32_t(s.slots.size());for(const auto& slot:s.slots){for(const auto& pass:slot.passes){d.descriptorOwners+=bool(pass.views);d.parameterOwners+=bool(pass.parameters);}d.pendingTickets+=bool(slot.pending);}
    if(s.timing){d.gpuTiming12Available=s.timing->Available12();d.gpuTimingDropped=s.timing->Dropped();}return d;}
}
