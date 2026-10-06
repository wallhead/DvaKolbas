#pragma once
#include "FrameGen/SourceDLSSGSettings.h"
#include "UpscaleType.h"
#include "Upscaling/FSRSettings.h"
namespace TheosRenderPipeline::NeuralRendering {
inline const char* NativeBeforeUnavailable(const SourceDLSSG::Preferences& p)
{
    if (p.neuralPasses < 1 || p.neuralPasses > 3) return "Community NR supports one to three passes.";
    const auto& r=p.neuralReconstruction;
    if (r.preset!=0 || r.inputScale!=1 || r.method>ResolveMethod::Ratio || EffectiveResolve(r)!=ResolveMethod::Auto ||
        r.colorIsHDR || r.producerColor || r.fusedPreparation || r.peripheralCompression)
        return "This NR trial requires the native SDR model with reconstruction off.";
    if (p.hdrOutput.enabled) return "HDR NR is not available in this trial.";
    if (p.neuralTuning.uiCorrection) return "NR processes the world before UI composition; UI correction must be off.";
    for(int i=1;i<p.neuralPasses;++i){
        const auto pass=EffectiveSecondPass(i==1?p.neuralSecondPass:p.neuralThirdPass,p.neuralReconstruction,p.neuralTuning);
        if(pass.inputScale!=1 || pass.preset!=0)return "Community NR passes require native SDR reconstruction settings.";
        if(pass.tuning.uiCorrection)return "NR processes the world before UI composition; UI correction must be off.";
    }
    return nullptr;
}
inline const char* NativeAfterUnavailable(const SourceDLSSG::Preferences& p,int mode,Upscaling::Quality quality,bool dynamicResolution){
    if(p.neuralBeforeUpscaling)return nullptr;
    if(dynamicResolution)
        return "After upscaling NR requires a fixed render scale; disable dynamic resolution.";
    if(mode!=DLAA && mode!=DLSS && mode!=FSR)
        return "After upscaling NR requires DLSS or FSR reconstruction.";
    if(mode==FSR && (quality<Upscaling::Quality::Quality || quality>Upscaling::Quality::NativeAA))
        return "After upscaling NR requires a supported FSR render scale.";
    return nullptr;
}
}
