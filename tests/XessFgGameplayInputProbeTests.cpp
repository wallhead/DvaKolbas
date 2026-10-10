#include "FrameGen/XessGenerationEngineHooks.h"
#include <cstdio>
#include <cstdlib>
#include <utility>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
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
    auto relocated=RelocateGameplayEntry(0x140647410,0x140700000,gameplayInputBytes);
    Require(bool(relocated),"complete verified prologue has a callable relocated predecessor");
    std::int32_t originalDisp{},newDisp{};
    std::memcpy(&originalDisp,gameplayInputBytes.data()+7,4);
    std::memcpy(&newDisp,relocated->data()+7,4);
    Require(0x140647410ull+11+originalDisp==0x140700000ull+11+newDisp,
        "relocated RIP load reads the exact original singleton slot");
    std::uint64_t resume{};std::memcpy(&resume,relocated->data()+17,8);
    Require(resume==0x14064741Bull && (*relocated)[11]==0xFF && (*relocated)[12]==0x25,
        "relocated predecessor resumes original job before its first call without clobbering registers");
    Require(!RelocateGameplayEntry(0x140647410,0x240700000,gameplayInputBytes) &&
        !RelocateGameplayEntry(0x140647410,0x140700000,changed),"out-of-range and changed job cannot install");
    // Execute the relocated stack/RIP-load/resume contract against a synthetic
    // singleton slot. Reserve the captured displacement range, committing only
    // two pages; this test neither opens Skyrim nor creates a GPU device.
    const auto reserveSize=static_cast<std::size_t>(originalDisp)+0x3000;
    auto* codeBase=static_cast<std::uint8_t*>(VirtualAlloc(nullptr,reserveSize,MEM_RESERVE,PAGE_NOACCESS));
    Require(codeBase && VirtualAlloc(codeBase,0x1000,MEM_COMMIT,PAGE_READWRITE),"CPU-only predecessor code page");
    auto* slot=codeBase+11+originalDisp;
    const auto slotPage=reinterpret_cast<std::uintptr_t>(slot)&~std::uintptr_t{0xFFF};
    Require(VirtualAlloc(reinterpret_cast<void*>(slotPage),0x1000,MEM_COMMIT,PAGE_READWRITE)!=nullptr,"synthetic singleton slot page");
    const std::uint64_t singleton=0x123456789ABCDEF0ull;std::memcpy(slot,&singleton,sizeof(singleton));
    const std::array<std::uint8_t,8> returnSingleton{0x48,0x89,0xC8,0x48,0x83,0xC4,0x28,0xC3};
    std::memcpy(codeBase+11,returnSingleton.data(),returnSingleton.size());
    auto runnable=RelocateGameplayEntry(reinterpret_cast<std::uintptr_t>(codeBase),reinterpret_cast<std::uintptr_t>(codeBase+128),gameplayInputBytes);
    Require(bool(runnable),"synthetic predecessor relocation");
    std::memcpy(codeBase+128,runnable->data(),runnable->size());DWORD oldProtection{};
    Require(VirtualProtect(codeBase,0x1000,PAGE_EXECUTE_READ,&oldProtection)!=0 &&
        FlushInstructionCache(GetCurrentProcess(),codeBase,0x1000)!=0,"publish CPU-only predecessor");
    auto execute=reinterpret_cast<std::uint64_t(*)()>(codeBase+128);
    Require(execute()==singleton,"actual relocated predecessor preserves singleton load, stack balance and continuation");
    Require(VirtualFree(codeBase,0,MEM_RELEASE)!=0,"CPU-only predecessor fixture released");
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
