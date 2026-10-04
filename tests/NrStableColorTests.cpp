#include "NeuralRendering/StableColorResolve.h"
#include "NeuralRendering/PerformanceMetrics.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <DirectXPackedVector.h>
#include <vector>
#include <cmath>
#include <cstdio>

using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
using DirectX::PackedVector::XMConvertFloatToHalf;
using DirectX::PackedVector::XMConvertHalfToFloat;
namespace {void Need(HRESULT hr){if(FAILED(hr)){std::printf("FAIL HRESULT %08X\n",unsigned(hr));ExitProcess(1);}}}
int main(){
    if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
    unsigned failures{};auto check=[&](bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);failures+=!ok;};
    constexpr UINT width=64,height=32;
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    Need(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context));
    auto texture=[&](UINT bind,bool staging=false){D3D11_TEXTURE2D_DESC d{};
        d.Width=width;d.Height=height;d.MipLevels=d.ArraySize=d.SampleDesc.Count=1;
        d.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;d.Usage=staging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;
        d.BindFlags=bind;d.CPUAccessFlags=staging?D3D11_CPU_ACCESS_READ:0;
        ComPtr<ID3D11Texture2D> t;Need(device->CreateTexture2D(&d,nullptr,&t));return t;};
    auto source=texture(D3D11_BIND_SHADER_RESOURCE),nr=texture(D3D11_BIND_SHADER_RESOURCE);
    auto output=texture(D3D11_BIND_RENDER_TARGET),readback=texture(0,true);
    StableColorResolve resolve;Need(resolve.Initialize(device.Get()));StableColorResolve::Views views;
    PerformanceMetrics metrics;metrics.Enable(true);metrics.BeginFrame(1,true);
    Need(resolve.Prepare(views,source.Get(),nr.Get(),output.Get(),&metrics));
    check(metrics.Snapshot().frames[0].descriptorCreations==3,"ThreeViewsAndNoExtraImages");
    metrics.BeginFrame(2,true);Need(resolve.Prepare(views,source.Get(),nr.Get(),output.Get(),&metrics));
    check(metrics.Snapshot().frames[1].descriptorCreations==0,"WarmViewsAreReused");
    check(resolve.Prepare(views,source.Get(),nr.Get(),source.Get())==E_INVALIDARG,"RejectReadWriteAlias");
    auto noTarget=texture(D3D11_BIND_SHADER_RESOURCE);
    check(resolve.Prepare(views,source.Get(),nr.Get(),noTarget.Get())==E_INVALIDARG,"RejectMissingRenderTarget");
    ComPtr<ID3D11DeviceContext> deferred;Need(device->CreateDeferredContext(0,&deferred));
    check(resolve.Draw(deferred.Get(),views)==E_INVALIDARG,"RejectDeferredContext");
    std::vector<unsigned short> original(size_t(width)*height*4),enhanced(original.size()),pixels(original.size());
    for(unsigned mode=0;mode<6;++mode){
        const bool fine=mode==1||mode>=4;
        const float period=mode==4?2.f:mode==5?4.f:8.f;
        for(UINT y=0;y<height;++y)for(UINT x=0;x<width;++x){auto i=(size_t(y)*width+x)*4;
            for(unsigned c=0;c<3;++c){const float base=mode==2?(x<width/2?.05f:.8f):mode==3?.0005f:.2f+.05f*c;
                const float offset=mode==2?(x<width/2?.2f:-.2f):.08f-.02f*c;
                const float detail=fine?.02f*std::cos(x*6.28318530718f/period):0;
                original[i+c]=XMConvertFloatToHalf(base);enhanced[i+c]=XMConvertFloatToHalf(base+offset+detail);}
            original[i+3]=XMConvertFloatToHalf(float(x+y)/float(width+height));enhanced[i+3]=0;
        }
        context->UpdateSubresource(source.Get(),0,nullptr,original.data(),width*8,0);
        context->UpdateSubresource(nr.Get(),0,nullptr,enhanced.data(),width*8,0);
        auto* saved=views.original.Get();context->PSSetShaderResources(3,1,&saved);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        Need(resolve.Draw(context.Get(),views));
        ComPtr<ID3D11ShaderResourceView> restored;context->PSGetShaderResources(3,1,&restored);
        D3D11_PRIMITIVE_TOPOLOGY topology{};context->IAGetPrimitiveTopology(&topology);
        check(restored.Get()==saved&&topology==D3D11_PRIMITIVE_TOPOLOGY_LINELIST,"ProducerContextBindingsRestored");
        context->CopyResource(readback.Get(),output.Get());D3D11_MAPPED_SUBRESOURCE mapped{};
        Need(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped));
        for(UINT y=0;y<height;++y)std::copy_n(reinterpret_cast<const unsigned short*>(static_cast<const unsigned char*>(mapped.pData)+y*mapped.RowPitch),width*4,pixels.data()+size_t(y)*width*4);
        context->Unmap(readback.Get(),0);bool alpha=true,colors=true;
        for(UINT y=0;y<height;++y)for(UINT x=0;x<width;++x){auto i=(size_t(y)*width+x)*4;
            alpha&=pixels[i+3]==original[i+3];
            // Away from image borders, fine detail is distinguishable
            // from the uniform color shift: restoring the source wholesale fails.
            if(fine&&(x<4||x>=width-4))continue;
            for(unsigned c=0;c<3;++c){const float base=XMConvertHalfToFloat(original[i+c]);
                const float actual=XMConvertHalfToFloat(pixels[i+c]);
                const float detail=fine?.02f*std::cos(x*6.28318530718f/period):0;
                if(fine){if(std::abs(detail)>.004f){const float gain=(actual-base)/detail;
                    colors&=gain>=(mode==4?.85f:mode==5?.4f:.1f)&&gain<=1.1f;}}
                else colors&=std::abs(actual-base)<(mode==3?.00004f:.0005f);}
        }
        check(alpha,"SourceAlphaPreservedEveryPixel");
        check(colors,mode==0?"UniformNrGradeRemoved":mode==1?"EightPixelNrDetailRetained":mode==2?"SourceEdgesDoNotBleedColor":mode==3?"NearBlackSourcePreserved":mode==4?"TwoPixelNrDetailRetained":"FourPixelNrDetailRetained");
    }
    return failures?1:0;
}
