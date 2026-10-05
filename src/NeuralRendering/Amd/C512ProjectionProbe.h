#pragma once
#include "C512ProjectionReference.h"
#include "FormatProbe.h"
namespace TheosRenderPipeline::NeuralRendering::Amd {
// Bounded diagnostic execution of our declared serial dot order, not recovered WMMA order.
// Canonical tensors, at most 256 pixels; single-threaded like FormatProbe.
class C512ProjectionProbe {
public:
    static std::expected<C512ProjectionProbe,ProbeError> Create(ID3D12Device*,ID3D12CommandQueue*);
    C512ProjectionProbe(C512ProjectionProbe&&) noexcept=default;
    C512ProjectionProbe& operator=(C512ProjectionProbe&&) noexcept=default;
    C512ProjectionProbe(const C512ProjectionProbe&)=delete;
    std::expected<FormatProbeJob,ProbeError> Submit(const C512TensorLayout&,std::span<const std::uint8_t> input,
        std::span<const std::uint8_t> residual,const C512ProjectionWeights&);
    std::expected<std::vector<std::uint32_t>,ProbeError> Readback(FormatProbeJob& job,std::chrono::milliseconds timeout) {return transport_.Readback(job,timeout);}
    std::expected<void,ProbeError> Drain(std::chrono::milliseconds timeout) {return transport_.Drain(timeout);}
private:
    explicit C512ProjectionProbe(FormatProbe&& transport):transport_(std::move(transport)){}
    FormatProbe transport_;
};
}
