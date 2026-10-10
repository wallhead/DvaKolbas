#include "XessGenerationPresentation.h"
#include <array>
namespace TheosRenderPipeline
{
    using namespace Upscaling;
    using Microsoft::WRL::ComPtr;
    namespace
    {
        thread_local bool internalCreation{};
        struct FactoryScope
        {
            bool previous{internalCreation};
            FactoryScope() { internalCreation=true; }
            ~FactoryScope() { internalCreation=previous; }
        };
        Result<void> Invalid(const char* message)
        { return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,message}); }
    }
    struct XessGenerationPresentation::State
    {
        std::shared_ptr<XessGenerationRuntime> runtime;
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;
        XellSession latency;
        XessGenerationTransport transport;
        xefg_swapchain_handle_t context{};
        ComPtr<IDXGISwapChain4> proxy;
        ComPtr<ID3D12DescriptorHeap> startupRtvs;
        std::array<ComPtr<ID3D12Resource>,2> startupBuffers;
        Extent display{};
        std::uint32_t flags{};
        XessGenerationFrame prepared{};
        xefg_swapchain_present_status_t status{};
        bool initialized{},ready{},closing{},suspended{},enabled{},hasPrepared{},tagged{};
        HRESULT fault{S_OK};
        Result<void> Check(xefg_swapchain_result_t result,const char* message)
        {
            if (static_cast<int>(result)>=0) return {};
            fault=result==XEFG_SWAPCHAIN_RESULT_ERROR_DEVICE?DXGI_ERROR_DEVICE_REMOVED:E_FAIL;
            return std::unexpected(RuntimeError{result==XEFG_SWAPCHAIN_RESULT_ERROR_DEVICE?ErrorKind::DeviceLost:ErrorKind::ContextFailure,result,message});
        }
        Result<void> Enabled(bool value)
        {
            if (enabled==value) return {};
            if (auto result=Check(runtime->Generation().SetEnabled(context,value),"Intel FG SetEnabled failed"); !result) return result;
            enabled=value;return {};
        }
        Result<void> Ready() const
        {
            if (!ready || closing || suspended || !bridge || !bridge->Ready() || FAILED(fault))
                return std::unexpected(RuntimeError{ErrorKind::ContextFailure,FAILED(fault)?fault:E_UNEXPECTED,"Intel FG owner is unavailable/suspended; generation inactive"});
            return {};
        }
    };
    XessGenerationPresentation::XessGenerationPresentation()=default;
    XessGenerationPresentation::~XessGenerationPresentation()
    { if (state_ && !Retire()) (void)state_.release(); }
    Result<void> XessGenerationPresentation::Create(IDXGIFactory* factory,std::shared_ptr<XessGenerationRuntime> runtime,
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge,const DXGI_SWAP_CHAIN_DESC& desc,std::uint32_t initFlags)
    {
        constexpr auto supportedFlags=XEFG_SWAPCHAIN_INIT_FLAG_INVERTED_DEPTH|XEFG_SWAPCHAIN_INIT_FLAG_HIGH_RES_MV|
            XEFG_SWAPCHAIN_INIT_FLAG_USE_NDC_VELOCITY|XEFG_SWAPCHAIN_INIT_FLAG_JITTERED_MV;
        if (state_ || !factory || !runtime || !bridge || !bridge->Ready() || !desc.Windowed || !IsWindow(desc.OutputWindow) ||
            !desc.BufferDesc.Width || !desc.BufferDesc.Height || desc.BufferDesc.Format!=DXGI_FORMAT_R8G8B8A8_UNORM ||
            desc.SampleDesc.Count!=1 || desc.SampleDesc.Quality || (initFlags & ~supportedFlags) ||
            (desc.Flags & ~(DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING|DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)))
            return Invalid("Intel FG first trial requires fixed borderless SDR R8G8B8A8, one sample and declared guide flags");
        ComPtr<IDXGIFactory2> nativeFactory;
        if (FAILED(factory->QueryInterface(IID_PPV_ARGS(&nativeFactory)))) return Invalid("Intel FG requires IDXGIFactory2");
        state_=std::make_unique<State>();auto& state=*state_;
        state.runtime=std::move(runtime);state.bridge=std::move(bridge);state.flags=initFlags;
        state.display={desc.BufferDesc.Width,desc.BufferDesc.Height};
        if (auto result=state.latency.Create(state.bridge->Device12(),state.runtime); !result) return result;
        const auto& api=state.runtime->Generation();
        if (auto result=state.Check(api.CreateContext(state.bridge->Device12(),&state.context),"Intel FG CreateContext failed"); !result) return result;
        if (!state.context) return std::unexpected(RuntimeError{ErrorKind::ContextFailure,0,"Intel FG CreateContext returned no context"});
        if (auto result=state.Check(api.SetLatencyReduction(state.context,state.latency.Context()),"Intel FG latency link failed"); !result) return result;
        xefg_swapchain_properties_t properties{};
        if (auto result=state.Check(api.GetProperties(state.context,&properties),"Intel FG properties query failed"); !result) return result;
        if (properties.maxSupportedInterpolations<1) return std::unexpected(RuntimeError{ErrorKind::UnsupportedDevice,0,"Intel FG reports no supported interpolated frames"});
        xefg_swapchain_d3d12_init_params_t init{};
        init.initFlags=initFlags;init.maxInterpolatedFrames=1;init.uiMode=XEFG_SWAPCHAIN_UI_MODE_HUDLESS_UITEXTURE;
        if (auto result=state.Check(api.GetD3D12Properties(state.context,&init,state.display.width,state.display.height,desc.BufferDesc.Format,&properties),"Intel FG native resource properties failed"); !result) return result;
        DXGI_SWAP_CHAIN_DESC1 native{};
        native.Width=state.display.width;native.Height=state.display.height;native.Format=desc.BufferDesc.Format;
        native.SampleDesc.Count=1;native.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;native.BufferCount=2;
        native.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;native.Scaling=DXGI_SCALING_STRETCH;
        native.AlphaMode=DXGI_ALPHA_MODE_IGNORE;native.Flags=desc.Flags;
        {
            FactoryScope guard;
            if (auto result=state.Check(api.InitFromSwapChainDesc(state.context,desc.OutputWindow,&native,nullptr,state.bridge->Queue(),nativeFactory.Get(),&init),"Intel FG proxy initialization failed"); !result) return result;
        }
        state.initialized=true;
        if (auto result=state.Check(api.GetSwapChainPtr(state.context,IID_PPV_ARGS(&state.proxy)),"Intel FG proxy query failed"); !result) return result;
        if (!state.proxy) return std::unexpected(RuntimeError{ErrorKind::ContextFailure,0,"Intel FG returned no proxy swap chain"});
        if (auto result=state.Check(api.SetUiCompositionState(state.context,XEFG_SWAPCHAIN_UI_COMPOSITION_STATE_ENABLED),"Intel FG HUD composition activation failed"); !result) return result;
        if (auto result=state.Check(api.SetEnabled(state.context,0),"Intel FG initial disable failed"); !result) return result;
        if (auto result=state.transport.Initialize(state.bridge,state.display); !result) return result;
        state.ready=true;return {};
    }
    Result<XessGenerationFrame> XessGenerationPresentation::Prepare(const UpscaleFrame& frame,ID3D11Texture2D* ui,
        ID3D11ShaderResourceView* overlay,bool complete,std::uint32_t sdkId,bool tag)
    {
        if (!state_) return std::unexpected(Invalid("Intel FG owner is not created").error());
        auto& state=*state_;
        if (auto ready=state.Ready(); !ready) return std::unexpected(ready.error());
        if (state.hasPrepared || frame.display!=state.display || !complete)
            return std::unexpected(Invalid("Intel FG needs one completed HUD source at the fixed display extent; changed extent requires restart").error());
        auto input=frame;
        XessGenerationFrame prepared{};
        if (tag) {
            // Transport verifies the source depth and owns its packed-to-R32
            // conversion. SDK parameters describe that prepared resource.
            input.depthFormat=DXGI_FORMAT_R32_FLOAT;
            const auto adapted=AdaptXessGenerationFrame(input,sdkId);
            if (!adapted) return std::unexpected(adapted.error());
            if (adapted->initFlags!=state.flags)
                return std::unexpected(Invalid("Intel FG guide conventions differ from immutable initialization; restart with measured flags").error());
            prepared=*adapted;
        } else {
            input.depth=input.motion=nullptr;
            prepared.sourceId=frame.sourceId;prepared.sourceEpoch=frame.sourceEpoch;prepared.sdkId=sdkId;
            prepared.display=frame.display;prepared.render=frame.render;prepared.constants.resetHistory=1;
        }
        if (auto result=state.transport.WaitBeforeProducer(); !result) return std::unexpected(result.error());
        const bool untimed=!tag && !frame.sourceId && !frame.sourceEpoch && !sdkId;
        const auto uploaded=untimed?state.transport.UploadReal(input,ui,overlay,complete):state.transport.Upload(input,ui,overlay,complete);
        if (!uploaded) return std::unexpected(uploaded.error());
        if (auto result=state.Enabled(tag); !result) return std::unexpected(result.error());
        if (tag) {
            const auto list=state.transport.BeginTag();if (!list) return std::unexpected(list.error());
            if (auto result=state.transport.Tag(*list,state.context,state.runtime->Generation(),prepared); !result) return std::unexpected(result.error());
        }
        ComPtr<ID3D12Resource> buffer;
        const auto hr=state.proxy->GetBuffer(state.proxy->GetCurrentBackBufferIndex(),IID_PPV_ARGS(&buffer));
        if (FAILED(hr)) return std::unexpected(RuntimeError{ErrorKind::ContextFailure,hr,"Intel FG real backbuffer acquisition failed"});
        if (auto result=state.transport.PublishTo(buffer.Get()); !result) return std::unexpected(result.error());
        state.prepared=prepared;state.hasPrepared=true;state.tagged=tag;return prepared;
    }
    Result<XessGenerationFrame> XessGenerationPresentation::PrepareReal(const UpscaleFrame& frame,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool complete)
    {
        auto real=frame;real.sourceId=real.sourceEpoch=0;real.depth=real.motion=nullptr;
        return Prepare(real,ui,overlay,complete,0,false);
    }
    HRESULT XessGenerationPresentation::Present(const XessGenerationFrame& frame,bool generate,UINT interval,UINT flags)
    {
        if (!state_ || !state_->Ready() || !state_->hasPrepared || frame.sdkId!=state_->prepared.sdkId ||
            frame.sourceId!=state_->prepared.sourceId || frame.sourceEpoch!=state_->prepared.sourceEpoch) return E_UNEXPECTED;
        auto& state=*state_;
        const bool untimed=!state.tagged && !frame.sourceId && !frame.sourceEpoch && !frame.sdkId;
        if (untimed && generate) return E_INVALIDARG;
        // Reset consumes a real frame but keeps a valid tagged history warm.
        if (!state.Enabled(state.tagged && (generate || state.prepared.constants.resetHistory!=0))) return state.fault;
        if (!untimed && !state.Check(state.runtime->Generation().SetPresentId(state.context,frame.sdkId),"Intel FG Present ID failed")) return state.fault;
        const auto hr=state.proxy->Present(interval,flags);
        state.hasPrepared=false;
        const auto status=state.runtime->Generation().GetLastPresentStatus(state.context,&state.status);
        if (FAILED(hr)) { state.fault=hr;return hr; }
        if (!state.Check(status,"Intel FG LastPresentStatus query failed")) return state.fault;
        return hr;
    }
    HRESULT XessGenerationPresentation::StartupPresent(UINT interval,UINT flags)
    {
        if (!state_ || !state_->Ready() || state_->hasPrepared) return E_UNEXPECTED;
        auto& state=*state_;
        if (!state.Enabled(false)) return state.fault;
        if((flags&DXGI_PRESENT_TEST)!=0)return state.proxy->Present(interval,flags);
        const auto index=state.proxy->GetCurrentBackBufferIndex();
        if(index>=state.startupBuffers.size())return E_UNEXPECTED;
        HRESULT hr=S_OK;
        if(!state.startupRtvs) {
            D3D12_DESCRIPTOR_HEAP_DESC desc{};desc.Type=D3D12_DESCRIPTOR_HEAP_TYPE_RTV;desc.NumDescriptors=2;
            if(FAILED(hr=state.bridge->Device12()->CreateDescriptorHeap(&desc,IID_PPV_ARGS(&state.startupRtvs))))return hr;
        }
        auto& buffer=state.startupBuffers[index];
        auto rtv=state.startupRtvs->GetCPUDescriptorHandleForHeapStart();
        rtv.ptr+=index*state.bridge->Device12()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        if(!buffer) {
            if(FAILED(hr=state.proxy->GetBuffer(index,IID_PPV_ARGS(&buffer))))return hr;
            state.bridge->Device12()->CreateRenderTargetView(buffer.Get(),nullptr,rtv);
        }
        ID3D12GraphicsCommandList* list{};
        if (FAILED(hr=state.bridge->Begin(Graphics::InteropWork::SwapChain,&list))) return hr;
        D3D12_RESOURCE_BARRIER barrier{};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource=buffer.Get();barrier.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore=D3D12_RESOURCE_STATE_PRESENT;barrier.Transition.StateAfter=D3D12_RESOURCE_STATE_RENDER_TARGET;
        list->ResourceBarrier(1,&barrier);const float clear[4]{0,0,0,1};list->ClearRenderTargetView(rtv,clear,0,nullptr);
        std::swap(barrier.Transition.StateBefore,barrier.Transition.StateAfter);list->ResourceBarrier(1,&barrier);
        hr=state.bridge->Submit(Graphics::InteropWork::SwapChain);
        // Cached buffers/RTVs survive until ordered retirement. Begin waits
        // only before reusing its command slot; no per-Present full GPU drain.
        if (FAILED(hr)) {
            state.fault=hr;return hr;
        }
        hr=state.proxy->Present(interval,flags);
        const auto status=state.runtime->Generation().GetLastPresentStatus(state.context,&state.status);
        if (FAILED(hr)) { state.fault=hr;return hr; }
        if (!state.Check(status,"Intel FG startup status query failed")) return state.fault;
        return hr;
    }
    Result<void> XessGenerationPresentation::Suspend()
    {
        if (!state_ || !state_->ready || state_->closing) return Invalid("Intel FG owner cannot suspend");
        if (auto result=state_->Enabled(false); !result) return result;
        state_->suspended=true;state_->hasPrepared=false;
        const auto hr=state_->bridge->Drain();
        if (FAILED(hr)) { state_->fault=hr;return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,hr,"Intel FG suspension could not drain tagged copies"}); }
        return {};
    }
    Result<void> XessGenerationPresentation::Resume()
    {
        if (!state_ || !state_->ready || state_->closing || !state_->suspended || FAILED(state_->fault) || !state_->bridge->Ready())
            return Invalid("Intel FG fixed-size owner cannot resume");
        state_->suspended=false;return {};
    }
    Result<void> XessGenerationPresentation::Retire()
    {
        if (!state_) return {};
        auto& state=*state_;state.closing=true;state.ready=false;
        const auto& api=state.runtime->Generation();
        if (state.context && state.initialized)
            if (auto result=state.Check(api.SetEnabled(state.context,0),"Intel FG teardown disable failed; retaining owners"); !result) return result;
        const auto drain=state.bridge->Drain();
        if (FAILED(drain)) return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,drain,"Intel FG tagged work did not retire; retain all native owners"});
        state.startupBuffers={};state.startupRtvs.Reset();
        state.proxy.Reset();
        if (state.context) {
            if (auto result=state.Check(api.Destroy(state.context),"Intel FG Destroy failed; retaining XeLL/context/runtime for proven retry"); !result) return result;
            state.context=nullptr;
        }
        if (auto result=state.transport.Retire(); !result) return result;
        if (auto result=state.latency.Retire(true,true); !result) return result;
        state_.reset();return {};
    }
    IDXGISwapChain4* XessGenerationPresentation::SwapChain() const { return state_ && state_->ready?state_->proxy.Get():nullptr; }
    xefg_swapchain_present_status_t XessGenerationPresentation::Status() const { return state_?state_->status:xefg_swapchain_present_status_t{}; }
    XellSession* XessGenerationPresentation::Latency() const { return state_?&state_->latency:nullptr; }
    bool XessGenerationPresentation::InternalFactoryCreation() { return internalCreation; }
}
