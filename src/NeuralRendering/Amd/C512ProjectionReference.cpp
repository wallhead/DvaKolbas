#include "C512ProjectionReference.h"
#include "NumericFormats.h"
#include <array>
#include <cmath>

namespace TheosRenderPipeline::NeuralRendering::Amd {
namespace {
bool Overlaps(const void* p,std::size_t bytes,const void* q,std::size_t otherBytes) noexcept {
    if(!bytes || !otherBytes) return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),b=reinterpret_cast<std::uintptr_t>(q);
    return a>=b?a-b<otherBytes:b-a<bytes;
}
std::uint16_t RoundHalf(float f) noexcept {
    return std::isnan(f)?std::uint16_t(0x7e00):FloatToHalfRne(f);
}
}
std::expected<void,ProjectionEvaluationError> ValidateC512ProjectionInputs(
    const C512TensorLayout& layout,std::span<const std::uint8_t> input,
    std::span<const std::uint8_t> residual,const C512ProjectionWeights& weights) noexcept {
    if(layout.ByteCount()>C512ProjectionMaxValues) return std::unexpected(ProjectionEvaluationError::Extent);
    if(input.size()!=layout.ByteCount() || (!residual.empty() && residual.size()!=input.size()))
        return std::unexpected(ProjectionEvaluationError::Count);
    if(weights.MatrixCodes().size()!=512u*512u || weights.ResidualHalfBits().size()!=512)
        return std::unexpected(ProjectionEvaluationError::Weights);
    return {};
}
std::expected<void,ProjectionEvaluationError> EvaluateC512ProjectionReference(
    const C512TensorLayout& layout,std::span<const std::uint8_t> input,
    std::span<const std::uint8_t> residual,const C512ProjectionWeights& weights,
    std::span<std::uint32_t> output) noexcept {
    const auto valid=ValidateC512ProjectionInputs(layout,input,residual,weights);
    if(!valid) return valid;
    if(output.size()!=layout.ByteCount()*C512ProjectionWordsPerValue) return std::unexpected(ProjectionEvaluationError::Count);
    const auto matrix=weights.MatrixCodes();const auto coefficients=weights.ResidualHalfBits();
    if(Overlaps(input.data(),input.size(),output.data(),output.size_bytes()) ||
       Overlaps(residual.data(),residual.size(),output.data(),output.size_bytes()) ||
       Overlaps(matrix.data(),matrix.size(),output.data(),output.size_bytes()) ||
       Overlaps(coefficients.data(),coefficients.size_bytes(),output.data(),output.size_bytes()))
        return std::unexpected(ProjectionEvaluationError::Overlap);
    std::array<float,256> decode{};
    for(unsigned i=0;i<256;++i) decode[i]=HalfToFloat(DecodeE4m3ToHalf(std::uint8_t(i)));
    for(std::size_t pixel=0;pixel<input.size()/512;++pixel) for(unsigned n=0;n<512;++n) {
        const auto index=pixel*512+n;
        const auto initial=RoundHalf(std::fma(decode[residual.empty()?0:residual[index]],HalfToFloat(coefficients[n]),0.0f));
        auto accumulator=initial;
        for(unsigned chunk=0;chunk<16;++chunk) {
            float sum=0.0f;
            for(unsigned k=chunk*32;k<(chunk+1)*32;++k)
                sum=sum+decode[input[pixel*512+k]]*decode[matrix[n*512+k]];
            accumulator=RoundHalf(sum+HalfToFloat(accumulator));
        }
        output[index*2]=accumulator|(std::uint32_t(EncodeFloatToE4m3Rne(HalfToFloat(accumulator)))<<16);
        output[index*2+1]=initial;
    }
    return {};
}
}
