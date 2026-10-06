#include "RuntimeOwner.h"
#include "RuntimeFileLease.h"
#include "CallerIdentityShim.h"
#include "RuntimeParameters.h"
#include "../NvidiaAppSettings.h"
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <array>
#include <atomic>
#include <mutex>
#include <optional>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
using Microsoft::WRL::ComPtr;
std::mutex processMutex;
std::string processProfile;
const void* activeOwner{};
enum class Phase { Unopened, Ready, Retired, Quarantined };
std::unexpected<Error> Fail(ErrorKind kind,const char* message,int64_t code=0){return std::unexpected(Error{kind,code,message});}
template<class T>T Export(HMODULE module,const char* symbol){return reinterpret_cast<T>(GetProcAddress(module,symbol));}
bool Matches(HMODULE module,const RuntimeFileLease& lease){
    std::array<wchar_t,32768> path{};const auto count=GetModuleFileNameW(module,path.data(),static_cast<DWORD>(path.size()));
    return count && count<path.size() && lease.Matches(path.data());
}
HMODULE CallerModule(){HMODULE self{};GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&CallerModule),&self);return self;}
}
Result<RuntimeExports> ResolveRuntimeExports(HMODULE nr,HMODULE core){
    if(!nr||!core)return Fail(ErrorKind::InvalidInput,"NR/core module missing before export resolution");
    RuntimeExports e;
    e.init=Export<RuntimeExports::InitFn>(nr,"NVSDK_NGX_D3D12_Init_Ext");e.shutdown=Export<RuntimeExports::ShutdownFn>(nr,"NVSDK_NGX_D3D12_Shutdown1");
    e.create=Export<RuntimeExports::CreateFn>(nr,"NVSDK_NGX_D3D12_CreateFeature");e.evaluate=Export<RuntimeExports::EvaluateFn>(nr,"NVSDK_NGX_D3D12_EvaluateFeature");e.release=Export<RuntimeExports::ReleaseFn>(nr,"NVSDK_NGX_D3D12_ReleaseFeature");
    e.allocate=Export<RuntimeExports::AllocateFn>(core,"NVSDK_NGX_D3D12_AllocateParameters");e.destroy=Export<RuntimeExports::DestroyFn>(core,"NVSDK_NGX_D3D12_DestroyParameters");
    if(!e.Complete())return Fail(ErrorKind::Unsupported,"Required NR/driver-core export missing");return e;
}
struct RuntimeOwner::State {
    RuntimeOwnerPaths paths;
    RuntimeExports exports;
    ComPtr<ID3D12Device> device;
    std::optional<RuntimeFileLease> runtimeLease,coreLease;
    HMODULE nr{},core{};
    CallerIdentityShim shim;
    std::atomic<Phase> phase{Phase::Unopened};
    bool attempted{},initAttempted{},initRejected{};
    uint32_t clients{};
    std::atomic_uint lastInit{},lastShutdown{};
    uint64_t shimRva{};
    std::string_view profileId;
    Result<void> InitializeRuntime(){
        initAttempted=true;
        lastInit=exports.init(kDirectNrAppId,paths.dataDirectory.c_str(),device.Get(),kDirectNrApiVersion,nullptr);
        if(lastInit!=1){initRejected=true;phase=Phase::Quarantined;return Fail(ErrorKind::Runtime,"NR Init_Ext rejected; partial runtime/device ownership retained",lastInit);}
        phase=Phase::Ready;return {};
    }
    ~State(){ if(nr)FreeLibrary(nr);if(core)FreeLibrary(core); }
};
RuntimeOwner::RuntimeOwner(RuntimeOwnerPaths paths):state_(std::make_unique<State>()){state_->paths=std::move(paths);}
RuntimeOwner::~RuntimeOwner(){
    // No implicit shutdown: absence of a confirmed client/reader retirement is
    // never permission to free the feature runtime, its device or its file lease.
    if(state_ && (state_->phase==Phase::Ready || state_->phase==Phase::Quarantined ||
        state_->initAttempted || state_->clients || state_->shim.Active())) (void)state_.release();
}
Result<void> RuntimeOwner::Open(const RuntimeProfile& requested,ID3D12Device* device,AdapterIdentity renderer){
    std::scoped_lock lock(processMutex);auto& s=*state_;
    if(s.attempted)return Fail(ErrorKind::Conflict,"NR owner Open is latched; no retry");s.attempted=true;
    if(!device)return Fail(ErrorKind::InvalidInput,"NR device is missing");
    const RuntimeProfile* profile{};
    for(const auto& p:RuntimeCatalog())if(p.id==requested.id && p.sha256==requested.sha256 && p.bytes==requested.bytes &&
        p.relativePath==requested.relativePath && p.primaryFamily==requested.primaryFamily && p.includeRtx30==requested.includeRtx30 &&
        p.compatibility==requested.compatibility)profile=&p;
    if(!profile)return Fail(ErrorKind::IdentityMismatch,"Requested NR profile does not match immutable catalog");
    const auto luid=device->GetAdapterLuid();ComPtr<IDXGIFactory4> factory;
    auto hr=CreateDXGIFactory1(IID_PPV_ARGS(&factory));if(FAILED(hr))return Fail(ErrorKind::Io,"Cannot inspect actual NR device adapter",hr);
    ComPtr<IDXGIAdapter1> adapter;hr=factory->EnumAdapterByLuid(luid,IID_PPV_ARGS(&adapter));if(FAILED(hr))return Fail(ErrorKind::IdentityMismatch,"NR device adapter is unavailable",hr);
    DXGI_ADAPTER_DESC1 desc{};hr=adapter->GetDesc1(&desc);if(FAILED(hr))return Fail(ErrorKind::Io,"Cannot describe NR device adapter",hr);
    const AdapterIdentity actual{desc.VendorId,desc.DeviceId,desc.SubSysId,{luid.LowPart,luid.HighPart},bool(desc.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)};
    auto matched=CheckAdapterMatch(renderer,actual);if(!matched)return matched;
    if(activeOwner || (!processProfile.empty() && processProfile!=profile->id))return Fail(ErrorKind::Conflict,"NR process owner/profile already latched; relaunch required");
    const ArtifactIdentity artifact{profile->id,true,true};const auto selection=SelectRuntime(actual,profile->id,{&artifact,1});
    if(!selection.profile)return Fail(ErrorKind::Unsupported,selection.reason.c_str());
    if(!s.paths.nrFile.is_absolute() || s.paths.nrFile.filename()!=L"nvngx_dlssnr.dll" || !s.paths.dataDirectory.is_absolute())
        return Fail(ErrorKind::InvalidInput,"NR/data paths must be explicit absolute paths");
    if(s.paths.callerIdentityShim && profile->compatibility!=CompatibilityPolicy::CallerIdentityProbeRequired)
        return Fail(ErrorKind::Unsupported,"This signed profile has no caller shim qualification");
    if(GetModuleHandleW(L"nvngx_dlssnr.dll"))return Fail(ErrorKind::Conflict,"Conflicting resident NR module");
    auto lease=RuntimeFileLease::Open(s.paths.nrFile,*profile);if(!lease)return std::unexpected(lease.error());s.runtimeLease=std::move(*lease);
    auto coreLease=RuntimeFileLease::OpenDriverCore(s.paths.coreFile);if(!coreLease)return std::unexpected(coreLease.error());s.coreLease=std::move(*coreLease);
    std::error_code error;std::filesystem::create_directories(s.paths.dataDirectory,error);if(error)return Fail(ErrorKind::Io,"Cannot create NR data directory",error.value());
    s.core=LoadLibraryExW(s.paths.coreFile.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!s.core)return Fail(ErrorKind::Io,"Cannot load trusted NVIDIA driver core",GetLastError());
    if(!Matches(s.core,*s.coreLease))return Fail(ErrorKind::IdentityMismatch,"Loaded core does not match held driver file");
    if(!NvidiaAppSettings::ProtectModule(s.core))return Fail(ErrorKind::Runtime,"Cannot establish application-controlled NGX settings");
    s.nr=LoadLibraryExW(s.paths.nrFile.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!s.nr)return Fail(ErrorKind::Io,"Cannot load NR profile",GetLastError());
    if(!Matches(s.nr,*s.runtimeLease))return Fail(ErrorKind::IdentityMismatch,"Loaded NR does not match held profile file");
    auto resolved=ResolveRuntimeExports(s.nr,s.core);if(!resolved)return std::unexpected(resolved.error());
    s.exports=*resolved;
    if(s.paths.callerIdentityShim){
        auto installed=s.shim.Install(s.nr,CallerModule(),*s.runtimeLease);
        if(!installed){if(s.shim.Active())s.phase=Phase::Quarantined;return installed;}
        s.shimRva=s.shim.SlotRva();
    }
    s.device=device;s.profileId=profile->id;processProfile=profile->id;activeOwner=&s;
    return s.InitializeRuntime();
}
const RuntimeExports& RuntimeOwner::Exports() const {return state_->exports;}
bool RuntimeOwner::Ready() const noexcept{return state_->phase==Phase::Ready;}
RuntimeOpenDisposition RuntimeOwner::OpenDisposition() const noexcept{
    const auto& s=*state_;if(s.phase==Phase::Ready)return RuntimeOpenDisposition::Ready;
    if(s.phase==Phase::Quarantined)return s.initRejected?RuntimeOpenDisposition::InitializationQuarantined:RuntimeOpenDisposition::TerminalQuarantined;
    return RuntimeOpenDisposition::Unavailable;
}
Result<void> RuntimeOwner::CheckInitializationFallbackSafety(){
    std::scoped_lock lock(processMutex);auto& s=*state_;
    if(s.phase!=Phase::Quarantined || !s.initRejected || s.clients || !s.device)
        return Fail(ErrorKind::Retirement,"NR quarantine cannot admit initialization fallback");
    const auto hr=s.device->GetDeviceRemovedReason();
    if(FAILED(hr))return Fail(ErrorKind::Runtime,"NR initialization fallback device is removed",hr);
    return s.shim.CheckOwnership();
}
std::string_view RuntimeOwner::ProfileId()const noexcept{return state_->profileId;}
Result<void> RuntimeOwner::CheckStageInitializationFallbackSafety(){
    std::scoped_lock lock(processMutex);auto& s=*state_;
    if(s.phase!=Phase::Ready || s.clients || !s.device)
        return Fail(ErrorKind::Retirement,"NR runtime cannot admit stage initialization fallback");
    const auto hr=s.device->GetDeviceRemovedReason();
    if(FAILED(hr))return Fail(ErrorKind::Runtime,"NR stage initialization fallback device is removed",hr);
    return s.shim.CheckOwnership();
}
Result<void> RuntimeOwner::CheckClientDevice(ID3D12Device* device)const{
    std::scoped_lock lock(processMutex);const auto& s=*state_;
    ComPtr<IUnknown> actual,expected;
    if(s.phase!=Phase::Ready || !device || FAILED(device->QueryInterface(IID_PPV_ARGS(&actual))) ||
        FAILED(s.device.As(&expected)) || actual.Get()!=expected.Get())
        return Fail(ErrorKind::IdentityMismatch,"NR client device differs from retained runtime device");
    return {};
}
Result<void> RuntimeOwner::AcquireClient(){std::scoped_lock lock(processMutex);auto& s=*state_;
    if(s.phase!=Phase::Ready || s.clients==UINT32_MAX)return Fail(ErrorKind::Conflict,"NR owner cannot admit another client");++s.clients;return {};}
Result<void> RuntimeOwner::ReleaseClientAfterRetirement(){std::scoped_lock lock(processMutex);auto& s=*state_;
    if(s.phase!=Phase::Ready || !s.clients)return Fail(ErrorKind::Conflict,"NR client retirement is not owned");--s.clients;return {};}
Result<void> RuntimeOwner::Retire(){
    std::scoped_lock lock(processMutex);auto& s=*state_;
    if(s.phase==Phase::Quarantined)return Fail(ErrorKind::Retirement,"NR retirement is terminal; no retry, ownership retained");
    if(s.clients)return Fail(ErrorKind::Retirement,"NR clients/recordings/readers have not retired");
    if(s.phase==Phase::Retired)return {};
    const auto owned=s.shim.CheckOwnership();if(!owned){s.phase=Phase::Quarantined;return owned;}
    if(s.phase==Phase::Ready){s.lastShutdown=s.exports.shutdown(s.device.Get());
        if(s.lastShutdown!=1){s.phase=Phase::Quarantined;return Fail(ErrorKind::Runtime,"NR shutdown failed; ownership retained",s.lastShutdown);}}
    const auto restored=s.shim.Restore();if(!restored){s.phase=Phase::Quarantined;return restored;}
    if(s.nr){FreeLibrary(s.nr);s.nr=nullptr;}if(s.core){FreeLibrary(s.core);s.core=nullptr;}
    s.runtimeLease.reset();s.coreLease.reset();s.device.Reset();s.exports={};s.initAttempted=false;s.phase=Phase::Retired;
    if(activeOwner==&s)activeOwner=nullptr;return {};
}
uint32_t RuntimeOwner::LastInitResult() const noexcept{return state_->lastInit;}
uint32_t RuntimeOwner::LastShutdownResult() const noexcept{return state_->lastShutdown;}
uint64_t RuntimeOwner::ShimSlotRva() const noexcept{return state_->shimRva;}
}
