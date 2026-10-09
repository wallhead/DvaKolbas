#include "Upscaling/XessHostResources.h"
#include "Upscaling/SdrColorConversion.h"
#include "NeuralRendering/BeforeHost.h"
#include "InteropTestRig.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>
using namespace TheosRenderPipeline;
namespace U=Upscaling;
namespace N=NeuralRendering;
using namespace InteropFixture;
template<class T> T Value(U::Result<T> result) {
    if(!result){std::fprintf(stderr,"XeSS: %s\n",result.error().message.c_str());std::exit(1);}return std::move(*result);
}
static void Accepted(U::Result<void> result){if(!result)std::fprintf(stderr,"XeSS: %s\n",result.error().message.c_str());Require(bool(result),"XeSS accepted");}
template<class T> T NrValue(N::Result<T> result) {
    if(!result){std::fprintf(stderr,"NR: %s\n",result.error().message.c_str());std::exit(1);}return std::move(*result);
}
static void NrAccepted(N::Result<void> result){if(!result)std::fprintf(stderr,"NR: %s\n",result.error().message.c_str());Require(bool(result),"NR accepted");}
static ComPtr<ID3D11Texture2D> Texture(Rig& rig,U::Extent extent,DXGI_FORMAT format) {
    D3D11_TEXTURE2D_DESC d{};d.Width=extent.width;d.Height=extent.height;d.MipLevels=d.ArraySize=d.SampleDesc.Count=1;
    d.Format=format;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> result;Check(rig.device11->CreateTexture2D(&d,nullptr,&result),"scene texture");return result;
}
static std::vector<uint32_t> Read(Rig& rig,ID3D11Texture2D* source) {
    D3D11_TEXTURE2D_DESC d{};source->GetDesc(&d);d.BindFlags=d.MiscFlags=0;d.Usage=D3D11_USAGE_STAGING;d.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Texture2D> staging;Check(rig.device11->CreateTexture2D(&d,nullptr,&staging),"NR readback");
    rig.context11->CopyResource(staging.Get(),source);D3D11_MAPPED_SUBRESOURCE m{};
    Check(rig.context11->Map(staging.Get(),0,D3D11_MAP_READ,0,&m),"read delivered image");
    std::vector<uint32_t> pixels(size_t(d.Width)*d.Height);
    for(unsigned y=0;y<d.Height;++y)std::memcpy(pixels.data()+size_t(y)*d.Width,static_cast<const char*>(m.pData)+y*m.RowPitch,d.Width*4);
    rig.context11->Unmap(staging.Get(),0);return pixels;
}
static N::PostSrInput PostInput(Rig& rig,U::Extent render,U::Extent display,ID3D11Texture2D* color,
    ID3D11Texture2D* depth,ID3D11Texture2D* motion,uint64_t id,U::ColorEncoding encoding) {
    N::PostSrInput p;auto& r=p.resources;auto& s=p.source;
    r.context=rig.context11;r.color=color;r.depth=depth;r.motion=motion;
    r.epoch=r.guideEpoch=1;r.sourceId=r.guideSourceId=id;r.previousSourceId=id-1;
    r.presentationTime=id/60.;r.colorExtent={display.width,display.height};r.guideExtent={render.width,render.height};
    r.colorDomain=N::ColorDomain::SdrBytes;
    s.backend=U::BackendKind::Xess;s.outcome=U::UpscaleOutcome::Temporal;s.epoch=s.guideEpoch=1;
    s.sourceId=s.guideSourceId=id;s.previousSourceId=id-1;s.sourceTime=s.guideTime=r.presentationTime;
    s.render=s.guides=r.guideExtent;s.display=s.color=r.colorExtent;s.colorDomain=N::ColorDomain::SdrBytes;s.encoding=encoding;
    s.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;s.depthFormat=DXGI_FORMAT_R32_FLOAT;s.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
    s.guideOrigin=N::GuideOrigin::RealSource;s.motion={float(render.width),float(render.height),true,false};
    NrValue(N::BindPostSrMotionScales(s,r.motionScaleX,r.motionScaleY));return p;
}
static void PendingStyleReader(Rig& rig,U::XessHostResources& sr,const std::filesystem::path& root,
    const std::filesystem::path& core,Graphics::SharedTexture& color,ID3D11Texture2D* depth,ID3D11Texture2D* motion,U::Extent display) {
    auto bridge=sr.Bridge();auto* device=bridge->Device12();const auto luid=device->GetAdapterLuid();
    DXGI_ADAPTER_DESC desc{};Check(rig.adapter->GetDesc(&desc),"reader adapter identity");
    N::StageContract contract;contract.device=device;contract.queue=bridge->Queue();
    contract.adapterLuid={luid.LowPart,luid.HighPart};contract.colorExtent={display.width,display.height};
    const auto render=sr.RenderExtent();contract.guideExtent={render.width,render.height};
    auto owner=std::make_shared<N::RuntimeOwner>(N::RuntimeOwnerPaths{root/"NR/rtx40/nvngx_dlssnr.dll",core,
        std::filesystem::absolute("xess-nr-reader-cache"),true});
    NrAccepted(owner->Open(N::RuntimeCatalog()[1],device,{desc.VendorId,desc.DeviceId,desc.SubSysId,contract.adapterLuid,false}));
    N::PostUpscale post;NrAccepted(post.Initialize(owner,rig.device11.Get(),contract));
    N::SettingsSnapshot settings;settings.enabled=true;settings.placement=N::Placement::After;settings.revision=1;
    auto input=PostInput(rig,render,display,color.texture11.Get(),depth,motion,100,U::ColorEncoding::Gamma22);
    auto delivered=NrValue(post.Evaluate(input,settings));Require(delivered.evaluated,"NR pending-reader source evaluated");
    ComPtr<ID3D12CommandQueue> queue;D3D12_COMMAND_QUEUE_DESC q{};Check(device->CreateCommandQueue(&q,IID_PPV_ARGS(&queue)),"genuine downstream reader queue");
    ComPtr<ID3D12Fence> ready,gate,done;uint64_t readyValue{};
    Check(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"reader gate");
    Check(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&done)),"reader completion");
    Check(bridge->SignalReader(&ready,&readyValue),"enhanced D3D11 source ready");
    Check(queue->Wait(ready.Get(),readyValue),"reader waits for actual enhanced source");Check(queue->Wait(gate.Get(),1),"hold reader");
    D3D11_TEXTURE2D_DESC d{};color.texture11->GetDesc(&d);Graphics::SharedTexture sink;Check(bridge->CreateSharedTexture(d,sink),"reader destination");
    ComPtr<ID3D12CommandAllocator> allocator;ComPtr<ID3D12GraphicsCommandList> list;
    Check(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator)),"reader allocator");
    Check(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list)),"reader list");
    Check(Interop::RecordCopy(list.Get(),color.texture12.Get(),sink.texture12.Get()),"real enhanced output reader copy");Check(list->Close(),"reader close");
    ID3D12CommandList* commands[]{list.Get()};queue->ExecuteCommandLists(1,commands);Check(queue->Signal(done.Get(),1),"reader fence follows real copy");
    NrAccepted(post.TrackReader(delivered.delivery,done.Get(),1));
    Require(done->GetCompletedValue()<1,"reader genuinely pending before style change");
    std::thread release([gate]{std::this_thread::sleep_for(std::chrono::milliseconds(100));gate->Signal(1);});
    settings.revision=2;settings.tuning.style=1;input.resources.previousSourceId=100;input.resources.sourceId=input.resources.guideSourceId=101;
    input.source.previousSourceId=100;input.source.sourceId=input.source.guideSourceId=101;
    input.resources.presentationTime=input.source.sourceTime=input.source.guideTime=101/60.;
    const auto start=std::chrono::steady_clock::now();auto changed=post.Evaluate(input,settings);
    const auto elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();release.join();
    Require(done->GetCompletedValue()>=1 && bool(changed) && changed->evaluated && elapsed>=50,"live style change drains pending genuine reader before reuse");
    NrAccepted(post.WaitDelivery(*changed));NrAccepted(post.Retire());NrAccepted(owner->Retire());
    std::printf("PENDING_STYLE_READER drained=%.2fms\n",elapsed);
}
int wmain(int argc,wchar_t** argv) {
    std::setvbuf(stdout,nullptr,_IONBF,0);
    if(argc!=4 || NrRuntimeResearch::GameRunningOrUnknown()){std::puts("REFUSED: args or Skyrim running/inventory unknown");return 1;}
    Rig rig;DXGI_ADAPTER_DESC desc{};Check(rig.adapter->GetDesc(&desc),"actual render adapter");
    Require(desc.VendorId==0x10de && desc.DeviceId==0x2702,"local qualified RTX4080 SUPER fixture");
    const auto root=std::filesystem::absolute(argv[2]),core=std::filesystem::absolute(argv[3]);
    for(auto quality:{U::Quality::NativeAA,U::Quality::Performance})for(auto encoding:{U::ColorEncoding::Gamma22,U::ColorEncoding::SRGB}) {
        const U::Extent display{640,360};U::XessHostResources sr(std::filesystem::absolute(argv[1]));
        const auto render=Value(sr.Initialize(rig.device11.Get(),quality,display,encoding));
        auto color=Texture(rig,render,DXGI_FORMAT_R8G8B8A8_UNORM),depth=Texture(rig,render,DXGI_FORMAT_R32_FLOAT),motion=Texture(rig,render,DXGI_FORMAT_R16G16_FLOAT);
        D3D11_TEXTURE2D_DESC d{};d.Width=display.width;d.Height=display.height;d.MipLevels=d.ArraySize=d.SampleDesc.Count=1;
        d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
        Graphics::SharedTexture encoded;Check(sr.Bridge()->CreateSharedTexture(d,encoded),"shared encoded actual XeSS result");
        std::vector<uint32_t> colors(size_t(render.width)*render.height),velocities(colors.size());std::vector<float> depths(colors.size(),.5f);
        rig.context11->UpdateSubresource(depth.Get(),0,nullptr,depths.data(),render.width*4,0);
        rig.context11->UpdateSubresource(motion.Get(),0,nullptr,velocities.data(),render.width*4,0);
        N::BeforeHost nr;N::StartupSettings startup;startup.community=true;startup.runtimeRoot=root;startup.driverCore=core;startup.sourceEncoding=encoding;
        NrAccepted(nr.Inspect(rig.device11.Get(),startup,std::filesystem::absolute("xess-nr-host-cache"),sr.Bridge()->Device12()));
        N::SettingsSnapshot settings;settings.enabled=true;settings.revision=1;settings.tuning.style=1;
        U::SdrColorConverter encode;unsigned beforeCount{},afterCount{};size_t changedRgb{};
        for(unsigned index=0;index<32;++index) {
            const uint64_t id=index+1;
            settings.placement=index<16?N::Placement::Before:N::Placement::After;
            settings.enabled=index!=8 && index!=24;settings.passes=(index==12 || index==28)?2:1;
            settings.tuning.style=(index==10 || index==26)?0:1;++settings.revision;
            for(unsigned y=0;y<render.height;++y)for(unsigned x=0;x<render.width;++x) {
                const uint32_t v=((x/12+y/12)%2)?150:60;colors[size_t(y)*render.width+x]=v|((v/2)<<8)|((v+40)<<16)|(255u<<24);
            }
            rig.context11->UpdateSubresource(color.Get(),0,nullptr,colors.data(),render.width*4,0);
            auto input=PostInput(rig,render,display,encoded.texture11.Get(),depth.Get(),motion.Get(),id,encoding);
            if(index<16) {
                auto pre=input.resources;pre.color=color;pre.colorExtent=pre.guideExtent;pre.motionScaleX=float(render.width);pre.motionScaleY=float(render.height);
                pre.colorDomain=N::ColorDomain::Unknown;pre.reset=index==0;
                auto result=NrValue(nr.Evaluate(pre,settings));Require(result.evaluated==settings.enabled,"Before NR enabled/bypass follows settings");
                beforeCount+=result.evaluated;
                auto pixels=Read(rig,color.Get());if(!settings.enabled)Require(pixels==colors,"disabled Before NR exact byte bypass");
                for(size_t p=0;p<pixels.size();++p){Require((pixels[p]>>24)==255,"Before NR alpha preserved");changedRgb+=(pixels[p]&0xffffff)!=(colors[p]&0xffffff);}
            }
            U::UpscaleFrame frame{};frame.backend=U::BackendKind::Xess;frame.render=frame.subrect=render;frame.display=display;
            frame.sourceId=id;frame.sourceEpoch=1;frame.reset=index==0 || index==16 || !settings.enabled;
            const auto jitter=Value(sr.Upscaler()->QueryJitter(index));frame.jitterX=-jitter[0];frame.jitterY=-jitter[1];
            frame.colorIsLinear=true;frame.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
            frame.motionConvention={float(render.width),float(render.height),true,false};
            Accepted(sr.PrepareInput(color.Get(),depth.Get(),motion.Get()));auto bridge=sr.Bridge();Check(bridge->SignalProducer(),"XeSS producer");
            ID3D12GraphicsCommandList* list{};Check(bridge->Begin(&list),"XeSS list");Accepted(sr.Upscaler()->Dispatch(list,sr.Resources(),frame));
            Check(bridge->Submit(),"XeSS submit");Check(bridge->WaitConsumer(),"XeSS consumer ownership");
            Check(encode.Convert(rig.context11.Get(),sr.Output11(),encoded.texture11.Get(),U::ColorEncoding::Linear,encoding),"actual XeSS SDR handoff");
            if(index>=16) {
                auto original=Read(rig,encoded.texture11.Get());auto result=NrValue(nr.EvaluatePost(input,settings));
                Require(result.evaluated==settings.enabled,"After NR enabled/bypass follows settings");afterCount+=result.evaluated;
                auto pixels=Read(rig,encoded.texture11.Get());if(!settings.enabled)Require(pixels==original,"disabled After NR exact byte bypass");
                for(size_t p=0;p<pixels.size();++p){Require((pixels[p]>>24)==(original[p]>>24),"After NR alpha preserved");changedRgb+=(pixels[p]&0xffffff)!=(original[p]&0xffffff);}
            }
        }
        Require(beforeCount==15 && afterCount==15 && changedRgb>colors.size(),"actual XeSS plus NR both placements visibly affect RGB");
        NrAccepted(nr.Retire());
        if(quality==U::Quality::Performance && encoding==U::ColorEncoding::Gamma22)
            PendingStyleReader(rig,sr,root,core,encoded,depth.Get(),motion.Get(),display);
        Accepted(sr.Retire());
        std::printf("PASS CASE quality=%d encoding=%d render=%ux%u display=640x360 before=%u after=%u changedRGB=%zu\n",int(quality),int(encoding),render.width,render.height,beforeCount,afterCount,changedRgb);
    }
    rig.ValidateDebug();std::puts("PASS PostXessGpu actual SR/NR handoff and pending style reader");return 0;
}
