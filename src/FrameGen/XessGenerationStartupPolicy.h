#pragma once
#include "Upscaling/UpscalerBackend.h"
namespace TheosRenderPipeline
{
    // Observations from the actual loader and producer/native adapter probe.
    // Device-name or device-ID routing tests cannot supply physical qualification.
    struct XessGenerationStartupCapabilities
    {
        bool runtimePairPresent{}, nativeDevice{}, sameAdapter{}, shaderModel64{}, intelAdapter{}, engineHooks{};
    };
    inline const char* ValidateXessGenerationStartup(Upscaling::PresentationKind selected,
        XessGenerationStartupCapabilities actual)
    {
        if(selected!=Upscaling::PresentationKind::Xess)return nullptr;
        if(!actual.runtimePairPresent)return "XeSS FG requires libxess_fg.dll and libxell.dll beneath SKSE/Plugins/RaZkolbaS/XeSS. Install the matching optional payload and restart.";
        if(!actual.nativeDevice)return "XeSS FG requires a native D3D12 device and direct queue on the rendering adapter.";
        if(!actual.sameAdapter)return "XeSS FG native device LUID differs from the game's rendering adapter; cross-adapter presentation is unavailable.";
        if(!actual.intelAdapter && !actual.shaderModel64)return "XeSS FG requires Shader Model 6.4 on non-Intel rendering adapters.";
        if(!actual.engineHooks)return "XeSS FG inactive: this Skyrim runtime's main source-loop/input/render hook sites are unqualified or have changed.";
        return nullptr;
    }
}
