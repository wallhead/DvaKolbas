#include "C512Reduction.h"
#include "NumericFormats.h"
#include <algorithm>
#include <limits>

namespace TheosRenderPipeline::NeuralRendering::Amd {
std::expected<void,TensorError> ReduceC512Half2x2(
    const C512TensorLayout& source,const C512TensorLayout& destination,
    std::span<const std::uint16_t> input,std::span<std::uint8_t> output) noexcept {
    if(source.ByteCount()>std::numeric_limits<std::size_t>::max()/2) return std::unexpected(TensorError::Overflow);
    if(input.size()!=source.ByteCount() || output.size()!=destination.ByteCount())
        return std::unexpected(TensorError::CountMismatch);
    const auto a=reinterpret_cast<std::uintptr_t>(input.data()),b=reinterpret_cast<std::uintptr_t>(output.data());
    if(a>=b ? a-b<output.size() : b-a<input.size()*2) return std::unexpected(TensorError::Overlap);
    const auto src=source.Dimensions(),dst=destination.Dimensions();
    const auto width=std::min(src.width/2,dst.width),height=std::min(src.height/2,dst.height);
    for(std::uint32_t x=0;x<width;++x) for(std::uint32_t y=0;y<height;++y) for(std::uint32_t n=0;n<512;++n) {
        const auto first=(std::size_t(x)*2*src.height+y*2)*512+n;
        const auto second=first+std::size_t(src.height)*512;
        const auto left=FloatToHalfRne(HalfToFloat(input[first])+HalfToFloat(input[first+512]));
        const auto right=FloatToHalfRne(HalfToFloat(input[second])+HalfToFloat(input[second+512]));
        const auto sum=FloatToHalfRne(HalfToFloat(left)+HalfToFloat(right));
        const auto average=FloatToHalfRne(HalfToFloat(sum)*0.25f);
        output[*destination.Offset(C512TensorOrder::Packed,x,y,n)]=EncodeFloatToE4m3Rne(HalfToFloat(average));
    }
    return {};
}
}
