#pragma once

#include <imgui.h>

namespace TheosRenderPipeline::Overlay
{
// Returning true transfers the open tab to the caller, which must call EndTabItem.
inline bool BeginNeuralRenderingTab(bool selected, bool fsrActive)
{
    if (!ImGui::BeginTabItem("Neural Rendering", nullptr,
                            selected ? ImGuiTabItemFlags_SetSelected : 0))
        return false;
    // Reject unsupported NR before opening the caller's nested settings columns.
    // Their cleanup is valid only after switching from the left to the right column.
    if (fsrActive)
    {
        ImGui::TextWrapped("Neural Rendering is unavailable with FSR. Set FSR sharpness in the Image tab.");
        ImGui::EndTabItem();
        return false;
    }
    return true;
}
}
