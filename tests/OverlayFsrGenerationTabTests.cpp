#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <stdexcept>
#include "OverlayFsrGenerationControls.h"
static void Require(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
static void NormalToggleRetainsPresenter()
{
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={1024,768};io.DeltaTime=1.f/60;io.ConfigInputTrickleEventQueue=false;
    unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    long backend=2;bool requested=true,changed=false;ImVec2 checkbox{};
    auto frame=[&]{
        ImGui::NewFrame();ImGui::SetNextWindowPos({50,50},ImGuiCond_Always);ImGui::SetNextWindowSize({800,600},ImGuiCond_Always);
        ImGui::Begin("Normal FG interaction");
        checkbox=ImGui::GetCursorScreenPos();checkbox.x+=8;
        checkbox.y+=ImGui::GetTextLineHeightWithSpacing()+ImGui::GetFrameHeight()/2;
        changed|=TheosRenderPipeline::Overlay::DrawFsrGenerationControls(true,true,backend,requested);
        ImGui::End();ImGui::Render();
    };
    frame();frame();io.AddMousePosEvent(checkbox.x,checkbox.y);io.AddMouseButtonEvent(0,true);frame();
    io.AddMouseButtonEvent(0,false);frame();
    Require(!requested&&changed&&backend==2,"normal FG toggle disables interpolation while retaining presenter 2");
    Require(ImGui::GetCurrentContext()->OpenPopupStack.empty(),"normal menu does not expose ordinary presentation dropdown");
    changed=false;io.AddMouseButtonEvent(0,true);frame();io.AddMouseButtonEvent(0,false);frame();
    Require(requested&&changed&&backend==2,"normal FG toggle can resume interpolation without switching presenters");
    ImGui::DestroyContext();
}
static void DiagnosticHostCanStageFg()
{
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={1024,768};io.DeltaTime=1.f/60;io.ConfigInputTrickleEventQueue=false;
    unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    long backend=0;bool requested=false,changed=false;ImVec2 button{};
    auto frame=[&]{
        ImGui::NewFrame();ImGui::SetNextWindowPos({50,50},ImGuiCond_Always);ImGui::SetNextWindowSize({800,600},ImGuiCond_Always);
        ImGui::Begin("Diagnostic host to normal FG");
        button=ImGui::GetCursorScreenPos();button.x+=50;
        button.y+=ImGui::GetTextLineHeightWithSpacing()+ImGui::GetFrameHeight()/2;
        changed|=TheosRenderPipeline::Overlay::DrawFsrGenerationControls(true,false,backend,requested);
        ImGui::End();ImGui::Render();
    };
    frame();frame();Require(backend==0,"opening the normal menu preserves an explicit diagnostic INI choice");
    io.AddMousePosEvent(button.x,button.y);io.AddMouseButtonEvent(0,true);frame();io.AddMouseButtonEvent(0,false);frame();
    Require(backend==2&&!requested&&!changed,"diagnostic host can stage normal FSR FG support without activating the old host");
    ImGui::DestroyContext();
}
static void OrdinarySelectionKeepsLiveRequest()
{
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={1024,768};io.DeltaTime=1.f/60;io.ConfigInputTrickleEventQueue=false;
    unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    long backend=2;bool requested=true,changed=false;ImVec2 combo{};
    auto frame=[&] {
        ImGui::NewFrame();ImGui::SetNextWindowPos({50,50},ImGuiCond_Always);ImGui::SetNextWindowSize({800,600},ImGuiCond_Always);
        ImGui::Begin("Presenter interaction");
        combo=ImGui::GetCursorScreenPos();combo.x+=100;combo.y+=ImGui::GetTextLineHeightWithSpacing()+ImGui::GetFrameHeight()/2;
        changed=TheosRenderPipeline::Overlay::DrawFsrGenerationControls(true,true,backend,requested,true);
        ImGui::End();ImGui::Render();
    };
    frame();frame();io.AddMousePosEvent(combo.x,combo.y);io.AddMouseButtonEvent(0,true);frame();
    io.AddMouseButtonEvent(0,false);frame();frame();
    auto& context=*ImGui::GetCurrentContext();
    Require(context.OpenPopupStack.Size>0,"presenter dropdown opened from input");
    auto* popup=context.OpenPopupStack.back().Window;
    Require(popup!=nullptr,"presenter popup rendered");
    const ImVec2 ordinary{popup->Pos.x+40,popup->Pos.y+ImGui::GetStyle().WindowPadding.y+ImGui::GetTextLineHeight()/2};
    io.AddMousePosEvent(ordinary.x,ordinary.y);io.AddMouseButtonEvent(0,true);frame();
    io.AddMouseButtonEvent(0,false);frame();
    Require(backend==0,"ordinary presenter selection staged");
    Require(requested&&!changed,"ordinary dropdown cannot emit a live FG disable");
    ImGui::DestroyContext();
}
static void PendingOrdinaryKeepsCurrentCheckboxLive()
{
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={1024,768};io.DeltaTime=1.f/60;io.ConfigInputTrickleEventQueue=false;
    unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    long backend=0;bool requested=false,changed=false;ImVec2 checkbox{};
    auto frame=[&]{
        ImGui::NewFrame();ImGui::SetNextWindowPos({50,50},ImGuiCond_Always);ImGui::SetNextWindowSize({800,600},ImGuiCond_Always);
        ImGui::Begin("Live AMD with ordinary pending");
        checkbox=ImGui::GetCursorScreenPos();checkbox.x+=8;
        checkbox.y+=2*ImGui::GetTextLineHeightWithSpacing()+ImGui::GetFrameHeightWithSpacing()+ImGui::GetFrameHeight()/2;
        changed|=TheosRenderPipeline::Overlay::DrawFsrGenerationControls(true,true,backend,requested,true);
        ImGui::End();ImGui::Render();
    };
    frame();frame();io.AddMousePosEvent(checkbox.x,checkbox.y);io.AddMouseButtonEvent(0,true);frame();
    io.AddMouseButtonEvent(0,false);frame();
    Require(requested&&changed&&backend==0,"pending ordinary presenter keeps actual AMD FG checkbox live");
    ImGui::DestroyContext();
}
int main()
{
 try {
    NormalToggleRetainsPresenter();
    DiagnosticHostCanStageFg();
    OrdinarySelectionKeepsLiveRequest();
    PendingOrdinaryKeepsCurrentCheckboxLive();
    for(bool built:{false,true})for(bool owned:{false,true}) {
        ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={1024,768};io.DeltaTime=1.f/60;
        unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
        unsigned selectedFrames{};
        for(unsigned i=0;i<120;++i){
            ImGui::NewFrame();ImGui::SetNextWindowSize({800,600},ImGuiCond_Always);ImGui::Begin("FSR FG regression");
            auto& context=*ImGui::GetCurrentContext();auto* parent=context.CurrentWindow;
            const int windows=context.CurrentWindowStack.Size,ids=parent->IDStack.Size;
            const bool selected=(i/10)%2==0;
            if(ImGui::BeginTabBar("Settings")){
                if(ImGui::BeginTabItem("Image",nullptr,selected?0:ImGuiTabItemFlags_SetSelected)){ImGui::TextUnformatted("Image");ImGui::EndTabItem();}
                if(ImGui::BeginTabItem("Frame generation",nullptr,selected?ImGuiTabItemFlags_SetSelected:0)){
                    ++selectedFrames;long backend=owned?2:0;bool requested=owned;
                    TheosRenderPipeline::Overlay::DrawFsrGenerationControls(built,owned,backend,requested);
                    Require(backend==(owned?2:0) && requested==owned,"rendering controls cannot alter a preference without input");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            Require(context.CurrentWindow==parent && context.CurrentWindowStack.Size==windows,"FSR FG tab leaked a child");
            Require(parent->IDStack.Size==ids && parent->DC.ItemWidthStack.Size==0 && parent->DC.TextWrapPosStack.Size==0 && context.DisabledStackSize==0,"FSR FG controls unbalanced");
            ImGui::End();ImGui::Render();
        }
        Require(selectedFrames>0,"repeated FSR FG selection actually exercised controls");ImGui::DestroyContext();
    }
    std::cout<<"PASS: repeated FSR FG selection and enabled/disabled capability stack balance\n";return 0;
 }catch(const std::exception& error){if(ImGui::GetCurrentContext())ImGui::DestroyContext();std::cerr<<error.what()<<'\n';return 1;}
}
