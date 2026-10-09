#pragma once
#include "Upscaling/UpscalerBackend.h"
namespace TheosRenderPipeline
{
    enum class GenerationBackendPreference { Auto, Nvidia, Fsr };
    inline GenerationBackendPreference NormalizeGenerationBackendPreference(GenerationBackendPreference preference,
        Upscaling::BackendKind upscaler)
    {
        return upscaler==Upscaling::BackendKind::Xess && preference==GenerationBackendPreference::Nvidia?
            GenerationBackendPreference::Auto:preference;
    }
    inline long ResolveGenerationBackend(GenerationBackendPreference preference,
        Upscaling::BackendKind upscaler, bool ordinaryDiagnostic = false)
    {
        // Auto preserves the SR-only XeSS path until both presenters qualify.
        // Explicit FSR retains its presenter even while interpolation is off.
        if(upscaler==Upscaling::BackendKind::Xess)return preference==GenerationBackendPreference::Fsr?2:
            preference==GenerationBackendPreference::Nvidia?1:0;
        if (upscaler == Upscaling::BackendKind::Fsr && ordinaryDiagnostic) return 0;
        if (preference == GenerationBackendPreference::Nvidia) return 1;
        if (preference == GenerationBackendPreference::Fsr) return 2;
        return upscaler == Upscaling::BackendKind::Fsr ? 2 : 1;
    }
}
