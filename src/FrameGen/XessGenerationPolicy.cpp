#include "XessGenerationPolicy.h"
namespace TheosRenderPipeline
{
    XessGenerationAdmission XessGenerationHistory::Decide(const Upscaling::UpscaleFrame& frame, Upscaling::UpscaleOutcome outcome,
        bool requested,bool hudComplete,bool menu)
    {
        using Upscaling::UpscaleOutcome;
        pendingSource_=pendingEpoch_=pendingCamera_=0;
        const bool real=frame.output && frame.display.width && frame.display.height &&
            (outcome==UpscaleOutcome::Temporal || outcome==UpscaleOutcome::SpatialRecovery || outcome==UpscaleOutcome::RepeatedOutput);
        const auto suppress=[&] { Invalidate();return XessGenerationAdmission{real,false,false,true}; };
        if (!real || !requested || !hudComplete || menu) return suppress();
        if (outcome==UpscaleOutcome::RepeatedOutput) {
            const auto adapted=AdaptXessGenerationFrame(frame,0);
            if (!adapted || frame.sourceId!=lastSource_ || frame.sourceEpoch!=lastEpoch_ ||
                frame.camera.identity!=lastCamera_ || frame.render!=lastRender_ || frame.display!=lastDisplay_ ||
                adapted->initFlags!=lastFlags_ || frame.reset || frame.camera.reset) return suppress();
            return {true,false,false,resetArmed_};
        }
        if (outcome!=UpscaleOutcome::Temporal) return suppress();
        const auto adapted=AdaptXessGenerationFrame(frame,0);
        if (!adapted) return suppress();
        if (frame.sourceEpoch==lastEpoch_ && frame.sourceId<=lastSource_) return suppress();
        const bool changed=lastSource_ && (frame.sourceEpoch!=lastEpoch_ || frame.camera.identity!=lastCamera_ ||
            frame.render!=lastRender_ || frame.display!=lastDisplay_ || adapted->initFlags!=lastFlags_ || frame.sourceId-lastSource_!=1);
        const bool reset=resetArmed_ || changed || frame.reset || frame.camera.reset;
        pendingSource_=frame.sourceId;pendingEpoch_=frame.sourceEpoch;pendingCamera_=frame.camera.identity;
        pendingRender_=frame.render;pendingDisplay_=frame.display;
        pendingFlags_=adapted->initFlags;
        return {true,true,!reset,reset};
    }
    void XessGenerationHistory::Accept(std::uint64_t sourceId,std::uint64_t epoch)
    {
        if (!pendingSource_ || sourceId!=pendingSource_ || epoch!=pendingEpoch_) return;
        lastSource_=sourceId;lastEpoch_=epoch;lastCamera_=pendingCamera_;
        lastRender_=pendingRender_;lastDisplay_=pendingDisplay_;resetArmed_=false;
        lastFlags_=pendingFlags_;
        pendingSource_=pendingEpoch_=pendingCamera_=0;
    }
    void XessGenerationHistory::Invalidate()
    { resetArmed_=true;pendingSource_=pendingEpoch_=pendingCamera_=0; }
}
