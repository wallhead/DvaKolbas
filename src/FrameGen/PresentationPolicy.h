#pragma once
#include "Upscaling/UpscalerBackend.h"
#include <dxgi.h>
#include <type_traits>
namespace TheosRenderPipeline
{
    template<class Nvidia,class Ordinary,class Fsr=std::nullptr_t,class Xess=std::nullptr_t> struct PresentationCreation
    {
        Nvidia nvidia;Ordinary ordinary;Fsr fsr{};Xess xess{};
        HRESULT CreateNvidia(){return nvidia();}HRESULT CreateOrdinary(){return ordinary();}
        HRESULT CreateFsr(){if constexpr(std::is_invocable_r_v<HRESULT,Fsr>)return fsr();else return E_NOTIMPL;}
        HRESULT CreateXess(){if constexpr(std::is_invocable_r_v<HRESULT,Xess>)return xess();else return E_NOTIMPL;}
    };
    template<class Operations> HRESULT CreatePresentation(const Upscaling::BackendDecision& backend,Operations& operations)
    {
        if(!backend.valid)return E_INVALIDARG;
        switch(backend.presentation){
        case Upscaling::PresentationKind::Ordinary:return operations.CreateOrdinary();
        case Upscaling::PresentationKind::Nvidia:return operations.CreateNvidia();
        case Upscaling::PresentationKind::Fsr:if constexpr(requires{operations.CreateFsr();})return operations.CreateFsr();else return E_NOTIMPL;
        case Upscaling::PresentationKind::Xess:if constexpr(requires{operations.CreateXess();})return operations.CreateXess();else return E_NOTIMPL;
        default:return E_INVALIDARG;}
    }
    template<class Operations> bool PresentationReady(const Upscaling::BackendDecision& backend,Operations& operations)
    {
        if(!backend.valid)return false;
        switch(backend.presentation){
        case Upscaling::PresentationKind::Ordinary:return operations.OrdinaryReady();
        case Upscaling::PresentationKind::Nvidia:return operations.NvidiaReady();
        case Upscaling::PresentationKind::Fsr:if constexpr(requires{operations.FsrReady();})return operations.FsrReady();else return false;
        case Upscaling::PresentationKind::Xess:if constexpr(requires{operations.XessReady();})return operations.XessReady();else return false;
        default:return false;}
    }
    template<class Operations> HRESULT RetirePresentation(const Upscaling::BackendDecision& backend,Operations& operations)
    {
        switch(backend.presentation){
        case Upscaling::PresentationKind::Ordinary:return operations.RetireOrdinary();
        case Upscaling::PresentationKind::Nvidia:return operations.RetireNvidia();
        case Upscaling::PresentationKind::Fsr:if constexpr(requires{operations.RetireFsr();})return operations.RetireFsr();else return E_NOTIMPL;
        case Upscaling::PresentationKind::Xess:if constexpr(requires{operations.RetireXess();})return operations.RetireXess();else return E_NOTIMPL;
        default:return E_INVALIDARG;}
    }
}
