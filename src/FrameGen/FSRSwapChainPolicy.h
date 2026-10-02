#pragma once
#include <dxgi1_4.h>

namespace TheosRenderPipeline
{
    // The selected AMD milestone supports SDR and windowed presentation only.
    // Gate before forwarding: the SDK changes transfer state even if DXGI fails.
    template<class Set> HRESULT SetFsrCompatibleColorSpace(bool fsr,DXGI_COLOR_SPACE_TYPE color,Set&& set)
    {
        if(fsr && color!=DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709)return E_INVALIDARG;
        return set();
    }
    template<class Set> HRESULT SetFsrCompatibleHdrMetadata(bool fsr,DXGI_HDR_METADATA_TYPE type,UINT size,Set&& set)
    {
        if(fsr && (type!=DXGI_HDR_METADATA_TYPE_NONE || size))return E_INVALIDARG;
        return set();
    }
    template<class Set> HRESULT SetFsrCompatibleFullscreen(bool fsr,BOOL fullscreen,Set&& set)
    {
        if(fsr && fullscreen)return E_INVALIDARG;
        return set();
    }
    // The AMD transport publishes complete frames. Reject partial updates before
    // invoking the source producer, so invalid Present1 calls consume no ID.
    template<class Prepare, class Submit, class Completed>
    HRESULT PresentFsrSourceBoundary(UINT flags, const DXGI_PRESENT_PARAMETERS* parameters,
        Prepare&& prepare, Submit&& submit, Completed&& completed,bool suspended=false)
    {
        if (parameters && (parameters->DirtyRectsCount || parameters->pScrollRect || parameters->pScrollOffset)) {
            return E_INVALIDARG;
        }
        if(suspended)return DXGI_STATUS_OCCLUDED;
        if (flags & DXGI_PRESENT_TEST) { return submit(); }
        const auto prepared = prepare();
        if (FAILED(prepared)) { return prepared; }
        const auto result = submit();
        completed(result);
        return result;
    }

    // The retained AMD owner has both buffers on its native D3D12 queue.
    // Caller queues (potentially D3D11 objects) never reach the AMD swapchain.
    // ResizeBuffers1 arrays have BufferCount entries. Count 0 with a non-null
    // mask has no supplied array length, so reject without inspecting memory.
    inline HRESULT ValidateFsrResize(UINT count, const UINT* nodeMasks, IUnknown* const* queues)
    {
        if (queues || count > 16 || (nodeMasks && !count)) { return E_INVALIDARG; }
        if (nodeMasks) {
            for (UINT i = 0; i < count; ++i) {
                if (nodeMasks[i]) { return E_INVALIDARG; }
            }
        }
        return S_OK;
    }
    inline HRESULT ValidateFsrResizeFlags(UINT previous,UINT requested)
    {
        constexpr UINT immutable=DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT|DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        return ((previous^requested)&immutable)?E_INVALIDARG:S_OK;
    }

}
