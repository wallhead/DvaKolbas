// Simulate NVIDIA App's override only in this process. No DRS writes.
#include "NvidiaAppSettings.h"
#include "NvidiaAppSettingsPolicy.h"
#include "HookSafety.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <nvsdk_ngx.h>
#include <sl.h>
#include <sl_core_api.h>
#include <cstdio>
#include <stdexcept>

namespace Policy=TheosRenderPipeline::NvidiaAppSettings;
using Query=void*(__cdecl*)(unsigned);
using Resolve=FARPROC(WINAPI*)(HMODULE,LPCSTR);
using Getter=int(__cdecl*)(void*,void*,unsigned,Policy::DrsSetting*);
using Find=int(__cdecl*)(void*,const wchar_t*,void**,void*);
using Base=int(__cdecl*)(void*,void**);
Query query{};Getter getter{};Resolve resolve{};Find findApp{};Base baseProfile{};
Query compatibilityQuery{};Resolve compatibilityResolve{};
unsigned reads{},suppressed{};bool overrideObserved{};
void Require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int __cdecl FindApp(void* session,const wchar_t* name,void** profile,void* app){
    auto result=findApp(session,name,profile,app);
    if(result==-166 || result==-163)result=baseProfile(session,profile);
    return result;
}
int __cdecl ForcedOverride(void* session,void* profile,unsigned id,Policy::DrsSetting* setting){
    auto result=getter(session,profile,id,setting);
    if(id==0x10e41e03 && (result==0 || result==-160) && setting && setting->type==0) {
        ++reads;setting->id=id;setting->current=1;result=0;
    }
    return result;
}
void* __cdecl FixtureQuery(unsigned id){
    auto* p=query(id);
    if(id==0xeee566b2 && p) {
        findApp=reinterpret_cast<Find>(p);baseProfile=reinterpret_cast<Base>(query(0xda8466a0));
        return reinterpret_cast<void*>(&FindApp);
    }
    if(id==0x73bf8338 && p){getter=reinterpret_cast<Getter>(p);return reinterpret_cast<void*>(&ForcedOverride);}
    return p;
}
FARPROC WINAPI FixtureResolve(HMODULE module,LPCSTR name){
    const auto p=resolve(module,name);
    if(p && reinterpret_cast<std::uintptr_t>(name)>65535 && !std::strcmp(name,"nvapi_QueryInterface")) {
        query=reinterpret_cast<Query>(p);return reinterpret_cast<FARPROC>(&FixtureQuery);
    }
    return p;
}
void* __cdecl CompatibilityQuery(unsigned id){
    auto* p=compatibilityQuery(id);
    if(id==0x73bf8338 && p){getter=reinterpret_cast<Getter>(p);p=reinterpret_cast<void*>(&ForcedOverride);}
    return Policy::FilterNvapiFunction(id,p);
}
FARPROC WINAPI CompatibilityResolve(HMODULE module,LPCSTR name){
    const auto p=compatibilityResolve(module,name);
    if(p && reinterpret_cast<std::uintptr_t>(name)>65535 && !std::strcmp(name,"nvapi_QueryInterface")) {
        compatibilityQuery=reinterpret_cast<Query>(p);return reinterpret_cast<FARPROC>(&CompatibilityQuery);
    }
    return p;
}
void PolicyLog(const char* text){
    if(std::strstr(text,"0x10E41E03: 1 -> 0"))++suppressed;
    std::printf("POLICY %s\n",text);
}
void NVSDK_CONV NgxLog(const char* text,NVSDK_NGX_Logging_Level,NVSDK_NGX_Feature){
    if(std::strstr(text,"Feature dlssg override enabled"))overrideObserved=true;
    if(std::strstr(text,"Feature dlssg override") || std::strstr(text,"feature dlssg snippet:"))std::printf("NGX %s\n",text);
}
template<class T>T Export(HMODULE module,const char* name){
    auto* p=GetProcAddress(module,name);Require(p!=nullptr,"missing runtime export");return reinterpret_cast<T>(p);
}
int wmain(int argc,wchar_t** argv){try {
    Require(argc==4,"expected core, Streamline directory, and control/filtered/streamline mode");
    const std::filesystem::path directory=argv[2];
    Require(directory.is_absolute(),"runtime directory must be absolute");
    const bool filtered=std::wcscmp(argv[3],L"control")!=0;
    const bool compatibility=std::wcscmp(argv[3],L"compatibility")==0;
    const bool streamline=std::wcscmp(argv[3],L"streamline")==0 || compatibility;
    const auto core=LoadLibraryExW(argv[1],nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    Require(core!=nullptr,"cannot load core");
    auto* slot=TheosRenderPipeline::HookSafety::ImportSlot(reinterpret_cast<std::uintptr_t>(core),"kernel32.dll","GetProcAddress");
    TheosRenderPipeline::HookSafety::SlotRegistry fixtureSlots;
    Require(fixtureSlots.Install(slot,reinterpret_cast<std::uintptr_t>(&FixtureResolve),resolve),"cannot install process-only override fixture");
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    Require(SUCCEEDED(D3D12CreateDevice(nullptr,D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device))),"D3D12 device creation failed");
    Policy::SetLog(&PolicyLog);
    std::uintptr_t* compatibilitySlot{};
    if(compatibility) {
        const auto common=LoadLibraryExW((directory/L"sl.common.dll").c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        Require(common!=nullptr,"cannot load compatibility common module");
        compatibilitySlot=TheosRenderPipeline::HookSafety::ImportSlot(reinterpret_cast<std::uintptr_t>(common),"kernel32.dll","GetProcAddress");
        Require(fixtureSlots.Install(compatibilitySlot,reinterpret_cast<std::uintptr_t>(&CompatibilityResolve),compatibilityResolve),"cannot install checked compatibility resolver fixture");
    }
    const auto owner=compatibility?Policy::StreamlineResolverOwner::Compatibility:Policy::StreamlineResolverOwner::Host;
    if(filtered)Require(streamline?Policy::PrepareStreamline(directory,owner):Policy::PrepareCore(),"production filter preparation failed");
    if(compatibility)Require(*compatibilitySlot==reinterpret_cast<std::uintptr_t>(&CompatibilityResolve),"checked compatibility resolver import must remain unchanged");
    if(streamline) {
        const auto interposer=LoadLibraryExW((directory/L"sl.interposer.dll").c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        Require(interposer!=nullptr,"cannot load interposer");
        const wchar_t* paths[]{directory.c_str()};const sl::Feature features[]{sl::kFeatureReflex,sl::kFeatureDLSS_G};
        sl::Preferences preferences{};preferences.pathsToPlugins=paths;preferences.numPathsToPlugins=1;
        preferences.featuresToLoad=features;preferences.numFeaturesToLoad=2;preferences.renderAPI=sl::RenderAPI::eD3D12;
        preferences.engine=sl::EngineType::eCustom;preferences.engineVersion="RaZkolbaS override test";
        preferences.projectId="f1b2e5d8-9c4a-4e7b-8a36-5d2e90c47a11";
        preferences.flags=sl::PreferenceFlags::eDisableCLStateTracking|sl::PreferenceFlags::eUseManualHooking|sl::PreferenceFlags::eUseDXGIFactoryProxy;
        Require(Export<PFun_slInit*>(interposer,"slInit")(preferences,sl::kSDKVersion)==sl::Result::eOk,"Streamline init failed");
        Require(Export<PFun_slSetD3DDevice*>(interposer,"slSetD3DDevice")(device.Get())==sl::Result::eOk,"Streamline device bind failed");
        auto luid=device->GetAdapterLuid();sl::AdapterInfo adapter{};adapter.deviceLUID=reinterpret_cast<std::uint8_t*>(&luid);adapter.deviceLUIDSizeInBytes=sizeof(luid);
        Require(Export<PFun_slIsFeatureSupported*>(interposer,"slIsFeatureSupported")(sl::kFeatureDLSS_G,adapter)==sl::Result::eOk,"FG support query failed");
    } else {
        using Init=unsigned(__cdecl*)(unsigned long long,const wchar_t*,ID3D12Device*,unsigned,const NVSDK_NGX_FeatureCommonInfo*);
        const wchar_t* paths[]{directory.c_str()};NVSDK_NGX_FeatureCommonInfo common{};
        common.PathListInfo={paths,1};common.LoggingInfo={&NgxLog,NVSDK_NGX_LOGGING_LEVEL_VERBOSE,true};
        const auto temporary=std::filesystem::temp_directory_path()/L"RaZkolbaS-override-test";std::filesystem::create_directories(temporary);
        Require(Export<Init>(core,"NVSDK_NGX_D3D12_Init_Ext")(0x0dba0147,temporary.c_str(),device.Get(),0x15,&common)==1,"NGX init failed");
        NVSDK_NGX_Parameter* parameters{};
        Require(Export<decltype(&NVSDK_NGX_D3D12_GetCapabilityParameters)>(core,"NVSDK_NGX_D3D12_GetCapabilityParameters")(&parameters)==NVSDK_NGX_Result_Success,"capability query failed");
    }
    const auto module=GetModuleHandleW(L"nvngx_dlssg.dll");std::array<wchar_t,32768> loaded{};
    if(module)GetModuleFileNameW(module,loaded.data(),static_cast<DWORD>(loaded.size()));
    const bool bundled=module && std::filesystem::equivalent(directory/L"nvngx_dlssg.dll",loaded.data());
    std::printf("RESULT reads=%u suppressed=%u bundled=%u observedOverride=%u\n",reads,suppressed,bundled,overrideObserved);
    Require(reads>0,"simulation was not consumed by real NGX");
    if(compatibility)Require(*compatibilitySlot==reinterpret_cast<std::uintptr_t>(&CompatibilityResolve),"compatibility resolver ownership changed during startup");
    if(filtered){Require(suppressed>0&&bundled,"application settings did not retain bundled FG runtime");}
    else {
        Require(overrideObserved,"vendor did not observe the simulated override");
        if(bundled){std::puts("SKIP: no cached NVIDIA override runtime on this machine");return 77;}
    }
    std::puts("PASS: real NVIDIA runtime selection verified; no driver profile writes");std::fflush(stdout);
    // NGX/Streamline workers and imports retain code/device references until process exit.
    device.Detach();ExitProcess(0);
}catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
