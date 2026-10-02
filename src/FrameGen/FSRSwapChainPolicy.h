#pragma once
#include <dxgi1_4.h>

namespace TheosRenderPipeline
{
    // The AMD transport publishes complete frames. Reject partial updates before
    // invoking the source producer, so invalid Present1 calls consume no ID.
    template<class Prepare, class Submit, class Completed>
    HRESULT PresentFsrSourceBoundary(UINT flags, const DXGI_PRESENT_PARAMETERS* parameters,
        Prepare&& prepare, Submit&& submit, Completed&& completed)
    {
        if (parameters && (parameters->DirtyRectsCount || parameters->pScrollRect || parameters->pScrollOffset)) {
            return E_INVALIDARG;
        }
        if (flags & DXGI_PRESENT_TEST) { return submit(); }
        const auto prepared = prepare();
        if (FAILED(prepared)) { return prepared; }
        const auto result = submit();
        completed(result);
        return result;
    }

    // A replacement owner creates both buffers on its retained D3D12 queue.
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

    template<class Retire, class Detach, class Recreate>
    HRESULT ResizeFsrSourceBoundary(UINT count, const UINT* nodeMasks, IUnknown* const* queues,
        Retire&& retire, Detach&& detach, Recreate&& recreate)
    {
        const auto valid = ValidateFsrResize(count, nodeMasks, queues);
        if (FAILED(valid)) { return valid; }
        const auto retired = retire();
        if (FAILED(retired)) { return retired; }
        detach();
        return recreate();
    }
}
