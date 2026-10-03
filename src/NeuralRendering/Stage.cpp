#include "Stage.h"
#include "History.h"
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
}
struct Stage::State {
    Detail::TicketOwnership ticketOwner;
    std::shared_ptr<RuntimeOwner> owner;
    StageContract contract;
    FenceDeviceIdentity fences;
    AllocationContext allocation;
    NVSDK_NGX_Parameter* parameters{};
    void* feature{};
    bool attempted{},ready{},client{},terminal{};
    unsigned preset{};
    uint64_t serial{},recorded{},submissionValue{1};
    uint32_t create{},evaluate{},release{},destroy{};
    History history;
    ComPtr<ID3D12RootSignature> alphaRoot;
    ComPtr<ID3D12PipelineState> alphaPipeline;
    ComPtr<ID3D12CommandAllocator> creationAllocator;
    ComPtr<ID3D12GraphicsCommandList> creationList;
    ComPtr<ID3D12Fence> creationFence;
    struct Reader {ComPtr<ID3D12Fence> fence;uint64_t value{};};
    struct Pending {ImagePacket packet;ComPtr<ID3D12DescriptorHeap> views;uint64_t id{};bool submitted{};std::vector<Reader> readers;};
    std::unique_ptr<Pending> pending;
    Result<void> Gpu(HRESULT code,const char* text){if(FAILED(code)){terminal=true;return Fail(ErrorKind::Runtime,text,code);}return {};}
    Result<void> Native(uint32_t code,const char* text){if(code!=1 || allocation.failed){terminal=true;return Fail(ErrorKind::Runtime,text,code);}return {};}
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
    bool Owns(const EvaluationTicket& ticket)const{return ticketOwner.Owns(ticket.owner_)&&pending&&pending->id==ticket.id_;}
};
Stage::Stage():state_(std::make_unique<State>()){}
Stage::~Stage(){
    // Never call vendor cleanup, wait queues or release uncertain recordings.
    if(state_->client || state_->feature || state_->parameters || state_->pending)state_.release();
}
Result<void> Stage::Initialize(std::shared_ptr<RuntimeOwner> owner,const StageContract& c,unsigned preset){
    auto& s=*state_;if(s.attempted)return Fail(ErrorKind::Conflict,"NR stage initialization already attempted");s.attempted=true;
    if(!owner||!owner->Ready()||!c.device||!c.queue||!c.adapterLuid.Valid()||preset>1||
        c.colorExtent!=c.guideExtent||!c.colorExtent.width||!c.colorExtent.height||c.colorExtent.width>16384||c.colorExtent.height>16384)
        return Fail(ErrorKind::InvalidInput,"NR stage runtime/native extent/device contract invalid");
    auto r=owner->CheckClientDevice(c.device.Get());if(!r)return r;const auto luid=c.device->GetAdapterLuid();
    if(luid.LowPart!=c.adapterLuid.low||luid.HighPart!=c.adapterLuid.high||!OnDevice(c.queue.Get(),c.device.Get())||c.queue->GetDesc().Type!=D3D12_COMMAND_LIST_TYPE_DIRECT)
        return Fail(ErrorKind::IdentityMismatch,"NR stage queue/device identity invalid");
    r=s.fences.Initialize(c.device.Get());if(!r)return r;
    r=owner->AcquireClient();if(!r)return r;s.client=true;s.owner=std::move(owner);s.contract=c;s.preset=preset;s.allocation.device=c.device;
    {std::scoped_lock lock(allocationMutex);if(activeAllocation){r=s.owner->ReleaseClientAfterRetirement();if(!r){s.terminal=true;return r;}s.client=false;return Fail(ErrorKind::Conflict,"Another NR shared stage owns allocator callbacks");}activeAllocation=&s.allocation;}
    r=s.BuildAlpha();if(!r)return r;const auto& e=s.owner->Exports();r=s.Native(e.allocate(&s.parameters),"NR shared parameters allocation failed");if(!r)return r;
    if(!s.parameters){s.terminal=true;return Fail(ErrorKind::Runtime,"NR allocator returned no parameters");}
    auto& p=*s.parameters;r=WriteDirectCreationParameters(p,{s.owner->ProfileId(),c.colorExtent,c.guideExtent,preset});if(!r){s.terminal=true;return r;}
    int roundtrip=-1;r=s.Native(static_cast<uint32_t>(p.Get("DLSSNR.Upscaling",&roundtrip)),"NR direct signed parameter ABI read failed");if(!r)return r;
    if(roundtrip!=0){s.terminal=true;return Fail(ErrorKind::Runtime,"NR direct signed parameter ABI differs");}
    p.Set("ResourceAllocCallback",reinterpret_cast<void*>(&Allocate));p.Set("ResourceReleaseCallback",reinterpret_cast<void*>(&ReleaseResource));p.Set("DLSSNRComputeScalingRatioCallback",reinterpret_cast<void*>(&Scaling));
    r=s.Gpu(c.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&s.creationAllocator)),"NR creation allocator failed");if(!r)return r;
    r=s.Gpu(c.device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,s.creationAllocator.Get(),nullptr,IID_PPV_ARGS(&s.creationList)),"NR creation list failed");if(!r)return r;
    r=s.Gpu(c.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&s.creationFence)),"NR creation fence failed");if(!r)return r;
    s.create=e.create(s.creationList.Get(),kDirectNrFeatureId,s.parameters,&s.feature);r=s.Native(s.create,"NR shared feature creation failed");if(!r)return r;
    if(!s.feature){s.terminal=true;return Fail(ErrorKind::Runtime,"NR create returned no feature");}
    r=s.Gpu(s.creationList->Close(),"NR creation list close failed");if(!r)return r;
    ID3D12CommandList* lists[]={s.creationList.Get()};c.queue->ExecuteCommandLists(1,lists);r=s.Gpu(c.queue->Signal(s.creationFence.Get(),1),"NR creation signal failed");if(!r)return r;
    const HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!event){s.terminal=true;return Fail(ErrorKind::Runtime,"NR creation completion event failed",GetLastError());}
    r=s.Gpu(s.creationFence->SetEventOnCompletion(1,event),"NR creation completion arm failed");
    const bool completed=r&&WaitForSingleObject(event,15000)==WAIT_OBJECT_0&&s.creationFence->GetCompletedValue()==1;CloseHandle(event);if(!r)return r;
    if(!completed){s.terminal=true;return Fail(ErrorKind::Retirement,"NR feature creation completion unconfirmed");}
    r=s.Gpu(c.device->GetDeviceRemovedReason(),"NR device removed during creation");if(!r)return r;s.ready=true;return {};
}
Result<EvaluationTicket> Stage::Record(ID3D12GraphicsCommandList* list,const ImagePacket& packet,const SettingsSnapshot& settings){
    auto& s=*state_;if(!s.ready||s.terminal)return Fail(ErrorKind::Runtime,"NR stage unavailable/terminal");if(s.pending)return Fail(ErrorKind::Retirement,"NR prior recording/readers have not retired");
    if(settings.reconstruction.preset!=s.preset || settings.reconstruction.inputScale!=1 || settings.reconstruction.peripheralCompression || settings.reconstruction.fusedPreparation || settings.reconstruction.producerColor || settings.reconstruction.colorIsHDR ||
        settings.reconstruction.method>ResolveMethod::Ratio || EffectiveResolve(settings.reconstruction)!=ResolveMethod::Auto)
        return Fail(ErrorKind::Unsupported,"NR shared native stage requires adapter-owned reconstruction before/after it");
    auto validated=ValidateImagePacket(list,packet,s.contract,&s.fences);if(!validated)return std::unexpected(validated.error());auto history=s.history.Check(packet,settings);if(!history)return std::unexpected(history.error());
    if(s.serial==UINT64_MAX)return Fail(ErrorKind::Runtime,"NR ticket sequence exhausted");
    auto pending=std::make_unique<State::Pending>();pending->packet=packet;pending->id=++s.serial;
    D3D12_DESCRIPTOR_HEAP_DESC heap{};heap.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;heap.NumDescriptors=2;heap.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    auto r=s.Gpu(s.contract.device->CreateDescriptorHeap(&heap,IID_PPV_ARGS(&pending->views)),"NR per-record descriptor owner creation failed");if(!r)return std::unexpected(r.error());
    auto cpu=pending->views->GetCPUDescriptorHandleForHeapStart();D3D12_SHADER_RESOURCE_VIEW_DESC srv{};srv.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;srv.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D;srv.Shader4ComponentMapping=D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;srv.Texture2D.MipLevels=1;
    s.contract.device->CreateShaderResourceView(packet.color.Get(),&srv,cpu);cpu.ptr+=s.contract.device->GetDescriptorHandleIncrementSize(heap.Type);
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav{};uav.Format=srv.Format;uav.ViewDimension=D3D12_UAV_DIMENSION_TEXTURE2D;s.contract.device->CreateUnorderedAccessView(packet.output.Get(),nullptr,&uav,cpu);
    s.pending=std::move(pending);auto& p=*s.parameters;p.Set("DLSSNR.Color",packet.color.Get());p.Set("DLSSNR.MVec",packet.motion.Get());p.Set("DLSSNR.Depth",packet.depth.Get());p.Set("DLSSNR.Output",packet.output.Get());
    p.Set("DLSSNR.UI",static_cast<ID3D12Resource*>(nullptr));p.Set("DLSSNR.UIAlpha",static_cast<ID3D12Resource*>(nullptr));
    for(const char* plane:{"Color","MVec","Depth","Output"}){const auto prefix=std::string("DLSSNR.")+plane+"Subrect";p.Set((prefix+"BaseX").c_str(),0u);p.Set((prefix+"BaseY").c_str(),0u);p.Set((prefix+"Width").c_str(),packet.colorExtent.width);p.Set((prefix+"Height").c_str(),packet.colorExtent.height);}
    p.Set("DLSSNR.MVecScaleX",packet.motionScaleX);p.Set("DLSSNR.MVecScaleY",packet.motionScaleY);auto tuning=settings.tuning;tuning.uiCorrection=false;WriteTuningParameters(p,tuning,history->Reset(),packet.depthInverted,RuntimeBuild::Build14);
    s.evaluate=s.owner->Exports().evaluate(list,s.feature,s.parameters,nullptr);r=s.Native(s.evaluate,"NR shared evaluation failed; recording ownership retained");if(!r)return std::unexpected(r.error());
    UavBarrier(list,packet.output.Get());ID3D12DescriptorHeap* heaps[]={s.pending->views.Get()};list->SetDescriptorHeaps(1,heaps);list->SetComputeRootSignature(s.alphaRoot.Get());list->SetPipelineState(s.alphaPipeline.Get());
    list->SetComputeRootDescriptorTable(0,s.pending->views->GetGPUDescriptorHandleForHeapStart());list->Dispatch((packet.colorExtent.width+7)/8,(packet.colorExtent.height+7)/8,1);UavBarrier(list,packet.output.Get());
    r=s.history.CommitRecorded(*history);if(!r){s.terminal=true;return std::unexpected(r.error());}++s.recorded;EvaluationTicket ticket;ticket.owner_=s.ticketOwner.Seal();ticket.id_=s.pending->id;ticket.image_=packet.imageId;ticket.output_=packet.output;return ticket;
}
Result<void> Stage::MarkSubmitted(const EvaluationTicket& ticket,ID3D12Fence* fence,uint64_t value){
    auto& s=*state_;if(s.terminal||!s.Owns(ticket)||s.pending->submitted||!value)return Fail(ErrorKind::InvalidInput,"NR submission ticket/fence invalid");
    auto identity=s.fences.Validate(fence,s.contract.device.Get());if(!identity)return identity;
    if(fence->GetCompletedValue()>=value || s.submissionValue==UINT64_MAX)
        return Fail(ErrorKind::InvalidInput,"NR submission value already completed/exhausted");
    // An external fence can be signalled by another owner. Our private fence
    // always also covers this stage's actual queue order and is never exported.
    s.pending->readers.push_back({s.creationFence,++s.submissionValue});s.pending->readers.push_back({fence,value});
    auto r=s.Gpu(s.contract.queue->Signal(s.creationFence.Get(),s.submissionValue),"NR private completion signal failed; ownership retained");if(!r)return r;
    r=s.Gpu(s.contract.queue->Signal(fence,value),"NR recording completion signal failed; ownership retained");if(!r)return r;s.pending->submitted=true;return {};
}
Result<void> Stage::TrackReader(const EvaluationTicket& ticket,ID3D12Fence* fence,uint64_t value){
    auto& s=*state_;if(s.terminal||!s.Owns(ticket)||!s.pending->submitted||!value)return Fail(ErrorKind::InvalidInput,"NR output reader ticket/fence invalid");
    auto identity=s.fences.Validate(fence,s.contract.device.Get());if(!identity)return identity;s.pending->readers.push_back({fence,value});return {};
}
Result<void> Stage::RetireTicket(const EvaluationTicket& ticket){
    auto& s=*state_;if(s.terminal||!s.Owns(ticket)||!s.pending->submitted)return Fail(ErrorKind::Retirement,"NR ticket unsubmitted/foreign/terminal");auto r=s.Gpu(s.contract.device->GetDeviceRemovedReason(),"NR device removed during ticket retirement");if(!r)return r;
    for(const auto& reader:s.pending->readers){const auto completed=reader.fence->GetCompletedValue();if(completed==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Retirement,"NR reader fence reports device removal");}if(completed<reader.value)return Fail(ErrorKind::Retirement,"NR output readers remain pending");}s.pending.reset();return {};
}
Result<void> Stage::Retire(){
    auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,"NR stage terminal; no teardown retry");if(s.pending)return Fail(ErrorKind::Retirement,"NR recorded output/reader owners remain pending");if(!s.client)return {};
    s.release=s.owner->Exports().release(s.feature);auto r=s.Native(s.release,"NR feature release failed; ownership retained");if(!r)return r;s.feature=nullptr;
    s.destroy=s.owner->Exports().destroy(s.parameters);r=s.Native(s.destroy,"NR parameter destruction failed; ownership retained");if(!r)return r;s.parameters=nullptr;
    {std::scoped_lock lock(allocationMutex);if(activeAllocation!=&s.allocation || !s.allocation.resources.empty() || s.allocation.failed){s.terminal=true;return Fail(ErrorKind::Retirement,"NR callback allocation ownership unbalanced");}activeAllocation=nullptr;}
    r=s.owner->ReleaseClientAfterRetirement();if(!r){s.terminal=true;return r;}s.client=false;s.ready=false;return {};
}
StageDiagnostics Stage::Diagnostics()const{const auto& s=*state_;return {s.create,s.evaluate,s.release,s.destroy,s.allocation.allocations,s.allocation.releases,s.recorded,s.terminal};}
}
