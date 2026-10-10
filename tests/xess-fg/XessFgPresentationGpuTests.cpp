#include "FrameGen/XessGenerationPresentation.h"
#include "FrameGen/XessGenerationPolicy.h"
#include "XessFgVisibleScene.h"
#include "XessFgFrameFixture.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <DirectXPackedVector.h>
#include <chrono>
#include <thread>
#include <vector>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
namespace
{
    constexpr UINT width=1280,height=720;
    void Success(const Result<void>& result,const char* stage)
    {
        if (!result) std::fprintf(stderr,"NOT QUALIFIED stage=%s kind=%d native=%lld %s\n",stage,int(result.error().kind),result.error().nativeResult,result.error().message.c_str());
        Require(bool(result),stage);
    }
    ComPtr<ID3D11Texture2D> Texture(Rig& rig,DXGI_FORMAT format,UINT bind)
    {
        auto desc=rig.Description();desc.Width=width;desc.Height=height;desc.Format=format;desc.BindFlags=bind;
        ComPtr<ID3D11Texture2D> result;Check(rig.device11->CreateTexture2D(&desc,nullptr,&result),"scene inputs");return result;
    }
    struct Inputs
    {
        ComPtr<ID3D11Texture2D> scene,ui,depth,motion;
        std::vector<uint32_t> colour,hud;
        std::vector<float> z;
        std::vector<uint16_t> vectors;
        int previousX{};
        void Create(Rig& rig)
        {
            scene=Texture(rig,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
            ui=Texture(rig,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
            depth=Texture(rig,DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS);
            motion=Texture(rig,DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
            colour.resize(width*height);hud.resize(width*height);z.resize(width*height);vectors.resize(width*height*2);
        }
        void Simulate(unsigned tick,bool reset)
        {
            const int x=120+int(300*(1+std::sin(tick*.045)));
            const auto velocity=DirectX::PackedVector::XMConvertFloatToHalf(reset?0.f:float(previousX-x));
            for (UINT py=0;py<height;++py) for (UINT px=0;px<width;++px) {
                const auto i=py*width+px;const bool object=int(px)>=x && int(px)<x+100 && py>260 && py<460;
                const auto checker=((px/24+py/24)&1)?0xff303030u:0xff505050u;
                colour[i]=object?0xff2090f0u:checker;z[i]=object?.96f:.99f;
                vectors[i*2]=object?velocity:0;vectors[i*2+1]=0;
                const bool red=px>=24 && px<180 && py>=24 && py<56;
                const bool green=px>=24 && px<180 && py>=72 && py<104;
                const bool cross=(px>=width/2-2 && px<=width/2+2 && py>=height/2-16 && py<=height/2+16) ||
                    (py>=height/2-2 && py<=height/2+2 && px>=width/2-16 && px<=width/2+16);
                hud[i]=cross?0xffffffffu:red?0xff0000ffu:green?0x80008000u:0u;
            }
            previousX=x;
        }
        void Upload(Rig& rig)
        {
            rig.context11->UpdateSubresource(scene.Get(),0,nullptr,colour.data(),width*4,0);
            rig.context11->UpdateSubresource(ui.Get(),0,nullptr,hud.data(),width*4,0);
            rig.context11->UpdateSubresource(depth.Get(),0,nullptr,z.data(),width*4,0);
            rig.context11->UpdateSubresource(motion.Get(),0,nullptr,vectors.data(),width*4,0);
        }
    };
    // Read the actual native real-image publication. This does not pretend to
    // expose Intel's private generated buffers or qualify visual HUD stability.
    void CheckPublishedScene(Rig& rig,Interop& bridge,IDXGISwapChain4* chain,uint32_t expected,UINT x=0,UINT y=0)
    {
        ComPtr<ID3D12Resource> buffer;Check(chain->GetBuffer(chain->GetCurrentBackBufferIndex(),IID_PPV_ARGS(&buffer)),"real publication buffer");
        const auto desc=buffer->GetDesc();D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};UINT64 bytes{};
        rig.device12->GetCopyableFootprints(&desc,0,1,0,&footprint,nullptr,nullptr,&bytes);
        D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_READBACK;
        D3D12_RESOURCE_DESC readDesc{};readDesc.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;readDesc.Width=bytes;
        readDesc.Height=readDesc.DepthOrArraySize=readDesc.MipLevels=1;readDesc.SampleDesc.Count=1;readDesc.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        ComPtr<ID3D12Resource> readback;Check(rig.device12->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&readDesc,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&readback)),"real image readback");
        ID3D12GraphicsCommandList* list{};Check(bridge.Begin(Work::SwapChain,&list),"readback list");
        D3D12_RESOURCE_BARRIER barrier{};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;barrier.Transition.pResource=buffer.Get();
        barrier.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;barrier.Transition.StateBefore=D3D12_RESOURCE_STATE_PRESENT;barrier.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
        list->ResourceBarrier(1,&barrier);
        D3D12_TEXTURE_COPY_LOCATION from{};from.pResource=buffer.Get();from.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        D3D12_TEXTURE_COPY_LOCATION to{};to.pResource=readback.Get();to.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;to.PlacedFootprint=footprint;
        list->CopyTextureRegion(&to,0,0,0,&from,nullptr);
        std::swap(barrier.Transition.StateBefore,barrier.Transition.StateAfter);list->ResourceBarrier(1,&barrier);
        Check(bridge.Submit(Work::SwapChain),"readback submit");Check(bridge.Drain(),"readback retirement");
        void* data{};Check(readback->Map(0,nullptr,&data),"map real image");
        const auto pixel=*reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(data)+footprint.Offset+y*footprint.Footprint.RowPitch+x*4);
        readback->Unmap(0,nullptr);
        std::printf("PIXEL x=%u y=%u actual=%08x expected=%08x\n",x,y,pixel,expected);
        Require(pixel==expected,"native real-image publication preserves scene and premultiplied HUD in both FG modes");
    }
}
int wmain(int argc,wchar_t** argv)
{
    if (argc<3 || std::wstring_view(argv[1])!=L"--plugin-directory") return 1;
    const bool visible=argc==4 && std::wstring_view(argv[3])==L"--visible";
    if (NrRuntimeResearch::GameRunningOrUnknown()) { std::puts("NOT QUALIFIED: Skyrim running or process guard unavailable");return 2; }
    setvbuf(stdout,nullptr,_IONBF,0);
    auto loaded=XessGenerationRuntime::Load(argv[2]);if (!loaded) { std::printf("NOT QUALIFIED loader %s\n",loaded.error().message.c_str());return 2; }
    Rig rig;auto bridge=std::make_shared<Interop>();rig.Initialize(*bridge);
    D3D12_FEATURE_DATA_SHADER_MODEL sm{D3D_SHADER_MODEL_6_4};Check(rig.device12->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&sm,sizeof(sm)),"SM6.4 query");Require(sm.HighestShaderModel>=D3D_SHADER_MODEL_6_4,"non-Intel physical SM6.4");
    DXGI_ADAPTER_DESC adapter{};Check(rig.adapter->GetDesc(&adapter),"render adapter");
    std::printf("ADAPTER vendor=%04x device=%04x LUID=%08lx:%08lx extent=%ux%u SDK=3.0.2 real_runtime=1\n",adapter.VendorId,adapter.DeviceId,adapter.AdapterLuid.HighPart,adapter.AdapterLuid.LowPart,width,height);
    XessFgVisibleScene window;window.Create(width,height);
    DXGI_SWAP_CHAIN_DESC desc{};desc.OutputWindow=window.Handle();desc.Windowed=TRUE;desc.BufferDesc.Width=width;desc.BufferDesc.Height=height;
    desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.SampleDesc.Count=1;desc.BufferCount=2;
    XessGenerationPresentation owner;Success(owner.Create(rig.factory.Get(),*loaded,bridge,desc),"real Intel proxy create");
    Check(rig.factory->MakeWindowAssociation(window.Handle(),DXGI_MWA_NO_ALT_ENTER),"fixed-size window policy");
    Check(owner.StartupPresent(0,0),"real startup present");
    SetForegroundWindow(window.Handle());window.Pump();
    Inputs inputs;inputs.Create(rig);XessGenerationHistory history;
    const unsigned phaseFrames=visible?180:40,total=phaseFrames*4;
    unsigned generated{},offFrames{},onFrames{},errors{},lostFocus{};bool interrupted{};
    auto deadline=std::chrono::steady_clock::now();
    for (unsigned tick=0;tick<total;++tick) {
        if (!window.Pump()) { interrupted=true;break; }
        // Numerical mode deliberately loses foreground while FG is requested.
        // It still presents a real source and resets before resuming tags.
        if (!visible && tick==phaseFrames+20) { ShowWindow(window.Handle(),SW_MINIMIZE);window.Pump(); }
        if (!visible && tick==phaseFrames+22) { ShowWindow(window.Handle(),SW_RESTORE);SetForegroundWindow(window.Handle());window.Pump();history.Invalidate(); }
        const auto phase=tick/phaseFrames;const bool request=phase==1 || phase==3,menu=phase==2;
        if (tick%phaseFrames==0) window.Mode(request?L"FG ON (30 → 60)":menu?L"MENU / FG OFF":L"FG OFF (30)");
        const bool focus=GetForegroundWindow()==window.Handle();if (!focus) ++lostFocus;
        const auto sdkId=tick+1;
        Success(owner.Latency()->BeginFrame(sdkId),"XeLL sleep and simulation start");
        // In this harness, unlike a fabricated Present-time marker cluster,
        // the genuine simulation and producer draw lie between the markers.
        inputs.Simulate(tick,tick%phaseFrames==0);
        Success(owner.Latency()->Marker(sdkId,XELL_SIMULATION_END),"simulation end");
        Success(owner.Latency()->Marker(sdkId,XELL_RENDERSUBMIT_START),"render begin");inputs.Upload(rig);
        auto frame=XessFgFrame();frame.sourceId=sdkId;frame.sourceEpoch=1;frame.render=frame.subrect=frame.display=frame.depthExtent=frame.motionExtent={width,height};
        frame.output=inputs.scene.Get();frame.depth=inputs.depth.Get();frame.motion=inputs.motion.Get();
        frame.motionConvention={1,1,true,false};frame.outputEncoding=frame.uiEncoding=ColorEncoding::SRGB;
        frame.deltaMilliseconds=1000.f/30.f;frame.reset=tick%phaseFrames==0;
        frame.camera.view={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};frame.camera.position={0,0,0};
        frame.camera.projection[0]=float(height)/width;
        const auto admission=history.Decide(frame,UpscaleOutcome::Temporal,request && focus,true,menu);
        frame.reset=admission.reset;
        if (tick==phaseFrames+4) Require(!owner.Prepare(frame,inputs.ui.Get(),nullptr,false,sdkId,true),"missing HUD rejected before SDK admission");
        const auto prepared=owner.Prepare(frame,inputs.ui.Get(),nullptr,true,sdkId,admission.tag);
        if (!prepared) { std::printf("NOT QUALIFIED prepare tick=%u %s\n",tick,prepared.error().message.c_str());return 2; }
        if (tick==0 || tick==phaseFrames+2) {
            CheckPublishedScene(rig,*bridge,owner.SwapChain(),inputs.colour[0]);
            CheckPublishedScene(rig,*bridge,owner.SwapChain(),0xff0000ff,32,32);
            CheckPublishedScene(rig,*bridge,owner.SwapChain(),0xff28a828,32,80);
        }
        Success(owner.Latency()->Marker(sdkId,XELL_RENDERSUBMIT_END),"render end");
        Success(owner.Latency()->Marker(sdkId,XELL_PRESENT_START),"present start");
        Check(owner.Present(*prepared,admission.generate,0,0),"real Intel present");
        Success(owner.Latency()->Marker(sdkId,XELL_PRESENT_END),"present end");
        if (admission.tag) history.Accept(frame.sourceId,frame.sourceEpoch);
        const auto status=owner.Status();
        if (!visible && tick>=phaseFrames+20 && tick<phaseFrames+22) Require(!admission.tag && status.framesPresented<=1,"lost-focus source passes through without interpolation");
        if (request && focus && !menu && admission.generate) { ++onFrames;if (status.framesPresented>1 && status.frameGenResult==0) ++generated; }
        else { ++offFrames;Require(status.framesPresented<=1,"rejected/off history cannot interpolate"); }
        if (int(status.frameGenResult)<0) ++errors;
        if (tick%20==0 || int(status.frameGenResult)<0) std::printf("FRAME id=%u phase=%u focus=%d tag=%d generate=%d sdk_frames=%u enabled=%u result=%d\n",sdkId,phase,focus,admission.tag,admission.generate,status.framesPresented,status.isFrameGenEnabled,int(status.frameGenResult));
        if (tick==phaseFrames+10 && admission.tag) {
            const auto duplicate=history.Decide(frame,UpscaleOutcome::Temporal,true,true,false);
            Require(!duplicate.tag && !duplicate.generate,"duplicate source rejected by history");
            Require(owner.Present(*prepared,true,0,0)==E_UNEXPECTED,"extra Present cannot invent another source/marker cycle");
        }
        if (!visible && tick==phaseFrames*2) {
            Success(owner.Suspend(),"disable/drain before minimize");ShowWindow(window.Handle(),SW_MINIMIZE);window.Pump();
            ShowWindow(window.Handle(),SW_RESTORE);SetForegroundWindow(window.Handle());window.Pump();
            Success(owner.Resume(),"fixed-size restore");history.Invalidate();
        }
        deadline+=std::chrono::milliseconds(33);std::this_thread::sleep_until(deadline);
    }
    Success(owner.Suspend(),"final admissions stop and GPU drain");Success(owner.Retire(),"real Intel FG then XeLL destruction");
    rig.ValidateDebug();
    const bool qualified=!interrupted && generated>10 && onFrames>10 && errors==0 && (!visible || lostFocus==0);
    std::printf("SUMMARY qualified=%d visual_requested=%d interrupted=%d lost_focus_frames=%u active_sources=%u interpolated_sources=%u off_sources=%u errors=%u clean_retirement=1\n",qualified,visible,interrupted,lostFocus,onFrames,generated,offFrames,errors);
    std::puts(qualified?"PASS: actual Intel SDK generated frames and ordered retirement; visual smoothness/HUD confirmation separate":"NOT QUALIFIED: missing actual generation, focus, completion or clean status evidence");
    return qualified?0:2;
}
