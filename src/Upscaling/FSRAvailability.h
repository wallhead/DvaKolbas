#pragma once
#include "UpscalerBackend.h"
#include <optional>
#include <format>

namespace TheosRenderPipeline::Upscaling
{
    // A catalog/file preflight is not a successful context-creation guarantee.
    // Unknown support stays distinct from known absence; startup revalidates it.
    struct FsrMlAvailability
    {
        std::optional<bool> upscale, generation;
        std::string upscaleReason{"FSR4 device support has not been checked; startup will validate it."};
        std::string generationReason{"FSR4 FG device support has not been checked; startup will validate it."};
    };
    inline constexpr const char* kFsrSrRecovery =
        "Close Skyrim, edit SKSE/Plugins/RaZkolbaS.ini: [FSR] ProviderPolicy=Analytical, then restart.";
    inline constexpr const char* kFsrFgRecovery =
        "Close Skyrim, edit SKSE/Plugins/RaZkolbaS.ini: [FrameGeneration] FsrProviderPolicy=Analytical, then restart.";

    inline std::string FsrStartupRecoveryMessage(const RuntimeError& error, ProviderPolicy sr, ProviderPolicy fg)
    {
        auto message=error.message;
        if(sr==ProviderPolicy::MachineLearning && message.find(kFsrSrRecovery)==std::string::npos) {
            message+='\n';message+=kFsrSrRecovery;
        }
        if(fg==ProviderPolicy::MachineLearning && message.find(kFsrFgRecovery)==std::string::npos) {
            message+='\n';message+=kFsrFgRecovery;
        }
        return message;
    }
    inline std::string FsrShaderModelFailure(std::uint32_t minimum, std::uint32_t reported, std::int32_t hr)
    {
        const auto version=[](std::uint32_t value){return std::format("{}.{}",value>>4,value&15);};
        return hr<0 ? std::format("FSR requires shader model {}; shader-model query failed HRESULT=0x{:08X} (requested {}, support unknown)",
            version(minimum),static_cast<std::uint32_t>(hr),version(reported)) :
            std::format("FSR requires shader model {}; device reports {}",version(minimum),version(reported));
    }
}
