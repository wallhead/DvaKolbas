#include "FormatConversions.hlsli"
ByteAddressBuffer Input : register(t0);
RWByteAddressBuffer Output : register(u0);
cbuffer Parameters : register(b0) { uint Count; uint Reserved; };
uint LoadByte(uint at) { return (Input.Load(at&~3u)>>(8*(at&3)))&255; }
float Decode(uint code) { return asfloat(HalfFloat(E4m3Half(code))); }
uint ProjectionHalf(float value) {
    uint bits=asuint(value);
    return (bits&0x7fffffff)>0x7f800000?0x7e00:FloatHalf(bits);
}
[numthreads(64,1,1)]
void main(uint3 id : SV_DispatchThreadID) {
    if(id.x>=Count) return;
    uint pixel=id.x/512, n=id.x%512;
    uint coefficientAt=2*Count+262144+2*n;
    uint coefficient=(Input.Load(coefficientAt&~3u)>>(8*(coefficientAt&3)))&65535;
    precise float product=Decode(LoadByte(Count+id.x))*asfloat(HalfFloat(coefficient));
    // FXC folds addition of +0 even with IEEE/precise. Its only finite effect here
    // is -0 -> +0; state that in bits without erasing negative nonzero underflow.
    precise float initialValue=asfloat((asuint(product)&0x7fffffff)==0?0:asuint(product));
    uint initial=ProjectionHalf(initialValue), accumulator=initial;
    // Our serial FP32 order. WMMA's internal addition order remains unresolved.
    [loop] for(uint chunk=0;chunk<16;++chunk) {
        precise float sum=0.0f;
        [loop] for(uint j=0;j<32;++j) {
            uint k=32*chunk+j;
            precise float term=Decode(LoadByte(pixel*512+k))*Decode(LoadByte(2*Count+n*512+k));
            sum=sum+term;
        }
        precise float combined=sum+asfloat(HalfFloat(accumulator));
        accumulator=ProjectionHalf(combined);
    }
    uint encoded=FloatE4m3(HalfFloat(accumulator));
    Output.Store(id.x*8,accumulator|(encoded<<16));
    Output.Store(id.x*8+4,initial);
}
