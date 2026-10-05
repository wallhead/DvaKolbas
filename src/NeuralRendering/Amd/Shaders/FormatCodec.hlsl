ByteAddressBuffer Input : register(t0);
RWByteAddressBuffer Output : register(u0);
cbuffer Parameters : register(b0) { uint Count; uint Operation; };
#include "FormatConversions.hlsli"
[numthreads(64,1,1)]
void main(uint3 id : SV_DispatchThreadID) {
    if(id.x>=Count) return;
    uint value=Input.Load(id.x*4);
    uint result=0;
    if(Operation==0) result=FloatHalf(value);
    else if(Operation==1) result=HalfFloat(value);
    else if(Operation==2) result=E4m3Half(value);
    else if(Operation==3) result=FloatE4m3(value);
    Output.Store(id.x*4,result);
}
