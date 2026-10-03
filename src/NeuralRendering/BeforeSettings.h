#pragma once
#include "FrameGen/SourceDLSSGSettings.h"
namespace TheosRenderPipeline::NeuralRendering {
inline const char* NativeBeforeUnavailable(const SourceDLSSG::Preferences& p)
{
    if (!p.neuralBeforeUpscaling) return "After FG is not available in this trial.";
    if (p.neuralPasses != 1) return "This NR trial supports one pass.";
    const auto& r=p.neuralReconstruction;
    if (r.preset!=0 || r.inputScale!=1 || r.method>ResolveMethod::Ratio || EffectiveResolve(r)!=ResolveMethod::Auto ||
        r.colorIsHDR || r.producerColor || r.fusedPreparation || r.peripheralCompression)
        return "This NR trial requires the native SDR model with reconstruction off.";
    if (p.hdrOutput.enabled) return "HDR NR is not available in this trial.";
    if (p.neuralTuning.uiCorrection) return "NR processes the world before UI composition; UI correction must be off.";
    return nullptr;
}
}
