#include "C512ProjectionProbe.h"
#include "C512ProjectionBytecode.h"
#include <new>
namespace TheosRenderPipeline::NeuralRendering::Amd {
std::expected<C512ProjectionProbe,ProbeError> C512ProjectionProbe::Create(ID3D12Device* device,ID3D12CommandQueue* queue) {
    auto transport=FormatProbe::CreateCompute(device,queue,AmdNrShader::C512Projection);
    if(!transport) return std::unexpected(transport.error());
    return C512ProjectionProbe(std::move(*transport));
}
std::expected<FormatProbeJob,ProbeError> C512ProjectionProbe::Submit(const C512TensorLayout& layout,
    std::span<const std::uint8_t> input,std::span<const std::uint8_t> residual,const C512ProjectionWeights& weights) {
    if(!transport_.state_) return std::unexpected(ProbeError::InvalidDevice);
    if(!ValidateC512ProjectionInputs(layout,input,residual,weights)) return std::unexpected(ProbeError::Count);
    try {
        // Byte payload: canonical input, residual (positive zeros if absent), matrix [n][k], LE half coefficients.
        const auto count=input.size();std::vector<std::uint32_t> payload((2*count+263168)/4);
        auto put=[&](std::size_t at,std::uint8_t value){payload[at/4]|=std::uint32_t(value)<<(8*(at%4));};
        for(std::size_t i=0;i<count;++i) {put(i,input[i]);if(!residual.empty()) put(count+i,residual[i]);}
        const auto matrix=weights.MatrixCodes();const auto coefficients=weights.ResidualHalfBits();
        for(std::size_t i=0;i<matrix.size();++i) put(2*count+i,matrix[i]);
        for(std::size_t i=0;i<coefficients.size();++i) {
            put(2*count+262144+2*i,std::uint8_t(coefficients[i]));
            put(2*count+262144+2*i+1,std::uint8_t(coefficients[i]>>8));
        }
        return transport_.SubmitCompute(payload,2*count,count,{std::uint32_t(count),0});
    } catch(const std::bad_alloc&) {return std::unexpected(ProbeError::Memory);}
}
}
