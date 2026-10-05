#include "FormatConversions.hlsli"
ByteAddressBuffer Input : register(t0);
RWByteAddressBuffer Output : register(u0);
cbuffer Parameters : register(b0) { uint WordCount; uint SourceValues; };
float HalfAt(uint index) {
    uint at=16+index*2;
    return asfloat(HalfFloat((Input.Load(at&~3u)>>(8*(at&3)))&65535));
}
[numthreads(64,1,1)]
void main(uint3 id : SV_DispatchThreadID) {
    if(id.x>=WordCount) return;
    uint sw=Input.Load(0),sh=Input.Load(4),dw=Input.Load(8),dh=Input.Load(12);
    uint result=Input.Load(16+2*SourceValues+id.x*4);
    // Each thread owns all four bytes of one packed word, including any padding.
    [unroll] for(uint lane=0;lane<4;++lane) {
        uint at=id.x*4+lane, local=at&8191, tile=at>>13;
        uint n=(local&3)|((local>>2)&12)|((local<<1)&16)|((local>>4)&480);
        uint pixel=((local>>6)&7)|((local<<1)&8);
        uint x=4*(tile/(dh/4))+pixel/4,y=4*(tile%(dh/4))+pixel%4;
        if(x<min(sw/2,dw) && y<min(sh/2,dh)) {
            uint first=(2*x*sh+2*y)*512+n,second=first+sh*512;
            precise float leftValue=HalfAt(first)+HalfAt(first+512);
            precise float rightValue=HalfAt(second)+HalfAt(second+512);
            uint left=FloatHalf(asuint(leftValue)),right=FloatHalf(asuint(rightValue));
            precise float totalValue=asfloat(HalfFloat(left))+asfloat(HalfFloat(right));
            uint total=FloatHalf(asuint(totalValue));
            precise float averageValue=asfloat(HalfFloat(total))*0.25f;
            uint average=FloatHalf(asuint(averageValue));
            uint encoded=FloatE4m3(HalfFloat(average));
            uint shift=8*lane;
            result=(result&~(255u<<shift))|(encoded<<shift);
        }
    }
    Output.Store(id.x*4,result);
}
