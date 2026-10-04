#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <thread>
#include <chrono>
// Test-only presenting-queue delay. It never substitutes the vendor's fence.
// The submitted-resource capsule retains this object on uncertain retirement.
class QueuedPresentationGate {
    Microsoft::WRL::ComPtr<ID3D12Fence> gate_;uint64_t value_{};std::jthread release_;
public:
    explicit QueuedPresentationGate(ID3D12Device* device){const auto h=device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate_));if(FAILED(h))throw h;}
    void Arm(ID3D12CommandQueue* queue){
        ++value_;
        release_=std::jthread([gate=gate_,value=value_]{std::this_thread::sleep_for(std::chrono::milliseconds(500));gate->Signal(value);});
        // The CPU release is running before a graphics queue can wait for it.
        const auto h=queue->Wait(gate_.Get(),value_);if(FAILED(h))throw h;
    }
};
