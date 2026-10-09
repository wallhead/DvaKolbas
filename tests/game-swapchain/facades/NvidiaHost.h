#pragma once
#include <dxgi1_5.h>
#include <vector>

class GameSwapChain;
// Only the host boundary is replaced. Each observable operation is recorded;
// all unexercised buffer/device/resize paths explicitly refuse admission.
class NvidiaHost
{
public:
    std::vector<int> events;
    HRESULT failure{S_OK}, prepareFailure{S_OK}, submitResult{S_OK}, completedResult{S_OK};
    bool fsrFg{}, suspended{};
    UINT sync{}, flags{};
    HRESULT FailureResult() const { return failure; }
    bool FsrFgActive() const { return fsrFg; }
    bool FsrActive() const { return false; }
    bool OrdinarySourceActive() const { return false; }
    bool FsrPresentSuspended() const { return suspended; }
    HRESULT UpdateFsrSuspension() { return S_OK; }
    void Prepare() { events.push_back(1); failure=prepareFailure; }
    HRESULT PresentFsrSource(UINT s, UINT f) { sync=s;flags=f;events.push_back(2);return submitResult; }
    void OnPresentCompleted(HRESULT result) { events.push_back(3);completedResult=result; }
    void OnGameFacingSwapChainDestroyed(GameSwapChain*) {}
    void AdjustLegacyDescForCaller(DXGI_SWAP_CHAIN_DESC*, void*) {}
    HRESULT QueryFsrProducerDevice(REFIID, void**) { return E_NOTIMPL; }
    HRESULT GetGameFacingBuffer(GameSwapChain*, UINT, REFIID, void**) { return E_NOTIMPL; }
    HRESULT ResizeFsrSwapChain(GameSwapChain&, UINT, UINT, UINT, DXGI_FORMAT, UINT,
        const UINT* = nullptr, IUnknown* const* = nullptr) { return E_NOTIMPL; }
};
