#pragma once
#include "Upscaling/UpscalerBackend.h"
namespace TheosRenderPipeline
{
    enum class GenerationBackendPreference { Auto, Nvidia, Fsr };
    inline long ResolveGenerationBackend(GenerationBackendPreference preference,
        Upscaling::BackendKind upscaler, bool ordinaryDiagnostic = false)
    {
        if (upscaler == Upscaling::BackendKind::Fsr && ordinaryDiagnostic) return 0;
        if (preference == GenerationBackendPreference::Nvidia) return 1;
        if (preference == GenerationBackendPreference::Fsr) return 2;
        return upscaler == Upscaling::BackendKind::Fsr ? 2 : 1;
    }
}
