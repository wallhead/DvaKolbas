#pragma once
#include "NumericFormats.h"
#include <d3d12.h>
#include <chrono>
#include <memory>
#include <vector>
namespace TheosRenderPipeline::NeuralRendering::Amd {
enum class ProbeError { InvalidDevice, InvalidQueue, InvalidOperation, Count, Busy, Api, Memory, Timeout, DeviceRemoved, InvalidJob };
struct ProbeJobData;
struct ProbeState;
class FormatProbeJob {
public:
    FormatProbeJob(FormatProbeJob&&) noexcept=default;
    FormatProbeJob& operator=(FormatProbeJob&&) noexcept=default;
    FormatProbeJob(const FormatProbeJob&)=delete;
private:
    explicit FormatProbeJob(std::shared_ptr<ProbeJobData> data):data_(std::move(data)){}
    std::shared_ptr<ProbeJobData> data_;
    friend class FormatProbe;
};
// Single-threaded diagnostic context; caller must serialize all operations.
class FormatProbe {
public:
    static std::expected<FormatProbe,ProbeError> Create(ID3D12Device*,ID3D12CommandQueue*);
    FormatProbe(FormatProbe&&) noexcept;
    FormatProbe& operator=(FormatProbe&&) noexcept;
    FormatProbe(const FormatProbe&)=delete;
    ~FormatProbe();
    std::expected<FormatProbeJob,ProbeError> Submit(FormatOperation,std::span<const std::uint32_t>);
    std::expected<std::vector<std::uint32_t>,ProbeError> Readback(FormatProbeJob&,std::chrono::milliseconds);
    std::expected<void,ProbeError> Drain(std::chrono::milliseconds);
private:
    explicit FormatProbe(std::unique_ptr<ProbeState>);
    void Shutdown() noexcept;
    std::unique_ptr<ProbeState> state_;
    friend struct FormatProbeTestAccess;
};
}
