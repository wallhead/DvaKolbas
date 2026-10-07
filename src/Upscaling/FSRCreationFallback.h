#pragma once
#include "FSRProviderPolicy.h"
#include <utility>

namespace TheosRenderPipeline::Upscaling
{
    // Startup only: no dispatch may have been recorded. Cleanup must succeed
    // before another creation, and the published game buffers cannot change.
    template<class Create, class Retire, class Analytical>
    Result<ProviderInfo> CreateFsrWithStartupFallback(const ProviderInfo& selected,
        ProviderPolicy policy, Extent publishedRender, Create&& create,
        Retire&& retire, Analytical&& analytical)
    {
        auto created=create(selected);
        if(created)return selected;
        const auto kind=created.error().kind;
        if(policy!=ProviderPolicy::Compatible || !IsFsr4Provider(selected) ||
            (kind!=ErrorKind::ContextFailure && kind!=ErrorKind::UnsupportedDevice &&
             kind!=ErrorKind::IncompatibleAbi && kind!=ErrorKind::NoProvider))
            return std::unexpected(created.error());
        auto retired=retire();
        if(!retired)return std::unexpected(retired.error());
        auto fallback=analytical();
        if(!fallback)return std::unexpected(fallback.error());
        if(fallback->second!=publishedRender)
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,
                "FSR4 Auto fallback needs different render dimensions; select FSR3 and restart"});
        auto retried=create(fallback->first);
        if(!retried)return std::unexpected(retried.error());
        return fallback->first;
    }
}
