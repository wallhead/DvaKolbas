#pragma once

#include <algorithm>
#include <cstdint>

namespace TheosRenderPipeline::Diagnostics
{
    struct Settings
    {
        bool frameDetails{false};
        bool performanceMetrics{false};
        long performanceIntervalSeconds{10};
    };

    template<class Ini> Settings Read(const Ini& ini)
    {
        Settings settings;
        settings.frameDetails=ini.GetBoolValue("Debug","LogFrameDiagnostics",settings.frameDetails);
        settings.performanceMetrics=ini.GetBoolValue("Debug","LogPerformanceMetrics",settings.performanceMetrics);
        settings.performanceIntervalSeconds=std::clamp(ini.GetLongValue("Debug","PerformanceLogIntervalSeconds",10),1L,120L);
        return settings;
    }

    template<class Ini> void Store(Ini& ini,const Settings& settings)
    {
        ini.SetBoolValue("Debug","LogFrameDiagnostics",settings.frameDetails);
        ini.SetBoolValue("Debug","LogPerformanceMetrics",settings.performanceMetrics);
        ini.SetLongValue("Debug","PerformanceLogIntervalSeconds",std::clamp(settings.performanceIntervalSeconds,1L,120L));
    }

    // Monotonic milliseconds; no frame-rate-dependent logging frequency.
    class PeriodicLogGate
    {
    public:
        bool Accept(bool enabled,std::uint64_t now,std::uint64_t interval)
        {
            if(!enabled) { Reset();return false; }
            if(ready_ && now>=last_ && now-last_<interval)return false;
            ready_=true;last_=now;return true;
        }
        void Reset(){ready_=false;last_=0;}
    private:
        bool ready_{};
        std::uint64_t last_{};
    };
}
