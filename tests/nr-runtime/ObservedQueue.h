#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <atomic>

// Test-only forwarding queue: all work/device identity remains real. Only
// Wait may inject an HRESULT, to prove admission fails before vendor commands.
class ObservedQueue final : public ID3D12CommandQueue {
    std::atomic<ULONG> references_{1};
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> native_;
public:
    explicit ObservedQueue(ID3D12CommandQueue* queue):native_(queue){}
    UINT waits{};bool failWait{};
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id,void** out) override {
        if(!out)return E_POINTER;
        if(id==__uuidof(IUnknown)||id==__uuidof(ID3D12Object)||id==__uuidof(ID3D12DeviceChild)||id==__uuidof(ID3D12Pageable)||id==__uuidof(ID3D12CommandQueue)){
            *out=static_cast<ID3D12CommandQueue*>(this);AddRef();return S_OK;
        }
        *out=nullptr;return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override{return ++references_;}
    ULONG STDMETHODCALLTYPE Release() override{const auto n=--references_;if(!n)delete this;return n;}
    HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID id,UINT* size,void* data) override{return native_->GetPrivateData(id,size,data);}
    HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID id,UINT size,const void* data) override{return native_->SetPrivateData(id,size,data);}
    HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID id,const IUnknown* data) override{return native_->SetPrivateDataInterface(id,data);}
    HRESULT STDMETHODCALLTYPE SetName(LPCWSTR name) override{return native_->SetName(name);}
    HRESULT STDMETHODCALLTYPE GetDevice(REFIID id,void** out) override{return native_->GetDevice(id,out);}
    void STDMETHODCALLTYPE UpdateTileMappings(ID3D12Resource* a,UINT b,const D3D12_TILED_RESOURCE_COORDINATE* c,const D3D12_TILE_REGION_SIZE* d,ID3D12Heap* e,UINT f,const D3D12_TILE_RANGE_FLAGS* g,const UINT* h,const UINT* i,D3D12_TILE_MAPPING_FLAGS j) override{native_->UpdateTileMappings(a,b,c,d,e,f,g,h,i,j);}
    void STDMETHODCALLTYPE CopyTileMappings(ID3D12Resource* a,const D3D12_TILED_RESOURCE_COORDINATE* b,ID3D12Resource* c,const D3D12_TILED_RESOURCE_COORDINATE* d,const D3D12_TILE_REGION_SIZE* e,D3D12_TILE_MAPPING_FLAGS f) override{native_->CopyTileMappings(a,b,c,d,e,f);}
    void STDMETHODCALLTYPE ExecuteCommandLists(UINT n,ID3D12CommandList* const* lists) override{native_->ExecuteCommandLists(n,lists);}
    void STDMETHODCALLTYPE SetMarker(UINT a,const void* b,UINT c) override{native_->SetMarker(a,b,c);}
    void STDMETHODCALLTYPE BeginEvent(UINT a,const void* b,UINT c) override{native_->BeginEvent(a,b,c);}
    void STDMETHODCALLTYPE EndEvent() override{native_->EndEvent();}
    HRESULT STDMETHODCALLTYPE Signal(ID3D12Fence* fence,UINT64 value) override{return native_->Signal(fence,value);}
    HRESULT STDMETHODCALLTYPE Wait(ID3D12Fence* fence,UINT64 value) override{++waits;return failWait?E_FAIL:native_->Wait(fence,value);}
    HRESULT STDMETHODCALLTYPE GetTimestampFrequency(UINT64* value) override{return native_->GetTimestampFrequency(value);}
    HRESULT STDMETHODCALLTYPE GetClockCalibration(UINT64* gpu,UINT64* cpu) override{return native_->GetClockCalibration(gpu,cpu);}
    D3D12_COMMAND_QUEUE_DESC STDMETHODCALLTYPE GetDesc() override{return native_->GetDesc();}
};
