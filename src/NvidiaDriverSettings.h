#pragma once
#include "NvidiaAppSettingsPolicy.h"
#include <array>
#include <algorithm>
#include <cstring>
#include <string_view>

namespace TheosRenderPipeline::NvidiaAppSettings {
// Public NVAPI DRS V1 ABI. Keep diagnostics independent of the optional SDK.
struct DrsApplication {
    std::uint32_t version{},predefined{};
    std::uint16_t name[2048]{},friendlyName[2048]{},launcher[2048]{};
};
static_assert(sizeof(DrsApplication)==12296);
struct DriverSettingsApi {
    int (__cdecl* create)(void**){};
    int (__cdecl* destroy)(void*){};
    int (__cdecl* load)(void*){};
    int (__cdecl* find)(void*,std::uint16_t*,void**,DrsApplication*){};
    int (__cdecl* global)(void*,void**){};
    int (__cdecl* read)(void*,void*,std::uint32_t,DrsSetting*){};
    int (__cdecl* base)(void*,void**){};
    int (__cdecl* availableIds)(std::uint32_t*,std::uint32_t*){};
};
enum class DriverSettingSupport { Unknown, Listed, NotListed };
enum class DriverProfile { Application, Global, Base, Unknown };
struct DriverSettingRead {
    int status{-3};
    std::uint32_t value{},location{},predefined{};
    bool predefinedValid{},currentPredefined{};
};
struct DriverSettingValue {
    std::uint32_t id{};
    const char* name{};
    int status{-160};
    std::uint32_t value{},location{};
    DriverProfile source{DriverProfile::Unknown};
    DriverSettingSupport support{DriverSettingSupport::Unknown};
    std::array<DriverSettingRead,3> reads{};
    bool absenceConfirmed{};
};
struct DriverSettingsSnapshot {
    int status{-1};
    bool globalProfile{};
    std::array<int,3> profileStatus{{-3,-3,-3}};
    int enumerationStatus{-3};
    std::uint32_t availableSettingCount{};
    std::array<DriverSettingValue,6> values{{
        {0xB0D384C0,"Smooth Motion enable"},{0xB0CC0875,"Smooth Motion API mask"},
        {0x00DD48FB,"RTX HDR"},{0x00980880,"RTX Dynamic Vibrance"},
        {0x10835002,"Frame rate limiter"},{0x00634291,"DLSS forced model profile"}
    }};
};
enum class DriverConflict { Unknown,Disabled,Enabled };
inline const char* DriverProfileName(DriverProfile profile) noexcept {
    switch(profile) {
    case DriverProfile::Application:return "application";
    case DriverProfile::Global:return "global";
    case DriverProfile::Base:return "base";
    default:return "unknown";
    }
}
inline const char* DriverLocationName(std::uint32_t location) noexcept {
    switch(location) {
    case 0:return "current profile";
    case 1:return "global profile";
    case 2:return "base profile";
    case 3:return "driver default";
    default:return "unknown";
    }
}
inline const char* DriverSettingSupportName(DriverSettingSupport support) noexcept {
    switch(support) {
    case DriverSettingSupport::Listed:return "listed";
    case DriverSettingSupport::NotListed:return "not listed (private or unavailable)";
    default:return "unknown";
    }
}
inline const char* SmoothMotionNotice(DriverConflict status, bool presentationInterposerLoaded = false) noexcept {
    if(status==DriverConflict::Unknown&&presentationInterposerLoaded)
        return "Disable NVIDIA Smooth Motion for Skyrim.\n"
            "NVIDIA's presentation interposer is loaded, but the Smooth Motion setting could not be read. This is a possible double frame generation conflict.\n"
            "If Smooth Motion is on, it can stack with RaZkolbaS DLSS or FSR frame generation. Module presence alone does not prove it is active.\n"
            "NVIDIA App -> Graphics -> Program settings -> Skyrim -> Driver Settings -> Smooth Motion: Off.\n"
            "Then restart Skyrim. RaZkolbaS has not changed this driver setting.";
    if(status!=DriverConflict::Enabled)return nullptr;
    return "Disable NVIDIA Smooth Motion for Skyrim.\n"
        "Smooth Motion is configured for DX11 and can stack with RaZkolbaS DLSS or FSR frame generation.\n"
        "NVIDIA App -> Graphics -> Program settings -> Skyrim -> Driver Settings -> Smooth Motion: Off.\n"
        "Then restart Skyrim. RaZkolbaS has not changed this driver setting.";
}
inline DriverSettingsSnapshot InspectDriverSettings(const DriverSettingsApi& api,std::wstring_view application) {
    DriverSettingsSnapshot result;
    if(!api.create||!api.destroy||!api.load||!api.find||!api.global||!api.read||application.empty()||application.size()>=2048)
        return result;
    void* session{};
    result.status=api.create(&session);
    if(result.status!=0||!session){if(result.status==0)result.status=-1;return result;}
    struct Session {void* value;const DriverSettingsApi& api;~Session(){api.destroy(value);}} held{session,api};
    result.status=api.load(session);
    if(result.status!=0)return result;
    std::array<std::uint16_t,2048> name{};
    for(std::size_t i=0;i<application.size();++i)name[i]=static_cast<std::uint16_t>(application[i]);
    DrsApplication app;app.version=sizeof(app)|0x10000;
    std::array<void*,3> profiles{};
    auto validProfile=[](int status,void* profile){return status==0&&!profile?-1:status;};
    result.profileStatus[0]=api.find(session,name.data(),&profiles[0],&app);
    result.profileStatus[0]=validProfile(result.profileStatus[0],profiles[0]);
    result.profileStatus[1]=api.global(session,&profiles[1]);
    result.profileStatus[1]=validProfile(result.profileStatus[1],profiles[1]);
    if(api.base) {
        result.profileStatus[2]=api.base(session,&profiles[2]);
        result.profileStatus[2]=validProfile(result.profileStatus[2],profiles[2]);
    }
    // Fully qualified lookup returns the profile applied to this installation.
    // A basename retry could instead match another path-conditioned association.
    result.globalProfile=result.profileStatus[0]==-166;
    result.status=result.profileStatus[result.globalProfile?1:0];
    // Enumeration is supporting evidence only: private settings can be readable
    // even when absent from this public list. Bound memory and calls at startup.
    if(api.availableIds) {
        std::array<std::uint32_t,8192> ids{};
        auto count=static_cast<std::uint32_t>(ids.size());
        result.enumerationStatus=api.availableIds(ids.data(),&count);
        if(result.enumerationStatus==0&&count>ids.size())result.enumerationStatus=-1;
        if(result.enumerationStatus==0) {
            result.availableSettingCount=count;
            for(auto& value:result.values)
                value.support=std::find(ids.begin(),ids.begin()+count,value.id)!=ids.begin()+count?
                    DriverSettingSupport::Listed:DriverSettingSupport::NotListed;
        }
    }
    for(auto& value:result.values) {
        for(std::size_t i=0;i<profiles.size();++i) {
            auto& read=value.reads[i];
            read.status=result.profileStatus[i];
            if(read.status!=0)continue;
            DrsSetting setting{};setting.version=sizeof(setting)|0x10000;
            read.status=api.read(session,profiles[i],value.id,&setting);
            if(read.status==0) {
                if(setting.id!=value.id||setting.type!=0||setting.location>3||
                    (setting.version&0xffff)<sizeof(setting)||!(setting.version>>16)) {
                    read.status=-1;continue;
                }
                read.value=setting.current;read.location=setting.location;
                read.predefinedValid=setting.predefinedValid!=0;
                read.currentPredefined=setting.currentPredefined!=0;
                if(read.predefinedValid)std::memcpy(&read.predefined,setting.predefined,sizeof(read.predefined));
            }
        }
        // Fall back only on absence. An access, lookup or ABI error must not be
        // replaced with a lower-priority value that could falsely establish Off.
        std::size_t i=result.globalProfile?1:0;
        while(i<2&&value.reads[i].status==-160)++i;
        const auto& selected=value.reads[i];
        value.status=selected.status;
        if(selected.status==0) {
            value.value=selected.value;value.location=selected.location;
            value.source=static_cast<DriverProfile>(i);
        }else if(i==2&&selected.status==-160) {
            value.absenceConfirmed=true;
        }
    }
    return result;
}
inline DriverConflict SmoothMotionDx11Configured(const DriverSettingsSnapshot& settings) {
    const auto& enabled=settings.values[0];const auto& mask=settings.values[1];
    if(settings.status!=0||enabled.status!=0)return DriverConflict::Unknown;
    if(enabled.value==0)return DriverConflict::Disabled;
    if(enabled.value!=1)return DriverConflict::Unknown;
    // Community-documented DRS API mask: a missing key permits all APIs.
    if(mask.status==-160&&mask.absenceConfirmed)return DriverConflict::Enabled;
    if(mask.status!=0)return DriverConflict::Unknown;
    return (mask.value&2)?DriverConflict::Enabled:DriverConflict::Disabled;
}
}
