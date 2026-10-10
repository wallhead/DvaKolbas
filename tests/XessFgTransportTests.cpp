#include "FrameGen/XessGenerationTransport.h"
#include "InteropTestRig.h"
#include "XessFgFrameFixture.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <array>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
static void Success(const Result<void>& result,const char* reason)
{ if (!result) std::fprintf(stderr,"ERROR %s native=%lld %s\n",reason,result.error().nativeResult,result.error().message.c_str());Require(bool(result),reason); }
static std::array<xefg_swapchain_d3d12_resource_data_t,4> tags;
static uint32_t tagCount{},constantCount{};
static xefg_swapchain_result_t sdkFailure{XEFG_SWAPCHAIN_RESULT_SUCCESS};
static ComPtr<ID3D12Resource> snapshots[3];
static D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprints[3];
static xefg_swapchain_result_t Capture(xefg_swapchain_handle_t,ID3D12CommandList* commandList,uint32_t id,const xefg_swapchain_d3d12_resource_data_t* desc)
{
    Require(id==17,"exact SDK ID in every tag");Require(tagCount<4,"one tag for each owned input");tags[tagCount++]=*desc;
    if (desc->type==XEFG_SWAPCHAIN_RES_HUDLESS_COLOR || desc->type==XEFG_SWAPCHAIN_RES_UI) {
        const auto index=desc->type==XEFG_SWAPCHAIN_RES_UI?1:0;
        ComPtr<ID3D12GraphicsCommandList> list;Check(commandList->QueryInterface(IID_PPV_ARGS(&list)),"snapshot command list");
        D3D12_TEXTURE_COPY_LOCATION from{};from.pResource=desc->pResource;from.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        D3D12_TEXTURE_COPY_LOCATION to{};to.pResource=snapshots[index].Get();to.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;to.PlacedFootprint=footprints[index];
        list->CopyTextureRegion(&to,0,0,0,&from,nullptr);
    }
    return sdkFailure;
}
static xefg_swapchain_result_t Constants(xefg_swapchain_handle_t,uint32_t id,const xefg_swapchain_frame_constant_data_t* constants)
{ Require(id==17 && constants->frameRenderTime>0,"constants use same SDK ID and measured time");++constantCount;return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
static xefg_swapchain_result_t NoopTag(xefg_swapchain_handle_t,ID3D12CommandList*,uint32_t,const xefg_swapchain_d3d12_resource_data_t*)
{ return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
static ComPtr<ID3D11Texture2D> Texture(Rig& rig,DXGI_FORMAT format,UINT binds)
{ auto desc=rig.Description();desc.Format=format;desc.BindFlags=binds;ComPtr<ID3D11Texture2D> result;Check(rig.device11->CreateTexture2D(&desc,nullptr,&result),"input texture");return result; }
int main()
{
    if (NrRuntimeResearch::GameRunningOrUnknown()) { std::puts("NOT QUALIFIED: Skyrim running or guard unavailable");return 2; }
    Rig rig;auto bridge=std::make_shared<Interop>();rig.Initialize(*bridge);
    XessGenerationTransport transport;Success(transport.Initialize(bridge,{3,2}),"owned transport initializes");
    for (int i=0;i<3;++i) {
        const auto desc=transport.Scene()->GetDesc();UINT64 bytes{};
        rig.device12->GetCopyableFootprints(&desc,0,1,0,&footprints[i],nullptr,nullptr,&bytes);
        D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_READBACK;
        D3D12_RESOURCE_DESC buffer{};buffer.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;buffer.Width=bytes;
        buffer.Height=buffer.DepthOrArraySize=buffer.MipLevels=1;buffer.SampleDesc.Count=1;buffer.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        Check(rig.device12->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&buffer,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&snapshots[i])),"owned-input snapshot");
    }
    auto scene=Texture(rig,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
    auto ui=Texture(rig,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
    auto depth=Texture(rig,DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS);
    auto motion=Texture(rig,DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
    // Upscaler scene alpha is not coverage. RGB must survive alpha 0 or 0.5,
    // while the separate premultiplied HUD must retain its real coverage.
    const std::array<uint32_t,6> scenePixels{0x00204080,0x00204080,0x80204080,0x80204080,0xff204080,0xff204080};
    const std::array<uint32_t,6> uiPixels{0x80402010,0,0x80402010,0,0x80402010,0};
    rig.context11->UpdateSubresource(scene.Get(),0,nullptr,scenePixels.data(),12,0);
    rig.context11->UpdateSubresource(ui.Get(),0,nullptr,uiPixels.data(),12,0);
    auto frame=XessFgFrame();frame.render=frame.subrect=frame.display=frame.depthExtent=frame.motionExtent={3,2};
    frame.output=scene.Get();frame.depth=depth.Get();frame.motion=motion.Get();frame.outputEncoding=frame.uiEncoding=ColorEncoding::SRGB;
    Require(!transport.Upload(frame,ui.Get(),nullptr,true),"pre-write wait required");
    Success(transport.WaitBeforeProducer(),"initial copy retirement");
    Require(!transport.Upload(frame,ui.Get(),nullptr,false),"incomplete HUD never admits stale UI");
    auto wrongDesc=rig.Description();wrongDesc.Width=1;ComPtr<ID3D11Texture2D> wrongUi;
    Check(rig.device11->CreateTexture2D(&wrongDesc,nullptr,&wrongUi),"wrong HUD size");
    Require(!transport.Upload(frame,wrongUi.Get(),nullptr,true),"wrong-sized HUD cannot tag stale UI");
    auto bad=frame;bad.outputEncoding=ColorEncoding::Unknown;Require(!transport.Upload(bad,ui.Get(),nullptr,true),"unknown output transfer rejected");
    Success(transport.Upload(frame,ui.Get(),nullptr,true),"ordered scene guides HUD upload");
    const auto parameters=AdaptXessGenerationFrame(frame,17);Require(bool(parameters),"CPU constants");
    XessGenerationFunctions api;api.TagFrameResource=Capture;api.TagFrameConstants=Constants;
    const auto list=transport.BeginTag();Require(bool(list),"owned tagging list");
    Require(!transport.Tag(nullptr,reinterpret_cast<xefg_swapchain_handle_t>(1),api,*parameters),"foreign or null recording cannot acknowledge SDK copies");
    Success(transport.Tag(*list,reinterpret_cast<xefg_swapchain_handle_t>(1),api,*parameters),"tag list sealed by actual submitted fence");
    Require(tagCount==4 && constantCount==1 && bridge->LastValue(Work::FrameGeneration)>0,"all four inputs and constants submitted");
    for (const auto& tag:tags) Require(tag.validity==XEFG_SWAPCHAIN_RV_ONLY_NOW && tag.incomingState==D3D12_RESOURCE_STATE_COPY_SOURCE &&
        tag.resourceBase.x==0 && tag.resourceBase.y==0 && tag.resourceSize.x==3 && tag.resourceSize.y==2 && tag.pResource,"ONLY_NOW COPY_SOURCE actual owned valid regions");
    Require(tags[0].pResource==transport.Scene() && tags[3].pResource==transport.Ui(),"scene and UI owned separately");
    D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_DEFAULT;
    auto desc=transport.Scene()->GetDesc();desc.Flags=D3D12_RESOURCE_FLAG_NONE;ComPtr<ID3D12Resource> backbuffer;
    Check(rig.device12->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&backbuffer)),"publication buffer");
    Success(transport.PublishTo(backbuffer.Get()),"same direct-queue real publication");
    ID3D12GraphicsCommandList* publication{};
    Check(bridge->Begin(Work::SwapChain,&publication),"final composition readback begin");
    D3D12_RESOURCE_BARRIER barrier{};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource=backbuffer.Get();barrier.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON;barrier.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    publication->ResourceBarrier(1,&barrier);
    D3D12_TEXTURE_COPY_LOCATION from{};from.pResource=backbuffer.Get();from.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    D3D12_TEXTURE_COPY_LOCATION to{};to.pResource=snapshots[2].Get();to.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;to.PlacedFootprint=footprints[2];
    publication->CopyTextureRegion(&to,0,0,0,&from,nullptr);
    std::swap(barrier.Transition.StateBefore,barrier.Transition.StateAfter);publication->ResourceBarrier(1,&barrier);
    Check(bridge->Submit(Work::SwapChain),"final composition readback submit");
    Check(bridge->Drain(),"snapshot tag queue completed");
    void* finalPixels{};Check(snapshots[2]->Map(0,nullptr,&finalPixels),"composed scene pixel readback");
    for(unsigned y=0;y<2;++y){
        const auto* row=reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(finalPixels)+footprints[2].Offset+y*footprints[2].Footprint.RowPitch);
        for(unsigned x=0;x<3;++x)if(!uiPixels[y*3+x])
            Require(row[x]==(scenePixels[y*3+x]|0xff000000u),"real image retains scene RGB regardless of upscaler alpha");
    }
    snapshots[2]->Unmap(0,nullptr);
    for (int i=0;i<2;++i) {
        void* mapped{};Check(snapshots[i]->Map(0,nullptr,&mapped),"owned-input pixel readback");
        const auto pixel=*reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(mapped)+footprints[i].Offset);
        snapshots[i]->Unmap(0,nullptr);
        Require(pixel==(i?uiPixels[0]:(scenePixels[0]|0xff000000u)),"opaque scene and premultiplied UI pixels preserved");
    }
    Success(transport.WaitBeforeProducer(),"actual copy fences before next writes");
    Require(!transport.Upload(frame,ui.Get(),nullptr,true),"duplicate source not uploaded twice");
    Success(transport.Retire(),"only transport copy work retires; no AMD SDK acknowledgments");
    Require(!transport.Scene() && !transport.Ui(),"proven retired resources released");
    // The shared bridge rejects a native device from a different adapter.
    ComPtr<IDXGIAdapter> warp;ComPtr<ID3D12Device> foreign;ComPtr<ID3D12CommandQueue> foreignQueue;
    Check(rig.factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)),"software foreign adapter");Check(D3D12CreateDevice(warp.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&foreign)),"foreign native device");
    D3D12_COMMAND_QUEUE_DESC queueDesc{};queueDesc.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;Check(foreign->CreateCommandQueue(&queueDesc,IID_PPV_ARGS(&foreignQueue)),"foreign queue");
    auto mismatch=std::make_shared<Interop>();Require(FAILED(mismatch->Initialize(rig.device11.Get(),foreign.Get(),foreignQueue.Get())),"mismatched render/native LUID rejected");
    auto stalledBridge=std::make_shared<Interop>();rig.Initialize(*stalledBridge);stalledBridge->SetRetirementWaitPolicy({10,40});
    XessGenerationTransport stalled;Success(stalled.Initialize(stalledBridge,{3,2}),"pending owner");
    Success(stalled.WaitBeforeProducer(),"pending initial wait");Success(stalled.Upload(frame,ui.Get(),nullptr,true),"pending upload");
    auto stalledList=stalled.BeginTag();Require(bool(stalledList),"pending tag list");
    ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"reader gate");
    Check(rig.queue->Wait(gate.Get(),1),"hold actual tag queue");tagCount=constantCount=0;
    Success(stalled.Tag(*stalledList,reinterpret_cast<xefg_swapchain_handle_t>(1),api,*parameters),"pending tag submit");
    auto* retained=stalled.Scene();Success(stalled.WaitBeforeProducer(),"queue native overwrite behind pending copy fence");
    auto resized=frame;resized.sourceId=2;resized.render=resized.subrect=resized.depthExtent=resized.motionExtent={1,1};
    const auto resizeAttempt=stalled.Upload(resized,ui.Get(),nullptr,true);
    Check(gate->Signal(1),"release held reader before assertion");
    ComPtr<ID3D12Fence> cleanup;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&cleanup)),"independent test cleanup fence");
    Check(rig.queue->Signal(cleanup.Get(),1),"independent test cleanup signal");
    const auto event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Require(event!=nullptr,"cleanup event");
    Check(cleanup->SetEventOnCompletion(1,event),"cleanup fence event");
    const auto waited=WaitForSingleObject(event,2000);CloseHandle(event);Require(waited==WAIT_OBJECT_0,"actual test copies complete before fixture readback release");
    Require(!resizeAttempt,"pending guide allocations cannot be replaced before CPU retirement");
    Require(stalled.Scene()==retained && !stalled.WaitBeforeProducer() && !stalled.Upload(resized,ui.Get(),nullptr,true) && !stalled.Retire(),"timeout retains owners and forbids reuse or false retirement");
    auto failureBridge=std::make_shared<Interop>();rig.Initialize(*failureBridge);
    XessGenerationTransport failing;Success(failing.Initialize(failureBridge,{3,2}),"SDK failure owner");
    Success(failing.WaitBeforeProducer(),"SDK failure pre-write wait");Success(failing.Upload(frame,ui.Get(),nullptr,true),"SDK failure upload");
    const auto failingList=failing.BeginTag();Require(bool(failingList),"SDK failure tag list");
    tagCount=constantCount=0;sdkFailure=XEFG_SWAPCHAIN_RESULT_ERROR_DEVICE;
    const auto failed=failing.Tag(*failingList,reinterpret_cast<xefg_swapchain_handle_t>(1),api,*parameters);
    Require(!failed && failed.error().kind==ErrorKind::DeviceLost,"SDK device failure preserved distinctly");
    Check(failureBridge->Drain(),"partially recorded SDK copy actually submitted before error");
    Require(failing.Scene() && !failing.WaitBeforeProducer() && !failing.BeginTag() && !failing.Retire(),"SDK failure retains owner and stops all reuse");
    // Deliberately closed native list models unexpected failure at Submit.
    // Ignore only the expected closed-list validation message during this case.
    if (rig.messages12) {
        D3D12_MESSAGE_ID expected=D3D12_MESSAGE_ID_COMMAND_LIST_CLOSED;
        D3D12_INFO_QUEUE_FILTER filter{};filter.DenyList.NumIDs=1;filter.DenyList.pIDList=&expected;
        Check(rig.messages12->PushStorageFilter(&filter),"expected submit-failure validation filter");
    }
    auto submitBridge=std::make_shared<Interop>();rig.Initialize(*submitBridge);
    XessGenerationTransport submitFailure;Success(submitFailure.Initialize(submitBridge,{3,2}),"submit failure owner");
    Success(submitFailure.WaitBeforeProducer(),"submit failure wait");Success(submitFailure.Upload(frame,ui.Get(),nullptr,true),"submit failure upload");
    const auto closed=submitFailure.BeginTag();Require(bool(closed),"submit failure list");Check((*closed)->Close(),"deliberately invalidate recording before Submit");
    auto noop=api;noop.TagFrameResource=NoopTag;
    const auto rejected=submitFailure.Tag(*closed,reinterpret_cast<xefg_swapchain_handle_t>(1),noop,*parameters);
    Require(!rejected && rejected.error().kind==ErrorKind::RetirementFailure && submitFailure.Scene() &&
        !submitFailure.WaitBeforeProducer() && !submitFailure.Upload(resized,ui.Get(),nullptr,true) && !submitFailure.Retire(),"failed native Submit cannot acknowledge copies, reuse or release owners");
    if (rig.messages12) rig.messages12->PopStorageFilter();
    rig.ValidateDebug();std::puts("PASS: owned Intel FG copies, direct queue, ONLY_NOW tags and copy-fence lifetime; actual SDK readers unqualified");
}
