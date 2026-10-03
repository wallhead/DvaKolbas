#include <PCH.h>
#include "NvidiaHost.h"
#include "SourceFrameGeneration.h"
#include "GameCameraMeasurements.h"
#include "../RenderPipeline.h"
#include "CommunityShaderIntegration.h"
#include "NeuralRendering/BeforeSettings.h"
#include "PluginPaths.h"
#include <chrono>

using namespace TheosRenderPipeline;
namespace NR=TheosRenderPipeline::NeuralRendering;
void NvidiaHost::InspectCommunityNeural()
{
    if (!SourceFrameGeneration::GetSingleton()->settings.neuralStartup.community || communityNeural_) return;
    communityNeural_=std::make_unique<NR::BeforeHost>();
    const auto& startup=SourceFrameGeneration::GetSingleton()->settings.neuralStartup;
    const auto cache=PluginPaths::Directory()/"TheosRenderPipeline"/"NR"/"cache";
    const auto inspected=communityNeural_->Inspect(device_.Get(),startup,cache);
    communityLastStatus_=communityNeural_->Status();
    logger::info("[Community NR startup] available={} profile={} encoding={} root={} core={} status={}",
        bool(inspected),communityNeural_->ProfileId(),Upscaling::ColorEncodingName(startup.sourceEncoding),
        startup.runtimeRoot.string(),startup.driverCore.string(),communityNeural_->Status());
}
bool NvidiaHost::EvaluateCommunityNeuralBefore(ID3D11Texture2D* color,ID3D11Texture2D* depth,
    ID3D11Texture2D* motion,UINT width,UINT height,uint64_t sourceId,bool& reset,bool eligible)
{
    const auto& startup=SourceFrameGeneration::GetSingleton()->settings.neuralStartup;
    if (!startup.community) return true;
    InspectCommunityNeural();
    const auto& p=SourceFrameGeneration::GetSingleton()->settings.sourceDLSSG;
    NR::SettingsSnapshot snapshot;
    snapshot.enabled=p.neuralEnabled;snapshot.placement=p.neuralBeforeUpscaling?NR::Placement::Before:NR::Placement::After;
    snapshot.tuning=p.neuralTuning;snapshot.reconstruction=p.neuralReconstruction;
    const bool changed=!communitySnapshotValid_ || snapshot.enabled!=communitySnapshot_.enabled ||
        snapshot.placement!=communitySnapshot_.placement || snapshot.tuning!=communitySnapshot_.tuning ||
        snapshot.reconstruction!=communitySnapshot_.reconstruction;
    snapshot.revision=communitySnapshot_.revision+(changed?1:0);
    communitySnapshot_=snapshot;communitySnapshotValid_=true;
    NR::BeforeInput input;input.context=context_;input.color=color;input.depth=depth;input.motion=motion;
    input.epoch=communityEpoch_;input.sourceId=input.guideSourceId=sourceId;
    input.guideEpoch=input.epoch;input.previousSourceId=sourceId?sourceId-1:0;
    input.colorExtent=input.guideExtent={width,height};input.reset=reset || changed;
    input.presentationTime=std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    input.motionScaleX=float(width);input.motionScaleY=float(height);
    auto* pipeline=RenderPipeline::GetSingleton();
    const char* unavailable=NR::NativeBeforeUnavailable(p);
    if (CommunityShaders::Active() || !nativeUI_.Dedicated() || !pipeline->mNativeUI) eligible=false;
    auto camera=eligible && snapshot.enabled && communityNeural_->Available() && !unavailable?
        CaptureGameCameraMeasurements(pipeline->mGraphicsState,{width,height},pipeline->mEnableJitter,reset):
        Upscaling::Result<Upscaling::CameraMeasurements>{std::unexpected(Upscaling::RuntimeError{Upscaling::ErrorKind::InvalidInput,0,"NR waiting for world camera"})};
    if (camera) {
        const auto decision=communityCameraHistory_.Accept(sourceId,*camera,{width,height});
        eligible=decision.valid;
        input.depthInverted=camera->depthInverted;
        input.reset|=decision.reset;
    } else eligible=false;
    if (!eligible || unavailable) communityCameraHistory_.Invalidate();
    // Reset retained temporal state on menus, invalid camera, or an unavailable
    // request; the user's authoritative enabled/placement choices stay intact.
    if (!eligible || unavailable) snapshot.enabled=false;
    const bool previouslyActive=communityNeural_->Active();
    const auto result=communityNeural_->Evaluate(input,snapshot);
    std::string status=communityNeural_->Status();
    if (p.neuralEnabled && communityNeural_->Available() && !communityNeural_->Terminal()) {
        if (unavailable) status=unavailable;
        else if (!eligible) status="NR paused; waiting for a valid TRP world frame";
    }
    if (status!=communityLastStatus_ || (result && result->evaluated && communityNeural_->Recorded()%600==0)) {
        logger::info("[Community NR frame] source={} requested={} evaluated={} profile={} revision={} reset={} frames={} resets={} status={}",
            sourceId,p.neuralEnabled,result && result->evaluated,communityNeural_->ProfileId(),snapshot.revision,
            result && result->effectiveReset,communityNeural_->Recorded(),communityNeural_->Resets(),status);
        communityLastStatus_=status;
    }
    if (!result || !result->evaluated) communityCameraHistory_.Invalidate();
    if (!result) {
        logger::error("[Community NR failure] source={} profile={} kind={} native=0x{:08X} extent={}x{} revision={} deviceRemoved=0x{:08X} reason={}",
            sourceId,communityNeural_->ProfileId(),static_cast<unsigned>(result.error().kind),static_cast<uint32_t>(result.error().nativeCode),
            width,height,snapshot.revision,static_cast<uint32_t>(device_->GetDeviceRemovedReason()),result.error().message);
        status_=result.error().message;FailLifecycle(E_FAIL,"community NR source evaluation");return false;}
    reset=NR::SourceResetAfterNr(reset,result->evaluated,previouslyActive,result->effectiveReset);
    return true;
}
bool NvidiaHost::RetireCommunityNeural()
{
    if (!communityNeural_) return true;
    const auto retired=communityNeural_->Retire();
    if (!retired) {status_=retired.error().message;FailLifecycle(E_FAIL,"community NR retirement");return false;}
    communityNeural_.reset();communitySnapshotValid_=false;communityCameraHistory_.Invalidate();
    ++communityEpoch_;communityLastStatus_.clear();return true;
}
