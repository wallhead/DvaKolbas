#include "NeuralRendering/BeforeHost.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <cstdio>
#include <vector>
#include <fstream>
#include "ProbeBuildIdentity.h"
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {int failures{};void Check(bool v,const char* name){std::printf("%s %s\n",v?"PASS":"FAIL",name);failures+=!v;}void Need(HRESULT h){if(FAILED(h))throw h;}}
int wmain(int argc,wchar_t** argv){try{
    if((argc!=3&&argc!=4)||!std::filesystem::exists(argv[1])||!std::filesystem::exists(argv[2]))return 77;
    if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
    BeforeHost prepared;SettingsSnapshot s;s.enabled=false;s.revision=1;
    Check(bool(prepared.Evaluate({},s)),"DisabledPreparedNrRequiresNoColorDecoder");
    ComPtr<IDXGIFactory6> f;Need(CreateDXGIFactory2(0,IID_PPV_ARGS(&f)));ComPtr<IDXGIAdapter1> a;DXGI_ADAPTER_DESC1 d{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> v;const auto hr=f->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&v));if(hr==DXGI_ERROR_NOT_FOUND)break;Need(hr);Need(v->GetDesc1(&d));if(d.VendorId==0x10de&&d.DeviceId==0x2702){a=v;break;}}
    if(!a)return 77;StageContract c;c.colorExtent=c.guideExtent={320,180};c.adapterLuid={d.AdapterLuid.LowPart,d.AdapterLuid.HighPart};
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;Need(D3D11CreateDevice(a.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context));
    StartupSettings startup;startup.community=true;startup.runtimeRoot=std::filesystem::absolute(argv[1]);startup.driverCore=argv[2];startup.sourceEncoding=TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22;
    auto inspected=prepared.Inspect(device.Get(),startup,std::filesystem::absolute("nr-host-frames-cache"));
    if(!inspected){std::puts(inspected.error().message.c_str());return 1;}
    Check(prepared.Available() && prepared.Recorded()==0 && !GetModuleHandleW(L"nvngx_dlssnr.dll"),"InspectedHostDoesNotInitializeVendorBeforeSource");
    unsigned width=320,height=180;
    const auto texture=[&](DXGI_FORMAT format,UINT bind,D3D11_USAGE usage=D3D11_USAGE_DEFAULT){D3D11_TEXTURE2D_DESC t{};t.Width=width;t.Height=height;t.ArraySize=t.MipLevels=t.SampleDesc.Count=1;t.Format=format;t.Usage=usage;t.BindFlags=bind;t.CPUAccessFlags=usage==D3D11_USAGE_STAGING?D3D11_CPU_ACCESS_READ:0;ComPtr<ID3D11Texture2D> v;Need(device->CreateTexture2D(&t,nullptr,&v));return v;};
    auto color=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
    auto depth=texture(DXGI_FORMAT_R32_TYPELESS,D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE);
    auto motion=texture(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
    auto readback=texture(DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_USAGE_STAGING);
    ComPtr<ID3D11RenderTargetView> rtv;Need(device->CreateRenderTargetView(color.Get(),nullptr,&rtv));
    ComPtr<ID3D11DepthStencilView> dsv;D3D11_DEPTH_STENCIL_VIEW_DESC view{};view.Format=DXGI_FORMAT_D32_FLOAT;view.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2D;Need(device->CreateDepthStencilView(depth.Get(),&view,&dsv));context->ClearDepthStencilView(dsv.Get(),D3D11_CLEAR_DEPTH,.5f,0);
    std::vector<unsigned short> velocities(320*180*2);context->UpdateSubresource(motion.Get(),0,nullptr,velocities.data(),320*4,0);
    BeforeInput input;input.context=context;input.color=color;input.depth=depth;input.motion=motion;input.colorExtent=input.guideExtent={320,180};input.epoch=input.guideEpoch=1;input.sourceId=input.guideSourceId=1;input.presentationTime=1;input.motionScaleX=320;input.motionScaleY=180;
    uint64_t alpha{},changed{},bypassPixels{};std::vector<unsigned char> pixels(320*180*4);
    uint64_t expectedAlpha{},expectedNr{};
    for(UINT frame=0;frame<240;++frame){
        if(frame==120){
            context->OMSetRenderTargets(0,nullptr,nullptr);width=160;height=90;
            color=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
            depth=texture(DXGI_FORMAT_R32_TYPELESS,D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE);
            motion=texture(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
            readback=texture(DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_USAGE_STAGING);
            rtv.Reset();dsv.Reset();Need(device->CreateRenderTargetView(color.Get(),nullptr,&rtv));Need(device->CreateDepthStencilView(depth.Get(),&view,&dsv));
            context->ClearDepthStencilView(dsv.Get(),D3D11_CLEAR_DEPTH,.5f,0);
            velocities.assign(width*height*2,0);context->UpdateSubresource(motion.Get(),0,nullptr,velocities.data(),width*4,0);
            input.color=color;input.depth=depth;input.motion=motion;input.colorExtent=input.guideExtent={width,height};input.motionScaleX=float(width);input.motionScaleY=float(height);
            pixels.resize(width*height*4);
        }
        expectedAlpha+=uint64_t(width)*height;expectedNr+=frame!=80;
for(UINT y=0;y<height;++y)for(UINT x=0;x<width;++x){const auto p=(y*width+x)*4;pixels[p]=((x/8+y/8+frame)%2)?48:208;pixels[p+1]=(x+frame)%256;pixels[p+2]=(y+frame)%256;pixels[p+3]=(x+y+frame)%256;}
        context->UpdateSubresource(color.Get(),0,nullptr,pixels.data(),width*4,0);
        input.sourceId=input.guideSourceId=frame+1;input.previousSourceId=frame;input.presentationTime=double(frame+1)/60.;s.enabled=frame!=80;s.revision=frame<80?1:frame==80?2:3;
        // Keep caller RTV bound: preparation must isolate and restore it.
        auto* target=rtv.Get();context->OMSetRenderTargets(1,&target,nullptr);
        auto result=prepared.Evaluate(input,s);
        if(!result){std::printf("frame %u %s\n",frame,result.error().message.c_str());return 1;}
        Check(result->evaluated==s.enabled,"OneNrEvaluationOnlyWhenEnabled");
        if(frame==0 || frame==81 || frame==120) Check(result->effectiveReset,"FirstReenabledAndResizedSourceResetHistory");
        ComPtr<ID3D11RenderTargetView> restored;context->OMGetRenderTargets(1,&restored,nullptr);if(restored.Get()!=rtv.Get())return 1;
        context->OMSetRenderTargets(0,nullptr,nullptr);context->CopyResource(readback.Get(),color.Get());D3D11_MAPPED_SUBRESOURCE m{};Need(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&m));
        for(UINT y=0;y<height;++y){const auto* row=static_cast<const unsigned char*>(m.pData)+y*m.RowPitch;for(UINT x=0;x<width;++x){const auto p=(y*width+x)*4;alpha+=row[x*4+3]==pixels[p+3];if(row[x*4]!=pixels[p]||row[x*4+1]!=pixels[p+1]||row[x*4+2]!=pixels[p+2]){if(!s.enabled)throw HRESULT(E_FAIL);++changed;}}}
        context->Unmap(readback.Get(),0);
        if(!s.enabled)bypassPixels+=uint64_t(width)*height;
    }
    Check(bypassPixels==320ull*180,"OneSourceBypassedWithoutNr");
    Check(alpha==expectedAlpha,"AllUnormAlphaValuesSurviveDecodeNrEncode");Check(changed>320*180,"NrModifiedRgbDeliveredToOriginalSdrColor");
    Check(prepared.Recorded()==expectedNr,"PreparedNrOffOnUsesOnePassPerEnabledSource");
    Check(bool(prepared.Retire()) && !prepared.Terminal(),"PreparedReadersAndRuntimeRetire");
    if(argc==4){std::ofstream report{std::filesystem::path(argv[3])};
        report << "{\n\"schema\":1,\"scope\":\"synthetic native SDR real-source host; game quality pending\",\n\"result\":\"" << (failures?"FAIL":"PASS")
            << "\",\"sourceRevision\":\"" << NrRuntimeResearch::buildRevision << "\",\"sourceClean\":" << (NrRuntimeResearch::buildClean?"true":"false")
            << ",\n\"profile\":\"rtx40\",\"vendorId\":" << d.VendorId << ",\"deviceId\":" << d.DeviceId
            << ",\"adapterLuidLow\":" << d.AdapterLuid.LowPart << ",\"adapterLuidHigh\":" << d.AdapterLuid.HighPart
            << ",\"frames\":240,\"nrEvaluations\":" << expectedNr << ",\"resizes\":1,\"sourceAlphaPixels\":" << alpha
            << ",\"changedRgbPixels\":" << changed << ",\"bypassedSourcePixels\":" << bypassPixels
            << ",\"runtimeSha256\":\"" << RuntimeCatalog()[1].sha256 << "\",\"driverCoreSha256\":\"" << QualifiedProbeDriverCore().sha256
            << "\",\"retired\":true,\n\"compiledSourcesSha256\":" << NrRuntimeResearch::compiledSourcesJson << "\n}\n";
        if(!report)return 1;
    }
    std::printf("BEFORE_HOST_NR frames=240 resize=1 alpha=%llu changed=%llu\n",(unsigned long long)alpha,(unsigned long long)changed);return failures?1:0;
}catch(...){return 1;}}
