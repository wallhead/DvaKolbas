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
    auto applied=controller.Apply(draft,false);
    Require(applied.applied&&!applied.error&&pipeline.saves==0,"Apply changes only session defaults");
    Require(fg.settings.sourceDLSSG.neuralEnabled&&!fg.settings.sourceDLSSG.neuralBeforeUpscaling,"actual Apply publishes NR After request");
    Require(!SourceDLSSG::Backend::Get().NeuralConfiguration().enabled,"community owner cannot also enable the legacy NR pass");
    for(bool enabled:{false,true,false,true}){
        draft=controller.Capture(true,false);draft.sourceDLSSG.neuralEnabled=enabled;draft.generationEnabled=enabled;
        applied=controller.Apply(draft,false);
        Require(applied.applied&&!applied.error&&fg.RuntimeInterpolationRequested()==enabled&&fg.settings.sourceDLSSG.neuralEnabled==enabled,"Apply preserves live NR/FG requests");
    }
    draft=controller.Capture(true,false);draft.sourceDLSSG.neuralBeforeUpscaling=true;
    Require(controller.Apply(draft,true).applied&&pipeline.saves==1,"Save reaches writer exactly once");
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
    std::cout<<"PASS production RendererSettingsController Apply/Save/staging/rejection\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
