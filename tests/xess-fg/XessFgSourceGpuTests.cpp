#include "FrameGen/XessGenerationCompletedSource.h"
#include "FrameGen/XessGenerationPresentation.h"
#include "FrameGen/XessGenerationPolicy.h"
#include "Upscaling/XessHostResources.h"
#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRFrameAdapter.h"
#include "Upscaling/SdrColorConversion.h"
#include "NeuralRendering/BeforeHost.h"
#include "XessFgVisibleScene.h"
#include "XessFgFrameFixture.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <chrono>
#include <thread>
#include <vector>
using namespace TheosRenderPipeline;
namespace U=Upscaling;
namespace N=NeuralRendering;
using namespace InteropFixture;
template<class T,class E> T Value(std::expected<T,E> result) {
    if(!result){std::fprintf(stderr,"NOT QUALIFIED: %s\n",result.error().message.c_str());std::exit(1);}return std::move(*result);
}
template<class E> void Accepted(std::expected<void,E> result) {
    if(!result)std::fprintf(stderr,"NOT QUALIFIED: %s\n",result.error().message.c_str());Require(bool(result),"operation accepted");
}
static ComPtr<ID3D11Texture2D> Texture(Rig& rig,U::Extent size,DXGI_FORMAT format) {
    auto d=rig.Description();d.Width=size.width;d.Height=size.height;d.Format=format;
    ComPtr<ID3D11Texture2D> result;Check(rig.device11->CreateTexture2D(&d,nullptr,&result),"source texture");return result;
}
static N::PostSrInput NrInput(Rig& rig,const U::UpscaleFrame& frame,ID3D11Texture2D* scene) {
    N::PostSrInput input;auto& r=input.resources;auto& s=input.source;
    r.context=rig.context11;r.color=scene;r.depth=frame.depth;r.motion=frame.motion;
    r.epoch=r.guideEpoch=frame.sourceEpoch;r.sourceId=r.guideSourceId=frame.sourceId;r.previousSourceId=frame.sourceId-1;
    r.presentationTime=frame.sourceId/30.;r.colorExtent={frame.display.width,frame.display.height};r.guideExtent={frame.render.width,frame.render.height};
    r.colorDomain=N::ColorDomain::SdrBytes;r.reset=frame.reset;
    s.backend=frame.backend;s.outcome=U::UpscaleOutcome::Temporal;s.epoch=s.guideEpoch=r.epoch;
    s.sourceId=s.guideSourceId=r.sourceId;s.previousSourceId=r.previousSourceId;s.sourceTime=s.guideTime=r.presentationTime;
    s.render=s.guides=r.guideExtent;s.display=s.color=r.colorExtent;s.colorDomain=N::ColorDomain::SdrBytes;s.encoding=U::ColorEncoding::Gamma22;
    s.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;s.depthFormat=DXGI_FORMAT_R32_FLOAT;s.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
    s.guideOrigin=N::GuideOrigin::RealSource;s.motion={float(frame.render.width),float(frame.render.height),true,false};
    Value(N::BindPostSrMotionScales(s,r.motionScaleX,r.motionScaleY));return input;
}
static void CheckHud(Interop& bridge,IDXGISwapChain4* chain) {
    ComPtr<ID3D12Resource> buffer;Check(chain->GetBuffer(chain->GetCurrentBackBufferIndex(),IID_PPV_ARGS(&buffer)),"published real buffer");
    const auto desc=buffer->GetDesc();D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};UINT64 bytes{};
    bridge.Device12()->GetCopyableFootprints(&desc,0,1,0,&footprint,nullptr,nullptr,&bytes);
    D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_READBACK;
    D3D12_RESOURCE_DESC read{};read.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;read.Width=bytes;
    read.Height=read.DepthOrArraySize=read.MipLevels=1;read.SampleDesc.Count=1;read.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    ComPtr<ID3D12Resource> data;Check(bridge.Device12()->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&read,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&data)),"HUD readback");
    ID3D12GraphicsCommandList* list{};Check(bridge.Begin(Work::SwapChain,&list),"HUD readback begin");
    D3D12_RESOURCE_BARRIER barrier{};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;barrier.Transition.pResource=buffer.Get();
    barrier.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;barrier.Transition.StateBefore=D3D12_RESOURCE_STATE_PRESENT;barrier.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    list->ResourceBarrier(1,&barrier);D3D12_TEXTURE_COPY_LOCATION from{},to{};
    from.pResource=buffer.Get();from.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    to.pResource=data.Get();to.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;to.PlacedFootprint=footprint;
    list->CopyTextureRegion(&to,0,0,0,&from,nullptr);std::swap(barrier.Transition.StateBefore,barrier.Transition.StateAfter);list->ResourceBarrier(1,&barrier);
    Check(bridge.Submit(Work::SwapChain),"HUD copy submit");Check(bridge.Drain(),"HUD copy drain");
    void* mapped{};Check(data->Map(0,nullptr,&mapped),"HUD pixel map");
    const auto pixel=*reinterpret_cast<const uint32_t*>(static_cast<const char*>(mapped)+footprint.Offset+16*footprint.Footprint.RowPitch+16*4);
    data->Unmap(0,nullptr);Require(pixel==0xff0000ffu,"opaque red HUD survives actual SR/NR real publication");
}
int wmain(int argc,wchar_t** argv) {
    std::setvbuf(stdout,nullptr,_IONBF,0);
    if(argc!=5 || NrRuntimeResearch::GameRunningOrUnknown()){std::puts("REFUSED: args or Skyrim running/inventory unknown");return 1;}
    Rig rig(true);DXGI_ADAPTER_DESC adapter{};Check(rig.adapter->GetDesc(&adapter),"actual adapter");
    if(adapter.VendorId!=0x10de){std::puts("NOT QUALIFIED: this NR matrix requires the actual NVIDIA model");return 77;}
    std::printf("ADAPTER vendor=%04x device=%04x LUID=%08lx:%08lx real_sdk=1\n",adapter.VendorId,adapter.DeviceId,adapter.AdapterLuid.HighPart,adapter.AdapterLuid.LowPart);
    const auto plugin=std::filesystem::absolute(argv[1]),root=std::filesystem::absolute(argv[2]),core=std::filesystem::absolute(argv[3]),fsrPlugin=std::filesystem::absolute(argv[4]);
    const U::Extent display{640,360};
    for(auto backend:{U::BackendKind::Xess,U::BackendKind::Fsr})for(auto quality:{U::Quality::NativeAA,U::Quality::Performance}) {
        U::XessHostResources xess(plugin);U::FsrHostResources fsr(fsrPlugin);U::Extent render{};
        if(backend==U::BackendKind::Xess)render=Value(xess.Initialize(rig.device11.Get(),quality,display,U::ColorEncoding::Gamma22));
        else {U::BackendConfiguration config;config.backend=backend;config.quality=quality;config.generationBackend=0;config.generationEnabled=false;
            render=Value(fsr.PrepareSizing(rig.device11.Get(),config,display,DXGI_FORMAT_R8G8B8A8_UNORM,U::ColorEncoding::Gamma22));Accepted(fsr.CompleteStartup());
            Require(fsr.Provider().name.find("3.1.5")!=std::string::npos,"actual analytical FSR 3.1.5, no ML fallback labelled FSR4");}
        auto bridge=backend==U::BackendKind::Xess?xess.Bridge():fsr.Bridge();
        auto color=Texture(rig,render,DXGI_FORMAT_R8G8B8A8_UNORM),depth=Texture(rig,render,DXGI_FORMAT_R32_FLOAT),motion=Texture(rig,render,DXGI_FORMAT_R16G16_FLOAT),hud=Texture(rig,display,DXGI_FORMAT_R8G8B8A8_UNORM);
        auto d=rig.Description();d.Width=display.width;d.Height=display.height;Graphics::SharedTexture encoded;Check(bridge->CreateSharedTexture(d,encoded),"shared completed scene");
        std::vector<float> z(size_t(render.width)*render.height,.99f);std::vector<uint32_t> velocities(z.size()),colors(z.size()),ui(size_t(display.width)*display.height);
        for(unsigned y=8;y<24;++y)for(unsigned x=8;x<24;++x)ui[size_t(y)*display.width+x]=0xff0000ffu;
        rig.context11->UpdateSubresource(depth.Get(),0,nullptr,z.data(),render.width*4,0);rig.context11->UpdateSubresource(motion.Get(),0,nullptr,velocities.data(),render.width*4,0);
        rig.context11->UpdateSubresource(hud.Get(),0,nullptr,ui.data(),display.width*4,0);
        std::unique_ptr<U::FsrFrameAdapter> fsrAdapter;
        if(backend==U::BackendKind::Fsr)fsrAdapter=std::make_unique<U::FsrFrameAdapter>(*fsr.Upscaler(),bridge,fsr.Resources(),fsr.Color11(),fsr.Depth11(),fsr.Motion11(),fsr.Output11(),U::ColorEncoding::Gamma22);
        N::BeforeHost nr;N::StartupSettings startup;startup.community=true;startup.runtimeRoot=root;startup.driverCore=core;startup.sourceEncoding=U::ColorEncoding::Gamma22;
        Accepted(nr.Inspect(rig.device11.Get(),startup,std::filesystem::absolute("intel-sr-nr-cache"),bridge->Device12()));
        auto runtime=Value(XessGenerationRuntime::Load(plugin));XessFgVisibleScene window;window.Create(display.width,display.height);
        DXGI_SWAP_CHAIN_DESC desc{};desc.OutputWindow=window.Handle();desc.Windowed=TRUE;desc.BufferDesc.Width=display.width;desc.BufferDesc.Height=display.height;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.SampleDesc.Count=1;desc.BufferCount=2;
        XessGenerationPresentation owner;Accepted(owner.Create(rig.factory.Get(),runtime,bridge,desc));Check(owner.StartupPresent(0,0),"Intel startup present");
        XessGenerationHistory history;U::SdrColorConverter encode;unsigned generated{},active{},before{},after{},nrSources{},errors{};
        for(unsigned index=0;index<56;++index) {
            Require(window.Pump(),"probe not interrupted");const unsigned phase=index/8;const uint32_t id=index+1;
            Accepted(owner.Latency()->BeginFrame(id));
            auto frame=XessFgFrame();frame.backend=backend;frame.render=frame.subrect=render;frame.display=display;
            frame.input=frame.color=color.Get();frame.output=encoded.texture11.Get();frame.depth=depth.Get();frame.motion=motion.Get();
            frame.sourceId=id;frame.sourceEpoch=phase+1;frame.reset=index%8==0;frame.deltaMilliseconds=1000.f/30;
            frame.motionConvention={float(render.width),float(render.height),true,false};frame.colorIsLinear=false;
            const auto jitter=backend==U::BackendKind::Xess?Value(xess.Upscaler()->QueryJitter(index)):Value(fsr.Upscaler()->QueryJitter(index));
            frame.jitterX=-jitter[0];frame.jitterY=-jitter[1];
            for(unsigned y=0;y<render.height;++y)for(unsigned x=0;x<render.width;++x){const uint32_t v=((x/12+y/12)%2)?150:60;colors[size_t(y)*render.width+x]=v|((v/2)<<8)|((v+40)<<16)|0xff000000u;}
            Accepted(owner.Latency()->Marker(id,XELL_SIMULATION_END));Accepted(owner.Latency()->Marker(id,XELL_RENDERSUBMIT_START));
            rig.context11->UpdateSubresource(color.Get(),0,nullptr,colors.data(),render.width*4,0);
            N::SettingsSnapshot settings;settings.enabled=phase!=0;settings.revision=phase+1;settings.tuning.style=1;
            settings.placement=phase<4?N::Placement::Before:N::Placement::After;settings.passes=phase==0?1:phase<4?phase:phase-3;
            auto input=NrInput(rig,frame,encoded.texture11.Get());
            if(phase>0 && phase<4){auto pre=input.resources;pre.color=color;pre.colorExtent=pre.guideExtent;pre.motionScaleX=float(render.width);pre.motionScaleY=float(render.height);pre.colorDomain=N::ColorDomain::Unknown;
                auto delivered=Value(nr.Evaluate(pre,settings));Require(delivered.evaluated,"actual Before NR evaluated");++before;++nrSources;}
            if(backend==U::BackendKind::Xess){Accepted(xess.PrepareInput(color.Get(),depth.Get(),motion.Get()));Check(bridge->SignalProducer(),"XeSS producer ready");
                ID3D12GraphicsCommandList* list{};Check(bridge->Begin(&list),"XeSS dispatch begin");auto dispatch=frame;dispatch.colorIsLinear=true;dispatch.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;
                Accepted(xess.Upscaler()->Dispatch(list,xess.Resources(),dispatch));Check(bridge->Submit(),"XeSS submit");Check(bridge->WaitConsumer(),"XeSS source retirement");
                Check(encode.Convert(rig.context11.Get(),xess.Output11(),encoded.texture11.Get(),U::ColorEncoding::Linear,U::ColorEncoding::Gamma22),"XeSS SDR scene encode");}
            else Require(Value(fsrAdapter->Evaluate(frame))==U::UpscaleOutcome::Temporal,"actual FSR temporal source");
            if(phase>=4){auto delivered=Value(nr.EvaluatePost(input,settings));Require(delivered.evaluated,"actual After NR evaluated");++after;++nrSources;}
            frame=Value(CompleteXessGenerationSource(frame,encoded.texture11.Get(),U::ColorEncoding::Gamma22));
            auto admission=history.Decide(frame,U::UpscaleOutcome::Temporal,true,true,false);frame.reset=admission.reset;
            auto prepared=Value(owner.Prepare(frame,hud.Get(),nullptr,true,id,admission.tag));
            Require(prepared.render==render && prepared.display==display && prepared.depth==render && prepared.motion==render,"actual copied source/guide extents");
            if(index%8==0)CheckHud(*bridge,owner.SwapChain());
            Accepted(owner.Latency()->Marker(id,XELL_RENDERSUBMIT_END));Accepted(owner.Latency()->Marker(id,XELL_PRESENT_START));
            Check(owner.Present(prepared,admission.generate,0,0),"actual Intel source Present");Accepted(owner.Latency()->Marker(id,XELL_PRESENT_END));
            if(admission.tag)history.Accept(frame.sourceId,frame.sourceEpoch);const auto status=owner.Status();
            if(admission.generate){++active;generated+=status.framesPresented==2 && int(status.frameGenResult)==0;}
            else Require(status.framesPresented<=1,"NR source transition resets FG");
            errors+=int(status.frameGenResult)<0;
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        Require(nrSources==48 && before==24 && after==24 && generated==active && active==49 && errors==0,"all NR passes precede actual Intel generation without vendor errors");
        Accepted(owner.Suspend());Accepted(owner.Retire());Accepted(nr.Retire());fsrAdapter.reset();
        if(backend==U::BackendKind::Xess)Accepted(xess.Retire());else Accepted(fsr.Retire());
        std::printf("PASS CASE backend=%d quality=%d render=%ux%u display=640x360 active=%u generated=%u nr_sources=%u before=%u after=%u errors=%u HUD_readbacks=7 clean_retirement=1\n",int(backend),int(quality),render.width,render.height,active,generated,nrSources,before,after,errors);
    }
    rig.ValidateDebug();std::puts("PASS actual XeSS/FSR315 Native/Performance -> NR off/Before/After 1..3 -> Intel FG -> HUD; DLSS and FSR4 actual coupling NOT RUN");return 0;
}
