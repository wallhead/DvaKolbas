#include "OverlayPipeline.h"

#include <algorithm>
#include <string>
#include <sstream>

namespace TheosRenderPipeline::Overlay
{
SettingsPage DrawPipelineDiagram(const PipelineDiagram& diagram)
{
    ImGui::PushID("pipelineDiagram");
    const auto origin = ImGui::GetCursorScreenPos();
    const auto& style = ImGui::GetStyle();
    const float width = ImGui::GetContentRegionAvail().x;
    const float lineHeight = ImGui::GetTextLineHeight();
    const float gap = lineHeight * 2.0f;
    const float stageCount = static_cast<float>(diagram.stages.size());
    const float nodeWidth = (std::max)(1.0f, (width - gap * (stageCount - 1)) / stageCount);
    const float textWidth = (std::max)(1.0f, nodeWidth - style.FramePadding.x * 2.0f);
    const std::string uiLabel = std::string("Native UI\n") + (diagram.nativeUI ? diagram.nativeDetail : "off");
    ImGui::TextUnformatted(uiLabel.c_str());
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Native-resolution menus and HUD join the processed scene after frame generation.\nThe diagram shows applied settings.");
    const float headerHeight = ImGui::GetItemRectSize().y;
    const float nodeTop = origin.y + headerHeight + style.ItemSpacing.y;
    std::array<std::string, PipelineDiagram::StageCount> labels;
    auto wrap = [&](const std::string& text) {
        std::string result, line;
        std::istringstream lines(text);
        while (std::getline(lines, line))
        {
            std::istringstream words(line);
            std::string word, current;
            while (words >> word)
            {
                const auto candidate = current.empty() ? word : current + " " + word;
                if (!current.empty() && ImGui::CalcTextSize(candidate.c_str()).x > textWidth)
                {
                    result += current + "\n";
                    current = word;
                }
                else current = candidate;
            }
            result += current + "\n";
        }
        if (!result.empty()) result.pop_back();
        return result;
    };
    float nodeHeight = ImGui::GetFrameHeight();
    for (std::size_t i = 0; i < diagram.stages.size(); ++i)
    {
        std::string detail = diagram.stages[i].detail;
        const auto separator = detail.find(" | ");
        if (separator != std::string::npos && ImGui::CalcTextSize(detail.c_str()).x > textWidth)
            detail.replace(separator, 3, "\n");
        labels[i] = wrap(std::string(diagram.stages[i].title) + "\n" + detail);
        nodeHeight = (std::max)(nodeHeight, ImGui::CalcTextSize(labels[i].c_str()).y + style.FramePadding.y * 2.0f);
    }
    auto* draw = ImGui::GetWindowDrawList();
    const auto lineColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    const float middle = nodeTop + nodeHeight * 0.5f;
    const float arrow = lineHeight * 0.3f;
    const float branchX = origin.x + static_cast<float>(PipelineDiagram::StageCount - 1) * (nodeWidth + gap) - gap * 0.5f;
    if (diagram.nativeUI)
    {
        const float branchY = origin.y + headerHeight * 0.5f;
        draw->AddLine(ImVec2(origin.x + nodeWidth, branchY), ImVec2(branchX, branchY), lineColor);
        draw->AddLine(ImVec2(branchX, branchY), ImVec2(branchX, middle - arrow), lineColor);
        draw->AddTriangleFilled(ImVec2(branchX - arrow, middle - arrow * 2),
                                ImVec2(branchX + arrow, middle - arrow * 2), ImVec2(branchX, middle), lineColor);
    }
    SettingsPage requested = SettingsPage::None;
    for (std::size_t i = 0; i < diagram.stages.size(); ++i)
    {
        const auto& stage = diagram.stages[i];
        const ImVec2 start(origin.x + static_cast<float>(i) * (nodeWidth + gap), nodeTop);
        ImGui::SetCursorScreenPos(start);
        ImGui::PushID(static_cast<int>(i));
        ImGui::BeginDisabled(stage.page == SettingsPage::None);
        // Runtime text may change between press and release; keep navigation IDs stable.
        const auto buttonLabel = labels[i] + "###stage";
        if (ImGui::Button(buttonLabel.c_str(), ImVec2(nodeWidth, nodeHeight)))
            requested = stage.page;
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            ImGui::SetTooltip("%s", stage.tooltip);
        ImGui::PopID();
        if (i + 1 < diagram.stages.size())
        {
            const float tip = start.x + nodeWidth + gap - 3;
            draw->AddLine(ImVec2(start.x + nodeWidth, middle), ImVec2(tip - arrow, middle), lineColor);
            draw->AddTriangleFilled(ImVec2(tip - arrow, middle - arrow), ImVec2(tip, middle),
                                    ImVec2(tip - arrow, middle + arrow), lineColor);
        }
    }
    ImGui::SetCursorScreenPos(origin);
    ImGui::Dummy(ImVec2(width, headerHeight + style.ItemSpacing.y + nodeHeight));
    ImGui::TextDisabled("Pipeline: %s", diagram.status);
    ImGui::Separator();
    ImGui::PopID();
    return requested;
}
} // namespace TheosRenderPipeline::Overlay
