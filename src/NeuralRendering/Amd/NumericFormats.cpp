#include "NumericFormats.h"
#include <bit>

namespace TheosRenderPipeline::NeuralRendering::Amd {
std::uint16_t FloatToHalfRne(float value) noexcept {
    const auto bits=std::bit_cast<std::uint32_t>(value);
    const auto sign=(bits>>16)&0x8000;
    const auto exponent=(bits>>23)&255, mantissa=bits&0x7fffff;
    if(exponent==255) return std::uint16_t(sign|0x7c00|(mantissa?((mantissa>>13)|0x200):0));
    if(exponent>=143) return std::uint16_t(sign|0x7c00);
    if(exponent<102) return std::uint16_t(sign);
    const unsigned shift=exponent<113?126-exponent:13;
    const auto significand=exponent<113?(mantissa|0x800000):mantissa;
    auto rounded=significand>>shift;
    const auto remainder=significand&((1u<<shift)-1), midpoint=1u<<(shift-1);
    rounded+=(remainder>midpoint || (remainder==midpoint && (rounded&1)));
    const auto halfExponent=exponent<113?0:((exponent-112)<<10);
    return std::uint16_t(sign|(halfExponent+rounded));
}
float HalfToFloat(std::uint16_t bits) noexcept {
    const auto sign=std::uint32_t(bits&0x8000)<<16;
    const auto exponent=(bits>>10)&31, mantissa=bits&1023;
    std::uint32_t result=sign;
    if(exponent==31) result|=0x7f800000|std::uint32_t(mantissa)<<13;
    else if(exponent) result|=std::uint32_t(exponent+112)<<23|std::uint32_t(mantissa)<<13;
    else if(mantissa) {
        const auto highest=31u-std::countl_zero(std::uint32_t(mantissa));
        result|=(highest+103)<<23|((std::uint32_t(mantissa)<<(23-highest))&0x7fffff);
    }
    return std::bit_cast<float>(result);
}
std::uint16_t DecodeE4m3ToHalf(std::uint8_t bits) noexcept {
    const auto sign=std::uint16_t(bits&128)<<8;
    const unsigned exponent=(bits>>3)&15, mantissa=bits&7;
    if((bits&127)==127) return 0x7e00;
    if(exponent) return std::uint16_t(sign|((exponent+8)<<10)|(mantissa<<7));
    if(!mantissa) return std::uint16_t(sign);
    const auto highest=31u-std::countl_zero(std::uint32_t(mantissa));
    return std::uint16_t(sign|((highest+6)<<10)|((mantissa<<(10-highest))&1023));
}
std::uint8_t EncodeFloatToE4m3Rne(float value) noexcept {
    const auto bits=std::bit_cast<std::uint32_t>(value);
    const auto sign=(bits>>24)&128, magnitude=bits&0x7fffffff;
    if(magnitude>0x7f800000) return 0x7f;
    if(magnitude>=0x43e00000) return std::uint8_t(sign|0x7e);
    // The midpoint between zero and the smallest subnormal is 2^-10.
    // Guarding it first also bounds every subsequent shift to [20,24].
    if(magnitude<=0x3a800000) return std::uint8_t(sign);
    const auto exponent=magnitude>>23, significand=(magnitude&0x7fffff)|0x800000;
    const unsigned shift=exponent<121?141-exponent:20;
    auto rounded=significand>>shift;
    const auto remainder=significand&((1u<<shift)-1), midpoint=1u<<(shift-1);
    rounded+=(remainder>midpoint || (remainder==midpoint && (rounded&1)));
    const auto code=(exponent<121?0:((exponent-120)<<3)-8)+rounded;
    return std::uint8_t(sign|code);
}
std::expected<void, FormatError> ConvertFormatWords(FormatOperation operation,
    std::span<const std::uint32_t> input, std::span<std::uint32_t> output) noexcept {
    if(operation!=FormatOperation::Float32ToHalf && operation!=FormatOperation::HalfToFloat32 && operation!=FormatOperation::E4m3ToHalf && operation!=FormatOperation::Float32ToE4m3)
        return std::unexpected(FormatError::InvalidOperation);
    if(input.size()!=output.size()) return std::unexpected(FormatError::CountMismatch);
    for(std::size_t i=0;i<input.size();++i) {
        switch(operation) {
        case FormatOperation::Float32ToHalf: output[i]=FloatToHalfRne(std::bit_cast<float>(input[i]));break;
        case FormatOperation::HalfToFloat32: output[i]=std::bit_cast<std::uint32_t>(HalfToFloat(std::uint16_t(input[i])));break;
        case FormatOperation::E4m3ToHalf: output[i]=DecodeE4m3ToHalf(std::uint8_t(input[i]));break;
        case FormatOperation::Float32ToE4m3: output[i]=EncodeFloatToE4m3Rne(std::bit_cast<float>(input[i]));break;
        }
    }
    return {};
}
}
