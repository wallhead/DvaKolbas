#include "CallerIdentityShim.h"
#include "HookSafety.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cstring>
#include <mutex>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
std::mutex mutex;
const CallerIdentityShim* owner{};
std::atomic<HMODULE> callerIdentity{};
std::unexpected<Error> Fail(ErrorKind kind,const char* message,int64_t code=0) {
    return std::unexpected(Error{kind,code,message});
}
DWORD WINAPI Proxy(HMODULE module,LPWSTR out,DWORD size) {
    if(module!=callerIdentity.load(std::memory_order_acquire)) return GetModuleFileNameW(module,out,size);
    if(!out || !size) {SetLastError(ERROR_INSUFFICIENT_BUFFER);return 0;}
    constexpr wchar_t name[]=L"nvngx.dll";
    const DWORD copied=std::min<DWORD>(9,size-1);
    std::copy_n(name,copied,out);out[copied]=0;
    if(copied!=9){SetLastError(ERROR_INSUFFICIENT_BUFFER);return size;}
    return copied;
}
void** UniqueImport(HMODULE module) {
    const auto base=reinterpret_cast<uintptr_t>(module);
    IMAGE_DOS_HEADER dos{};IMAGE_NT_HEADERS64 nt{};
    if(!HookSafety::Read(base,&dos,sizeof(dos)) || dos.e_magic!=IMAGE_DOS_SIGNATURE || dos.e_lfanew<=0 ||
        !HookSafety::Read(base+dos.e_lfanew,&nt,sizeof(nt)) || nt.Signature!=IMAGE_NT_SIGNATURE ||
        nt.FileHeader.Machine!=IMAGE_FILE_MACHINE_AMD64 || nt.OptionalHeader.Magic!=IMAGE_NT_OPTIONAL_HDR64_MAGIC) return nullptr;
    const auto size=nt.OptionalHeader.SizeOfImage;
    const auto inside=[size](uint64_t rva,size_t bytes){return rva<size && bytes<=size-rva;};
    const auto text=[&](uint64_t rva,auto& out) {
        for(size_t i=0;i<out.size();++i){
            if(!inside(rva+i,1)||!HookSafety::Read(base+rva+i,&out[i],1))return false;
            if(!out[i])return true;
        }return false;
    };
    const auto dir=nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if(!dir.VirtualAddress || !inside(dir.VirtualAddress,dir.Size))return nullptr;
    void** found{};
    for(size_t i=0;i+sizeof(IMAGE_IMPORT_DESCRIPTOR)<=dir.Size;i+=sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
        IMAGE_IMPORT_DESCRIPTOR d{};
        if(!HookSafety::Read(base+dir.VirtualAddress+i,&d,sizeof(d)))return nullptr;
        if(!d.Name)break;
        std::array<char,256> dll{};
        if(!text(d.Name,dll))return nullptr;
        if(_stricmp(dll.data(),"KERNEL32.dll"))continue;
        if(!d.OriginalFirstThunk || !d.FirstThunk)return nullptr;
        for(size_t j=0;;++j) {
            const uint64_t name=uint64_t(d.OriginalFirstThunk)+j*sizeof(IMAGE_THUNK_DATA64);
            const uint64_t slot=uint64_t(d.FirstThunk)+j*sizeof(void*);
            IMAGE_THUNK_DATA64 thunk{};
            if(!inside(name,sizeof(thunk)) || !inside(slot,sizeof(void*)) ||
                !HookSafety::Read(base+name,&thunk,sizeof(thunk)))return nullptr;
            if(!thunk.u1.AddressOfData)break;
            if(IMAGE_SNAP_BY_ORDINAL64(thunk.u1.Ordinal))continue;
            std::array<char,256> symbol{};
            if(!text(thunk.u1.AddressOfData+offsetof(IMAGE_IMPORT_BY_NAME,Name),symbol))return nullptr;
            if(!std::strcmp(symbol.data(),"GetModuleFileNameW")) {
                if(found)return nullptr;
                found=reinterpret_cast<void**>(base+slot);
            }
        }
    }return found;
}
bool Retain(const void* address,HMODULE& module) {
    return GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,reinterpret_cast<LPCWSTR>(address),&module)!=FALSE;
}
}
Result<void> CallerIdentityShim::Install(HMODULE feature,HMODULE caller,const RuntimeFileLease& lease) {
    std::scoped_lock lock(mutex);
    if(owner || Active() || terminalFailure_)return Fail(ErrorKind::Conflict,"Caller identity shim already owned/terminal");
    if(!feature || !caller || !lease.Valid())return Fail(ErrorKind::InvalidInput,"Missing caller/feature/file lease");
    std::array<wchar_t,32768> path{};
    const auto count=GetModuleFileNameW(feature,path.data(),static_cast<DWORD>(path.size()));
    if(!count || count>=path.size() || !lease.Matches(path.data()))return Fail(ErrorKind::IdentityMismatch,"Shim target differs from held NR file");
    auto* slot=UniqueImport(feature);
    if(!slot || reinterpret_cast<uintptr_t>(slot)%alignof(void*))return Fail(ErrorKind::Unsupported,"Unique aligned NR GetModuleFileNameW import missing");
    void* expected=reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"GetModuleFileNameW"));
    void* current{};
    if(!expected || !HookSafety::Read(reinterpret_cast<uintptr_t>(slot),&current,sizeof(current)) || current!=expected)
        return Fail(ErrorKind::Conflict,"NR import is already changed; no hook chaining permitted here");
    HMODULE featureRef{},callerRef{},proxyRef{};
    if(!Retain(feature,featureRef) || !Retain(caller,callerRef) || !Retain(reinterpret_cast<void*>(&Proxy),proxyRef)) {
        if(featureRef)FreeLibrary(featureRef);if(callerRef)FreeLibrary(callerRef);if(proxyRef)FreeLibrary(proxyRef);
        return Fail(ErrorKind::Io,"Cannot retain shim code/feature/caller modules",GetLastError());
    }
    DWORD old{},ignored{};
    if(!VirtualProtect(slot,sizeof(void*),PAGE_READWRITE,&old)) {
        const auto error=GetLastError();FreeLibrary(featureRef);FreeLibrary(callerRef);FreeLibrary(proxyRef);
        return Fail(ErrorKind::Io,"Cannot protect NR import",error);
    }
    // Immutable callback state is published before the hook becomes callable.
    owner=this;callerIdentity.store(caller,std::memory_order_release);
    const auto before=InterlockedCompareExchangePointer(slot,reinterpret_cast<void*>(&Proxy),expected);
    const bool protectedAgain=VirtualProtect(slot,sizeof(void*),old,&ignored)!=FALSE;
    if(before!=expected) {
        owner=nullptr;callerIdentity.store(nullptr,std::memory_order_release);
        FreeLibrary(featureRef);FreeLibrary(callerRef);FreeLibrary(proxyRef);
        return Fail(ErrorKind::Conflict,"NR import changed during installation");
    }
    slot_=slot;original_=expected;feature_=featureRef;caller_=callerRef;proxyModule_=proxyRef;
    if(!protectedAgain){terminalFailure_=true;return Fail(ErrorKind::Retirement,"Import installed but page protection restoration failed; ownership retained",GetLastError());}
    return {};
}
Result<void> CallerIdentityShim::Restore() {
    std::scoped_lock lock(mutex);
    if(terminalFailure_)return Fail(ErrorKind::Conflict,"Shim restoration is terminal; references retained, no retry");
    if(!Active())return {};
    DWORD old{},ignored{};
    if(!VirtualProtect(slot_,sizeof(void*),PAGE_READWRITE,&old)) {
        terminalFailure_=true;return Fail(ErrorKind::Retirement,"Cannot protect import for restoration",GetLastError());
    }
    const auto before=InterlockedCompareExchangePointer(slot_,original_,reinterpret_cast<void*>(&Proxy));
    const bool protectedAgain=VirtualProtect(slot_,sizeof(void*),old,&ignored)!=FALSE;
    if(before!=reinterpret_cast<void*>(&Proxy) || !protectedAgain) {
        terminalFailure_=true;
        return Fail(ErrorKind::Conflict,"Shim ownership/page protection changed; retained without overwriting later owner");
    }
    slot_=nullptr;owner=nullptr;callerIdentity.store(nullptr,std::memory_order_release);
    FreeLibrary(feature_);FreeLibrary(caller_);FreeLibrary(proxyModule_);
    feature_=caller_=proxyModule_=nullptr;original_=nullptr;
    return {};
}
Result<void> CallerIdentityShim::CheckOwnership() {
    std::scoped_lock lock(mutex);
    if(terminalFailure_)return Fail(ErrorKind::Conflict,"Shim ownership is terminal; no vendor calls permitted");
    if(!Active())return {};
    void* current{};
    if(!HookSafety::Read(reinterpret_cast<uintptr_t>(slot_),&current,sizeof(current)) ||
        current!=reinterpret_cast<void*>(&Proxy)){
        terminalFailure_=true;
        return Fail(ErrorKind::Conflict,"Shim import changed; vendor calls/restoration refused and ownership retained");
    }
    return {};
}
uint64_t CallerIdentityShim::SlotRva() const noexcept {
    return slot_?reinterpret_cast<uintptr_t>(slot_)-reinterpret_cast<uintptr_t>(feature_):0;
}
}
