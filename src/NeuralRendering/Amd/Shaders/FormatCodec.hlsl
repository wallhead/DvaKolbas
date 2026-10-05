ByteAddressBuffer Input : register(t0);
RWByteAddressBuffer Output : register(u0);
cbuffer Parameters : register(b0) { uint Count; uint Operation; };
uint FloatHalf(uint bits) {
    uint sign=(bits>>16)&0x8000, exponent=(bits>>23)&255, mantissa=bits&0x7fffff;
    uint result=sign;
    if(exponent==255) result=sign|0x7c00|(mantissa?((mantissa>>13)|0x200):0);
    else if(exponent>=143) result=sign|0x7c00;
    else if(exponent>=102) {
        uint shift=exponent<113?126-exponent:13;
        uint significand=exponent<113?(mantissa|0x800000):mantissa;
        uint rounded=significand>>shift, remainder=significand&((1u<<shift)-1), midpoint=1u<<(shift-1);
        rounded+=(remainder>midpoint || (remainder==midpoint && (rounded&1)))?1:0;
        result=sign|((exponent<113?0:((exponent-112)<<10))+rounded);
    }
    return result;
}
uint HalfFloat(uint bits) {
    uint sign=(bits&0x8000)<<16, exponent=(bits>>10)&31, mantissa=bits&1023;
    uint result=sign;
    if(exponent==31) result=sign|0x7f800000|(mantissa<<13);
    else if(exponent) result=sign|((exponent+112)<<23)|(mantissa<<13);
    else if(mantissa) {
        uint highest=firstbithigh(mantissa);
        result=sign|((highest+103)<<23)|((mantissa<<(23-highest))&0x7fffff);
    }
    return result;
}
uint E4m3Half(uint bits) {
    bits&=255;
    uint sign=(bits&128)<<8, exponent=(bits>>3)&15, mantissa=bits&7;
    uint result=sign;
    if((bits&127)==127) result=0x7e00;
    else if(exponent) result=sign|((exponent+8)<<10)|(mantissa<<7);
    else if(mantissa) {
        uint highest=firstbithigh(mantissa);
        result=sign|((highest+6)<<10)|((mantissa<<(10-highest))&1023);
    }
    return result;
}
[numthreads(64,1,1)]
void main(uint3 id : SV_DispatchThreadID) {
    if(id.x>=Count) return;
    uint value=Input.Load(id.x*4);
    uint result=0;
    if(Operation==0) result=FloatHalf(value);
    else if(Operation==1) result=HalfFloat(value);
    else result=E4m3Half(value);
    Output.Store(id.x*4,result);
}
