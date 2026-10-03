#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
namespace TheosRenderPipeline::NeuralRendering {
enum class CpuPhase : size_t { Total, Bridge, PrepareColor, PrepareGuides, InputCopy, ProducerSignal,
    ProducerWait, Begin, Record, Submit, Delivery, ConsumerWait, CompletionWait, Drain,
    Encode, PreparedWait, InteropWait, FsrTotal, FsrPrepareColor, FsrPrepareGuides,
    FsrProducerSignal, FsrBegin, FsrRecord, FsrSubmit, FsrDelivery, Count };
enum class GpuPhase : size_t { PrepareColor, PrepareGuides, InputCopy, Vendor, Alpha,
    Delivery, Encode, FsrPrepare, FsrDispatch, FsrDelivery, Count };
std::string_view PhaseName(CpuPhase) noexcept;
std::string_view PhaseName(GpuPhase) noexcept;
uint64_t PerformanceNow() noexcept;
struct PerformanceWorkload {
    uint32_t width{},height{};
    std::string placement,runtimeHash;
    uint32_t passes{1};
    float localTone{};
};
bool EquivalentWorkload(const PerformanceWorkload&,const PerformanceWorkload&) noexcept;
struct CpuInterval {uint64_t begin{},end{};};
struct PerformanceFrame {
    uint64_t sourceId{},waitCalls{},blockingCalls{},blockNanoseconds{},flushes{},descriptorCreations{};
    uint64_t resourceCreations{},submitted{},completed{},slotPressure{},cpuUnionNanoseconds{};
    bool nrEnabled{};
    std::array<uint64_t,size_t(CpuPhase::Count)> cpuNanoseconds{},waitNanoseconds{};
    std::array<std::optional<double>,size_t(GpuPhase::Count)> gpuMilliseconds{};
    std::array<CpuInterval,64> intervals{};
    size_t intervalCount{};
};
struct PerformanceSnapshot {
    std::vector<PerformanceFrame> frames;
    uint64_t droppedFrames{},droppedIntervals{};
};
// The serialized render/probe owner controls this optional, bounded collector.
// It never logs, waits, signals a fence or changes rendering admission.
class PerformanceMetrics {
public:
    explicit PerformanceMetrics(size_t capacity=4096);
    void Enable(bool);
    bool Enabled()const noexcept{return enabled_;}
    void BeginFrame(uint64_t sourceId,bool nrEnabled);
    void EndFrame()noexcept{current_=nullptr;}
    void RecordCpu(CpuPhase,uint64_t begin,uint64_t end);
    void RecordWait(CpuPhase,bool blocked,uint64_t blockNanoseconds);
    void RecordGpu(GpuPhase,uint64_t begin,uint64_t end,uint64_t frequency,bool retired,bool disjoint);
    void RecordGpuFor(uint64_t sourceId,GpuPhase,uint64_t begin,uint64_t end,uint64_t frequency,bool retired,bool disjoint);
    void RecordFlush();
    void RecordDescriptorCreation();
    void RecordResourceCreation();
    void RecordSubmitted();
    void RecordCompleted();
    void RecordCompletedFor(uint64_t sourceId);
    void RecordSlotPressure();
    PerformanceSnapshot Snapshot()const;
private:
    size_t capacity_;
    bool enabled_{};
    PerformanceFrame* current_{};
    PerformanceSnapshot data_;
};
class PerformanceScope {
public:
    PerformanceScope(PerformanceMetrics* metrics,CpuPhase phase)noexcept;
    ~PerformanceScope();
    PerformanceScope(const PerformanceScope&)=delete;
private:
    PerformanceMetrics* metrics_{};CpuPhase phase_{};uint64_t begin_{};
};
template<class Operation>decltype(auto) MeasurePerformance(PerformanceMetrics* metrics,CpuPhase phase,Operation&& operation){
    PerformanceScope scope(metrics,phase);return std::forward<Operation>(operation)();
}
}
