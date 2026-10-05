#pragma once

#include <imgui.h>

namespace TheosRenderPipeline::Overlay
{
    // Compatibility names for existing panels; all use the stock ImGui palette.
    inline const ImVec4 kIvory = ImGuiStyle{}.Colors[ImGuiCol_Text];
    inline const ImVec4 kMuted = ImGuiStyle{}.Colors[ImGuiCol_TextDisabled];
    inline const ImVec4 kAmber = kIvory;
    inline const ImVec4 kAmberDim = kMuted;
    inline const ImVec4 kSage = kIvory;
    inline const ImVec4 kOchre = kIvory;
    inline const ImVec4 kRust = kIvory;
    inline const ImVec4 kPanel = ImGuiStyle{}.Colors[ImGuiCol_ChildBg];

	enum class UIHealth
	{
		kIdle,
		kHealthy,
		kWarning,
		kError
	};

    const char* ModeName(int mode);

	ImVec4 HealthColor(UIHealth a_health);
    inline void ApplyRendererStyle()
    {
        ImGui::GetStyle() = ImGuiStyle{};
        ImGui::StyleColorsDark();
    }
	void DrawHealthDot(UIHealth a_health);
	void DrawStatusLabel(const char* a_label, UIHealth a_health);
	void DrawBadge(const char* a_label, const ImVec4& a_color);
    void DrawSettingsHeading(const char* title, const char* behavior = "");
    void DrawSettingsHelp(const char* text);
    void DrawSettingsValue(const char* label, const char* value);
}
