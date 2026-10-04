#include "StableColorResolve.h"
#include "PerformanceMetrics.h"
#include "FrameGen/D3D11FrameCopy.h"
#include <d3dcompiler.h>

namespace TheosRenderPipeline::NeuralRendering {
HRESULT StableColorResolve::Initialize(ID3D11Device* device){
    if(!device)return E_INVALIDARG;
    if(device_)return D3D11FrameCopy::SameObject(device_.Get(),device)?S_OK:E_INVALIDARG;
    constexpr char program[]=R"(
Texture2D<float4> original : register(t0);
Texture2D<float4> enhanced : register(t1);
float4 vs(uint id:SV_VertexID):SV_Position {
    float2 uv=float2((id<<1)&2,id&2);
    return float4(uv*float2(2,-2)+float2(-1,1),0,1);
}
float4 ps(float4 position:SV_Position):SV_Target {
    uint width,height;original.GetDimensions(width,height);
    int2 p=int2(position.xy);
    float4 source=original.Load(int3(p,0));
    float3 nr=enhanced.Load(int3(p,0)).rgb;
    float3 broadDelta=0;float weights=0;
    [unroll] for(int y=-1;y<=1;++y) [unroll] for(int x=-1;x<=1;++x) {
        // Adjacent taps avoid spatial aliases that classify repeating fine
        // detail as broad color. Medium structure is intentionally attenuated.
        int2 q=clamp(p+int2(x,y),int2(0,0),int2(width-1,height-1));
        float3 neighbor=original.Load(int3(q,0)).rgb;
        float3 difference=neighbor-source.rgb;
        float w=(x==0?2:1)*(y==0?2:1)*exp(-dot(difference,difference)/0.02);
        broadDelta+=(enhanced.Load(int3(q,0)).rgb-neighbor)*w;weights+=w;
    }
    return float4(saturate(nr-broadDelta/weights),source.a);
}
)";
    Microsoft::WRL::ComPtr<ID3DBlob> code;
    auto hr=D3DCompile(program,sizeof(program)-1,"NRStableColorResolve",nullptr,nullptr,"vs","vs_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&code,nullptr);
    if(FAILED(hr))return hr;
    if(FAILED(hr=device->CreateVertexShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&vertex_)))return hr;
    code.Reset();hr=D3DCompile(program,sizeof(program)-1,"NRStableColorResolve",nullptr,nullptr,"ps","ps_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&code,nullptr);
    if(FAILED(hr))return hr;
    if(FAILED(hr=device->CreatePixelShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&pixel_)))return hr;
    device_=device;return S_OK;
}
HRESULT StableColorResolve::Prepare(Views& views,ID3D11Texture2D* original,ID3D11Texture2D* nr,ID3D11Texture2D* output,PerformanceMetrics* metrics){
    if(!device_||!original||!nr||!output||D3D11FrameCopy::SameObject(original,nr)||
        D3D11FrameCopy::SameObject(original,output)||D3D11FrameCopy::SameObject(nr,output))return E_INVALIDARG;
    D3D11_TEXTURE2D_DESC base{};original->GetDesc(&base);
    for(auto* image:{original,nr,output}){
        Microsoft::WRL::ComPtr<ID3D11Device> device;image->GetDevice(&device);
        D3D11_TEXTURE2D_DESC d{};image->GetDesc(&d);
        if(!D3D11FrameCopy::SameObject(device.Get(),device_.Get())||d.Format!=DXGI_FORMAT_R16G16B16A16_FLOAT||
            d.Width!=base.Width||d.Height!=base.Height||d.ArraySize!=1||d.MipLevels!=1||
            d.SampleDesc.Count!=1||d.SampleDesc.Quality||d.Usage!=D3D11_USAGE_DEFAULT||
            !(d.BindFlags&(image==output?D3D11_BIND_RENDER_TARGET:D3D11_BIND_SHADER_RESOURCE)))return E_INVALIDARG;
    }
    // Each bridge slot caches three views, never additional image allocations.
    auto hr=S_OK;
    if(views.originalImage.Get()!=original){
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
        if(FAILED(hr=device_->CreateShaderResourceView(original,nullptr,&view)))return hr;
        if(metrics)metrics->RecordDescriptorCreation();
        views.original=view;views.originalImage=original;
    }
    if(views.nrImage.Get()!=nr){
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
        if(FAILED(hr=device_->CreateShaderResourceView(nr,nullptr,&view)))return hr;
        if(metrics)metrics->RecordDescriptorCreation();
        views.nr=view;views.nrImage=nr;
    }
    if(views.outputImage.Get()!=output){
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> view;
        if(FAILED(hr=device_->CreateRenderTargetView(output,nullptr,&view)))return hr;
        if(metrics)metrics->RecordDescriptorCreation();
        views.output=view;views.outputImage=output;
    }
    views.width=base.Width;views.height=base.Height;return S_OK;
}
HRESULT StableColorResolve::Draw(ID3D11DeviceContext* context,const Views& views){
    if(!device_||!views.original||!views.nr||!views.output||!views.width||!views.height||
        !D3D11FrameCopy::ValidResources(context,views.originalImage.Get(),views.outputImage.Get()))return E_INVALIDARG;
    Microsoft::WRL::ComPtr<ID3D11Device> device;context->GetDevice(&device);
    if(!D3D11FrameCopy::SameObject(device.Get(),device_.Get()))return E_INVALIDARG;
    D3D11ContextIsolation::Scope scope(isolation_,context);if(!scope)return E_FAIL;
    ID3D11ShaderResourceView* sources[]{views.original.Get(),views.nr.Get()};auto* target=views.output.Get();
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(vertex_.Get(),nullptr,0);context->PSSetShader(pixel_.Get(),nullptr,0);
    context->PSSetShaderResources(0,2,sources);context->OMSetRenderTargets(1,&target,nullptr);
    const D3D11_VIEWPORT viewport{0,0,float(views.width),float(views.height),0,1};
    context->RSSetViewports(1,&viewport);context->Draw(3,0);
    return device_->GetDeviceRemovedReason();
}
}
