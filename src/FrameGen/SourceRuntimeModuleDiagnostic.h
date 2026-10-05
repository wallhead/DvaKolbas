#pragma once
#include <format>
#include <string>
#include <string_view>

namespace TheosRenderPipeline::SourceDLSSG
{
inline std::string RuntimeModuleFailureMessage(std::string_view name, bool frameGenerationOverrideObserved)
{
    auto message = std::format("{} was not loaded from the configured Streamline directory", name);
    if (name == "nvngx_dlssg.dll" && frameGenerationOverrideObserved) {
        message += ". NVIDIA reported a DLSS Frame Generation override despite application-setting filtering. "
            "Restart Skyrim and include RaZkolbaS.log when reporting this failure; it records the selected runtime paths";
    }
    return message;
}
}
