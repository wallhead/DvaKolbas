#include "PreparedBeforeUpscale.h"
#include "PerformanceQueries.h"
#include "History.h"
#include "RuntimeParameters.h"
#include "RetirementEvent.h"
#include "Upscaling/FSRColorConversion.h"
#include "FrameGen/D3D11FrameCopy.h"
#include <chrono>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
using Microsoft::WRL::ComPtr;
std::unexpected<Error> Fail(ErrorKind kind,const char* text,int64_t hr=0){return std::unexpected(Error{kind,hr,text});}
}
struct PreparedBeforeUpscale::State {
    PerformanceMetrics* metrics{};std::unique_ptr<PerformanceQueries> timing;
    BeforeUpscale bridge;History history;
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;ComPtr<ID3D11DeviceContext4> context4;ComPtr<ID3D12Device> device12;
    struct Reader {ComPtr<ID3D12Fence> fence;uint64_t value{};};
    struct Slot {ComPtr<ID3D11Texture2D> color,depth,motion;ComPtr<ID3D12Fence> leaseFence;BeforeInput held;BeforeResult bridgeDelivery;std::vector<Reader> readers;uint64_t id{},source{},epoch{},readerValue{},leaseValue{};bool busy{},borrowed{};};
    // Intentional quarantine until genuine retirement; leases also own State.
    std::shared_ptr<State> retainedSelf;
    RetirementEvent retirement;
    std::array<Slot,3> slots;size_t nextSlot{};
    Detail::TicketOwnership deliveryOwner;uint64_t deliverySerial{},readerSerial{};ComPtr<ID3D12Fence> readerFence;ComPtr<ID3D11Fence> reader11;
    Upscaling::FsrColorConverter decode,encode;
    D3D11FrameCopy::Depth depthCopy;D3D11ContextIsolation isolation;
    ImageExtent extent,guideExtent;unsigned preset{};
    ColorDomain colorDomain{ColorDomain::Linear};
    Placement placement{Placement::Before};
    bool attempted{},ready{},uncertain{},terminal{},allocationRolledBack{};
    Slot* Find(const DeliveryTicket& t){if(!deliveryOwner.Owns(t.owner_)||t.slot_>=slots.size())return nullptr;auto& slot=slots[t.slot_];return slot.id&&slot.id==t.id_&&slot.source==t.source_&&slot.epoch==t.epoch_?&slot:nullptr;}
    Result<void> Gpu(HRESULT hr,const char* text){if(FAILED(hr)){terminal=true;return Fail(ErrorKind::Runtime,text,hr);}return {};}
    Result<uint32_t> Collect(){
        if(terminal)return Fail(ErrorKind::Retirement,"NR preparation terminal; ownership retained");if(!ready)return uint32_t{0};
        auto r=Gpu(device->GetDeviceRemovedReason(),"NR preparation D3D11 device removed");if(!r)return std::unexpected(r.error());r=Gpu(device12->GetDeviceRemovedReason(),"NR preparation D3D12 device removed");if(!r)return std::unexpected(r.error());auto collected=bridge.CollectCompleted();if(!collected){terminal=true;return std::unexpected(collected.error());}auto completed=readerFence->GetCompletedValue();if(completed==UINT64_MAX){terminal=true;return Fail(ErrorKind::Retirement,"NR preparation private reader fence reports device loss");}uint32_t count{};
        for(auto& slot:slots)if(slot.busy&&!slot.borrowed&&slot.readerValue&&completed>=slot.readerValue){bool done=true;for(const auto& reader:slot.readers){auto value=reader.fence->GetCompletedValue();if(value==UINT64_MAX){terminal=true;return Fail(ErrorKind::Retirement,"NR preparation external reader reports device loss");}done&=value>=reader.value;}if(done){slot.held={};slot.readers.clear();slot.busy=false;++count;}}
        return count;
    }
    Result<size_t> Acquire(){
        auto find=[&]()->std::optional<size_t>{for(size_t i=0;i<slots.size();++i){auto index=(nextSlot+i)%slots.size();if(!slots[index].busy)return index;}return std::nullopt;};
        auto collected=Collect();if(!collected)return std::unexpected(collected.error());if(auto index=find())return *index;
        PerformanceScope performance(metrics,CpuPhase::PreparedWait);if(metrics)metrics->RecordSlotPressure();context->Flush();if(metrics)metrics->RecordFlush();auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(20);uint64_t blocked{};
        for(;;){collected=Collect();if(!collected)return std::unexpected(collected.error());if(auto index=find()){if(metrics)metrics->RecordWait(CpuPhase::PreparedWait,true,blocked);return *index;}auto start=metrics?PerformanceNow():0;auto waited=WaitProgress(deadline);if(!waited)return std::unexpected(waited.error());if(metrics)blocked+=PerformanceNow()-start;}
    }
    Result<void> WaitPending(Slot* selected=nullptr){
        if(terminal)return Fail(ErrorKind::Retirement,"NR preparation terminal; ownership retained");if(!ready)return {};
        auto initial=Collect();if(!initial)return std::unexpected(initial.error());bool pending=false;for(const auto& slot:slots)pending|=slot.busy&&(!selected||selected==&slot);if(!pending)return {};
        PerformanceScope performance(metrics,CpuPhase::PreparedWait);context->Flush();if(metrics)metrics->RecordFlush();auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(20);uint64_t blocked{};bool slept=false;
        for(;;){auto collected=Collect();if(!collected)return std::unexpected(collected.error());bool busy=false;for(const auto& slot:slots)busy|=slot.busy&&(!selected||selected==&slot);if(!busy){if(metrics)metrics->RecordWait(CpuPhase::PreparedWait,slept,blocked);return {};}
            auto start=metrics?PerformanceNow():0;auto waited=WaitProgress(deadline,selected);if(!waited)return waited;slept=true;if(metrics)blocked+=PerformanceNow()-start;}
    }
    Result<void> WaitProgress(std::chrono::steady_clock::time_point deadline,Slot* selected=nullptr){
        if(!selected){std::vector<RetirementEvent::Dependency> readers;for(const auto& slot:slots)if(slot.busy){readers.push_back({readerFence.Get(),slot.readerValue});if(slot.borrowed)readers.push_back({slot.leaseFence.Get(),slot.leaseValue});for(const auto& reader:slot.readers)readers.push_back({reader.fence.Get(),reader.value});}auto r=retirement.WaitAny(readers,device12.Get(),deadline);if(!r)terminal=true;return r;}
        Slot* oldest{};for(auto& slot:slots)if(slot.busy&&(!selected||selected==&slot)&&(!oldest||slot.id<oldest->id))oldest=&slot;if(!oldest)return {};
        auto wait=[&](ID3D12Fence* fence,uint64_t value){auto r=retirement.Wait(fence,value,device12.Get(),deadline);if(!r)terminal=true;return r;};
        auto r=wait(readerFence.Get(),oldest->readerValue);if(!r)return r;
        if(oldest->borrowed){r=wait(oldest->leaseFence.Get(),oldest->leaseValue);if(!r)return r;}
        for(const auto& reader:oldest->readers){r=wait(reader.fence.Get(),reader.value);if(!r)return r;}
        return {};
    }
    bool Shape(ID3D11Texture2D* texture,D3D11_TEXTURE2D_DESC& d,ImageExtent expected)const{
        if(!texture)return false;ComPtr<ID3D11Device> actual;texture->GetDevice(&actual);texture->GetDesc(&d);
        return D3D11FrameCopy::SameObject(actual.Get(),device.Get())&&d.Width==expected.width&&d.Height==expected.height&&
            d.MipLevels==1&&d.ArraySize==1&&d.SampleDesc.Count==1&&!d.SampleDesc.Quality&&d.Usage==D3D11_USAGE_DEFAULT;
    }
};
PreparedBeforeUpscale::PreparedBeforeUpscale():state_(std::make_shared<State>()){}
PreparedBeforeUpscale::~PreparedBeforeUpscale()=default;
Result<void> PreparedBeforeUpscale::Initialize(std::shared_ptr<RuntimeOwner> owner,ID3D11Device* device,const StageContract& c,unsigned preset,PerformanceMetrics* metrics,ColorDomain domain,Placement placement,unsigned passes){
    auto& s=*state_;if(s.attempted)return Fail(ErrorKind::Conflict,"NR source preparation initialization already attempted");s.attempted=true;
    if(!device||!owner||!c.device||!c.queue||!ValidDirectExtents(c.colorExtent,c.guideExtent)||(placement==Placement::Before&&c.colorExtent!=c.guideExtent)||preset>1||passes<1||passes>3||NrColorFormat(domain)==DXGI_FORMAT_UNKNOWN||
        (placement!=Placement::Before&&placement!=Placement::After))return Fail(ErrorKind::InvalidInput,"NR source preparation native contract invalid");
    s.device=device;s.device12=c.device;device->GetImmediateContext(&s.context);s.extent=c.colorExtent;s.guideExtent=c.guideExtent;s.preset=preset;s.colorDomain=domain;s.placement=placement;
    // No bridge/vendor work exists at this allocation boundary. Only a
    // healthy pair of devices admits source passthrough after failure.
    auto rollback=[&](Result<void> failed)->Result<void>{
        auto healthy=s.Gpu(device->GetDeviceRemovedReason(),"NR preparation D3D11 device lost during allocation rollback");if(!healthy)return healthy;
        healthy=s.Gpu(c.device->GetDeviceRemovedReason(),"NR preparation D3D12 device lost during allocation rollback");if(!healthy)return healthy;
        s.terminal=false;s.allocationRolledBack=true;return failed;
    };
    auto ready=s.Gpu(s.context.As(&s.context4),"NR preparation requires authoritative context4");if(!ready)return rollback(ready);
    if(!c.device)return Fail(ErrorKind::InvalidInput,"NR preparation retained D3D12 device missing");
    ready=s.Gpu(c.device->CreateFence(0,D3D12_FENCE_FLAG_SHARED,IID_PPV_ARGS(&s.readerFence)),"NR preparation private reader fence creation failed");if(!ready)return rollback(ready);
    HANDLE shared{};ready=s.Gpu(c.device->CreateSharedHandle(s.readerFence.Get(),nullptr,GENERIC_ALL,nullptr,&shared),"NR preparation reader shared handle failed");if(!ready)return rollback(ready);
    ComPtr<ID3D11Device5> device5;auto query=device->QueryInterface(IID_PPV_ARGS(&device5));HRESULT opened=FAILED(query)?query:device5->OpenSharedFence(shared,IID_PPV_ARGS(&s.reader11));CloseHandle(shared);ready=s.Gpu(opened,"NR preparation private reader fence open failed");if(!ready)return rollback(ready);
    if(metrics&&metrics->Enabled()){s.metrics=metrics;s.timing=std::make_unique<PerformanceQueries>(metrics);s.timing->Initialize11(device);}
    auto make=[&](DXGI_FORMAT format,UINT bind,ComPtr<ID3D11Texture2D>& out){const auto extent=format==DXGI_FORMAT_R32_FLOAT||format==DXGI_FORMAT_R16G16_FLOAT?s.guideExtent:s.extent;D3D11_TEXTURE2D_DESC d{};d.Width=extent.width;d.Height=extent.height;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=format;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=bind;return s.Gpu(device->CreateTexture2D(&d,nullptr,&out),"NR prepared source allocation failed");};
    Result<void> r;
    for(auto& slot:s.slots){r=make(NrColorFormat(domain),D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE,slot.color);if(!r)return rollback(r);
        r=s.Gpu(c.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&slot.leaseFence)),"NR prepared lease fence allocation failed");if(!r)return rollback(r);
        r=make(DXGI_FORMAT_R32_FLOAT,D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE,slot.depth);if(!r)return rollback(r);
        r=make(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE,slot.motion);if(!r)return rollback(r);
    }
    s.uncertain=true;s.retainedSelf=state_;r=s.bridge.Initialize(std::move(owner),device,c,preset,s.metrics,s.timing.get(),domain,placement,passes);if(!r){
        if(s.bridge.InitializationRolledBackBeforeCreate()){
            auto healthy=s.Gpu(device->GetDeviceRemovedReason(),"NR preparation D3D11 device removed during initialization rollback");if(!healthy)return healthy;
            healthy=s.Gpu(c.device->GetDeviceRemovedReason(),"NR preparation D3D12 device removed during initialization rollback");if(!healthy)return healthy;
            s.uncertain=false;s.terminal=false;s.retainedSelf.reset();
        }else s.terminal=true;
        return r;
    }s.ready=true;return {};
}
Result<BeforeResult> PreparedBeforeUpscale::Evaluate(const BeforeInput& input,Upscaling::ColorEncoding encoding,const SettingsSnapshot& settings,PreparedFsrInput* linearOutput){
    auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Runtime,"NR source preparation terminal; ownership retained");
    if(linearOutput&&linearOutput->Valid())return Fail(ErrorKind::InvalidInput,"NR prepared output already holds a reader lease");
    PerformanceScope performance(s.metrics,CpuPhase::Total);
    if(!settings.enabled){auto drained=s.WaitPending();if(!drained)return std::unexpected(drained.error());s.history.ResetNext();return s.bridge.Evaluate(input,settings);}
    if(!s.ready)return Fail(ErrorKind::InvalidInput,"NR source preparation not initialized");
    if(!Upscaling::IsKnownColorEncoding(encoding))return Fail(ErrorKind::InvalidInput,"NR source encoding unknown; explicit Linear/Gamma22/SRGB required");
    const bool sdr=s.colorDomain==ColorDomain::SdrBytes;
    if(sdr&&(encoding==Upscaling::ColorEncoding::Linear||linearOutput))
        return Fail(ErrorKind::Unsupported,"NR SDR byte trial requires encoded SDR and encoded delivery");
    if(settings.placement!=s.placement||settings.reconstruction.preset!=s.preset||settings.reconstruction.inputScale!=1||
        settings.reconstruction.method>ResolveMethod::Ratio||EffectiveResolve(settings.reconstruction)!=ResolveMethod::Auto||
        settings.reconstruction.colorIsHDR||settings.reconstruction.producerColor||settings.reconstruction.fusedPreparation||settings.reconstruction.peripheralCompression)
        return Fail(ErrorKind::Unsupported,"NR source adapter requires native SDR preparation at its latched placement");
    D3D11_TEXTURE2D_DESC color{},depth{},motion{};
    if(input.colorExtent!=s.extent||input.guideExtent!=s.guideExtent||input.guideEpoch!=input.epoch||input.guideSourceId!=input.sourceId||
        !input.context||!D3D11FrameCopy::SameObject(input.context.Get(),s.context.Get())||input.context->GetType()!=D3D11_DEVICE_CONTEXT_IMMEDIATE||
        !s.Shape(input.color.Get(),color,s.extent)||!s.Shape(input.depth.Get(),depth,s.guideExtent)||!s.Shape(input.motion.Get(),motion,s.guideExtent)||
        !Upscaling::SupportsFsrHandoffFormat(color.Format)||(sdr&&color.Format!=DXGI_FORMAT_R8G8B8A8_UNORM)||(color.BindFlags&(D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET))!=(D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET)||
        !(depth.BindFlags&D3D11_BIND_SHADER_RESOURCE)||(depth.Format!=DXGI_FORMAT_R32_TYPELESS&&depth.Format!=DXGI_FORMAT_R32_FLOAT&&depth.Format!=DXGI_FORMAT_R24G8_TYPELESS&&depth.Format!=DXGI_FORMAT_R32G8X24_TYPELESS)||motion.Format!=DXGI_FORMAT_R16G16_FLOAT)
        return Fail(ErrorKind::InvalidInput,"NR native source color/depth/motion ownership or shape invalid");
    ImagePacket p;p.epoch=input.epoch;p.sourceId=input.sourceId;p.previousSourceId=input.previousSourceId;p.imageId=p.batchId=input.sourceId;p.presentationTime=input.presentationTime;p.reset=input.reset;
    auto history=s.history.Check(p,settings);if(!history)return std::unexpected(history.error());
    auto index=s.Acquire();if(!index)return std::unexpected(index.error());auto& slot=s.slots[*index];slot.held=input;slot.busy=true;slot.readerValue=0;
    if(s.deliverySerial==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR preparation delivery sequence exhausted");}slot.id=++s.deliverySerial;slot.source=input.sourceId;slot.epoch=input.epoch;
    if(s.timing){s.timing->Begin11(input.context.Get(),input.sourceId);s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareColor,true);}
    auto r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareColor,[&]{return sdr?
        D3D11FrameCopy::Color(input.context.Get(),input.color.Get(),slot.color.Get(),{s.extent.width,s.extent.height}):
        s.decode.Convert(input.context.Get(),input.color.Get(),slot.color.Get(),encoding,Upscaling::ColorEncoding::Linear);}),"NR explicit SDR source preparation failed");if(!r)return std::unexpected(r.error());
    if(s.timing){s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareColor,false);s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareGuides,true);}
    {
        D3D11ContextIsolation::Scope scope(s.isolation,input.context.Get());if(!scope){s.terminal=true;return Fail(ErrorKind::Runtime,"NR guide preparation state isolation failed");}
        r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareGuides,[&]{return s.depthCopy.Copy(input.context.Get(),input.depth.Get(),slot.depth.Get(),{s.guideExtent.width,s.guideExtent.height});}),"NR source depth preparation failed");if(!r)return std::unexpected(r.error());
        r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareGuides,[&]{return D3D11FrameCopy::Color(input.context.Get(),input.motion.Get(),slot.motion.Get(),{s.guideExtent.width,s.guideExtent.height});}),"NR source motion preparation failed");if(!r)return std::unexpected(r.error());
    }
    if(s.timing)s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareGuides,false);
    auto preparedInput=input;preparedInput.color=slot.color;preparedInput.depth=slot.depth;preparedInput.motion=slot.motion;preparedInput.colorDomain=s.colorDomain;preparedInput.reset=history->Reset();
    auto result=s.bridge.Evaluate(preparedInput,settings);if(!result){s.terminal=true;return std::unexpected(result.error());}
    slot.bridgeDelivery=*result;
    if(!linearOutput){
        if(s.timing)s.timing->Stamp11(input.context.Get(),GpuPhase::Encode,true);
        r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::Encode,[&]{return sdr?
            D3D11FrameCopy::Color(input.context.Get(),slot.color.Get(),input.color.Get(),{s.extent.width,s.extent.height}):
            s.encode.Convert(input.context.Get(),slot.color.Get(),input.color.Get(),Upscaling::ColorEncoding::Linear,encoding);}),"NR explicit SDR source delivery failed");if(!r)return std::unexpected(r.error());
        if(s.timing)s.timing->Stamp11(input.context.Get(),GpuPhase::Encode,false);
    }
    if(s.timing)s.timing->End11(input.context.Get());
    if(s.readerSerial==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR preparation reader sequence exhausted");}slot.readerValue=++s.readerSerial;
    r=s.Gpu(s.context4->Signal(s.reader11.Get(),slot.readerValue),"NR preparation encoder reader signal failed");if(!r)return std::unexpected(r.error());
    r=s.bridge.TrackReader(slot.bridgeDelivery.delivery,s.readerFence.Get(),slot.readerValue);if(!r){s.terminal=true;return std::unexpected(r.error());}
    r=s.history.CommitRecorded(*history);if(!r){s.terminal=true;return std::unexpected(r.error());}
    DeliveryTicket delivery;delivery.owner_=s.deliveryOwner.Seal();delivery.id_=slot.id;delivery.source_=slot.source;delivery.epoch_=slot.epoch;delivery.slot_=*index;s.nextSlot=(*index+1)%s.slots.size();result->delivery=delivery;
    if(linearOutput){
        // This CPU-owned per-slot lease gate is never a GPU dependency. Register
        // the genuine FSR reader before releasing it; no unrelated later lease
        // can advance this slot's gate or impersonate completion of that reader.
        if(slot.leaseValue==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR prepared lease sequence exhausted");}
        r=s.bridge.TrackReader(slot.bridgeDelivery.delivery,slot.leaseFence.Get(),++slot.leaseValue);if(!r){s.terminal=true;return std::unexpected(r.error());}
        slot.borrowed=true;PreparedFsrInput lease;lease.source_=slot.source;lease.epoch_=slot.epoch;lease.reset_=result->effectiveReset;lease.width_=s.extent.width;lease.height_=s.extent.height;
        lease.context_=input.context;lease.color_=slot.color;lease.depth_=slot.depth;lease.motion_=slot.motion;lease.producer_=s.readerFence;lease.producerValue_=slot.readerValue;
        lease.track_=[owner=state_,delivery](ID3D12Fence* fence,uint64_t value)->Result<void>{auto* held=owner->Find(delivery);if(owner->terminal||!held||!held->busy||!held->borrowed)return Fail(ErrorKind::InvalidInput,"NR prepared FSR lease expired/foreign");auto r=owner->bridge.TrackReader(held->bridgeDelivery.delivery,fence,value);if(!r){owner->terminal=true;return r;}held->readers.push_back({fence,value});r=owner->Gpu(held->leaseFence->Signal(held->leaseValue),"NR prepared lease release failed");if(!r)return r;held->borrowed=false;return {};};
        lease.abandon_=[owner=state_,delivery]{auto* held=owner->Find(delivery);if(held&&held->busy&&held->borrowed)owner->terminal=true;};
        *linearOutput=std::move(lease);
    }
    return *result;
}
Result<uint32_t> PreparedBeforeUpscale::CollectCompleted(){return state_->Collect();}
Result<void> PreparedBeforeUpscale::WaitDelivery(const BeforeResult& result){auto& s=*state_;if(!result.evaluated)return {};auto* slot=s.Find(result.delivery);if(!slot)return Fail(ErrorKind::InvalidInput,"NR prepared delivery ticket expired/foreign");auto drained=s.WaitPending(slot);if(!drained)return drained;return s.bridge.WaitDelivery(slot->bridgeDelivery);}
Result<void> PreparedBeforeUpscale::TrackReader(const DeliveryTicket& delivery,ID3D12Fence* fence,uint64_t value){auto& s=*state_;auto* slot=s.Find(delivery);if(s.terminal||!slot||!slot->busy)return Fail(ErrorKind::InvalidInput,"NR prepared reader delivery expired/foreign");auto result=s.bridge.TrackReader(slot->bridgeDelivery.delivery,fence,value);if(!result){s.terminal=true;return result;}slot->readers.push_back({fence,value});return {};}
Result<void> PreparedBeforeUpscale::Retire(){auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,"NR preparation terminal; no teardown retry");if(!s.ready)return {};
    auto drained=s.WaitPending();if(!drained)return drained;if(s.timing)s.timing->Collect11(s.context.Get());auto r=s.bridge.Retire();if(!r){s.terminal=true;return r;}s.ready=false;s.uncertain=false;s.retainedSelf.reset();return {};}
bool PreparedBeforeUpscale::InitializationRolledBackBeforeCreate()const noexcept{return !state_->terminal&&!state_->uncertain&&(state_->allocationRolledBack||state_->bridge.InitializationRolledBackBeforeCreate());}
StageDiagnostics PreparedBeforeUpscale::Diagnostics()const{auto d=state_->bridge.Diagnostics();d.terminal|=state_->terminal;d.preparedSlots=state_->ready?uint32_t(state_->slots.size()):0;if(state_->timing){d.gpuTiming11Available=state_->timing->Available11();d.gpuTimingDropped+=state_->timing->Dropped();}return d;}
}
