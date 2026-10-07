#pragma once
#include "FSRGenerationPolicy.h"
#include "RendererSettingsAction.h"

namespace TheosRenderPipeline::Upscaling
{
    inline SettingsActionStatus DescribeFsrGenerationStatus(bool owned, bool requested, bool submitted,
        bool failed, const FsrGenerationDecision& decision, unsigned callbackCount)
    {
        if (failed) return {"FSR FG failed; restart required.", SettingsStatusKind::Error};
        if (!owned) return {"FSR FG unavailable on the current presenter; selecting an FSR FG presenter requires restart.", SettingsStatusKind::Neutral};
        if (!requested) return {"FSR FG off; AMD presenter remains active.", SettingsStatusKind::Neutral};
        if (!submitted) return {"FSR FG requested; waiting for a completed temporal source.", SettingsStatusKind::Pending};
        if (decision.generate && callbackCount) return {"FSR FG active (generation callback observed).", SettingsStatusKind::Success};
        if (decision.generate) return {"FSR FG requested; waiting for a generation callback.", SettingsStatusKind::Pending};
        return {"FSR FG suppressed: " + std::string(decision.reason), SettingsStatusKind::Pending};
    }
}
