#pragma once
#include "RuntimeCatalog.h"
#include <Windows.h>
#include <d3d12.h>
#include <memory>
struct NVSDK_NGX_Parameter;
namespace TheosRenderPipeline::NeuralRendering {
struct RuntimeExports {
    using InitFn=uint32_t(__cdecl*)(uint64_t,const wchar_t*,ID3D12Device*,uint32_t,const void*);
    using ShutdownFn=uint32_t(__cdecl*)(ID3D12Device*);
    using AllocateFn=uint32_t(__cdecl*)(NVSDK_NGX_Parameter**);
    using DestroyFn=uint32_t(__cdecl*)(NVSDK_NGX_Parameter*);
    using CreateFn=uint32_t(__cdecl*)(ID3D12GraphicsCommandList*,uint32_t,NVSDK_NGX_Parameter*,void**);
    using EvaluateFn=uint32_t(__cdecl*)(ID3D12GraphicsCommandList*,void*,NVSDK_NGX_Parameter*,void*);
    using ReleaseFn=uint32_t(__cdecl*)(void*);
    InitFn init{};ShutdownFn shutdown{};AllocateFn allocate{};DestroyFn destroy{};
    CreateFn create{};EvaluateFn evaluate{};ReleaseFn release{};
    bool Complete() const noexcept { return init&&shutdown&&allocate&&destroy&&create&&evaluate&&release; }
};
struct RuntimeOwnerPaths {
    std::filesystem::path nrFile, coreFile, dataDirectory;
    bool callerIdentityShim{};
};
// Resolves only; the owner must already have verified/retained these modules.
Result<RuntimeExports> ResolveRuntimeExports(HMODULE nr,HMODULE core);
// An Init_Ext rejection can disable NR for this session while retaining every
// partial runtime owner. Other quarantine reasons never permit source fallback.
enum class RuntimeOpenDisposition { Unavailable, Ready, InitializationQuarantined, TerminalQuarantined };
class RuntimeOwner {
public:
    explicit RuntimeOwner(RuntimeOwnerPaths);
    ~RuntimeOwner();
    RuntimeOwner(const RuntimeOwner&)=delete;
    RuntimeOwner& operator=(const RuntimeOwner&)=delete;
    Result<void> Open(const RuntimeProfile&,ID3D12Device*,AdapterIdentity renderer);
    const RuntimeExports& Exports() const;
    bool Ready() const noexcept;
    RuntimeOpenDisposition OpenDisposition() const noexcept;
    Result<void> CheckInitializationFallbackSafety();
    // Called only with the stage's independent proof of pre-create rollback.
    Result<void> CheckStageInitializationFallbackSafety();
    std::string_view ProfileId()const noexcept;
    Result<void> CheckClientDevice(ID3D12Device*)const;
    // A client covers parameters, feature, every recording and every reader.
    // Only its owner may release it after confirmed fences + feature/parameter
    // release. Abandoning a client prevents module/device retirement.
    Result<void> AcquireClient();
    Result<void> ReleaseClientAfterRetirement();
    Result<void> Retire();
    uint32_t LastInitResult() const noexcept;
    uint32_t LastShutdownResult() const noexcept;
    uint64_t ShimSlotRva() const noexcept;
private:
    struct State;
    std::unique_ptr<State> state_;
};
}
