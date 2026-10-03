#include "NeuralRendering/PreparedBeforeUpscale.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <cstdio>
#include <vector>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {int failures{};void Check(bool v,const char* name){std::printf("%s %s\n",v?"PASS":"FAIL",name);failures+=!v;}void Need(HRESULT h){if(FAILED(h))throw h;}}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=3||!std::filesystem::exists(argv[1])||!std::filesystem::exists(argv[2]))return 77;
    if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
    PreparedBeforeUpscale prepared;SettingsSnapshot s;s.enabled=false;s.revision=1;
    Check(bool(prepared.Evaluate({},TheosRenderPipeline::Upscaling::ColorEncoding::Unknown,s)),"DisabledPreparedNrRequiresNoColorDecoder");
    ComPtr<IDXGIFactory6> f;Need(CreateDXGIFactory2(0,IID_PPV_ARGS(&f)));ComPtr<IDXGIAdapter1> a;DXGI_ADAPTER_DESC1 d{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> v;const auto hr=f->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&v));if(hr==DXGI_ERROR_NOT_FOUND)break;Need(hr);Need(v->GetDesc1(&d));if(d.VendorId==0x10de&&d.DeviceId==0x2702){a=v;break;}}
    if(!a)return 77;StageContract c;c.colorExtent=c.guideExtent={320,180};c.adapterLuid={d.AdapterLuid.LowPart,d.AdapterLuid.HighPart};
    Need(D3D12CreateDevice(a.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&c.device)));D3D12_COMMAND_QUEUE_DESC q{};Need(c.device->CreateCommandQueue(&q,IID_PPV_ARGS(&c.queue)));
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;Need(D3D11CreateDevice(a.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context));
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-prepared-cache"),true});
    auto opened=owner->Open(RuntimeCatalog()[1],c.device.Get(),{d.VendorId,d.DeviceId,d.SubSysId,c.adapterLuid,false});if(!opened){std::puts(opened.error().message.c_str());return 1;}
    auto init=prepared.Initialize(owner,device.Get(),c);if(!init){std::puts(init.error().message.c_str());return 1;}
    const auto texture=[&](DXGI_FORMAT format,UINT bind,D3D11_USAGE usage=D3D11_USAGE_DEFAULT){D3D11_TEXTURE2D_DESC t{};t.Width=320;t.Height=180;t.ArraySize=t.MipLevels=t.SampleDesc.Count=1;t.Format=format;t.Usage=usage;t.BindFlags=bind;t.CPUAccessFlags=usage==D3D11_USAGE_STAGING?D3D11_CPU_ACCESS_READ:0;ComPtr<ID3D11Texture2D> v;Need(device->CreateTexture2D(&t,nullptr,&v));return v;};
    auto color=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
    auto depth=texture(DXGI_FORMAT_R32_TYPELESS,D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE);
    auto motion=texture(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
    auto readback=texture(DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_USAGE_STAGING);
    ComPtr<ID3D11RenderTargetView> rtv;Need(device->CreateRenderTargetView(color.Get(),nullptr,&rtv));
    ComPtr<ID3D11DepthStencilView> dsv;D3D11_DEPTH_STENCIL_VIEW_DESC view{};view.Format=DXGI_FORMAT_D32_FLOAT;view.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2D;Need(device->CreateDepthStencilView(depth.Get(),&view,&dsv));context->ClearDepthStencilView(dsv.Get(),D3D11_CLEAR_DEPTH,.5f,0);
    std::vector<unsigned short> velocities(320*180*2);context->UpdateSubresource(motion.Get(),0,nullptr,velocities.data(),320*4,0);
    BeforeInput input;input.context=context;input.color=color;input.depth=depth;input.motion=motion;input.colorExtent=input.guideExtent={320,180};input.epoch=input.guideEpoch=1;input.sourceId=input.guideSourceId=1;input.presentationTime=1;input.motionScaleX=320;input.motionScaleY=180;
    s.enabled=true;auto unknown=prepared.Evaluate(input,TheosRenderPipeline::Upscaling::ColorEncoding::Unknown,s);
    Check(!unknown&&prepared.Diagnostics().evaluate==0,"UnknownSourceEncodingRejectedWithoutNrWork");
    uint64_t alpha{},changed{};std::vector<unsigned char> pixels(320*180*4);
    for(UINT frame=0;frame<240;++frame){for(UINT y=0;y<180;++y)for(UINT x=0;x<320;++x){const auto p=(y*320+x)*4;pixels[p]=((x/8+y/8+frame)%2)?48:208;pixels[p+1]=(x+frame)%256;pixels[p+2]=(y+frame)%256;pixels[p+3]=(x+y+frame)%256;}
        context->UpdateSubresource(color.Get(),0,nullptr,pixels.data(),320*4,0);
        input.sourceId=input.guideSourceId=frame+1;input.previousSourceId=frame;input.presentationTime=double(frame+1)/60.;s.enabled=frame!=80;s.revision=frame<80?1:frame==80?2:3;
        // Keep caller RTV bound: preparation must isolate and restore it.
        auto* target=rtv.Get();context->OMSetRenderTargets(1,&target,nullptr);
        auto result=prepared.Evaluate(input,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,s);
        if(!result){std::printf("frame %u %s\n",frame,result.error().message.c_str());return 1;}
        if(result->evaluated&&!prepared.WaitDelivery(*result))return 1;
        ComPtr<ID3D11RenderTargetView> restored;context->OMGetRenderTargets(1,&restored,nullptr);if(restored.Get()!=rtv.Get())return 1;
        context->OMSetRenderTargets(0,nullptr,nullptr);context->CopyResource(readback.Get(),color.Get());D3D11_MAPPED_SUBRESOURCE m{};Need(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&m));
        for(UINT y=0;y<180;++y){const auto* row=static_cast<const unsigned char*>(m.pData)+y*m.RowPitch;for(UINT x=0;x<320;++x){const auto p=(y*320+x)*4;alpha+=row[x*4+3]==pixels[p+3];if(row[x*4]!=pixels[p]||row[x*4+1]!=pixels[p+1]||row[x*4+2]!=pixels[p+2])++changed;}}
        context->Unmap(readback.Get(),0);
    }
    Check(alpha==320ull*180*240,"AllUnormAlphaValuesSurviveDecodeNrEncode");Check(changed>320*180,"NrModifiedRgbDeliveredToOriginalSdrColor");
    Check(prepared.Diagnostics().recorded==239,"PreparedNrOffOnUsesOnePassPerEnabledSource");
    const auto pooled=prepared.Diagnostics();
    Check(pooled.preparedSlots==3&&pooled.bridgeSlots==3&&pooled.parameterOwners==3&&pooled.descriptorOwners==3&&!pooled.pendingTickets,"PreparedAndBridgeRetainThreeCompleteImageSlots");
    Check(bool(prepared.Retire())&&bool(owner->Retire()),"PreparedReadersAndRuntimeRetire");
    std::printf("PREPARED_NR frames=240 alpha=%llu changed=%llu\n",(unsigned long long)alpha,(unsigned long long)changed);return failures?1:0;
}catch(...){return 1;}}
