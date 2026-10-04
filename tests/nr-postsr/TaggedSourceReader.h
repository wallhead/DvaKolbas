#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <chrono>
#include <cstring>
#include <thread>
#include <vector>

// Test-only independent GPU reader. CPU releases only the gate; completion is
// signalled by the reader queue after copying the actual shared tagged texture.
// The enclosing submission capsule must retain this object on uncertain work.
class TaggedSourceReader {
    template<class T> using ComPtr=Microsoft::WRL::ComPtr<T>;
    static void Need(HRESULT hr){if(FAILED(hr))throw hr;}
public:
    TaggedSourceReader(ID3D12Device* device,UINT width,UINT height):width_(width),height_(height),pitch_((width*4+255)&~255){
        D3D12_COMMAND_QUEUE_DESC q{};Need(device->CreateCommandQueue(&q,IID_PPV_ARGS(&queue_)));
        Need(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate_)));
        Need(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&done_)));
        Need(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator_)));
        Need(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator_.Get(),nullptr,IID_PPV_ARGS(&list_)));Need(list_->Close());
        D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_READBACK;D3D12_RESOURCE_DESC d{};
        d.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;d.Width=uint64_t(pitch_)*height_;d.Height=1;
        d.DepthOrArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        Need(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&capture_)));
    }
    void Arm(ID3D12Resource* source,const std::vector<unsigned char>& expected){
        if(Pending()||expected.size()!=size_t(width_)*height_*4)throw E_UNEXPECTED;
        Verify();source_=source;expected_=expected;Need(allocator_->Reset());Need(list_->Reset(allocator_.Get(),nullptr));
        D3D12_RESOURCE_BARRIER b{};b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition={source,D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_SOURCE};list_->ResourceBarrier(1,&b);
        D3D12_TEXTURE_COPY_LOCATION from{};from.pResource=source;from.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        D3D12_TEXTURE_COPY_LOCATION to{};to.pResource=capture_.Get();to.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        to.PlacedFootprint.Footprint={DXGI_FORMAT_R8G8B8A8_UNORM,width_,height_,1,pitch_};list_->CopyTextureRegion(&to,0,0,0,&from,nullptr);
        std::swap(b.Transition.StateBefore,b.Transition.StateAfter);list_->ResourceBarrier(1,&b);Need(list_->Close());
        ++value_;
        // Start before any queue wait. Exceptional unwind never depends on an
        // unstarted CPU helper, and no helper signals the completion fence.
        release_=std::jthread([gate=gate_,value=value_]{std::this_thread::sleep_for(std::chrono::milliseconds(500));gate->Signal(value);});
        Need(queue_->Wait(gate_.Get(),value_));ID3D12CommandList* lists[]{list_.Get()};queue_->ExecuteCommandLists(1,lists);Need(queue_->Signal(done_.Get(),value_));
    }
    void Verify(){
        if(verified_==value_)return;
        HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!event)throw HRESULT_FROM_WIN32(GetLastError());
        const auto hr=done_->SetEventOnCompletion(value_,event);
        const auto wait=SUCCEEDED(hr)?WaitForSingleObject(event,3000):WAIT_FAILED;CloseHandle(event);Need(hr);
        if(wait!=WAIT_OBJECT_0||done_->GetCompletedValue()==UINT64_MAX)throw HRESULT_FROM_WIN32(WAIT_TIMEOUT);
        D3D12_RANGE range{0,size_t(pitch_)*height_};void* mapped{};Need(capture_->Map(0,&range,&mapped));
        for(UINT y=0;y<height_;++y)for(UINT x=0;x<width_*4;++x)exact_+=static_cast<unsigned char*>(mapped)[size_t(y)*pitch_+x]==expected_[size_t(y)*width_*4+x];
        expectedBytes_+=expected_.size();D3D12_RANGE noWrite{};capture_->Unmap(0,&noWrite);verified_=value_;++captures_;
    }
    bool Pending()const{return value_&&done_->GetCompletedValue()<value_;}
    bool Completed()const{return done_->GetCompletedValue()!=UINT64_MAX&&done_->GetCompletedValue()>=value_;}
    ID3D12Fence* Fence()const{return done_.Get();}
    uint64_t Value()const{return value_;}
    uint64_t Exact()const{return exact_;}
    uint64_t Expected()const{return expectedBytes_;}
    unsigned Captures()const{return captures_;}
private:
    ComPtr<ID3D12CommandQueue> queue_;ComPtr<ID3D12Fence> gate_,done_;
    ComPtr<ID3D12CommandAllocator> allocator_;ComPtr<ID3D12GraphicsCommandList> list_;
    ComPtr<ID3D12Resource> capture_,source_;UINT width_{},height_{},pitch_{};
    uint64_t value_{},verified_{},exact_{},expectedBytes_{};unsigned captures_{};
    std::vector<unsigned char> expected_;std::jthread release_;
};
