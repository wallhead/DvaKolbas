#include "NeuralRendering/BeforeHost.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <DirectXPackedVector.h>
#include <dxgi1_6.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
constexpr unsigned width=640,height=360,frames=160;
void Need(HRESULT hr){if(FAILED(hr))throw hr;}
// Fixed surface; only its surroundings change between four 40-source phases.
void Source(std::vector<unsigned char>& pixels,unsigned phase){
    for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x){
        const auto i=(size_t(y)*width+x)*4;
        const bool surface=x>=160&&x<480&&y>=80&&y<280;
        const auto texture=std::sin(x*.075)+std::cos(y*.061)+.3*std::sin((x+y)*.29);
        if(surface){
            pixels[i]=static_cast<unsigned char>(std::clamp(110+30*texture,16.,239.));
            pixels[i+1]=static_cast<unsigned char>(std::clamp(100+25*texture,16.,239.));
            pixels[i+2]=static_cast<unsigned char>(std::clamp(85+20*texture,16.,239.));
        }else{
            const unsigned char backgrounds[4][3]={{16,16,16},{235,235,235},{25,70,230},{16,16,16}};
            for(unsigned c=0;c<3;++c)pixels[i+c]=backgrounds[phase][c];
        }
        pixels[i+3]=static_cast<unsigned char>(64+(x+y)%192);
    }
}
void Ppm(const std::filesystem::path& path,const std::vector<unsigned char>& pixels){
    std::ofstream file(path,std::ios::binary);file<<"P6\n"<<width<<" "<<height<<"\n255\n";
    for(size_t i=0;i<pixels.size();i+=4)file.write(reinterpret_cast<const char*>(pixels.data()+i),3);
    if(!file)throw HRESULT(E_FAIL);
}
}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=5)return 2;
    if(NrRuntimeResearch::GameRunningOrUnknown()){
        std::puts("BLOCKED Skyrim is running or process enumeration failed");return 1;
    }
    const std::wstring mode=argv[4];
    if(mode!=L"stable"&&mode!=L"vendor"&&mode!=L"toggle"&&mode!=L"off")return 2;
    const auto output=std::filesystem::absolute(argv[3]);std::filesystem::create_directories(output);
    ComPtr<IDXGIFactory6> factory;Need(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 desc{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> candidate;
        const auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));
        if(hr==DXGI_ERROR_NOT_FOUND)break;Need(hr);Need(candidate->GetDesc1(&desc));
        if(desc.VendorId==0x10de&&desc.DeviceId==0x2702){adapter=candidate;break;}
    }
    if(!adapter){std::puts("UNSUPPORTED probe requires qualified RTX 4080 SUPER");return 77;}
    std::printf("ADAPTER vendor=0x%04X device=0x%04X luid=%u:%d panStep=%u\n",desc.VendorId,desc.DeviceId,desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart,0u);
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
    settings.stableColors=mode==L"stable"||mode==L"toggle";settings.tuning.localToneStrength=1;
    BeforeInput input;input.context=context;input.color=color;input.depth=depth;input.motion=motion;
    input.colorExtent=input.guideExtent={width,height};input.epoch=input.guideEpoch=1;
    input.motionScaleX=float(width);input.motionScaleY=float(height);
    std::vector<unsigned char> source(size_t(width)*height*4),result(source.size()),previous;
    std::vector<unsigned short> vectors(size_t(width)*height*2);
    std::ofstream csv(output/"frames.csv");csv<<"source,panPixels,motionPixels,nrEvaluated,reset,meanRgbCorrection,warpedMeanRgbDifference,meanR,meanG,meanB,sourceR,sourceG,sourceB\n";
    unsigned evaluationCount{},resetCount{};double settled[4][3]{};
    for(unsigned frame=0;frame<frames;++frame){
        input.reset=false;
        if(mode==L"toggle"&&frame&&frame%40==0){settings.stableColors=!settings.stableColors;++settings.revision;input.reset=true;}
        Source(source,frame/40);context->UpdateSubresource(color.Get(),0,nullptr,source.data(),width*4,0);
        const float guide=0.f;
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
        for(size_t i=3;i<result.size();i+=4)if(result[i]!=source[i]){std::puts("FAIL source alpha changed");return 1;}
        double correction{},warped{},means[3]{},sourceMeans[3]{};uint64_t samples{};
        // Ignore exposed borders when comparing the same surface point.
        for(unsigned y=120;y<240;++y)for(unsigned x=220;x<420;++x){
            const auto i=(size_t(y)*width+x)*4;
            if(result[i+3]!=source[i+3]){std::puts("FAIL source alpha changed");return 1;}
            for(unsigned c=0;c<3;++c){means[c]+=result[i+c];sourceMeans[c]+=source[i+c];correction+=std::abs(int(result[i+c])-int(source[i+c]));
                if(!previous.empty())warped+=std::abs(int(result[i+c])-int(previous[i+c]));++samples;}
        }
        correction/=samples;warped/=samples;
        csv<<frame+1<<','<<0<<','<<guide<<','<<evaluated->evaluated<<','<<evaluated->effectiveReset<<','<<correction<<','<<warped;for(auto value:means)csv<<','<<value/(samples/3);for(auto value:sourceMeans)csv<<','<<value/(samples/3);csv<<'\n';
        if(frame==39||frame==79||frame==119||frame==159){
            Ppm(output/("source-"+std::to_string(frame+1)+".ppm"),source);
            Ppm(output/("nr-"+std::to_string(frame+1)+".ppm"),result);
        }
        if(frame%40>=20)for(unsigned c=0;c<3;++c)settled[frame/40][c]+=means[c]/(samples/3)/20.;
        previous=result;
    }
    if(!csv||!host.Retire()){std::puts("FAIL receipt/retirement");return 1;}
    std::printf("SCENE_RECORDED sources=%u nrEvaluations=%u resets=%u; visual quality is not automatically accepted\n",frames,evaluationCount,resetCount);
    double maxDrift{};
    for(unsigned c=0;c<3;++c){double low=settled[0][c],high=low;
        for(const auto& phase:settled){low=std::min(low,phase[c]);high=std::max(high,phase[c]);}
        maxDrift=std::max(maxDrift,high-low);
    }
    bool ok=mode==L"vendor"?maxDrift>5.:maxDrift<=1.;
    if(mode==L"toggle"){
        double stableDrift{},vendorDifference{};
        for(unsigned c=0;c<3;++c){stableDrift=std::max(stableDrift,std::abs(settled[0][c]-settled[2][c]));
            vendorDifference=std::max(vendorDifference,std::abs(settled[1][c]-settled[0][c]));}
        ok=stableDrift<=1.&&vendorDifference>5.&&resetCount==4;
        std::printf("TOGGLE stable drift=%.6f vendor difference=%.6f resets=%u\n",stableDrift,vendorDifference,resetCount);
    }
    std::printf("%s fixed-surface context drift %.6f code values; threshold stable<=1 / vendor>5\n",ok?"PASS":"FAIL",maxDrift);
    return ok?0:1;
}catch(HRESULT hr){std::printf("FAIL HRESULT 0x%08X\n",unsigned(hr));return 1;}
catch(const std::exception& e){std::puts(e.what());return 1;}}


