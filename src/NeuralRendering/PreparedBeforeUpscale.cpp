#include "PreparedBeforeUpscale.h"
#include "PerformanceQueries.h"
#include "History.h"
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
    struct Slot {ComPtr<ID3D11Texture2D> color,depth,motion;BeforeInput held;BeforeResult bridgeDelivery;std::vector<Reader> readers;uint64_t id{},source{},epoch{},readerValue{};bool busy{};};
    std::array<Slot,3> slots;size_t nextSlot{};
    Detail::TicketOwnership deliveryOwner;uint64_t deliverySerial{},readerSerial{};ComPtr<ID3D12Fence> readerFence;ComPtr<ID3D11Fence> reader11;
    Upscaling::FsrColorConverter decode,encode;
    D3D11FrameCopy::Depth depthCopy;D3D11ContextIsolation isolation;
    ImageExtent extent;unsigned preset{};
    bool attempted{},ready{},uncertain{},terminal{};
    Slot* Find(const DeliveryTicket& t){if(!deliveryOwner.Owns(t.owner_)||t.slot_>=slots.size())return nullptr;auto& slot=slots[t.slot_];return slot.id&&slot.id==t.id_&&slot.source==t.source_&&slot.epoch==t.epoch_?&slot:nullptr;}
    Result<void> Gpu(HRESULT hr,const char* text){if(FAILED(hr)){terminal=true;return Fail(ErrorKind::Runtime,text,hr);}return {};}
    Result<uint32_t> Collect(){
        if(terminal)return Fail(ErrorKind::Retirement,"NR preparation terminal; ownership retained");if(!ready)return uint32_t{0};
        auto r=Gpu(device->GetDeviceRemovedReason(),"NR preparation D3D11 device removed");if(!r)return std::unexpected(r.error());r=Gpu(device12->GetDeviceRemovedReason(),"NR preparation D3D12 device removed");if(!r)return std::unexpected(r.error());auto collected=bridge.CollectCompleted();if(!collected){terminal=true;return std::unexpected(collected.error());}auto completed=readerFence->GetCompletedValue();if(completed==UINT64_MAX){terminal=true;return Fail(ErrorKind::Retirement,"NR preparation private reader fence reports device loss");}uint32_t count{};
        for(auto& slot:slots)if(slot.busy&&slot.readerValue&&completed>=slot.readerValue){bool done=true;for(const auto& reader:slot.readers){auto value=reader.fence->GetCompletedValue();if(value==UINT64_MAX){terminal=true;return Fail(ErrorKind::Retirement,"NR preparation external reader reports device loss");}done&=value>=reader.value;}if(done){slot.held={};slot.readers.clear();slot.busy=false;++count;}}
        return count;
    }
    Result<size_t> Acquire(){
        auto find=[&]()->std::optional<size_t>{for(size_t i=0;i<slots.size();++i){auto index=(nextSlot+i)%slots.size();if(!slots[index].busy)return index;}return std::nullopt;};
        auto collected=Collect();if(!collected)return std::unexpected(collected.error());if(auto index=find())return *index;
        PerformanceScope performance(metrics,CpuPhase::PreparedWait);if(metrics)metrics->RecordSlotPressure();context->Flush();if(metrics)metrics->RecordFlush();auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(20);uint64_t blocked{};
        for(;;){collected=Collect();if(!collected)return std::unexpected(collected.error());if(auto index=find()){if(metrics)metrics->RecordWait(CpuPhase::PreparedWait,true,blocked);return *index;}if(std::chrono::steady_clock::now()>=deadline){terminal=true;return Fail(ErrorKind::Retirement,"NR preparation slot capacity reader retirement deadline exceeded; ownership retained");}auto start=metrics?PerformanceNow():0;Sleep(1);if(metrics)blocked+=PerformanceNow()-start;}
    }
    Result<void> WaitPending(Slot* selected=nullptr){
        if(terminal)return Fail(ErrorKind::Retirement,"NR preparation terminal; ownership retained");if(!ready)return {};
        PerformanceScope performance(metrics,CpuPhase::PreparedWait);context->Flush();if(metrics)metrics->RecordFlush();auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(20);uint64_t blocked{};bool slept=false;
        for(;;){auto collected=Collect();if(!collected)return std::unexpected(collected.error());bool busy=false;for(const auto& slot:slots)busy|=slot.busy&&(!selected||selected==&slot);if(!busy){if(metrics)metrics->RecordWait(CpuPhase::PreparedWait,slept,blocked);return {};}
            if(std::chrono::steady_clock::now()>=deadline){terminal=true;return Fail(ErrorKind::Retirement,"NR preparation delivery retirement deadline exceeded; ownership retained");}auto start=metrics?PerformanceNow():0;Sleep(1);slept=true;if(metrics)blocked+=PerformanceNow()-start;}
    }
    bool Shape(ID3D11Texture2D* texture,D3D11_TEXTURE2D_DESC& d)const{
        if(!texture)return false;ComPtr<ID3D11Device> actual;texture->GetDevice(&actual);texture->GetDesc(&d);
        return D3D11FrameCopy::SameObject(actual.Get(),device.Get())&&d.Width==extent.width&&d.Height==extent.height&&
            d.MipLevels==1&&d.ArraySize==1&&d.SampleDesc.Count==1&&!d.SampleDesc.Quality&&d.Usage==D3D11_USAGE_DEFAULT;
    }
};
PreparedBeforeUpscale::PreparedBeforeUpscale():state_(std::make_unique<State>()){}
PreparedBeforeUpscale::~PreparedBeforeUpscale(){if(state_->uncertain)state_.release();}
Result<void> PreparedBeforeUpscale::Initialize(std::shared_ptr<RuntimeOwner> owner,ID3D11Device* device,const StageContract& c,unsigned preset,PerformanceMetrics* metrics){
    auto& s=*state_;if(s.attempted)return Fail(ErrorKind::Conflict,"NR source preparation initialization already attempted");s.attempted=true;
    if(!device||c.colorExtent!=c.guideExtent||!c.colorExtent.width||!c.colorExtent.height||preset>1)return Fail(ErrorKind::InvalidInput,"NR source preparation native contract invalid");
    s.device=device;s.device12=c.device;device->GetImmediateContext(&s.context);s.extent=c.colorExtent;s.preset=preset;
    auto ready=s.Gpu(s.context.As(&s.context4),"NR preparation requires authoritative context4");if(!ready)return ready;
    if(!c.device)return Fail(ErrorKind::InvalidInput,"NR preparation retained D3D12 device missing");
    ready=s.Gpu(c.device->CreateFence(0,D3D12_FENCE_FLAG_SHARED,IID_PPV_ARGS(&s.readerFence)),"NR preparation private reader fence creation failed");if(!ready)return ready;
    HANDLE shared{};ready=s.Gpu(c.device->CreateSharedHandle(s.readerFence.Get(),nullptr,GENERIC_ALL,nullptr,&shared),"NR preparation reader shared handle failed");if(!ready)return ready;
    ComPtr<ID3D11Device5> device5;auto query=device->QueryInterface(IID_PPV_ARGS(&device5));HRESULT opened=FAILED(query)?query:device5->OpenSharedFence(shared,IID_PPV_ARGS(&s.reader11));CloseHandle(shared);ready=s.Gpu(opened,"NR preparation private reader fence open failed");if(!ready)return ready;
    if(metrics&&metrics->Enabled()){s.metrics=metrics;s.timing=std::make_unique<PerformanceQueries>(metrics);s.timing->Initialize11(device);}
    auto make=[&](DXGI_FORMAT format,UINT bind,ComPtr<ID3D11Texture2D>& out){D3D11_TEXTURE2D_DESC d{};d.Width=s.extent.width;d.Height=s.extent.height;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=format;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=bind;return s.Gpu(device->CreateTexture2D(&d,nullptr,&out),"NR prepared source allocation failed");};
    Result<void> r;
    for(auto& slot:s.slots){r=make(DXGI_FORMAT_R16G16B16A16_FLOAT,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE,slot.color);if(!r)return r;
        r=make(DXGI_FORMAT_R32_FLOAT,D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE,slot.depth);if(!r)return r;
        r=make(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE,slot.motion);if(!r)return r;
    }
    s.uncertain=true;r=s.bridge.Initialize(std::move(owner),device,c,preset,s.metrics,s.timing.get());if(!r){s.terminal=true;return r;}s.ready=true;return {};
}
Result<BeforeResult> PreparedBeforeUpscale::Evaluate(const BeforeInput& input,Upscaling::ColorEncoding encoding,const SettingsSnapshot& settings){
    auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Runtime,"NR source preparation terminal; ownership retained");
    PerformanceScope performance(s.metrics,CpuPhase::Total);
    if(!settings.enabled){auto drained=s.WaitPending();if(!drained)return std::unexpected(drained.error());s.history.ResetNext();return s.bridge.Evaluate(input,settings);}
    if(!s.ready)return Fail(ErrorKind::InvalidInput,"NR source preparation not initialized");
    if(!Upscaling::IsKnownColorEncoding(encoding))return Fail(ErrorKind::InvalidInput,"NR source encoding unknown; explicit Linear/Gamma22/SRGB required");
    if(settings.placement!=Placement::Before||settings.reconstruction.preset!=s.preset||settings.reconstruction.inputScale!=1||
        settings.reconstruction.method>ResolveMethod::Ratio||EffectiveResolve(settings.reconstruction)!=ResolveMethod::Auto||
        settings.reconstruction.colorIsHDR||settings.reconstruction.producerColor||settings.reconstruction.fusedPreparation||settings.reconstruction.peripheralCompression)
        return Fail(ErrorKind::Unsupported,"NR first source adapter supports one native SDR Before pass");
    D3D11_TEXTURE2D_DESC color{},depth{},motion{};
    if(input.colorExtent!=s.extent||input.guideExtent!=s.extent||input.guideEpoch!=input.epoch||input.guideSourceId!=input.sourceId||
        !input.context||!D3D11FrameCopy::SameObject(input.context.Get(),s.context.Get())||input.context->GetType()!=D3D11_DEVICE_CONTEXT_IMMEDIATE||
        !s.Shape(input.color.Get(),color)||!s.Shape(input.depth.Get(),depth)||!s.Shape(input.motion.Get(),motion)||
        !Upscaling::SupportsFsrHandoffFormat(color.Format)||(color.BindFlags&(D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET))!=(D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET)||
        !(depth.BindFlags&D3D11_BIND_SHADER_RESOURCE)||(depth.Format!=DXGI_FORMAT_R32_TYPELESS&&depth.Format!=DXGI_FORMAT_R32_FLOAT&&depth.Format!=DXGI_FORMAT_R24G8_TYPELESS&&depth.Format!=DXGI_FORMAT_R32G8X24_TYPELESS)||motion.Format!=DXGI_FORMAT_R16G16_FLOAT)
        return Fail(ErrorKind::InvalidInput,"NR native source color/depth/motion ownership or shape invalid");
    ImagePacket p;p.epoch=input.epoch;p.sourceId=input.sourceId;p.previousSourceId=input.previousSourceId;p.imageId=p.batchId=input.sourceId;p.presentationTime=input.presentationTime;p.reset=input.reset;
    auto history=s.history.Check(p,settings);if(!history)return std::unexpected(history.error());
    auto index=s.Acquire();if(!index)return std::unexpected(index.error());auto& slot=s.slots[*index];slot.held=input;slot.busy=true;slot.readerValue=0;
    if(s.deliverySerial==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR preparation delivery sequence exhausted");}slot.id=++s.deliverySerial;slot.source=input.sourceId;slot.epoch=input.epoch;
    if(s.timing){s.timing->Begin11(input.context.Get(),input.sourceId);s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareColor,true);}
    auto r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareColor,[&]{return s.decode.Convert(input.context.Get(),input.color.Get(),slot.color.Get(),encoding,Upscaling::ColorEncoding::Linear);}),"NR explicit SDR source decoding failed");if(!r)return std::unexpected(r.error());
    if(s.timing){s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareColor,false);s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareGuides,true);}
    {
        D3D11ContextIsolation::Scope scope(s.isolation,input.context.Get());if(!scope){s.terminal=true;return Fail(ErrorKind::Runtime,"NR guide preparation state isolation failed");}
        r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareGuides,[&]{return s.depthCopy.Copy(input.context.Get(),input.depth.Get(),slot.depth.Get(),{s.extent.width,s.extent.height});}),"NR source depth preparation failed");if(!r)return std::unexpected(r.error());
        r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareGuides,[&]{return D3D11FrameCopy::Color(input.context.Get(),input.motion.Get(),slot.motion.Get(),{s.extent.width,s.extent.height});}),"NR source motion preparation failed");if(!r)return std::unexpected(r.error());
    }
    if(s.timing)s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareGuides,false);
    auto linear=input;linear.color=slot.color;linear.depth=slot.depth;linear.motion=slot.motion;linear.colorDomain=ColorDomain::Linear;linear.reset=history->Reset();
    auto result=s.bridge.Evaluate(linear,settings);if(!result){s.terminal=true;return std::unexpected(result.error());}
    slot.bridgeDelivery=*result;
    if(s.timing)s.timing->Stamp11(input.context.Get(),GpuPhase::Encode,true);
    r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::Encode,[&]{return s.encode.Convert(input.context.Get(),slot.color.Get(),input.color.Get(),Upscaling::ColorEncoding::Linear,encoding);}),"NR explicit SDR source delivery failed");if(!r)return std::unexpected(r.error());
    if(s.timing){s.timing->Stamp11(input.context.Get(),GpuPhase::Encode,false);s.timing->End11(input.context.Get());}
    if(s.readerSerial==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR preparation reader sequence exhausted");}slot.readerValue=++s.readerSerial;
    r=s.Gpu(s.context4->Signal(s.reader11.Get(),slot.readerValue),"NR preparation encoder reader signal failed");if(!r)return std::unexpected(r.error());
    r=s.bridge.TrackReader(slot.bridgeDelivery.delivery,s.readerFence.Get(),slot.readerValue);if(!r){s.terminal=true;return std::unexpected(r.error());}
    r=s.history.CommitRecorded(*history);if(!r){s.terminal=true;return std::unexpected(r.error());}
    DeliveryTicket delivery;delivery.owner_=s.deliveryOwner.Seal();delivery.id_=slot.id;delivery.source_=slot.source;delivery.epoch_=slot.epoch;delivery.slot_=*index;s.nextSlot=(*index+1)%s.slots.size();result->delivery=delivery;return *result;
}
Result<uint32_t> PreparedBeforeUpscale::CollectCompleted(){return state_->Collect();}
Result<void> PreparedBeforeUpscale::WaitDelivery(const BeforeResult& result){auto& s=*state_;if(!result.evaluated)return {};auto* slot=s.Find(result.delivery);if(!slot)return Fail(ErrorKind::InvalidInput,"NR prepared delivery ticket expired/foreign");auto drained=s.WaitPending(slot);if(!drained)return drained;return s.bridge.WaitDelivery(slot->bridgeDelivery);}
Result<void> PreparedBeforeUpscale::TrackReader(const DeliveryTicket& delivery,ID3D12Fence* fence,uint64_t value){auto& s=*state_;auto* slot=s.Find(delivery);if(s.terminal||!slot||!slot->busy)return Fail(ErrorKind::InvalidInput,"NR prepared reader delivery expired/foreign");auto result=s.bridge.TrackReader(slot->bridgeDelivery.delivery,fence,value);if(!result){s.terminal=true;return result;}slot->readers.push_back({fence,value});return {};}
Result<void> PreparedBeforeUpscale::Retire(){auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,"NR preparation terminal; no teardown retry");if(!s.ready)return {};
    auto drained=s.WaitPending();if(!drained)return drained;if(s.timing)s.timing->Collect11(s.context.Get());auto r=s.bridge.Retire();if(!r){s.terminal=true;return r;}s.ready=false;s.uncertain=false;return {};}
StageDiagnostics PreparedBeforeUpscale::Diagnostics()const{auto d=state_->bridge.Diagnostics();d.terminal|=state_->terminal;d.preparedSlots=state_->ready?uint32_t(state_->slots.size()):0;if(state_->timing){d.gpuTiming11Available=state_->timing->Available11();d.gpuTimingDropped+=state_->timing->Dropped();}return d;}
}
