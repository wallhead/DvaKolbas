#include "XessGenerationHost.h"
#include <dxgi1_4.h>
namespace TheosRenderPipeline
{
    using namespace Upscaling;
    using Microsoft::WRL::ComPtr;
    namespace
    {
        Result<void> Invalid(const char* reason)
        { return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,reason}); }
        bool SameDevice(ID3D11Device* left,ID3D11Device* right)
        {
            ComPtr<IUnknown> a,b;
            return left && right && SUCCEEDED(left->QueryInterface(IID_PPV_ARGS(&a))) &&
                SUCCEEDED(right->QueryInterface(IID_PPV_ARGS(&b))) && a.Get()==b.Get();
        }
    }
    struct XessGenerationHost::State
    {
        std::filesystem::path directory;
        std::string reason{"Intel host not created"};
        ComPtr<ID3D11Device> producer;
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;
        XessGenerationPresentation presentation;
        XessGenerationEngineTiming timing;
        XessGenerationHistory history;
        DXGI_SWAP_CHAIN_DESC descriptor{};
        bool created{},attempted{},closing{},suspended{},bound{};
        HRESULT fault{S_OK};
        DWORD thread{};
        std::uint64_t loopSource{},loopEpoch{};
        bool unfinished{};
        std::uint32_t initFlags{};
        unsigned requiredOrdered{},ordered{};
        Result<void> Ready() const
        {
            if(thread && thread!=GetCurrentThreadId())return Invalid("Intel host operation differs from its producer thread");
            if(!created || closing || suspended || !bridge || !bridge->Ready() || FAILED(fault))
                return std::unexpected(RuntimeError{ErrorKind::ContextFailure,FAILED(fault)?fault:E_UNEXPECTED,"Intel host is inactive/suspended or failed"});
            return {};
        }
        Result<void> Failure(const RuntimeError& error)
        { reason=error.message;fault=error.kind==ErrorKind::DeviceLost?DXGI_ERROR_DEVICE_REMOVED:E_FAIL;return std::unexpected(error); }
    };
    XessGenerationHost::XessGenerationHost(std::filesystem::path path):state_(std::make_unique<State>())
    { state_->directory=std::move(path); }
    XessGenerationHost::~XessGenerationHost()
    { if(state_ && !Retire())(void)state_.release(); }
    Result<void> XessGenerationHost::Create(IDXGIFactory* factory,ID3D11Device* producer,const DXGI_SWAP_CHAIN_DESC& input,
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge,std::uint32_t flags)
    {
        if(state_->attempted || !factory || !producer || !bridge || !bridge->Ready())return Invalid("Intel creation requires an empty host and a ready native bridge");
        ComPtr<ID3D11Device> actual;bridge->Context11()->GetDevice(&actual);
        if(!SameDevice(producer,actual.Get()))return Invalid("Intel bridge D3D11 identity differs from the game's retained producer");
        ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;DXGI_ADAPTER_DESC adapterDesc{};
        auto hr=producer->QueryInterface(IID_PPV_ARGS(&dxgi));
        if(SUCCEEDED(hr))hr=dxgi->GetAdapter(&adapter);
        if(SUCCEEDED(hr))hr=adapter->GetDesc(&adapterDesc);
        if(FAILED(hr))return std::unexpected(RuntimeError{ErrorKind::ContextFailure,hr,"Intel producer adapter identity query failed"});
        const auto native=bridge->Device12()->GetAdapterLuid();
        if(native.HighPart!=adapterDesc.AdapterLuid.HighPart || native.LowPart!=adapterDesc.AdapterLuid.LowPart)
            return Invalid("Intel native device LUID differs from the rendering D3D11 adapter");
        D3D12_FEATURE_DATA_SHADER_MODEL model{D3D_SHADER_MODEL_6_4};
        if(adapterDesc.VendorId!=0x8086 && (FAILED(bridge->Device12()->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&model,sizeof(model))) || model.HighestShaderModel<D3D_SHADER_MODEL_6_4))
            return std::unexpected(RuntimeError{ErrorKind::UnsupportedDevice,E_NOINTERFACE,"Intel FG requires measured Shader Model 6.4 on this non-Intel rendering adapter"});
        auto descriptor=input;
        if(!descriptor.Windowed || descriptor.SampleDesc.Count!=1 || descriptor.BufferDesc.Format!=DXGI_FORMAT_R8G8B8A8_UNORM)
            return Invalid("Intel first trial requires borderless single-sample R8 SDR output");
        if(!descriptor.BufferDesc.Width || !descriptor.BufferDesc.Height) {
            RECT client{};if(!GetClientRect(descriptor.OutputWindow,&client) || client.right<=client.left || client.bottom<=client.top)return Invalid("Intel output client extent is unavailable");
            descriptor.BufferDesc.Width=client.right-client.left;descriptor.BufferDesc.Height=client.bottom-client.top;
        }
        descriptor.BufferCount=2;descriptor.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
        descriptor.Flags&=DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT|DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        auto runtime=XessGenerationRuntime::Load(state_->directory);
        if(!runtime)return state_->Failure(runtime.error());
        state_->producer=producer;state_->bridge=std::move(bridge);state_->descriptor=descriptor;
        state_->thread=GetCurrentThreadId();state_->attempted=true;
        auto created=state_->presentation.Create(factory,*runtime,state_->bridge,descriptor,flags);
        if(!created)return state_->Failure(created.error());
        state_->initFlags=flags;
        state_->created=true;state_->reason="Intel presenter ready; interpolation inactive until verified engine source timing";return {};
    }
    Result<void> XessGenerationHost::BindTiming(bool verified)
    {
        if(auto ready=state_->Ready();!ready)return ready;
        auto bound=state_->timing.Bind(state_->presentation.Latency(),verified);
        if(!bound){state_->reason=bound.error().message;return bound;}
        state_->bound=true;return {};
    }
    void XessGenerationHost::RequireOrderedSources(unsigned count)
    { state_->requiredOrdered=count;state_->ordered=0; }
    unsigned XessGenerationHost::OrderedSources()const{return state_->ordered;}
    Result<std::uint32_t> XessGenerationHost::BeforeSourceLoop(std::uint64_t source,std::uint64_t epoch)
    {
        if(auto ready=state_->Ready();!ready)return std::unexpected(ready.error());
        if(!state_->bound)return std::unexpected(Invalid("Intel engine timing is unqualified").error());
        if(source<=state_->loopSource && epoch==state_->loopEpoch)
            return std::unexpected(Invalid("Intel source loop has not advanced").error());
        if(state_->unfinished || (state_->loopEpoch && epoch!=state_->loopEpoch)) {
            if(auto paused=Suspend();!paused)return std::unexpected(paused.error());
            if(auto resumed=Resume();!resumed)return std::unexpected(resumed.error());
        }
        auto begin=state_->timing.BeginSourceLoop(source,epoch);
        if(!begin)state_->reason=begin.error().message;
        else {state_->loopSource=source;state_->loopEpoch=epoch;state_->unfinished=true;}
        return begin;
    }
    Result<void> XessGenerationHost::InputSampled(std::uint64_t source)
    {
        if(auto ready=state_->Ready();!ready)return ready;
        return state_->timing.InputSampled(source);
    }
    Result<void> XessGenerationHost::BeforeRender(std::uint64_t source)
    {
        if(auto ready=state_->Ready();!ready)return ready;
        if(auto end=state_->timing.EndSimulation(source);!end)return end;
        return state_->timing.BeginRender(source);
    }
    HRESULT XessGenerationHost::WaitBeforeProducer()
    {
        if(auto ready=state_->Ready();!ready)return E_UNEXPECTED;
        auto hr=state_->bridge->WaitD3D11(Graphics::InteropWork::FrameGeneration);
        if(SUCCEEDED(hr))hr=state_->bridge->WaitD3D11(Graphics::InteropWork::SwapChain);
        if(FAILED(hr)){state_->fault=hr;state_->reason="Intel copied inputs/native publication did not retire before producer reuse";}
        return hr;
    }
    HRESULT XessGenerationHost::StartupPresent(UINT interval,UINT flags)
    {
        if(auto ready=state_->Ready();!ready)return E_UNEXPECTED;
        return state_->presentation.StartupPresent(interval,flags);
    }
    HRESULT XessGenerationHost::Present(const UpscaleFrame& frame,UpscaleOutcome outcome,ID3D11Texture2D* ui,
        ID3D11ShaderResourceView* overlay,bool complete,bool menu,bool requested,UINT interval,UINT flags)
    {
        if(auto ready=state_->Ready();!ready)return E_UNEXPECTED;
        auto id=state_->bound?state_->timing.CurrentRenderId(frame.sourceId,frame.sourceEpoch):Result<std::uint32_t>{std::unexpected(Invalid("no engine source").error())};
        if(!id) {
            state_->history.Invalidate();
            auto real=state_->presentation.PrepareReal(frame,ui,overlay,complete);
            if(!real){state_->reason=real.error().message;return E_FAIL;}
            const auto hr=state_->presentation.Present(*real,false,interval,flags);
            if(FAILED(hr)){state_->fault=hr;state_->reason="Intel real-only Present failed; retaining owners";}
            return hr;
        }
        // The transport converts readable engine depth into owned R32 guides.
        auto input=frame;input.depthFormat=DXGI_FORMAT_R32_FLOAT;
        const auto measured=AdaptXessGenerationFrame(input,*id);
        const bool compatible=measured && measured->initFlags==state_->initFlags;
        const bool orderedSource=compatible && outcome==UpscaleOutcome::Temporal && complete && !menu;
        const bool qualified=state_->ordered>=state_->requiredOrdered;
        auto admission=state_->history.Decide(input,outcome,requested && qualified,complete,menu);
        input.reset|=admission.reset;
        std::string heldReason;
        if(!compatible && outcome==UpscaleOutcome::Temporal && !menu) {
            heldReason=measured?"Intel guide flags differ from immutable initialization; real output retained":measured.error().message;
            admission.tag=admission.generate=false;state_->history.Invalidate();
        } else if(!qualified) {
            heldReason="Intel interpolation held while genuine ordered engine sources are collected";
        }
        auto prepared=state_->presentation.Prepare(input,ui,overlay,complete,*id,admission.tag);
        if(!prepared){state_->Failure(prepared.error());return E_FAIL;}
        if(auto end=state_->timing.EndRender(frame.sourceId);!end){state_->Failure(end.error());return E_FAIL;}
        if(auto start=state_->timing.BeforePresent(*id);!start){state_->Failure(start.error());return E_FAIL;}
        const auto hr=state_->presentation.Present(*prepared,admission.generate,interval,flags);
        auto end=state_->timing.AfterPresent(*id);
        if(FAILED(hr)){state_->fault=hr;state_->reason="Intel timed Present failed; retaining native/XeLL owners";return hr;}
        if(!end){state_->Failure(end.error());return E_FAIL;}
        state_->unfinished=false;
        if(orderedSource && state_->ordered<state_->requiredOrdered)++state_->ordered;
        if(admission.tag)state_->history.Accept(frame.sourceId,frame.sourceEpoch);
        state_->reason=!heldReason.empty()?heldReason:admission.generate?"Intel FG active on an accepted timed source":"Intel real-only source; history warmup/menu/disabled";
        return hr;
    }
    Result<void> XessGenerationHost::Suspend()
    {
        if(state_->suspended)return {};
        if(auto ready=state_->Ready();!ready)return ready;
        if(auto paused=state_->presentation.Suspend();!paused)return state_->Failure(paused.error());
        if(state_->bound)if(auto abandoned=state_->timing.AbandonAfterDrain(true);!abandoned)return state_->Failure(abandoned.error());
        state_->suspended=true;state_->unfinished=false;state_->history.Invalidate();return {};
    }
    Result<void> XessGenerationHost::Resume()
    {
        if(!state_->created || state_->closing || !state_->suspended || FAILED(state_->fault))return Invalid("Intel resume requires a safely suspended fixed-size owner");
        if(auto resumed=state_->presentation.Resume();!resumed)return state_->Failure(resumed.error());
        state_->suspended=false;return {};
    }
    Result<void> XessGenerationHost::Resize(const DXGI_SWAP_CHAIN_DESC& descriptor)
    {
        if(auto ready=state_->Ready();!ready)return ready;
        if(descriptor.BufferDesc.Width!=state_->descriptor.BufferDesc.Width || descriptor.BufferDesc.Height!=state_->descriptor.BufferDesc.Height ||
            descriptor.BufferDesc.Format!=state_->descriptor.BufferDesc.Format || !descriptor.Windowed ||
            descriptor.OutputWindow!=state_->descriptor.OutputWindow || descriptor.SampleDesc.Count!=1 ||
            descriptor.SampleDesc.Quality!=state_->descriptor.SampleDesc.Quality ||
            descriptor.BufferCount!=state_->descriptor.BufferCount || descriptor.Flags!=state_->descriptor.Flags)
            return Invalid("Intel first trial keeps a fixed SDR output extent; changed extent/format/fullscreen requires restart");
        return {};
    }
    Result<void> XessGenerationHost::Retire()
    {
        if(!state_ || !state_->attempted)return {};
        if(state_->thread!=GetCurrentThreadId())return Invalid("Intel retirement requires the producer thread");
        state_->closing=true;state_->bound=false;
        if(auto retired=state_->presentation.Retire();!retired)return state_->Failure(retired.error());
        auto path=state_->directory;state_=std::make_unique<State>();state_->directory=std::move(path);state_->reason="Intel owner retired";return {};
    }
    IDXGISwapChain4* XessGenerationHost::SwapChain() const{return state_->created?state_->presentation.SwapChain():nullptr;}
    HRESULT XessGenerationHost::GetProducerDevice(REFIID iid,void** result) const
    { if(!result)return E_POINTER;*result=nullptr;return state_->producer?state_->producer->QueryInterface(iid,result):E_UNEXPECTED; }
    xefg_swapchain_present_status_t XessGenerationHost::Status() const{return state_->presentation.Status();}
    const std::string& XessGenerationHost::Reason() const{return state_->reason;}
    bool XessGenerationHost::Suspended() const{return state_->suspended;}
    std::shared_ptr<Graphics::D3D11D3D12Interop> XessGenerationHost::Bridge() const{return state_->bridge;}
}
