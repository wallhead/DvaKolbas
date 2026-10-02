#include "NeuralRendering/RuntimeOwner.h"
#include "HookSafety.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <array>
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
int failed{},shutdownCalls{};
void Check(bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);if(!ok)++failed;}
uint32_t __cdecl FailShutdown(ID3D12Device*){++shutdownCalls;return 0xbad00002;}
DWORD WINAPI Foreign(HMODULE,LPWSTR,DWORD){return 0;}
}
int wmain(int argc,wchar_t** argv){
    setvbuf(stdout,nullptr,_IONBF,0);
    if(argc==4 && std::wstring_view(argv[3])==L"missing-export"){
        const auto kernel=GetModuleHandleW(L"kernel32.dll");
        const auto r=ResolveRuntimeExports(kernel,kernel);
        Check(!r&&r.error().kind==ErrorKind::Unsupported,"MissingDirectExportsRejectedWithoutCallingVendorCode");
        return failed?1:0;
    }
    if(argc!=4 || !std::filesystem::exists(argv[1]) || !std::filesystem::exists(argv[2])){std::puts("SKIPPED: local exact RTX40 runtime/core required");return 77;}
    if(NrRuntimeResearch::GameRunningOrUnknown()){
        std::puts("REFUSED: Skyrim running or process inventory unavailable");return 1;
    }
    ComPtr<IDXGIFactory6> factory;if(FAILED(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory))))return 77;
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 d{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> a;const auto r=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&a));if(r==DXGI_ERROR_NOT_FOUND)break;if(FAILED(r))return 1;
        a->GetDesc1(&d);if(d.VendorId==0x10de&&d.DeviceId==0x2702&&!(d.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)){adapter=a;break;}}
    if(!adapter)return 77;ComPtr<ID3D12Device> device;if(FAILED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device))))return 77;
    AdapterIdentity id{d.VendorId,d.DeviceId,d.SubSysId,{d.AdapterLuid.LowPart,d.AdapterLuid.HighPart},false};
    const auto mode=std::wstring_view(argv[3]);
    auto owner=std::make_unique<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-owner-cache"),true});
    if(mode==L"resident-conflict"){
        const auto module=LoadLibraryExW(argv[1],nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
        Check(module!=nullptr,"ResidentConflictFixtureLoadedWithoutInit");
        const auto r=owner->Open(RuntimeCatalog()[1],device.Get(),id);
        Check(!r&&r.error().kind==ErrorKind::Conflict&&owner->LastInitResult()==0,"ResidentNrRejectedBeforeInitialization");
        if(module)FreeLibrary(module);return failed?1:0;
    }
    if(mode==L"foreign-device"){
        id.luid.low++;const auto r=owner->Open(RuntimeCatalog()[1],device.Get(),id);
        Check(!r&&r.error().kind==ErrorKind::IdentityMismatch,"ActualDeviceRejectsForeignRendererLuid");
        Check(!GetModuleHandleW(L"nvngx_dlssnr.dll"),"ForeignDeviceRejectedBeforeNrLoad");return failed?1:0;
    }
    auto r=owner->Open(RuntimeCatalog()[1],device.Get(),id);
    Check(bool(r)&&owner->Ready()&&owner->Exports().Complete()&&owner->LastInitResult()==1,"DirectOwnerInitializesExactRuntime");
    if(!r){std::printf("%s\n",r.error().message.c_str());return 1;}
    Check(!owner->Open(RuntimeCatalog()[1],device.Get(),id),"DoubleOpenRejected");
    if(mode==L"shutdown-failure"){
        const_cast<RuntimeExports&>(owner->Exports()).shutdown=&FailShutdown; // Test-only fault, no production injection API.
        Check(!owner->Retire() && shutdownCalls==1,"FailedShutdownIsTerminal");
        Check(!owner->Retire() && shutdownCalls==1,"ShutdownNotRetried");owner.reset();
        Check(shutdownCalls==1 && GetModuleHandleW(L"nvngx_dlssnr.dll"),"DestructorRetainsUncertainModuleAndDoesNotRetry");
        const auto writer=CreateFileW(argv[1],GENERIC_WRITE,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
        Check(writer==INVALID_HANDLE_VALUE&&GetLastError()==ERROR_SHARING_VIOLATION,"QuarantinedOwnerRetainsVerifiedFileLease");if(writer!=INVALID_HANDLE_VALUE)CloseHandle(writer);
    }else if(mode==L"changed-shim"){
        const auto nr=GetModuleHandleW(L"nvngx_dlssnr.dll");auto slot=TheosRenderPipeline::HookSafety::ImportSlot(reinterpret_cast<uintptr_t>(nr),"KERNEL32.dll","GetModuleFileNameW");
        DWORD old{},ignored{};Check(slot&&VirtualProtect(slot,sizeof(void*),PAGE_READWRITE,&old),"ShimFaultInjectionProtection");
        if(slot){InterlockedExchangePointer(reinterpret_cast<void**>(slot),reinterpret_cast<void*>(&Foreign));VirtualProtect(slot,sizeof(void*),old,&ignored);}
        Check(!owner->Retire() && owner->LastShutdownResult()==0,"ChangedShimBlocksVendorShutdownAndUnload");
        owner.reset();Check(GetModuleHandleW(L"nvngx_dlssnr.dll")&&*slot==reinterpret_cast<uintptr_t>(&Foreign),"LaterHookOwnerPreservedThroughDestructor");
    }else{
        Check(bool(owner->AcquireClient()),"ClientOwnershipAdmitted");
        Check(!owner->Retire() && owner->LastShutdownResult()==0,"OutstandingClientPreventsShutdown");
        Check(bool(owner->ReleaseClientAfterRetirement()),"RetiredClientOwnershipReleased");
        Check(!owner->ReleaseClientAfterRetirement(),"DoubleClientReleaseRejected");
        Check(bool(owner->Retire()) && owner->LastShutdownResult()==1 && !owner->Ready(),"RetiredOwnerUnloadsAfterVerifiedShutdownAndRestore");
        Check(!GetModuleHandleW(L"nvngx_dlssnr.dll"),"NrModuleUnloadedAfterCleanRetirement");
    }
    return failed?1:0;
}
