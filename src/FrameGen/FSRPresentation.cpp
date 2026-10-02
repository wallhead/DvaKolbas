#include "FSRPresentation.h"
#include <dx12/ffx_api_framegeneration_dx12.h>
#include <mutex>
#include <set>
namespace TheosRenderPipeline
{
    using namespace Upscaling;using namespace Graphics;using Microsoft::WRL::ComPtr;
    namespace
    {
        thread_local unsigned factoryDepth{};
        struct FactoryScope{FactoryScope(){++factoryDepth;}~FactoryScope(){--factoryDepth;}};
        std::mutex windowsMutex;std::set<HWND> ownedWindows;
        bool Reserve(HWND window){std::lock_guard lock(windowsMutex);return ownedWindows.insert(window).second;}
        void Unreserve(HWND window){std::lock_guard lock(windowsMutex);ownedWindows.erase(window);}
        std::unexpected<RuntimeError> Error(ErrorKind kind,std::int64_t code,const char* message)
        {return std::unexpected(RuntimeError{kind,code,message});}
        Result<void> GraphicsResult(HRESULT hr,const char* message)
        {if(FAILED(hr))return Error(ErrorKind::RetirementFailure,hr,message);return {};}
        void Transition(ID3D12GraphicsCommandList* list,ID3D12Resource* depth,ID3D12Resource* motion,bool restore)
        {
            D3D12_RESOURCE_BARRIER barriers[2]{};ID3D12Resource* inputs[]{depth,motion};
            for(int i=0;i<2;++i){auto& b=barriers[i];b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;b.Transition.pResource=inputs[i];
                b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;b.Transition.StateBefore=restore?D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE:D3D12_RESOURCE_STATE_COMMON;
                b.Transition.StateAfter=restore?D3D12_RESOURCE_STATE_COMMON:D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;}
            list->ResourceBarrier(2,barriers);
        }
    }
    struct FsrPresentation::State
    {
        const std::shared_ptr<FsrSdkSession> session=std::make_shared<FsrSdkSession>();
        FsrFrameGeneration generation{session};FsrPresentationTransport transport;FsrGenerationHistory history;
        std::shared_ptr<FsrRuntime> runtime;std::shared_ptr<D3D11D3D12Interop> bridge;ComPtr<IDXGIFactory> factory;ComPtr<IDXGISwapChain4> chain;
        ffxContext swapContext{};DXGI_SWAP_CHAIN_DESC descriptor{};
        ffxCreateContextDescFrameGenerationSwapChainNewDX12 create{};
        ffxCreateContextDescFrameGenerationSwapChainVersionDX12 version{};ffxOverrideVersion override{};
        FsrPresentationStatus status{};HRESULT fault{S_OK};HWND reserved{};
        FsrGenerationLimits limits{};
        bool created{},started{},closing{},retired{},uiRegisteredEver{};
        HRESULT Fail(HRESULT hr){if(SUCCEEDED(fault))fault=hr;status.result=hr;return hr;}
    };
    FsrPresentation::FsrPresentation():state_(std::make_unique<State>()){}
    FsrPresentation::~FsrPresentation(){if(!Retire())(void)state_.release();}
    bool FsrPresentation::InternalFactoryCreation(){return factoryDepth!=0;}
    Result<DXGI_SWAP_CHAIN_DESC> FsrPresentation::TranslateDescriptor(const DXGI_SWAP_CHAIN_DESC& input)
    {
        if(!input.OutputWindow || !IsWindow(input.OutputWindow))return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR OutputWindow must be a valid HWND");
        if(!input.Windowed)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR Windowed must be true (exclusive fullscreen unsupported)");
        if(input.SampleDesc.Count!=1 || input.SampleDesc.Quality)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR SampleDesc requires Count=1, Quality=0");
        if(input.BufferDesc.Format!=DXGI_FORMAT_R8G8B8A8_UNORM)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR BufferDesc.Format requires SDR R8G8B8A8_UNORM");
        constexpr UINT allowedFlags=DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT|DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        if(input.Flags&~allowedFlags)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR Flags supports only waitable latency and allow tearing");
        constexpr UINT allowedUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT|DXGI_USAGE_SHADER_INPUT;
        if(!(input.BufferUsage&DXGI_USAGE_RENDER_TARGET_OUTPUT) || (input.BufferUsage&~allowedUsage))return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR BufferUsage requires render target output; only shader input is additionally supported");
        if(input.BufferDesc.RefreshRate.Numerator && !input.BufferDesc.RefreshRate.Denominator)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR BufferDesc.RefreshRate denominator must be nonzero for an explicit rate");
        if(input.BufferDesc.ScanlineOrdering!=DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED && input.BufferDesc.ScanlineOrdering!=DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR BufferDesc.ScanlineOrdering interlacing is unsupported");
        if(input.BufferDesc.Scaling!=DXGI_MODE_SCALING_UNSPECIFIED && input.BufferDesc.Scaling!=DXGI_MODE_SCALING_CENTERED && input.BufferDesc.Scaling!=DXGI_MODE_SCALING_STRETCHED)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR BufferDesc.Scaling is unknown");
        if(input.SwapEffect!=DXGI_SWAP_EFFECT_DISCARD && input.SwapEffect!=DXGI_SWAP_EFFECT_SEQUENTIAL && input.SwapEffect!=DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL && input.SwapEffect!=DXGI_SWAP_EFFECT_FLIP_DISCARD)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR SwapEffect is unknown");
        auto desc=input;desc.BufferCount=2;desc.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
        if(!desc.BufferDesc.Width || !desc.BufferDesc.Height){RECT client{};if(!GetClientRect(desc.OutputWindow,&client))return Error(ErrorKind::InvalidInput,HRESULT_FROM_WIN32(GetLastError()),"FSR client extent could not be resolved");
            if(!desc.BufferDesc.Width)desc.BufferDesc.Width=client.right-client.left;if(!desc.BufferDesc.Height)desc.BufferDesc.Height=client.bottom-client.top;}
        if(!desc.BufferDesc.Width || !desc.BufferDesc.Height || desc.BufferDesc.Width>D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION || desc.BufferDesc.Height>D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION)return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR BufferDesc extent is empty or exceeds D3D12 limits");
        return desc;
    }
    Result<void> FsrPresentation::Create(IDXGIFactory* factory,std::shared_ptr<FsrRuntime> runtime,
        std::shared_ptr<D3D11D3D12Interop> bridge,const DXGI_SWAP_CHAIN_DESC& desc,const FsrEffectProvider& provider)
    {
        auto lock=state_->session->Lock();if(!lock.Owns(*state_->session) || state_->created || state_->closing || !factory || !runtime || !bridge || !bridge->Ready())return Error(ErrorKind::InvalidInput,E_INVALIDARG,"FSR creation requires a fresh admitted owner and matching graphics bridge");
        auto translated=TranslateDescriptor(desc);if(!translated)return std::unexpected(translated.error());
        if(provider.effect!=FsrEffect::FrameGenerationSwapChain)return Error(ErrorKind::NoProvider,0,"FSR presenter requires a swapchain-tagged provider");
        auto catalog=runtime->EnumerateForEffect(bridge->Device12(),provider.effect);if(!catalog)return std::unexpected(catalog.error());
        auto selected=SelectFsrEffectProvider(*catalog,provider.effect);if(!selected)return std::unexpected(selected.error());
        if(selected->identity.id!=provider.identity.id || selected->identity.name!=provider.identity.name)return Error(ErrorKind::NoProvider,0,"FSR swapchain provider differs from pinned discovered identity");
        if(!Reserve(translated->OutputWindow))return Error(ErrorKind::InvalidInput,DXGI_ERROR_INVALID_CALL,"FSR HWND already has an owned presenter");
        state_->reserved=translated->OutputWindow;state_->created=true;state_->factory=factory;state_->runtime=std::move(runtime);state_->bridge=std::move(bridge);state_->descriptor=*translated;
        state_->create.header={FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_NEW_DX12,&state_->version.header};
        state_->version={{FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_VERSION_DX12,&state_->override.header},FFX_FRAMEGENERATION_SWAPCHAIN_DX12_VERSION};
        state_->override={{FFX_API_DESC_TYPE_OVERRIDE_VERSION,nullptr},provider.identity.id};
        state_->create.swapchain=state_->chain.GetAddressOf();state_->create.desc=&state_->descriptor;state_->create.dxgiFactory=factory;state_->create.gameQueue=state_->bridge->Queue();
        ffxReturnCode_t result;{FactoryScope guard;result=state_->runtime->Functions().CreateContext(&state_->swapContext,&state_->create.header,nullptr);}
        if(result!=FFX_API_RETURN_OK || !state_->swapContext || !state_->chain){state_->Fail(E_FAIL);return Error(ErrorKind::ContextFailure,result,"AMD NewDX12 creation failed; no creation API fallback, retain any partial owner");}
        auto verified=state_->runtime->VerifyActualProvider(state_->swapContext,provider);if(!verified){state_->Fail(E_FAIL);return verified;}
        // The pinned AMD implementation allocates replacement application
        // buffers lazily in GetBuffer, not Present. Startup Present must not
        // reach its UI compositor with an uninitialized replacement buffer.
        for(UINT i=0;i<2;++i){ComPtr<ID3D12Resource> buffer;auto hr=state_->chain->GetBuffer(i,IID_PPV_ARGS(&buffer));
            if(FAILED(hr) || !buffer){state_->Fail(FAILED(hr)?hr:E_FAIL);return Error(ErrorKind::ContextFailure,hr,"AMD application buffer initialization failed before startup Present");}}
        const auto hr=state_->transport.Initialize(state_->bridge,{state_->descriptor.BufferDesc.Width,state_->descriptor.BufferDesc.Height});
        if(FAILED(hr)){state_->Fail(hr);return Error(ErrorKind::ContextFailure,hr,"FSR scene/UI transport initialization failed");}
        return {};
    }
    Result<void> FsrPresentation::CompleteStartup(const FsrGenerationLimits& limits,const FsrEffectProvider& provider)
    {
        auto lock=state_->session->Lock();if(!lock.Owns(*state_->session) || !state_->chain || state_->started || state_->closing || FAILED(state_->fault) ||
            limits.display!=Extent{state_->descriptor.BufferDesc.Width,state_->descriptor.BufferDesc.Height} || limits.format!=state_->descriptor.BufferDesc.Format)return Error(ErrorKind::InvalidInput,0,"FSR deferred startup requires its created fixed-extent presenter");
        auto result=state_->generation.Create(lock,state_->runtime,state_->bridge->Device12(),provider,limits);
        if(!result){state_->Fail(E_FAIL);return result;}state_->limits=limits;state_->started=true;return {};
    }
    HRESULT FsrPresentation::WaitBeforeProducer()
    {auto lock=state_->session->Lock();if(!lock.Owns(*state_->session) || state_->closing || FAILED(state_->fault))return E_UNEXPECTED;return state_->transport.WaitBeforeProducer();}
    HRESULT FsrPresentation::Present(const UpscaleFrame& frame,UpscaleOutcome outcome,const GpuFrameResources& guides,
        ID3D11Texture2D* scene,ColorEncoding encoding,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool complete,bool menu,bool requested,UINT syncInterval,UINT flags)
    {
        auto lock=state_->session->Lock();if(!lock.Owns(*state_->session) || !state_->chain || state_->closing || FAILED(state_->fault))return E_UNEXPECTED;
        if(flags&DXGI_PRESENT_TEST || (!state_->started && !frame.sourceId)){
            if(!state_->session->BeginPresent(lock))return E_UNEXPECTED;
            const auto hr=state_->chain->Present(syncInterval,flags);state_->session->EndPresent(lock);return hr;
        }
        if(!complete || !scene || !ui)return state_->Fail(DXGI_ERROR_INVALID_CALL); // Never silently omit native HUD.
        const auto decision=state_->history.Decide(frame,outcome,complete,menu,requested && state_->started);
        state_->status={frame.sourceId,decision,{},S_OK,false};if(!decision.admit)return DXGI_ERROR_INVALID_CALL;
        auto hr=state_->transport.WaitBeforeProducer();if(FAILED(hr))return state_->Fail(hr);
        hr=state_->transport.Upload(scene,encoding,ui,overlay,complete,frame.sourceId);if(FAILED(hr))return state_->Fail(hr);
        if(state_->started){auto configured=state_->generation.Configure(lock,state_->chain.Get(),frame.sourceId,decision.generate);if(!configured)return state_->Fail(E_FAIL);}
        if(decision.prepare){
            auto resources=state_->transport.Resources();resources.depth=guides.depth;resources.motion=guides.motion;
            // Validate before recording barriers, including foreign/aliased guides.
            ID3D12GraphicsCommandList* list{};hr=state_->bridge->Begin(InteropWork::FrameGeneration,&list);if(FAILED(hr))return state_->Fail(hr);
            auto preparedDesc=BuildFsrGenerationPrepare(list,frame,resources,state_->limits,decision.reset);
            if(!preparedDesc){state_->bridge->DiscardUnsubmitted(InteropWork::FrameGeneration);return state_->Fail(E_INVALIDARG);}
            Transition(list,resources.depth,resources.motion,false);
            auto prepared=state_->generation.Prepare(lock,list,frame,resources,decision.reset);
            Transition(list,resources.depth,resources.motion,true);
            if(!prepared){state_->bridge->DiscardUnsubmitted(InteropWork::FrameGeneration);return state_->Fail(E_FAIL);}
            hr=state_->transport.RecordPrepareRetirement();if(FAILED(hr))return state_->Fail(hr);state_->history.AcknowledgePrepared(frame.sourceId);
        }
        ComPtr<ID3D12Resource> applicationBuffer;hr=state_->chain->GetBuffer(state_->chain->GetCurrentBackBufferIndex(),IID_PPV_ARGS(&applicationBuffer));if(FAILED(hr))return state_->Fail(hr);
        hr=state_->transport.PublishTo(applicationBuffer.Get());if(FAILED(hr))return state_->Fail(hr);
        ffxConfigureDescFrameGenerationSwapChainRegisterUiResourceDX12 registration{};registration.header.type=FFX_API_CONFIGURE_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_REGISTERUIRESOURCE_DX12;
        registration.uiResource=ffxApiGetResourceDX12(state_->transport.Resources().ui,FFX_API_RESOURCE_STATE_COMMON);
        registration.flags=FFX_FRAMEGENERATION_UI_COMPOSITION_FLAG_USE_PREMUL_ALPHA|FFX_FRAMEGENERATION_UI_COMPOSITION_FLAG_ENABLE_INTERNAL_UI_DOUBLE_BUFFERING;
        if(state_->runtime->Functions().Configure(&state_->swapContext,&registration.header)!=FFX_API_RETURN_OK)return state_->Fail(E_FAIL);
        state_->uiRegisteredEver=true;hr=state_->transport.MarkUiRegistered();if(FAILED(hr))return state_->Fail(hr);
        if(!state_->session->BeginPresent(lock))return state_->Fail(E_UNEXPECTED);
        hr=state_->chain->Present(syncInterval,flags);state_->session->EndPresent(lock);
        state_->status.callback=state_->generation.LastCallback(lock);state_->status.result=hr;state_->status.submitted=SUCCEEDED(hr);
        if(FAILED(hr))return state_->Fail(hr);
        if(state_->status.callback.result!=FFX_API_RETURN_OK)return state_->Fail(E_FAIL);
        hr=state_->transport.NotifyPresentReturned(hr);return FAILED(hr)?state_->Fail(hr):hr;
    }
    Result<void> FsrPresentation::Retire()
    {
        auto lock=state_->session->Lock();if(!lock.Owns(*state_->session))return Error(ErrorKind::RetirementFailure,E_UNEXPECTED,"FSR retirement could not acquire its SDK session");
        if(state_->retired)return {};state_->closing=true;
        auto closed=state_->session->StopAdmissions(lock);if(!closed)return closed;
        if(state_->generation.ContextOwned(lock)){auto detached=state_->generation.DisableAndDetach(lock);if(!detached)return detached;}
        if(state_->swapContext){
            ffxConfigureDescFrameGenerationSwapChainRegisterUiResourceDX12 unregister{};unregister.header.type=FFX_API_CONFIGURE_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_REGISTERUIRESOURCE_DX12;
            auto result=state_->runtime->Functions().Configure(&state_->swapContext,&unregister.header);
            if(result!=FFX_API_RETURN_OK)return Error(ErrorKind::RetirementFailure,result,"FSR UI unregister failed; keep all owners");
            ffxDispatchDescFrameGenerationSwapChainWaitForPresentsDX12 wait{};wait.header.type=FFX_API_DISPATCH_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_WAIT_FOR_PRESENTS_DX12;
            result=state_->runtime->Functions().Dispatch(&state_->swapContext,&wait.header);
            if(result!=FFX_API_RETURN_OK)return Error(ErrorKind::RetirementFailure,result,"AMD WaitForPresents failed; keep all owners");
            if(state_->uiRegisteredEver){auto hr=state_->transport.AcknowledgeSdkRetirement(S_OK);if(FAILED(hr))return GraphicsResult(hr,"FSR SDK retirement acknowledgement failed");}
        }
        auto hr=state_->transport.DrainForRetirement();if(FAILED(hr))return GraphicsResult(hr,"FSR Prepare/copy/native-producer retirement failed; retain contexts");
        if(state_->generation.ContextOwned(lock)){auto destroyed=state_->generation.DestroyAfterRetirement(lock);if(!destroyed)return destroyed;}
        if(state_->swapContext){const auto result=state_->runtime->Functions().DestroyContext(&state_->swapContext,nullptr);if(result!=FFX_API_RETURN_OK)return Error(ErrorKind::RetirementFailure,result,"AMD swapchain context destruction failed; keep owner");}
        state_->chain.Reset();hr=state_->transport.Retire();if(FAILED(hr))return GraphicsResult(hr,"FSR final transport retirement failed");
        state_->runtime.reset();state_->bridge.reset();state_->factory.Reset();state_->started=false;state_->retired=true;
        if(state_->reserved){Unreserve(state_->reserved);state_->reserved=nullptr;}return {};
    }
    Result<void> FsrPresentation::BeforeResize(){return Retire();}
    Result<void> FsrPresentation::AfterResize(HRESULT result){return GraphicsResult(result,"FSR replacement presenter creation/resize failed; retain its partial owner");}
    IDXGISwapChain4* FsrPresentation::SwapChain()const{return state_->chain.Get();}
    ID3D11Texture2D* FsrPresentation::SceneTarget11()const{return state_->transport.SceneTarget11();}
    std::shared_ptr<FsrSdkSession> FsrPresentation::Session()const{return state_->session;}
    FsrPresentationStatus FsrPresentation::Status()const{auto lock=state_->session->Lock();if(!lock.Owns(*state_->session))return {};return state_->status;}
}
