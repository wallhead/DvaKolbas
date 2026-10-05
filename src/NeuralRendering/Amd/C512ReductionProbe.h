#pragma once
#include "C512Tensor.h"
#include "FormatProbe.h"
namespace TheosRenderPipeline::NeuralRendering::Amd {
inline constexpr std::size_t C512ReductionMaxSourceValues=4096u*512u;
inline constexpr std::size_t C512ReductionMaxDestinationBytes=1024u*512u;
// Serialized diagnostic: canonical half input, packed FP8 result. Every inactive
// byte is preserved from initialPackedOutput, which is snapshotted at Submit.
class C512ReductionProbe {
public:
    static std::expected<C512ReductionProbe,ProbeError> Create(ID3D12Device*,ID3D12CommandQueue*);
    C512ReductionProbe(C512ReductionProbe&&) noexcept=default;
    C512ReductionProbe& operator=(C512ReductionProbe&&) noexcept=default;
    C512ReductionProbe(const C512ReductionProbe&)=delete;
    std::expected<FormatProbeJob,ProbeError> Submit(const C512TensorLayout& source,const C512TensorLayout& destination,
        std::span<const std::uint16_t> canonicalInput,std::span<const std::uint8_t> initialPackedOutput);
    std::expected<std::vector<std::uint8_t>,ProbeError> Readback(FormatProbeJob&,std::chrono::milliseconds);
    std::expected<void,ProbeError> Drain(std::chrono::milliseconds timeout) {return transport_.Drain(timeout);}
private:
    explicit C512ReductionProbe(FormatProbe&& transport):transport_(std::move(transport)){}
    FormatProbe transport_;
};
}
