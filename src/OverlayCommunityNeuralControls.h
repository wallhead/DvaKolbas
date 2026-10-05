#pragma once
#include <imgui.h>
#include "NeuralRendering/BeforeSettings.h"

namespace TheosRenderPipeline::Overlay
{
inline void DrawCommunityNeuralCompatibility(SourceDLSSG::Preferences& preferences, int mode,
                                             Upscaling::Quality quality, bool dynamicResolution)
{
    const auto nativeError=NeuralRendering::NativeBeforeUnavailable(preferences);
    const auto placementError=NeuralRendering::NativeAfterUnavailable(preferences,mode,quality,dynamicResolution);
    if (nativeError) ImGui::TextWrapped("%s",nativeError);
    if (placementError) ImGui::TextWrapped("%s",placementError);
    if ((nativeError || placementError) && ImGui::Button("Use supported native SDR settings")) {
        preferences.neuralPasses=std::clamp(preferences.neuralPasses,1,3);
        preferences.neuralTuning.uiCorrection=false;
        for(auto* pass : {&preferences.neuralSecondPass,&preferences.neuralThirdPass}) {
            pass->tuning.uiCorrection=false;pass->preset=0;pass->inputScale=1;
        }
        auto& reconstruction=preferences.neuralReconstruction;
        reconstruction.preset=0;reconstruction.inputScale=1;
        reconstruction.method=NeuralRendering::ResolveMethod::Auto;
        reconstruction.colorIsHDR=false;reconstruction.producerColor=false;
        reconstruction.fusedPreparation=false;reconstruction.peripheralCompression=false;
        preferences.hdrOutput.enabled=false;
        if(placementError) preferences.neuralBeforeUpscaling=true;
    }
}

inline void DrawCommunityNeuralTuning(NeuralRendering::Tuning& tuning)
{
    const char* styles[]{"Style 0","Style 1","Style 2","Style 3","Style 4","Style 5","Style 6","Style 7"};
    ImGui::Combo("Style",&tuning.style,styles,IM_ARRAYSIZE(styles));
    ImGui::SliderFloat("Intensity",&tuning.intensity,0,2);
    ImGui::SliderFloat("Tone",&tuning.localToneStrength,0,2);
    ImGui::SliderFloat("Structure",&tuning.localStructureStrength,0,2);
    ImGui::SliderFloat("Skin structure",&tuning.skinStructureStrength,-1,2);
    if(ImGui::IsItemHovered()) {
        ImGui::SetTooltip("-1 uses the runtime's automatic skin-structure setting; 0-2 sets its strength.");
    }
    ImGui::Checkbox("Automatic skin mask",&tuning.useAutoSkinMask);
}

inline void DrawCommunityNeuralPassControls(SourceDLSSG::Preferences& preferences)
{
    int selected=std::clamp(preferences.neuralPasses,1,3)-1;
    const char* counts[]{"1","2","3"};
    if(ImGui::Combo("Pass count",&selected,counts,IM_ARRAYSIZE(counts)))
        preferences.neuralPasses=selected+1;
    if(ImGui::CollapsingHeader("Pass 1",ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushID(1);
        DrawCommunityNeuralTuning(preferences.neuralTuning);
        ImGui::PopID();
    }
    for(int pass=2;pass<=std::clamp(preferences.neuralPasses,1,3);++pass) {
        ImGui::PushID(pass);
        if(ImGui::CollapsingHeader(pass==2?"Pass 2":"Pass 3",ImGuiTreeNodeFlags_DefaultOpen)) {
            auto& settings=pass==2?preferences.neuralSecondPass:preferences.neuralThirdPass;
            ImGui::Checkbox("Use Pass 1 settings",&settings.linked);
            // Display the effective values while linked without overwriting
            // independently saved overrides when the link is removed again.
            auto linked=preferences.neuralTuning;
            ImGui::BeginDisabled(settings.linked);
            DrawCommunityNeuralTuning(settings.linked?linked:settings.tuning);
            ImGui::EndDisabled();
        }
        ImGui::PopID();
    }
}
}
