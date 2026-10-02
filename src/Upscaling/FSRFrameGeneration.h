#pragma once
#include "FSRRuntime.h"
#include "FSRGenerationParameters.h"
#include <memory>
#include <mutex>
#include <dxgi1_6.h>
namespace TheosRenderPipeline::Upscaling
{
    class FsrSdkSession;
    class FsrSdkLock final
    {
    public:
        FsrSdkLock() = default;
        ~FsrSdkLock();
        FsrSdkLock(FsrSdkLock&&) noexcept;
        FsrSdkLock(const FsrSdkLock&) = delete;
        FsrSdkLock& operator=(const FsrSdkLock&) = delete;
        bool Owns(const FsrSdkSession&) const noexcept;
    private:
        friend class FsrSdkSession;
        explicit FsrSdkLock(std::shared_ptr<FsrSdkSession>);
        std::shared_ptr<FsrSdkSession> session_;
        std::unique_lock<std::mutex> lock_;
    };
    class FsrSdkSession final : public std::enable_shared_from_this<FsrSdkSession>
    {
    public:
        FsrSdkSession();
        ~FsrSdkSession();
        FsrSdkLock Lock();
        Result<void> BeginPresent(const FsrSdkLock&);
        void EndPresent(const FsrSdkLock&) noexcept;
        Result<void> StopAdmissions(const FsrSdkLock&);
    private:
        friend class FsrSdkLock;
        friend class FsrFrameGeneration;
        struct CallbackState;
        std::mutex mutex_;
        std::unique_ptr<CallbackState> callback_;
        bool closing_{}, present_{};
        static thread_local FsrSdkSession* ownedByThread_;
    };
    struct FsrGenerationCallbackOutcome
    {
        ffxReturnCode_t result{FFX_API_RETURN_OK};
        unsigned invocations{};
    };
    class FsrFrameGeneration final
    {
    public:
        explicit FsrFrameGeneration(std::shared_ptr<FsrSdkSession>);
        ~FsrFrameGeneration();
        FsrFrameGeneration(const FsrFrameGeneration&) = delete;
        FsrFrameGeneration& operator=(const FsrFrameGeneration&) = delete;
        Result<void> Create(const FsrSdkLock&, std::shared_ptr<FsrRuntime>, ID3D12Device*, const FsrEffectProvider&, const FsrGenerationLimits&);
        Result<void> Configure(const FsrSdkLock&, IDXGISwapChain4*, uint64_t sourceId, bool enabled);
        Result<void> Prepare(const FsrSdkLock&, ID3D12GraphicsCommandList*, const UpscaleFrame&, const FsrGenerationResources&, bool reset);
        Result<void> DisableAndDetach(const FsrSdkLock&);
        // Caller must first prove Prepare GPU retirement and SDK async Present
        // retirement. No main-queue fence substitutes for WaitForPresents.
        Result<void> DestroyAfterRetirement(const FsrSdkLock&);
        Result<FfxApiEffectMemoryUsage> QueryMemoryUsage(const FsrSdkLock&);
        bool ContextOwned(const FsrSdkLock&) const;
        FsrGenerationCallbackOutcome LastCallback(const FsrSdkLock&) const;
        static ffxReturnCode_t GenerationCallback(ffxDispatchDescFrameGeneration*, void*) noexcept;
    private:
        struct State;
        std::unique_ptr<State> state_;
    };
}
