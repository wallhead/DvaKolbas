#include "NvidiaAppSettings.h"
#include "NvidiaAppSettingsPolicy.h"
#include "HookSafety.h"
#include <atomic>
#include <array>
#include <cstdio>
#include <mutex>

namespace TheosRenderPipeline::NvidiaAppSettings {
namespace {
using Resolver=FARPROC(WINAPI*)(HMODULE,LPCSTR);
using Query=void*(__cdecl*)(std::uint32_t);
using GetSetting=int(__cdecl*)(void*,void*,std::uint32_t,DrsSetting*);
struct Record {
    HMODULE module{};
    Resolver resolve{};
    std::atomic<Query> query{};
    std::atomic<GetSetting> get{};
    bool installed{};
};
struct State {
    std::mutex mutex;
    HookSafety::SlotRegistry slots;
    std::array<Record,8> records;
    std::atomic<Log> log{};
    HMODULE nvapi{},self{};
    std::atomic<unsigned> reports{};
};
State& Get(){static auto* state=new State;return *state;}
void Report(const char* message) noexcept {
    if(auto log=Get().log.load()) {try {log(message);}catch(...) {}}
}
template<class T> bool Bind(std::atomic<T>& slot,T function) {
    T empty{};return slot.compare_exchange_strong(empty,function)||empty==function;
}
template<std::size_t I> int __cdecl ReadSetting(void* session,void* profile,std::uint32_t id,DrsSetting* setting) {
    const auto real=Get().records[I].get.load();
    if(!real)return -1;
    const auto status=real(session,profile,id,setting);
    const auto previous=(status==0&&setting&&(setting->version&0xffff)>=sizeof(DrsSetting))?setting->current:0;
    if(FilterSetting(status,id,setting) && Get().reports.fetch_add(1)<32) {
        char text[160]{};std::snprintf(text,sizeof(text),"driver override read 0x%08X: %u -> %u; application settings retained (no profile write)",id,previous,setting->current);
        Report(text);
    }
    return status;
}
template<std::size_t I> void* __cdecl QueryInterface(std::uint32_t id) {
    const auto real=Get().records[I].query.load();
    if(!real)return nullptr;
    auto* address=real(id);
    if(id==0x73bf8338 && address) {
        if(Bind(Get().records[I].get,reinterpret_cast<GetSetting>(address)))return reinterpret_cast<void*>(&ReadSetting<I>);
        Report("NVAPI GetSetting resolver changed; override filter could not bind");
    }
    return address;
}
template<std::size_t I> FARPROC WINAPI Resolve(HMODULE module,LPCSTR name) {
    auto& record=Get().records[I];
    const auto result=record.resolve(module,name);
    if(module==Get().nvapi && result && reinterpret_cast<std::uintptr_t>(name)>65535 &&
        std::strcmp(name,"nvapi_QueryInterface")==0) {
        if(Bind(record.query,reinterpret_cast<Query>(result)))return reinterpret_cast<FARPROC>(&QueryInterface<I>);
        Report("NVAPI query resolver changed; override filter could not bind");
    }
    return result;
}
constexpr std::array<Resolver,8> resolvers{Resolve<0>,Resolve<1>,Resolve<2>,Resolve<3>,Resolve<4>,Resolve<5>,Resolve<6>,Resolve<7>};
std::uintptr_t* ResolverSlot(HMODULE module) {
    std::uintptr_t* found{};
    for(const auto* name:{"kernel32.dll","api-ms-win-core-libraryloader-l1-2-0.dll","api-ms-win-core-libraryloader-l1-2-1.dll"}) {
        auto* candidate=HookSafety::ImportSlot(reinterpret_cast<std::uintptr_t>(module),name,"GetProcAddress");
        if(candidate) {if(found)return nullptr;found=candidate;}
    }
    return found;
}
}
void SetLog(Log log) noexcept {Get().log.store(log);}
bool ProtectModule(HMODULE module) {
    auto& s=Get();std::scoped_lock lock(s.mutex);
    if(!module)return false;
    for(auto& record:s.records)if(record.module==module)return record.installed;
    auto* slot=ResolverSlot(module);
    if(!slot) {Report("NVIDIA module has no unique GetProcAddress import; cannot install override filter");return false;}
    if(!s.nvapi)s.nvapi=LoadLibraryExW(L"nvapi64.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!s.nvapi)return false;
    if(!s.self && !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,reinterpret_cast<LPCWSTR>(&ProtectModule),&s.self))return false;
    // Last reader slot is reserved for the existing checked compatibility wrapper.
    for(std::size_t i=0;i<s.records.size()-1;++i) {
        auto& record=s.records[i];if(record.module)continue;
        HMODULE held{};
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,reinterpret_cast<LPCWSTR>(module),&held))return false;
        // Record remains reserved even on a protection failure: a installed callback
        // may already be callable. Keep its predecessor and module references alive.
        record.module=held;
        if(!s.slots.Install(slot,reinterpret_cast<std::uintptr_t>(resolvers[i]),record.resolve)) {
            Report("NVIDIA override import installation failed; module retained");return false;
        }
        record.installed=true;
        Report("process-local NVIDIA override filter installed; saved driver profiles unchanged");return true;
    }
    Report("NVIDIA override filter module capacity exceeded");return false;
}
bool PrepareCore() {
    // Device creation loads its actual NVIDIA user-mode driver. Use that driver
    // directory rather than picking an arbitrary installed driver version.
    const auto driver=GetModuleHandleW(L"nvwgf2umx.dll");
    std::array<wchar_t,32768> path{};
    const auto count=driver?GetModuleFileNameW(driver,path.data(),static_cast<DWORD>(path.size())):0;
    if(!count || count>=path.size()) {Report("active NVIDIA driver module path unavailable");return false;}
    const auto corePath=std::filesystem::path(path.data()).parent_path()/L"_nvngx.dll";
    const auto core=LoadLibraryExW(corePath.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!core) {Report("cannot preload active driver NGX core for application settings");return false;}
    if(!ProtectModule(core)) {FreeLibrary(core);return false;}
    // ProtectModule holds the core and the callback code until process exit.
    FreeLibrary(core);return true;
}
void* FilterNvapiFunction(std::uint32_t id,void* function){
    if(id==0x73bf8338 && function && Bind(Get().records[7].get,reinterpret_cast<GetSetting>(function)))
        return reinterpret_cast<void*>(&ReadSetting<7>);
    return function;
}
bool PrepareStreamline(const std::filesystem::path& directory,StreamlineResolverOwner owner) {
    if(!directory.is_absolute() || !PrepareCore())return false;
    for(const auto* name:{L"sl.interposer.dll",L"sl.common.dll"}) {
        // Compatibility owns and verifies sl.common's raw resolver import.
        // Its NVAPI wrapper composes FilterNvapiFunction directly instead.
        if(owner==StreamlineResolverOwner::Compatibility && std::wcscmp(name,L"sl.common.dll")==0)continue;
        const auto module=LoadLibraryExW((directory/name).c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if(!module)return false;
        const auto ok=ProtectModule(module);FreeLibrary(module);if(!ok)return false;
    }
    return true;
}
}
