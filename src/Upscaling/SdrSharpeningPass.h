#pragma once
#include "RCASParameters.h"
#include "FrameGen/D3D11ContextIsolation.h"
#include <d3dcompiler.h>
#include <filesystem>

namespace TheosRenderPipeline::Upscaling
{
    // In-place, HUD-less output sharpening. Zero strength is an exact bypass.
    // The context and all producer bindings are restored before returning.
    class SdrSharpeningPass
    {
    public:
        HRESULT Initialize(ID3D11Device* device,const std::filesystem::path& shaderPath)
        {
            if(!device)return E_INVALIDARG;
            if(device_)return device_.Get()==device?S_OK:E_INVALIDARG;
            const D3D_SHADER_MACRO macros[]{{"TRP_PRESERVE_ALPHA","1"},{nullptr,nullptr}};
            Microsoft::WRL::ComPtr<ID3DBlob> code;
            auto hr=D3DCompileFromFile(shaderPath.c_str(),macros,nullptr,"main","cs_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&code,nullptr);
            if(FAILED(hr))return hr;
            hr=device->CreateComputeShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&shader_);
            if(SUCCEEDED(hr))device_=device;
            return hr;
        }
        HRESULT Apply(ID3D11DeviceContext* context,ID3D11Texture2D* image,float strength)
        {
            if(!context || !image || !std::isfinite(strength) || strength<0 || strength>1)return E_INVALIDARG;
            if(strength==0)return S_OK;
            if(!device_ || !shader_)return E_UNEXPECTED;
            Microsoft::WRL::ComPtr<ID3D11Device> producer,owner;
            context->GetDevice(&producer);image->GetDevice(&owner);
            if(producer.Get()!=device_.Get() || owner.Get()!=device_.Get())return E_INVALIDARG;
            D3D11_TEXTURE2D_DESC desc{};image->GetDesc(&desc);
            if(desc.SampleDesc.Count!=1 || desc.ArraySize!=1 || desc.MipLevels!=1 ||
                (desc.Format!=DXGI_FORMAT_R8G8B8A8_UNORM && desc.Format!=DXGI_FORMAT_R16G16B16A16_FLOAT && desc.Format!=DXGI_FORMAT_R32G32B32A32_FLOAT))return E_INVALIDARG;
            if(!input_ || width_!=desc.Width || height_!=desc.Height || format_!=desc.Format) {
                // Build the replacement completely before publishing it.
                Microsoft::WRL::ComPtr<ID3D11Texture2D> input,output;
                Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
                Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;
                desc.Usage=D3D11_USAGE_DEFAULT;desc.CPUAccessFlags=desc.MiscFlags=0;
                desc.BindFlags=D3D11_BIND_SHADER_RESOURCE;
                auto hr=device_->CreateTexture2D(&desc,nullptr,&input);if(FAILED(hr))return hr;
                if(FAILED(hr=device_->CreateShaderResourceView(input.Get(),nullptr,&srv)))return hr;
                desc.BindFlags=D3D11_BIND_UNORDERED_ACCESS;
                if(FAILED(hr=device_->CreateTexture2D(&desc,nullptr,&output)))return hr;
                if(FAILED(hr=device_->CreateUnorderedAccessView(output.Get(),nullptr,&uav)))return hr;
                input_=std::move(input);output_=std::move(output);srv_=std::move(srv);uav_=std::move(uav);
                width_=desc.Width;height_=desc.Height;format_=desc.Format;
            }
            D3D11ContextIsolation::Scope isolated(isolation_,context);if(!isolated)return E_UNEXPECTED;
            auto hr=parameters_.Update(device_.Get(),context,strength);if(FAILED(hr))return hr;
            context->CopyResource(input_.Get(),image);
            auto constants=parameters_.Bind(context);
            auto* srv=srv_.Get();auto* uav=uav_.Get();
            context->CSSetShader(shader_.Get(),nullptr,0);context->CSSetShaderResources(0,1,&srv);
            context->CSSetUnorderedAccessViews(0,1,&uav,nullptr);
            context->Dispatch((width_+7)/8,(height_+7)/8,1);
            srv=nullptr;uav=nullptr;context->CSSetShaderResources(0,1,&srv);context->CSSetUnorderedAccessViews(0,1,&uav,nullptr);
            context->CopyResource(image,output_.Get());
            return S_OK;
        }
    private:
        Microsoft::WRL::ComPtr<ID3D11Device> device_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> shader_;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> input_,output_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav_;
        RCASParameters parameters_;D3D11ContextIsolation isolation_;
        UINT width_{},height_{};DXGI_FORMAT format_{DXGI_FORMAT_UNKNOWN};
    };
}
