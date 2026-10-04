#pragma once
#include "FrameGen/SourceDLSSGSettings.h"
#include "UpscaleType.h"
#include "Upscaling/FSRSettings.h"
namespace TheosRenderPipeline::NeuralRendering {
inline const char* NativeBeforeUnavailable(const SourceDLSSG::Preferences& p)
{
    if (p.neuralPasses != 1) return "This NR trial supports one pass.";
    const auto& r=p.neuralReconstruction;
    if (r.preset!=0 || r.inputScale!=1 || r.method>ResolveMethod::Ratio || EffectiveResolve(r)!=ResolveMethod::Auto ||
        r.colorIsHDR || r.producerColor || r.fusedPreparation || r.peripheralCompression)
        return "This NR trial requires the native SDR model with reconstruction off.";
    if (p.hdrOutput.enabled) return "HDR NR is not available in this trial.";
    if (p.neuralTuning.uiCorrection) return "NR processes the world before UI composition; UI correction must be off.";
    return nullptr;
}
inline const char* NativeAfterUnavailable(const SourceDLSSG::Preferences& p,int mode,Upscaling::Quality quality,bool dynamicResolution){
    if(p.neuralBeforeUpscaling)return nullptr;
    if(dynamicResolution || (mode!=DLAA && (mode!=FSR || quality!=Upscaling::Quality::NativeAA)))
        return "After upscaling NR requires DLAA or FSR Native AA; scaled guides remain unqualified.";
    return nullptr;
}
}
