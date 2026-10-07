#include "BeforeUpscale.h"
#include "PerformanceQueries.h"
#include "History.h"
#include "RuntimeParameters.h"
#include "Graphics/D3D11D3D12Interop.h"
#include <cmath>
#include <chrono>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
using Microsoft::WRL::ComPtr;
std::unexpected<Error> Fail(ErrorKind kind,const char* text,int64_t code=0){return std::unexpected(Error{kind,code,text});}
void Transition(ID3D12GraphicsCommandList* list,ID3D12Resource* resource,D3D12_RESOURCE_STATES from,D3D12_RESOURCE_STATES to){
    D3D12_RESOURCE_BARRIER b{};b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    b.Transition={resource,D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,from,to};list->ResourceBarrier(1,&b);
}
}
struct BeforeUpscale::State {
    PerformanceMetrics* metrics{};PerformanceQueries* queries11{};std::unique_ptr<PerformanceQueries> ownTiming11;
    Stage stage;
    Graphics::D3D11D3D12Interop interop;
    StageContract contract;
    ComPtr<ID3D11Device> device11;
    ComPtr<ID3D11DeviceContext> context;
    struct Slot {Graphics::SharedTexture color,depth,motion,output;BeforeInput heldInput;std::optional<EvaluationTicket> ticket;uint64_t id{},source{},epoch{};};
    std::array<Slot,3> slots;size_t nextSlot{};
    Detail::TicketOwnership deliveryOwner;uint64_t deliverySerial{};
    ComPtr<ID3D12Fence> handoffFence,completionFence;
    ComPtr<ID3D11Fence> handoff11;
    uint64_t handoffValue{},completionValue{};
    unsigned preset{},passes{1};
    ColorDomain colorDomain{ColorDomain::Linear};
    Placement placement{Placement::Before};
    std::optional<std::array<int,3>> recordedStyles;
    History history;
    bool attempted{},ready{},uncertain{},terminal{},allocationRolledBack{};
    Slot* Find(const DeliveryTicket& t){if(!deliveryOwner.Owns(t.owner_)||t.slot_>=slots.size())return nullptr;auto& slot=slots[t.slot_];return slot.id&&slot.id==t.id_&&slot.source==t.source_&&slot.epoch==t.epoch_?&slot:nullptr;}
    Result<void> Gpu(HRESULT hr,const char* text){if(FAILED(hr)){terminal=true;return Fail(ErrorKind::Runtime,text,hr);}return {};}
    Result<uint32_t> Collect(){
        if(terminal)return Fail(ErrorKind::Retirement,"NR Before terminal; ownership retained");if(!ready)return uint32_t{0};
        auto r=Gpu(device11->GetDeviceRemovedReason(),"NR Before D3D11 device removed during collection");if(!r)return std::unexpected(r.error());uint32_t count{};
        for(auto& slot:slots)if(slot.ticket){auto retired=stage.RetireTicket(*slot.ticket);if(retired){slot.ticket.reset();slot.heldInput={};++count;}else if(stage.Diagnostics().terminal||retired.error().kind!=ErrorKind::Retirement){terminal=true;return std::unexpected(retired.error());}}
        return count;
    }
    Result<size_t> Acquire(){
        auto find=[&]()->std::optional<size_t>{for(size_t i=0;i<slots.size();++i){auto index=(nextSlot+i)%slots.size();if(!slots[index].ticket)return index;}return std::nullopt;};
        auto collected=Collect();if(!collected)return std::unexpected(collected.error());if(auto index=find())return *index;
        PerformanceScope performance(metrics,CpuPhase::ConsumerWait);if(metrics)metrics->RecordSlotPressure();interop.Context11()->Flush();if(metrics)metrics->RecordFlush();auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(20);uint64_t blocked{};
        for(;;){collected=Collect();if(!collected)return std::unexpected(collected.error());if(auto index=find()){if(metrics)metrics->RecordWait(CpuPhase::ConsumerWait,true,blocked);return *index;}auto start=metrics?PerformanceNow():0;auto waited=WaitProgress(deadline);if(!waited)return std::unexpected(waited.error());if(metrics)blocked+=PerformanceNow()-start;}
    }
    Result<void> WaitPending(Slot* selected=nullptr){
        if(terminal)return Fail(ErrorKind::Retirement,"NR Before terminal; ownership retained");if(!ready)return {};
        auto initial=Collect();if(!initial)return std::unexpected(initial.error());bool pending=false;for(const auto& slot:slots)pending|=bool(slot.ticket)&&(!selected||selected==&slot);if(!pending)return {};
        PerformanceScope performance(metrics,CpuPhase::ConsumerWait);interop.Context11()->Flush();if(metrics)metrics->RecordFlush();auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(20);uint64_t blocked{};bool slept=false;
        for(;;){auto collected=Collect();if(!collected)return std::unexpected(collected.error());bool busy=false;for(const auto& slot:slots)busy|=bool(slot.ticket)&&(!selected||selected==&slot);if(!busy){if(metrics)metrics->RecordWait(CpuPhase::ConsumerWait,slept,blocked);return {};}
            auto start=metrics?PerformanceNow():0;auto waited=WaitProgress(deadline,selected);if(!waited)return waited;slept=true;if(metrics)blocked+=PerformanceNow()-start;}
    }
    Result<void> WaitProgress(std::chrono::steady_clock::time_point deadline,Slot* selected=nullptr){
        Slot* oldest{};for(auto& slot:slots)if(slot.ticket&&(!selected||selected==&slot)&&(!oldest||slot.id<oldest->id))oldest=&slot;if(!oldest)return {};
        auto remaining=std::chrono::ceil<std::chrono::milliseconds>(deadline-std::chrono::steady_clock::now()).count();if(remaining<=0){terminal=true;return Fail(ErrorKind::Retirement,"NR Before reader retirement deadline exceeded; ownership retained");}
        auto waited=selected?stage.WaitForRetirement(*oldest->ticket,uint32_t(remaining)):stage.WaitForProgress(uint32_t(remaining));if(!waited)terminal=true;return waited;
    }
    bool Texture(ID3D11Texture2D* texture,DXGI_FORMAT format,ImageExtent extent)const {
        if(!texture)return false;ComPtr<ID3D11Device> device;texture->GetDevice(&device);
        D3D11_TEXTURE2D_DESC d{};texture->GetDesc(&d);
        return D3D11FrameCopy::SameObject(device.Get(),device11.Get())&&d.Format==format&&
            d.Width==extent.width&&d.Height==extent.height&&
            d.ArraySize==1&&d.MipLevels==1&&d.SampleDesc.Count==1&&!d.SampleDesc.Quality&&
            !(d.BindFlags&D3D11_BIND_DEPTH_STENCIL)&&d.Usage==D3D11_USAGE_DEFAULT;
    }
};
BeforeUpscale::BeforeUpscale():state_(std::make_unique<State>()){}
BeforeUpscale::~BeforeUpscale(){if(state_->uncertain)state_.release();}
Result<void> BeforeUpscale::Initialize(std::shared_ptr<RuntimeOwner> owner,ID3D11Device* device,const StageContract& contract,unsigned preset,PerformanceMetrics* metrics,PerformanceQueries* queries11,ColorDomain domain,Placement placement,unsigned passes){
    auto& s=*state_;if(s.attempted)return Fail(ErrorKind::Conflict,"NR Before initialization already attempted");s.attempted=true;
    if(!device||!owner||!contract.device||!contract.queue||!contract.colorExtent.width||!contract.colorExtent.height||
        !ValidDirectExtents(contract.colorExtent,contract.guideExtent)||(placement==Placement::Before&&contract.colorExtent!=contract.guideExtent)||preset>1||passes<1||passes>3||NrColorFormat(domain)==DXGI_FORMAT_UNKNOWN||
        (placement!=Placement::Before&&placement!=Placement::After))return Fail(ErrorKind::InvalidInput,"NR native source device/extent/color/placement contract incomplete");
    s.device11=device;s.contract=contract;
    // Until Stage is entered, these allocations have no vendor recordings or
    // submitted readers. Device loss still forbids a healthy fallback.
    auto rollback=[&](Result<void> failed)->Result<void>{
        auto healthy=s.Gpu(device->GetDeviceRemovedReason(),"NR Before D3D11 device lost during allocation rollback");if(!healthy)return healthy;
        healthy=s.Gpu(contract.device->GetDeviceRemovedReason(),"NR Before D3D12 device lost during allocation rollback");if(!healthy)return healthy;
        s.terminal=false;s.allocationRolledBack=true;return failed;
    };
    auto r=s.Gpu(s.interop.Initialize(device,contract.device.Get(),contract.queue.Get()),"NR Before same-adapter bridge initialization failed");if(!r)return rollback(r);
    s.preset=preset;s.passes=passes;s.colorDomain=domain;s.placement=placement;device->GetImmediateContext(&s.context);
    if(metrics&&metrics->Enabled()){
        s.metrics=metrics;s.queries11=queries11;
        if(!queries11){s.ownTiming11=std::make_unique<PerformanceQueries>(metrics);s.ownTiming11->Initialize11(device);s.queries11=s.ownTiming11.get();}
        s.interop.SetPerformanceSink({metrics,[](void* p,bool blocked,uint64_t ns){static_cast<PerformanceMetrics*>(p)->RecordWait(CpuPhase::InteropWait,blocked,ns);},[](void* p){static_cast<PerformanceMetrics*>(p)->RecordFlush();}});
    }
    r=s.Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_SHARED,IID_PPV_ARGS(&s.handoffFence)),"NR Before shared handoff fence creation failed");if(!r)return rollback(r);
    ComPtr<ID3D11Device5> device5;r=s.Gpu(device->QueryInterface(IID_PPV_ARGS(&device5)),"NR Before requires D3D11 shared-fence device");if(!r)return rollback(r);
    HANDLE shared{};r=s.Gpu(contract.device->CreateSharedHandle(s.handoffFence.Get(),nullptr,GENERIC_ALL,nullptr,&shared),"NR Before shared fence handle failed");if(!r)return rollback(r);
    const auto opened=device5->OpenSharedFence(shared,IID_PPV_ARGS(&s.handoff11));CloseHandle(shared);
    r=s.Gpu(opened,"NR Before D3D11 fence open failed");if(!r)return rollback(r);
    r=s.Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&s.completionFence)),"NR Before completion fence creation failed");if(!r)return rollback(r);
    auto make=[&](DXGI_FORMAT format,Graphics::SharedTexture& texture,bool uav){
        const auto extent=format==DXGI_FORMAT_R32_FLOAT||format==DXGI_FORMAT_R16G16_FLOAT?contract.guideExtent:contract.colorExtent;
        D3D11_TEXTURE2D_DESC d{};d.Width=extent.width;d.Height=extent.height;
        d.MipLevels=d.ArraySize=d.SampleDesc.Count=1;d.Format=format;d.Usage=D3D11_USAGE_DEFAULT;
        // The accepted D3D12-to-D3D11 shared-open route needs RTV capability,
        // including input resources which this adapter only copies/samples.
        d.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE|(uav?D3D11_BIND_UNORDERED_ACCESS:0);
        const auto created=s.interop.CreateSharedTexture(d,texture);
        if(FAILED(created)) {s.terminal=true;return Result<void>{std::unexpected(Error{ErrorKind::Runtime,created,"NR Before shared texture creation failed, DXGI format="+std::to_string(unsigned(format))+" bind="+std::to_string(d.BindFlags)})};}
        return Result<void>{};};
    for(auto& slot:s.slots)for(auto [format,texture,uav]:{std::tuple{NrColorFormat(domain),&slot.color,false},
        std::tuple{DXGI_FORMAT_R32_FLOAT,&slot.depth,false},std::tuple{DXGI_FORMAT_R16G16_FLOAT,&slot.motion,false},
        std::tuple{NrColorFormat(domain),&slot.output,true}}){r=make(format,*texture,uav);if(!r)return rollback(r);}
    // Stage initialization can submit feature creation; never destroy this
    // bridge's retained devices/queue after an uncertain result.
    s.uncertain=true;r=s.stage.Initialize(std::move(owner),contract,preset,s.metrics,passes);if(!r){
        if(s.stage.InitializationRolledBackBeforeCreate()){
            auto healthy=s.Gpu(device->GetDeviceRemovedReason(),"NR Before D3D11 device removed during initialization rollback");if(!healthy)return healthy;
            healthy=s.Gpu(contract.device->GetDeviceRemovedReason(),"NR Before D3D12 device removed during initialization rollback");if(!healthy)return healthy;
            // Initialize allocated bridge resources but recorded/submitted none.
            s.uncertain=false;s.terminal=false;
        }else s.terminal=true;
        return r;
    }
    s.ready=true;return {};
}
Result<BeforeResult> BeforeUpscale::Evaluate(const BeforeInput& input,const SettingsSnapshot& settings){
    auto& s=*state_;
    PerformanceScope performance(s.metrics,CpuPhase::Bridge);
    if(s.terminal)return Fail(ErrorKind::Runtime,"NR Before terminal; ownership retained");
    if(!settings.enabled){auto drained=s.WaitPending();if(!drained)return std::unexpected(drained.error());s.history.ResetNext();return BeforeResult{false,input.reset};}
    if(!s.ready)return Fail(ErrorKind::InvalidInput,"NR Before runtime/bridge not initialized");
    if(settings.placement!=s.placement)return Fail(ErrorKind::Unsupported,"NR native source request differs from latched placement");
    if(settings.passes!=int(s.passes))return Fail(ErrorKind::InvalidInput,"NR native source pass count differs from initialized chain");
    if(input.colorDomain!=s.colorDomain||input.colorExtent!=s.contract.colorExtent||input.guideExtent!=s.contract.guideExtent||
        !input.context||input.context->GetType()!=D3D11_DEVICE_CONTEXT_IMMEDIATE||!D3D11FrameCopy::SameObject(input.context.Get(),s.context.Get())||
        !s.Texture(input.color.Get(),NrColorFormat(s.colorDomain),s.contract.colorExtent)||!s.Texture(input.depth.Get(),DXGI_FORMAT_R32_FLOAT,s.contract.guideExtent)||!s.Texture(input.motion.Get(),DXGI_FORMAT_R16G16_FLOAT,s.contract.guideExtent)||
        input.color.Get()==input.depth.Get()||input.color.Get()==input.motion.Get()||input.depth.Get()==input.motion.Get()||
        input.guideEpoch!=input.epoch||input.guideSourceId!=input.sourceId||!std::isfinite(input.motionScaleX)||!std::isfinite(input.motionScaleY)||!input.motionScaleX||!input.motionScaleY)
        return Fail(ErrorKind::InvalidInput,"NR Before input ownership/encoding/native guides invalid");
    if(settings.reconstruction.preset!=s.preset||settings.reconstruction.inputScale!=1||settings.reconstruction.colorIsHDR||settings.reconstruction.producerColor||settings.reconstruction.peripheralCompression||settings.reconstruction.fusedPreparation||
        settings.reconstruction.method>ResolveMethod::Ratio||EffectiveResolve(settings.reconstruction)!=ResolveMethod::Auto)
        return Fail(ErrorKind::Unsupported,"NR Before native bridge requires adapter-owned color/reconstruction preparation");
    std::array<int,3> styles{};for(unsigned i=0;i<s.passes;++i)styles[i]=SanitizeBuild14Tuning(PassTuning(settings,i)).style;
    if(s.recordedStyles&&*s.recordedStyles!=styles){
        // A live style switch in the supplied runtime reproduced a device hang
        // even with Reset set while older evaluations
        // still in flight. Retire their genuine vendor AND delivery readers
        // before recording the new style; unchanged styles stay asynchronous.
        auto drained=s.WaitPending();if(!drained)return std::unexpected(drained.error());
        s.history.ResetNext();
    }
    ImagePacket p;p.epoch=input.epoch;p.guideEpoch=input.guideEpoch;p.sourceId=p.batchId=p.imageId=input.sourceId;
    p.guideSourceId=input.guideSourceId;p.previousSourceId=input.previousSourceId;p.presentationTime=input.presentationTime;
    p.kind=ImageKind::Real;p.interpolationFraction=1.;p.colorDomain=input.colorDomain;p.guideOrigin=GuideOrigin::RealSource;
    p.colorExtent=input.colorExtent;p.guideExtent=input.guideExtent;p.motionScaleX=input.motionScaleX;p.motionScaleY=input.motionScaleY;
    p.reset=input.reset;p.depthInverted=input.depthInverted;
    auto history=s.history.Check(p,settings);if(!history)return std::unexpected(history.error());
    auto index=s.Acquire();if(!index)return std::unexpected(index.error());auto& slot=s.slots[*index];slot.heldInput=input;
    if(s.deliverySerial==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR Before delivery sequence exhausted");}slot.id=++s.deliverySerial;slot.source=input.sourceId;slot.epoch=input.epoch;
    auto gpu=[&](HRESULT hr,const char* text)->Result<void>{return s.Gpu(hr,text);};
    if(s.ownTiming11)s.queries11->Begin11(input.context.Get(),input.sourceId);
    if(s.queries11)s.queries11->Stamp11(input.context.Get(),GpuPhase::InputCopy,true);
    auto r=gpu(MeasurePerformance(s.metrics,CpuPhase::InputCopy,[&]{return s.interop.CopyInput(input.color.Get(),slot.color);}),"NR Before color copy rejected");if(!r)return std::unexpected(r.error());
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::InputCopy,[&]{return s.interop.CopyInput(input.depth.Get(),slot.depth);}),"NR Before depth copy rejected");if(!r)return std::unexpected(r.error());
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::InputCopy,[&]{return s.interop.CopyInput(input.motion.Get(),slot.motion);}),"NR Before motion copy rejected");if(!r)return std::unexpected(r.error());
    if(s.queries11)s.queries11->Stamp11(input.context.Get(),GpuPhase::InputCopy,false);
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::ProducerSignal,[&]{return s.interop.SignalProducer();}),"NR Before producer submission failed");if(!r)return std::unexpected(r.error());
    r=gpu(s.interop.ProducerDependency(&p.producerFence,&p.producerFenceValue),"NR Before submitted producer dependency missing");if(!r)return std::unexpected(r.error());
    ID3D12GraphicsCommandList* list{};r=gpu(MeasurePerformance(s.metrics,CpuPhase::Begin,[&]{return s.interop.BeginForStageProducer(&list);}),"NR Before command recording begin failed");if(!r)return std::unexpected(r.error());
    constexpr auto read=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    for(auto* resource:{slot.color.texture12.Get(),slot.depth.texture12.Get(),slot.motion.texture12.Get()})Transition(list,resource,D3D12_RESOURCE_STATE_COMMON,read);
    Transition(list,slot.output.texture12.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    p.color=slot.color.texture12;p.depth=slot.depth.texture12;p.motion=slot.motion.texture12;p.output=slot.output.texture12;
    p.colorState=p.depthState=p.motionState=read;p.outputState=D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    p.reset=history->Reset();auto ticket=s.stage.RecordQueued(list,p,settings);
    if(!ticket){s.terminal=true;return std::unexpected(ticket.error());}
    s.recordedStyles=styles;
    slot.ticket=*ticket;
    r=s.history.CommitRecorded(*history);if(!r){s.terminal=true;return std::unexpected(r.error());}
    for(auto* resource:{slot.color.texture12.Get(),slot.depth.texture12.Get(),slot.motion.texture12.Get()})Transition(list,resource,read,D3D12_RESOURCE_STATE_COMMON);
    Transition(list,slot.output.texture12.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_COMMON);
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::Submit,[&]{return s.interop.Submit();}),"NR Before recorded list submission failed");if(!r)return std::unexpected(r.error());
    if(s.completionValue==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR Before completion sequence exhausted");}
    r=s.stage.MarkSubmitted(*ticket,s.completionFence.Get(),++s.completionValue);if(!r){s.terminal=true;return std::unexpected(r.error());}
    r=gpu(s.interop.WaitConsumer(),"NR Before D3D11 consumer wait failed");if(!r)return std::unexpected(r.error());
    if(s.queries11)s.queries11->Stamp11(input.context.Get(),GpuPhase::Delivery,true);
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::Delivery,[&]{return
        D3D11FrameCopy::Color(input.context.Get(),slot.output.texture11.Get(),input.color.Get(),{input.colorExtent.width,input.colorExtent.height});}),"NR Before color delivery failed");if(!r)return std::unexpected(r.error());
    if(s.queries11){s.queries11->Stamp11(input.context.Get(),GpuPhase::Delivery,false);if(s.ownTiming11)s.queries11->End11(input.context.Get());}
    // Cover shared source and final NR output reads. Stage retains the entire
    // chain through this signal's genuine completion.
    if(s.handoffValue==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR Before consumer sequence exhausted");}
    r=gpu(s.interop.Context11()->Signal(s.handoff11.Get(),++s.handoffValue),"NR Before output reader signal failed");if(!r)return std::unexpected(r.error());
    r=s.stage.TrackReader(*ticket,s.handoffFence.Get(),s.handoffValue);if(!r){s.terminal=true;return std::unexpected(r.error());}
    s.interop.Context11()->Flush();if(s.metrics)s.metrics->RecordFlush();
    DeliveryTicket delivery;delivery.owner_=s.deliveryOwner.Seal();delivery.id_=slot.id;delivery.source_=slot.source;delivery.epoch_=slot.epoch;delivery.slot_=*index;s.nextSlot=(*index+1)%s.slots.size();return BeforeResult{true,history->Reset(),delivery};
}
Result<uint32_t> BeforeUpscale::CollectCompleted(){return state_->Collect();}
Result<void> BeforeUpscale::WaitDelivery(const BeforeResult& result){auto& s=*state_;if(!result.evaluated)return {};auto* slot=s.Find(result.delivery);if(!slot)return Fail(ErrorKind::InvalidInput,"NR Before delivery ticket expired/foreign");return s.WaitPending(slot);}
Result<void> BeforeUpscale::TrackReader(const DeliveryTicket& delivery,ID3D12Fence* fence,uint64_t value){auto& s=*state_;auto* slot=s.Find(delivery);if(s.terminal||!slot||!slot->ticket)return Fail(ErrorKind::InvalidInput,"NR Before reader delivery expired/foreign");auto result=s.stage.TrackReader(*slot->ticket,fence,value);if(!result){s.terminal=true;return result;}return {};}
Result<void> BeforeUpscale::Retire(){
    auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,"NR Before terminal; no teardown retry");
    if(!s.ready)return {};
    auto drained=s.WaitPending();if(!drained)return drained;
    auto r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::Drain,[&]{return s.interop.Drain();}),"NR Before final consumer retirement failed");if(!r)return r;
    if(s.ownTiming11)s.queries11->Collect11(s.context.Get());
    r=s.stage.Retire();if(!r){s.terminal=true;return r;}s.ready=false;s.uncertain=false;return {};
}
bool BeforeUpscale::InitializationRolledBackBeforeCreate()const noexcept{return !state_->terminal&&!state_->uncertain&&(state_->allocationRolledBack||state_->stage.InitializationRolledBackBeforeCreate());}
StageDiagnostics BeforeUpscale::Diagnostics()const{auto d=state_->stage.Diagnostics();d.terminal|=state_->terminal;d.bridgeSlots=state_->ready?uint32_t(state_->slots.size()):0;if(state_->ownTiming11){d.gpuTiming11Available=state_->queries11->Available11();d.gpuTimingDropped+=state_->queries11->Dropped();}return d;}
}
