#include "PCH.h"
#include "RendererSettingsController.h"
#include "FrameGen/NvidiaHost.h"
#include "FrameGen/SourceFrameGeneration.h"
#include "PerformanceTuning.h"
#include "FrameGen/SourceDLSSGBackend.h"
#include <stdexcept>
using namespace TheosRenderPipeline;
void Require(bool v,const char* message){if(!v)throw std::runtime_error(message);}
int main(){try{
    auto controller=RendererSettingsController::Current();auto& host=*NvidiaHost::GetSingleton();
    auto& fg=*SourceFrameGeneration::GetSingleton();auto& pipeline=*RenderPipeline::GetSingleton();
    fg.settings.neuralStartup.community=true;
    auto draft=controller.Capture(true,false);draft.sourceDLSSG.neuralEnabled=true;
    draft.sourceDLSSG.neuralBeforeUpscaling=false;draft.sourceDLSSG.neuralTuning.style=0;
    draft.sourceDLSSG.neuralPasses=3;
    draft.sourceDLSSG.neuralSecondPass.linked=false;draft.sourceDLSSG.neuralSecondPass.tuning.intensity=.5f;
    draft.sourceDLSSG.neuralThirdPass.linked=false;draft.sourceDLSSG.neuralThirdPass.tuning.intensity=.7f;
    auto applied=controller.Apply(draft,false);
    Require(applied.applied&&!applied.error&&pipeline.saves==0,"Apply changes only session defaults");
    Require(fg.settings.sourceDLSSG.neuralEnabled&&!fg.settings.sourceDLSSG.neuralBeforeUpscaling,"actual Apply publishes NR After request");
    Require(fg.settings.sourceDLSSG.neuralPasses==3&&
        fg.settings.sourceDLSSG.neuralSecondPass.tuning.intensity==.5f&&
        fg.settings.sourceDLSSG.neuralThirdPass.tuning.intensity==.7f,
        "Apply publishes all three requested passes with independent tuning");
    Require(!SourceDLSSG::Backend::Get().NeuralConfiguration().enabled,"community owner cannot also enable the legacy NR pass");
    for(bool enabled:{false,true,false,true}){
        draft=controller.Capture(true,false);const auto beforeEdits=draft;
        draft.sourceDLSSG.neuralEnabled=enabled;draft.generationEnabled=enabled;
        const auto previousRequests=host.requests;
        Require(ApplyRendererSettingsEdits(beforeEdits,draft,[&]{
            applied=controller.Apply(draft,false);
            if(applied.applied)RefreshAppliedRendererSettingsDraft(draft,controller.Capture(true,false));
        }),"checkbox edits automatically reach the production controller");
        Require(applied.applied&&!applied.error&&fg.RuntimeInterpolationRequested()==enabled&&fg.settings.sourceDLSSG.neuralEnabled==enabled&&pipeline.saves==0,
            "automatic Apply preserves live NR/FG requests without writing defaults");
        const auto idle=draft;
        for(int frame=0;frame<120;++frame)
            Require(!ApplyRendererSettingsEdits(idle,draft,[&]{controller.Apply(draft,false);}),
                "idle menu frames cannot resubmit the production controller");
        Require(host.requests==previousRequests+1,"one checkbox interaction publishes one configuration request");
    }
    draft=controller.Capture(true,false);draft.sourceDLSSG.neuralBeforeUpscaling=true;
    Require(controller.Apply(draft,true).applied&&pipeline.saves==1,"Save reaches writer exactly once");
    draft=controller.Capture(true,false);draft.upscaleType=FSR;draft.generationBackend=0;
    draft.generationEnabled=false;draft.fsr.sourceColorEncoding=Upscaling::ColorEncoding::Gamma22;
    applied=controller.Apply(draft,false);
    Require(applied.applied&&!applied.error&&fg.RuntimeInterpolationRequested(),
        "staging ordinary FSR must preserve current NVIDIA interpolation");
    Require(!fg.settings.enabled&&!controller.Capture(true,false).generationEnabled,
        "staged ordinary FSR retains its disabled next-launch preference");
    // Applying again must use the actual owner, even though backend 0 is already staged.
    Require(controller.Apply(controller.Capture(true,false),true).applied&&fg.RuntimeInterpolationRequested(),
        "Save of an already staged presenter cannot disable the current owner");
    draft=controller.Capture(true,false);draft.upscaleType=DLAA;draft.generationBackend=1;draft.generationEnabled=false;
    Require(controller.Apply(draft,false).applied&&!fg.RuntimeInterpolationRequested(),
        "matching current presenter still accepts intentional Apply FG changes");
    SetLiveGenerationRequest(draft,fg,true,1);
    Require(controller.Apply(draft,false).applied&&fg.RuntimeInterpolationRequested(),
        "explicit live checkbox survives Apply after returning to current presenter");
    pipeline.saves=1;
    draft=controller.Capture(true,false);draft.upscaleType=FSR;draft.generationBackend=2;
    draft.fsr.quality=Upscaling::Quality::NativeAA;draft.fsr.sourceColorEncoding=Upscaling::ColorEncoding::Gamma22;
    applied=controller.Apply(draft,false);
    Require(applied.applied&&!applied.error&&applied.message.find("restart")!=std::string::npos,"provider switch is explicitly staged for restart");
    Require(pipeline.mUpscaleType==DLAA&&host.configuration.Effective().mode==DLAA&&host.configuration.Requested().mode==FSR&&!host.FsrFgActive(),"staged provider cannot replace active source or presenter");
    draft=controller.Capture(true,false);draft.sourceDLSSG.neuralBeforeUpscaling=false;
    draft.fsr.quality=Upscaling::Quality::Quality;
    const auto old=fg.settings.sourceDLSSG;const auto requests=host.requests;const auto saves=pipeline.saves;
    applied=controller.Apply(draft,true);
    Require(applied.error&&!applied.applied&&host.requests==requests&&pipeline.saves==saves&&fg.settings.sourceDLSSG==old,"invalid scaled After is rejected before state/writer changes");
    draft=controller.Capture(true,false);draft.textureProviderConnected=true;
    applied=controller.Apply(draft,false);
    Require(applied.error&&!applied.applied&&host.requests==requests,"external provider failure is rejected before renderer mutations");
    host.terminal=true;fg.settings.sourceDLSSG.neuralEnabled=false;
    Require(controller.SetNeuralRenderingEnabled(true).error&&!fg.settings.sourceDLSSG.neuralEnabled,"shortcut cannot revive terminal NR owner");
    Require(controller.SetNeuralRenderingEnabled(false).applied,"NR can be disabled despite terminal owner");
    host.terminal=false;
    draft=controller.Capture(true,false);pipeline.saveResult=false;
    applied=controller.Apply(draft,true);
    Require(applied.applied&&applied.error&&pipeline.saves==saves+1,"failed Save reports persistence failure while retaining session changes");
    draft.valid=false;const auto rejectedRequests=host.requests;const auto rejectedSaves=pipeline.saves;
    applied=controller.Apply(draft,true);
    Require(!applied.applied&&applied.error&&host.requests==rejectedRequests&&pipeline.saves==rejectedSaves,"unavailable draft cannot mutate or save settings");
    Upscaler::Creation fsrStartup{FSR,4,11,false,true};
    fsrStartup.fsr.quality=Upscaling::Quality::NativeAA;
    fsrStartup.fsr.sourceColorEncoding=Upscaling::ColorEncoding::Gamma22;
    host.configuration.Initialize(fsrStartup);host.configuration.BeginSubmission();host.configuration.Completed(true);
    host.fsrFg=true;pipeline.saveResult=true;fg.settings.generationBackend=2;fg.RequestRuntimeInterpolation(true);
    draft=controller.Capture(true,false);draft.generationBackend=0;draft.generationEnabled=false;
    Require(controller.Apply(draft,false).applied&&fg.RuntimeInterpolationRequested(),
        "resolved AMD FG owner remains live while ordinary presenter is staged");
    Require(controller.Apply(controller.Capture(true,false),true).applied&&fg.RuntimeInterpolationRequested()&&!fg.settings.enabled,
        "repeated Save uses AMD startup owner rather than stored ordinary backend");
    draft=controller.Capture(true,false);draft.generationBackend=2;
    Require(controller.Apply(draft,false).applied&&!fg.RuntimeInterpolationRequested(),
        "matching resolved AMD FG presenter applies intentional off request");
    SetLiveGenerationRequest(draft,fg,true,2);
    draft.upscaleType=DLAA;draft.generationBackend=1;draft.generationEnabled=false;
    Require(controller.Apply(draft,false).applied&&fg.RuntimeInterpolationRequested(),
        "staging NVIDIA cannot disable the current AMD owner");
    std::cout<<"PASS production RendererSettingsController Apply/Save/staging/rejection\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
