// Execute the production host operations with SDK/bridge boundaries replaced
// by CPU witnesses. Actual SDK admission/resumption is covered by XessTemporalGpu.
#include "Upscaling/XessUpscaler.h"
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
    unsigned preparations{},retirements{};
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
        std::optional<RuntimeError> error;
        Result<UpscaleOutcome> Spatial(const UpscaleFrame&,const std::string& reason){++host.spatial;host.xessRecoveryReason_=reason;return UpscaleOutcome::SpatialRecovery;}
        Result<UpscaleOutcome> EvaluateUpscaler(UpscaleFrame&);
        bool NeuralEligible(){return true;}
        bool EvaluateOptionalPreUpscale(UpscaleFrame&);
        bool EvaluateOptionalPostUpscale(UpscaleFrame&,UpscaleOutcome);
        GenerationPreparationStatus PrepareGeneration(const UpscaleFrame&);
    };
    bool xessRecovery_{};unsigned xessDeferredReported_{},spatial{},nrCalls{};
    std::uint64_t communityEpoch_{1};std::string xessRecoveryReason_;
    bool fg{};UpscaleFrame fsrGenerationFrame_{};
    bool FsrFgActive()const{return fg;}
    std::unique_ptr<HostResources> xessResources_=std::make_unique<HostResources>();
    struct Context { ID3D11DeviceContext* Get(){return nullptr;} } context_;
    struct Encoder { HRESULT Convert(ID3D11DeviceContext*,ID3D11Texture2D*,ID3D11Texture2D*,ColorEncoding,ColorEncoding){return S_OK;} } xessEncode_;
    struct Settings { struct Value { struct XeSS {ColorEncoding sourceEncoding{ColorEncoding::Gamma22};} xess; } value;const Value& Effective(){return value;} } sourceUpscalerSettings_;
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
    std::puts("PASS: actual XeSS host deferred frames, NR isolation, resumption and vendor-failure retirement");
}
