#pragma once
#include "NvidiaAppSettingsPolicy.h"
#include <array>
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
};
struct DriverSettingValue {
    std::uint32_t id{};
    const char* name{};
    int status{-160};
    std::uint32_t value{},location{};
};
struct DriverSettingsSnapshot {
    int status{-1};
    bool globalProfile{};
    std::array<DriverSettingValue,6> values{{
        {0xB0D384C0,"Smooth Motion enable"},{0xB0CC0875,"Smooth Motion API mask"},
        {0x00DD48FB,"RTX HDR"},{0x00980880,"RTX Dynamic Vibrance"},
        {0x10835002,"Frame rate limiter"},{0x00634291,"DLSS forced model profile"}
    }};
};
enum class DriverConflict { Unknown,Disabled,Enabled };
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
    void* profile{};
    result.status=api.find(session,name.data(),&profile,&app);
    // Fully qualified lookup returns the profile applied to this installation.
    // A basename retry could instead match another path-conditioned association.
    if(result.status==-166) {
        result.status=api.global(session,&profile);result.globalProfile=true;
    }
    if(result.status!=0||!profile){if(result.status==0)result.status=-1;return result;}
    for(auto& value:result.values) {
        DrsSetting setting{};setting.version=sizeof(setting)|0x10000;
        value.status=api.read(session,profile,value.id,&setting);
        if(value.status==0) {
            if(setting.id!=value.id||setting.type!=0||(setting.version&0xffff)<sizeof(setting)||!(setting.version>>16)) {value.status=-1;continue;}
            value.value=setting.current;value.location=setting.location;
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
    if(mask.status==-160)return DriverConflict::Enabled;
    if(mask.status!=0)return DriverConflict::Unknown;
    return (mask.value&2)?DriverConflict::Enabled:DriverConflict::Disabled;
}
}
