#include "FSRPresentationColor.h"
#include "FrameGen/D3D11FrameCopy.h"
#include <d3dcompiler.h>
namespace TheosRenderPipeline::Upscaling
{
    HRESULT FsrPresentationUiConverter::Initialize(ID3D11Device* device)
    {
        if(device_)return D3D11FrameCopy::SameObject(device_.Get(),device)?S_OK:E_INVALIDARG;
        constexpr char program[]=R"(
cbuffer Transfer : register(b0) { uint sourceEncoding; uint hasOverlay; uint2 unused; };
Texture2D<float4> hud : register(t0);Texture2D<float4> overlay : register(t1);
struct Vertex { float4 position:SV_Position; };
Vertex vs(uint id:SV_VertexID){Vertex v;float2 uv=float2((id<<1)&2,id&2);v.position=float4(uv*float2(2,-2)+float2(-1,1),0,1);return v;}
float3 decode(float3 c){c=saturate(c);if(sourceEncoding==2)return pow(c,2.2);if(sourceEncoding==3)return float3(c.x<=0.04045?c.x/12.92:pow((c.x+0.055)/1.055,2.4),c.y<=0.04045?c.y/12.92:pow((c.y+0.055)/1.055,2.4),c.z<=0.04045?c.z/12.92:pow((c.z+0.055)/1.055,2.4));return c;}
float3 encode(float3 c){c=saturate(c);return float3(c.x<=0.0031308?c.x*12.92:1.055*pow(c.x,1.0/2.4)-0.055,c.y<=0.0031308?c.y*12.92:1.055*pow(c.y,1.0/2.4)-0.055,c.z<=0.0031308?c.z*12.92:1.055*pow(c.z,1.0/2.4)-0.055);}
float4 ps(Vertex v):SV_Target{
    int3 p=int3(int2(v.position.xy),0);float4 c=hud.Load(p);
    if(hasOverlay){float4 o=overlay.Load(p);c=o+c*(1-saturate(o.a));}
    float alpha=saturate(c.a);if(alpha<=0)return float4(0,0,0,0);
    return float4(encode(decode(c.rgb/alpha))*alpha,alpha);
})";
        Ptr<ID3DBlob> code;auto hr=D3DCompile(program,sizeof(program)-1,"FsrPresentationUi",nullptr,nullptr,"vs","vs_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&code,nullptr);
        if(FAILED(hr))return hr;
        if(FAILED(hr=device->CreateVertexShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&vertex_)))return hr;
        code.Reset();hr=D3DCompile(program,sizeof(program)-1,"FsrPresentationUi",nullptr,nullptr,"ps","ps_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&code,nullptr);
        if(FAILED(hr))return hr;
        if(FAILED(hr=device->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&pixel_)))return hr;
        D3D11_BUFFER_DESC desc{};desc.ByteWidth=16;desc.Usage=D3D11_USAGE_DEFAULT;desc.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
        if(FAILED(hr=device->CreateBuffer(&desc,nullptr,&constants_)))return hr;
        device_=device;return S_OK;
    }
    HRESULT FsrPresentationUiConverter::Convert(ID3D11DeviceContext* context,ID3D11Texture2D* ui,
        ID3D11ShaderResourceView* overlay,ID3D11Texture2D* output,ColorEncoding source)
    {
        if(!IsKnownColorEncoding(source) || !D3D11FrameCopy::ValidResources(context,ui,output))return E_INVALIDARG;
        D3D11_TEXTURE2D_DESC in{},out{};ui->GetDesc(&in);output->GetDesc(&out);
        const auto supported=[](const D3D11_TEXTURE2D_DESC& d){return SupportsFsrHandoffFormat(d.Format) && d.Width && d.Height &&
            d.MipLevels==1 && d.ArraySize==1 && d.SampleDesc.Count==1 && d.SampleDesc.Quality==0;};
        if(!supported(in) || !supported(out) || in.Width!=out.Width || in.Height!=out.Height ||
            !(in.BindFlags&D3D11_BIND_SHADER_RESOURCE) || !(out.BindFlags&D3D11_BIND_RENDER_TARGET) || out.Format!=DXGI_FORMAT_R8G8B8A8_UNORM)return E_INVALIDARG;
        Ptr<ID3D11Device> device;context->GetDevice(&device);
        if(overlay){
            Ptr<ID3D11Resource> resource;overlay->GetResource(&resource);Ptr<ID3D11Texture2D> texture;Ptr<ID3D11Device> overlayDevice;
            if(!resource || FAILED(resource.As(&texture)))return E_INVALIDARG;
            texture->GetDevice(&overlayDevice);D3D11_SHADER_RESOURCE_VIEW_DESC view{};overlay->GetDesc(&view);
            if(!D3D11FrameCopy::SameObject(device.Get(),overlayDevice.Get()) ||
                D3D11FrameCopy::SameObject(texture.Get(),output) || D3D11FrameCopy::SameObject(texture.Get(),ui))return E_INVALIDARG;
            D3D11_TEXTURE2D_DESC d{};texture->GetDesc(&d);
            if(!supported(d) || d.Width!=in.Width || d.Height!=in.Height || view.Format!=d.Format ||
                view.ViewDimension!=D3D11_SRV_DIMENSION_TEXTURE2D || view.Texture2D.MostDetailedMip || view.Texture2D.MipLevels!=1)return E_INVALIDARG;
        }
        auto hr=Initialize(device.Get());if(FAILED(hr))return hr;
        if(input_.Get()!=ui){Ptr<ID3D11ShaderResourceView> view;hr=device->CreateShaderResourceView(ui,nullptr,&view);if(FAILED(hr))return hr;uiView_=view;input_=ui;}
        if(output_.Get()!=output){Ptr<ID3D11RenderTargetView> view;hr=device->CreateRenderTargetView(output,nullptr,&view);if(FAILED(hr))return hr;outputView_=view;output_=output;}
        D3D11ContextIsolation::Scope scope(isolation_,context);if(!scope)return E_FAIL;
        const UINT values[]{static_cast<UINT>(source),overlay?1u:0u,0,0};context->UpdateSubresource(constants_.Get(),0,nullptr,values,0,0);
        auto* buffer=constants_.Get();ID3D11ShaderResourceView* views[]{uiView_.Get(),overlay};auto* rtv=outputView_.Get();
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);context->VSSetShader(vertex_.Get(),nullptr,0);context->PSSetShader(pixel_.Get(),nullptr,0);
        context->PSSetConstantBuffers(0,1,&buffer);context->PSSetShaderResources(0,2,views);context->OMSetRenderTargets(1,&rtv,nullptr);
        const D3D11_VIEWPORT viewport{0,0,float(out.Width),float(out.Height),0,1};context->RSSetViewports(1,&viewport);context->Draw(3,0);
        return device->GetDeviceRemovedReason();
    }
}
