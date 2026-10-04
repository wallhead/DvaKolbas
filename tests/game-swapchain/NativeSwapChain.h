#pragma once
#include "NvidiaHost.h"
#include <atomic>

// Recording native DXGI boundary. Unused COM methods return E_NOTIMPL.
class NativeSwapChain final : public IDXGISwapChain4
{
public:
    NvidiaHost* host{};
    std::atomic<ULONG> references{1};
    bool interfaces{true};
    const DXGI_PRESENT_PARAMETERS* parameters{};
    HRESULT NativePresent(UINT s, UINT f) { host->sync=s;host->flags=f;host->events.push_back(2);return host->submitResult; }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID a_iid, void** a_object) override { if(!a_object)return E_POINTER;*a_object=nullptr;const bool base=a_iid==__uuidof(IUnknown)||a_iid==__uuidof(IDXGIObject)||a_iid==__uuidof(IDXGIDeviceSubObject)||a_iid==__uuidof(IDXGISwapChain);const bool ext=a_iid==__uuidof(IDXGISwapChain1)||a_iid==__uuidof(IDXGISwapChain2)||a_iid==__uuidof(IDXGISwapChain3)||a_iid==__uuidof(IDXGISwapChain4);if(!base&&!(interfaces&&ext))return E_NOINTERFACE;*a_object=static_cast<IDXGISwapChain4*>(this);AddRef();return S_OK; }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++references; }
    ULONG STDMETHODCALLTYPE Release() override { return --references; }
    HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID a_name, UINT a_size, const void* a_data) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID a_name, const IUnknown* a_unknown) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID a_name, UINT* a_size, void* a_data) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetParent(REFIID a_iid, void** a_parent) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetDevice(REFIID a_iid, void** a_device) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Present(UINT a_syncInterval, UINT a_flags) override { return NativePresent(a_syncInterval,a_flags); }
    HRESULT STDMETHODCALLTYPE GetBuffer(UINT a_buffer, REFIID a_iid, void** a_surface) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL a_fullscreen, IDXGIOutput* a_target) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetFullscreenState(BOOL* a_fullscreen, IDXGIOutput** a_target) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC* a_desc) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT a_bufferCount, UINT a_width, UINT a_height, DXGI_FORMAT a_format, UINT a_flags) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC* a_desc) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput** a_output) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS* a_stats) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT* a_count) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetDesc1(DXGI_SWAP_CHAIN_DESC1* a_desc) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* a_desc) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetHwnd(HWND* a_window) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetCoreWindow(REFIID a_iid, void** a_window) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Present1(UINT a_syncInterval, UINT a_flags, const DXGI_PRESENT_PARAMETERS* a_parameters) override { parameters=a_parameters;return NativePresent(a_syncInterval,a_flags); }
    BOOL STDMETHODCALLTYPE IsTemporaryMonoSupported() override { return FALSE; }
    HRESULT STDMETHODCALLTYPE GetRestrictToOutput(IDXGIOutput** a_output) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetBackgroundColor(const DXGI_RGBA* a_color) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetBackgroundColor(DXGI_RGBA* a_color) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetRotation(DXGI_MODE_ROTATION a_rotation) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetRotation(DXGI_MODE_ROTATION* a_rotation) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetSourceSize(UINT a_width, UINT a_height) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetSourceSize(UINT* a_width, UINT* a_height) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(UINT a_latency) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(UINT* a_latency) override { return E_NOTIMPL; }
    HANDLE STDMETHODCALLTYPE GetFrameLatencyWaitableObject() override { return nullptr; }
    HRESULT STDMETHODCALLTYPE SetMatrixTransform(const DXGI_MATRIX_3X2_F* a_matrix) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetMatrixTransform(DXGI_MATRIX_3X2_F* a_matrix) override { return E_NOTIMPL; }
    UINT STDMETHODCALLTYPE GetCurrentBackBufferIndex() override { return 0; }
    HRESULT STDMETHODCALLTYPE CheckColorSpaceSupport(DXGI_COLOR_SPACE_TYPE a_colorSpace, UINT* a_support) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetColorSpace1(DXGI_COLOR_SPACE_TYPE a_colorSpace) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE ResizeBuffers1(UINT a_bufferCount, UINT a_width, UINT a_height, DXGI_FORMAT a_format, UINT a_flags, const UINT* a_creationNodeMask, IUnknown* const* a_presentQueue) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetHDRMetaData(DXGI_HDR_METADATA_TYPE a_type, UINT a_size, void* a_metadata) override { return E_NOTIMPL; }
};
