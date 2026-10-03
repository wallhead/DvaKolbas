#include "InteropTestRig.h"
using namespace InteropFixture;
int main()
{
    Rig rig;Interop interop;rig.Initialize(interop);
    ID3D12GraphicsCommandList* list{};
    Require(FAILED(interop.Begin(&list)) && !list,"unsubmitted producer cannot start ordinary dispatch");
    ComPtr<ID3D12Fence> producerFence;uint64_t producerValue=42;
    Require(FAILED(interop.ProducerDependency(&producerFence,&producerValue))&&!producerFence&&!producerValue,"unsubmitted producer exports no dependency");
    ComPtr<IDXGIAdapter> warp;ComPtr<ID3D12Device> foreign;Check(rig.factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)),"foreign WARP adapter");
    Check(D3D12CreateDevice(warp.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&foreign)),"foreign actual device");
    Interop mismatch;Require(mismatch.Initialize(rig.device11.Get(),foreign.Get(),rig.queue.Get())==E_INVALIDARG,"adapter mismatch rejected");
    D3D12_COMMAND_QUEUE_DESC q{};q.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;ComPtr<ID3D12CommandQueue> foreignQueue;
    Check(foreign->CreateCommandQueue(&q,IID_PPV_ARGS(&foreignQueue)),"foreign queue");
    Interop queueMismatch;Require(queueMismatch.Initialize(rig.device11.Get(),rig.device12.Get(),foreignQueue.Get())==E_INVALIDARG,"foreign queue device rejected");
    SharedTexture input,output;const auto desc=rig.Description();
    SharedTexture invalid;auto illegal=desc;illegal.SampleDesc.Count=2;
    Require(interop.CreateSharedTexture(illegal,invalid)==E_INVALIDARG && !invalid.texture11 && !invalid.texture12,"multisampled shared texture rejected before allocation");
    illegal=desc;illegal.BindFlags|=D3D11_BIND_DEPTH_STENCIL;
    Require(interop.CreateSharedTexture(illegal,invalid)==E_INVALIDARG,"depth-stencil shared descriptor rejected");
    Check(interop.CreateSharedTexture(desc,input),"shared input");Check(interop.CreateSharedTexture(desc,output),"shared output");
    Require(interop.CopyInput(input.texture11.Get(),input)==E_INVALIDARG,"self-copy rejected");
    auto wrongDesc=desc;wrongDesc.Width=2;ComPtr<ID3D11Texture2D> wrongSource;
    Check(rig.device11->CreateTexture2D(&wrongDesc,nullptr,&wrongSource),"wrong-size source");
    Require(interop.CopyInput(wrongSource.Get(),input)==E_INVALIDARG && interop.LastValue(Work::Upscaling)==0,"mismatched input rejected without a producer submission");
    ComPtr<ID3D11Texture2D> source,staging;Check(rig.device11->CreateTexture2D(&desc,nullptr,&source),"source texture");
    auto readDesc=desc;readDesc.BindFlags=0;readDesc.Usage=D3D11_USAGE_STAGING;readDesc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
    Check(rig.device11->CreateTexture2D(&readDesc,nullptr,&staging),"staging readback");
    for(unsigned frame=0;frame<12;++frame){
        uint32_t pixels[6];for(unsigned i=0;i<6;++i)pixels[i]=0xff001000+frame*17+i;
        rig.context11->UpdateSubresource(source.Get(),0,nullptr,pixels,12,0);
        Check(interop.CopyInput(source.Get(),input),"copy source once");Check(interop.SignalProducer(),"submit/flush producer");
        Check(interop.ProducerDependency(&producerFence,&producerValue),"capture actual submitted producer");
        Require(producerFence&&producerValue==interop.LastValue(Work::Upscaling),"producer dependency matches real bridge signal");
        Check(interop.Begin(&list),"begin only after producer");Check(Interop::RecordCopy(list,input.texture12.Get(),output.texture12.Get()),"COMMON-state D3D12 copy");
        Check(interop.Submit(),"submit D3D12 work");Check(interop.WaitConsumer(),"D3D11 waits for native output");
        Require(FAILED(interop.ProducerDependency(&producerFence,&producerValue))&&!producerFence&&!producerValue,"consumer phase exports no stale producer");
        rig.context11->CopyResource(staging.Get(),output.texture11.Get());Check(interop.Drain(),"retire native output readback");
        D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"read GPU pixels");
        for(unsigned row=0;row<2;++row)Require(!std::memcmp(static_cast<char*>(mapped.pData)+row*mapped.RowPitch,pixels+row*3,12),"changing exact pixels survive D3D11/D3D12 round trip");
        rig.context11->Unmap(staging.Get(),0);
    }
    rig.ValidateDebug();
    std::puts("PASS: producer ordering, actual adapter/queue rejection, twelve changing hardware copies");
}
