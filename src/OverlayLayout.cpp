#include "OverlayLayout.h"

#include <imgui.h>

namespace TheosRenderPipeline::Overlay
{
ColumnSizes DrawColumnSplitter(float width, float, float&)
{
    // Keep old saved layout values readable, but the ordinary menu uses one column.
    return {(std::max)(1.0f, width), 0.0f};
}

bool BeginSettingsBody()
{
    const auto& style = ImGui::GetStyle();
    const float footer = ImGui::GetFrameHeightWithSpacing() +
        ImGui::GetTextLineHeightWithSpacing() * 2 + style.ItemSpacing.y * 3;
    return ImGui::BeginChild("##settingsBody", ImVec2(0, -footer), ImGuiChildFlags_None,
                            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
}

void EndSettingsBody() { ImGui::EndChild(); }

void BeginScrollableSettings(const char* id)
{
    ImGui::PushID(id);
    ImGui::BeginChild("##settings", ImVec2(0, 0), ImGuiChildFlags_None);
    ImGui::PushTextWrapPos(0);
    ImGui::PushItemWidth((std::min)(310.0f, ImGui::GetContentRegionAvail().x * 0.5f));
}

void EndScrollableSettings()
{
    ImGui::PopItemWidth();
    ImGui::PopTextWrapPos();
    ImGui::EndChild();
    ImGui::PopID();
}

bool DrawSaveDefaultsButton()
{
    ImGui::Separator();
    return ImGui::Button("Save as default");
}

void DrawSettingsActionStatus(const char* text)
{
    if (ImGui::BeginChild("##settingsActionStatus", ImVec2(0, 0), ImGuiChildFlags_None))
        ImGui::TextWrapped("%s", text);
    ImGui::EndChild();
}
} // namespace TheosRenderPipeline::Overlay
