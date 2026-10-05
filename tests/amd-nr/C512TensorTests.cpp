#include "NeuralRendering/Amd/C512Tensor.h"
#include "NeuralRendering/Amd/C512Reduction.h"
#include "NeuralRendering/Amd/NumericFormats.h"
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
void HalfReductionBoundaries() {
    constexpr std::array<std::array<std::uint16_t,4>,9> values{{
        {0x6400,0x3800,0xe400,0x3800}, // half boundaries: 0.125, not 0.25
        {0x8000,0x8000,0x8000,0x8000},
        {0x7c00,0,0xfc00,0},
        {0x7bff,0x7bff,0x7bff,0x7bff},
        {0x7d01,0,0,0},
        {0x0001,0x0001,0x0001,0x0001},
        {0x3c00,0x3c00,0x3c00,0x3c00},
        {0xbc00,0xbc00,0xbc00,0xbc00},
        {0x0000,0x8000,0x8000,0x8000}
    }};
    constexpr std::array<std::uint8_t,9> encoded{0x20,0x80,0x7f,0x7e,0x7f,0x00,0x38,0xb8,0x00};
    struct Pair { Extent source,destination; };
    constexpr Pair cases[]{{{4,4},{4,4}},{{60,36},{32,20}},{{8,12},{4,4}}};
    for(auto dims:cases) {
        const auto src=MakeC512TensorLayout(dims.source).value(),dst=MakeC512TensorLayout(dims.destination).value();
        const auto e=src.Dimensions(),d=dst.Dimensions();
        std::vector<std::uint16_t> input(src.ByteCount());
        std::vector<std::uint8_t> output(dst.ByteCount(),0xa5);
        for(unsigned x=0;x<e.width;++x) for(unsigned y=0;y<e.height;++y) for(unsigned n=0;n<512;++n) {
            const auto pattern=((x/2)*7+(y/2)*3+n)%values.size();
            input[(std::size_t(x)*e.height+y)*512+n]=values[pattern][(x%2)*2+y%2];
        }
        Require(bool(ReduceC512Half2x2(src,dst,input,output)),"half reduction succeeds");
        std::size_t padding=0;
        for(unsigned x=0;x<d.width;++x) for(unsigned y=0;y<d.height;++y) for(unsigned n=0;n<512;++n) {
            const auto active=x<e.width/2 && y<e.height/2;
            const auto want=active?encoded[(x*7+y*3+n)%values.size()]:0xa5;
            Require(output[PackedAddress(d,x,y,n)]==want,"literal values, spatial/channel association and padding");
            padding+=!active;
        }
        if(e==Extent{60,36}) Require(padding==51200,"exact padded tail count");
        const auto before=output;
        auto bad=ReduceC512Half2x2(src,dst,std::span(input).first(input.size()-1),output);
        Require(!bad && bad.error()==TensorError::CountMismatch && output==before,"half count error before writes");
        bad=ReduceC512Half2x2(src,dst,input,std::span(output).first(output.size()-1));
        Require(!bad && bad.error()==TensorError::CountMismatch && output==before,"output count error before writes");
    }
    const auto l=MakeC512TensorLayout({4,4}).value();
    std::vector<std::uint16_t> data(l.ByteCount(),0x1234);
    for(auto offset:{0u,1u,8192u}) {
        const auto before=data;
        auto bytes=std::span(reinterpret_cast<std::uint8_t*>(data.data())+offset,l.ByteCount());
        auto r=ReduceC512Half2x2(l,l,data,bytes);
        Require(!r && r.error()==TensorError::Overlap && data==before,"half/byte overlap before writes");
    }
    std::vector<std::uint16_t> shifted(l.ByteCount()+1,0x4321);
    const auto before=shifted;
    auto bytes=std::span(reinterpret_cast<std::uint8_t*>(shifted.data()),l.ByteCount());
    auto reverse=ReduceC512Half2x2(l,l,std::span(shifted).subspan(1),bytes);
    Require(!reverse && reverse.error()==TensorError::Overlap && shifted==before,"reverse half/byte overlap");
    auto extended=ReduceC512Half2x2(l,l,shifted,bytes);
    Require(!extended && extended.error()==TensorError::CountMismatch && shifted==before,"extended half count before writes");
    const auto huge=MakeC512TensorLayout({0x40000000u,0x01000000u},std::numeric_limits<std::size_t>::max()).value();
    auto overflow=ReduceC512Half2x2(huge,l,{},{});
    Require(!overflow && overflow.error()==TensorError::Overflow,"half byte count overflow before span access");
}
void OracleDriver(const char* inputPath,const char* outputPath) {
    const auto src=MakeC512TensorLayout({64,16}).value(),dst=MakeC512TensorLayout({32,8}).value();
    std::vector<std::uint16_t> half(src.ByteCount());
    std::ifstream input(inputPath,std::ios::binary);
    Require(bool(input.read(reinterpret_cast<char*>(half.data()),std::streamsize(half.size()*2))),"oracle exact input read");
    Require(input.peek()==std::char_traits<char>::eof(),"oracle input has no tail");
    std::vector<std::uint8_t> packed(dst.ByteCount(),0xa5),canonical(dst.ByteCount());
    Require(bool(ReduceC512Half2x2(src,dst,half,packed)),"oracle reduction");
    Require(bool(ReorderC512Tensor(dst,C512TensorOrder::Packed,C512TensorOrder::Canonical,packed,canonical)),"oracle output view");
    std::ofstream output(outputPath,std::ios::binary|std::ios::trunc);
    output.write(reinterpret_cast<const char*>(canonical.data()),std::streamsize(canonical.size()));
    output.close();Require(bool(output),"oracle output write/close");
}
}
int main(int argc,char** argv) {
    if(argc==4 && std::string(argv[1])=="--oracle") { OracleDriver(argv[2],argv[3]);return 0; }
    Require(argc==1,"test executable arguments");
    LayoutsAndForwardOracle();InvalidLayoutsAndBuffers();HalfReductionBoundaries();
    std::puts("PASS: C512 tensor layout, storage conversion and half reduction");
}
