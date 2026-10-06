#include "NvidiaDriverSettings.h"
#include <cstdio>
#include <stdexcept>
#include <vector>
using namespace TheosRenderPipeline::NvidiaAppSettings;
namespace {
int findStatus{},loadStatus{},destroyed{},globalCalls{},findCalls{};
std::vector<std::uint32_t> requested;
void Need(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
int __cdecl Create(void** s){*s=reinterpret_cast<void*>(1);return 0;}
int __cdecl Destroy(void*){++destroyed;return 0;}
int __cdecl Load(void*){return loadStatus;}
int __cdecl Find(void*,std::uint16_t*,void** p,DrsApplication*){++findCalls;*p=reinterpret_cast<void*>(2);return findStatus;}
int __cdecl Global(void*,void** p){++globalCalls;*p=reinterpret_cast<void*>(3);return 0;}
int __cdecl Read(void*,void* p,std::uint32_t id,DrsSetting* s){
    requested.push_back(id);s->id=id;s->location=p==reinterpret_cast<void*>(3)?1:0;
    s->current=id==0xB0D384C0?1:7;
    return id==0xB0CC0875?-160:0;
}
}
int main(){try{
    DriverSettingsApi api{Create,Destroy,Load,Find,Global,Read};
    auto s=InspectDriverSettings(api,L"C:/Game/SkyrimSE.exe");
    Need(s.status==0&&!s.globalProfile&&destroyed==1&&globalCalls==0,"application profile read with session cleanup");
    Need(s.values[0].status==0&&s.values[0].value==1,"Smooth Motion enable is observed, not filtered");
    Need(s.values[1].status==-160,"missing API mask stays missing, not incorrectly off");
    Need(requested.size()==s.values.size(),"each diagnostic read exactly once");
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Enabled,"missing Smooth Motion API mask means all APIs when enabled");
    Need(SmoothMotionNotice(DriverConflict::Enabled)!=nullptr,"configured Smooth Motion needs an actionable player-facing notice");
    Need(SmoothMotionNotice(DriverConflict::Disabled)==nullptr&&SmoothMotionNotice(DriverConflict::Unknown)==nullptr,
        "disabled and unknown settings must not falsely tell the player Smooth Motion is enabled");
    s.values[1].status=0;s.values[1].value=1;
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Disabled,"DX12-only mask does not enable DX11");
    s.values[1].value=2;
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Enabled,"DX11 bit is honored");
    s.values[0].status=-9;
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"driver read error must not be called disabled");
    findStatus=-166;findCalls=0;requested.clear();s=InspectDriverSettings(api,L"C:/Game/SkyrimSE.exe");
    Need(s.status==0&&s.globalProfile&&globalCalls==1&&destroyed==2,"absent game profile inherits current global profile");
    Need(findCalls==1,"full-path absence must not substitute a basename profile for another installation");
    findStatus=-9;s=InspectDriverSettings(api,L"C:/Game/SkyrimSE.exe");
    Need(s.status==-9&&globalCalls==1&&destroyed==3,"profile errors are not treated as absent");
    findStatus=0;loadStatus=-3;s=InspectDriverSettings(api,L"SkyrimSE.exe");
    Need(s.status==-3&&destroyed==4,"load failure still destroys session");
    api.destroy=nullptr;s=InspectDriverSettings(api,L"SkyrimSE.exe");
    Need(s.status!=0&&destroyed==4,"incomplete API cannot allocate an unowned session");
    std::puts("PASS: driver diagnostics preserve inheritance, unknown states and cleanup; no profile write API");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
