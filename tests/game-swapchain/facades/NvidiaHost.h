#pragma once
#include <dxgi1_5.h>
#include <vector>
#include <d3d11.h>

class GameSwapChain;
// Only the host boundary is replaced. Each observable operation is recorded;
// all unexercised buffer/device/resize paths explicitly refuse admission.
class NvidiaHost
{
public:
    std::vector<int> events;
    HRESULT failure{S_OK}, prepareFailure{S_OK}, submitResult{S_OK}, completedResult{S_OK};
    bool fsrFg{}, xessFg{}, suspended{};
    UINT sync{}, flags{};
    ID3D11Device* producer{};
    ID3D11Texture2D* buffer{};
    HRESULT FailureResult() const { return failure; }
    bool FsrFgActive() const { return fsrFg; }
    bool XessFgActive() const { return xessFg; }
    bool NativeGenerationActive() const { return fsrFg || xessFg; }
    bool FsrActive() const { return false; }
    bool OrdinarySourceActive() const { return false; }
    bool FsrPresentSuspended() const { return suspended; }
    HRESULT UpdateFsrSuspension() { return S_OK; }
    HRESULT UpdateXessSuspension() { return suspended?DXGI_STATUS_OCCLUDED:S_OK; }
    HRESULT PresentXessSource(UINT s, UINT f) { return PresentFsrSource(s,f); }
    void Prepare() { events.push_back(1); failure=prepareFailure; }
    HRESULT PresentFsrSource(UINT s, UINT f) { sync=s;flags=f;events.push_back(2);return submitResult; }
    void OnPresentCompleted(HRESULT result) { events.push_back(3);completedResult=result; }
    void OnGameFacingSwapChainDestroyed(GameSwapChain*) {}
    void AdjustLegacyDescForCaller(DXGI_SWAP_CHAIN_DESC*, void*) {}
    HRESULT QueryFsrProducerDevice(REFIID iid, void** out) { return producer?producer->QueryInterface(iid,out):E_NOTIMPL; }
    HRESULT GetGameFacingBuffer(GameSwapChain*, UINT, REFIID iid, void** out) { return buffer?buffer->QueryInterface(iid,out):E_NOTIMPL; }
    HRESULT ResizeFsrSwapChain(GameSwapChain&, UINT, UINT, UINT, DXGI_FORMAT, UINT,
        const UINT* = nullptr, IUnknown* const* = nullptr) { return E_NOTIMPL; }
    HRESULT ResizeXessSwapChain(GameSwapChain&, UINT, UINT, UINT, DXGI_FORMAT, UINT) { return E_NOTIMPL; }
};
