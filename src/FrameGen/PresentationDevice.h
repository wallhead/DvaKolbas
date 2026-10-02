#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

namespace TheosRenderPipeline
{
    inline HRESULT AcquirePresentationDevice(IDXGISwapChain* chain, ID3D11Device* creationDevice,
        bool ordinary, Microsoft::WRL::ComPtr<ID3D11Device>& device)
    {
        device.Reset();
        if(!chain || !creationDevice)return E_INVALIDARG;
        // Ordinary presentation was created with this exact device. ReShade's
        // isolated native chain can report its underlying device while texture
        // GetDevice still reports the creation proxy. Keep the producer-facing
        // device/context pair; do not reinterpret unrelated device identities.
        if(ordinary) { device=creationDevice;return S_OK; }
        return chain->GetDevice(IID_PPV_ARGS(&device));
    }
}
