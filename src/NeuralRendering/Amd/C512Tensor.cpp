#include "C512Tensor.h"
#include <algorithm>
#include <array>
#include <limits>

namespace TheosRenderPipeline::NeuralRendering::Amd {
namespace {
bool ValidOrder(C512TensorOrder o) noexcept {
    return o==C512TensorOrder::Canonical || o==C512TensorOrder::Packed || o==C512TensorOrder::Blocked16;
}
template<std::size_t N>
std::size_t Deposit(std::uint32_t value,const std::array<unsigned,N>& positions) noexcept {
    std::size_t result=0;
    for(unsigned i=0;i<N;++i) result|=std::size_t((value>>i)&1)<<positions[i];
    return result;
}
std::size_t Address(Extent e,C512TensorOrder order,std::uint32_t x,std::uint32_t y,std::uint32_t n) noexcept {
    if(order==C512TensorOrder::Canonical) return (std::size_t(x)*e.height+y)*512+n;
    if(order==C512TensorOrder::Blocked16) return ((std::size_t(n/16)*e.width+x)*e.height+y)*16+n%16;
    constexpr std::array<unsigned,9> channels{0,1,4,5,3,9,10,11,12};
    constexpr std::array<unsigned,4> pixels{6,7,8,2};
    const auto tile=std::size_t(x/4)*(e.height/4)+y/4;
    return tile*8192+(Deposit(n,channels)|Deposit((x%4)*4+y%4,pixels));
}
}
std::expected<C512TensorLayout,TensorError> MakeC512TensorLayout(Extent e,std::size_t budget) noexcept {
    if(!e.width || !e.height || e.width%4 || e.height%4) return std::unexpected(TensorError::Extent);
    if(std::size_t(e.width)>std::numeric_limits<std::size_t>::max()/512/e.height)
        return std::unexpected(TensorError::Overflow);
    const auto bytes=std::size_t(e.width)*e.height*512;
    if(bytes>budget) return std::unexpected(TensorError::Budget);
    return C512TensorLayout(e,bytes);
}
std::expected<std::size_t,TensorError> C512TensorLayout::Offset(C512TensorOrder order,
    std::uint32_t x,std::uint32_t y,std::uint32_t n) const noexcept {
    if(!ValidOrder(order)) return std::unexpected(TensorError::Order);
    if(x>=extent_.width || y>=extent_.height || n>=512) return std::unexpected(TensorError::Coordinate);
    return Address(extent_,order,x,y,n);
}
std::expected<void,TensorError> ReorderC512Tensor(const C512TensorLayout& l,
    C512TensorOrder from,C512TensorOrder to,
    std::span<const std::uint8_t> input,std::span<std::uint8_t> output) noexcept {
    if(!ValidOrder(from) || !ValidOrder(to)) return std::unexpected(TensorError::Order);
    if(input.size()!=l.ByteCount() || output.size()!=l.ByteCount()) return std::unexpected(TensorError::CountMismatch);
    if(from==to && input.data()==output.data()) return {};
    const auto a=reinterpret_cast<std::uintptr_t>(input.data()),b=reinterpret_cast<std::uintptr_t>(output.data());
    if(a>=b ? a-b<output.size() : b-a<input.size()) return std::unexpected(TensorError::Overlap);
    if(from==to) { std::copy(input.begin(),input.end(),output.begin());return {}; }
    const auto e=l.Dimensions();
    for(std::uint32_t x=0;x<e.width;++x) for(std::uint32_t y=0;y<e.height;++y) for(std::uint32_t n=0;n<512;++n)
        output[Address(e,to,x,y,n)]=input[Address(e,from,x,y,n)];
    return {};
}
}
