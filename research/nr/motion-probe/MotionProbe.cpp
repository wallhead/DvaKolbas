#include "NeuralRendering/BeforeHost.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <DirectXPackedVector.h>
#include <dxgi1_6.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
constexpr unsigned width=640,height=360,frames=120;
void Need(HRESULT hr){if(FAILED(hr))throw hr;}
// A repeatable textured surface translated exactly two pixels per source.
// Current-to-previous motion is +2 pixels: a point at x was at x+2 previously.
void Source(std::vector<unsigned char>& pixels,unsigned pan){
    for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x){
        const auto u=x+pan;const auto i=(size_t(y)*width+x)*4;
        const auto texture=std::sin(u*.075)+std::cos(y*.061)+.3*std::sin((u+y)*.29);
        pixels[i]=static_cast<unsigned char>(std::clamp(110+45*texture,16.,239.));
        pixels[i+1]=static_cast<unsigned char>(std::clamp(100+35*texture+20*std::sin(u*.025),16.,239.));
        pixels[i+2]=static_cast<unsigned char>(std::clamp(85+30*texture+15*std::cos(y*.04),16.,239.));
        pixels[i+3]=255;
    }
}
void Ppm(const std::filesystem::path& path,const std::vector<unsigned char>& pixels){
    std::ofstream file(path,std::ios::binary);file<<"P6\n"<<width<<" "<<height<<"\n255\n";
    for(size_t i=0;i<pixels.size();i+=4)file.write(reinterpret_cast<const char*>(pixels.data()+i),3);
    if(!file)throw HRESULT(E_FAIL);
}
}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=5&&argc!=6)return 2;
    if(NrRuntimeResearch::GameRunningOrUnknown()){
        std::puts("BLOCKED Skyrim is running or process enumeration failed");return 1;
    }
    const std::wstring mode=argv[4];
    if(mode!=L"correct"&&mode!=L"reverse"&&mode!=L"zero"&&mode!=L"off")return 2;
    const unsigned panStep=argc==6?static_cast<unsigned>(std::stoul(argv[5])):2;
    if(!panStep||panStep>width/4)return 2;
    const auto output=std::filesystem::absolute(argv[3]);std::filesystem::create_directories(output);
    ComPtr<IDXGIFactory6> factory;Need(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 desc{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> candidate;
        const auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));
        if(hr==DXGI_ERROR_NOT_FOUND)break;Need(hr);Need(candidate->GetDesc1(&desc));
        if(desc.VendorId==0x10de&&desc.DeviceId==0x2702){adapter=candidate;break;}
    }
    if(!adapter){std::puts("UNSUPPORTED probe requires qualified RTX 4080 SUPER");return 77;}
    std::printf("ADAPTER vendor=0x%04X device=0x%04X luid=%u:%d panStep=%u\n",desc.VendorId,desc.DeviceId,desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart,panStep);
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    Need(D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context));
    const auto texture=[&](DXGI_FORMAT format,UINT bind,bool staging=false){
        D3D11_TEXTURE2D_DESC d{};d.Width=width;d.Height=height;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;
        d.Format=format;d.Usage=staging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;d.BindFlags=bind;
        d.CPUAccessFlags=staging?D3D11_CPU_ACCESS_READ:0;
        ComPtr<ID3D11Texture2D> result;Need(device->CreateTexture2D(&d,nullptr,&result));return result;
    };
    auto color=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
    auto depth=texture(DXGI_FORMAT_R32_TYPELESS,D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE);
    auto motion=texture(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
    auto readback=texture(DXGI_FORMAT_R8G8B8A8_UNORM,0,true);
    std::vector<float> depths(size_t(width)*height,.5f);
    context->UpdateSubresource(depth.Get(),0,nullptr,depths.data(),width*sizeof(float),0);
    StartupSettings startup;startup.community=true;startup.runtimeRoot=std::filesystem::absolute(argv[1]);startup.driverCore=argv[2];
    startup.sourceEncoding=TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22;
    BeforeHost host;auto inspected=host.Inspect(device.Get(),startup,output/"cache");
    if(!inspected){std::puts(inspected.error().message.c_str());return 1;}
    SettingsSnapshot settings;settings.enabled=mode!=L"off";settings.revision=1;
    BeforeInput input;input.context=context;input.color=color;input.depth=depth;input.motion=motion;
    input.colorExtent=input.guideExtent={width,height};input.epoch=input.guideEpoch=1;
    input.motionScaleX=float(width);input.motionScaleY=float(height);
    std::vector<unsigned char> source(size_t(width)*height*4),result(source.size()),previous;
    std::vector<unsigned short> vectors(size_t(width)*height*2);
    std::ofstream csv(output/"frames.csv");csv<<"source,panPixels,motionPixels,nrEvaluated,reset,meanRgbCorrection,warpedMeanRgbDifference\n";
    unsigned pan{},evaluationCount{},resetCount{};
    for(unsigned frame=0;frame<frames;++frame){
        const bool moving=frame>=40&&frame<80;const unsigned step=moving?panStep:0;pan+=step;
        Source(source,pan);context->UpdateSubresource(color.Get(),0,nullptr,source.data(),width*4,0);
        const float guide=mode==L"zero"?0.f:(mode==L"reverse"?-float(step):float(step));
        const auto half=DirectX::PackedVector::XMConvertFloatToHalf(guide/width);
        for(size_t i=0;i<vectors.size();i+=2){vectors[i]=half;vectors[i+1]=0;}
        context->UpdateSubresource(motion.Get(),0,nullptr,vectors.data(),width*4,0);
        input.sourceId=input.guideSourceId=frame+1;input.previousSourceId=frame;
        input.presentationTime=double(frame+1)/60.;
        auto evaluated=host.Evaluate(input,settings);
        if(!evaluated){std::puts(evaluated.error().message.c_str());return 1;}
        if(evaluated->evaluated!=settings.enabled){std::puts("FAIL unexpected NR bypass");return 1;}
        evaluationCount+=evaluated->evaluated;resetCount+=evaluated->effectiveReset;
        context->CopyResource(readback.Get(),color.Get());D3D11_MAPPED_SUBRESOURCE mapped{};
        Need(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped));
        for(unsigned y=0;y<height;++y)std::copy_n(static_cast<const unsigned char*>(mapped.pData)+y*mapped.RowPitch,width*4,result.data()+size_t(y)*width*4);
        context->Unmap(readback.Get(),0);
        double correction{},warped{};uint64_t samples{};
        // Ignore exposed borders when comparing the same surface point.
        for(unsigned y=24;y<height-24;++y)for(unsigned x=24;x<width-24-step;++x){
            const auto i=(size_t(y)*width+x)*4;
            if(result[i+3]!=255){std::puts("FAIL source alpha changed");return 1;}
            for(unsigned c=0;c<3;++c){correction+=std::abs(int(result[i+c])-int(source[i+c]));
                if(!previous.empty())warped+=std::abs(int(result[i+c])-int(previous[i+step*4+c]));++samples;}
        }
        correction/=samples;warped/=samples;
        csv<<frame+1<<','<<pan<<','<<guide<<','<<evaluated->evaluated<<','<<evaluated->effectiveReset<<','<<correction<<','<<warped<<'\n';
        if(frame==39||frame==59||frame==79||frame==99){
            Ppm(output/("source-"+std::to_string(frame+1)+".ppm"),source);
            Ppm(output/("nr-"+std::to_string(frame+1)+".ppm"),result);
        }
        previous=result;
    }
    if(!csv||!host.Retire()){std::puts("FAIL receipt/retirement");return 1;}
    std::printf("SCENE_RECORDED sources=%u nrEvaluations=%u resets=%u; visual quality is not automatically accepted\n",frames,evaluationCount,resetCount);
    return 0;
}catch(HRESULT hr){std::printf("FAIL HRESULT 0x%08X\n",unsigned(hr));return 1;}
catch(const std::exception& e){std::puts(e.what());return 1;}}
