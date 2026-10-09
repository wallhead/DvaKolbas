// Execute the production host operations with SDK/bridge boundaries replaced
// by CPU witnesses. Actual SDK admission/resumption is covered by XessTemporalGpu.
#include "Upscaling/XessUpscaler.h"
#include "Upscaling/XessRepeatPolicy.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
#define TRP_ENABLE_FSR_FG
using namespace TheosRenderPipeline::Upscaling;
namespace logger { template<class... T> void warn(T&&...){} template<class... T> void error(T&&...){} }
struct RenderPipeline { int mGraphicsState{};bool mEnableJitter{}; };
static Result<CameraMeasurements> CaptureGameCameraMeasurements(int,Extent,bool,bool){return CameraMeasurements{};}
struct Bridge {
    unsigned signals{},begins{},submissions{},discards{};
    HRESULT SignalProducer(){++signals;return S_OK;}
    HRESULT Begin(ID3D12GraphicsCommandList**){++begins;return S_OK;}
    HRESULT Submit(){++submissions;return S_OK;}
    HRESULT WaitConsumer(){return S_OK;}
    HRESULT DiscardRecording(){++discards;return S_OK;}
};
struct HostResources {
    struct Owner {
        unsigned dispatches{};bool vendorFailure{};
        DWORD OwnerThread(){return 1;}
        Result<void> Dispatch(ID3D12GraphicsCommandList*,int,const UpscaleFrame&){
            ++dispatches;
            if(vendorFailure)return std::unexpected(RuntimeError{ErrorKind::DispatchFailure,-1,"vendor failure"});
            return {};
        }
    } owner;
    std::shared_ptr<::Bridge> bridge=std::make_shared<::Bridge>();
    unsigned preparations{},retirements{},sharpenings{};bool sharpeningFailure{};float lastSharpness{};
    Result<void> SharpenOutput(ID3D11DeviceContext*,ID3D11Texture2D*,float strength){
        ++sharpenings;lastSharpness=strength;
        if(sharpeningFailure)return std::unexpected(RuntimeError{ErrorKind::DispatchFailure,-1,"sharp failure"});
        return {};
    }
    Result<void> PrepareInput(ID3D11Texture2D*,ID3D11Texture2D*,ID3D11Texture2D*){++preparations;return {};}
    auto Bridge(){return bridge;}
    Owner* Upscaler(){return &owner;}
    int Resources(){return 0;}
    ID3D11Texture2D* Output11(){return nullptr;}
    Result<void> Retire(){++retirements;return {};}
};
struct NvidiaHost {
    struct SourceXessEvaluationOperations {
        NvidiaHost& host;RenderPipeline& pipeline;
        bool menu{};XessFrameAdmission admission{XessFrameAdmission::Ready};
        bool reuseCompleted{};
        std::optional<RuntimeError> error;
        Result<UpscaleOutcome> Spatial(const UpscaleFrame&,const std::string& reason){++host.spatial;host.xessRecoveryReason_=reason;return UpscaleOutcome::SpatialRecovery;}
        Result<UpscaleOutcome> EvaluateUpscaler(UpscaleFrame&);
        bool NeuralEligible(){return true;}
        bool EvaluateOptionalPreUpscale(UpscaleFrame&);
        bool EvaluateOptionalPostUpscale(UpscaleFrame&,UpscaleOutcome);
        GenerationPreparationStatus PrepareGeneration(const UpscaleFrame&);
    };
    bool xessRecovery_{};unsigned xessDeferredReported_{},spatial{},nrCalls{};
    std::uint64_t xessDuplicateCount_{},xessForeignThreadCount_{},xessRepeatedCount_{},xessEpoch_{1},presentCount_{20};
    UpscaleFrame xessCompletedFrame_{};
    std::uint64_t communityEpoch_{1};std::string xessRecoveryReason_;
    bool fg{};UpscaleFrame fsrGenerationFrame_{};
    bool FsrFgActive()const{return fg;}
    std::unique_ptr<HostResources> xessResources_=std::make_unique<HostResources>();
    struct Context { ID3D11DeviceContext* Get(){return nullptr;} } context_;
    struct Encoder { HRESULT Convert(ID3D11DeviceContext*,ID3D11Texture2D*,ID3D11Texture2D*,ColorEncoding,ColorEncoding){return S_OK;} } xessEncode_;
    struct Settings { struct Value { struct XeSS {ColorEncoding sourceEncoding{ColorEncoding::Gamma22};float sharpness{};} xess; } value;const Value& Effective(){return value;} } sourceUpscalerSettings_;
    bool EvaluateCommunityNeuralBefore(ID3D11Texture2D*,ID3D11Texture2D*,ID3D11Texture2D*,unsigned,unsigned,std::uint64_t,bool&,bool){++nrCalls;return true;}
    bool EvaluateCommunityNeuralAfter(UpscaleFrame&,UpscaleOutcome,bool){++nrCalls;return true;}
};
#include "XessHostOperations.inc"
static void Require(bool value,const char* message){if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
int main(){
    RenderPipeline pipeline;UpscaleFrame frame{};frame.depth=frame.motion=reinterpret_cast<ID3D11Texture2D*>(1);
    frame.sourceId=10;frame.sourceEpoch=1;frame.render={640,360};
    for(auto deferred:{XessFrameAdmission::DuplicateSource,XessFrameAdmission::OffOwnerThread}){
        NvidiaHost host;NvidiaHost::SourceXessEvaluationOperations operations{host,pipeline,false,deferred};
        Require(operations.EvaluateOptionalPreUpscale(frame),"deferred Before handoff succeeds");
        const auto result=operations.EvaluateUpscaler(frame);
        Require(result && *result==UpscaleOutcome::SpatialRecovery,"deferred source produces one spatial frame");
        Require(operations.EvaluateOptionalPostUpscale(frame,*result),"deferred After handoff succeeds");
        Require(!host.nrCalls && !host.xessResources_->preparations && !host.xessResources_->owner.dispatches &&
            !host.xessResources_->bridge->signals && !host.xessResources_->retirements && !host.xessRecovery_,
            "duplicate/worker frame touches no NR, shared inputs, SDK or retirement and does not latch recovery");
        operations.admission=XessFrameAdmission::Ready;frame.reset=true;
        Require(operations.EvaluateOptionalPreUpscale(frame),"owner Before NR resumes");
        const auto next=operations.EvaluateUpscaler(frame);
        Require(next && *next==UpscaleOutcome::Temporal && operations.EvaluateOptionalPostUpscale(frame,*next),"next new owner-thread source resumes temporal and After NR");
        Require(host.nrCalls==2 && host.xessResources_->owner.dispatches==1 && host.xessResources_->bridge->submissions==1,
            "resumption executes NR and submits exactly one source");
    }
    NvidiaHost failed;failed.xessResources_->owner.vendorFailure=true;
    NvidiaHost::SourceXessEvaluationOperations operations{failed,pipeline};
    const auto result=operations.EvaluateUpscaler(frame);
    Require(result && *result==UpscaleOutcome::SpatialRecovery && failed.xessRecovery_ && failed.xessResources_->retirements==1 &&
        failed.xessResources_->bridge->discards==1 && !failed.xessResources_->bridge->submissions,
        "real SDK failure still discards unsubmitted work, retires and latches recovery");
    const auto again=operations.EvaluateUpscaler(frame);
    Require(again && *again==UpscaleOutcome::SpatialRecovery && failed.xessResources_->owner.dispatches==1,
        "terminal recovery never reexecutes a failed SDK context");
    NvidiaHost generated;generated.fg=true;
    NvidiaHost::SourceXessEvaluationOperations prepare{generated,pipeline};
    frame.camera.identity=37;frame.reset=true;frame.sourceEpoch=9;
    Require(prepare.PrepareGeneration(frame)==GenerationPreparationStatus::Succeeded &&
        generated.fsrGenerationFrame_.camera.identity==37 && generated.fsrGenerationFrame_.sourceEpoch==9 && generated.fsrGenerationFrame_.reset,
        "FG receives completed XeSS/NR frame including measured camera and reset");
    generated.fg=false;
    Require(prepare.PrepareGeneration(frame)==GenerationPreparationStatus::NotRequested,"ordinary XeSS does not prepare FG");
    frame.reset=false;
    NvidiaHost cached;cached.fg=true;cached.xessCompletedFrame_=frame;
    Require(CanRepeatXessOutput(frame,cached.xessCompletedFrame_,true,true,false,false),"same completed source can be repeated");
    for(unsigned change=0;change<10;++change) {
        auto invalid=frame;
        if(change==0)++invalid.sourceId;
        if(change==1)++invalid.sourceEpoch;
        if(change==2)++invalid.render.width;
        if(change==3)++invalid.display.width;
        if(change==4)invalid.reset=true;
        if(change==5)invalid.output=reinterpret_cast<ID3D11Texture2D*>(9);
        Require(!CanRepeatXessOutput(invalid,cached.xessCompletedFrame_,change!=6,change!=7,change==8,change==9),
            "new source/epoch/extent/allocation, reset, invalid cache, thread, menu or recovery cannot reuse stale output");
    }
    cached.xessCompletedFrame_.camera.identity=91;
    NvidiaHost::SourceXessEvaluationOperations repeat{cached,pipeline,false,XessFrameAdmission::DuplicateSource,true};
    const auto reused=repeat.EvaluateUpscaler(frame);
    Require(reused && *reused==UpscaleOutcome::RepeatedOutput && !cached.spatial && !cached.nrCalls &&
        !cached.xessResources_->owner.dispatches && !cached.xessResources_->bridge->signals,
        "completed duplicate reuses final NR/effects image without spatial or SDK work");
    Require(cached.fsrGenerationFrame_.camera.identity==91 && cached.fsrGenerationFrame_.sourceId==21 &&
        !cached.fsrGenerationFrame_.reset && !cached.fsrGenerationFrame_.camera.reset,
        "repeat preserves completed camera and maps fresh transport sequence without a source reset");
    repeat.admission=XessFrameAdmission::Ready;repeat.reuseCompleted=false;frame.reset=false;++frame.sourceId;
    const auto resumed=repeat.EvaluateUpscaler(frame);
    Require(resumed && *resumed==UpscaleOutcome::Temporal && !frame.reset && cached.xessResources_->owner.dispatches==1,
        "new source after cached repeat dispatches with reset false");
    NvidiaHost sharpHost;sharpHost.fg=true;sharpHost.sourceUpscalerSettings_.value.xess.sharpness=.6f;
    NvidiaHost::SourceXessEvaluationOperations sharpOps{sharpHost,pipeline,false,XessFrameAdmission::Ready};
    Require(sharpOps.EvaluateOptionalPostUpscale(frame,UpscaleOutcome::Temporal) && sharpHost.nrCalls==1 &&
        sharpHost.xessResources_->sharpenings==1 && sharpHost.xessResources_->lastSharpness==.6f,"temporal XeSS output sharpened after NR");
    Require(sharpOps.PrepareGeneration(frame)==GenerationPreparationStatus::Succeeded && sharpHost.fsrGenerationFrame_.sharpness==.6f,
        "FG receives completed sharpening metadata");
    Require(sharpOps.EvaluateOptionalPostUpscale(frame,UpscaleOutcome::SpatialRecovery) && sharpHost.xessResources_->sharpenings==1,
        "spatial recovery does not sharpen");
    sharpOps.admission=XessFrameAdmission::DuplicateSource;
    Require(sharpOps.EvaluateOptionalPostUpscale(frame,UpscaleOutcome::RepeatedOutput) && sharpHost.xessResources_->sharpenings==1,
        "completed repeats never sharpen twice");
    sharpOps.admission=XessFrameAdmission::Ready;sharpHost.xessResources_->sharpeningFailure=true;
    Require(!sharpOps.EvaluateOptionalPostUpscale(frame,UpscaleOutcome::Temporal) && sharpOps.error.has_value(),
        "failed sharpening never publishes an accepted FG source");
    std::puts("PASS: actual XeSS host deferred frames, NR isolation, sharpening/FG order, resumption and vendor-failure retirement");
}
