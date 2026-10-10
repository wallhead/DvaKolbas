#include "FrameGen/XessGenerationPolicy.h"
#include "XessFgFrameFixture.h"
#include <cstdio>
#include <cstdlib>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
static void Require(bool value,const char* reason) { if (!value) { std::fprintf(stderr,"FAIL: %s\n",reason);std::exit(1); } }
int main()
{
    XessGenerationHistory history;auto frame=XessFgFrame();
    auto decision=history.Decide(frame,UpscaleOutcome::Temporal,true,true,false);
    Require(decision.presentReal && decision.tag && decision.reset && !decision.generate,"first 13-FPS source warms history without FPS gate");
    history.Accept(999,7);frame.sourceId=2;
    Require(history.Decide(frame,UpscaleOutcome::Temporal,true,true,false).reset,"wrong acknowledgment cannot advance history");
    history.Accept(2,7);frame.sourceId=3;
    Require(history.Decide(frame,UpscaleOutcome::Temporal,true,true,false).generate,"accepted complete low-FPS source generates");
    history.Accept(3,7);
    decision=history.Decide(frame,UpscaleOutcome::Temporal,true,true,false);
    Require(decision.presentReal && !decision.tag && !decision.generate,"duplicate source presents real without invented tags");
    for (const auto outcome:{UpscaleOutcome::SpatialRecovery,UpscaleOutcome::RepeatedOutput,UpscaleOutcome::SkippedInvalidInput,UpscaleOutcome::Fatal}) {
        ++frame.sourceId;decision=history.Decide(frame,outcome,true,true,false);
        Require(!decision.generate && !decision.tag,"non-temporal/repeated sources never generate");
        Require(decision.presentReal==(outcome==UpscaleOutcome::SpatialRecovery || outcome==UpscaleOutcome::RepeatedOutput),"only completed real outcomes present");
    }
    for (int kind=0;kind<3;++kind) {
        ++frame.sourceId;decision=history.Decide(frame,UpscaleOutcome::Temporal,kind!=0,kind!=1,kind==2);
        Require(decision.presentReal && !decision.tag && !decision.generate && decision.reset,"off/incomplete HUD/menu suppress and reset");
    }
    ++frame.sourceId;frame.sourceEpoch=8;
    Require(history.Decide(frame,UpscaleOutcome::Temporal,true,true,false).reset,"epoch change resets");
    history.Accept(frame.sourceId,8);++frame.sourceId;
    Require(history.Decide(frame,UpscaleOutcome::Temporal,true,true,false).generate,"resume after accepted epoch");
    history.Accept(frame.sourceId,8);frame.sourceId+=2;
    Require(history.Decide(frame,UpscaleOutcome::Temporal,true,true,false).reset,"source gap resets");
    history.Invalidate();++frame.sourceId;
    Require(!history.Decide(frame,UpscaleOutcome::Temporal,true,true,false).generate,"explicit invalidation suppresses first generation");
    std::puts("PASS: Intel FG accepted-source admission and history");
}
