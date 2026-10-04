#pragma once
#include "NvidiaHost.h"
extern NvidiaHost* preparationHost;
inline void BeforeGameSwapChainPresent(GameSwapChain*)
{
    if(preparationHost) preparationHost->Prepare();
}
namespace TheosRenderPipeline
{
    template<class Resize> HRESULT ResizeHostBuffers(NvidiaHost&, IDXGISwapChain*, Resize&&)
    { return E_NOTIMPL; } // Resizing is outside this dispatch fixture.
}
