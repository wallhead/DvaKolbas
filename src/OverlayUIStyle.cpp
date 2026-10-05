#include "OverlayUIStyle.h"
#include "RenderPipeline.h"

namespace TheosRenderPipeline::Overlay
{
    ImVec4 HealthColor(UIHealth health)
    {
        return ImGui::GetStyleColorVec4(health == UIHealth::kIdle ? ImGuiCol_TextDisabled : ImGuiCol_Text);
    }
    void DrawHealthDot(UIHealth)
    {
        // Retained for callers; status is expressed by ordinary text.
    }

    void DrawStatusLabel(const char* label, UIHealth health)
    {
        if (health == UIHealth::kError)
            ImGui::TextWrapped("%s (error)", label);
        else if (health == UIHealth::kWarning)
            ImGui::TextWrapped("%s (attention required)", label);
        else if (health == UIHealth::kIdle)
            ImGui::TextDisabled("%s", label);
        else
            ImGui::TextWrapped("%s", label);
    }

    void DrawBadge(const char* label, const ImVec4&)
    {
        ImGui::TextUnformatted(label);
    }
    void DrawSettingsHeading(const char* title, const char* behavior)
    {
        ImGui::Spacing();
        ImGui::SeparatorText(title);
        if (behavior && *behavior)
        {
            ImGui::TextDisabled("%s", behavior);
        }
    }

    void DrawSettingsValue(const char* label, const char* value)
    {
        ImGui::TextWrapped("%s: %s", label, value);
    }

    void DrawSettingsHelp(const char* text)
    {
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.0f);
            ImGui::TextUnformatted(text);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }


}

namespace TheosRenderPipeline::Overlay
{
const char* ModeName(int a_mode)
{
    switch (a_mode)
    {
    case FSR:
        return "FSR";
    case DLAA:
        return "DLSS Native";
    default:
        return "DLSS";
    }
}

}
