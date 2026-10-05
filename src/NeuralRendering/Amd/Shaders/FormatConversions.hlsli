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
uint FloatE4m3(uint bits) {
    uint sign=(bits>>24)&128, magnitude=bits&0x7fffffff;
    uint result=sign;
    if(magnitude>0x7f800000) result=0x7f;
    else if(magnitude>=0x43e00000) result=sign|0x7e;
    // Covers zero, deep underflow and the even zero/subnormal midpoint.
    // Remaining shift counts are bounded to [20,24].
    else if(magnitude>0x3a800000) {
        uint exponent=magnitude>>23, significand=(magnitude&0x7fffff)|0x800000;
        uint shift=exponent<121?141-exponent:20;
        uint rounded=significand>>shift, remainder=significand&((1u<<shift)-1), midpoint=1u<<(shift-1);
        rounded+=(remainder>midpoint || (remainder==midpoint && (rounded&1)))?1:0;
        result=sign|((exponent<121?0:((exponent-120)<<3)-8)+rounded);
    }
    return result;
}
