#pragma once
#include "Graphics/D3D11D3D12Interop.h"
#include <dxgi1_4.h>
#include <d3d11sdklayers.h>
#include <d3d12sdklayers.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
namespace InteropFixture
{
    using Microsoft::WRL::ComPtr;
    inline void Require(bool value,const char* why) { if(!value){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);} }
    inline void Check(HRESULT hr,const char* why) { if(FAILED(hr)){std::fprintf(stderr,"FAIL: %s HRESULT=%08lx\n",why,hr);std::exit(1);} }

    using Interop=TheosRenderPipeline::Graphics::D3D11D3D12Interop;
    using SharedTexture=TheosRenderPipeline::Graphics::SharedTexture;
    using Work=TheosRenderPipeline::Graphics::InteropWork;
    using WaitPolicy=TheosRenderPipeline::Graphics::RetirementWaitPolicy;

    struct Rig
    {
        ComPtr<IDXGIFactory4> factory;ComPtr<IDXGIAdapter> adapter;
        ComPtr<ID3D11Device> device11;ComPtr<ID3D11DeviceContext> context11;
        ComPtr<ID3D11Device5> device5;ComPtr<ID3D11DeviceContext4> context4;
        ComPtr<ID3D12Device> device12;ComPtr<ID3D12CommandQueue> queue;
        ComPtr<ID3D11InfoQueue> messages11;ComPtr<ID3D12InfoQueue> messages12;
        explicit Rig(bool skipUnsupported=false)
        {
            ComPtr<ID3D12Debug> debug;
            if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))debug->EnableDebugLayer();
            else std::puts("SKIPPED: D3D12 debug layer unavailable");
            Check(CreateDXGIFactory1(IID_PPV_ARGS(&factory)),"DXGI factory");
            auto hr=D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,D3D11_CREATE_DEVICE_DEBUG,nullptr,0,D3D11_SDK_VERSION,&device11,nullptr,&context11);
            if(hr==DXGI_ERROR_SDK_COMPONENT_MISSING){std::puts("SKIPPED: D3D11 debug layer unavailable");hr=D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device11,nullptr,&context11);}
            if(FAILED(hr) && skipUnsupported){std::puts("SKIPPED: hardware D3D11 unavailable");std::exit(77);}
            Check(hr,"actual hardware D3D11");
            ComPtr<IDXGIDevice> dxgi;Check(device11.As(&dxgi),"DXGI device");Check(dxgi->GetAdapter(&adapter),"actual adapter");
            hr=D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device12));
            if(FAILED(hr) && skipUnsupported){std::puts("SKIPPED: matching hardware D3D12 unavailable");std::exit(77);}
            Check(hr,"same adapter D3D12");
            D3D12_COMMAND_QUEUE_DESC desc{};desc.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
            Check(device12->CreateCommandQueue(&desc,IID_PPV_ARGS(&queue)),"SR queue");
            Check(device11.As(&device5),"shared-fence device");Check(context11.As(&context4),"shared-fence context");
            device11.As(&messages11);device12.As(&messages12);
        }
        void ValidateDebug()
        {
            if(messages11)for(UINT64 i=0;i<messages11->GetNumStoredMessagesAllowedByRetrievalFilter();++i){
                SIZE_T size{};messages11->GetMessage(i,nullptr,&size);std::vector<unsigned char> bytes(size);auto* message=reinterpret_cast<D3D11_MESSAGE*>(bytes.data());Check(messages11->GetMessage(i,message,&size),"read D3D11 diagnostics");
                if(message->Severity==D3D11_MESSAGE_SEVERITY_ERROR || message->Severity==D3D11_MESSAGE_SEVERITY_CORRUPTION){std::fprintf(stderr,"D3D11: %s\n",message->pDescription);Require(false,"D3D11 debug error");}
            }
            if(messages12)for(UINT64 i=0;i<messages12->GetNumStoredMessagesAllowedByRetrievalFilter();++i){
                SIZE_T size{};messages12->GetMessage(i,nullptr,&size);std::vector<unsigned char> bytes(size);auto* message=reinterpret_cast<D3D12_MESSAGE*>(bytes.data());Check(messages12->GetMessage(i,message,&size),"read D3D12 diagnostics");
                if(message->Severity==D3D12_MESSAGE_SEVERITY_ERROR || message->Severity==D3D12_MESSAGE_SEVERITY_CORRUPTION){std::fprintf(stderr,"D3D12: %s\n",message->pDescription);Require(false,"D3D12 debug error");}
            }
        }
        D3D11_TEXTURE2D_DESC Description()const
        { D3D11_TEXTURE2D_DESC d{};d.Width=3;d.Height=2;d.MipLevels=1;d.ArraySize=1;d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.SampleDesc.Count=1;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;return d; }
        void Initialize(Interop& i){Check(i.Initialize(device11.Get(),device12.Get(),queue.Get()),"initialize interop");}
        void SharedGate(ComPtr<ID3D12Fence>& f12,ComPtr<ID3D11Fence>& f11)
        {
            Check(device12->CreateFence(0,D3D12_FENCE_FLAG_SHARED,IID_PPV_ARGS(&f12)),"shared reader gate");
            HANDLE handle{};Check(device12->CreateSharedHandle(f12.Get(),nullptr,GENERIC_ALL,nullptr,&handle),"gate handle");
            const auto hr=device5->OpenSharedFence(handle,IID_PPV_ARGS(&f11));CloseHandle(handle);Check(hr,"D3D11 reader gate");
        }
    };
}
