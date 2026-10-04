#include "NeuralRendering/PostSrContract.h"
#include "FrameGen/D3D11FrameCopy.h"
#include "FrameGen/D3D11ContextIsolation.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include "../nr-postfg/SyntheticScene.h"
#include <DirectXPackedVector.h>
#include <dxgi1_6.h>
#include <cstdio>
#include <cstring>
using namespace TheosRenderPipeline;
using namespace NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
void Need(HRESULT result){if(FAILED(result))throw result;}
ComPtr<ID3D11Texture2D> Texture(ID3D11Device* device,ImageExtent extent,DXGI_FORMAT format,UINT bind,bool staging=false){
    D3D11_TEXTURE2D_DESC d{};d.Width=extent.width;d.Height=extent.height;
    d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=format;
    d.Usage=staging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;d.BindFlags=staging?0:bind;
    d.CPUAccessFlags=staging?D3D11_CPU_ACCESS_READ:0;
    ComPtr<ID3D11Texture2D> result;Need(device->CreateTexture2D(&d,nullptr,&result));return result;
}
template<class T> uint64_t Compare(ID3D11DeviceContext* context,ID3D11Texture2D* staging,const std::vector<T>& expected,ImageExtent extent,unsigned channels){
    D3D11_MAPPED_SUBRESOURCE map{};Need(context->Map(staging,0,D3D11_MAP_READ,0,&map));
    uint64_t matched{};
    for(unsigned y=0;y<extent.height;++y){const auto* row=static_cast<const unsigned char*>(map.pData)+size_t(y)*map.RowPitch;
        for(unsigned x=0;x<extent.width;++x){const auto index=(size_t(y)*extent.width+x)*channels;
            matched+=std::memcmp(row+size_t(x)*sizeof(T)*channels,expected.data()+index,sizeof(T)*channels)==0;}}
    context->Unmap(staging,0);return matched;
}
}
int main(){try{
    if(NrRuntimeResearch::GameRunningOrUnknown()){std::puts("REFUSED: Skyrim running or inventory unavailable");return 1;}
    ComPtr<IDXGIFactory6> factory;Need(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));
    ComPtr<IDXGIAdapter1> adapter;Need(factory->EnumAdapterByGpuPreference(0,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&adapter)));
    DXGI_ADAPTER_DESC1 description{};Need(adapter->GetDesc1(&description));
    if(description.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)return 1;
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    Need(D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context));
    std::printf("ADAPTER vendor=0x%04x device=0x%04x luidLow=%u luidHigh=%d\n",description.VendorId,description.DeviceId,description.AdapterLuid.LowPart,description.AdapterLuid.HighPart);
    uint64_t tested{},colorMatched{},depthMatched{},motionMatched{};unsigned sources{},restored{},epoch{};
    for(const auto extent:{ImageExtent{64,32},ImageExtent{80,40},ImageExtent{64,32}}){
        if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
        ++epoch;
        auto color=Texture(device.Get(),extent,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
        auto copiedColor=Texture(device.Get(),extent,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
        auto uploadedDepth=Texture(device.Get(),extent,DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE);
        auto depth=Texture(device.Get(),extent,DXGI_FORMAT_R32_TYPELESS,D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE);
        auto copiedDepth=Texture(device.Get(),extent,DXGI_FORMAT_R32_FLOAT,D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE);
        auto motion=Texture(device.Get(),extent,DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
        auto copiedMotion=Texture(device.Get(),extent,DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
        auto readColor=Texture(device.Get(),extent,DXGI_FORMAT_R8G8B8A8_UNORM,0,true);
        auto readDepth=Texture(device.Get(),extent,DXGI_FORMAT_R32_FLOAT,0,true);
        auto readMotion=Texture(device.Get(),extent,DXGI_FORMAT_R16G16_FLOAT,0,true);
        ComPtr<ID3D11RenderTargetView> rtv;Need(device->CreateRenderTargetView(color.Get(),nullptr,&rtv));
        D3D11ContextIsolation isolation;D3D11FrameCopy::Depth depthCopy;
        for(unsigned frame=0;frame<16;++frame){
            NrPostFgResearch::Scene scene;scene.cameraVelocityX=1.5;scene.cameraVelocityY=.5;
            scene.boxes={{8,4,16,12,3,0,2,{.8f,.2f,.1f,1},1},{12,8,4,8,0,1,1,{.1f,.8f,.2f,1},2}};
            const double time=double(sources+1)/60.;
            const auto reference=NrPostFgResearch::Rasterize(scene,extent.width,extent.height,time,time-1./60.);
            std::vector<unsigned char> rgba(reference.pixels.size()*4);
            std::vector<float> depths(reference.pixels.size());
            std::vector<unsigned short> velocities(reference.pixels.size()*2);
            for(size_t i=0;i<reference.pixels.size();++i){const auto& p=reference.pixels[i];
                for(unsigned c=0;c<3;++c)rgba[4*i+c]=static_cast<unsigned char>(p.color[c]*255);
                rgba[4*i+3]=static_cast<unsigned char>((i+frame)%256);
                // Standard finite device-depth encoding, not linear metres.
                depths[i]=100.f/99.9f-.1f*100.f/(99.9f*p.depthMetres);
                velocities[2*i]=DirectX::PackedVector::XMConvertFloatToHalf(p.currentToHistoryPixelsX/extent.width);
                velocities[2*i+1]=DirectX::PackedVector::XMConvertFloatToHalf(p.currentToHistoryPixelsY/extent.height);
            }
            context->UpdateSubresource(color.Get(),0,nullptr,rgba.data(),extent.width*4,0);
            context->UpdateSubresource(uploadedDepth.Get(),0,nullptr,depths.data(),extent.width*4,0);
            context->CopyResource(depth.Get(),uploadedDepth.Get());
            context->UpdateSubresource(motion.Get(),0,nullptr,velocities.data(),extent.width*4,0);
            PostSrSourceContract metadata;metadata.backend=Upscaling::BackendKind::Fsr;metadata.outcome=Upscaling::UpscaleOutcome::Temporal;
            metadata.epoch=metadata.guideEpoch=epoch;metadata.sourceId=metadata.guideSourceId=++sources;metadata.previousSourceId=sources-1;
            metadata.sourceTime=metadata.guideTime=time;metadata.render=metadata.display=metadata.color=metadata.guides=extent;
            metadata.colorDomain=ColorDomain::SdrBytes;metadata.encoding=Upscaling::ColorEncoding::Gamma22;
            metadata.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;metadata.depthFormat=DXGI_FORMAT_R32_FLOAT;metadata.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
            metadata.guideOrigin=GuideOrigin::RealSource;metadata.motion={float(extent.width),float(extent.height),true,false};
            const auto admitted=ValidatePostSrSourceContract(metadata);if(!admitted)return 1;
            auto* target=rtv.Get();context->OMSetRenderTargets(1,&target,nullptr);
            {
                D3D11ContextIsolation::Scope scope(isolation,context.Get());if(!scope)return 1;
                context->OMSetRenderTargets(0,nullptr,nullptr);
                Need(D3D11FrameCopy::Color(context.Get(),color.Get(),copiedColor.Get(),{extent.width,extent.height}));
                Need(depthCopy.Copy(context.Get(),depth.Get(),copiedDepth.Get(),{extent.width,extent.height}));
                Need(D3D11FrameCopy::Color(context.Get(),motion.Get(),copiedMotion.Get(),{extent.width,extent.height}));
            }
            ComPtr<ID3D11RenderTargetView> actual;context->OMGetRenderTargets(1,&actual,nullptr);restored+=actual.Get()==rtv.Get();
            context->OMSetRenderTargets(0,nullptr,nullptr);
            context->CopyResource(readColor.Get(),copiedColor.Get());context->CopyResource(readDepth.Get(),copiedDepth.Get());context->CopyResource(readMotion.Get(),copiedMotion.Get());
            // Map waits for the actual queued copies before reusing any owner.
            colorMatched+=Compare(context.Get(),readColor.Get(),rgba,extent,4);
            depthMatched+=Compare(context.Get(),readDepth.Get(),depths,extent,1);
            motionMatched+=Compare(context.Get(),readMotion.Get(),velocities,extent,2);
            tested+=reference.pixels.size();
        }
    }
    const bool passed=tested && colorMatched==tested && depthMatched==tested && motionMatched==tested && restored==sources;
    std::printf("NATIVE_GUIDES sources=%u pixels=%llu colorAlphaExact=%llu depthExact=%llu motionExact=%llu stateRestored=%u gpuReadbackComplete=true\n",sources,
        (unsigned long long)tested,(unsigned long long)colorMatched,(unsigned long long)depthMatched,(unsigned long long)motionMatched,restored);
    std::puts(passed?"PASS NativeSourcePreparation_NotSrNrFgIntegration":"FAIL NativeSourcePreparation");return passed?0:1;
}catch(HRESULT error){std::printf("GPU failure 0x%08x\n",unsigned(error));return 1;}catch(...){return 1;}}
