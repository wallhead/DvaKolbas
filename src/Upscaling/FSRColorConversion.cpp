#include "FSRColorConversion.h"
#include "FrameGen/D3D11FrameCopy.h"
#include <d3dcompiler.h>
#include <algorithm>
#include <cmath>
namespace TheosRenderPipeline::Upscaling
{
    static bool Known(ColorEncoding encoding){return encoding==ColorEncoding::Linear || encoding==ColorEncoding::Gamma22 || encoding==ColorEncoding::SRGB;}
    Result<float> ConvertColorChannel(float value,ColorEncoding from,ColorEncoding to)
    {
        if(!std::isfinite(value) || !Known(from) || !Known(to))return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,"Explicit finite SDR encoding required"});
        float c=std::clamp(value,0.0f,1.0f);
        if(from==ColorEncoding::Gamma22)c=std::pow(c,2.2f);
        else if(from==ColorEncoding::SRGB)c=c<=0.04045f?c/12.92f:std::pow((c+0.055f)/1.055f,2.4f);
        if(to==ColorEncoding::Gamma22)c=std::pow(c,1.0f/2.2f);
        else if(to==ColorEncoding::SRGB)c=c<=0.0031308f?c*12.92f:1.055f*std::pow(c,1.0f/2.4f)-0.055f;
        return c;
    }
    HRESULT FsrColorConverter::Initialize(ID3D11Device* device)
    {
        if(device_)return D3D11FrameCopy::SameObject(device_.Get(),device)?S_OK:E_INVALIDARG;
        constexpr char program[]=R"(
cbuffer Transfer : register(b0) { uint sourceEncoding; uint targetEncoding; uint2 unused; };
Texture2D<float4> inputImage : register(t0);SamplerState sampling : register(s0);
struct Vertex { float4 position:SV_Position;float2 uv:TEXCOORD0; };
Vertex vs(uint id:SV_VertexID){Vertex v;v.uv=float2((id<<1)&2,id&2);v.position=float4(v.uv*float2(2,-2)+float2(-1,1),0,1);return v;}
float3 decode(float3 c){c=saturate(c);if(sourceEncoding==2)return pow(c,2.2);if(sourceEncoding==3)return float3(c.x<=0.04045?c.x/12.92:pow((c.x+0.055)/1.055,2.4),c.y<=0.04045?c.y/12.92:pow((c.y+0.055)/1.055,2.4),c.z<=0.04045?c.z/12.92:pow((c.z+0.055)/1.055,2.4));return c;}
float3 encode(float3 c){c=saturate(c);if(targetEncoding==2)return pow(c,1.0/2.2);if(targetEncoding==3)return float3(c.x<=0.0031308?c.x*12.92:1.055*pow(c.x,1.0/2.4)-0.055,c.y<=0.0031308?c.y*12.92:1.055*pow(c.y,1.0/2.4)-0.055,c.z<=0.0031308?c.z*12.92:1.055*pow(c.z,1.0/2.4)-0.055);return c;}
float4 ps(Vertex v):SV_Target{float4 c=inputImage.SampleLevel(sampling,v.uv,0);return float4(encode(decode(c.rgb)),c.a);}
)";
        Ptr<ID3DBlob> code;
        auto hr=D3DCompile(program,sizeof(program)-1,"FsrColorConversion",nullptr,nullptr,"vs","vs_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&code,nullptr);
        if(FAILED(hr))return hr;
        if(FAILED(hr=device->CreateVertexShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&vertex_)))return hr;
        code.Reset();hr=D3DCompile(program,sizeof(program)-1,"FsrColorConversion",nullptr,nullptr,"ps","ps_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&code,nullptr);
        if(FAILED(hr))return hr;
        if(FAILED(hr=device->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&pixel_)))return hr;
        D3D11_SAMPLER_DESC sampler{};sampler.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;sampler.AddressU=sampler.AddressV=sampler.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;sampler.MaxLOD=D3D11_FLOAT32_MAX;
        if(FAILED(hr=device->CreateSamplerState(&sampler,&sampler_)))return hr;
        D3D11_BUFFER_DESC desc{};desc.ByteWidth=16;desc.BindFlags=D3D11_BIND_CONSTANT_BUFFER;desc.Usage=D3D11_USAGE_DEFAULT;
        if(FAILED(hr=device->CreateBuffer(&desc,nullptr,&constants_)))return hr;
        device_=device;return S_OK;
    }
    HRESULT FsrColorConverter::Convert(ID3D11DeviceContext* context,ID3D11Texture2D* input,ID3D11Texture2D* output,ColorEncoding from,ColorEncoding to)
    {
        if(!Known(from) || !Known(to) || !D3D11FrameCopy::ValidResources(context,input,output))return E_INVALIDARG;
        D3D11_TEXTURE2D_DESC in{},out{};input->GetDesc(&in);output->GetDesc(&out);
        auto supported=[](DXGI_FORMAT format){return format==DXGI_FORMAT_R8G8B8A8_UNORM || format==DXGI_FORMAT_B8G8R8A8_UNORM || format==DXGI_FORMAT_R16G16B16A16_FLOAT || format==DXGI_FORMAT_R32G32B32A32_FLOAT;};
        if(!supported(in.Format) || !supported(out.Format) || !(in.BindFlags&D3D11_BIND_SHADER_RESOURCE) || !(out.BindFlags&D3D11_BIND_RENDER_TARGET) ||
            in.SampleDesc.Count!=1 || out.SampleDesc.Count!=1 || in.MipLevels!=1 || out.MipLevels!=1 || in.ArraySize!=1 || out.ArraySize!=1)return E_INVALIDARG;
        Ptr<ID3D11Device> device;context->GetDevice(&device);auto hr=Initialize(device.Get());if(FAILED(hr))return hr;
        if(input_.Get()!=input){Ptr<ID3D11ShaderResourceView> view;hr=device->CreateShaderResourceView(input,nullptr,&view);if(FAILED(hr))return hr;srv_=view;input_=input;}
        if(output_.Get()!=output){Ptr<ID3D11RenderTargetView> view;hr=device->CreateRenderTargetView(output,nullptr,&view);if(FAILED(hr))return hr;rtv_=view;output_=output;}
        D3D11ContextIsolation::Scope scope(isolation_,context);if(!scope)return E_FAIL;
        const UINT values[]{static_cast<UINT>(from),static_cast<UINT>(to),0,0};context->UpdateSubresource(constants_.Get(),0,nullptr,values,0,0);
        auto* buffer=constants_.Get();auto* sampler=sampler_.Get();auto* srv=srv_.Get();auto* rtv=rtv_.Get();
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);context->VSSetShader(vertex_.Get(),nullptr,0);context->PSSetShader(pixel_.Get(),nullptr,0);
        context->PSSetConstantBuffers(0,1,&buffer);context->PSSetSamplers(0,1,&sampler);context->PSSetShaderResources(0,1,&srv);context->OMSetRenderTargets(1,&rtv,nullptr);
        const D3D11_VIEWPORT viewport{0,0,float(out.Width),float(out.Height),0,1};context->RSSetViewports(1,&viewport);context->Draw(3,0);
        return device->GetDeviceRemovedReason();
    }
}
