#pragma once
#include <cstddef>
#include <cstdint>

namespace TheosRenderPipeline::NvidiaAppSettings {
// NVDRS_SETTING V1 public ABI (NVIDIA/nvapi). Newer versions retain this prefix.
#pragma pack(push, 4)
struct DrsSetting {
    std::uint32_t version;
    std::uint16_t name[2048];
    std::uint32_t id, type, location, currentPredefined, predefinedValid;
    std::uint8_t predefined[4100];
    std::uint32_t current;
    std::uint8_t currentTail[4096];
};
#pragma pack(pop)
static_assert(sizeof(DrsSetting) == 0x3020);
static_assert(offsetof(DrsSetting, current) == 0x201c);
inline bool ApplicationValue(std::uint32_t id, std::uint32_t& value) noexcept {
    value=0;
    switch(id) {
    // Published NVAPI override enable/preset/scaling settings. Zero disables overrides.
    case 0x10e41e01: case 0x10e41e03: case 0x10e41e04: case 0x10e41e05: case 0x10e41e06:
    case 0x10e41df1: case 0x10e41df3: case 0x10e41df8: case 0x10e41df5:
    case 0x10308298: case 0x104d6667: case 0x10562d0f: case 0x10cf4125:
    case 0x10afb76c: case 0x10e41df4: return true;
    case 0x10afb768: value=3; return true; // NGX_DLSS_SR_MODE_SNIPPET_CONTROLLED
    default: return false;
    }
}
inline bool FilterSetting(int status, std::uint32_t id, DrsSetting* setting) noexcept {
    std::uint32_t value{};
    if(status!=0 || !setting || (setting->version&0xffff)<sizeof(DrsSetting) ||
        !(setting->version>>16) || setting->type!=0 || setting->id!=id ||
        !ApplicationValue(id,value) || setting->current==value) return false;
    setting->current=value;
    return true;
}
}
