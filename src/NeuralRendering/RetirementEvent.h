#pragma once
#include "Error.h"
#include <d3d12.h>
#include <chrono>
#include <algorithm>
#include <span>
#include <vector>
#include <wrl/client.h>
namespace TheosRenderPipeline::NeuralRendering {
// One serialized waiter, retained if retirement becomes uncertain. A fence
// completion event avoids dependence on the process's Windows timer period.
class RetirementEvent {
public:
    using Deadline=std::chrono::steady_clock::time_point;
    ~RetirementEvent(){if(event_)CloseHandle(event_);if(progress_)CloseHandle(progress_);}
    RetirementEvent()=default;
    RetirementEvent(const RetirementEvent&)=delete;
    struct Dependency {ID3D12Fence* fence;uint64_t value;};
    // Capacity waits observe every pending reader. Registrations retain their
    // fences and are deduplicated until genuine completion; a timeout slice
    // merely lets the caller rescan all slots under its original deadline.
    Result<void> WaitAny(std::span<const Dependency> readers,ID3D12Device* device,Deadline deadline){
        const auto fail=[](const char* why,int64_t code=0)->Result<void>{return std::unexpected(Error{ErrorKind::Retirement,code,why});};
        auto hr=device->GetDeviceRemovedReason();if(FAILED(hr))return fail("NR device removed during capacity retirement",hr);
        if(std::chrono::steady_clock::now()>=deadline)return fail("NR capacity retirement deadline exceeded; ownership retained");
        if(!progress_)progress_=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!progress_)return fail("NR capacity event allocation failed",GetLastError());
        std::erase_if(armed_,[](const Armed& r){return r.fence->GetCompletedValue()>=r.value;});
        bool pending=false;
        for(const auto& reader:readers){auto completed=reader.fence->GetCompletedValue();if(completed==UINT64_MAX)return fail("NR capacity reader reports device removal");if(completed>=reader.value)continue;pending=true;
            if(std::none_of(armed_.begin(),armed_.end(),[&](const Armed& r){return r.fence.Get()==reader.fence&&r.value==reader.value;})){
                armed_.push_back({reader.fence,reader.value});hr=reader.fence->SetEventOnCompletion(reader.value,progress_);if(FAILED(hr))return fail("NR capacity event registration failed",hr);
            }
        }
        if(!pending)return {};
        auto remaining=std::chrono::ceil<std::chrono::milliseconds>(deadline-std::chrono::steady_clock::now()).count();if(remaining<=0)return fail("NR capacity retirement deadline exceeded; ownership retained");
        auto status=WaitForSingleObject(progress_,DWORD(std::min<int64_t>(1000,remaining)));if(status!=WAIT_OBJECT_0&&status!=WAIT_TIMEOUT)return fail("NR capacity event wait failed",GetLastError());return {};
    }
    Result<void> Wait(ID3D12Fence* fence,uint64_t target,ID3D12Device* device,Deadline deadline){
        const auto fail=[](const char* why,int64_t code=0)->Result<void>{return std::unexpected(Error{ErrorKind::Retirement,code,why});};
        auto completed=fence->GetCompletedValue();if(completed==UINT64_MAX)return fail("NR retirement fence reports device removal");if(completed>=target)return {};
        if(!event_)event_=CreateEventW(nullptr,FALSE,FALSE,nullptr);if(!event_)return fail("NR retirement event allocation failed",GetLastError());
        auto hr=fence->SetEventOnCompletion(target,event_);if(FAILED(hr))return fail("NR retirement event registration failed",hr);
        while(completed<target){
            hr=device->GetDeviceRemovedReason();if(FAILED(hr))return fail("NR device removed during reader retirement",hr);
            auto remaining=deadline-std::chrono::steady_clock::now();if(remaining<=decltype(remaining)::zero())return fail("NR reader retirement deadline exceeded; ownership retained");
            auto ms=std::chrono::ceil<std::chrono::milliseconds>(remaining).count();auto status=WaitForSingleObject(event_,DWORD(std::min<int64_t>(1000,ms)));
            if(status!=WAIT_OBJECT_0&&status!=WAIT_TIMEOUT)return fail("NR reader retirement event wait failed",GetLastError());
            completed=fence->GetCompletedValue();if(completed==UINT64_MAX)return fail("NR retirement fence reports device removal");
        }
        return {};
    }
private:
    struct Armed {Microsoft::WRL::ComPtr<ID3D12Fence> fence;uint64_t value;};
    HANDLE event_{},progress_{};std::vector<Armed> armed_;
};
}
