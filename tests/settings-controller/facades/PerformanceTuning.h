#pragma once
class PerformanceTuning {
public:
    struct Settings{bool enableGPUTimings{},enableFrameTrace{},directRCASOutput{},directDLSSOutput{};}settings;
    unsigned applications{};
    static PerformanceTuning* GetSingleton(){static PerformanceTuning v;return &v;}
    void ApplySettings(Settings v){settings=v;++applications;}
};
