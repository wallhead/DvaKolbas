#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "D3D11FrameCopy.h"

namespace TheosRenderPipeline
{
    inline bool UseDedicatedPresentUi(bool fsrPresenter, bool gameUiDrawn)
    { return fsrPresenter || gameUiDrawn; }

    // Call before ImGui/ReShade draw the late foreground, never at the final
    // capture: an empty game HUD must not erase their completed pixels.
    inline bool PrepareFsrPresentUi(ID3D11DeviceContext* context, ID3D11RenderTargetView* target, bool gameUiDrawn)
    {
        if (!context || !target || context->GetType() != D3D11_DEVICE_CONTEXT_IMMEDIATE) return false;
        Microsoft::WRL::ComPtr<ID3D11Device> producer, owner;
        context->GetDevice(&producer);target->GetDevice(&owner);
        if (!D3D11FrameCopy::SameObject(producer.Get(), owner.Get())) return false;
        if (!gameUiDrawn) { const float empty[4]{};context->ClearRenderTargetView(target, empty); }
        return true;
    }
}
