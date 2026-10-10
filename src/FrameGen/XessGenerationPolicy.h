#pragma once
#include "XessGenerationFrameAdapter.h"
namespace TheosRenderPipeline
{
    struct XessGenerationAdmission { bool presentReal{}, tag{}, generate{}, reset{}; };
    class XessGenerationHistory
    {
    public:
        XessGenerationAdmission Decide(const Upscaling::UpscaleFrame&, Upscaling::UpscaleOutcome, bool requested, bool hudComplete, bool menu);
        void Accept(std::uint64_t sourceId, std::uint64_t epoch);
        void Invalidate();
    private:
        std::uint64_t lastSource_{}, lastEpoch_{}, lastCamera_{}, pendingSource_{}, pendingEpoch_{}, pendingCamera_{};
        Upscaling::Extent lastRender_{},lastDisplay_{},pendingRender_{},pendingDisplay_{};
        bool resetArmed_{true};
        std::uint32_t lastFlags_{}, pendingFlags_{};
    };
}
