#pragma once
#include "UpscalerBackend.h"

namespace TheosRenderPipeline::Upscaling
{
    // The completed output allocation remains owned by the host. Any genuine
    // transition must evaluate a fresh source or enter spatial recovery instead.
    inline bool CanRepeatXessOutput(const UpscaleFrame& frame, const UpscaleFrame& completed,
        bool hasCompleted, bool duplicate, bool menu, bool recovery)
    {
        return duplicate && hasCompleted && !menu && !recovery && !frame.reset &&
            frame.sourceId==completed.sourceId && frame.sourceEpoch==completed.sourceEpoch &&
            frame.render==completed.render && frame.display==completed.display && frame.output==completed.output;
    }
}
