#pragma once
class PerformanceTuning
{
public:
    static PerformanceTuning* GetSingleton() { static PerformanceTuning instance;return &instance; }
    bool TimingEnabled() const { return false; }
    void RecordSourcePresentCpuMs(float) {}
};
