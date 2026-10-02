#include "FrameGen/FSRPresentationTransport.h"
#include "InteropTestRig.h"
#include <array>
#include <chrono>
using namespace InteropFixture;
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
ComPtr<ID3D12Resource> AppBuffer(Rig& rig)
{
    D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_DESC d{};d.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D;d.Width=3;d.Height=2;d.DepthOrArraySize=d.MipLevels=1;
    d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.SampleDesc.Count=1;
    ComPtr<ID3D12Resource> buffer;Check(rig.device12->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&buffer)),"application buffer");return buffer;
}
void ExpectPixel(Rig& rig,ID3D11Texture2D* input,std::uint32_t expected,const char* name)
{
    auto d=rig.Description();d.Usage=D3D11_USAGE_STAGING;d.BindFlags=0;d.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Texture2D> staging;Check(rig.device11->CreateTexture2D(&d,nullptr,&staging),"staging");rig.context11->CopyResource(staging.Get(),input);
    D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"transport readback");
    Require(*static_cast<std::uint32_t*>(mapped.pData)==expected,name);rig.context11->Unmap(staging.Get(),0);
}
int main()
{
    Rig rig;auto bridge=std::make_shared<Interop>();rig.Initialize(*bridge);
    FsrPresentationTransport transport;Check(transport.Initialize(bridge,{3,2}),"initialize transport");
    ComPtr<ID3D11Device> producer;transport.SceneTarget11()->GetDevice(&producer);
    Require(D3D11FrameCopy::SameObject(producer.Get(),rig.device11.Get()),"ProducerDeviceIdentityPreserved");
    auto d=rig.Description();ComPtr<ID3D11Texture2D> ui;Check(rig.device11->CreateTexture2D(&d,nullptr,&ui),"completed HUD");
    const std::array<std::uint32_t,6> zero{};rig.context11->UpdateSubresource(ui.Get(),0,nullptr,zero.data(),12,0);
    Check(transport.WaitBeforeProducer(),"initial producer boundary");
    Require(FAILED(transport.Upload(transport.SceneTarget11(),ColorEncoding::SRGB,ui.Get(),nullptr,false,41)),"UnfinishedUiCannotPublish");
    Check(transport.Upload(transport.SceneTarget11(),ColorEncoding::SRGB,ui.Get(),nullptr,true,41),"completed source 41");
    auto app=AppBuffer(rig);Check(transport.PublishTo(app.Get()),"publication copies");Check(transport.MarkUiRegistered(),"SDK UI registration");
    Check(transport.WaitBeforeProducer(),"guide preparation independent of Present");
    Check(transport.Upload(transport.SceneTarget11(),ColorEncoding::SRGB,ui.Get(),nullptr,true,42),"next D3D11 publication source");
    Require(FAILED(transport.PublishTo(app.Get())),"UiRegistrationAloneDoesNotPermitReuse");
    Check(transport.NotifyPresentReturned(S_OK),"internal-buffered Present return");
    Check(transport.PublishTo(app.Get()),"UiMayReuseAfterPresentReturnsWithInternalBuffering");Check(transport.MarkUiRegistered(),"next registration");
    Check(transport.NotifyPresentReturned(S_OK),"second Present return");Check(bridge->Drain(),"main queue drained");
    auto* retainedUi=transport.Resources().ui;
    Require(FAILED(transport.Retire()) && transport.Resources().ui==retainedUi,"MainQueueFenceDoesNotClaimAsyncPresentRetirement");
    Require(FAILED(transport.AcknowledgeSdkRetirement(E_FAIL)) && transport.Resources().ui==retainedUi,"FailedSdkRetirementKeepsOwners");
    Check(transport.AcknowledgeSdkRetirement(S_OK),"owner acknowledges unregister and WaitForPresents");Check(transport.Retire(),"full transport retirement");
    Require(!transport.Resources().ui,"successful retirement releases publication UI");

    // Hold an actual D3D12 guide reader, enqueue a D3D11 overwrite, then release.
    // Without the pre-write wait the delayed reader would observe the new pixel.
    FsrPresentationTransport delayed;Check(delayed.Initialize(bridge,{3,2}),"delayed transport");
    SharedTexture guide,snapshot;Check(bridge->CreateSharedTexture(d,guide),"guide");Check(bridge->CreateSharedTexture(d,snapshot),"snapshot");
    const std::array<std::uint32_t,6> old{0xff112233,0xff112233,0xff112233,0xff112233,0xff112233,0xff112233};
    const std::array<std::uint32_t,6> newer{0xff445566,0xff445566,0xff445566,0xff445566,0xff445566,0xff445566};
    rig.context11->UpdateSubresource(guide.texture11.Get(),0,nullptr,old.data(),12,0);Check(bridge->SignalD3D11(Work::FrameGeneration),"guide producer");
    ID3D12GraphicsCommandList* list{};Check(bridge->Begin(Work::FrameGeneration,&list),"prepare reader list");
    ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"reader gate");
    Check(rig.queue->Wait(gate.Get(),1),"hold Prepare reader");Check(Interop::RecordCopy(list,guide.texture12.Get(),snapshot.texture12.Get()),"reader snapshot");
    Check(delayed.RecordPrepareRetirement(),"signal immediately after Prepare submission");
    auto before=std::chrono::steady_clock::now();Check(delayed.WaitBeforeProducer(),"pre-write guide wait");
    Require(std::chrono::steady_clock::now()-before<std::chrono::milliseconds(100),"PrepareRetirementDoesNotWaitForPresent");
    rig.context11->UpdateSubresource(guide.texture11.Get(),0,nullptr,newer.data(),12,0);Check(bridge->SignalD3D11(Work::SwapChain),"new producer submitted");
    Sleep(40); // Give an incorrectly unguarded producer time to overtake the held reader.
    Check(gate->Signal(1),"release delayed reader");Check(bridge->Drain(),"reader and producer retired");
    ExpectPixel(rig,snapshot.texture11.Get(),old[0],"DelayedPrepareReaderBlocksDepthOverwrite");ExpectPixel(rig,guide.texture11.Get(),newer[0],"ProducerProgressAfterReaderRetirement");
    Check(delayed.Retire(),"delayed transport retired");

    // Native scene work can be queued before Upload. Retirement must seal that
    // D3D11 work, not just wait for fences from a previous source.
    auto producerBridge=std::make_shared<Interop>();rig.Initialize(*producerBridge);producerBridge->SetRetirementWaitPolicy({10,40});
    FsrPresentationTransport producerPending;Check(producerPending.Initialize(producerBridge,{3,2}),"producer retirement transport");
    ComPtr<ID3D12Fence> producerGate;ComPtr<ID3D11Fence> producerGate11;rig.SharedGate(producerGate,producerGate11);
    Check(rig.context4->Wait(producerGate11.Get(),1),"hold native D3D11 scene producer");
    rig.context11->UpdateSubresource(producerPending.SceneTarget11(),0,nullptr,old.data(),12,0);rig.context11->Flush();
    auto* producerOwner=producerPending.SceneTarget11();
    const auto producerRetirement=producerPending.Retire();Check(producerGate->Signal(1),"release native producer after test");
    Require(FAILED(producerRetirement) && producerPending.SceneTarget11()==producerOwner,"UnpublishedD3D11ProducerMustRetireBeforeRelease");

    auto stalledBridge=std::make_shared<Interop>();rig.Initialize(*stalledBridge);stalledBridge->SetRetirementWaitPolicy({10,40});
    FsrPresentationTransport stalled;Check(stalled.Initialize(stalledBridge,{3,2}),"stalled transport");
    Check(stalledBridge->Begin(Work::FrameGeneration,&list),"stalled Prepare list");
    ComPtr<ID3D12Fence> stalledGate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&stalledGate)),"stalled gate");
    Check(rig.queue->Wait(stalledGate.Get(),1),"hold stalled queue");Check(stalled.RecordPrepareRetirement(),"stalled reader submit");
    auto* retained=stalled.Resources().scene;
    Require(FAILED(stalled.Retire()) && stalled.Resources().scene==retained,"DeviceRemovedOrTimeoutDoesNotRelease");
    Check(stalledGate->Signal(1),"allow GPU cleanup after intentional timeout");
    rig.ValidateDebug();std::puts("PASS: independent Prepare/copy fences, real delayed guide pixels, UI borrowing and failure ownership retention");
}
