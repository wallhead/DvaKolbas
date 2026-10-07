#pragma once
#include <imgui.h>

namespace TheosRenderPipeline::Overlay
{
    // The enclosing tab belongs to the caller. Every temporary UI stack opened
    // here closes here; this branch performs no NVIDIA runtime queries.
    inline bool DrawFsrGenerationControls(bool built, bool owned, long& backend, bool& requested,
                                         bool diagnostics = false)
    {
        ImGui::PushID("fsr-generation");
        bool changed{};
        if (diagnostics) {
            ImGui::TextUnformatted("Testing presentation backend (restart required)");
            const int selected=backend==2?1:0;
            ImGui::SetNextItemWidth(-1.0f);
            if(ImGui::BeginCombo("##presenter",selected?"FSR FG":"Ordinary (SR only)")) {
                if(ImGui::Selectable("Ordinary (SR only)",selected==0)) backend=0;
                ImGui::BeginDisabled(!built);
                if(ImGui::Selectable("FSR FG",selected==1)) backend=2;
                ImGui::EndDisabled();
                ImGui::EndCombo();
            }
        } else {
            ImGui::TextUnformatted(owned ? "Backend: FSR FG" : "FSR FG support");
            if (built && !owned && backend != 2) {
                if (ImGui::Button("Enable FSR FG support (save and restart)")) backend = 2;
            }
        }
        if(!built)ImGui::TextWrapped("FSR frame generation is unavailable in this build.");
        else if((backend==2)!=owned)ImGui::TextWrapped("Save the presenter choice and restart to activate it.");
        ImGui::BeginDisabled(!built || !owned);
        changed |= ImGui::Checkbox("FSR frame generation",&requested);
        ImGui::EndDisabled();
        if(owned && backend!=2) ImGui::TextWrapped("The current FSR FG presenter remains active. The testing presenter choice is pending until restart.");
        ImGui::TextWrapped("On/off takes effect immediately without changing the backend. Save as default to keep it for the next launch.");
        ImGui::TextWrapped("Generation uses completed native UI and valid temporal guides. Menus, loading, invalid guides and source stalls use real frames.");
        ImGui::PopID();return changed;
    }
}
