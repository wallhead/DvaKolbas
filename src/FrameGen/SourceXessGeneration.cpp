#include <PCH.h>
#include "NvidiaHost.h"
#include "RenderPipeline.h"
#include "SourceFrameGeneration.h"
#include "PluginPaths.h"
#include "SourceInternalScope.h"
#include "GameSwapChain.h"
#include "XessGenerationCompletedSource.h"
#include "HookSafety.h"
#include "PerformanceTuning.h"
#include <chrono>
#include "XessGenerationPacingWindow.h"

using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;

Telemetry::OutputCounter NvidiaHost::XessFgOutputCounter()const
{
#if defined(TRP_ENABLE_XESS_FG)
    if(XessFgActive() && xessPresentation_ && proxyActive_ && SUCCEEDED(FailureResult()) && lastPresentResult_==S_OK)
        return xessPresentation_->OutputCounter();
#endif
    return {};
}
SettingsActionStatus NvidiaHost::XessFgStatus()const
{
#if defined(TRP_ENABLE_XESS_FG)
    if(XessFgActive() && xessPresentation_) {
        if(!XessEngineHooks::installed.load(std::memory_order_acquire))
            return {"Intel real-only output: engine timing inactive; requires inspected Skyrim 1.6.1170 call sites.",SettingsStatusKind::Pending};
        const auto status=xessPresentation_->Status();
        if(static_cast<int>(status.frameGenResult)<0)
            return {std::format("Intel SDK could not generate this frame (result {}). Real output retained.",static_cast<int>(status.frameGenResult)),SettingsStatusKind::Error};
        const bool active=XessFgOutputCounter().available && status.isFrameGenEnabled && status.framesPresented>1;
        return {xessPresentation_->Reason(),FAILED(FailureResult())?SettingsStatusKind::Error:active?SettingsStatusKind::Success:SettingsStatusKind::Neutral};
    }
#endif
    return {"XeSS FG is not the running presenter. Save the backend selection and restart.",SettingsStatusKind::Neutral};
}

#if defined(TRP_ENABLE_XESS_FG)
namespace
{
    // Fixed-size, sparse bursts retain adjacent frames without per-frame I/O.
    // Durations are CPU API waits, never isolated GPU or display latency.
    struct PacingBurst
    {
        struct Sample
        {
            std::uint64_t source{},epoch{};
            std::uint32_t sdkId{},nextSdkId{};
            double inputMs{},prepareMs{},presentMs{},sleepMs{},totalMs{};
            UINT interval{},flags{};
            unsigned frames{};
            bool requested{};
        };
        std::array<Sample,XessGenerationPacingWindow::Size> samples{};
        XessGenerationPacingWindow window;
        void Reset() { window.Reset(); }
        bool Begin(std::uint64_t source,std::uint64_t epoch,std::uint64_t generation)
        { return window.Begin(source,epoch,generation); }
        void Record(const Sample& sample)
        {
            if(window.Capturing())samples[window.Index()]=sample;
            if(window.Commit()) {
                std::string rows;
                for(const auto& s:samples)
                    rows+=std::format("\n{},{},{},{},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{},{},{},{}",
                        s.source,s.epoch,s.sdkId,s.nextSdkId,s.inputMs,s.prepareMs,s.presentMs,s.sleepMs,s.totalMs,
                        s.interval,s.flags,s.frames,s.requested);
                logger::info("[XeSS pacing burst] CPU milliseconds; -1=not called; Sleep belongs to NEXT source; SDK frameRenderTime hint=unavailable(0); inputFrameMs retains source cadence; tagged non-Intel prepareMs includes current publication readiness; no physical-display claim. Columns=source,epoch,sdkId,nextSdkId,inputFrameMs,prepareMs,proxyPresentMs,nextSleepMs,sourceCallMs,interval,flags,frames,requested{}",rows);
            }
        }
    };
    XessEngineHooks::Observer& EngineObserver()
    {
        // Singleton and descriptor both outlive all hook callbacks.
        static XessEngineHooks::Observer observer{NvidiaHost::GetSingleton(),nullptr};
        return observer;
    }
}
HRESULT NvidiaHost::CreateXessPresenter(IDXGIFactory* factory,ID3D11Device* producer,
    const DXGI_SWAP_CHAIN_DESC& input,IDXGISwapChain** output)
{
    ++xessDiagnosticGeneration_;
    if(!output)return E_POINTER;*output=nullptr;
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgi;
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    Microsoft::WRL::ComPtr<ID3D12Device> native;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
    auto hr=producer->QueryInterface(IID_PPV_ARGS(&dxgi));
    if(SUCCEEDED(hr))hr=dxgi->GetAdapter(&adapter);
    if(SUCCEEDED(hr))hr=ReShadeIntegration::Get().CreateSourceDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,&native,true);
    D3D12_COMMAND_QUEUE_DESC queueDesc{};queueDesc.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
    if(SUCCEEDED(hr))hr=native->CreateCommandQueue(&queueDesc,IID_PPV_ARGS(&queue));
    auto bridge=std::make_shared<Graphics::D3D11D3D12Interop>();
    if(SUCCEEDED(hr))hr=bridge->Initialize(producer,native.Get(),queue.Get());
    if(FAILED(hr)){status_="Intel FG same-adapter native device/queue ownership unavailable";return hr;}
    auto descriptor=input;
    if(!descriptor.BufferDesc.Width || !descriptor.BufferDesc.Height) {
        RECT client{};
        if(!GetClientRect(descriptor.OutputWindow,&client) || client.right<=0 || client.bottom<=0)return E_INVALIDARG;
        descriptor.BufferDesc.Width=client.right;descriptor.BufferDesc.Height=client.bottom;
    }
    descriptor.BufferCount=2;descriptor.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
    descriptor.Flags&=DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT|DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    xessPresentation_=std::make_unique<XessGenerationHost>(PluginPaths::Directory());
    auto created=xessPresentation_->Create(factory,producer,descriptor,bridge);
    if(!created){status_=created.error().message;return E_FAIL;}
    D3D11_TEXTURE2D_DESC scene{};
    scene.Width=descriptor.BufferDesc.Width;scene.Height=descriptor.BufferDesc.Height;
    scene.Format=descriptor.BufferDesc.Format;scene.SampleDesc.Count=1;scene.MipLevels=scene.ArraySize=1;
    scene.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    if(FAILED(hr=bridge->CreateSharedTexture(scene,xessPresentationScene_))) {
        status_="Intel game-facing SDR scene allocation failed";return hr;
    }
    xessPresentationDescriptor_=descriptor;
    auto& observer=EngineObserver();
    observer.observe=+[](void* owner,XessEngineHooks::Boundary boundary) noexcept {
        static_cast<NvidiaHost*>(owner)->ObserveXessEngine(boundary);
    };
    if(!XessEngineHooks::Bind(&observer)){status_="Intel engine observer already has an owner";return E_UNEXPECTED;}
    *output=xessPresentation_->SwapChain();(*output)->AddRef();
    logger::info("[XeSS FG startup] native queue={} output={}x{} SDR; other FG/Reflex initialization suppressed; timing pending",
        static_cast<void*>(bridge->Queue()),scene.Width,scene.Height);
    return S_OK;
}
void NvidiaHost::StopXessEngineObserver()
{ ++xessDiagnosticGeneration_;XessEngineHooks::gameplayInput.Cancel();XessEngineHooks::Unbind(&EngineObserver()); }
std::uint64_t NvidiaHost::IntelSourceEpoch()const
{
#if !defined(TRP_NO_NEURAL_RENDERING)
    return communityEpoch_;
#elif defined(TRP_ENABLE_XESS)
    return XessActive()?xessEpoch_:1;
#else
    return 1;
#endif
}
void NvidiaHost::ObserveXessEngine(XessEngineHooks::Boundary boundary) noexcept
{
    try {
        // Native input is diagnostic only in presentation-pacing mode.
        // This inspected boundary supplies render timing, not pre-input proof.
        if(boundary!=XessEngineHooks::Boundary::BeforeRender)return;
        if(!XessFgActive() || !xessPresentation_ || !proxyActive_ || FAILED(FailureResult()) ||
            !XessEngineHooks::installed.load(std::memory_order_acquire) || XessPresentSuspended())return;
        if(!xessTimingBound_) {
            const auto bound=xessPresentation_->BindTiming(true,XessGenerationEngineTiming::Mode::PresentationPacing);
            if(!bound){logger::warn("[XeSS engine] timing not bound: {}",bound.error().message);return;}
            xessTimingOwnerThread_.store(GetCurrentThreadId(),std::memory_order_release);
            xessTimingBound_=true;
            logger::info("[XeSS timing] mode=presentation-pacing; input timing diagnostic; pre-input latency unqualified; consecutive input gate disabled");
        }
        const auto source=RenderPipeline::GetSingleton()->mRenderedFrameCount+1;
        const auto epoch=IntelSourceEpoch();
        Result<void> result=std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,
            "no exact-source presentation pacing reservation"});
        if(source==xessEngineSource_ && epoch==xessEngineEpoch_)
            result=xessPresentation_->BeforeRender(source);
        // Present owns render-end and Present markers.
        auto* ui=RE::UI::GetSingleton();
        const bool world=ui && !ui->IsMenuOpen(RE::MainMenu::MENU_NAME) && !ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME);
        if(world && xessEngineTrace_==0) {
            logger::info("[XeSS engine hooks] preInputCalls={} inputCalls={} renderCalls={}",
                XessEngineHooks::boundaryCalls[0].load(),XessEngineHooks::boundaryCalls[1].load(),
                XessEngineHooks::boundaryCalls[2].load());
            for(unsigned index=0;index<XessEngineHooks::witnesses.size();++index) {
                const auto& witness=XessEngineHooks::witnesses[index];std::uint64_t prefix{};
                const bool readable=HookSafety::Read(witness.callee,&prefix,sizeof(prefix));
                logger::info("[XeSS engine hooks] site={} retainedCall={} originalCallee={} prefixReadable={} prefix=0x{:016X}",
                    index,HookSafety::Bytes(witness.site,witness.installedCall),
                    reinterpret_cast<void*>(witness.callee),readable,prefix);
            }
        }
        if(world && xessEngineTrace_<512) {
            ++xessEngineTrace_;
            logger::info("[XeSS engine trace] event={} boundary={} source={} epoch={} rendered={} thread={} accepted={} reason={}",
                xessEngineTrace_,static_cast<unsigned>(boundary),xessEngineSource_,xessEngineEpoch_,
                RenderPipeline::GetSingleton()->mRenderedFrameCount,GetCurrentThreadId(),bool(result),
                result?"ordered":result.error().message);
        }
    } catch(...) { XessEngineHooks::gameplayInput.Cancel();logger::error("[XeSS engine] callback failed; interpolation held off"); }
}
#endif

HRESULT NvidiaHost::WaitXessProducer()
{
#if defined(TRP_ENABLE_XESS_FG)
    if(!XessFgActive() || !xessPresentation_)return E_UNEXPECTED;
    const auto result=xessPresentation_->WaitBeforeProducer();
    if(FAILED(result))return FailLifecycle(result,"Intel producer ownership");
    return result;
#else
    return E_NOTIMPL;
#endif
}
bool NvidiaHost::XessPresentSuspended()const
{
#if defined(TRP_ENABLE_XESS_FG)
    return xessPresentation_ && xessPresentation_->Suspended();
#else
    return false;
#endif
}
HRESULT NvidiaHost::UpdateXessSuspension()
{
#if defined(TRP_ENABLE_XESS_FG)
    if(!XessFgActive() || !xessPresentation_)return S_OK;
    RECT client{};
    if(IsIconic(outputWindow_) || !GetClientRect(outputWindow_,&client) || client.right<=0 || client.bottom<=0) {
        XessEngineHooks::gameplayInput.Cancel();
        if(!XessPresentSuspended()) {
            ++xessDiagnosticGeneration_;
            auto paused=xessPresentation_->Suspend();if(!paused){status_=paused.error().message;return FailLifecycle(E_FAIL,"Intel minimize retirement");}
        }
        return DXGI_STATUS_OCCLUDED;
    }
    if(XessPresentSuspended()) {
        ++xessDiagnosticGeneration_;
        auto resumed=xessPresentation_->Resume();if(!resumed){status_=resumed.error().message;return FailLifecycle(E_FAIL,"Intel restore");}
        resetNextEvaluation_=true;nativeUIPass_.ResetEvaluation();
    }
#endif
    return S_OK;
}
HRESULT NvidiaHost::ResizeXessSwapChain(GameSwapChain& outer,UINT count,UINT width,UINT height,DXGI_FORMAT format,UINT flags)
{
#if defined(TRP_ENABLE_XESS_FG)
    if(!XessFgActive() || &outer!=outerSwapChain_ || !xessPresentation_)return E_UNEXPECTED;
    if(FAILED(FailureResult()))return FailureResult();
    if(IsIconic(outputWindow_))return UpdateXessSuspension()==DXGI_STATUS_OCCLUDED?S_OK:FailureResult();
    auto request=xessPresentationDescriptor_;
    if(count)request.BufferCount=count;
    if(width)request.BufferDesc.Width=width;
    if(height)request.BufferDesc.Height=height;
    if(format!=DXGI_FORMAT_UNKNOWN)request.BufferDesc.Format=format;
    request.Flags=flags;
    XessEngineHooks::gameplayInput.Cancel();
    const auto resized=xessPresentation_->Resize(request);
    if(!resized){logger::warn("[XeSS FG resize] {}",resized.error().message);return DXGI_ERROR_INVALID_CALL;}
    return S_OK;
#else
    return E_NOTIMPL;
#endif
}
HRESULT NvidiaHost::PresentXessSource(UINT interval,UINT flags)
{
#if defined(TRP_ENABLE_XESS_FG)
    // Include early startup/test/error paths and foreign loading callbacks,
    // without changing the order or number of existing readiness queries.
    if(FAILED(FailureResult())){++xessDiagnosticGeneration_;return FailureResult();}
    if(!xessPresentation_){++xessDiagnosticGeneration_;return E_UNEXPECTED;}
    if((flags&DXGI_PRESENT_TEST) || !UpscalerReady()){
        ++xessDiagnosticGeneration_;return xessPresentation_->StartupPresent(interval,flags);
    }
    if(!fsrUiComplete_ || !nativeUI_.Dedicated()){
        ++xessDiagnosticGeneration_;return FailLifecycle(DXGI_ERROR_INVALID_CALL,"Intel completed scene/HUD boundary");
    }
    auto frame=fsrGenerationFrame_;
    // The final scene includes SR, NR, ReShade and fade, while the dedicated
    // game HUD/foreground remains separate for Intel interpolation.
    frame.display={outputWidth_,outputHeight_};
    const auto encoding=XessActive()?sourceUpscalerSettings_.Startup().xess.sourceEncoding:sourceUpscalerSettings_.Startup().fsr.sourceColorEncoding;
    auto completed=CompleteXessGenerationSource(frame,presentation_.Texture(),encoding);
    if(!completed){status_=completed.error().message;return FailLifecycle(E_FAIL,"Intel final real scene identity");}
    frame=*completed;
    const auto outcome=fsrSourcePending_?fsrGenerationOutcome_:UpscaleOutcome::RepeatedOutput;
    if(!fsrSourcePending_)frame.sourceId=frame.sourceEpoch=0;
    const bool requested=SourceFrameGeneration::GetSingleton()->RuntimeInterpolationRequested();
    const bool sourceProof=frame.sourceId && frame.sourceId==xessEngineSource_ && frame.sourceEpoch==xessEngineEpoch_ &&
        xessTimingOwnerThread_.load(std::memory_order_acquire)==GetCurrentThreadId();
    static thread_local PacingBurst pacing;
    const auto& diagnostics=PerformanceTuning::GetSingleton()->settings;
    const bool diagnosticWorld=diagnostics.enableGPUTimings && diagnostics.diagnostics.performanceMetrics &&
        !fsrMenu_ && outcome==UpscaleOutcome::Temporal;
    if(!diagnosticWorld)pacing.Reset();
    const bool capture=diagnosticWorld && pacing.Begin(frame.sourceId,frame.sourceEpoch,xessDiagnosticGeneration_.load());
    XessGenerationHost::PresentTiming timing;
    double nextSleepMs=-1;
    std::uint32_t nextSdkId{};
    const auto callStart=capture?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
    const auto result=xessPresentation_->Present(frame,outcome,nativeUI_.TaggedTexture(),fsrForeground_.Get(),true,fsrMenu_,requested,interval,flags,sourceProof,capture?&timing:nullptr);
    const auto status=xessPresentation_->Status();
    const bool previouslyEnabled=frameGenerationEnabled_;
    frameGenerationEnabled_=result==S_OK && static_cast<int>(status.frameGenResult)>=0 && status.framesPresented==2;
    if(previouslyEnabled!=frameGenerationEnabled_)
        logger::info("[XeSS FG state] active={} requested={} source={} epoch={} sdk={} reason={}",frameGenerationEnabled_,requested,
            frame.sourceId,frame.sourceEpoch,static_cast<int>(status.frameGenResult),xessPresentation_->Reason());
    if((!fsrMenu_ && xessPresentTrace_++<160) || FAILED(result) || (presentCount_%600==0))
        logger::info("[XeSS FG Present] source={} epoch={} temporal={} sourceProof={} timing=presentation-pacing requested={} frames={} sdk={} orderedSources={} drainSuspends={} skippedCycles={} result=0x{:08X} reason={}",
            frame.sourceId,frame.sourceEpoch,outcome==UpscaleOutcome::Temporal,sourceProof,requested,status.framesPresented,
            static_cast<int>(status.frameGenResult),xessPresentation_->OrderedSources(),xessPresentation_->DrainSuspends(),
            xessPresentation_->SkippedCycles(),static_cast<std::uint32_t>(result),xessPresentation_->Reason());
    fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();
    // AIO19-style pacing: sleep for the next exact source after real Present.
    // Render and completed scene IDs still have to match this reservation.
    // Engine input may already have run; no pre-input latency claim is made.
    // Menu, loading/repeated/test/error Presents cannot start another cycle.
    using NextSource=XessGenerationInputHandoff::NextSource;
    const auto nextAction=XessGenerationInputHandoff::AfterPresent(result==S_OK,outcome==UpscaleOutcome::Temporal,
        outcome==UpscaleOutcome::RepeatedOutput,fsrMenu_,frame.reset || frame.camera.reset);
    if(nextAction!=NextSource::Preserve)XessEngineHooks::gameplayInput.Cancel();
    if(nextAction==NextSource::Reserve && xessTimingOwnerThread_.load(std::memory_order_acquire)==GetCurrentThreadId() &&
        frame.sourceId && frame.sourceId<std::numeric_limits<std::uint64_t>::max() && frame.sourceEpoch &&
        XessEngineHooks::installed.load(std::memory_order_acquire) && !XessPresentSuspended()) {
        const auto next=frame.sourceId+1;
        const auto begin=xessPresentation_->BeforeSourceLoop(next,frame.sourceEpoch,capture?&nextSleepMs:nullptr,capture?&nextSdkId:nullptr);
        if(begin) {
            xessEngineSource_=next;xessEngineEpoch_=frame.sourceEpoch;
            if(xessPresentTrace_<160 || presentCount_%600==0)
                logger::info("[XeSS next source] source={} epoch={} sdkId={} timing=presentation-pacing",next,frame.sourceEpoch,*begin);
        } else if(xessPresentTrace_<160)
            logger::warn("[XeSS next source] source={} epoch={} not reserved: {}",next,frame.sourceEpoch,begin.error().message);
    }
    if(diagnosticWorld && result==S_OK) {
        const auto total=capture?std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-callStart).count():0;
        pacing.Record({frame.sourceId,frame.sourceEpoch,timing.sdkId,nextSdkId,frame.deltaMilliseconds,
            timing.prepareMs,timing.proxyPresentMs,nextSleepMs,total,interval,flags,status.framesPresented,requested});
    } else pacing.Reset(); // Occluded/non-S_OK output cannot bridge adjacent samples.
    return FAILED(result)?FailLifecycle(result,"Intel source Present"):result;
#else
    return E_NOTIMPL;
#endif
}
