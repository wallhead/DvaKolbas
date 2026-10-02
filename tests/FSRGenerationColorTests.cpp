#include "Upscaling/FSRPresentationColor.h"
#include "Upscaling/FSRColorConversion.h"
#include "InteropTestRig.h"
#include <DirectXPackedVector.h>
#include <array>
#include <cmath>
using namespace InteropFixture;
using namespace TheosRenderPipeline::Upscaling;
int main()
{
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    Check(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context),"WARP color device");
    D3D11_TEXTURE2D_DESC desc{};desc.Width=desc.Height=4;desc.MipLevels=desc.ArraySize=1;desc.SampleDesc.Count=1;
    desc.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;desc.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> ui,overlay,output,readback;
    Check(device->CreateTexture2D(&desc,nullptr,&ui),"HUD input");Check(device->CreateTexture2D(&desc,nullptr,&overlay),"overlay input");
    ComPtr<ID3D11ShaderResourceView> overlayView;Check(device->CreateShaderResourceView(overlay.Get(),nullptr,&overlayView),"overlay view");
    desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;Check(device->CreateTexture2D(&desc,nullptr,&output),"sRGB publication output");
    desc.BindFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;desc.Usage=D3D11_USAGE_STAGING;
    Check(device->CreateTexture2D(&desc,nullptr,&readback),"readback");
    auto fill=[&](ID3D11Texture2D* texture,float rgb,float alpha){
        std::array<DirectX::PackedVector::HALF,64> values{};
        for(int i=0;i<16;++i){for(int j=0;j<3;++j)values[i*4+j]=DirectX::PackedVector::XMConvertFloatToHalf(rgb);values[i*4+3]=DirectX::PackedVector::XMConvertFloatToHalf(alpha);}
        context->UpdateSubresource(texture,0,nullptr,values.data(),32,0);
    };
    auto expect=[&](int rgb,int alpha,const char* name){
        context->CopyResource(readback.Get(),output.Get());D3D11_MAPPED_SUBRESOURCE mapped{};Check(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped),"pixel readback");
        for(UINT y=0;y<4;++y)for(UINT x=0;x<4;++x){auto* p=static_cast<unsigned char*>(mapped.pData)+y*mapped.RowPitch+x*4;
            Require(std::abs(int(p[0])-rgb)<=1 && std::abs(int(p[1])-rgb)<=1 && std::abs(int(p[2])-rgb)<=1 && std::abs(int(p[3])-alpha)<=1,name);}
        context->Unmap(readback.Get(),0);
    };
    FsrPresentationUiConverter converter;FsrColorConverter scene;
    D3D11_VIEWPORT viewport{7,9,13,17,0,1};context->RSSetViewports(1,&viewport);
    fill(ui.Get(),.125f,.5f);
    Check(converter.Convert(context.Get(),ui.Get(),nullptr,output.Get(),ColorEncoding::Linear),"linear premultiplied HUD");expect(68,128,"HalfAlphaCoveragePreserved");
    Check(converter.Convert(context.Get(),ui.Get(),nullptr,output.Get(),ColorEncoding::Gamma22),"gamma premultiplied HUD");expect(31,128,"Gamma22HalfAlphaCoveragePreserved");
    fill(ui.Get(),1,0);Check(converter.Convert(context.Get(),ui.Get(),nullptr,output.Get(),ColorEncoding::Linear),"zero coverage HUD");expect(0,0,"ZeroAlphaHasNoHalo");
    fill(ui.Get(),.125f,.5f);fill(overlay.Get(),.1f,.25f);
    Check(converter.Convert(context.Get(),ui.Get(),overlayView.Get(),output.Get(),ColorEncoding::Linear),"HUD plus overlay");expect(95,159,"OverlayIncludedOnce");
    fill(ui.Get(),.25f,1);
    Check(scene.Convert(context.Get(),ui.Get(),output.Get(),ColorEncoding::Gamma22,ColorEncoding::SRGB),"Gamma22 scene to SRGB");expect(61,255,"Gamma22SceneConvertedToSrgb");
    UINT count=1;D3D11_VIEWPORT restored{};context->RSGetViewports(&count,&restored);
    Require(count==1 && restored.TopLeftX==7 && restored.TopLeftY==9 && restored.Width==13 && restored.Height==17,"ContextStatePreserved");
    Require(FAILED(converter.Convert(context.Get(),ui.Get(),nullptr,output.Get(),ColorEncoding::Unknown)),"UnknownEncodingRejected");
    Require(FAILED(converter.Convert(context.Get(),ui.Get(),nullptr,ui.Get(),ColorEncoding::Linear)),"OutputAliasRejected");
    ComPtr<ID3D11Device> foreign;ComPtr<ID3D11DeviceContext> foreignContext;
    Check(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&foreign,nullptr,&foreignContext),"foreign D3D11 device");
    desc.Usage=D3D11_USAGE_DEFAULT;desc.CPUAccessFlags=0;desc.BindFlags=D3D11_BIND_SHADER_RESOURCE;ComPtr<ID3D11Texture2D> foreignOverlay;
    Check(foreign->CreateTexture2D(&desc,nullptr,&foreignOverlay),"foreign overlay");ComPtr<ID3D11ShaderResourceView> foreignView;
    Check(foreign->CreateShaderResourceView(foreignOverlay.Get(),nullptr,&foreignView),"foreign overlay view");
    Require(FAILED(converter.Convert(context.Get(),ui.Get(),foreignView.Get(),output.Get(),ColorEncoding::Linear)),"ForeignOverlayRejected");
    std::puts("PASS: independent literal WARP pixels verify scene transfer, premultiplied HUD coverage, transparent edges and single overlay composition");
}
