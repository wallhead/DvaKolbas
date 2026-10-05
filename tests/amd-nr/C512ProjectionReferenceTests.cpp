#include "NeuralRendering/Amd/C512ProjectionReference.h"
#include "NeuralRendering/Amd/NumericFormats.h"
#include "TestSupport.h"
#include <array>
#include <algorithm>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using AmdNrTest::Require;

namespace {
std::size_t WeightAddress(unsigned n,unsigned k) {
    const auto lane=16*((k%16)/8)+n%16;
    auto v2=lane>>1;
    const auto v25=v2&8,v31=(lane<<2)&8;
    v2=(v2&6)|(lane&1);
    return ((v2<<6)|(v25<<2))+v31+(n/16)*512+(k/32)*0x4000+
        (((k%16)/4)%2)*16+((k/16)%2)*4+k%4;
}
C512ProjectionWeights Weights(std::span<const std::uint8_t> matrix,const std::array<std::uint16_t,512>& coefficients={}) {
    std::vector<std::byte> raw(263168);
    for(unsigned n=0;n<512;++n) for(unsigned k=0;k<512;++k) raw[WeightAddress(n,k)]=std::byte(matrix[n*512+k]);
    for(unsigned thread=0;thread<256;++thread) for(unsigned group=thread>>5;group<32;group+=8) {
        const auto n=16*group|(thread&15);
        const auto address=262144+(((n&0x1f1)|((thread<<2)&8))<<1)|(thread&12);
        AmdNrTest::Put(raw,address,coefficients[n],2);
    }
    return DecodeC512Projection(raw).value();
}
std::vector<std::uint32_t> Evaluate(const C512TensorLayout& l,std::span<const std::uint8_t> input,
    std::span<const std::uint8_t> residual,const C512ProjectionWeights& weights) {
    std::vector<std::uint32_t> out(l.ByteCount()*C512ProjectionWordsPerValue,0xa5a5a5a5);
    Require(bool(EvaluateC512ProjectionReference(l,input,residual,weights,out)),"CPU ordered reference");
    for(std::size_t i=0;i<out.size();i+=2) Require(!(out[i]&0xff000000) && !(out[i+1]&0xffff0000),"diagnostic zero extension");
    return out;
}
void AsymmetricPixelChannels() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount());
    for(unsigned k=0;k<512;++k) matrix[((37*k+11)%512)*512+k]=0x38;
    for(unsigned p=0;p<16;++p) for(unsigned k=0;k<512;++k) input[p*512+k]=std::uint8_t((p+k)%126+1|(((p+k)&1)<<7));
    const auto weights=Weights(matrix);
    const auto out=Evaluate(l,input,{},weights);
    for(unsigned p=0;p<16;++p) for(unsigned k=0;k<512;++k) {
        const auto at=2*(p*512+(37*k+11)%512);
        const auto code=input[p*512+k];
        Require(out[at]==(std::uint32_t(code)<<16|DecodeE4m3ToHalf(code)) && out[at+1]==0,"asymmetric pixel/channel values");
    }
}
void ResidualAndChunkBoundaries() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount(),1),residual(l.ByteCount(),0xb0);
    std::array<std::uint16_t,512> coef{};coef.fill(0x3800);
    auto w=Weights(matrix,coef);
    auto out=Evaluate(l,input,residual,w);
    for(std::size_t i=0;i<out.size();i+=2) Require(out[i]==0x00a8b400 && out[i+1]==0xb400,"zero matrix preserves early half residual");
}
void ChunkRounding() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount(),1);
    std::fill(matrix.begin(),matrix.begin()+512,1);matrix[0]=0x7e;
    const auto weights=Weights(matrix);const auto out=Evaluate(l,input,{},weights);
    for(unsigned p=0;p<16;++p) Require(out[p*1024]==0x00363b00 && out[p*1024+1]==0,"half boundary per 32 terms");
}
void EarlyResidualRounding() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount()),residual(l.ByteCount());
    matrix[0]=0xb8;
    std::array<std::uint16_t,512> coef{};coef[0]=0x3c01;
    for(unsigned p=0;p<16;++p) { input[p*512]=0x39;residual[p*512]=0x39; }
    const auto weights=Weights(matrix,coef);const auto out=Evaluate(l,input,residual,weights);
    for(unsigned p=0;p<16;++p) Require(out[p*1024]==0x00001400 && out[p*1024+1]==0x3c81,"early residual half rounding");
}
void SpecialValues() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount()),residual(l.ByteCount(),0x80);
    matrix[511*512]=0x7f;
    std::array<std::uint16_t,512> coef{};coef.fill(0x3c00);
    coef[0]=coef[1]=coef[2]=0x7c00;coef[3]=0x7d01;
    for(unsigned p=0;p<16;++p) {
        residual[p*512]=0;residual[p*512+1]=0x38;residual[p*512+2]=0xb8;residual[p*512+3]=0x38;
    }
    const auto weights=Weights(matrix,coef);const auto out=Evaluate(l,input,residual,weights);
    constexpr std::uint32_t final[]{0x007f7e00,0x007e7c00,0x00fefc00,0x007f7e00};
    constexpr std::uint16_t initial[]{0x7e00,0x7c00,0xfc00,0x7e00};
    for(unsigned p=0;p<16;++p) for(unsigned n=0;n<512;++n) {
        const auto at=2*(p*512+n);
        Require(out[at]==(n<4?final[n]:n==511?0x007f7e00:0),"special final half/encoding");
        Require(out[at+1]==(n<4?initial[n]:0),"special and negative-zero initialization");
    }
}
void ValidationBeforeWrites() {
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint8_t> matrix(262144),input(l.ByteCount()),residual(l.ByteCount());
    auto weights=Weights(matrix);
    std::vector<std::uint32_t> output(l.ByteCount()*2,0x69426942);
    auto check=[&](const auto& layout,std::span<const std::uint8_t> in,std::span<const std::uint8_t> res,
                  const auto& w,std::span<std::uint32_t> out,ProjectionEvaluationError error) {
        const auto old=output;
        const auto r=EvaluateC512ProjectionReference(layout,in,res,w,out);
        Require(!r && r.error()==error && output==old,"validation before writes");
    };
    check(l,std::span(input).first(input.size()-1),{},weights,output,ProjectionEvaluationError::Count);
    check(l,input,std::span(residual).first(residual.size()-1),weights,output,ProjectionEvaluationError::Count);
    check(l,input,{},weights,std::span(output).first(output.size()-1),ProjectionEvaluationError::Count);
    check(MakeC512TensorLayout({20,16}).value(),input,{},weights,output,ProjectionEvaluationError::Extent);
    auto bytes=std::span(reinterpret_cast<const std::uint8_t*>(output.data()),l.ByteCount());
    check(l,bytes,{},weights,output,ProjectionEvaluationError::Overlap);
    check(l,input,bytes,weights,output,ProjectionEvaluationError::Overlap);
    auto owner=std::move(weights);
    check(l,input,{},weights,output,ProjectionEvaluationError::Weights);
    Require(bool(ValidateC512ProjectionInputs(l,input,{},owner)),"moved-to weights valid");
}
}
int main() {
    AsymmetricPixelChannels();ResidualAndChunkBoundaries();ChunkRounding();EarlyResidualRounding();SpecialValues();ValidationBeforeWrites();
    std::puts("PASS: ordered C512 CPU reference, rounding boundaries, special values and validation");
}
