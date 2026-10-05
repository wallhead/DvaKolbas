#include "OverlayNumericInput.h"
#include "RendererSettingsEdits.h"
#include <imgui_internal.h>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace TheosRenderPipeline;
void Require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
int main(){try{
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;
    io.DisplaySize={800,600};io.DeltaTime=1.f/60;
    unsigned char* pixels{};int width{},height{};io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    RendererSettingsDraft draft;draft.valid=true;draft.sourceDLSSG.outputFPSLimit=165;
    RendererSettingsEditTransaction edits;std::vector<int> limits;std::vector<float> tones;int toggles{};
    ImVec2 input,targetInput,slider,checkbox;std::vector<int> targets;bool combined{};
    const auto frame=[&] {
        ImGui::NewFrame();ImGui::SetNextWindowPos({0,0});ImGui::SetNextWindowSize({400,300});ImGui::Begin("Settings");
        const auto before=draft;
        Overlay::FPSInput("##fps",draft.sourceDLSSG.outputFPSLimit);
        input=ImGui::GetItemRectMin();input.x+=80;input.y+=8;
        int target=static_cast<int>(draft.sourceDLSSG.generation.dynamicTargetFPS);
        if(Overlay::FPSInput("##target",target))draft.sourceDLSSG.generation.dynamicTargetFPS=static_cast<unsigned>(target);
        targetInput=ImGui::GetItemRectMin();targetInput.x+=80;targetInput.y+=8;
        ImGui::SliderFloat("Tone",&draft.sourceDLSSG.neuralTuning.localToneStrength,0,2);
        slider=ImGui::GetItemRectMin();slider.x+=30;slider.y+=8;
        ImGui::Checkbox("NR",&draft.sourceDLSSG.neuralEnabled);
        checkbox=ImGui::GetItemRectMin();checkbox.x+=8;checkbox.y+=8;
        edits.Observe(before,draft,ImGui::GetActiveID(),[&](const auto& a,const auto& b){
            const bool targetChanged=a.sourceDLSSG.generation.dynamicTargetFPS!=b.sourceDLSSG.generation.dynamicTargetFPS;
            if(targetChanged)targets.push_back(static_cast<int>(b.sourceDLSSG.generation.dynamicTargetFPS));
            if(a.sourceDLSSG.outputFPSLimit!=b.sourceDLSSG.outputFPSLimit){limits.push_back(b.sourceDLSSG.outputFPSLimit);combined|=targetChanged;}
            if(a.sourceDLSSG.neuralTuning!=b.sourceDLSSG.neuralTuning)tones.push_back(b.sourceDLSSG.neuralTuning.localToneStrength);
            if(a.sourceDLSSG.neuralEnabled!=b.sourceDLSSG.neuralEnabled)++toggles;
        });
        ImGui::End();ImGui::Render();
    };
    frame();frame();io.AddMousePosEvent(input.x,input.y);io.AddMouseButtonEvent(0,true);frame();
    io.AddMouseButtonEvent(0,false);frame();frame();
    for(char digit:{'1','4','4'}){io.AddInputCharacter(digit);frame();frame();Require(limits.empty(),"unfinished FPS values cannot reach application");}
    Require(draft.sourceDLSSG.outputFPSLimit==144,"numeric editing retains the complete draft");
    io.AddKeyEvent(ImGuiKey_Enter,true);frame();io.AddKeyEvent(ImGuiKey_Enter,false);frame();
    Require(limits==std::vector<int>{144},"Enter commits the completed FPS value once");
    io.AddMousePosEvent(slider.x,slider.y);io.AddMouseButtonEvent(0,true);frame();
    for(float dx:{60.f,100.f,140.f}){io.AddMousePosEvent(slider.x+dx,slider.y);frame();Require(tones.empty(),"slider motion cannot repeatedly apply NR settings");}
    io.AddMouseButtonEvent(0,false);frame();Require(tones.size()==1,"slider release commits once");
    io.AddMousePosEvent(checkbox.x,checkbox.y);io.AddMouseButtonEvent(0,true);frame();io.AddMouseButtonEvent(0,false);frame();
    Require(toggles==1&&draft.sourceDLSSG.neuralEnabled,"checkbox release applies immediately");
    for(int idle=0;idle!=120;++idle)frame();Require(limits.size()==1&&tones.size()==1&&toggles==1,"idle frames cannot repeat transactions");
    io.AddMousePosEvent(input.x,input.y);io.AddMouseButtonEvent(0,true);frame();io.AddMouseButtonEvent(0,false);frame();frame();
    io.AddInputCharacter('9');frame();frame();Require(limits.size()==1,"focus-loss edit is still pending while active");
    io.AddMousePosEvent(380,270);io.AddMouseButtonEvent(0,true);frame();io.AddMouseButtonEvent(0,false);frame();
    Require(limits.size()==2&&limits.back()==9,"clicking away commits numeric input without requiring Enter");
    draft.sourceDLSSG.generation.dynamicTargetFPS=120;frame();
    io.AddMousePosEvent(targetInput.x,targetInput.y);io.AddMouseButtonEvent(0,true);frame();io.AddMouseButtonEvent(0,false);frame();frame();
    for(char digit:{'1','2'}){io.AddInputCharacter(digit);frame();frame();}
    Require(targets.empty(),"unfinished dynamic targets are not submitted");
    io.AddMousePosEvent(input.x,input.y);io.AddMouseButtonEvent(0,true);frame();io.AddMouseButtonEvent(0,false);frame();frame();
    Require(targets==std::vector<int>{12},"direct editor handoff finishes the old field separately");
    for(char digit:{'2','4','0'}){io.AddInputCharacter(digit);frame();frame();}
    io.AddKeyEvent(ImGuiKey_Enter,true);frame();io.AddKeyEvent(ImGuiKey_Enter,false);frame();
    Require(limits.size()==3&&limits.back()==240&&!combined,"new FPS input cannot be bundled with the preceding invalid target");
    RendererSettingsEditTransaction closing;auto beforeClose=draft;
    draft.sourceDLSSG.neuralTuning.localToneStrength=.25f;int closes{};
    closing.Observe(beforeClose,draft,true,[&](const auto&,const auto&){++closes;});
    Require(closing.Commit(draft,[&](const auto&,const auto&){++closes;})&&closes==1,
        "closing the overlay commits the final active edit once");
    Require(!closing.Commit(draft,[&](const auto&,const auto&){++closes;}),"close cannot duplicate a transaction");
    auto reverted=draft;closing.Observe(draft,reverted,true,[&](const auto&,const auto&){++closes;});
    reverted.sharpness=.3f;closing.Observe(draft,reverted,true,[&](const auto&,const auto&){++closes;});
    Require(!closing.Observe(reverted,draft,false,[&](const auto&,const auto&){++closes;}),"reverting a drag to its initial value needs no application");
    ImGui::DestroyContext();std::cout<<"PASS: completed numeric edits, one slider transaction and immediate toggles\n";return 0;
}catch(const std::exception& e){if(ImGui::GetCurrentContext())ImGui::DestroyContext();std::cerr<<e.what()<<'\n';return 1;}}
