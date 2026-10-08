#pragma once

#include <imgui.h>

namespace TheosRenderPipeline::Overlay
{
// Returning true transfers the open tab to the caller, which must call EndTabItem.
inline bool BeginNeuralRenderingTab(bool selected, bool fsrActive, bool community=false, bool fsrOnly=false)
{
    if (!ImGui::BeginTabItem("NR", nullptr,
                            selected ? ImGuiTabItemFlags_SetSelected : 0))
        return false;
    if (fsrOnly) {
        ImGui::TextWrapped("Neural Rendering requires a supported NVIDIA RTX GPU. Use FSR upscaling and optional FSR frame generation.");
        ImGui::EndTabItem();
        return false;
    }
    // Reject unsupported NR before opening the caller's nested settings columns.
    // Their cleanup is valid only after switching from the left to the right column.
    if (fsrActive && !community)
    {
        ImGui::TextWrapped("Neural Rendering is unavailable with FSR. Set FSR sharpness in the DLSS tab.");
        ImGui::EndTabItem();
        return false;
    }
    return true;
}
}
