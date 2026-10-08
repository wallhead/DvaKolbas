#pragma once
#include "Upscaling/FSRHistoryPolicy.h"
#include <Windows.h>
namespace TheosRenderPipeline::NeuralRendering {
inline bool SourceWorldEligible(bool world,bool nativeUIHandoff,bool dedicatedUI,bool externalWorld)
{ return world && nativeUIHandoff && dedicatedUI && !externalWorld; }
inline bool SourceResetAfterNr(bool sourceReset,bool evaluated,bool previouslyActive,bool nrReset)
{return sourceReset || ((evaluated || previouslyActive) && nrReset);}
inline bool SourceResetForNrSettings(bool sourceReset,bool changed,bool beforeUpscaling,bool placementChanged)
{return sourceReset || (changed && (beforeUpscaling || placementChanged));}
template<class RetireNeural,class RetirePresentation>
HRESULT RetireBeforeSourceResize(RetireNeural neural,RetirePresentation presentation)
{ const HRESULT result=neural(); return FAILED(result)?result:presentation(); }
class SourceCameraHistory {
public:
    Upscaling::HistoryDecision Accept(uint64_t source,const Upscaling::CameraMeasurements& camera,Upscaling::Extent extent,bool eligible=true)
    {return history_.Accept(source,camera,extent,false,!eligible);}
    void Invalidate(){history_.Invalidate();}
private:Upscaling::FsrHistoryPolicy history_;
};
}
