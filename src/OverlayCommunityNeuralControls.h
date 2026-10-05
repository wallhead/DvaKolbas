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
        preferences.neuralPasses=1;
        preferences.neuralTuning.uiCorrection=false;
        auto& reconstruction=preferences.neuralReconstruction;
        reconstruction.preset=0;reconstruction.inputScale=1;
        reconstruction.method=NeuralRendering::ResolveMethod::Auto;
        reconstruction.colorIsHDR=false;reconstruction.producerColor=false;
        reconstruction.fusedPreparation=false;reconstruction.peripheralCompression=false;
        preferences.hdrOutput.enabled=false;
        if(placementError) preferences.neuralBeforeUpscaling=true;
    }
}
}
