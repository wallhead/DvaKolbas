#include "NeuralRendering/PostUpscale.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <cstdio>
#include <vector>
#include <cstring>
#if defined(TRP_POSTSR_FSR_SOURCE)
#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRFrameAdapter.h"
#include "FrameGen/FSRPresentationTransport.h"
#include <DirectXMath.h>
#include <cstring>
#endif
using namespace TheosRenderPipeline;
using namespace NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
int failures{};
void Check(bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);failures+=!ok;}
void Need(HRESULT result){if(FAILED(result))throw result;}
}
int wmain(int argc,wchar_t** argv){try{
    std::setvbuf(stdout,nullptr,_IONBF,0);
#if defined(TRP_POSTSR_FSR_SOURCE)
    if(argc!=4)return 1;
#else
    if(argc!=3)return 1;
#endif
    if(NrRuntimeResearch::GameRunningOrUnknown()){std::puts("REFUSED: Skyrim running or inventory unavailable");return 1;}
    PostUpscale post;SettingsSnapshot settings;settings.enabled=false;settings.revision=1;settings.placement=Placement::After;settings.stableColors=false;
    Check(bool(post.Evaluate({},settings)),"DisabledPostSourceRequiresNoRuntimeOrDecoder");
    ComPtr<IDXGIFactory6> factory;Need(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 desc{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> candidate;const auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));
        if(hr==DXGI_ERROR_NOT_FOUND)break;Need(hr);Need(candidate->GetDesc1(&desc));if(desc.VendorId==0x10de&&desc.DeviceId==0x2702){adapter=candidate;break;}}
    if(!adapter){std::puts("REFUSED: qualified RTX40 adapter unavailable");return 1;}
    std::printf("ADAPTER vendor=0x%04x device=0x%04x luidLow=%u luidHigh=%d\n",desc.VendorId,desc.DeviceId,desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart);
    StageContract contract;contract.colorExtent=contract.guideExtent={320,180};contract.adapterLuid={desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart};
    Need(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&contract.device)));
    D3D12_COMMAND_QUEUE_DESC queue{};Need(contract.device->CreateCommandQueue(&queue,IID_PPV_ARGS(&contract.queue)));
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    Need(D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context));
#if defined(TRP_POSTSR_FSR_SOURCE)
    Upscaling::FsrHostResources sr(std::filesystem::absolute(argv[3]));
    Upscaling::BackendConfiguration configuration;configuration.backend=Upscaling::BackendKind::Fsr;
    configuration.quality=Upscaling::Quality::NativeAA;configuration.generationEnabled=false;configuration.generationBackend=0;
    auto sizing=sr.PrepareSizing(device.Get(),configuration,{320,180},DXGI_FORMAT_R8G8B8A8_UNORM,Upscaling::ColorEncoding::Gamma22);
    if(!sizing){std::printf("FAIL FSR sizing: %s\n",sizing.error().message.c_str());return 1;}
    if(*sizing!=Upscaling::Extent{320,180})return 1;
    auto started=sr.CompleteStartup();if(!started){std::printf("FAIL FSR startup: %s\n",started.error().message.c_str());return 1;}
    contract.device=sr.Bridge()->Device12();contract.queue=sr.Bridge()->Queue();
    Upscaling::FsrFrameAdapter reconstruction(*sr.Upscaler(),sr.Bridge(),sr.Resources(),sr.Color11(),sr.Depth11(),sr.Motion11(),sr.Output11(),sr.HandoffEncoding());
    FsrPresentationTransport foreground;Need(foreground.Initialize(sr.Bridge(),{320,180}));
    std::printf("ACTUAL_FSR provider=%s id=%llu encoding=Gamma22 quality=NativeAA\n",sr.Provider().name.c_str(),(unsigned long long)sr.Provider().id);
#endif
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-postsr-cache"),true});
    auto opened=owner->Open(RuntimeCatalog()[1],contract.device.Get(),{desc.VendorId,desc.DeviceId,desc.SubSysId,contract.adapterLuid,false});
    if(!opened){std::puts(opened.error().message.c_str());return 1;}
    auto initialized=post.Initialize(owner,device.Get(),contract);if(!initialized){std::puts(initialized.error().message.c_str());return 1;}
    const auto texture=[&](DXGI_FORMAT format,UINT bind,bool staging=false){D3D11_TEXTURE2D_DESC d{};
        d.Width=320;d.Height=180;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=format;d.BindFlags=staging?0:bind;
        d.Usage=staging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;d.CPUAccessFlags=staging?D3D11_CPU_ACCESS_READ:0;
        ComPtr<ID3D11Texture2D> result;Need(device->CreateTexture2D(&d,nullptr,&result));return result;};
    auto color=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
    auto depth=texture(DXGI_FORMAT_R32_TYPELESS,D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE);
    auto motion=texture(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
    auto readback=texture(DXGI_FORMAT_R8G8B8A8_UNORM,0,true);
    ComPtr<ID3D11DepthStencilView> dsv;D3D11_DEPTH_STENCIL_VIEW_DESC depthView{};depthView.Format=DXGI_FORMAT_D32_FLOAT;depthView.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2D;
    Need(device->CreateDepthStencilView(depth.Get(),&depthView,&dsv));context->ClearDepthStencilView(dsv.Get(),D3D11_CLEAR_DEPTH,.5f,0);
    std::vector<unsigned short> velocity(320*180*2);context->UpdateSubresource(motion.Get(),0,nullptr,velocity.data(),320*4,0);
    const auto pixels=[&](ID3D11Texture2D* texture){
        D3D11_TEXTURE2D_DESC shape{};texture->GetDesc(&shape);const unsigned pixelBytes=shape.Format==DXGI_FORMAT_R16G16B16A16_FLOAT?8:4;
        ComPtr<ID3D11Texture2D> staging=readback;
        if(pixelBytes!=4){shape.Usage=D3D11_USAGE_STAGING;shape.BindFlags=shape.MiscFlags=0;shape.CPUAccessFlags=D3D11_CPU_ACCESS_READ;Need(device->CreateTexture2D(&shape,nullptr,&staging));}
        std::vector<unsigned char> result(320*180*pixelBytes);context->CopyResource(staging.Get(),texture);
        D3D11_MAPPED_SUBRESOURCE mapped{};Need(context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped));
        for(unsigned y=0;y<180;++y)std::memcpy(result.data()+y*320*pixelBytes,static_cast<const unsigned char*>(mapped.pData)+y*mapped.RowPitch,320*pixelBytes);
        context->Unmap(staging.Get(),0);return result;};
#if defined(TRP_POSTSR_FSR_SOURCE)
    auto source=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
    auto reference=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
    auto ui=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
    std::vector<unsigned char> hud(320*180*4);for(size_t i=0;i<320*180;++i){hud[i*4]=(i%320<20)?255:0;hud[i*4+1]=(i%320>=20&&i%320<40)?128:0;hud[i*4+3]=(i%320<20)?255:(i%320<40)?128:0;}
    context->UpdateSubresource(ui.Get(),0,nullptr,hud.data(),320*4,0);
    Graphics::SharedTexture fgRead;D3D11_TEXTURE2D_DESC readDesc{};color->GetDesc(&readDesc);Need(sr.Bridge()->CreateSharedTexture(readDesc,fgRead));
    Upscaling::FsrColorConverter referenceEncoder;
    Upscaling::UpscaleFrame real;real.backend=Upscaling::BackendKind::Fsr;real.color=real.input=source.Get();real.output=color.Get();
    real.depth=depth.Get();real.motion=motion.Get();real.render=real.subrect=real.display={320,180};real.motionConvention={320,180,true,false};real.deltaMilliseconds=1000.f/60;
    real.camera.identity=1;real.camera.nearDistance=.1f;real.camera.farDistance=100;real.camera.verticalFovRadians=1;
    DirectX::XMFLOAT4X4 view,projection;DirectX::XMStoreFloat4x4(&view,DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&projection,DirectX::XMMatrixPerspectiveFovLH(1,320.f/180,.1f,100));
    std::memcpy(real.camera.view.data(),&view,64);std::memcpy(real.camera.projection.data(),&projection,64);
    uint64_t fgExact{},hudExact{},srUntouched{};unsigned srFrames{};
#endif
    PostSrInput input;auto& native=input.resources;auto& metadata=input.source;
    native.context=context;native.color=color;native.depth=depth;native.motion=motion;native.colorExtent=native.guideExtent={320,180};
    native.epoch=native.guideEpoch=1;native.sourceId=native.guideSourceId=1;native.presentationTime=1./60.;native.motionScaleX=320;native.motionScaleY=180;native.colorDomain=ColorDomain::SdrBytes;
    metadata.backend=Upscaling::BackendKind::Dlaa;
#if defined(TRP_POSTSR_FSR_SOURCE)
    metadata.backend=Upscaling::BackendKind::Fsr;
#endif
    metadata.outcome=Upscaling::UpscaleOutcome::Temporal;
    metadata.epoch=metadata.guideEpoch=1;metadata.sourceId=metadata.guideSourceId=1;metadata.sourceTime=metadata.guideTime=native.presentationTime;
    metadata.render=metadata.display=metadata.color=metadata.guides={320,180};metadata.colorDomain=ColorDomain::SdrBytes;metadata.encoding=Upscaling::ColorEncoding::Gamma22;
    metadata.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;metadata.depthFormat=DXGI_FORMAT_R32_FLOAT;metadata.motionFormat=DXGI_FORMAT_R16G16_FLOAT;metadata.guideOrigin=GuideOrigin::RealSource;metadata.motion={320,180,true,false};
    settings.enabled=true;
    auto stale=input;stale.source.guideSourceId=0;auto rejected=post.Evaluate(stale,settings);
    Check(!rejected && rejected.error().kind==ErrorKind::InvalidInput && post.Diagnostics().evaluate==0,"StalePostGuidesRejectedBeforeVendorWork");
    stale=input;stale.resources.sourceId=2;rejected=post.Evaluate(stale,settings);
    Check(!rejected && rejected.error().kind==ErrorKind::InvalidInput && post.Diagnostics().evaluate==0,"PostMetadataMustBindActualResourceSource");
    stale=input;stale.resources.depthInverted=true;rejected=post.Evaluate(stale,settings);
    Check(!rejected && rejected.error().kind==ErrorKind::InvalidInput && post.Diagnostics().evaluate==0,"PostDepthConventionMustBindNativeInput");
    auto wrongPlacement=settings;wrongPlacement.placement=Placement::Before;
    Check(!post.Evaluate(input,wrongPlacement) && post.Diagnostics().evaluate==0,"PostAdapterCannotSilentlyRunBefore");
    uint64_t alpha{},changed{},bypass{};unsigned resets{};
    std::vector<unsigned char> bytes(320*180*4);
    for(unsigned frame=0;frame<240;++frame){
        for(size_t i=0;i<320*180;++i){bytes[4*i]=((i/8+frame)%2)?48:208;bytes[4*i+1]=(i+frame)%256;bytes[4*i+2]=(i/320+frame)%256;bytes[4*i+3]=(i+frame)%256;}
        context->UpdateSubresource(color.Get(),0,nullptr,bytes.data(),320*4,0);
        native.sourceId=native.guideSourceId=metadata.sourceId=metadata.guideSourceId=frame+1;
        native.previousSourceId=metadata.previousSourceId=frame;
        native.presentationTime=metadata.sourceTime=metadata.guideTime=double(frame+1)/60.;
#if defined(TRP_POSTSR_FSR_SOURCE)
        Need(foreground.WaitBeforeProducer());context->CopyResource(source.Get(),color.Get());
        real.sourceId=frame+1;real.sourceEpoch=1;real.reset=frame==0 || frame==80 || frame==81;
        auto jitter=sr.Upscaler()->QueryJitter(real.sourceId);if(!jitter)return 1;real.jitterX=(*jitter)[0];real.jitterY=(*jitter)[1];
        auto reconstructed=reconstruction.Evaluate(real);
        if(!reconstructed || *reconstructed!=Upscaling::UpscaleOutcome::Temporal){std::puts("FAIL actual FSR did not complete temporal source");return 1;}
        ++srFrames;bytes=pixels(color.Get());auto retainedSr=pixels(sr.Output11());
#endif
        settings.enabled=frame!=80;settings.revision=frame<80?1:frame==80?2:3;
        const auto output=post.Evaluate(input,settings);
        if(!output){std::printf("FAIL SourceAfter frame=%u error=%s\n",frame,output.error().message.c_str());return 1;}
        resets+=output->evaluated && output->effectiveReset;
        if(output->evaluated && !post.WaitDelivery(*output))return 1;
        context->CopyResource(readback.Get(),color.Get());D3D11_MAPPED_SUBRESOURCE map{};Need(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&map));
        for(unsigned y=0;y<180;++y){const auto* row=static_cast<const unsigned char*>(map.pData)+y*map.RowPitch;
            for(unsigned x=0;x<320;++x){const auto p=(size_t(y)*320+x)*4;alpha+=row[x*4+3]==bytes[p+3];
                const bool same=row[x*4]==bytes[p] && row[x*4+1]==bytes[p+1] && row[x*4+2]==bytes[p+2];changed+=!same;
                bypass+=!settings.enabled && same && row[x*4+3]==bytes[p+3];}}
        context->Unmap(readback.Get(),0);
#if defined(TRP_POSTSR_FSR_SOURCE)
        // Snapshot the real SDK source resource supplied by FG Configure, not
        // just the visible D3D11 output. No generated-output claim is made here.
        Need(referenceEncoder.Convert(context.Get(),color.Get(),reference.Get(),Upscaling::ColorEncoding::Gamma22,Upscaling::ColorEncoding::SRGB));
        auto expected=pixels(reference.Get());
        Need(foreground.Upload(color.Get(),Upscaling::ColorEncoding::Gamma22,ui.Get(),nullptr,true,real.sourceId));
        Need(sr.Bridge()->WaitD3D12(Graphics::InteropWork::SwapChain));ID3D12GraphicsCommandList* list{};
        Need(sr.Bridge()->Begin(Graphics::InteropWork::FrameGeneration,&list));
        Need(Graphics::D3D11D3D12Interop::RecordCopy(list,foreground.Resources().scene,fgRead.texture12.Get()));
        Need(sr.Bridge()->Submit(Graphics::InteropWork::FrameGeneration));Need(sr.Bridge()->WaitD3D11(Graphics::InteropWork::FrameGeneration));
        auto actual=pixels(fgRead.texture11.Get());
        for(size_t i=0;i<actual.size();++i)fgExact+=actual[i]==expected[i];
        hudExact+=pixels(ui.Get())==hud;srUntouched+=pixels(sr.Output11())==retainedSr;
#endif
    }
    Check(alpha==320ull*180*240,"PostSourcePreservesEveryAlphaByte");
    Check(changed>320*180,"PostSourceNrRgbOutputObserved");
    Check(bypass==320*180,"DisabledPostSourceLeavesEveryByteUnchanged");
    Check(post.Diagnostics().recorded==239,"PostSourceOnePassPerEnabledRealSource");
    Check(resets==2,"PostSourceHistoryResetsOnFirstAndReenable");
    Check(bool(post.Retire()) && bool(owner->Retire()),"PostSourceReadersAndRuntimeRetire");
    std::printf("POST_SOURCE frames=240 nrEvaluations=239 alphaExact=%llu changedRgbPixels=%llu bypassExact=%llu resets=%u\n",(unsigned long long)alpha,(unsigned long long)changed,(unsigned long long)bypass,resets);
#if defined(TRP_POSTSR_FSR_SOURCE)
    Check(srFrames==240 && fgExact==320ull*180*4*240,"ActualFsrEnhancedFgSourceReadbackExactEveryByte");
    Check(hudExact==240 && srUntouched==240,"NativeHudAndSrRetainedOutputNeverReceiveNrFeedback");
    Check(SUCCEEDED(foreground.Retire()) && bool(sr.Retire()),"ActualFsrSourceTransportAndReadersRetire");
    std::printf("FSR_NR_SOURCE srFrames=%u fgInputBytesExact=%llu unchangedHudFrames=%llu untouchedSrFrames=%llu\n",srFrames,(unsigned long long)fgExact,(unsigned long long)hudExact,(unsigned long long)srUntouched);
    std::puts("SCOPE actual pinned FSR Native AA -> actual NR -> actual FG source upload; generation/present and Skyrim NOT RUN");
#else
    std::puts("SCOPE source adapter only; actual SR/FG provider handoff NOT RUN");
#endif
    return failures?1:0;
}catch(HRESULT error){std::printf("GPU failure 0x%08x\n",unsigned(error));return 1;}catch(const std::exception& error){std::printf("Exception: %s\n",error.what());return 1;}catch(...){std::puts("Unknown fixture exception");return 1;}}
