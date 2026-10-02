#include "FSRPresentationTransport.h"
#include "Upscaling/FSRColorConversion.h"
#include "Upscaling/FSRPresentationColor.h"
namespace TheosRenderPipeline
{
    using namespace Upscaling;using namespace Graphics;using Microsoft::WRL::ComPtr;
    struct FsrPresentationTransport::State
    {
        std::shared_ptr<D3D11D3D12Interop> bridge;Extent extent{};
        ComPtr<ID3D11Texture2D> sceneTarget;SharedTexture scene,ui;ComPtr<ID3D12Resource> publicationUi;
        FsrColorConverter sceneConverter;FsrPresentationUiConverter uiConverter;FsrGenerationResources resources{};
        std::uint64_t uploaded{},published{};
        bool waited{},pendingPresent{},registered{},everRegistered{},sdkRetired{},closing{};
        HRESULT fault{S_OK};
        HRESULT Check(HRESULT hr){if(FAILED(hr) && SUCCEEDED(fault))fault=hr;return hr;}
        bool Ready()const{return bridge && bridge->Ready() && !closing && SUCCEEDED(fault);}
    };
    FsrPresentationTransport::FsrPresentationTransport()=default;
    FsrPresentationTransport::~FsrPresentationTransport()
    {
        // Failed retirement is not permission to destroy borrowed UI, shared
        // sources or converter-held views. Retain their complete owner.
        if(state_ && FAILED(Retire()))(void)state_.release();
    }
    HRESULT FsrPresentationTransport::Initialize(std::shared_ptr<D3D11D3D12Interop> bridge,Extent extent)
    {
        if(state_ || !bridge || !bridge->Ready() || !extent.width || !extent.height ||
            extent.width>D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION || extent.height>D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION)return E_INVALIDARG;
        auto state=std::make_unique<State>();state->bridge=std::move(bridge);state->extent=extent;
        D3D11_TEXTURE2D_DESC d{};d.Width=extent.width;d.Height=extent.height;d.MipLevels=d.ArraySize=1;d.SampleDesc.Count=1;
        d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
        ComPtr<ID3D11Device> producer;state->bridge->Context11()->GetDevice(&producer);
        auto hr=producer->CreateTexture2D(&d,nullptr,&state->sceneTarget);if(FAILED(hr))return hr;
        if(FAILED(hr=state->bridge->CreateSharedTexture(d,state->scene)))return hr;
        if(FAILED(hr=state->bridge->CreateSharedTexture(d,state->ui)))return hr;
        auto desc=state->ui.texture12->GetDesc();desc.Flags=D3D12_RESOURCE_FLAG_NONE;
        D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_DEFAULT;
        hr=state->bridge->Device12()->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&state->publicationUi));
        if(FAILED(hr))return hr;
        state->resources={state->scene.texture12.Get(),nullptr,nullptr,state->publicationUi.Get(),ColorEncoding::SRGB};state_=std::move(state);return S_OK;
    }
    HRESULT FsrPresentationTransport::WaitBeforeProducer()
    {
        if(!state_ || !state_->Ready())return E_UNEXPECTED;
        // These waits precede writes, not the producer signal that follows them.
        auto hr=state_->bridge->WaitD3D11(InteropWork::FrameGeneration);if(FAILED(hr))return state_->Check(hr);
        hr=state_->bridge->WaitD3D11(InteropWork::SwapChain);if(SUCCEEDED(hr))state_->waited=true;
        return state_->Check(hr);
    }
    HRESULT FsrPresentationTransport::Upload(ID3D11Texture2D* scene,ColorEncoding encoding,ID3D11Texture2D* ui,
        ID3D11ShaderResourceView* overlay,bool complete,std::uint64_t sourceId)
    {
        if(!state_ || !state_->Ready() || !state_->waited)return E_UNEXPECTED;
        if(!complete || !sourceId || sourceId<=state_->uploaded || !IsKnownColorEncoding(encoding) || !scene || !ui)return E_INVALIDARG;
        D3D11_TEXTURE2D_DESC desc{};scene->GetDesc(&desc);
        if(desc.Width!=state_->extent.width || desc.Height!=state_->extent.height)return E_INVALIDARG;
        auto* context=state_->bridge->Context11();
        auto hr=state_->sceneConverter.Convert(context,scene,state_->scene.texture11.Get(),encoding,ColorEncoding::SRGB);
        if(SUCCEEDED(hr))hr=state_->uiConverter.Convert(context,ui,overlay,state_->ui.texture11.Get(),encoding);
        // Even a failed second conversion may follow a queued scene draw.
        // Seal that producer before reporting failure and retaining its owners.
        const auto sealed=state_->bridge->SignalD3D11(InteropWork::SwapChain);
        if(FAILED(sealed))return state_->Check(sealed);
        if(FAILED(hr))return state_->Check(hr);
        state_->uploaded=sourceId;state_->waited=false;return S_OK;
    }
    HRESULT FsrPresentationTransport::PublishTo(ID3D12Resource* backbuffer)
    {
        if(!state_ || !state_->Ready() || !state_->uploaded || state_->uploaded<=state_->published || state_->pendingPresent)return E_UNEXPECTED;
        if(!backbuffer)return E_INVALIDARG;
        ComPtr<ID3D12Device> device;auto hr=backbuffer->GetDevice(IID_PPV_ARGS(&device));if(FAILED(hr))return hr;
        if(!D3D11FrameCopy::SameObject(device.Get(),state_->bridge->Device12()))return E_INVALIDARG;
        const auto d=backbuffer->GetDesc();const auto expected=state_->scene.texture12->GetDesc();
        if(d.Dimension!=expected.Dimension || d.Width!=expected.Width || d.Height!=expected.Height || d.Format!=expected.Format ||
            d.DepthOrArraySize!=1 || d.MipLevels!=1 || d.SampleDesc.Count!=1 || d.SampleDesc.Quality)return E_INVALIDARG;
        ID3D12GraphicsCommandList* list{};hr=state_->bridge->Begin(InteropWork::SwapChain,&list);if(FAILED(hr))return state_->Check(hr);
        hr=D3D11D3D12Interop::RecordCopy(list,state_->scene.texture12.Get(),backbuffer);
        if(SUCCEEDED(hr))hr=D3D11D3D12Interop::RecordCopy(list,state_->ui.texture12.Get(),state_->publicationUi.Get());
        // Descriptor validation above makes RecordCopy deterministic. On an
        // unexpected error retain the recording owner; never fake submission.
        if(FAILED(hr))return state_->Check(hr);
        hr=state_->bridge->Submit(InteropWork::SwapChain);if(FAILED(hr))return state_->Check(hr);
        state_->published=state_->uploaded;state_->pendingPresent=true;state_->registered=false;return S_OK;
    }
    HRESULT FsrPresentationTransport::MarkUiRegistered()
    {
        if(!state_ || !state_->Ready() || !state_->pendingPresent || state_->registered)return E_UNEXPECTED;
        state_->registered=state_->everRegistered=true;state_->sdkRetired=false;return S_OK;
    }
    HRESULT FsrPresentationTransport::NotifyPresentReturned(HRESULT hr)
    {
        if(!state_ || !state_->Ready() || !state_->pendingPresent || !state_->registered)return E_UNEXPECTED;
        if(FAILED(hr))return state_->Check(hr);
        state_->pendingPresent=false;state_->registered=false;return S_OK;
    }
    HRESULT FsrPresentationTransport::RecordPrepareRetirement()
    {
        if(!state_ || !state_->Ready())return E_UNEXPECTED;
        return state_->Check(state_->bridge->Submit(InteropWork::FrameGeneration));
    }
    HRESULT FsrPresentationTransport::AcknowledgeSdkRetirement(HRESULT hr)
    {
        if(!state_ || !state_->everRegistered)return E_UNEXPECTED;
        state_->closing=true;if(FAILED(hr))return hr;
        state_->sdkRetired=true;state_->pendingPresent=false;state_->registered=false;return S_OK;
    }
    HRESULT FsrPresentationTransport::DrainForRetirement()
    {
        if(!state_)return S_OK;
        state_->closing=true;
        if(state_->everRegistered && !state_->sdkRetired)return E_UNEXPECTED;
        if(!state_->bridge->Ready())return FAILED(state_->bridge->Fault())?state_->bridge->Fault():E_UNEXPECTED;
        // The game may have queued native scene work before reaching Upload.
        // Its final D3D11 reader/writer is part of this transport's lifetime.
        auto hr=state_->bridge->SignalD3D11(InteropWork::SwapChain);if(FAILED(hr))return hr;
        hr=state_->bridge->Drain();if(FAILED(hr))return hr;
        return S_OK;
    }
    HRESULT FsrPresentationTransport::Retire()
    {
        auto hr=DrainForRetirement();if(FAILED(hr))return hr;
        state_.reset();return S_OK;
    }
    ID3D11Texture2D* FsrPresentationTransport::SceneTarget11()const{return state_?state_->sceneTarget.Get():nullptr;}
    const FsrGenerationResources& FsrPresentationTransport::Resources()const
    {static const FsrGenerationResources empty{};return state_?state_->resources:empty;}
}
