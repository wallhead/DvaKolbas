#include "FrameGen/XessGenerationTelemetry.h"
#include <cstdio>
#include <cstdlib>
using namespace TheosRenderPipeline;
static void Require(bool value,const char* why){if(!value){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
int main() {
    XessGenerationTelemetry totals;Require(!totals.Counter().available,"unobserved output is unavailable");
    totals.Observe(0,1,0);auto first=totals.Counter();
    Require(first.available && first.frames==1 && first.observations==1,"real-only SDK observation counted");
    totals.Observe(0,2,0);auto generated=totals.Counter();
    Require(generated.frames==3 && generated.observations==2 && generated.epoch==first.epoch,"actual generated count accumulated without a requested multiplier");
    totals.Observe(0,2,0,true);Require(totals.Counter().frames==3,"test Present never creates output evidence");
    for(auto badFrames:{0u,3u,7u}){totals.Observe(0,badFrames,0);Require(!totals.Counter().available,"invalid single-generated-frame count unavailable");}
    totals.Observe(0,2,-1);Require(!totals.Counter().available,"negative SDK result unavailable");
    totals.Observe(1,1,0);Require(!totals.Counter().available,"occluded/non-S_OK Present unavailable");
    totals.Observe(0,1,0);auto restarted=totals.Counter();Require(restarted.available && restarted.epoch!=first.epoch,"fresh measurement after interruption has a new epoch");
    Telemetry::OutputRateSampler sampler;
    for(unsigned step=0;step<=10;++step){totals.Observe(0,step%2?2:1,0);sampler.Update(step*100.,totals.Counter());}
    Require(sampler.Rate().available && std::abs(sampler.Rate().fps-15.f)<.01f,"graph rate reflects mixed one/two real SDK counts over measured time");
    totals.Invalidate();sampler.Update(1100,totals.Counter());Require(!sampler.Rate().available,"suspend invalidates graph");
    std::puts("PASS Intel reported output counts, interruptions and measured rate; no raster multiplier");
}
