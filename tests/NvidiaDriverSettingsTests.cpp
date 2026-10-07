#include "NvidiaDriverSettings.h"
#include <cstdio>
#include <stdexcept>
#include <vector>
using namespace TheosRenderPipeline::NvidiaAppSettings;
namespace {
constexpr std::uint32_t Enable=0xB0D384C0,Mask=0xB0CC0875;
struct Cell {
    int status{-160};
    std::uint32_t value{},location{},type{},version{sizeof(DrsSetting)|0x10000};
    bool wrongId{};
};
struct Fixture {
    int createStatus{},loadStatus{},findStatus{},globalStatus{},baseStatus{},enumStatus{};
    int created{},destroyed{},globalCalls{},baseCalls{},findCalls{},enumCalls{};
    bool nullSession{},nullGlobal{},oversizedEnumeration{};
    std::uint32_t enumCount{2};
    std::array<std::uint32_t,2> ids{{Enable,Mask}};
    std::array<std::array<Cell,6>,3> cells{};
    std::vector<std::pair<std::size_t,std::uint32_t>> requested;
    std::wstring application;
} fixture;
int scenarios{};
void Need(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
void Reset(){fixture=Fixture{};++scenarios;}
int __cdecl Create(void** s){++fixture.created;*s=fixture.nullSession||fixture.createStatus!=0?nullptr:reinterpret_cast<void*>(1);return fixture.createStatus;}
int __cdecl Destroy(void*){++fixture.destroyed;return 0;}
int __cdecl Load(void*){return fixture.loadStatus;}
int __cdecl Find(void*,std::uint16_t* name,void** p,DrsApplication*){
    ++fixture.findCalls;fixture.application.clear();
    for(;*name;++name)fixture.application.push_back(static_cast<wchar_t>(*name));
    *p=reinterpret_cast<void*>(2);return fixture.findStatus;
}
int __cdecl Global(void*,void** p){++fixture.globalCalls;*p=fixture.nullGlobal?nullptr:reinterpret_cast<void*>(3);return fixture.globalStatus;}
int __cdecl Base(void*,void** p){++fixture.baseCalls;*p=reinterpret_cast<void*>(4);return fixture.baseStatus;}
int __cdecl Available(std::uint32_t* ids,std::uint32_t* count){
    ++fixture.enumCalls;
    Need(*count>=fixture.ids.size(),"enumeration buffer must be bounded and sufficient");
    std::copy(fixture.ids.begin(),fixture.ids.end(),ids);
    *count=fixture.oversizedEnumeration?8193:fixture.enumCount;return fixture.enumStatus;
}
int __cdecl Read(void*,void* p,std::uint32_t id,DrsSetting* s){
    const auto profile=static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(p)-2);
    const auto index=id==Enable?0:id==Mask?1:id==0x00DD48FB?2:id==0x00980880?3:id==0x10835002?4:5;
    fixture.requested.emplace_back(profile,id);
    const auto& cell=fixture.cells[profile][index];
    s->id=cell.wrongId?0:id;s->version=cell.version;s->type=cell.type;s->location=cell.location;
    s->current=cell.value;s->predefinedValid=1;s->currentPredefined=1;
    const std::uint32_t predefined=17;std::memcpy(s->predefined,&predefined,sizeof(predefined));
    return cell.status;
}
DriverSettingsApi Api(){return {Create,Destroy,Load,Find,Global,Read,Base,Available};}
DriverSettingsSnapshot Inspect(){return InspectDriverSettings(Api(),L"C:/Game/SkyrimSE.exe");}
void Put(std::size_t profile,std::size_t setting,std::uint32_t value,std::uint32_t location=0){
    fixture.cells[profile][setting]={0,value,location};
}
}
int main(){try{
    Reset();Put(0,0,1);Put(1,0,0);Put(2,0,0);
    auto s=Inspect();
    Need(s.status==0&&!s.globalProfile&&fixture.destroyed==1,"application snapshot owns session cleanup");
    Need(fixture.globalCalls==1&&fixture.baseCalls==1&&fixture.enumCalls==1,"each profile and setting catalog inspected once");
    Need(fixture.requested.size()==18,"each setting inspected once in each available profile");
    Need(s.values[0].source==DriverProfile::Application&&s.values[0].value==1,"application override wins over lower-priority Off");
    Need(s.values[0].support==DriverSettingSupport::Listed&&s.values[2].support==DriverSettingSupport::NotListed,"recognized IDs distinguished from unlisted private IDs");
    Need(s.values[0].reads[0].predefinedValid&&s.values[0].reads[0].predefined==17,"predefined driver value captured independently");
    Need(s.values[1].absenceConfirmed&&SmoothMotionDx11Configured(s)==DriverConflict::Enabled,"mask absent from complete chain permits all APIs when enable is known On");

    Reset();Put(1,0,1);Put(1,1,2);s=Inspect();
    Need(s.values[0].source==DriverProfile::Global&&SmoothMotionDx11Configured(s)==DriverConflict::Enabled,"missing application key inherits global DX11 On");
    Need(s.values[0].reads[0].status==-160&&s.values[0].reads[1].status==0,"raw missing application result remains visible");

    Reset();Put(2,0,1);Put(2,1,2);s=Inspect();
    Need(s.values[0].source==DriverProfile::Base&&SmoothMotionDx11Configured(s)==DriverConflict::Enabled,"missing application/global key inherits base value");

    Reset();Put(0,0,0);Put(1,0,1);s=Inspect();
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Disabled,"explicit application Off overrides global On");

    Reset();Put(0,0,1,1);Put(1,0,0);Put(0,1,2,1);s=Inspect();
    Need(s.values[0].location==1&&s.values[0].value==1,"successful inherited GetSetting result is retained without reinterpretation");

    Reset();fixture.findStatus=-166;Put(1,0,1);Put(1,1,2);s=Inspect();
    Need(s.status==0&&s.globalProfile&&fixture.findCalls==1,"absent executable uses global without ambiguous basename retry");
    Need(fixture.application==L"C:/Game/SkyrimSE.exe"&&fixture.requested.size()==12,"only available profiles read with exact application path");

    Reset();fixture.findStatus=-175;Put(1,0,0);s=Inspect();
    Need(s.status==-175&&SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"application lookup errors must not become global Off");

    Reset();fixture.cells[0][0].status=-175;Put(1,0,0);s=Inspect();
    Need(s.values[0].status==-175&&SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"application setting error must not become global Off");

    Reset();fixture.globalStatus=-175;Put(2,0,0);s=Inspect();
    Need(s.values[0].status==-175&&SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"global lookup error must not fall through to base Off");

    Reset();fixture.cells[1][0].status=-9;Put(2,0,0);s=Inspect();
    Need(s.values[0].status==-9&&SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"global setting error must not fall through to base Off");

    Reset();Put(0,0,1);fixture.cells[1][1].status=-175;s=Inspect();
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"unreadable inherited API mask is not interpreted as all APIs");

    Reset();s=Inspect();
    Need(s.values[0].status==-160&&s.values[0].absenceConfirmed&&SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"missing enable even in complete chain does not prove Off");

    Reset();Put(0,0,1);auto api=Api();api.base=nullptr;s=InspectDriverSettings(api,L"C:/Game/SkyrimSE.exe");
    Need(s.values[1].status==-3&&!s.values[1].absenceConfirmed&&SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"missing base API cannot confirm inherited mask absence");
    Put(0,1,2);s=InspectDriverSettings(api,L"C:/Game/SkyrimSE.exe");
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Enabled,"optional base API does not hide readable application settings");

    Reset();Put(0,0,1);Put(0,1,1);s=Inspect();
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Disabled,"DX12-only mask does not enable DX11");
    Put(0,1,2);s=Inspect();
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Enabled,"DX11 mask bit is honored");
    Put(0,0,2);s=Inspect();
    Need(SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"unexpected enable value is not called Off");

    Reset();fixture.enumCount=0;Put(0,0,1);Put(0,1,2);s=Inspect();
    Need(s.values[0].support==DriverSettingSupport::NotListed&&SmoothMotionDx11Configured(s)==DriverConflict::Enabled,"private readable IDs remain evidence even if not enumerated");

    Reset();fixture.enumStatus=-7;Put(0,0,0);s=Inspect();
    Need(s.values[0].support==DriverSettingSupport::Unknown&&SmoothMotionDx11Configured(s)==DriverConflict::Disabled,"incomplete enumeration is unknown and does not erase readable Off");
    fixture.oversizedEnumeration=true;fixture.enumStatus=0;s=Inspect();
    Need(s.enumerationStatus!=0&&s.values[0].support==DriverSettingSupport::Unknown,"oversized driver enumeration result is rejected");

    Reset();api=Api();api.availableIds=nullptr;Put(0,0,0);
    s=InspectDriverSettings(api,L"C:/Game/SkyrimSE.exe");
    Need(s.enumerationStatus==-3&&s.values[0].support==DriverSettingSupport::Unknown&&
        SmoothMotionDx11Configured(s)==DriverConflict::Disabled,"missing optional enumeration API preserves valid setting reads");

    for(int invalid=0;invalid<5;++invalid) {
        Reset();Put(0,0,0);auto& cell=fixture.cells[0][0];
        if(invalid==0)cell.wrongId=true;
        if(invalid==1)cell.type=1;
        if(invalid==2)cell.version=sizeof(DrsSetting)-1;
        if(invalid==3)cell.version=sizeof(DrsSetting);
        if(invalid==4)cell.location=4;
        Put(1,0,0);s=Inspect();
        Need(s.values[0].status!=0&&SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"malformed driver ABI/value must not become Off via fallback");
    }

    Reset();fixture.nullGlobal=true;s=Inspect();
    Need(s.profileStatus[1]!=0&&SmoothMotionDx11Configured(s)==DriverConflict::Unknown,"success with null global handle is rejected");

    Reset();fixture.loadStatus=-3;s=Inspect();
    Need(s.status==-3&&fixture.destroyed==1&&fixture.findCalls==0,"load failure still destroys session before profile lookup");

    Reset();fixture.createStatus=-1;s=Inspect();
    Need(s.status==-1&&fixture.destroyed==0&&fixture.findCalls==0,"failed session creation cannot proceed or destroy an invalid handle");

    Reset();fixture.nullSession=true;s=Inspect();
    Need(s.status!=0&&fixture.destroyed==0,"null session is rejected without invalid destroy");

    Reset();api=Api();api.destroy=nullptr;s=InspectDriverSettings(api,L"C:/Game/SkyrimSE.exe");
    Need(s.status!=0&&fixture.created==0,"incomplete lifecycle cannot allocate an unowned session");
    api=Api();s=InspectDriverSettings(api,L"");
    Need(s.status!=0&&fixture.created==0,"empty executable path cannot accidentally inspect a different profile");

    Need(SmoothMotionNotice(DriverConflict::Enabled)!=nullptr,"configured On provides disable instructions");
    Need(SmoothMotionNotice(DriverConflict::Disabled)==nullptr&&SmoothMotionNotice(DriverConflict::Unknown)==nullptr,"unknown is not falsely called enabled");
    const auto* possible=SmoothMotionNotice(DriverConflict::Unknown,true);
    Need(possible&&std::string_view(possible).find("possible double frame generation")!=std::string_view::npos&&
        std::string_view(possible).find("could not be read")!=std::string_view::npos&&
        std::string_view(possible).find("Disable NVIDIA Smooth Motion for Skyrim")!=std::string_view::npos,"loaded interposer warning states uncertainty and actionable instructions");
    Need(SmoothMotionNotice(DriverConflict::Disabled,true)==nullptr,"module presence alone cannot override known Off");
    std::printf("PASS: %d driver diagnostics scenarios; inheritance, errors, private IDs, ABI and cleanup; no profile write API\n",scenarios);return 0;
}catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
