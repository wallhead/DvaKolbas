#include <cstdio>
#include "NeuralRendering/FsrRouteDiagnostics.h"
using namespace TheosRenderPipeline::NeuralRendering;
int main(){
    FsrRouteDiagnostics diagnostics;
    unsigned notifications{};
    const auto observe=[&](bool evaluated,bool lease,bool reshade,bool probe,FsrNrRoute expected,bool changed){
        auto event=diagnostics.Observe(evaluated,lease,reshade,probe);
        if(bool(event)!=changed || (event&&(event->route!=expected||event->reshadeBefore!=reshade)))return false;
        notifications+=bool(event);return true;
    };
    if(!observe(false,false,false,false,FsrNrRoute::Bypassed,true))return 1;
    for(unsigned i=0;i<1000;++i)if(!observe(false,false,false,false,FsrNrRoute::Bypassed,false))return 1;
    if(!observe(true,true,false,false,FsrNrRoute::PreparedLinear,true))return 1;
    for(unsigned i=0;i<1000;++i)if(!observe(true,true,false,false,FsrNrRoute::PreparedLinear,false))return 1;
    if(!observe(true,false,true,false,FsrNrRoute::EncodedForReShadeBefore,true))return 1;
    if(!observe(true,false,true,true,FsrNrRoute::EncodedForDiagnostic,true))return 1;
    if(!observe(true,true,true,false,FsrNrRoute::PreparedLinear,true))return 1; // actual lease beats a requested setting
    if(!observe(true,false,false,false,FsrNrRoute::Encoded,true))return 1;
    if(!observe(false,false,true,true,FsrNrRoute::Bypassed,true))return 1;
    diagnostics.Reset();
    if(!observe(false,false,true,true,FsrNrRoute::Bypassed,true)||notifications!=8)return 1;
    std::puts("PASS actual lease, encoded/probe/bypass route selection and change-only notifications across reset");return 0;
}
