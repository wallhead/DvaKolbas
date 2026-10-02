#pragma once
#include <dxgi1_6.h>
#include <atomic>
#include <array>
static std::array<std::atomic<unsigned>,13> presentationStats{};
extern "C" __declspec(dllexport) unsigned FixturePresentationStat(unsigned index){return index<presentationStats.size()?presentationStats[index].load():0;}
class FixtureSwapChain final : public IDXGISwapChain4 {
    std::atomic<ULONG> references_{1};bool buffersInitialized_{};
    Microsoft::WRL::ComPtr<IDXGISwapChain4> inner_;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
public:
    explicit FixtureSwapChain(IDXGISwapChain4* inner):inner_(inner){inner_->GetDevice(IID_PPV_ARGS(&device_));}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID a_iid, void** a_object) override {if(!a_object)return E_POINTER;*a_object=nullptr;if(a_iid==__uuidof(IUnknown)||a_iid==__uuidof(IDXGIObject)||a_iid==__uuidof(IDXGIDeviceSubObject)||a_iid==__uuidof(IDXGISwapChain)||a_iid==__uuidof(IDXGISwapChain1)||a_iid==__uuidof(IDXGISwapChain2)||a_iid==__uuidof(IDXGISwapChain3)||a_iid==__uuidof(IDXGISwapChain4)){*a_object=static_cast<IDXGISwapChain4*>(this);AddRef();return S_OK;}return E_NOINTERFACE;}
    ULONG STDMETHODCALLTYPE AddRef() override {return ++references_;}
    ULONG STDMETHODCALLTYPE Release() override {auto remaining=--references_;if(!remaining)delete this;return remaining;}
    HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID a_name, UINT a_size, const void* a_data) override {return inner_->SetPrivateData(a_name,a_size,a_data);}
    HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID a_name, const IUnknown* a_unknown) override {return inner_->SetPrivateDataInterface(a_name,a_unknown);}
    HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID a_name, UINT* a_size, void* a_data) override {return inner_->GetPrivateData(a_name,a_size,a_data);}
    HRESULT STDMETHODCALLTYPE GetParent(REFIID a_iid, void** a_parent) override {return inner_->GetParent(a_iid,a_parent);}
    HRESULT STDMETHODCALLTYPE GetDevice(REFIID a_iid, void** a_device) override {return inner_->GetDevice(a_iid,a_device);}
    HRESULT STDMETHODCALLTYPE Present(UINT a_syncInterval, UINT a_flags) override {if(a_flags&DXGI_PRESENT_TEST)return inner_->Present(a_syncInterval,a_flags);if(!buffersInitialized_)return E_FAIL;
        presentationStats[12]=1;if(mode==22)Sleep(120);
        if(fgConfiguration.frameGenerationEnabled && fgConfiguration.frameGenerationCallback){
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> list;
            if(FAILED(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator))) || FAILED(device_->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list))))return E_FAIL;
            Microsoft::WRL::ComPtr<ID3D12Resource> buffer;if(FAILED(inner_->GetBuffer(inner_->GetCurrentBackBufferIndex(),IID_PPV_ARGS(&buffer))))return E_FAIL;
            ffxDispatchDescFrameGeneration descriptor{};descriptor.header.type=FFX_API_DISPATCH_DESC_TYPE_FRAMEGENERATION;
            descriptor.commandList=list.Get();descriptor.presentColor=ffxApiGetResourceDX12(buffer.Get());descriptor.outputs[0]=descriptor.presentColor;
            descriptor.numGeneratedFrames=1;descriptor.frameID=fgConfiguration.frameID;descriptor.backbufferTransferFunction=FFX_API_BACKBUFFER_TRANSFER_FUNCTION_SRGB;
            const auto result=fgConfiguration.frameGenerationCallback(&descriptor,fgConfiguration.frameGenerationCallbackUserContext);
            list->Close();if(result!=FFX_API_RETURN_OK)return E_FAIL;
        }
        return inner_->Present(a_syncInterval,a_flags);}
    HRESULT STDMETHODCALLTYPE GetBuffer(UINT a_buffer, REFIID a_iid, void** a_surface) override {auto hr=inner_->GetBuffer(a_buffer,a_iid,a_surface);if(SUCCEEDED(hr))buffersInitialized_=true;return hr;}
    HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL a_fullscreen, IDXGIOutput* a_target) override {return inner_->SetFullscreenState(a_fullscreen,a_target);}
    HRESULT STDMETHODCALLTYPE GetFullscreenState(BOOL* a_fullscreen, IDXGIOutput** a_target) override {return inner_->GetFullscreenState(a_fullscreen,a_target);}
    HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC* a_desc) override {return inner_->GetDesc(a_desc);}
    HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT a_bufferCount, UINT a_width, UINT a_height, DXGI_FORMAT a_format, UINT a_flags) override {return inner_->ResizeBuffers(a_bufferCount,a_width,a_height,a_format,a_flags);}
    HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC* a_desc) override {return inner_->ResizeTarget(a_desc);}
    HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput** a_output) override {return inner_->GetContainingOutput(a_output);}
    HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS* a_stats) override {return inner_->GetFrameStatistics(a_stats);}
    HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT* a_count) override {return inner_->GetLastPresentCount(a_count);}
    HRESULT STDMETHODCALLTYPE GetDesc1(DXGI_SWAP_CHAIN_DESC1* a_desc) override {return inner_->GetDesc1(a_desc);}
    HRESULT STDMETHODCALLTYPE GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* a_desc) override {return inner_->GetFullscreenDesc(a_desc);}
    HRESULT STDMETHODCALLTYPE GetHwnd(HWND* a_window) override {return inner_->GetHwnd(a_window);}
    HRESULT STDMETHODCALLTYPE GetCoreWindow(REFIID a_iid, void** a_window) override {return inner_->GetCoreWindow(a_iid,a_window);}
    HRESULT STDMETHODCALLTYPE Present1(UINT a_syncInterval, UINT a_flags, const DXGI_PRESENT_PARAMETERS* a_parameters) override {return inner_->Present1(a_syncInterval,a_flags,a_parameters);}
    BOOL STDMETHODCALLTYPE IsTemporaryMonoSupported() override {return inner_->IsTemporaryMonoSupported();}
    HRESULT STDMETHODCALLTYPE GetRestrictToOutput(IDXGIOutput** a_output) override {return inner_->GetRestrictToOutput(a_output);}
    HRESULT STDMETHODCALLTYPE SetBackgroundColor(const DXGI_RGBA* a_color) override {return inner_->SetBackgroundColor(a_color);}
    HRESULT STDMETHODCALLTYPE GetBackgroundColor(DXGI_RGBA* a_color) override {return inner_->GetBackgroundColor(a_color);}
    HRESULT STDMETHODCALLTYPE SetRotation(DXGI_MODE_ROTATION a_rotation) override {return inner_->SetRotation(a_rotation);}
    HRESULT STDMETHODCALLTYPE GetRotation(DXGI_MODE_ROTATION* a_rotation) override {return inner_->GetRotation(a_rotation);}
    HRESULT STDMETHODCALLTYPE SetSourceSize(UINT a_width, UINT a_height) override {return inner_->SetSourceSize(a_width,a_height);}
    HRESULT STDMETHODCALLTYPE GetSourceSize(UINT* a_width, UINT* a_height) override {return inner_->GetSourceSize(a_width,a_height);}
    HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(UINT a_latency) override {return inner_->SetMaximumFrameLatency(a_latency);}
    HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(UINT* a_latency) override {return inner_->GetMaximumFrameLatency(a_latency);}
    HANDLE STDMETHODCALLTYPE GetFrameLatencyWaitableObject() override {return inner_->GetFrameLatencyWaitableObject();}
    HRESULT STDMETHODCALLTYPE SetMatrixTransform(const DXGI_MATRIX_3X2_F* a_matrix) override {return inner_->SetMatrixTransform(a_matrix);}
    HRESULT STDMETHODCALLTYPE GetMatrixTransform(DXGI_MATRIX_3X2_F* a_matrix) override {return inner_->GetMatrixTransform(a_matrix);}
    UINT STDMETHODCALLTYPE GetCurrentBackBufferIndex() override {return inner_->GetCurrentBackBufferIndex();}
    HRESULT STDMETHODCALLTYPE CheckColorSpaceSupport(DXGI_COLOR_SPACE_TYPE a_colorSpace, UINT* a_support) override {return inner_->CheckColorSpaceSupport(a_colorSpace,a_support);}
    HRESULT STDMETHODCALLTYPE SetColorSpace1(DXGI_COLOR_SPACE_TYPE a_colorSpace) override {return inner_->SetColorSpace1(a_colorSpace);}
    HRESULT STDMETHODCALLTYPE ResizeBuffers1(UINT a_bufferCount, UINT a_width, UINT a_height, DXGI_FORMAT a_format, UINT a_flags, const UINT* a_creationNodeMask, IUnknown* const* a_presentQueue) override {return inner_->ResizeBuffers1(a_bufferCount,a_width,a_height,a_format,a_flags,a_creationNodeMask,a_presentQueue);}
    HRESULT STDMETHODCALLTYPE SetHDRMetaData(DXGI_HDR_METADATA_TYPE a_type, UINT a_size, void* a_metadata) override {return inner_->SetHDRMetaData(a_type,a_size,a_metadata);}
};
// This seam forwards a normal DXGI chain and exercises callback ownership only.
// It does not interpolate pixels or prove AMD async UI/presentation behavior.
