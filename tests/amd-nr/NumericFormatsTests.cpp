#include "NeuralRendering/Amd/NumericFormats.h"
#include "TestSupport.h"
#include <array>
#include <bit>
#include <cmath>
#include <limits>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using AmdNrTest::Require;

static float MathematicalHalf(unsigned code) {
    const unsigned e=(code>>10)&31, m=code&1023;
    double value=e==0?std::ldexp(double(m),-24):std::ldexp(1.0+double(m)/1024,int(e)-15);
    if(e==31) value=m?std::numeric_limits<double>::quiet_NaN():std::numeric_limits<double>::infinity();
    return std::copysign(float(value),(code&0x8000)?-1.0f:1.0f);
}
int main() {
    const std::pair<unsigned,unsigned> anchors[]{{0,0},{1,0x1800},{7,0x2300},{8,0x2400},{0x38,0x3c00},{0x7e,0x5f00},{0x7f,0x7e00},{0x80,0x8000},{0xb8,0xbc00},{0xfe,0xdf00},{0xff,0x7e00}};
    for(auto [a,b]:anchors) Require(DecodeE4m3ToHalf(std::uint8_t(a))==b,"E4M3 anchor");
    for(unsigned c=0;c<256;++c) {
        auto half=DecodeE4m3ToHalf(std::uint8_t(c));
        if((c&127)==127) { Require(half==0x7e00,"E4M3 canonical NaN"); continue; }
        unsigned e=(c>>3)&15,m=c&7;
        float expected=std::copysign(float(e?std::ldexp(1.0+double(m)/8,int(e)-7):double(m)/512),(c&128)?-1.0f:1.0f);
        Require(std::bit_cast<std::uint32_t>(HalfToFloat(half))==std::bit_cast<std::uint32_t>(expected),"E4M3 mathematical decoding");
        Require(DecodeE4m3ToHalf(std::uint8_t(c^128))==(half^0x8000),"E4M3 sign symmetry");
    }
    for(unsigned c=0;c<65536;++c) {
        float result=HalfToFloat(std::uint16_t(c));
        unsigned e=(c>>10)&31,m=c&1023;
        if(e==31 && m) {
            Require(std::bit_cast<std::uint32_t>(result)==((c&0x8000)<<16|0x7f800000|m<<13),"half NaN payload and signalling preserved");
        } else {
            Require(std::bit_cast<std::uint32_t>(result)==std::bit_cast<std::uint32_t>(MathematicalHalf(c)),"all half values mathematical expansion");
            Require(FloatToHalfRne(result)==c,"half finite/infinite roundtrip");
        }
    }
    // Adjacent positive finite values have an exactly representable FP32 midpoint.
    for(unsigned h=0;h<0x7bff;++h) {
        float a=MathematicalHalf(h),b=MathematicalHalf(h+1),mid=(a+b)*0.5f;
        for(unsigned sign:{0u,0x8000u}) {
            float s=sign?-1.0f:1.0f;
            Require(FloatToHalfRne(mid*s)==(sign|(h+(h&1))),"midpoint tie to even");
            Require(FloatToHalfRne(std::nextafter(mid,a)*s)==(sign|h),"below midpoint");
            Require(FloatToHalfRne(std::nextafter(mid,b)*s)==(sign|(h+1)),"above midpoint");
        }
    }
    for(unsigned sign:{0u,0x80000000u}) {
        auto f=[sign](unsigned bits){return std::bit_cast<float>(sign|bits);};
        unsigned hs=sign>>16;
        Require(FloatToHalfRne(f(0))==hs,"signed zero");
        Require(FloatToHalfRne(f(0x477fe000))==(hs|0x7bff),"largest finite half");
        Require(FloatToHalfRne(f(0x477ff000))==(hs|0x7c00),"overflow midpoint");
        Require(FloatToHalfRne(f(0x7f800000))==(hs|0x7c00),"infinity");
        for(unsigned bits:{0x7f800001u,0x7fa00000u,0x7fc12345u,0x7fffffffu})
            Require(FloatToHalfRne(f(bits))==(hs|0x7c00|((bits&0x7fffff)>>13)|0x200),"quiet retained FP32 NaN payload");
    }
    std::array<std::uint32_t,3> in{0xffff0001,0xffff8000,0xffff7c01},out{9,9,9};
    Require(bool(ConvertFormatWords(FormatOperation::HalfToFloat32,in,out)),"batch half expansion");
    Require(out[0]==0x33800000 && out[1]==0x80000000 && out[2]==0x7f802000,"batch ignores high half bits");
    Require(bool(ConvertFormatWords(FormatOperation::E4m3ToHalf,in,out)),"batch byte expansion");
    Require(out[0]==0x1800 && out[1]==0 && out[2]==0x1800,"batch ignores high byte bits and clears high output bits");
    in={0x3f800000,0x80000000,0x7fc00000};
    Require(bool(ConvertFormatWords(FormatOperation::Float32ToHalf,in,out)) && out==std::array<std::uint32_t,3>{0x3c00,0x8000,0x7e00},"batch FP32 conversion");
    out={9,9,9};
    Require(!ConvertFormatWords(static_cast<FormatOperation>(99),in,out) && out==std::array<std::uint32_t,3>{9,9,9},"invalid operation makes no writes");
    Require(!ConvertFormatWords(FormatOperation::Float32ToHalf,in,std::span(out).first(2)) && out==std::array<std::uint32_t,3>{9,9,9},"unequal spans make no writes");
    Require(bool(ConvertFormatWords(FormatOperation::Float32ToHalf,{},{})),"empty spans valid");
    std::puts("PASS: AMD NR numeric formats");
}
