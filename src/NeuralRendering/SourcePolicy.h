#pragma once
#include "Upscaling/FSRHistoryPolicy.h"
#include <Windows.h>
namespace TheosRenderPipeline::NeuralRendering {
inline bool SourceWorldEligible(bool world,bool nativeUIHandoff,bool dedicatedUI,bool externalWorld)
{ return world && nativeUIHandoff && dedicatedUI && !externalWorld; }
inline bool SourceResetAfterNr(bool sourceReset,bool evaluated,bool previouslyActive,bool nrReset)
{return sourceReset || ((evaluated || previouslyActive) && nrReset);}
template<class RetireNeural,class RetirePresentation>
HRESULT RetireBeforeSourceResize(RetireNeural neural,RetirePresentation presentation)
{ const HRESULT result=neural(); return FAILED(result)?result:presentation(); }
class SourceCameraHistory {
public:
    Upscaling::HistoryDecision Accept(uint64_t source,const Upscaling::CameraMeasurements& camera,Upscaling::Extent extent)
    {return history_.Accept(source,camera,extent,false,false);}
    void Invalidate(){history_.Invalidate();}
private:Upscaling::FsrHistoryPolicy history_;
};
}
