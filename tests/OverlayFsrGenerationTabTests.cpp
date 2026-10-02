#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <stdexcept>
#include "OverlayFsrGenerationControls.h"
static void Require(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
int main()
{
 try {
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
