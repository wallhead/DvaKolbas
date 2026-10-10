#include <PCH.h>
#include "NvidiaHost.h"
#include "RenderPipeline.h"
#include "SourceFrameGeneration.h"
#include "PluginPaths.h"
#include "SourceInternalScope.h"
#include "GameSwapChain.h"

using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;

#if defined(TRP_ENABLE_XESS_FG)
namespace
{
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
    xessPresentation_->RequireOrderedSources(128);
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
{ XessEngineHooks::Unbind(&EngineObserver()); }
void NvidiaHost::ObserveXessEngine(XessEngineHooks::Boundary boundary) noexcept
{
    try {
        if(!XessFgActive() || !xessPresentation_ || !proxyActive_ || FAILED(FailureResult()) ||
            !XessEngineHooks::installed.load(std::memory_order_acquire) || XessPresentSuspended())return;
        if(!xessTimingBound_) {
            const auto bound=xessPresentation_->BindTiming(true);
            if(!bound){logger::warn("[XeSS engine] timing not bound: {}",bound.error().message);return;}
            xessTimingBound_=true;
        }
        Result<void> result;
        if(boundary==XessEngineHooks::Boundary::BeforeUpdate) {
            xessEngineSource_=RenderPipeline::GetSingleton()->mRenderedFrameCount+1;
#if !defined(TRP_NO_NEURAL_RENDERING)
            xessEngineEpoch_=communityEpoch_;
#else
            xessEngineEpoch_=1;
#endif
            const auto begin=xessPresentation_->BeforeSourceLoop(xessEngineSource_,xessEngineEpoch_);
            if(!begin)result=std::unexpected(begin.error());
        } else if(boundary==XessEngineHooks::Boundary::InputSampled)result=xessPresentation_->InputSampled(xessEngineSource_);
        else if(boundary==XessEngineHooks::Boundary::BeforeRender)result=xessPresentation_->BeforeRender(xessEngineSource_);
        // AfterUpdate is observational. Present owns render-end and Present markers.
        auto* ui=RE::UI::GetSingleton();
        const bool world=ui && !ui->IsMenuOpen(RE::MainMenu::MENU_NAME) && !ui->IsMenuOpen(RE::LoadingMenu::MENU_NAME);
        if(world && xessEngineTrace_<512) {
            ++xessEngineTrace_;
            logger::info("[XeSS engine trace] event={} boundary={} source={} epoch={} rendered={} thread={} accepted={} reason={}",
                xessEngineTrace_,static_cast<unsigned>(boundary),xessEngineSource_,xessEngineEpoch_,
                RenderPipeline::GetSingleton()->mRenderedFrameCount,GetCurrentThreadId(),bool(result),
                result?"ordered":result.error().message);
        }
    } catch(...) { logger::error("[XeSS engine] callback failed; interpolation held off"); }
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
        if(!XessPresentSuspended()) {
            auto paused=xessPresentation_->Suspend();if(!paused){status_=paused.error().message;return FailLifecycle(E_FAIL,"Intel minimize retirement");}
        }
        return DXGI_STATUS_OCCLUDED;
    }
    if(XessPresentSuspended()) {
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
    if(FAILED(FailureResult()))return FailureResult();
    if(!xessPresentation_)return E_UNEXPECTED;
    if((flags&DXGI_PRESENT_TEST) || !UpscalerReady())return xessPresentation_->StartupPresent(interval,flags);
    if(!fsrUiComplete_ || !nativeUI_.Dedicated())return FailLifecycle(DXGI_ERROR_INVALID_CALL,"Intel completed scene/HUD boundary");
    auto frame=fsrGenerationFrame_;
    // The final scene includes SR, NR, ReShade and fade, while the dedicated
    // game HUD/foreground remains separate for Intel interpolation.
    frame.output=presentation_.Texture();frame.display={outputWidth_,outputHeight_};
    frame.outputEncoding=XessActive()?sourceUpscalerSettings_.Startup().xess.sourceEncoding:sourceUpscalerSettings_.Startup().fsr.sourceColorEncoding;
    frame.uiEncoding=ColorEncoding::SRGB;
    if(frame.depth && frame.motion) {
        D3D11_TEXTURE2D_DESC depth{},motion{};frame.depth->GetDesc(&depth);frame.motion->GetDesc(&motion);
        frame.depthExtent={depth.Width,depth.Height};frame.motionExtent={motion.Width,motion.Height};
        frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=motion.Format;
    }
    const auto outcome=fsrSourcePending_?fsrGenerationOutcome_:UpscaleOutcome::RepeatedOutput;
    if(!fsrSourcePending_)frame.sourceId=frame.sourceEpoch=0;
    const bool requested=SourceFrameGeneration::GetSingleton()->RuntimeInterpolationRequested();
    const auto result=xessPresentation_->Present(frame,outcome,nativeUI_.TaggedTexture(),fsrForeground_.Get(),true,fsrMenu_,requested,interval,flags);
    const auto status=xessPresentation_->Status();
    frameGenerationEnabled_=SUCCEEDED(result) && status.framesPresented>1;
    if((!fsrMenu_ && xessPresentTrace_++<160) || FAILED(result) || (presentCount_%600==0))
        logger::info("[XeSS FG Present] source={} epoch={} temporal={} requested={} frames={} sdk={} orderedSources={} result=0x{:08X} reason={}",
            frame.sourceId,frame.sourceEpoch,outcome==UpscaleOutcome::Temporal,requested,status.framesPresented,
            static_cast<int>(status.frameGenResult),xessPresentation_->OrderedSources(),static_cast<std::uint32_t>(result),xessPresentation_->Reason());
    fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();
    return FAILED(result)?FailLifecycle(result,"Intel source Present"):result;
#else
    return E_NOTIMPL;
#endif
}
