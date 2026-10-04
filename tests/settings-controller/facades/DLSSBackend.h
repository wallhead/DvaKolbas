#pragma once
#include "FrameGen/SourceDLSSGBackend.h"
#include <dxgiformat.h>
// Feature creation is a recording/fault-injection boundary, not vendor NGX.
class DLSSBackend {
public:
    struct Request {UINT renderWidth{},renderHeight{},outputWidth{},outputHeight{};DXGI_FORMAT format{};bool sharpening{},autoExposure{};int preset{},quality{};}last;
    std::function<bool()> create=[] {return true;};
    static DLSSBackend* GetSingleton(){static DLSSBackend v;return &v;}
    bool InitUpscale(UINT rw,UINT rh,UINT ow,UINT oh,DXGI_FORMAT f,bool s,bool e,int p,int q){
        last={rw,rh,ow,oh,f,s,e,p,q};
        TheosRenderPipeline::SourceDLSSG::Backend::Get().events.push_back("create");return create();
    }
};
