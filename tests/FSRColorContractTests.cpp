#include "Upscaling/FSRColorConversion.h"
#include "InteropTestRig.h"
#include <DirectXPackedVector.h>
#include <cmath>
#include <limits>
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
int main()
{
    Rig rig;FsrColorConverter converter;auto desc=rig.Description();desc.Width=desc.Height=4;
    desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> input,encoded;Check(rig.device11->CreateTexture2D(&desc,nullptr,&input),"encoded input");Check(rig.device11->CreateTexture2D(&desc,nullptr,&encoded),"encoded output");
    std::uint32_t pixels[16];std::fill_n(pixels,16,0x804080C0u);rig.context11->UpdateSubresource(input.Get(),0,nullptr,pixels,16,0);
    desc.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;ComPtr<ID3D11Texture2D> linear;Check(rig.device11->CreateTexture2D(&desc,nullptr,&linear),"linear intermediate");
    desc.BindFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;desc.Usage=D3D11_USAGE_STAGING;ComPtr<ID3D11Texture2D> staging;Check(rig.device11->CreateTexture2D(&desc,nullptr,&staging),"linear staging");
    desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;ComPtr<ID3D11Texture2D> encodedReadback;Check(rig.device11->CreateTexture2D(&desc,nullptr,&encodedReadback),"native encoded staging");
    D3D11_VIEWPORT viewport{7,9,13,17,0,1};rig.context11->RSSetViewports(1,&viewport);
    for(auto transfer:{ColorEncoding::Gamma22,ColorEncoding::SRGB,ColorEncoding::Linear}) {
        Check(converter.Convert(rig.context11.Get(),input.Get(),linear.Get(),transfer,ColorEncoding::Linear),"explicit decode to linear");
        rig.context11->CopyResource(staging.Get(),linear.Get());D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"linear readback");
        auto* half=static_cast<DirectX::PackedVector::HALF*>(mapped.pData);auto expected=ConvertColorChannel(192.0f/255,transfer,ColorEncoding::Linear);
        Require(expected && std::abs(DirectX::PackedVector::XMConvertHalfToFloat(half[0])-*expected)<0.001f,"FsrColorContract: known encoded patch decoded once");
        Require(std::abs(DirectX::PackedVector::XMConvertHalfToFloat(half[3])-128.0f/255)<0.001f,"alpha remains independent of transfer");rig.context11->Unmap(staging.Get(),0);
        Check(converter.Convert(rig.context11.Get(),linear.Get(),encoded.Get(),ColorEncoding::Linear,transfer),"return to native handoff transfer");
        rig.context11->CopyResource(encodedReadback.Get(),encoded.Get());Check(rig.context11->Map(encodedReadback.Get(),0,D3D11_MAP_READ,0,&mapped),"encoded round trip readback");
        auto* bytes=static_cast<unsigned char*>(mapped.pData);Require(std::abs(int(bytes[0])-192)<=1 && std::abs(int(bytes[1])-128)<=1 && std::abs(int(bytes[2])-64)<=1 && std::abs(int(bytes[3])-128)<=1,"native handoff transfer round trip preserves known SDR patch and alpha");rig.context11->Unmap(encodedReadback.Get(),0);
        UINT count=1;D3D11_VIEWPORT restored{};rig.context11->RSGetViewports(&count,&restored);Require(count==1 && restored.TopLeftX==7 && restored.Width==13,"downstream graphics state restored");
    }
    Require(FAILED(converter.Convert(rig.context11.Get(),input.Get(),linear.Get(),ColorEncoding::Unknown,ColorEncoding::Linear)),"format cannot establish color encoding");
    Require(!ConvertColorChannel(std::numeric_limits<float>::quiet_NaN(),ColorEncoding::Linear,ColorEncoding::SRGB),"nonfinite reference input rejected");
    rig.ValidateDebug();std::puts("PASS: explicit linear/gamma/sRGB GPU conversion, independent alpha and state restoration");
}
