#pragma once
#include "RuntimeCatalog.h"
#include <array>
namespace TheosRenderPipeline::NeuralRendering {
// Public NVAPI ABI, x64. No SDK import library, private IDs or profile writes.
struct NvLogicalGpuData {
    uint32_t version{};
    void* osAdapterId{};
    uint32_t physicalGpuCount{};
    std::array<void*,64> physicalGpus{};
    std::array<uint32_t,8> reserved{};
};
struct NvGpuArchInfo { uint32_t version{}, architecture{}, implementation{}, revision{}; };
static_assert(sizeof(void*)==8 && sizeof(NvLogicalGpuData)==568 && sizeof(NvGpuArchInfo)==16);
struct GpuArchitectureApi {
    int(__cdecl* enumerate)(void**,uint32_t*){};
    int(__cdecl* logicalInfo)(void*,NvLogicalGpuData*){};
    int(__cdecl* archInfo)(void*,NvGpuArchInfo*){};
    int(__cdecl* fullName)(void*,char*){};
};
void DiscoverGpuArchitecture(AdapterIdentity&,const GpuArchitectureApi&);
}
