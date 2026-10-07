#include "PCH.h"
#include "RendererSettingsController.h"
#include "RendererSettingsEdits.h"
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
    {
        pipeline.mAdapterVendorId=0x1002;
        const auto invalid=controller.Capture(true,false);
        const auto saves=pipeline.saves, requests=host.requests;
        const auto result=controller.Apply(invalid,true);
        Require(result.error&&!result.applied&&pipeline.saves==saves&&host.requests==requests,
            "AMD refuses NVIDIA settings before saving or issuing runtime requests");
        Require(controller.SetNeuralRenderingEnabled(true).error,"AMD hotkey cannot enable NR");
        pipeline.mAdapterVendorId=0;
    }
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
    {
        auto staged=controller.Capture(true,false);
        const auto before=staged;StageRendererUpscaleProvider(staged,true);
        staged.fsr.sourceColorEncoding=Upscaling::ColorEncoding::Gamma22;
        const auto oldRequests=host.requests;
        const auto result=controller.ApplyLiveEdits(before,staged);
        Require(!result.error&&host.requests==oldRequests&&fg.settings.sourceDLSSG.neuralEnabled&&
            fg.RuntimeInterpolationRequested(),"browsing FSR stages startup choices without disabling live NR/FG");
        const auto beforeTone=staged;staged.sourceDLSSG.neuralTuning.localToneStrength=.8f;
        Require(controller.ApplyLiveEdits(beforeTone,staged).applied&&fg.settings.sourceDLSSG.neuralEnabled&&
            pipeline.mUpscaleType==DLAA&&fg.settings.generationBackend==1&&
            fg.settings.sourceDLSSG.neuralTuning.localToneStrength==.8f,
            "live NR tuning works on the actual owner while FSR remains staged");
        auto invalid=staged;invalid.sourceDLSSG.generation.dynamic=true;invalid.sourceDLSSG.generation.dynamicTargetFPS=12;
        Require(controller.ApplyLiveEdits(staged,invalid).error,"invalid completed numeric edit is rejected");
        auto multiplier=invalid;multiplier.sourceDLSSG.generation.generatedFrames=2;
        Require(controller.ApplyLiveEdits(invalid,multiplier).applied&&
            fg.settings.sourceDLSSG.generation.generatedFrames==2&&
            fg.settings.sourceDLSSG.generation.dynamicTargetFPS!=12,
            "unchanged rejected target cannot block an independent multiplier selection");
        auto unrelated=invalid;unrelated.sourceDLSSG.neuralEnabled=false;
        Require(controller.ApplyLiveEdits(invalid,unrelated).applied&&!fg.settings.sourceDLSSG.neuralEnabled,
            "unchanged rejected values cannot block an unrelated NR checkbox");
        auto on=unrelated;on.sourceDLSSG.neuralEnabled=true;
        Require(controller.ApplyLiveEdits(unrelated,on).applied,"restore NR after isolated checkbox check");
        auto beforeSanitize=controller.Capture(true,false);auto raw=beforeSanitize;
        StageRendererUpscaleProvider(raw,true);raw.fsr.quality=Upscaling::Quality::NativeAA;
        raw.sourceDLSSG.neuralReconstruction.whitePoint=0;raw.sourceDLSSG.neuralTuning.intensity=3;
        Require(controller.ApplyLiveEdits(beforeSanitize,raw).applied,"out-of-range editable NR values are sanitized");
        const auto accepted=controller.Capture(true,false);auto menu=raw;
        menu.sourceDLSSG.neuralTuning.localToneStrength=.75f;
        menu=ProjectRendererLiveEdits(beforeSanitize,raw,menu,&accepted);
        Require(menu.sourceDLSSG.neuralReconstruction.whitePoint==1&&menu.sourceDLSSG.neuralTuning.intensity==2&&
            menu.upscaleType==FSR&&menu.fsr.quality==Upscaling::Quality::NativeAA&&
            menu.sourceDLSSG.neuralTuning.localToneStrength==.75f,
            "accepted values merge into the editor without losing pending startup choices or a newly edited field");
    }
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
    draft.fsr.sharpness=.7f;
    applied=controller.Apply(draft,false);
    Require(applied.applied&&!applied.error&&fg.RuntimeInterpolationRequested(),
        "staging ordinary FSR must preserve current NVIDIA interpolation");
    Require(!fg.settings.enabled&&!controller.Capture(true,false).generationEnabled,
        "staged ordinary FSR retains its disabled next-launch preference");
    {
        const auto savedRequest=host.configuration.Requested();
        const auto before=controller.Capture(true,false);auto after=before;
        after.sourceDLSSG.neuralTuning.localToneStrength=.6f;
        Require(controller.ApplyLiveEdits(before,after).applied&&host.configuration.Requested().mode==savedRequest.mode&&
            host.configuration.Requested().fsr.sourceColorEncoding==savedRequest.fsr.sourceColorEncoding&&
            host.configuration.Requested().fsr.sharpness==savedRequest.fsr.sharpness&&
            fg.settings.generationBackend==0&&!fg.settings.enabled&&fg.RuntimeInterpolationRequested(),
            "live edits preserve an already pending startup allocation and presenter default");
    }
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
    draft.dynamicResolution=true;
    const auto old=fg.settings.sourceDLSSG;const auto requests=host.requests;const auto saves=pipeline.saves;
    applied=controller.Apply(draft,true);
    Require(applied.error&&!applied.applied&&host.requests==requests&&pipeline.saves==saves&&fg.settings.sourceDLSSG==old,"dynamic After is rejected before state/writer changes");
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
    for(const float sharpness : {.8f,0.0f,1.0f}) {
        const auto before=controller.Capture(true,false);auto after=before;after.fsr.sharpness=sharpness;
        const auto result=controller.ApplyLiveEdits(before,after);
        Require(result.applied && !result.error && host.configuration.NeedsLiveChange(),
            "FSR menu sharpness must queue a live edit");
        host.ApplySourceUpscalerSettingsAfterPresent();
        Require(host.configuration.Effective().fsr.sharpness==sharpness && pipeline.mFsrSettings.sharpness==sharpness &&
                controller.Capture(true,false).fsr.sharpness==sharpness && !host.configuration.NeedsRestart(),
            "FSR menu sharpness becomes effective after Present without restart");
    }
    draft=controller.Capture(true,false);draft.fsr.generationProviderPolicy=Upscaling::ProviderPolicy::MachineLearning;
    Require(controller.Apply(draft,false).applied && host.configuration.NeedsRestart(),"FG provider is staged through the settings controller");
    const auto beforeFgEdit=controller.Capture(true,false);auto afterFgEdit=beforeFgEdit;afterFgEdit.fsr.sharpness=.65f;
    Require(controller.ApplyLiveEdits(beforeFgEdit,afterFgEdit).applied,"sharpness remains live with a pending FG provider");
    host.ApplySourceUpscalerSettingsAfterPresent();
    Require(host.configuration.Requested().fsr.generationProviderPolicy==Upscaling::ProviderPolicy::MachineLearning &&
        host.configuration.Effective().fsr.generationProviderPolicy==Upscaling::ProviderPolicy::Analytical &&
        host.configuration.Effective().fsr.sharpness==.65f,"live edits preserve pending FG policy without applying it");
    host.configuration.Initialize(fsrStartup);host.configuration.BeginSubmission();host.configuration.Completed(true);pipeline.mFsrSettings=fsrStartup.fsr;
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
    auto mixedStartup=Upscaler::Creation{DLAA,4,11,false,true};mixedStartup.fsr.sourceColorEncoding=Upscaling::ColorEncoding::Gamma22;
    host.configuration.Initialize(mixedStartup);host.configuration.BeginSubmission();host.configuration.Completed(true);
    fg.settings.generationBackend=1;fg.settings.generationBackendPreference=GenerationBackendPreference::Nvidia;
    fg.settings.enabled=true;fg.RequestRuntimeInterpolation(true);
    auto mixedBefore=controller.Capture(true,false);auto mixedAfter=mixedBefore;mixedAfter.generationEnabled=false;
    Require(controller.ApplyLiveEdits(mixedBefore,mixedAfter).applied && !fg.RuntimeInterpolationRequested(),
        "staged NVIDIA backend cannot redirect the running DLAA FSR owner toggle");
    Require(fg.settings.generationBackend==1 && fg.settings.generationBackendPreference==GenerationBackendPreference::Nvidia,
        "live toggle retains the configured preference for Save");
    Require(fg.settings.enabled,"mixed live toggle preserves pending NVIDIA enabled default");
    draft=controller.Capture(true,false);draft.fsr.generationProviderPolicy=Upscaling::ProviderPolicy::MachineLearning;
    Require(controller.Apply(draft,false).applied && host.configuration.NeedsRestart(),
        "DLSS FSR FG provider choice is staged for restart independently of SR");
    std::cout<<"PASS production RendererSettingsController Apply/Save/staging/rejection\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
