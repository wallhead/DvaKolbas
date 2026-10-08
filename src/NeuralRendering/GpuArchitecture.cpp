#include "GpuArchitecture.h"
#include "GpuProductName.h"
#include <Windows.h>
#include <algorithm>
#include <string_view>
namespace TheosRenderPipeline::NeuralRendering {
void DiscoverGpuArchitecture(AdapterIdentity& adapter,const GpuArchitectureApi& api) {
    adapter.architecture={};
    if(adapter.vendorId!=0x10de || adapter.software || !adapter.luid.Valid() ||
        !api.enumerate || !api.logicalInfo || !api.archInfo || !api.fullName) return;
    std::array<void*,64> logicals{};
    uint32_t count{};
    if(api.enumerate(logicals.data(),&count)!=0 || count>logicals.size()) return;
    bool matched{};
    for(uint32_t i=0;i<count;++i) {
        AdapterLuid luid{};
        NvLogicalGpuData info{};
        info.version=sizeof(info)|(1u<<16);info.osAdapterId=&luid;
        if(!logicals[i] || api.logicalInfo(logicals[i],&info)!=0 || luid!=adapter.luid) continue;
        // Multi-node or ambiguous mappings have no reviewed single-renderer NR model.
        if(matched || info.physicalGpuCount!=1 || !info.physicalGpus[0]) {
            adapter.architecture={adapter.luid,0,false,true,matched || info.physicalGpuCount>1};return;
        }
        matched=true;
        NvGpuArchInfo arch{sizeof(NvGpuArchInfo)|(2u<<16)};
        auto result=api.archInfo(info.physicalGpus[0],&arch);
        if(result==-9) { // NVAPI_INCOMPATIBLE_STRUCT_VERSION; same layout in public V1.
            arch={sizeof(NvGpuArchInfo)|(1u<<16)};
            result=api.archInfo(info.physicalGpus[0],&arch);
        }
        std::array<char,64> name{};
        if(result!=0 || api.fullName(info.physicalGpus[0],name.data())!=0) continue;
        name.back()='\0';
        const std::string_view product{name.data()};
        // The name gates RTX hardware only; family comes from the driver's arch ID.
        // This prevents GTX 16 (also Turing) from using the RTX 20 model.
        const bool rtx=IsRtxProductName(product);
        adapter.architecture={adapter.luid,arch.architecture,rtx,true};
    }
}
void DiscoverGpuArchitecture(AdapterIdentity& adapter) {
    adapter.architecture={};
    if(adapter.vendorId!=0x10de || adapter.software || !adapter.luid.Valid()) return;
    std::array<wchar_t,MAX_PATH> directory{};
    const auto length=GetSystemDirectoryW(directory.data(),static_cast<UINT>(directory.size()));
    if(!length || length>=directory.size()) return;
    const auto path=std::filesystem::path(directory.data())/L"nvapi64.dll";
    struct Module {HMODULE value;~Module(){if(value)FreeLibrary(value);}} module{
        LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32)};
    using Query=void*(__cdecl*)(uint32_t);
    const auto query=module.value?reinterpret_cast<Query>(GetProcAddress(module.value,"nvapi_QueryInterface")):nullptr;
    if(!query) return;
    const auto init=reinterpret_cast<int(__cdecl*)()>(query(0x0150e828));
    const auto unload=reinterpret_cast<int(__cdecl*)()>(query(0xd22bdd7e));
    if(!init || !unload || init()!=0) return;
    struct Lifetime {int(__cdecl* unload)();~Lifetime(){unload();}} lifetime{unload};
    const GpuArchitectureApi api{
        reinterpret_cast<decltype(GpuArchitectureApi::enumerate)>(query(0x48b3ea59)),
        reinterpret_cast<decltype(GpuArchitectureApi::logicalInfo)>(query(0x842b066e)),
        reinterpret_cast<decltype(GpuArchitectureApi::archInfo)>(query(0xd8265d24)),
        reinterpret_cast<decltype(GpuArchitectureApi::fullName)>(query(0xceee8e9f))};
    DiscoverGpuArchitecture(adapter,api);
}
}
