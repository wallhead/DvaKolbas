#include "NeuralRendering/CallerIdentityShim.h"
#include "HookSafety.h"
#include <array>
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
using namespace TheosRenderPipeline;
namespace {
int failed{};
void Check(bool ok,const char* name){ std::printf("%s %s\n",ok?"PASS":"FAIL",name); if(!ok)++failed; }
DWORD WINAPI Foreign(HMODULE,LPWSTR out,DWORD n) { if(out&&n)out[0]=0;return 0; }
bool Replace(void** slot,void* value){DWORD old{},ignored{};if(!VirtualProtect(slot,sizeof(void*),PAGE_READWRITE,&old))return false;
    InterlockedExchangePointer(slot,value);return VirtualProtect(slot,sizeof(void*),old,&ignored)!=FALSE;}
}
int wmain(int argc,wchar_t** argv) {
    if(argc!=3 || !std::filesystem::exists(argv[1])) {std::puts("SKIPPED: supplied RTX40 NR DLL required");return 77;}
    const auto profile=RuntimeCatalog()[1];
    auto lease=RuntimeFileLease::Open(argv[1],profile);
    if(!lease){std::printf("FAIL lease %s\n",lease.error().message.c_str());return 1;}
    const auto module=LoadLibraryExW(lease->Path().c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!module){std::puts("FAIL fixture NR module load");return 1;}
    auto* slot=reinterpret_cast<void**>(HookSafety::ImportSlot(reinterpret_cast<uintptr_t>(module),"KERNEL32.dll","GetModuleFileNameW"));
    if(!slot){std::puts("FAIL NR import missing");FreeLibrary(module);return 1;}
    void* original=*slot;
    const bool conflict=std::wstring_view(argv[2])==L"conflict";
    {
        CallerIdentityShim shim;
        const auto installed=shim.Install(module,GetModuleHandleW(nullptr),*lease);
        Check(bool(installed)&&shim.Active(),"NarrowVerifiedCallerShimInstalled");
        if(!installed){ FreeLibrary(module);return 1; }
        using Query=DWORD(WINAPI*)(HMODULE,LPWSTR,DWORD);
        const auto query=reinterpret_cast<Query>(*slot);
        std::array<wchar_t,32768> buffer{},expected{};
        const auto count=query(GetModuleHandleW(nullptr),buffer.data(),static_cast<DWORD>(buffer.size()));
        Check(count==9 && std::wstring_view(buffer.data())==L"nvngx.dll","OnlyCallerIdentitySubstituted");
        GetModuleFileNameW(module,expected.data(),static_cast<DWORD>(expected.size()));
        query(module,buffer.data(),static_cast<DWORD>(buffer.size()));
        Check(std::wstring_view(buffer.data())==expected.data(),"FeatureModuleQueryForwarded");
        const auto kernel=GetModuleHandleW(L"kernel32.dll");
        GetModuleFileNameW(kernel,expected.data(),static_cast<DWORD>(expected.size()));
        query(kernel,buffer.data(),static_cast<DWORD>(buffer.size()));
        Check(std::wstring_view(buffer.data())==expected.data(),"UnrelatedModuleQueryForwarded");
        wchar_t shortBuffer[4]{}; SetLastError(0);
        Check(query(GetModuleHandleW(nullptr),shortBuffer,4)==4 && shortBuffer[3]==0 &&
            GetLastError()==ERROR_INSUFFICIENT_BUFFER,"TruncatedCallerQueryFollowsWin32Contract");
        CallerIdentityShim second;
        Check(!second.Install(module,GetModuleHandleW(nullptr),*lease),"SecondShimOwnerRejected");
        if(conflict) {
            Check(Replace(slot,reinterpret_cast<void*>(&Foreign)),"InjectLaterOwner");
            const auto restored=shim.Restore();
            Check(!restored && restored.error().kind==ErrorKind::Conflict && *slot==reinterpret_cast<void*>(&Foreign),"LaterOwnerNeverOverwritten");
            Check(!shim.Restore() && *slot==reinterpret_cast<void*>(&Foreign),"TerminalRestorationNotRetried");
            // This child intentionally retains the imported module/proxy references
            // until exit. No worker calls and no vendor initialization occurred.
        } else {
            Check(bool(shim.Restore()) && !shim.Active() && *slot==original,"OriginalImportRestoredBeforeUnload");
        }
    }
    if(conflict) Check(*slot==reinterpret_cast<void*>(&Foreign),"DestructorDoesNotRetryRestoration");
    FreeLibrary(module);
    return failed?1:0;
}
