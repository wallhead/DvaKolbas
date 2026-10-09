#pragma once
#include "UpscalerBackend.h"
#include <string_view>

namespace TheosRenderPipeline::Upscaling
{
    struct FsrGenerationDecision
    {
        bool prepare{}, generate{}, reset{true}, admit{};
        std::string_view reason;
    };

    // Tracks consumed real sources, not generated presents. A returned generate
    // flag requests generation; only actual SDK observations establish output.
    class FsrGenerationHistory
    {
    public:
        FsrGenerationDecision Decide(const UpscaleFrame&, UpscaleOutcome, bool uiComplete, bool menu, bool requested);
        void AcknowledgePrepared(uint64_t sourceId);
        void Invalidate();
    private:
        void ArmReset();
        Extent lastRender_{}, lastDisplay_{};
        uint64_t lastSource_{}, lastEpoch_{}, lastCamera_{}, pendingPrepare_{};
        bool resetArmed_{true};
    };
}
