#include "PCH.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "OverlayCommunityNeuralControls.h"
#include "RendererSettings.h"
#include <stdexcept>

using namespace TheosRenderPipeline;
void Require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
int main(){try{
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={1024,768};io.DeltaTime=1.f/60;io.ConfigInputTrickleEventQueue=false;
    unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    SourceDLSSG::Preferences p;p.neuralEnabled=true;p.neuralBeforeUpscaling=false;p.neuralPasses=2;
    p.neuralTuning.style=3;p.neuralTuning.intensity=.6f;p.neuralTuning.uiCorrection=true;
    p.neuralReconstruction.preset=1;p.neuralReconstruction.inputScale=.5f;
    p.neuralReconstruction.method=NeuralRendering::ResolveMethod::Ratio;
    p.neuralReconstruction.colorIsHDR=true;p.neuralReconstruction.producerColor=true;
    p.neuralReconstruction.fusedPreparation=true;p.neuralReconstruction.peripheralCompression=true;
    p.hdrOutput.enabled=true;
    const auto original=p;ImRect action;
    auto frame=[&]{
        ImGui::NewFrame();ImGui::SetNextWindowPos({50,50},ImGuiCond_Always);ImGui::SetNextWindowSize({800,600},ImGuiCond_Always);
        ImGui::Begin("Community NR repair");
        Overlay::DrawCommunityNeuralCompatibility(p,DLSS,Upscaling::Quality::Quality,false);
        action=ImGui::GetCurrentContext()->LastItemData.Rect;
        ImGui::End();ImGui::Render();
    };
    frame();frame();Require(p==original,"rendering unsupported saved NR preferences cannot silently normalize them");
    const auto click=action.GetCenter();io.AddMousePosEvent(click.x,click.y);io.AddMouseButtonEvent(0,true);frame();
    io.AddMouseButtonEvent(0,false);frame();
    Require(!NeuralRendering::NativeBeforeUnavailable(p)&&!NeuralRendering::NativeAfterUnavailable(p,DLSS,Upscaling::Quality::Quality,false),
        "visible repair action must recover unsupported community NR preferences");
    Require(p.neuralEnabled&&p.neuralTuning.style==3&&p.neuralTuning.intensity==.6f,
        "repair retains enabled state and supported visual tuning");
    Require(p.neuralPasses==2&&!p.neuralTuning.uiCorrection&&!p.hdrOutput.enabled&&p.neuralBeforeUpscaling,
        "repair preserves supported pass count while selecting native SDR Before settings");
    RendererSettingsDraft draft;draft.valid=true;draft.sourceDLSSG=p;
    RendererSettingsCapabilities capabilities{true,true,true,false};capabilities.communityNeural=true;
    Require(!ValidateRendererSettings(draft,capabilities),"repaired draft passes actual Apply validation");
    p.neuralPasses=3;
    p.neuralSecondPass.linked=true;p.neuralSecondPass.tuning.style=5;
    p.neuralThirdPass.linked=false;p.neuralThirdPass.tuning.style=2;
    p.neuralThirdPass.tuning.localToneStrength=.4f;
    const auto three=p;
    for(int i=0;i<2;++i) {
        ImGui::NewFrame();ImGui::SetNextWindowSize({800,650},ImGuiCond_Always);
        ImGui::Begin("Three NR passes");
        const auto ids=ImGui::GetCurrentWindow()->IDStack.Size;
        Overlay::DrawCommunityNeuralPassControls(p);
        Require(ImGui::GetCurrentWindow()->IDStack.Size==ids,"pass controls balance per-pass widget IDs");
        ImGui::End();ImGui::Render();
    }
    Require(p==three,"drawing three passes preserves independent and linked saved overrides");
    ImGui::DestroyContext();std::cout<<"PASS explicit community NR draft repair without silent normalization\n";return 0;
}catch(const std::exception& error){if(ImGui::GetCurrentContext())ImGui::DestroyContext();std::cerr<<error.what()<<'\n';return 1;}}
