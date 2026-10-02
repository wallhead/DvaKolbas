#pragma once
#include "RuntimeFileLease.h"
namespace TheosRenderPipeline::NeuralRendering {
// Narrow import hook: only the verified NR module's GetModuleFileNameW import,
// only one retained caller handle. All other module queries use Windows normally.
// Caller must establish worker quiescence before Restore. Destructor never retries
// a failed restoration or unloads a module which might still call the proxy.
class CallerIdentityShim {
public:
    CallerIdentityShim() = default;
    ~CallerIdentityShim() = default;
    CallerIdentityShim(const CallerIdentityShim&) = delete;
    CallerIdentityShim& operator=(const CallerIdentityShim&) = delete;
    Result<void> Install(HMODULE feature, HMODULE caller, const RuntimeFileLease&);
    Result<void> Restore();
    Result<void> CheckOwnership();
    bool Active() const noexcept { return slot_ != nullptr; }
    uint64_t SlotRva() const noexcept;
private:
    void** slot_{};
    void* original_{};
    HMODULE feature_{}, caller_{}, proxyModule_{};
    bool terminalFailure_{};
};
}
