#pragma once
#include "UpscalerBackend.h"
#include <span>
#include <cctype>
namespace TheosRenderPipeline::Upscaling
{
    inline Result<ProviderInfo> SelectProvider(std::span<const ProviderInfo> providers, ProviderPolicy policy)
    {
        for (const auto& provider : providers) {
            if (provider.name.empty()) continue;
            if (policy == ProviderPolicy::Compatible) return provider;
            const auto version = provider.name.find("3.1.5");
            if (policy == ProviderPolicy::Analytical && version != std::string::npos &&
                (version == 0 || !std::isdigit(static_cast<unsigned char>(provider.name[version-1]))) &&
                (version+5 == provider.name.size() || (!std::isdigit(static_cast<unsigned char>(provider.name[version+5])) && provider.name[version+5] != '.')))
                return provider;
        }
        return std::unexpected(RuntimeError{ErrorKind::NoProvider, 0, "Requested FSR provider is unavailable; no version ID was fabricated"});
    }
}
