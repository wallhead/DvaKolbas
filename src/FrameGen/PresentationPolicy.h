#pragma once
#include "Upscaling/UpscalerBackend.h"
#include <dxgi.h>
namespace TheosRenderPipeline
{
    template<class Nvidia,class Ordinary> struct PresentationCreation
    {
        Nvidia nvidia;Ordinary ordinary;
        HRESULT CreateNvidia(){return nvidia();}HRESULT CreateOrdinary(){return ordinary();}
    };
    template<class Operations> HRESULT CreatePresentation(const Upscaling::BackendDecision& backend,Operations& operations)
    {
        if(!backend.valid)return E_INVALIDARG;
        return backend.presentation==Upscaling::PresentationKind::Ordinary?operations.CreateOrdinary():operations.CreateNvidia();
    }
    template<class Operations> bool PresentationReady(const Upscaling::BackendDecision& backend,Operations& operations)
    {
        return backend.valid && (backend.presentation==Upscaling::PresentationKind::Ordinary?operations.OrdinaryReady():operations.NvidiaReady());
    }
    template<class Operations> HRESULT RetirePresentation(const Upscaling::BackendDecision& backend,Operations& operations)
    {
        return backend.presentation==Upscaling::PresentationKind::Ordinary?operations.RetireOrdinary():operations.RetireNvidia();
    }
}
