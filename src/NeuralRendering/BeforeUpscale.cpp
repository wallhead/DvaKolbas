#include "BeforeUpscale.h"
#include "PerformanceQueries.h"
#include "History.h"
#include "Graphics/D3D11D3D12Interop.h"
#include <cmath>
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
    struct Slot {Graphics::SharedTexture color,depth,motion,output;BeforeInput heldInput;};
    std::array<Slot,3> slots;size_t nextSlot{};
    ComPtr<ID3D12Fence> handoffFence,completionFence;
    ComPtr<ID3D11Fence> handoff11;
    HANDLE event{};
    uint64_t handoffValue{},completionValue{};
    unsigned preset{};
    History history;
    bool attempted{},ready{},uncertain{},terminal{};
    ~State(){if(event)CloseHandle(event);}
    Result<void> Gpu(HRESULT hr,const char* text){if(FAILED(hr)){terminal=true;return Fail(ErrorKind::Runtime,text,hr);}return {};}
    Result<void> Wait(ID3D12Fence* fence,uint64_t value,CpuPhase phase=CpuPhase::ProducerWait){
        PerformanceScope timing(metrics,phase);
        auto completed=fence->GetCompletedValue();
        if(completed==UINT64_MAX){terminal=true;return Fail(ErrorKind::Retirement,"NR Before device removed at handoff fence");}
        if(completed<value){auto r=Gpu(fence->SetEventOnCompletion(value,event),"NR Before fence event registration failed");if(!r)return r;
            const auto begin=metrics?PerformanceNow():0;const auto waited=WaitForSingleObject(event,20000);
            if(metrics)metrics->RecordWait(phase,true,PerformanceNow()-begin);
            if(waited!=WAIT_OBJECT_0){terminal=true;return Fail(ErrorKind::Retirement,"NR Before handoff completion unconfirmed; ownership retained");}
            completed=fence->GetCompletedValue();}
        else if(metrics)metrics->RecordWait(phase,false,0);
        if(completed==UINT64_MAX||completed<value){terminal=true;return Fail(ErrorKind::Retirement,"NR Before handoff fence incomplete");}
        auto r=Gpu(contract.device->GetDeviceRemovedReason(),"NR Before D3D12 device removed");if(!r)return r;
        return Gpu(device11->GetDeviceRemovedReason(),"NR Before D3D11 device removed");
    }
    bool Texture(ID3D11Texture2D* texture,DXGI_FORMAT format)const {
        if(!texture)return false;ComPtr<ID3D11Device> device;texture->GetDevice(&device);
        D3D11_TEXTURE2D_DESC d{};texture->GetDesc(&d);
        return D3D11FrameCopy::SameObject(device.Get(),device11.Get())&&d.Format==format&&
            d.Width==contract.colorExtent.width&&d.Height==contract.colorExtent.height&&
            d.ArraySize==1&&d.MipLevels==1&&d.SampleDesc.Count==1&&!d.SampleDesc.Quality&&
            !(d.BindFlags&D3D11_BIND_DEPTH_STENCIL)&&d.Usage==D3D11_USAGE_DEFAULT;
    }
};
BeforeUpscale::BeforeUpscale():state_(std::make_unique<State>()){}
BeforeUpscale::~BeforeUpscale(){if(state_->uncertain)state_.release();}
Result<void> BeforeUpscale::Initialize(std::shared_ptr<RuntimeOwner> owner,ID3D11Device* device,const StageContract& contract,unsigned preset,PerformanceMetrics* metrics,PerformanceQueries* queries11){
    auto& s=*state_;if(s.attempted)return Fail(ErrorKind::Conflict,"NR Before initialization already attempted");s.attempted=true;
    if(!device||!owner||!contract.device||!contract.queue||!contract.colorExtent.width||!contract.colorExtent.height||
        contract.colorExtent!=contract.guideExtent||preset>1)return Fail(ErrorKind::InvalidInput,"NR Before native device/extent contract incomplete");
    auto r=s.Gpu(s.interop.Initialize(device,contract.device.Get(),contract.queue.Get()),"NR Before same-adapter bridge initialization failed");if(!r)return r;
    s.device11=device;s.contract=contract;s.preset=preset;device->GetImmediateContext(&s.context);
    if(metrics&&metrics->Enabled()){
        s.metrics=metrics;s.queries11=queries11;
        if(!queries11){s.ownTiming11=std::make_unique<PerformanceQueries>(metrics);s.ownTiming11->Initialize11(device);s.queries11=s.ownTiming11.get();}
        s.interop.SetPerformanceSink({metrics,[](void* p,bool blocked,uint64_t ns){static_cast<PerformanceMetrics*>(p)->RecordWait(CpuPhase::InteropWait,blocked,ns);},[](void* p){static_cast<PerformanceMetrics*>(p)->RecordFlush();}});
    }
    s.event=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!s.event)return Fail(ErrorKind::Io,"NR Before event creation failed",GetLastError());
    r=s.Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_SHARED,IID_PPV_ARGS(&s.handoffFence)),"NR Before shared handoff fence creation failed");if(!r)return r;
    ComPtr<ID3D11Device5> device5;r=s.Gpu(device->QueryInterface(IID_PPV_ARGS(&device5)),"NR Before requires D3D11 shared-fence device");if(!r)return r;
    HANDLE shared{};r=s.Gpu(contract.device->CreateSharedHandle(s.handoffFence.Get(),nullptr,GENERIC_ALL,nullptr,&shared),"NR Before shared fence handle failed");if(!r)return r;
    const auto opened=device5->OpenSharedFence(shared,IID_PPV_ARGS(&s.handoff11));CloseHandle(shared);
    r=s.Gpu(opened,"NR Before D3D11 fence open failed");if(!r)return r;
    r=s.Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&s.completionFence)),"NR Before completion fence creation failed");if(!r)return r;
    auto make=[&](DXGI_FORMAT format,Graphics::SharedTexture& texture,bool uav){
        D3D11_TEXTURE2D_DESC d{};d.Width=contract.colorExtent.width;d.Height=contract.colorExtent.height;
        d.MipLevels=d.ArraySize=d.SampleDesc.Count=1;d.Format=format;d.Usage=D3D11_USAGE_DEFAULT;
        // The accepted D3D12-to-D3D11 shared-open route needs RTV capability,
        // including input resources which this adapter only copies/samples.
        d.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE|(uav?D3D11_BIND_UNORDERED_ACCESS:0);
        const auto created=s.interop.CreateSharedTexture(d,texture);
        if(FAILED(created)) {s.terminal=true;return Result<void>{std::unexpected(Error{ErrorKind::Runtime,created,"NR Before shared texture creation failed, DXGI format="+std::to_string(unsigned(format))+" bind="+std::to_string(d.BindFlags)})};}
        return Result<void>{};};
    for(auto& slot:s.slots)for(auto [format,texture,uav]:{std::tuple{DXGI_FORMAT_R16G16B16A16_FLOAT,&slot.color,false},
        std::tuple{DXGI_FORMAT_R32_FLOAT,&slot.depth,false},std::tuple{DXGI_FORMAT_R16G16_FLOAT,&slot.motion,false},
        std::tuple{DXGI_FORMAT_R16G16B16A16_FLOAT,&slot.output,true}}){r=make(format,*texture,uav);if(!r)return r;}
    // Stage initialization can submit feature creation; never destroy this
    // bridge's retained devices/queue after an uncertain result.
    s.uncertain=true;r=s.stage.Initialize(std::move(owner),contract,preset,s.metrics);if(!r){s.terminal=true;return r;}
    s.ready=true;return {};
}
Result<BeforeResult> BeforeUpscale::Evaluate(const BeforeInput& input,const SettingsSnapshot& settings){
    auto& s=*state_;
    PerformanceScope performance(s.metrics,CpuPhase::Bridge);
    if(s.terminal)return Fail(ErrorKind::Runtime,"NR Before terminal; ownership retained");
    if(!settings.enabled){s.history.ResetNext();return BeforeResult{false,input.reset};}
    if(!s.ready)return Fail(ErrorKind::InvalidInput,"NR Before runtime/bridge not initialized");
    if(settings.placement!=Placement::Before)return Fail(ErrorKind::Unsupported,"NR Before cannot evaluate an After request");
    if(input.colorDomain!=ColorDomain::Linear||input.colorExtent!=s.contract.colorExtent||input.guideExtent!=s.contract.guideExtent||
        !input.context||input.context->GetType()!=D3D11_DEVICE_CONTEXT_IMMEDIATE||!D3D11FrameCopy::SameObject(input.context.Get(),s.context.Get())||
        !s.Texture(input.color.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT)||!s.Texture(input.depth.Get(),DXGI_FORMAT_R32_FLOAT)||!s.Texture(input.motion.Get(),DXGI_FORMAT_R16G16_FLOAT)||
        input.color.Get()==input.depth.Get()||input.color.Get()==input.motion.Get()||input.depth.Get()==input.motion.Get()||
        input.guideEpoch!=input.epoch||input.guideSourceId!=input.sourceId||!std::isfinite(input.motionScaleX)||!std::isfinite(input.motionScaleY)||!input.motionScaleX||!input.motionScaleY)
        return Fail(ErrorKind::InvalidInput,"NR Before input ownership/encoding/native guides invalid");
    if(settings.reconstruction.preset!=s.preset||settings.reconstruction.inputScale!=1||settings.reconstruction.colorIsHDR||settings.reconstruction.producerColor||settings.reconstruction.peripheralCompression||settings.reconstruction.fusedPreparation||
        settings.reconstruction.method>ResolveMethod::Ratio||EffectiveResolve(settings.reconstruction)!=ResolveMethod::Auto)
        return Fail(ErrorKind::Unsupported,"NR Before native bridge requires adapter-owned color/reconstruction preparation");
    ImagePacket p;p.epoch=input.epoch;p.guideEpoch=input.guideEpoch;p.sourceId=p.batchId=p.imageId=input.sourceId;
    p.guideSourceId=input.guideSourceId;p.previousSourceId=input.previousSourceId;p.presentationTime=input.presentationTime;
    p.kind=ImageKind::Real;p.interpolationFraction=1.;p.colorDomain=input.colorDomain;p.guideOrigin=GuideOrigin::RealSource;
    p.colorExtent=input.colorExtent;p.guideExtent=input.guideExtent;p.motionScaleX=input.motionScaleX;p.motionScaleY=input.motionScaleY;
    p.reset=input.reset;p.depthInverted=input.depthInverted;
    auto history=s.history.Check(p,settings);if(!history)return std::unexpected(history.error());
    auto& slot=s.slots[s.nextSlot];slot.heldInput=input;
    auto gpu=[&](HRESULT hr,const char* text)->Result<void>{return s.Gpu(hr,text);};
    if(s.ownTiming11)s.queries11->Begin11(input.context.Get(),input.sourceId);
    if(s.queries11)s.queries11->Stamp11(input.context.Get(),GpuPhase::InputCopy,true);
    auto r=gpu(MeasurePerformance(s.metrics,CpuPhase::InputCopy,[&]{return s.interop.CopyInput(input.color.Get(),slot.color);}),"NR Before color copy rejected");if(!r)return std::unexpected(r.error());
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::InputCopy,[&]{return s.interop.CopyInput(input.depth.Get(),slot.depth);}),"NR Before depth copy rejected");if(!r)return std::unexpected(r.error());
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::InputCopy,[&]{return s.interop.CopyInput(input.motion.Get(),slot.motion);}),"NR Before motion copy rejected");if(!r)return std::unexpected(r.error());
    if(s.queries11)s.queries11->Stamp11(input.context.Get(),GpuPhase::InputCopy,false);
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::ProducerSignal,[&]{return s.interop.SignalProducer();}),"NR Before producer submission failed");if(!r)return std::unexpected(r.error());
    r=gpu(s.interop.ProducerDependency(&p.producerFence,&p.producerFenceValue),"NR Before submitted producer dependency missing");if(!r)return std::unexpected(r.error());
    ID3D12GraphicsCommandList* list{};r=gpu(MeasurePerformance(s.metrics,CpuPhase::Begin,[&]{return s.interop.Begin(&list);}),"NR Before command recording begin failed");if(!r)return std::unexpected(r.error());
    constexpr auto read=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    for(auto* resource:{slot.color.texture12.Get(),slot.depth.texture12.Get(),slot.motion.texture12.Get()})Transition(list,resource,D3D12_RESOURCE_STATE_COMMON,read);
    Transition(list,slot.output.texture12.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    p.color=slot.color.texture12;p.depth=slot.depth.texture12;p.motion=slot.motion.texture12;p.output=slot.output.texture12;
    p.colorState=p.depthState=p.motionState=read;p.outputState=D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    p.reset=history->Reset();auto ticket=s.stage.RecordQueued(list,p,settings);
    if(!ticket){s.terminal=true;return std::unexpected(ticket.error());}
    r=s.history.CommitRecorded(*history);if(!r){s.terminal=true;return std::unexpected(r.error());}
    for(auto* resource:{slot.color.texture12.Get(),slot.depth.texture12.Get(),slot.motion.texture12.Get()})Transition(list,resource,read,D3D12_RESOURCE_STATE_COMMON);
    Transition(list,slot.output.texture12.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_COMMON);
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::Submit,[&]{return s.interop.Submit();}),"NR Before recorded list submission failed");if(!r)return std::unexpected(r.error());
    if(s.completionValue==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR Before completion sequence exhausted");}
    r=s.stage.MarkSubmitted(*ticket,s.completionFence.Get(),++s.completionValue);if(!r){s.terminal=true;return std::unexpected(r.error());}
    r=gpu(s.interop.WaitConsumer(),"NR Before D3D11 consumer wait failed");if(!r)return std::unexpected(r.error());
    if(s.queries11)s.queries11->Stamp11(input.context.Get(),GpuPhase::Delivery,true);
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::Delivery,[&]{return D3D11FrameCopy::Color(input.context.Get(),slot.output.texture11.Get(),input.color.Get(),{input.colorExtent.width,input.colorExtent.height});}),"NR Before copy back failed");if(!r)return std::unexpected(r.error());
    if(s.queries11){s.queries11->Stamp11(input.context.Get(),GpuPhase::Delivery,false);if(s.ownTiming11)s.queries11->End11(input.context.Get());}
    // Queue a real D3D11 signal after the copy reading the shared NR output.
    // Stage retains the source and output through that signal's completion.
    if(s.handoffValue==UINT64_MAX){s.terminal=true;return Fail(ErrorKind::Runtime,"NR Before consumer sequence exhausted");}
    r=gpu(s.interop.Context11()->Signal(s.handoff11.Get(),++s.handoffValue),"NR Before output reader signal failed");if(!r)return std::unexpected(r.error());
    r=s.stage.TrackReader(*ticket,s.handoffFence.Get(),s.handoffValue);if(!r){s.terminal=true;return std::unexpected(r.error());}
    s.interop.Context11()->Flush();if(s.metrics)s.metrics->RecordFlush();r=s.Wait(s.handoffFence.Get(),s.handoffValue,CpuPhase::ConsumerWait);if(!r)return std::unexpected(r.error());
    r=s.Wait(s.completionFence.Get(),s.completionValue,CpuPhase::CompletionWait);if(!r)return std::unexpected(r.error());
    r=gpu(MeasurePerformance(s.metrics,CpuPhase::Drain,[&]{return s.interop.Drain();}),"NR Before allocator/consumer retirement failed");if(!r)return std::unexpected(r.error());
    r=s.stage.RetireTicket(*ticket);if(!r){s.terminal=true;return std::unexpected(r.error());}
    slot.heldInput={};s.nextSlot=(s.nextSlot+1)%s.slots.size();return BeforeResult{true,history->Reset()};
}
Result<void> BeforeUpscale::Retire(){
    auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,"NR Before terminal; no teardown retry");
    if(!s.ready)return {};
    auto r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::Drain,[&]{return s.interop.Drain();}),"NR Before final consumer retirement failed");if(!r)return r;
    if(s.ownTiming11)s.queries11->Collect11(s.context.Get());
    r=s.stage.Retire();if(!r){s.terminal=true;return r;}s.ready=false;s.uncertain=false;return {};
}
StageDiagnostics BeforeUpscale::Diagnostics()const{auto d=state_->stage.Diagnostics();d.bridgeSlots=state_->ready?uint32_t(state_->slots.size()):0;if(state_->ownTiming11){d.gpuTiming11Available=state_->queries11->Available11();d.gpuTimingDropped+=state_->queries11->Dropped();}return d;}
}
