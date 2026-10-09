#include "Upscaling/XessHostResources.h"
#include "Upscaling/SdrColorConversion.h"
#include "InteropTestRig.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include "XessVisibleScene.h"
#include <DirectXPackedVector.h>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
static void Accepted(Result<void> result)
{
    if(!result)std::printf("ERROR native=%lld %s\n",result.error().nativeResult,result.error().message.c_str());
    Require(bool(result),"XeSS operation accepted");
}
static ComPtr<ID3D11Texture2D> Texture(Rig& rig,Extent size,DXGI_FORMAT format,bool staging=false)
{
    D3D11_TEXTURE2D_DESC desc{};desc.Width=size.width;desc.Height=size.height;desc.MipLevels=desc.ArraySize=1;desc.SampleDesc.Count=1;desc.Format=format;
    desc.Usage=staging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;desc.CPUAccessFlags=staging?D3D11_CPU_ACCESS_READ:0;
    desc.BindFlags=staging?0:D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> texture;Check(rig.device11->CreateTexture2D(&desc,nullptr,&texture),"scene/readback texture");return texture;
}
struct ImageStats { double mean[3]{};double center{};float alphaMin{100};std::vector<float> rgb; };
static ImageStats Read(Rig& rig,ID3D11Texture2D* output,ID3D11Texture2D* staging,Extent size)
{
    rig.context11->CopyResource(staging,output);
    D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(staging,0,D3D11_MAP_READ,0,&mapped),"read actual reconstructed image");
    ImageStats stats;stats.rgb.reserve(size_t(size.width)*size.height*3);double weight{},position{};size_t samples{};
    for(unsigned y=0;y<size.height;++y) {
        const auto* row=reinterpret_cast<const DirectX::PackedVector::HALF*>(static_cast<const unsigned char*>(mapped.pData)+y*mapped.RowPitch);
        for(unsigned x=0;x<size.width;++x) {
            float rgb[3]{};
            for(unsigned c=0;c<3;++c){rgb[c]=DirectX::PackedVector::XMConvertHalfToFloat(row[x*4+c]);Require(std::isfinite(rgb[c]) && rgb[c]>=-.05f && rgb[c]<2,"actual SDK output finite and bounded");stats.rgb.push_back(rgb[c]);}
            const auto alpha=DirectX::PackedVector::XMConvertHalfToFloat(row[x*4+3]);Require(std::isfinite(alpha),"output alpha finite");stats.alphaMin=std::min(stats.alphaMin,alpha);
            if(x>size.width/10 && x<size.width*9/10 && y>size.height/10 && y<size.height*9/10){for(unsigned c=0;c<3;++c)stats.mean[c]+=rgb[c];++samples;}
            if(rgb[0]>.5f){weight+=1;position+=x+.5;}
        }
    }
    rig.context11->Unmap(staging,0);for(auto& channel:stats.mean)channel/=samples;stats.center=weight?position/weight:-1;
    return stats;
}
static unsigned Byte(float linear,ColorEncoding encoding)
{
    const auto value=encoding==ColorEncoding::Gamma22?std::pow(linear,1.f/2.2f):linear<=.0031308f?12.92f*linear:1.055f*std::pow(linear,1.f/2.4f)-.055f;
    return static_cast<unsigned>(std::lround(std::clamp(value,0.f,1.f)*255));
}
static void PendingReader(Rig& rig,XessHostResources& host)
{
    auto bridge=host.Bridge();auto* device=bridge->Device12();
    ComPtr<ID3D12CommandQueue> queue;D3D12_COMMAND_QUEUE_DESC queueDesc{};queueDesc.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
    Check(device->CreateCommandQueue(&queueDesc,IID_PPV_ARGS(&queue)),"independent downstream reader queue");
    ComPtr<ID3D12Fence> gate,done,ready;uint64_t readyValue{};
    Check(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"reader interruption gate");
    Check(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&done)),"actual reader completion fence");
    Check(bridge->SignalReader(&ready,&readyValue),"signal actual producer/output-reader completion");
    Check(queue->Wait(ready.Get(),readyValue),"reader depends on completed output");Check(queue->Wait(gate.Get(),1),"intentionally hold downstream reader");
    D3D11_TEXTURE2D_DESC description{};host.Output11()->GetDesc(&description);
    SharedTexture sink;Check(bridge->CreateSharedTexture(description,sink),"retained downstream copy destination");
    ComPtr<ID3D12CommandAllocator> allocator;ComPtr<ID3D12GraphicsCommandList> list;
    Check(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator)),"reader allocator");
    Check(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list)),"reader command list");
    Check(Interop::RecordCopy(list.Get(),host.Resources().output,sink.texture12.Get()),"record real output reader copy");Check(list->Close(),"close reader list");
    ID3D12CommandList* commands[]{list.Get()};queue->ExecuteCommandLists(1,commands);Check(queue->Signal(done.Get(),1),"signal after genuine output copy");
    Accepted(host.Upscaler()->TrackReader(done.Get(),1));
    const auto pending=host.Retire();Require(!pending && pending.error().kind==ErrorKind::RetirementFailure && host.Resources().output && GetModuleHandleW(L"libxess.dll"),"pending actual GPU reader retains output/context/runtime");
    Check(gate->Signal(1),"release reader interruption");HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Require(event!=nullptr,"reader event");
    Check(done->SetEventOnCompletion(1,event),"reader completion event");Require(WaitForSingleObject(event,5000)==WAIT_OBJECT_0,"actual output copy completed");CloseHandle(event);
    Accepted(host.Retire());
}
static void AllocatorReuse(Rig& rig)
{
    Interop bridge;rig.Initialize(bridge);bridge.SetRetirementWaitPolicy({5,2000});
    ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"allocator gate");
    Check(rig.queue->Wait(gate.Get(),1),"hold genuine queue submissions before allocator reuse");
    ID3D12GraphicsCommandList* list{};
    for(unsigned i=0;i<3;++i){Check(bridge.Begin(Work::FrameGeneration,&list),"record each allocator slot");Check(bridge.Submit(Work::FrameGeneration),"submit held allocator slot");}
    std::thread release([gate]{std::this_thread::sleep_for(std::chrono::milliseconds(60));gate->Signal(1);});
    const auto start=std::chrono::steady_clock::now();Check(bridge.Begin(Work::FrameGeneration,&list),"reused allocator waits for actual fence");
    const auto elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();release.join();
    Require(elapsed>=25,"allocator was not reset while its actual submission remained held");
    Check(bridge.Submit(Work::FrameGeneration),"submit retired reused slot");Check(bridge.Drain(),"allocator test drain");
    std::printf("Allocator reuse genuine wait=%.2f ms\n",elapsed);
}
static void VendorFailure(Rig& rig,const std::filesystem::path& fixture)
{
    XessHostResources host(fixture);const auto extent=host.Initialize(rig.device11.Get(),Quality::NativeAA,{640,360},ColorEncoding::Gamma22);
    Require(bool(extent),"fault fixture shared host initialized");
    const auto mode=reinterpret_cast<void(*)(unsigned)>(GetProcAddress(GetModuleHandleW(L"libxess.dll"),"FixtureMode"));Require(mode!=nullptr,"explicit test fixture selected");mode(2);
    UpscaleFrame frame{};frame.backend=BackendKind::Xess;frame.render=frame.subrect=*extent;frame.display={640,360};frame.sourceId=frame.sourceEpoch=1;
    frame.colorIsLinear=true;frame.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
    frame.motionConvention={640,360,true,false};
    auto bridge=host.Bridge();Check(bridge->SignalProducer(),"fault-path producer signal");ID3D12GraphicsCommandList* list{};Check(bridge->Begin(&list),"fault-path list");
    const auto dispatch=host.Upscaler()->Dispatch(list,host.Resources(),frame);
    Require(!dispatch && dispatch.error().kind==ErrorKind::DispatchFailure && dispatch.error().nativeResult==XESS_RESULT_ERROR_DEVICE,"actual owner reports forced vendor Execute failure");
    Require(!host.Retire() && host.Resources().output,"unsubmitted recording retains context/resources until explicitly discarded");
    Check(bridge->DiscardRecording(),"discard partial unsubmitted vendor work");mode(0);Accepted(host.Retire());
    std::puts("PASS: forced SDK failure discarded without submitting partial work; context/resources retired");
}
int wmain(int argc,wchar_t** argv)
{
    Require(argc==3 || (argc==4 && std::wstring_view(argv[3])==L"--visible"),"official plugin directory and separate fault-fixture root required; optional --visible");
    const bool visible=argc==4;
    if(NrRuntimeResearch::GameRunningOrUnknown()){std::puts("NOT QUALIFIED: Skyrim running or guard unavailable");return 2;}
    Rig rig;
    const Extent display=visible?Extent{1280,720}:Extent{640,360};unsigned executed{};
    XessVisibleScene scene;if(visible)scene.Create(rig,display.width,display.height);
    for(auto encoding:{ColorEncoding::Gamma22,ColorEncoding::SRGB})for(auto quality:{Quality::NativeAA,Quality::Quality,Quality::Performance}) {
        if(visible && encoding==ColorEncoding::SRGB)continue; // numerical suite qualifies both; visible ENB comparison uses Gamma22
        if(visible)scene.Mode(quality==Quality::NativeAA?L"Native":quality==Quality::Quality?L"Quality":L"Performance");
        XessHostResources host(std::filesystem::absolute(argv[1]));
        const auto initialized=host.Initialize(rig.device11.Get(),quality,display,encoding);
        if(!initialized)std::printf("Initialization: %s native=%lld\n",initialized.error().message.c_str(),initialized.error().nativeResult);
        Require(bool(initialized),"real XeSS host creates SDK-sized shared resources");
        const auto render=*initialized;
        if(quality==Quality::NativeAA)Require(render==display,"Native actual SDK dimensions");
        std::printf("CASE quality=%d encoding=%d render=%ux%u display=%ux%u requestedFlags=%u effectiveFlags=%u\n",static_cast<int>(quality),static_cast<int>(encoding),render.width,render.height,display.width,display.height,host.Upscaler()->RequestedFlags(),host.Upscaler()->EffectiveFlags());
        auto color=Texture(rig,render,DXGI_FORMAT_R8G8B8A8_UNORM),depth=Texture(rig,render,DXGI_FORMAT_R32_FLOAT),motion=Texture(rig,render,DXGI_FORMAT_R16G16_FLOAT);
        auto readback=Texture(rig,display,DXGI_FORMAT_R16G16B16A16_FLOAT,true);
        auto encoded=Texture(rig,display,DXGI_FORMAT_R8G8B8A8_UNORM),encodedReadback=Texture(rig,display,DXGI_FORMAT_R8G8B8A8_UNORM,true);
        std::vector<uint32_t> colors(size_t(render.width)*render.height);
        std::vector<float> depths(colors.size(),.5f);
        std::vector<DirectX::PackedVector::HALF> motions(colors.size()*2,0);
        SdrColorConverter encode;
        ImageStats resetReference,staticReference;double maximumTrackingError{},previousCenter{},staticRms{};UpscaleFrame lastFrame;
        const auto visibleStart=std::chrono::steady_clock::now();
        for(unsigned index=0;index<(visible?320u:80u);++index) {
            if(visible && !scene.Pump()){Accepted(host.Retire());std::puts("NOT QUALIFIED: visible scene cancelled; resources retired");return 2;}
            const auto jitter=host.Upscaler()->QueryJitter(index);Require(bool(jitter),"context produces real-size jitter");
            UpscaleFrame frame{};frame.backend=BackendKind::Xess;frame.render=frame.subrect=render;frame.display=display;
            frame.sourceId=index+1;frame.sourceEpoch=index<16?1:2;frame.jitterX=-(*jitter)[0];frame.jitterY=-(*jitter)[1];frame.reset=index==0 || index==16 || index==32;
            frame.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;frame.colorIsLinear=true;
            frame.motionConvention={float(render.width),float(render.height),true,false};
            lastFrame=frame;
            const bool uniform=index<32;
            const float step=visible?.0005f:.002f;
            const float pan=index<64?0:float(index-64)*step,previousPan=index<=64?pan:pan-step;
            for(unsigned y=0;y<render.height;++y)for(unsigned x=0;x<render.width;++x) {
                const auto pixel=size_t(y)*render.width+x;
                if(uniform){colors[pixel]=128u|(64u<<8)|(192u<<16)|(255u<<24);depths[pixel]=.5f;motions[pixel*2]=motions[pixel*2+1]=0;continue;}
                const auto wx=(x+.5f-frame.jitterX)/render.width+pan,wy=(y+.5f-frame.jitterY)/render.height;
                const bool foreground=std::abs(wx-.5f)<.12f && std::abs(wy-.5f)<.2f;
                const auto checker=static_cast<unsigned>(std::floor(wx*48)+std::floor(wy*24))%2;
                const float r=foreground?.8f:.08f+.12f*checker,g=foreground?.2f:.15f+.15f*checker,b=foreground?.05f:.25f+.2f*checker;
                colors[pixel]=Byte(r,encoding)|(Byte(g,encoding)<<8)|(Byte(b,encoding)<<16)|(255u<<24);
                depths[pixel]=foreground?.9339339f:.9676343f;
                motions[pixel*2]=DirectX::PackedVector::XMConvertFloatToHalf(pan-previousPan);motions[pixel*2+1]=0;
            }
            rig.context11->UpdateSubresource(color.Get(),0,nullptr,colors.data(),render.width*4,0);
            rig.context11->UpdateSubresource(depth.Get(),0,nullptr,depths.data(),render.width*4,0);
            rig.context11->UpdateSubresource(motion.Get(),0,nullptr,motions.data(),render.width*4,0);
            Accepted(host.PrepareInput(color.Get(),depth.Get(),motion.Get()));
            auto bridge=host.Bridge();Check(bridge->SignalProducer(),"signal prepared scene producer");
            ID3D12GraphicsCommandList* list{};Check(bridge->Begin(&list),"begin actual SDK temporal command list");
            if(index==0)Require(!host.PrepareInput(color.Get(),depth.Get(),motion.Get()),"input overwrite refused after producer ownership commits");
            Accepted(host.Upscaler()->Dispatch(list,host.Resources(),frame));
            Check(bridge->Submit(),"submit genuine temporal reconstruction");Check(bridge->WaitConsumer(),"queue real D3D11 output reader");++executed;
            const auto image=Read(rig,host.Output11(),readback.Get(),display);
            if(index==0)resetReference=image;
            if(index==16){float maximum{};for(size_t p=0;p<image.rgb.size();++p)maximum=std::max(maximum,std::abs(image.rgb[p]-resetReference.rgb[p]));Require(maximum<.002f,"source epoch and camera reset reproduce independent uniform image");}
            if(index==15 || index==31) {
                const double gamma[3]{.2195197181,.04777575356,.5356416093},srgb[3]{.2158605001,.05126945837,.5271151257};
                const auto* expected=encoding==ColorEncoding::Gamma22?gamma:srgb;
                std::printf("  uniform mean=%.6f,%.6f,%.6f alphaMin=%.3f\n",image.mean[0],image.mean[1],image.mean[2],image.alphaMin);
                for(unsigned c=0;c<3;++c)Require(std::abs(image.mean[c]-expected[c])<.02,"real SDK preserves decoded constant colour despite effective LDR flag removal");
                Check(encode.Convert(rig.context11.Get(),host.Output11(),encoded.Get(),ColorEncoding::Linear,encoding),"encode reconstructed SDR handoff");
                rig.context11->CopyResource(encodedReadback.Get(),encoded.Get());D3D11_MAPPED_SUBRESOURCE mapped{};
                Check(rig.context11->Map(encodedReadback.Get(),0,D3D11_MAP_READ,0,&mapped),"read re-encoded handoff");
                const auto* pixel=static_cast<const unsigned char*>(mapped.pData)+(display.height/2)*mapped.RowPitch+(display.width/2)*4;
                Require(std::abs(int(pixel[0])-128)<=3 && std::abs(int(pixel[1])-64)<=3 && std::abs(int(pixel[2])-192)<=3,"Gamma22/SRGB source bytes roundtrip through actual XeSS and output conversion");
                rig.context11->Unmap(encodedReadback.Get(),0);
            }
            if(index==56)staticReference=image;
            if(index==63) {
                double squared{};
                for(size_t p=0;p<image.rgb.size();++p){const double difference=image.rgb[p]-staticReference.rgb[p];squared+=difference*difference;}
                staticRms=std::sqrt(squared/image.rgb.size());
                Require(staticRms<.02,"stationary checker edges settle across different subpixel jitter samples");
            }
            if(!uniform){Require(image.center>0,"foreground survives actual edge reconstruction");const auto error=std::abs(image.center-display.width*(.5-pan));maximumTrackingError=std::max(maximumTrackingError,error);if(index>64)Require(image.center<previousCenter+1,"moving edge does not reverse into whole-image wobble");previousCenter=image.center;}
            if(visible){Check(encode.Convert(rig.context11.Get(),host.Output11(),encoded.Get(),ColorEncoding::Linear,encoding),"visible SDR handoff");scene.Present(rig,encoded.Get());std::this_thread::sleep_until(visibleStart+std::chrono::milliseconds((index+1)*33));}
        }
        std::printf("  static-edge late-frame RMS=%.6f; moving-edge max centroid error=%.4f pixels\n",staticRms,maximumTrackingError);
        Require(host.Upscaler()->AdmitFrame(lastFrame.sourceId,lastFrame.sourceEpoch)==XessFrameAdmission::DuplicateSource,
            "repeated rendered source is deferred before preparing shared input");
        Require(host.Upscaler()->AdmitFrame(lastFrame.sourceId-1,lastFrame.sourceEpoch)==XessFrameAdmission::DuplicateSource,
            "older source in the same epoch cannot enter temporal processing");
        Require(host.Upscaler()->AdmitFrame(1,lastFrame.sourceEpoch+1)==XessFrameAdmission::Ready,
            "new epoch permits reset source numbering");
        // A deferred duplicate never records work. The next source must execute
        // on the actual retained context with the vendor reset flag still off.
        ++lastFrame.sourceId;lastFrame.reset=false;
        Require(host.Upscaler()->AdmitFrame(lastFrame.sourceId,lastFrame.sourceEpoch)==XessFrameAdmission::Ready,
            "new source after duplicate admitted without reset");
        auto retained=host.Bridge();Check(retained->SignalProducer(),"retained producer");ID3D12GraphicsCommandList* retainedList{};
        Check(retained->Begin(&retainedList),"retained recording");
        Accepted(host.Upscaler()->Dispatch(retainedList,host.Resources(),lastFrame));
        Check(retained->Submit(),"retained SDK submission");Check(retained->WaitConsumer(),"retained consumer");++executed;
        Require(!lastFrame.reset,"duplicate did not request SDK history reset");
        bool workerDeferred{};
        std::thread worker([&]{workerDeferred=host.Upscaler()->AdmitFrame(lastFrame.sourceId+1,lastFrame.sourceEpoch)==XessFrameAdmission::OffOwnerThread;});
        worker.join();Require(workerDeferred,"foreign thread defers before SDK or shared guide writes");
        ++lastFrame.sourceId;lastFrame.reset=true;
        Require(host.Upscaler()->AdmitFrame(lastFrame.sourceId,lastFrame.sourceEpoch)==XessFrameAdmission::Ready,
            "new owner-thread source remains eligible after duplicate and worker deferrals");
        auto resumed=host.Bridge();Check(resumed->SignalProducer(),"resumed producer");ID3D12GraphicsCommandList* resumedList{};
        Check(resumed->Begin(&resumedList),"resumed temporal recording");
        Accepted(host.Upscaler()->Dispatch(resumedList,host.Resources(),lastFrame));
        Check(resumed->Submit(),"resumed SDK submission");Check(resumed->WaitConsumer(),"resumed SDK consumer");++executed;
        Require(maximumTrackingError<3,"guide/jitter alignment tracks analytic non-square camera pan within three display pixels");
        auto bridge=host.Bridge();Check(bridge->SignalProducer(),"rejected-frame producer");ID3D12GraphicsCommandList* list{};Check(bridge->Begin(&list),"rejected-frame list");
        const auto duplicate=host.Upscaler()->Dispatch(list,host.Resources(),lastFrame);
        Require(!duplicate && duplicate.error().kind==ErrorKind::InvalidInput,"duplicate source identity does not execute SDK");
        ++lastFrame.sourceId;
        SharedTexture replacement;D3D11_TEXTURE2D_DESC replacementDesc{};host.Output11()->GetDesc(&replacementDesc);
        Check(bridge->CreateSharedTexture(replacementDesc,replacement),"valid replacement output identity");
        auto replaced=host.Resources();replaced.output=replacement.texture12.Get();
        const auto identity=host.Upscaler()->Dispatch(list,replaced,lastFrame);
        Require(!identity && identity.error().kind==ErrorKind::InvalidInput,"same-size output replacement refused until retained context retires");
        ++lastFrame.render.width;
        const auto resize=host.Upscaler()->Dispatch(list,host.Resources(),lastFrame);Require(!resize && resize.error().kind==ErrorKind::InvalidInput,"live changed extent refused without retirement");
        Check(bridge->DiscardRecording(),"discard rejected frame without GPU submission");Check(bridge->Drain(),"retire rejected-frame producer");
        PendingReader(rig,host);Require(!host.Resources().color && !host.Upscaler(),"retired views/context released together");
    }
    AllocatorReuse(rig);VendorFailure(rig,std::filesystem::absolute(argv[2]));
    rig.ValidateDebug();
    std::printf("PASS: %u actual XeSS executions; Native/Quality/Performance %s roundtrip, static edges, non-square moving guides, resets and retirement. Visible quality requires human confirmation.\n",executed,visible?"Gamma22":"Gamma22/SRGB");
}
