#include "NeuralRendering/Amd/C512Tensor.h"
#include "TestSupport.h"
#include <algorithm>
#include <array>
#include <limits>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using AmdNrTest::Require;

namespace {
std::size_t ForwardGather(unsigned q) {
    auto a=(q<<4)&0x1e00;
    const auto b=q&31;
    const auto c=((q>>3)&0x3c0)|(q&3);
    const auto d=std::min(b,(b-16)&0xffffu)>>2;
    const auto correction=q>4095?-508:0;
    return 16*d+((int(c)+int(a)+correction)|(b>15?8:0));
}
std::size_t PackedAddress(Extent e,unsigned x,unsigned y,unsigned n) {
    return ((x/4)*(e.height/4)+y/4)*8192+ForwardGather((x%4*4+y%4)*512+n);
}
constexpr auto orders=std::array{C512TensorOrder::Canonical,C512TensorOrder::Packed,C512TensorOrder::Blocked16};
void LayoutsAndForwardOracle() {
    std::size_t total=0;
    for(auto e:std::array<Extent,5>{{{4,4},{8,12},{12,8},{32,20},{60,36}}}) {
        const auto l=MakeC512TensorLayout(e).value();
        Require(l.Dimensions()==e && l.ByteCount()==std::size_t(e.width)*e.height*512,"layout metadata");
        std::vector<unsigned char> visits(l.ByteCount());
        for(unsigned x=0;x<e.width;++x) for(unsigned y=0;y<e.height;++y) for(unsigned n=0;n<512;++n) {
            auto p=l.Offset(C512TensorOrder::Packed,x,y,n);
            Require(p && *p==PackedAddress(e,x,y,n),"forward gather address");
            Require(visits[*p]++==0,"packed bijection");
            Require(l.Offset(C512TensorOrder::Canonical,x,y,n).value()==(std::size_t(x)*e.height+y)*512+n,"canonical address");
            Require(l.Offset(C512TensorOrder::Blocked16,x,y,n).value()==((std::size_t(n/16)*e.width+x)*e.height+y)*16+n%16,"blocked address");
        }
        Require(std::all_of(visits.begin(),visits.end(),[](auto n){return n==1;}),"complete packed coverage");
        total+=l.ByteCount();
        std::vector<std::uint8_t> canonical(l.ByteCount()),input(l.ByteCount()),output(l.ByteCount());
        for(auto shift:{0,7,14}) {
            for(std::size_t i=0;i<canonical.size();++i) canonical[i]=std::uint8_t(i>>shift);
            for(auto from:orders) {
                Require(bool(ReorderC512Tensor(l,C512TensorOrder::Canonical,from,canonical,input)),"prepare order");
                // Verify prepared storage independently, not just inverse conversion.
                for(unsigned x=0;x<e.width;++x) for(unsigned y=0;y<e.height;++y) for(unsigned n=0;n<512;++n) {
                    const auto c=(std::size_t(x)*e.height+y)*512+n;
                    const auto a=from==C512TensorOrder::Packed?PackedAddress(e,x,y,n):
                        from==C512TensorOrder::Blocked16?((std::size_t(n/16)*e.width+x)*e.height+y)*16+n%16:c;
                    Require(input[a]==canonical[c],"prepared bits independently checked");
                }
                for(auto to:orders) {
                    Require(bool(ReorderC512Tensor(l,from,to,input,output)),"convert all order pairs");
                    std::vector<std::uint8_t> restored(l.ByteCount());
                    Require(bool(ReorderC512Tensor(l,to,C512TensorOrder::Canonical,output,restored)),"restore canonical");
                    Require(restored==canonical,"bit preserving roundtrip");
                }
            }
        }
    }
    Require(total==1540096,"recovered full extent coverage");
}
void InvalidLayoutsAndBuffers() {
    for(auto e:std::array<Extent,6>{{{0,4},{4,0},{1,4},{4,7},{6,8},{8,6}}}) {
        auto r=MakeC512TensorLayout(e);Require(!r && r.error()==TensorError::Extent,"invalid extents");
    }
    auto huge=MakeC512TensorLayout({0xfffffffcu,0xfffffffcu},std::numeric_limits<std::size_t>::max());
    Require(!huge && huge.error()==TensorError::Overflow,"overflow before budget");
    auto under=MakeC512TensorLayout({4,4},8191);
    Require(!under && under.error()==TensorError::Budget,"insufficient budget");
    const auto l=MakeC512TensorLayout({4,4},8192).value();
    for(auto o:orders) {
        for(auto c:std::array<std::array<unsigned,3>,3>{{{4,0,0},{0,4,0},{0,0,512}}}) {
            auto r=l.Offset(o,c[0],c[1],c[2]);Require(!r && r.error()==TensorError::Coordinate,"coordinate bounds");
        }
    }
    const auto bad=static_cast<C512TensorOrder>(99);
    Require(l.Offset(bad,0,0,0).error()==TensorError::Order,"invalid offset order");
    std::vector<std::uint8_t> a(8193,0xa5),b(8192,0x69);
    auto expect=[&](auto from,auto to,std::span<const std::uint8_t> in,std::span<std::uint8_t> out,TensorError error) {
        const auto oldA=a,oldB=b;
        auto r=ReorderC512Tensor(l,from,to,in,out);
        Require(!r && r.error()==error && a==oldA && b==oldB,"reorder fails before writes");
    };
    expect(bad,orders[0],std::span(a).first(8192),b,TensorError::Order);
    expect(orders[0],bad,std::span(a).first(8192),b,TensorError::Order);
    expect(orders[0],orders[1],std::span(a).first(8191),b,TensorError::CountMismatch);
    expect(orders[0],orders[1],a,b,TensorError::CountMismatch);
    expect(orders[0],orders[1],std::span(a).first(8192),std::span(b).first(8191),TensorError::CountMismatch);
    for(auto from:orders) for(auto to:orders) {
        if(from!=to) expect(from,to,std::span(a).first(8192),std::span(a).first(8192),TensorError::Overlap);
        expect(from,to,std::span(a).first(8192),std::span(a).subspan(1),TensorError::Overlap);
        expect(from,to,std::span(a).subspan(1),std::span(a).first(8192),TensorError::Overlap);
    }
    for(auto o:orders) Require(bool(ReorderC512Tensor(l,o,o,b,b)),"in-place identity permitted");
    Require(std::all_of(b.begin(),b.end(),[](auto n){return n==0x69;}),"identity preserves bits");
}
}
int main() {
    LayoutsAndForwardOracle();InvalidLayoutsAndBuffers();
    std::puts("PASS: C512 tensor layout and byte-preserving conversions");
}
