#pragma once
#include "Error.h"
#include <d3d12.h>
#include <chrono>
#include <algorithm>
namespace TheosRenderPipeline::NeuralRendering {
// One serialized waiter, retained if retirement becomes uncertain. A fence
// completion event avoids dependence on the process's Windows timer period.
class RetirementEvent {
public:
    using Deadline=std::chrono::steady_clock::time_point;
    ~RetirementEvent(){if(event_)CloseHandle(event_);}
    RetirementEvent()=default;
    RetirementEvent(const RetirementEvent&)=delete;
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
private:HANDLE event_{};
};
}
