#include "InteropTestRig.h"
#include <atomic>
#include <thread>
using namespace InteropFixture;
// Inject the documented device-removal sentinel at the fence boundary. This
// tests production classification without removing a graphics device.
class TestFence final : public ID3D12Fence
{
    std::atomic<ULONG> references{1};std::atomic<unsigned>& signals;bool removed;
public:
    explicit TestFence(std::atomic<unsigned>& s,bool r=true):signals(s),removed(r){}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void** out)override{if(!out)return E_POINTER;*out=nullptr;if(iid==__uuidof(IUnknown)||iid==__uuidof(ID3D12Fence)){*out=static_cast<ID3D12Fence*>(this);AddRef();return S_OK;}return E_NOINTERFACE;}
    ULONG STDMETHODCALLTYPE AddRef()override{return ++references;}
    ULONG STDMETHODCALLTYPE Release()override{const auto left=--references;if(!left)delete this;return left;}
    HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID,UINT*,void*)override{return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID,UINT,const void*)override{return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID,const IUnknown*)override{return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetName(LPCWSTR)override{return S_OK;}
    HRESULT STDMETHODCALLTYPE GetDevice(REFIID,void**)override{return E_NOINTERFACE;}
    UINT64 STDMETHODCALLTYPE GetCompletedValue()override{return removed?UINT64_MAX:0;}
    HRESULT STDMETHODCALLTYPE SetEventOnCompletion(UINT64,HANDLE)override{return HRESULT_FROM_WIN32(WAIT_TIMEOUT);}
    HRESULT STDMETHODCALLTYPE Signal(UINT64)override{++signals;return E_FAIL;}
};
class RemovalProbe : public Interop
{
public:
    HRESULT Probe(std::atomic<unsigned>& signals){WorkContext work;work.fence12.Attach(new TestFence(signals));return WaitCPU(work,1);}
};
class TrackedAllocator final : public ID3D12CommandAllocator
{
    std::atomic<ULONG> references{1};bool& destroyed;
public:
    explicit TrackedAllocator(bool& d):destroyed(d){}
    ~TrackedAllocator(){destroyed=true;}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void** out)override{if(!out)return E_POINTER;*out=nullptr;if(iid==__uuidof(IUnknown)||iid==__uuidof(ID3D12CommandAllocator)){*out=static_cast<ID3D12CommandAllocator*>(this);AddRef();return S_OK;}return E_NOINTERFACE;}
    ULONG STDMETHODCALLTYPE AddRef()override{return ++references;}
    ULONG STDMETHODCALLTYPE Release()override{const auto left=--references;if(!left)delete this;return left;}
    HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID,UINT*,void*)override{return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID,UINT,const void*)override{return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID,const IUnknown*)override{return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetName(LPCWSTR)override{return S_OK;}
    HRESULT STDMETHODCALLTYPE GetDevice(REFIID,void**)override{return E_NOINTERFACE;}
    HRESULT STDMETHODCALLTYPE Reset()override{return E_UNEXPECTED;}
};
class AbandonmentProbe : public Interop
{
public:
    void AttachUnretired(ID3D12Fence* fence,ID3D12CommandAllocator* allocator)
    {auto& work=work_[0];work.value=1;work.fence12.Attach(fence);work.allocators[0].Attach(allocator);}
};
static void EmptyFrame(Interop& interop)
{ID3D12GraphicsCommandList* list{};Check(interop.SignalProducer(),"signal producer");Check(interop.Begin(&list),"begin source work");Check(interop.Submit(),"submit source work");Check(interop.WaitConsumer(),"queue consumer dependency");}
int main()
{
    Rig rig;
    {
        Interop interop;rig.Initialize(interop);EmptyFrame(interop);
        ComPtr<ID3D12Fence> gate12;ComPtr<ID3D11Fence> gate11;rig.SharedGate(gate12,gate11);
        Check(rig.context4->Wait(gate11.Get(),1),"delay final D3D11 output reader");
        HANDLE returned=CreateEventW(nullptr,FALSE,FALSE,nullptr);Require(returned!=nullptr,"reader test event");
        std::atomic<bool> early{false};
        std::thread release([&]{early=WaitForSingleObject(returned,100)==WAIT_OBJECT_0;Check(gate12->Signal(1),"release real output reader");});
        const auto drained=interop.Drain();SetEvent(returned);release.join();CloseHandle(returned);
        Check(drained,"drain retires final reader");Require(!early,"Drain cannot return while final D3D11 output reader is pending");
    }
    {
        Interop interop;rig.Initialize(interop);interop.SetRetirementWaitPolicy({10,40});
        ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"queue gate");
        Check(rig.queue->Wait(gate.Get(),1),"hold GPU submissions");
        for(int i=0;i<3;++i)EmptyFrame(interop);
        const auto slot=interop.CurrentSlot(Work::Upscaling);
        Check(interop.SignalProducer(),"fourth producer queued");const auto value=interop.LastValue(Work::Upscaling);
        ID3D12GraphicsCommandList* list{};const auto result=interop.Begin(&list);
        Require(result==HRESULT_FROM_WIN32(WAIT_TIMEOUT) && !list,"fourth allocator cannot reset before retirement");
        Require(interop.CurrentSlot(Work::Upscaling)==slot && interop.LastValue(Work::Upscaling)==value,"stall neither advances slot nor fabricates completion");
        Check(gate->Signal(1),"release stalled GPU through real gate");Check(interop.Drain(),"retire stalled objects before release");
    }
    {
        // A queued D3D11 read must consume the previous output before the next
        // D3D12 write, even with two frames submitted ahead of the GPU.
        Interop interop;rig.Initialize(interop);SharedTexture input,output;auto desc=rig.Description();
        Check(interop.CreateSharedTexture(desc,input),"delayed shared input");Check(interop.CreateSharedTexture(desc,output),"delayed shared output");
        ComPtr<ID3D11Texture2D> source,staging;Check(rig.device11->CreateTexture2D(&desc,nullptr,&source),"delayed source");
        auto readDesc=desc;readDesc.BindFlags=0;readDesc.Usage=D3D11_USAGE_STAGING;readDesc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
        Check(rig.device11->CreateTexture2D(&readDesc,nullptr,&staging),"delayed output readback");
        uint32_t first[6]{1,2,3,4,5,6},next[6]{10,20,30,40,50,60};ID3D12GraphicsCommandList* list{};
        auto submit=[&](const uint32_t* pixels){rig.context11->UpdateSubresource(source.Get(),0,nullptr,pixels,12,0);Check(interop.CopyInput(source.Get(),input),"delayed input copy");Check(interop.SignalProducer(),"delayed producer");Check(interop.Begin(&list),"delayed begin");Check(Interop::RecordCopy(list,input.texture12.Get(),output.texture12.Get()),"delayed output write");Check(interop.Submit(),"delayed submit");Check(interop.WaitConsumer(),"delayed consumer");};
        submit(first);
        ComPtr<ID3D12Fence> gate12;ComPtr<ID3D11Fence> gate11;rig.SharedGate(gate12,gate11);
        Check(rig.context4->Wait(gate11.Get(),1),"gate actual output reader");rig.context11->CopyResource(staging.Get(),output.texture11.Get());
        submit(next);
        ComPtr<ID3D12Fence> progress;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&progress)),"output progress fence");
        HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Require(event!=nullptr,"output progress event");
        Check(progress->SetEventOnCompletion(1,event),"observe following output work");Check(rig.queue->Signal(progress.Get(),1),"following output work");
        Require(WaitForSingleObject(event,20)==WAIT_TIMEOUT,"next output write remains gated by the previous D3D11 reader");
        Check(gate12->Signal(1),"release actual output reader");Require(WaitForSingleObject(event,2000)==WAIT_OBJECT_0,"output work resumes");CloseHandle(event);
        Check(interop.Drain(),"retire both output frames");D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"read delayed native output");
        for(unsigned row=0;row<2;++row)Require(!std::memcmp(static_cast<char*>(mapped.pData)+row*mapped.RowPitch,first+row*3,12),"delayed reader sees prior pixels without overwrite");rig.context11->Unmap(staging.Get(),0);
    }
    {
        Interop interop;rig.Initialize(interop);interop.SetRetirementWaitPolicy({10,100});
        ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"delayed gate");
        Check(rig.queue->Wait(gate.Get(),1),"delay multiple in-flight frames");for(int i=0;i<3;++i)EmptyFrame(interop);
        Check(interop.SignalProducer(),"producer before slot reuse");
        std::thread release([&]{Sleep(35);Check(gate->Signal(1),"advance retirement");});
        ID3D12GraphicsCommandList* list{};const auto begun=interop.Begin(&list);release.join();Check(begun,"reuse resumes after true retirement");
        Check(interop.Submit(),"submit reused slot");Check(interop.WaitConsumer(),"final consumer");Check(interop.Drain(),"retire reused frame");
    }
    std::atomic<unsigned> fakeSignals{};RemovalProbe removed;
    Require(removed.Probe(fakeSignals)==DXGI_ERROR_DEVICE_REMOVED && removed.Fault()==DXGI_ERROR_DEVICE_REMOVED && fakeSignals==0,"device-removal sentinel is a fault, never successful retirement or a fabricated signal");
    bool released=false;auto* pending=new TestFence(fakeSignals,false);auto* allocator=new TrackedAllocator(released);
    {AbandonmentProbe owner;owner.AttachUnretired(pending,allocator);}
    Require(!released && fakeSignals==0,"destructor preserves unretired allocator rather than signaling/freeing it on timeout");
    // Only the fixture objects are manually retired here; they own no GPU work.
    pending->Release();allocator->Release();Require(released,"fixture cleanup after abandonment observation");
    rig.ValidateDebug();
    std::puts("PASS: delayed output readers, three in-flight slots, no false completion on stall, reuse after retirement");
}
