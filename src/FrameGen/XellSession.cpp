#include "XellSession.h"
#include <wrl/client.h>
#include <optional>
#include <limits>
#include <chrono>
namespace TheosRenderPipeline
{
    namespace
    {
        using Upscaling::RuntimeError;
        using Upscaling::ErrorKind;
        Upscaling::Result<void> Invalid(const char* reason)
        { return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,reason}); }
    }
    struct XellSession::State
    {
        std::shared_ptr<XessGenerationRuntime> runtime;
        Microsoft::WRL::ComPtr<ID3D12Device> device;
        xell_context_handle_t context{};
        std::optional<std::uint32_t> lastId;
        std::uint32_t nextMarker{};
        bool enabled{},active{},deviceLost{};
        Upscaling::Result<void> Check(xell_result_t result,const char* stage)
        {
            if (result==XELL_RESULT_SUCCESS) return {};
            deviceLost|=result==XELL_RESULT_ERROR_DEVICE;
            const auto kind=deviceLost?ErrorKind::DeviceLost:
                result==XELL_RESULT_ERROR_UNSUPPORTED_DEVICE || result==XELL_RESULT_ERROR_UNSUPPORTED_DRIVER?ErrorKind::UnsupportedDevice:ErrorKind::DispatchFailure;
            return std::unexpected(RuntimeError{kind,result,stage});
        }
        Upscaling::Result<void> Ready() const
        {
            if (!context || !runtime) return Invalid("XeLL context is not created");
            if (deviceLost) return std::unexpected(RuntimeError{ErrorKind::DeviceLost,XELL_RESULT_ERROR_DEVICE,"XeLL device lost; stop admissions and retain pending owners"});
            return {};
        }
    };
    XellSession::XellSession():state_(std::make_unique<State>()) {}
    XellSession::~XellSession()
    {
        // Destructor cannot prove the FG proxy is destroyed or the GPU drained.
        // Explicit Retire is mandatory; failure keeps the context/device/modules.
        if (state_->context) {
            OutputDebugStringW(L"RaZkolbaS: XeLL context retained until process exit without proven retirement\n");
            (void)state_.release();
        }
    }
    Upscaling::Result<void> XellSession::Create(ID3D12Device* device,std::shared_ptr<XessGenerationRuntime> runtime)
    {
        if (!device || !runtime || state_->context) return Invalid("XeLL creation needs a device, retained runtime and empty session");
        state_->device=device;state_->runtime=std::move(runtime);
        const auto result=state_->Check(state_->runtime->Latency().CreateContext(device,&state_->context),"XeLL CreateContext failed");
        if (!result) return result;
        if (!state_->context) return std::unexpected(RuntimeError{ErrorKind::ContextFailure,0,"XeLL CreateContext returned success without a context"});
        return SetEnabled(true,true);
    }
    Upscaling::Result<void> XellSession::BeginFrame(std::uint32_t id,double* sleepMs,std::uint32_t* sleepSdkId)
    {
        if(sleepMs)*sleepMs=-1;
        if(sleepSdkId)*sleepSdkId=0;
        if (auto ready=state_->Ready(); !ready) return ready;
        if (state_->active) return Invalid("XeLL frame is incomplete; extra Presents cannot begin a simulation");
        if (state_->lastId && (*state_->lastId==std::numeric_limits<std::uint32_t>::max() || id!=*state_->lastId+1))
            return Invalid("XeLL SDK ID must advance exactly once; wrap requires a drained epoch reset");
        // Reserve the identity before sleeping. A failed partial Begin cannot
        // repeat sleep under that same identity without explicit drained reset.
        state_->lastId=id;state_->active=true;state_->nextMarker=0;
        const auto& api=state_->runtime->Latency();
        if(sleepSdkId)*sleepSdkId=id;
        const auto start=sleepMs?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
        const auto result=api.Sleep(state_->context,id);
        if(sleepMs)*sleepMs=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
        if (auto sleep=state_->Check(result,"XeLL Sleep failed"); !sleep) return sleep;
        if (auto marker=state_->Check(api.AddMarkerData(state_->context,id,XELL_SIMULATION_START),"XeLL SimulationStart failed"); !marker) return marker;
        state_->nextMarker=1;
        return {};
    }
    Upscaling::Result<void> XellSession::Marker(std::uint32_t id,xell_latency_marker_type_t marker)
    {
        if (auto ready=state_->Ready(); !ready) return ready;
        if (!state_->active || !state_->lastId || id!=*state_->lastId || state_->nextMarker==0 ||
            static_cast<std::uint32_t>(marker)!=state_->nextMarker || marker>XELL_PRESENT_END)
            return Invalid("XeLL marker ID/phase does not match the active engine frame");
        if (auto result=state_->Check(state_->runtime->Latency().AddMarkerData(state_->context,id,marker),"XeLL marker failed"); !result) return result;
        ++state_->nextMarker;
        if (marker==XELL_PRESENT_END) state_->active=false;
        return {};
    }
    Upscaling::Result<void> XellSession::SetEnabled(bool enabled,bool gpuQuiescent)
    {
        if (auto ready=state_->Ready(); !ready) return ready;
        if (state_->enabled==enabled) return {};
        if (!gpuQuiescent || state_->active)
            return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,0,"XeLL sleep-mode change requires a completed frame and quiescent GPU"});
        xell_sleep_params_t params{};
        params.minimumIntervalUs=0;params.bLowLatencyMode=enabled;
        if (auto result=state_->Check(state_->runtime->Latency().SetSleepMode(state_->context,&params),"XeLL SetSleepMode failed"); !result) return result;
        state_->enabled=enabled;return {};
    }
    Upscaling::Result<void> XellSession::ResetAfterDrain(bool gpuQuiescent)
    {
        if (auto ready=state_->Ready(); !ready) return ready;
        if (!gpuQuiescent) return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,0,"XeLL epoch reset requires drained GPU work"});
        state_->active=false;state_->lastId.reset();state_->nextMarker=0;
        return {};
    }
    Upscaling::Result<void> XellSession::AbandonUnsubmittedFrame()
    {
        if (auto ready=state_->Ready(); !ready) return ready;
        state_->active=false;state_->nextMarker=0;
        return {};
    }
    Upscaling::Result<void> XellSession::Retire(bool fgDestroyed,bool gpuQuiescent)
    {
        if (!state_->context) { state_=std::make_unique<State>();return {}; }
        if (!fgDestroyed || !gpuQuiescent)
            return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,0,"XeLL retirement requires destroyed FG proxy and quiescent GPU; retaining owners"});
        const auto result=state_->runtime->Latency().DestroyContext(state_->context);
        if (result!=XELL_RESULT_SUCCESS)
            return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,result,"XeLL DestroyContext failed; retaining owners for proven cleanup retry"});
        state_->context=nullptr;state_=std::make_unique<State>();return {};
    }
    xell_context_handle_t XellSession::Context() const { return state_->context; }
}
