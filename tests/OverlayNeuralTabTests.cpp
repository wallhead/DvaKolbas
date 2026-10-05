#include "OverlayNeuralTab.h"
#include <imgui_internal.h>

#include <iostream>
#include <stdexcept>

namespace
{
void Require(bool condition, const char* message)
{
    if (!condition)
        throw std::runtime_error(message);
}

void Frame(bool fsr, bool selectNeural, int& neuralContents, int& imageContents, bool community=false, bool amd=false)
{
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Always);
    ImGui::Begin("NR tab regression");
    auto& context = *ImGui::GetCurrentContext();
    auto* parent = context.CurrentWindow;
    const auto windows = context.CurrentWindowStack.Size;
    const auto ids = parent->IDStack.Size;
    if (ImGui::BeginTabBar("Settings"))
    {
        if (ImGui::BeginTabItem("DLSS", nullptr, selectNeural ? 0 : ImGuiTabItemFlags_SetSelected))
        {
            ++imageContents;
            ImGui::TextUnformatted("Image controls");
            ImGui::EndTabItem();
        }
        if (TheosRenderPipeline::Overlay::BeginNeuralRenderingTab(selectNeural, fsr, community, amd))
        {
            ++neuralContents;
            // Represents the caller's settings layout. An unavailable tab must
            // close itself before permitting any layout or backend work here.
            ImGui::PushID("neural");
            ImGui::BeginChild("details", ImVec2(0, 200));
            ImGui::PushItemWidth(-180);
            ImGui::TextUnformatted("NR controls");
            ImGui::PopItemWidth();
            ImGui::EndChild();
            ImGui::PopID();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Frame generation"))
        {
            ImGui::TextUnformatted("Frame generation controls");
            ImGui::EndTabItem();
        }
        auto* tabs = ImGui::GetCurrentTabBar();
        Require(tabs->Tabs.Size == 3, "settings navigation must keep all three pages available");
        Require(std::string(ImGui::TabBarGetTabName(tabs, &tabs->Tabs[1])) == "NR",
                "NR navigation must use the requested compact tab label");
        ImGui::EndTabBar();
    }
    Require(context.CurrentWindow == parent && context.CurrentWindowStack.Size == windows,
            "NR tab leaked a child window");
    Require(parent->IDStack.Size == ids && parent->DC.ItemWidthStack.Size == 0 &&
                parent->DC.TextWrapPosStack.Size == 0,
            "NR tab left unbalanced control stacks");
    ImGui::End();
    ImGui::Render();
    Require(!fsr || community || neuralContents == 0, "FSR NR tab allowed unsupported layout/backend work");
    Require(!amd || neuralContents == 0, "AMD NR tab allowed unsupported layout/backend work");
}

void Exercise(bool fsr, bool community=false, bool amd=false)
{
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(1024, 768);
    io.DeltaTime = 1.0f / 60;
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    int neural = 0, image = 0;
    for (int i = 0; i < 120; ++i)
        Frame(fsr, (i / 10) % 2 == 0, neural, image, community, amd);
    Require(image > 0, "Image tab stopped working after NR selection");
    Require(amd || (fsr && !community) ? neural == 0 : neural > 0, "NR availability routing changed");
    ImGui::DestroyContext();
}
}

int main()
{
    try
    {
        Exercise(true);
        Exercise(false);
        Exercise(true,true);
        Exercise(true,true,true);
        std::cout << "Repeated FSR NR selection, ImGui stack balance and supported NR routing passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        if (ImGui::GetCurrentContext())
            ImGui::DestroyContext();
        std::cerr << error.what() << '\n';
        return 1;
    }
}
