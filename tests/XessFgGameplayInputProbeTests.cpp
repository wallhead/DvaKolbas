#include "FrameGen/XessGenerationEngineHooks.h"
#include <cstdio>
#include <cstdlib>
#include <utility>
using namespace TheosRenderPipeline::XessEngineHooks;
static void Require(bool value,const char* message)
{ if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);} }
int main()
{
    Require(!Qualified({1,6,1170,0},inputBytes,renderBytes),
        "menu-only input witnesses cannot qualify the gameplay probe installation");
    Require(Qualified({1,6,1170,0},inputBytes,renderBytes,gameplayInputBytes),"all three captured sites admitted");
    auto changed=gameplayInputBytes;changed[35]=0xE8;
    Require(!Qualified({1,6,1170,0},inputBytes,renderBytes,changed),"call cannot replace the captured tail jump");
    changed=gameplayInputBytes;changed[34]=0x20;
    Require(!Qualified({1,6,1170,0},inputBytes,renderBytes,changed),"different stack restoration rejected");
    changed=gameplayInputBytes;changed[36]^=1;
    Require(!Qualified({1,6,1170,0},inputBytes,renderBytes,changed),"foreign tail target rejected");
    GameplayInputProbe probe;unsigned stamps{},polls{},markers{};
    Observer sink{&markers,[](void* context,Boundary) noexcept{++*static_cast<unsigned*>(context);}};
    Require(Bind(&sink),"timing observer retained while probe runs");
    probe.PollInput([&]{
        ++polls;
        Require(probe.started.load()==1 && probe.completed.load()==0 && stamps==1,
            "entry recorded before preserved native input, completion still absent");
    },[&]{return std::pair<std::uint32_t,std::uint64_t>{17,++stamps};});
    Require(probe.started.load()==1 && probe.completed.load()==1 && polls==1 &&
        probe.beginTicks.load()==1 && probe.endTicks.load()==2 && probe.endThread.load()==17,
        "preserved input runs once and completion is recorded after return");
    Require(markers==0 && Unbind(&sink),"diagnostic probe cannot fabricate XeSS timing boundaries");
    std::puts("PASS: gameplay input probe witness policy");
}
