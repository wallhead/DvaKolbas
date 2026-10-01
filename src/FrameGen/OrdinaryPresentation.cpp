#include "OrdinaryPresentation.h"
#include <limits>
#include <algorithm>
namespace TheosRenderPipeline
{
    using Microsoft::WRL::ComPtr;
    struct OrdinaryPresentation::State
    {
        ComPtr<ID3D11Device5> device;ComPtr<ID3D11DeviceContext4> context;ComPtr<ID3D11Fence> fence;
        HANDLE event{};std::uint64_t value{};HRESULT fault{S_OK};bool ready{};
        Graphics::RetirementWaitPolicy wait;
        ~State(){if(event)CloseHandle(event);}
    };
    OrdinaryPresentation::OrdinaryPresentation():state_(std::make_unique<State>()){}
    OrdinaryPresentation::~OrdinaryPresentation(){if(FAILED(Retire()))(void)state_.release();}
    bool OrdinaryPresentation::Ready()const{return state_->ready && SUCCEEDED(state_->fault);}
    void OrdinaryPresentation::SetRetirementWaitPolicy(Graphics::RetirementWaitPolicy wait){state_->wait=wait;}
    HRESULT OrdinaryPresentation::CreateSwapChain(IDXGIFactory* factory,ID3D11Device* device,const DXGI_SWAP_CHAIN_DESC& desc,
        IDXGISwapChain** output,OriginalCreateSwapChain original)
    {
        if(!output)return E_POINTER;*output=nullptr;
        if(!factory || !device || !original || state_->device || !desc.Windowed || desc.SampleDesc.Count!=1)return E_INVALIDARG;
        auto hr=device->QueryInterface(IID_PPV_ARGS(&state_->device));if(FAILED(hr))return hr;
        ComPtr<ID3D11DeviceContext> context;device->GetImmediateContext(&context);hr=context.As(&state_->context);if(FAILED(hr))return hr;
        hr=state_->device->CreateFence(0,D3D11_FENCE_FLAG_NONE,IID_PPV_ARGS(&state_->fence));if(FAILED(hr))return hr;
        state_->event=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!state_->event)return HRESULT_FROM_WIN32(GetLastError());
        auto copy=desc;
        copy.BufferCount=2;copy.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;
        // This is the exact pre-hook function, supplied by the factory detour.
        // Calling the virtual factory entry here would recurse into the hook.
        hr=(factory->*original)(device,&copy,output);
        state_->ready=SUCCEEDED(hr) && *output;
        if(!state_->ready)return FAILED(hr)?hr:E_FAIL;
        return hr;
    }
    HRESULT OrdinaryPresentation::Retire()
    {
        if(!state_->context || !state_->fence)return S_OK;
        auto hr=state_->device->GetDeviceRemovedReason();if(FAILED(hr))return state_->fault=hr;
        hr=state_->context->Signal(state_->fence.Get(),++state_->value);state_->context->Flush();if(FAILED(hr))return state_->fault=hr;
        if(FAILED(hr=state_->fence->SetEventOnCompletion(state_->value,state_->event)))return state_->fault=hr;
        auto progress=state_->fence->GetCompletedValue();DWORD stalled{};
        for(;;){
            const auto completed=state_->fence->GetCompletedValue();
            if(completed==std::numeric_limits<std::uint64_t>::max())return state_->fault=DXGI_ERROR_DEVICE_REMOVED;
            if(completed>=state_->value)return S_OK;
            const auto result=WaitForSingleObject(state_->event,std::max(1ul,state_->wait.sliceMs));
            if(result!=WAIT_OBJECT_0 && result!=WAIT_TIMEOUT)return state_->fault=HRESULT_FROM_WIN32(GetLastError());
            if(FAILED(hr=state_->device->GetDeviceRemovedReason()))return state_->fault=hr;
            const auto next=state_->fence->GetCompletedValue();
            if(next>progress){progress=next;stalled=0;}else if(result==WAIT_TIMEOUT && (stalled+=std::max(1ul,state_->wait.sliceMs))>=state_->wait.stallLimitMs)
                return state_->fault=HRESULT_FROM_WIN32(WAIT_TIMEOUT);
        }
    }
    HRESULT OrdinaryPresentation::BeforeResize(){auto hr=Retire();if(SUCCEEDED(hr))state_->ready=false;return hr;}
    HRESULT OrdinaryPresentation::AfterResize(HRESULT result){if(FAILED(result))state_->fault=result;else if(SUCCEEDED(state_->fault))state_->ready=true;return FAILED(state_->fault)?state_->fault:result;}
    void OrdinaryPresentation::ResetAfterRetirement(){if(SUCCEEDED(Retire()))state_=std::make_unique<State>();}
}
