#include "BeforeHost.h"
#include "RuntimeFileLease.h"
#include "FrameGen/D3D11FrameCopy.h"
#include <dxgi1_6.h>
#include <optional>
namespace TheosRenderPipeline::NeuralRendering {
namespace {using Microsoft::WRL::ComPtr;
std::unexpected<Error> Fail(ErrorKind k,const char* message,int64_t native=0){return std::unexpected(Error{k,native,message});}}
struct BeforeHost::State {
    StartupSettings settings;std::filesystem::path cache,nrFile;
    AdapterIdentity adapter;ComPtr<IDXGIAdapter1> dxgiAdapter;ComPtr<ID3D11Device> device11;
    StageContract contract;const RuntimeProfile* profile{};
    std::optional<RuntimeFileLease> runtimeLease,coreLease;
    std::shared_ptr<RuntimeOwner> owner;std::unique_ptr<PreparedBeforeUpscale> prepared;std::unique_ptr<PostUpscale> post;Placement placement{Placement::Before};
    bool inspected{},available{},terminal{},uncertain{},active{},initializationBypassed{};
    uint64_t recorded{},resets{};
    int passes{1};
    std::string status{"NR off"};
    bool HasPreparation()const{return bool(prepared)||bool(post);}
    Result<void> RetirePreparation(){auto r=post?post->Retire():prepared?prepared->Retire():Result<void>{};if(r){post.reset();prepared.reset();}return r;}
    Result<BeforeResult> Run(const BeforeInput& input,const SettingsSnapshot& snapshot,PreparedFsrInput* output,const PostSrSourceContract* metadata){
        if(post){PostSrInput frame;frame.resources=input;if(metadata)frame.source=*metadata;return post->Evaluate(frame,snapshot);}
        return prepared->Evaluate(input,settings.sourceEncoding,snapshot,settings.sdrBytesTrial?nullptr:output);
    }
    void MarkTerminal(const Error& error){status=error.message;active=false;terminal=true;}
    Result<void> DisableAfterPreparationFailure(const Error& error){
        const bool rolledBack=post?post->InitializationRolledBackBeforeCreate():prepared&&prepared->InitializationRolledBackBeforeCreate();
        if(!rolledBack||!device11||!contract.device||!owner){MarkTerminal(error);return std::unexpected(error);}
        auto hr=device11->GetDeviceRemovedReason();if(SUCCEEDED(hr))hr=contract.device->GetDeviceRemovedReason();
        if(FAILED(hr)){Error removed{ErrorKind::Runtime,hr,"NR device removed during preparation initialization rollback"};MarkTerminal(removed);return std::unexpected(removed);}
        auto safe=owner->CheckStageInitializationFallbackSafety();if(!safe){MarkTerminal(safe.error());return safe;}
        auto retired=owner->Retire();if(!retired){MarkTerminal(retired.error());return retired;}
        // Prior preparations have already retired; this failed preparation never
        // created a feature or submitted work. Keep the session unavailable.
        post.reset();prepared.reset();owner.reset();uncertain=false;available=false;active=false;
        status=error.message+" (native="+std::to_string(error.nativeCode)+"); NR unavailable for this session; source upscaling continues";return {};
    }
    Result<void> DisableAfterOpenFailure(const Error& error){
        // This boundary precedes preparation, feature creation and NR frames.
        // A partial Init_Ext owner remains untouched for the process lifetime.
        if(HasPreparation()||recorded||active){MarkTerminal(error);return std::unexpected(error);}
        const auto hr=contract.device->GetDeviceRemovedReason();
        if(FAILED(hr)){Error removed{ErrorKind::Runtime,hr,"NR device removed during initialization"};MarkTerminal(removed);return std::unexpected(removed);}
        const auto disposition=owner->OpenDisposition();
        if(disposition==RuntimeOpenDisposition::InitializationQuarantined){
            auto safe=owner->CheckInitializationFallbackSafety();
            if(!safe){MarkTerminal(safe.error());return safe;}
            initializationBypassed=true;
        }else if(disposition==RuntimeOpenDisposition::Unavailable){
            auto retired=owner->Retire();
            if(!retired){MarkTerminal(error);return std::unexpected(error);}
            owner.reset();uncertain=false;
        }else{MarkTerminal(error);return std::unexpected(error);}
        available=false;active=false;status=error.message+"; NR unavailable for this session; source upscaling continues";return {};
    }
};
BeforeHost::BeforeHost():state_(std::make_unique<State>()){}
BeforeHost::~BeforeHost(){if(state_->uncertain)state_.release();}
Result<void> BeforeHost::Inspect(ID3D11Device* device,const StartupSettings& settings,const std::filesystem::path& cache,ID3D12Device* presenter){
    auto& s=*state_;if(s.inspected)return s.available?Result<void>{}:Fail(ErrorKind::Unsupported,s.status.c_str());s.inspected=true;
    auto stop=[&](ErrorKind kind,const char* why,int64_t native=0)->Result<void>{s.status=why;return Fail(kind,why,native);};
    if(!settings.community||!device||!settings.runtimeRoot.is_absolute()||!settings.driverCore.is_absolute()||!cache.is_absolute())
        return stop(ErrorKind::InvalidInput,"Community NR requires controlled absolute runtime/core/cache paths");
    if(!Upscaling::IsKnownColorEncoding(settings.sourceEncoding))return stop(ErrorKind::Unsupported,"Community NR source color encoding is unknown");
    if(settings.sdrBytesTrial&&settings.sourceEncoding==Upscaling::ColorEncoding::Linear)
        return stop(ErrorKind::Unsupported,"NR SDR byte trial requires Gamma22 or SRGB source encoding");
    s.settings=settings;s.cache=cache;s.device11=device;ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;DXGI_ADAPTER_DESC1 d{};
    auto hr=device->QueryInterface(IID_PPV_ARGS(&dxgi));if(FAILED(hr))return stop(ErrorKind::IdentityMismatch,"NR cannot inspect renderer DXGI device",hr);
    if(FAILED(hr=dxgi->GetAdapter(&adapter))||FAILED(hr=adapter.As(&s.dxgiAdapter))||FAILED(hr=s.dxgiAdapter->GetDesc1(&d)))return stop(ErrorKind::IdentityMismatch,"NR renderer adapter unavailable",hr);
    s.adapter={d.VendorId,d.DeviceId,d.SubSysId,{d.AdapterLuid.LowPart,d.AdapterLuid.HighPart},bool(d.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)};
    if(presenter){
        const auto luid=presenter->GetAdapterLuid();
        if(AdapterLuid{luid.LowPart,luid.HighPart}!=s.adapter.luid)
            return stop(ErrorKind::IdentityMismatch,"NR presenter device does not match renderer adapter");
        if(FAILED(hr=presenter->GetDeviceRemovedReason()))return stop(ErrorKind::IdentityMismatch,"NR presenter device is removed",hr);
        s.contract.device=presenter;
    }
    const auto family=ClassifyGpu(d.VendorId,d.DeviceId,s.adapter.software);
    if(family==GpuFamily::AmdUnsupported||settings.profile=="amd-unsupported")return stop(ErrorKind::Unsupported,"AMD 6000/7000/9000 NR is unsupported");
    for(const auto& p:RuntimeCatalog())if((settings.profile=="Auto"&&(p.primaryFamily==family||(p.includeRtx30&&family==GpuFamily::Rtx30)))||p.id==settings.profile){s.profile=&p;break;}
    if(!s.profile)return stop(ErrorKind::Unsupported,"NR GPU/profile is absent from the reviewed catalog");
    // Validate family before leasing/loading the requested model.
    if(s.profile->primaryFamily!=family&&!(s.profile->includeRtx30&&family==GpuFamily::Rtx30))return stop(ErrorKind::Unsupported,"NR profile does not match the actual renderer GPU family");
    auto path=RuntimePath(settings.runtimeRoot,*s.profile);if(!path){s.status=path.error().message;return std::unexpected(path.error());}s.nrFile=*path;
    auto lease=RuntimeFileLease::Open(s.nrFile,*s.profile);if(!lease){s.status=lease.error().message;return std::unexpected(lease.error());}s.runtimeLease=std::move(*lease);
    auto core=RuntimeFileLease::Open(settings.driverCore,QualifiedProbeDriverCore());if(!core){s.status=core.error().message;return std::unexpected(core.error());}s.coreLease=std::move(*core);
    const ArtifactIdentity verified{s.profile->id,true,s.runtimeLease->Valid()};const auto selected=SelectRuntime(s.adapter,settings.profile,{&verified,1});
    if(!selected.profile)return stop(ErrorKind::Unsupported,selected.reason.c_str());
    s.available=true;s.status="NR ready for first real-world frame; GPU profile="+std::string(s.profile->id);return {};
}
Result<BeforeResult> BeforeHost::Evaluate(const BeforeInput& input,const SettingsSnapshot& settings,PreparedFsrInput* linearOutput){
    return EvaluateSource(input,settings,linearOutput,nullptr);
}
Result<BeforeResult> BeforeHost::EvaluatePost(const PostSrInput& input,const SettingsSnapshot& settings){
    return EvaluateSource(input.resources,settings,nullptr,&input.source);
}
Result<BeforeResult> BeforeHost::EvaluateSource(const BeforeInput& input,const SettingsSnapshot& settings,PreparedFsrInput* linearOutput,const PostSrSourceContract* metadata){
    auto& s=*state_;const bool wasActive=s.active;s.active=false;if(s.terminal)return Fail(ErrorKind::Runtime,s.status.c_str());
    if(s.initializationBypassed){auto safe=s.owner->CheckInitializationFallbackSafety();if(!safe){s.MarkTerminal(safe.error());return std::unexpected(safe.error());}}
    if(!settings.enabled){if(s.available)s.status="NR off";if(s.HasPreparation()){auto result=s.Run(input,settings,nullptr,metadata);if(result)result->effectiveReset|=wasActive;return result;}return BeforeResult{false,input.reset};}
    if(!s.available)return BeforeResult{false,input.reset};
    if(settings.passes<1||settings.passes>3||settings.placement!=(metadata?Placement::After:Placement::Before)||settings.reconstruction.preset!=0||settings.reconstruction.inputScale!=1||EffectiveResolve(settings.reconstruction)!=ResolveMethod::Auto||settings.reconstruction.method>ResolveMethod::Ratio||
        settings.reconstruction.colorIsHDR||settings.reconstruction.producerColor||settings.reconstruction.peripheralCompression||settings.reconstruction.fusedPreparation){s.status="Requested NR stage/reconstruction is unavailable in this native source trial";if(s.HasPreparation()){auto off=settings;off.enabled=false;auto drained=s.Run(input,off,nullptr,metadata);if(!drained){s.MarkTerminal(drained.error());return std::unexpected(drained.error());}}return BeforeResult{false,wasActive||input.reset};}
    if(metadata){
        const auto valid=ValidatePostSrSourceContract(*metadata);
        if(!s.settings.sdrBytesTrial || metadata->colorDomain!=ColorDomain::SdrBytes || metadata->encoding!=s.settings.sourceEncoding || !valid){
            s.status=!valid?valid.error().message:"Post-upscale NR requires the qualified encoded SDR byte route";
            if(s.HasPreparation()){auto off=settings;off.enabled=false;auto drained=s.Run(input,off,nullptr,metadata);if(!drained){s.MarkTerminal(drained.error());return std::unexpected(drained.error());}}
            return BeforeResult{false,wasActive||input.reset};
        }
    }
    if(!input.context||!input.color||!input.depth||!input.motion||!input.colorExtent.width||!input.colorExtent.height||input.colorExtent!=input.guideExtent||!input.epoch||!input.sourceId||input.guideSourceId!=input.sourceId||input.guideEpoch!=input.epoch){
        s.status="NR waiting for matching real-world guides";if(s.HasPreparation()){auto off=settings;off.enabled=false;auto reset=s.Run(input,off,nullptr,metadata);if(!reset){s.MarkTerminal(reset.error());return std::unexpected(reset.error());}}return BeforeResult{false,wasActive||input.reset};}
    if(s.HasPreparation()&&(s.contract.colorExtent!=input.colorExtent||s.placement!=settings.placement||s.passes!=settings.passes)){
        auto retired=s.RetirePreparation();if(!retired){s.MarkTerminal(retired.error());return std::unexpected(retired.error());}s.prepared.reset();s.post.reset();
    }
    if(!s.owner){
        s.contract.adapterLuid=s.adapter.luid;
        auto hr=S_OK;
        if(!s.contract.device)hr=D3D12CreateDevice(s.dxgiAdapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&s.contract.device));
        if(FAILED(hr)){s.status="NR D3D12 device unavailable; source upscaling continues";s.available=false;return BeforeResult{false,wasActive||input.reset};}
        D3D12_COMMAND_QUEUE_DESC q{};hr=s.contract.device->CreateCommandQueue(&q,IID_PPV_ARGS(&s.contract.queue));if(FAILED(hr)){s.status="NR DIRECT queue unavailable; source upscaling continues";s.available=false;return BeforeResult{false,wasActive||input.reset};}
        s.owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{s.nrFile,s.settings.driverCore,s.cache,s.profile->compatibility==CompatibilityPolicy::CallerIdentityProbeRequired});s.uncertain=true;
        auto opened=s.owner->Open(*s.profile,s.contract.device.Get(),s.adapter);
        if(!opened){auto disabled=s.DisableAfterOpenFailure(opened.error());if(!disabled)return std::unexpected(disabled.error());return BeforeResult{false,wasActive||input.reset};}
    }
    if(!s.HasPreparation()){
        s.contract.colorExtent=s.contract.guideExtent=input.colorExtent;s.placement=settings.placement;s.passes=settings.passes;
        Result<void> initialized;
        if(metadata){s.post=std::make_unique<PostUpscale>();initialized=s.post->Initialize(s.owner,s.device11.Get(),s.contract,0,nullptr,ColorDomain::SdrBytes,unsigned(s.passes));}
        else{s.prepared=std::make_unique<PreparedBeforeUpscale>();initialized=s.prepared->Initialize(s.owner,s.device11.Get(),s.contract,0,nullptr,
            s.settings.sdrBytesTrial?ColorDomain::SdrBytes:ColorDomain::Linear,Placement::Before,unsigned(s.passes));}
        if(!initialized){auto disabled=s.DisableAfterPreparationFailure(initialized.error());if(!disabled)return std::unexpected(disabled.error());return BeforeResult{false,wasActive||input.reset};}
    }
    if(s.settings.sdrBytesTrial&&linearOutput&&linearOutput->Valid())
        return Fail(ErrorKind::InvalidInput,"NR SDR byte trial cannot replace a retained FSR lease");
    // Encoded trial delivery is decoded by the existing FSR source path. Never
    // label encoded RGBA8 as a prepared linear FSR lease.
    auto result=s.Run(input,settings,linearOutput,metadata);
    if(!result){if(result.error().kind==ErrorKind::InvalidInput||result.error().kind==ErrorKind::Unsupported){s.status=result.error().message;auto off=settings;off.enabled=false;auto reset=s.Run(input,off,nullptr,metadata);if(reset)return BeforeResult{false,wasActive||input.reset};}
        s.MarkTerminal(result.error());return std::unexpected(result.error());}
    s.active=result->evaluated;if(s.active){++s.recorded;s.resets+=result->effectiveReset;s.status=std::string(metadata?"NR active after upscaling, before FG; ":"NR active before upscaling; ")+std::string(s.profile->id)+
        "; "+std::to_string(s.passes)+(s.settings.sdrBytesTrial?" SDR RGBA8 byte trial ":" native SDR ")+(s.passes==1?"pass":"passes");}return *result;
}
Result<void> BeforeHost::Retire(){auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,s.status.c_str());
    if(s.initializationBypassed){
        auto safe=s.owner->CheckInitializationFallbackSafety();if(!safe){s.MarkTerminal(safe.error());return safe;}
        s.available=false;s.active=false;return {};
    }
    if(s.HasPreparation()){auto r=s.RetirePreparation();if(!r){s.MarkTerminal(r.error());return r;}s.prepared.reset();}
    if(s.owner){auto r=s.owner->Retire();if(!r){s.MarkTerminal(r.error());return r;}s.owner.reset();}
    s.uncertain=false;s.available=false;s.active=false;s.status="NR retired";return {};
}
bool BeforeHost::Available()const{return state_->available&&!state_->terminal;}
bool BeforeHost::Terminal()const{return state_->terminal;}
bool BeforeHost::Active()const{return state_->active;}
std::string_view BeforeHost::ProfileId()const{return state_->profile?state_->profile->id:std::string_view{};}
const std::string& BeforeHost::Status()const{return state_->status;}
uint64_t BeforeHost::Recorded()const{return state_->recorded;}
uint64_t BeforeHost::Resets()const{return state_->resets;}
}
