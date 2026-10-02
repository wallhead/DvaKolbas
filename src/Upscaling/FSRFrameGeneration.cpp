#include "FSRFrameGeneration.h"
#include <dx12/ffx_api_dx12.h>
#include <wrl/client.h>
#include <atomic>
#include <utility>
namespace TheosRenderPipeline::Upscaling
{
    using Microsoft::WRL::ComPtr;
    static std::unexpected<RuntimeError> Error(ErrorKind kind, int64_t code, const char* why)
    { return std::unexpected(RuntimeError{kind, code, why}); }
    struct FsrSdkSession::CallbackState
    {
        FsrSdkSession* session{};
        void* owner{};
        ffxContext context{};
        std::shared_ptr<FsrRuntime> runtime;
        uint64_t sourceId{}, preparedId{};
        bool configured{}, enabled{}, detached{true};
        std::atomic<ffxReturnCode_t> result{FFX_API_RETURN_OK};
        std::atomic<unsigned> invocations{};
    };
    thread_local FsrSdkSession* FsrSdkSession::ownedByThread_{};
    FsrSdkSession::FsrSdkSession() : callback_(std::make_unique<CallbackState>()) { callback_->session = this; }
    FsrSdkSession::~FsrSdkSession() = default;
    FsrSdkLock::FsrSdkLock(std::shared_ptr<FsrSdkSession> session)
    {
        // Recursive or nested SDK sessions return an invalid token instead of
        // deadlocking. Callback routing uses inherited ownership, never Lock().
        if (!session || FsrSdkSession::ownedByThread_) return;
        session_ = std::move(session); lock_ = std::unique_lock(session_->mutex_);
        FsrSdkSession::ownedByThread_ = session_.get();
    }
    FsrSdkLock::~FsrSdkLock() { if (lock_.owns_lock()) FsrSdkSession::ownedByThread_ = nullptr; }
    FsrSdkLock::FsrSdkLock(FsrSdkLock&& other) noexcept : session_(std::move(other.session_)), lock_(std::move(other.lock_)) {}
    bool FsrSdkLock::Owns(const FsrSdkSession& session) const noexcept
    { return session_.get() == &session && lock_.owns_lock() && FsrSdkSession::ownedByThread_ == &session; }
    FsrSdkLock FsrSdkSession::Lock() { return FsrSdkLock(weak_from_this().lock()); }
    Result<void> FsrSdkSession::BeginPresent(const FsrSdkLock& lock)
    {
        if (!lock.Owns(*this) || closing_ || present_) return Error(ErrorKind::InvalidInput, 0, "Present requires admitted, non-recursive SDK session ownership");
        present_ = true; callback_->result = FFX_API_RETURN_OK; callback_->invocations = 0; return {};
    }
    void FsrSdkSession::EndPresent(const FsrSdkLock& lock) noexcept { if (lock.Owns(*this)) present_ = false; }
    Result<void> FsrSdkSession::StopAdmissions(const FsrSdkLock& lock)
    {
        if (!lock.Owns(*this) || present_) return Error(ErrorKind::InvalidInput, 0, "Closing requires idle SDK session ownership");
        closing_ = true; return {};
    }
    struct FsrFrameGeneration::State
    {
        std::shared_ptr<FsrSdkSession> session;
        ComPtr<ID3D12Device> device;
        ComPtr<IDXGISwapChain4> chain;
        FsrGenerationLimits limits;
        FsrEffectProvider provider;
        ffxCreateContextDescFrameGeneration create{};
        ffxCreateBackendDX12Desc backend{};
        ffxCreateContextDescFrameGenerationVersion version{};
        ffxOverrideVersion override{};
        std::array<ComPtr<ID3D12Resource>,4> resources;
        bool poisoned{};
    };
    FsrFrameGeneration::FsrFrameGeneration(std::shared_ptr<FsrSdkSession> session) : state_(std::make_unique<State>())
    { state_->session = std::move(session); }
    FsrFrameGeneration::~FsrFrameGeneration()
    {
        // No destructor can infer GPU/async retirement. An unfinished owner
        // retains the session, stable callback, creation descriptors and module.
        const auto session = state_->session;
        if (!session) return;
        if (FsrSdkSession::ownedByThread_ && FsrSdkSession::ownedByThread_ != session.get()) {
            // Never acquire another SDK session while holding one. Unknown
            // ownership is retained rather than read without serialization.
            (void)state_.release(); return;
        }
        auto lock = session->Lock();
        if (!lock.Owns(*session) && FsrSdkSession::ownedByThread_ != session.get()) { (void)state_.release(); return; }
        if (session->callback_->owner == state_.get() && session->callback_->context)
            (void)state_.release();
    }
    Result<void> FsrFrameGeneration::Create(const FsrSdkLock& lock, std::shared_ptr<FsrRuntime> runtime, ID3D12Device* device,
        const FsrEffectProvider& provider, const FsrGenerationLimits& limits)
    {
        const auto session = state_->session;
        if (!session || !lock.Owns(*session) || session->closing_ || session->present_ || session->callback_->owner ||
            !runtime || !runtime->Functions().CreateContext || !device || provider.effect != FsrEffect::FrameGeneration ||
            !limits.render.width || !limits.render.height || !limits.display.width || !limits.display.height ||
            limits.render.width > limits.display.width || limits.render.height > limits.display.height ||
            limits.display.width > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION || limits.display.height > D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION ||
            limits.format != DXGI_FORMAT_R8G8B8A8_UNORM || !limits.input.colorIsLinear)
            return Error(ErrorKind::InvalidInput, 0, "FG creation requires its own SDK lock, verified runtime and supported fixed SDR limits");
        auto available = runtime->EnumerateForEffect(device, FsrEffect::FrameGeneration);
        if (!available) return std::unexpected(available.error());
        auto selected = SelectFsrEffectProvider(*available, FsrEffect::FrameGeneration);
        if (!selected) return std::unexpected(selected.error());
        if (selected->identity.id != provider.identity.id || selected->identity.name != provider.identity.name)
            return Error(ErrorKind::NoProvider, 0, "FG request differs from the discovered pinned analytical provider");
        const auto removed = device->GetDeviceRemovedReason();
        if (FAILED(removed)) return Error(ErrorKind::DeviceLost, removed, "FG device removed before creation");
        state_->device = device; state_->limits = limits; state_->provider = provider;
        state_->create.header = {FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATION, &state_->backend.header};
        state_->create.displaySize = {limits.display.width, limits.display.height};
        state_->create.maxRenderSize = {limits.render.width, limits.render.height};
        state_->create.backBufferFormat = ffxApiGetSurfaceFormatDX12(limits.format);
        state_->create.flags = (limits.input.depthInverted ? FFX_FRAMEGENERATION_ENABLE_DEPTH_INVERTED : 0) |
            (limits.input.depthInfinite ? FFX_FRAMEGENERATION_ENABLE_DEPTH_INFINITE : 0) |
            (limits.input.motionIncludesJitter ? FFX_FRAMEGENERATION_ENABLE_MOTION_VECTORS_JITTER_CANCELLATION : 0) |
            (limits.debugChecking ? FFX_FRAMEGENERATION_ENABLE_DEBUG_CHECKING : 0);
        state_->backend = {{FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12, &state_->version.header}, device};
        state_->version = {{FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATION_VERSION, &state_->override.header}, FFX_FRAMEGENERATION_VERSION};
        state_->override = {{FFX_API_DESC_TYPE_OVERRIDE_VERSION, nullptr}, provider.identity.id};
        auto& callback = *session->callback_; callback.owner = state_.get(); callback.runtime = std::move(runtime);
        const auto result = callback.runtime->Functions().CreateContext(&callback.context, &state_->create.header, nullptr);
        if (result != FFX_API_RETURN_OK) {
            state_->poisoned = true;
            if (!callback.context) { callback.runtime.reset(); callback.owner = nullptr; }
            return Error(ErrorKind::ContextFailure, result, "Official FG context creation failed");
        }
        auto verified = callback.runtime->VerifyActualProvider(callback.context, provider);
        if (!verified) { state_->poisoned = true; return verified; }
        return {};
    }
    Result<void> FsrFrameGeneration::Configure(const FsrSdkLock& lock, IDXGISwapChain4* chain, uint64_t sourceId, bool enabled)
    {
        const auto session = state_->session;
        if (!session || !lock.Owns(*session) || session->closing_ || session->present_ || state_->poisoned ||
            session->callback_->owner != state_.get() || !session->callback_->context || !chain || !sourceId ||
            (session->callback_->configured && sourceId <= session->callback_->sourceId))
            return Error(ErrorKind::InvalidInput, 0, "FG Configure requires one newly consumed source under its SDK session lock");
        auto& callback = *session->callback_;
        state_->chain = chain;
        ffxConfigureDescFrameGeneration config{}; config.header.type = FFX_API_CONFIGURE_DESC_TYPE_FRAMEGENERATION;
        config.swapChain = chain; config.frameID = sourceId; config.frameGenerationEnabled = enabled;
        config.frameGenerationCallback = &GenerationCallback; config.frameGenerationCallbackUserContext = &callback;
        config.generationRect = {0, 0, static_cast<int32_t>(state_->limits.display.width), static_cast<int32_t>(state_->limits.display.height)};
        const auto result = callback.runtime->Functions().Configure(&callback.context, &config.header);
        callback.detached = false;
        if (result != FFX_API_RETURN_OK) { state_->poisoned = true; return Error(ErrorKind::ContextFailure, result, "FG Configure failed; lifecycle cleanup required"); }
        callback.configured = true; callback.sourceId = sourceId; callback.enabled = enabled; callback.preparedId = 0;
        return {};
    }
    Result<void> FsrFrameGeneration::Prepare(const FsrSdkLock& lock, ID3D12GraphicsCommandList* list,
        const UpscaleFrame& frame, const FsrGenerationResources& resources, bool reset)
    {
        const auto session = state_->session;
        if (!session || !lock.Owns(*session) || session->closing_ || session->present_ || state_->poisoned ||
            session->callback_->owner != state_.get() || !session->callback_->context || !session->callback_->enabled ||
            !session->callback_->configured || session->callback_->sourceId != frame.sourceId || session->callback_->preparedId == frame.sourceId)
            return Error(ErrorKind::InvalidInput, 0, "FG Prepare requires enabled Configure of exactly the same unprepared source");
        auto mapped = BuildFsrGenerationPrepare(list, frame, resources, state_->limits, reset);
        if (!mapped) return std::unexpected(mapped.error());
        ComPtr<ID3D12Device> device;
        if (FAILED(list->GetDevice(IID_PPV_ARGS(&device))) || device.Get() != state_->device.Get())
            return Error(ErrorKind::InvalidInput, 0, "FG Prepare resources belong to a different context device");
        const std::array<ID3D12Resource*,4> inputs{resources.scene, resources.depth, resources.motion, resources.ui};
        for (size_t i = 0; i < inputs.size(); ++i) {
            if (state_->resources[i] && state_->resources[i].Get() != inputs[i])
                return Error(ErrorKind::InvalidInput, 0, "FG transport allocations must remain stable until retirement and recreation");
        }
        for (size_t i = 0; i < inputs.size(); ++i) state_->resources[i] = inputs[i];
        auto& callback = *session->callback_;
        const auto result = callback.runtime->Functions().Dispatch(&callback.context, &mapped->header);
        if (result != FFX_API_RETURN_OK) { state_->poisoned = true; return Error(ErrorKind::DispatchFailure, result, "FG Prepare failed; retain resources for lifecycle retirement"); }
        callback.preparedId = frame.sourceId;
        return {};
    }
    ffxReturnCode_t FsrFrameGeneration::GenerationCallback(ffxDispatchDescFrameGeneration* descriptor, void* opaque) noexcept
    {
        if (!opaque) return FFX_API_RETURN_ERROR_PARAMETER;
        auto& callback = *static_cast<FsrSdkSession::CallbackState*>(opaque);
        const auto fail = [&callback](ffxReturnCode_t result) { callback.result.store(result, std::memory_order_relaxed); return result; };
        if (!callback.session || FsrSdkSession::ownedByThread_ != callback.session) return fail(FFX_API_RETURN_ERROR_PARAMETER);
        callback.invocations.fetch_add(1, std::memory_order_relaxed);
        if (!descriptor || descriptor->header.type != FFX_API_DISPATCH_DESC_TYPE_FRAMEGENERATION || !callback.session->present_ ||
            !callback.context || !callback.runtime || !callback.enabled || !callback.configured || callback.detached ||
            descriptor->frameID != callback.sourceId || callback.preparedId != callback.sourceId ||
            descriptor->numGeneratedFrames != 1 || descriptor->backbufferTransferFunction != FFX_API_BACKBUFFER_TRANSFER_FUNCTION_SRGB)
            return fail(FFX_API_RETURN_ERROR_PARAMETER);
        try {
            // SDK owns this descriptor and submission. Never rewrite it, query
            // a manual output/list, acquire a mutex, log, or call the game here.
            return fail(callback.runtime->Functions().Dispatch(&callback.context, &descriptor->header));
        } catch (...) { return fail(FFX_API_RETURN_ERROR); }
    }
    FsrGenerationCallbackOutcome FsrFrameGeneration::LastCallback(const FsrSdkLock& lock) const
    {
        if (!state_->session || !lock.Owns(*state_->session)) return {FFX_API_RETURN_ERROR_PARAMETER, 0};
        const auto& callback = *state_->session->callback_;
        return {callback.result.load(std::memory_order_relaxed), callback.invocations.load(std::memory_order_relaxed)};
    }
    Result<void> FsrFrameGeneration::DisableAndDetach(const FsrSdkLock& lock)
    {
        const auto session = state_->session;
        if (!session || !lock.Owns(*session) || !session->closing_ || session->present_ || session->callback_->owner != state_.get() || !session->callback_->context)
            return Error(ErrorKind::InvalidInput, 0, "FG lifecycle disable requires idle context ownership");
        auto& callback = *session->callback_;
        // Creation installs no swapchain callback. Before the first Configure
        // there is nothing to detach and a null-chain SDK Configure is invalid.
        if(callback.detached && !callback.configured && !state_->chain)return {};
        ffxConfigureDescFrameGeneration config{}; config.header.type = FFX_API_CONFIGURE_DESC_TYPE_FRAMEGENERATION;
        config.swapChain = state_->chain.Get(); config.frameID = callback.sourceId;
        const auto result = callback.runtime->Functions().Configure(&callback.context, &config.header);
        if (result != FFX_API_RETURN_OK) return Error(ErrorKind::ContextFailure, result, "FG lifecycle detach failed; retain owners");
        callback.enabled = false; callback.detached = true; callback.preparedId = 0;
        return {};
    }
    Result<void> FsrFrameGeneration::DestroyAfterRetirement(const FsrSdkLock& lock)
    {
        const auto session = state_->session;
        if (!session || !lock.Owns(*session) || session->present_ || session->callback_->owner != state_.get() ||
            !session->callback_->context || !session->callback_->detached)
            return Error(ErrorKind::InvalidInput, 0, "FG destruction requires detached callbacks and externally proven retirement");
        auto& callback = *session->callback_;
        const auto result = callback.runtime->Functions().DestroyContext(&callback.context, nullptr);
        if (result != FFX_API_RETURN_OK) return Error(ErrorKind::ContextFailure, result, "FG context destruction failed; retain owners");
        callback.runtime.reset(); callback.owner = nullptr; callback.configured = false; callback.sourceId = callback.preparedId = 0;
        state_->resources = {}; state_->chain.Reset(); state_->device.Reset(); state_->poisoned = false;
        return {};
    }
    Result<FfxApiEffectMemoryUsage> FsrFrameGeneration::QueryMemoryUsage(const FsrSdkLock& lock)
    {
        const auto session = state_->session;
        if (!session || !lock.Owns(*session) || session->callback_->owner != state_.get() || !session->callback_->context)
            return Error(ErrorKind::InvalidInput, 0, "FG memory query requires context session ownership");
        FfxApiEffectMemoryUsage usage{};
        ffxQueryDescFrameGenerationGetGPUMemoryUsage query{{FFX_API_QUERY_DESC_TYPE_FRAMEGENERATION_GPU_MEMORY_USAGE, nullptr}, &usage};
        const auto result = session->callback_->runtime->Functions().Query(&session->callback_->context, &query.header);
        if (result != FFX_API_RETURN_OK) return Error(ErrorKind::ContextFailure, result, "FG memory query failed");
        return usage;
    }
    bool FsrFrameGeneration::ContextOwned(const FsrSdkLock& lock)const
    {
        const auto session=state_->session;
        return session && lock.Owns(*session) && session->callback_->owner==state_.get() && session->callback_->context;
    }
}
