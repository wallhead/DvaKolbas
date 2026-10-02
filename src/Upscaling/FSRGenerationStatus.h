#pragma once
#include "FSRGenerationPolicy.h"
#include "RendererSettingsAction.h"

namespace TheosRenderPipeline::Upscaling
{
    inline SettingsActionStatus DescribeFsrGenerationStatus(bool owned, bool requested, bool submitted,
        bool failed, const FsrGenerationDecision& decision, unsigned callbackCount)
    {
        if (failed) return {"FSR FG failed; restart required.", SettingsStatusKind::Error};
        if (!owned) return {"FSR FG unavailable on the current presenter; backend 2 requires restart.", SettingsStatusKind::Neutral};
        if (!requested) return {"FSR FG off; AMD presenter remains active.", SettingsStatusKind::Neutral};
        if (!submitted) return {"FSR FG requested; waiting for a completed temporal source.", SettingsStatusKind::Pending};
        if (decision.generate && callbackCount) return {"FSR FG active (generation callback observed).", SettingsStatusKind::Success};
        if (decision.generate) return {"FSR FG requested; waiting for a generation callback.", SettingsStatusKind::Pending};
        if (decision.reason == "Source rate warmup") return {"FSR FG warming up the source rate history.", SettingsStatusKind::Pending};
        if (decision.reason == "Source rate suppressed") return {"FSR FG source rate suppressed; waits for sustained recovery.", SettingsStatusKind::Pending};
        return {"FSR FG suppressed: " + std::string(decision.reason), SettingsStatusKind::Pending};
    }
}
