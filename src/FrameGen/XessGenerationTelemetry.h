#pragma once
#include "FrameTelemetry.h"
#include <atomic>
namespace TheosRenderPipeline {
    inline std::atomic<std::uint64_t> xessOutputEpoch{};
    class XessGenerationTelemetry {
    public:
        XessGenerationTelemetry(){Invalidate();}
        void Observe(long result,unsigned frames,int sdkResult,bool test=false) {
            if(test)return;
            if(result!=0 || sdkResult<0 || frames<1 || frames>2){Invalidate();return;}
            ++counter_.observations;counter_.frames+=frames;counter_.available=true;
        }
        void Invalidate() {counter_={Telemetry::OutputSource::Xess,xessOutputEpoch.fetch_add(1)+1,0,0,false};}
        Telemetry::OutputCounter Counter()const {return counter_;}
    private:
        Telemetry::OutputCounter counter_;
    };
}
