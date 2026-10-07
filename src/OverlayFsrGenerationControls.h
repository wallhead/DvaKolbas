#pragma once
#include <imgui.h>
#include "FrameGen/GenerationBackendPreference.h"

namespace TheosRenderPipeline::Overlay
{
    inline bool DrawGenerationBackendChoice(GenerationBackendPreference& preference,bool nvidiaSupported)
    {
        const char* labels[]{"Auto", "NVIDIA FG", "FSR FG"};
        const int current=static_cast<int>(preference);bool changed{};
        ImGui::SetNextItemWidth(-1.0f);
        if(ImGui::BeginCombo("Backend##generation",labels[current>=0 && current<3?current:0])) {
            for(int i=0;i<3;++i) {
                ImGui::BeginDisabled(i==1 && !nvidiaSupported);
                if(ImGui::Selectable(labels[i],current==i)){preference=static_cast<GenerationBackendPreference>(i);changed=true;}
                ImGui::EndDisabled();
            }
            ImGui::EndCombo();
        }
        return changed;
    }
    inline bool DrawFsrProviderChoice(const char* label, int& selected, const char* const* names,
                                     bool mlAvailable, const char* reason)
    {
        bool changed{};
        ImGui::SetNextItemWidth(-1.0f);
        if(ImGui::BeginCombo(label,names[selected>=0 && selected<3?selected:0])) {
            for(int i=0;i<3;++i) {
                ImGui::BeginDisabled(i==2 && !mlAvailable);
                if(ImGui::Selectable(names[i],selected==i)) {selected=i;changed=true;}
                ImGui::EndDisabled();
            }
            ImGui::EndCombo();
        }
        if(reason && *reason)ImGui::TextWrapped("%s",reason);
        return changed;
    }
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
