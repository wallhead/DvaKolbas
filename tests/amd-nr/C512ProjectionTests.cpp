#include "NeuralRendering/Amd/C512Projection.h"
#include "TestSupport.h"
#include <algorithm>
#include <array>
#include <type_traits>
#include <utility>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using namespace AmdNrTest;

namespace {
constexpr std::size_t BankBytes=262144, RecordSize=263168;
// Forward load-address oracle, independent of the codec's bit-deposit map.
std::size_t Address(unsigned n,unsigned k) {
    const auto lane=16*((k%16)/8)+n%16;
    const auto reg=((k%16)/4)%2, byte=k%4;
    auto v2=lane>>1;
    const auto v25=v2&8, v31=(lane<<2)&8;
    v2=(v2&6)|(lane&1);
    const auto root=((v2<<6)|(v25<<2))+v31;
    return root+(n/16)*512+(k/32)*0x4000+reg*16+((k/16)%2)*4+byte;
}
std::array<std::size_t,512> CoefficientAddresses() {
    std::array<std::size_t,512> offsets{};
    std::array<unsigned,512> visits{};
    std::array<unsigned,512> physicalVisits{};
    for(unsigned thread=0;thread<256;++thread) for(unsigned group=thread>>5;group<32;group+=8) {
        const auto n=16*group|(thread&15);
        const auto relative=(((n&0x1f1)|((thread<<2)&8))<<1)|(thread&12);
        Require(relative<1024 && relative%2==0,"coefficient oracle bounds");
        if(visits[n]) Require(offsets[n]==BankBytes+relative,"replicated lane address");
        offsets[n]=BankBytes+relative;
        ++visits[n];++physicalVisits[relative/2];
    }
    for(unsigned n=0;n<512;++n) Require(visits[n]==2 && physicalVisits[n]==2,"coefficient oracle coverage");
    return offsets;
}
void ExactRecordLength() {
    for(auto size:{0u,1u,262143u,262144u,263167u,263169u}) {
        std::vector<std::byte> raw(size);
        auto r=DecodeC512Projection(raw);
        Require(!r && r.error()==ProjectionError::RecordSize,"ExactRecordLength");
    }
}
void ForwardAddressOracle() {
    std::vector<unsigned char> visits(BankBytes);
    for(unsigned n=0;n<512;++n) for(unsigned k=0;k<512;++k) {
        const auto address=Address(n,k);
        Require(address<BankBytes && visits[address]++==0,"forward oracle bijection");
    }
    for(auto shift:{0,5,10,15}) {
        std::vector<std::byte> raw(RecordSize);
        for(std::size_t i=0;i<BankBytes;++i) raw[i]=std::byte((i>>shift)&255);
        auto decoded=DecodeC512Projection(raw);Require(bool(decoded),"ForwardAddressOracle decoded");
        const auto codes=decoded->MatrixCodes();
        Require(codes.size()==BankBytes,"matrix shape");
        for(unsigned n=0;n<512;++n) for(unsigned k=0;k<512;++k)
            Require(codes[n*512+k]==((Address(n,k)>>shift)&255),"ForwardAddressOracle coordinate");
    }
}
void NonSymmetricPermutation() {
    std::vector<std::byte> raw(RecordSize);
    for(unsigned k=0;k<512;++k) raw[Address((37*k+11)%512,k)]=std::byte{0x38};
    auto decoded=DecodeC512Projection(raw);Require(bool(decoded),"NonSymmetricPermutation decoded");
    for(unsigned n=0;n<512;++n) for(unsigned k=0;k<512;++k)
        Require(decoded->MatrixCodes()[n*512+k]==(n==(37*k+11)%512?0x38:0),"NonSymmetricPermutation");
}
void RawStorageBits() {
    std::vector<std::byte> raw(RecordSize);
    for(unsigned n=0;n<512;++n) for(unsigned k=0;k<512;++k) raw[Address(n,k)]=std::byte(k&255);
    const auto offsets=CoefficientAddresses();
    constexpr std::uint16_t special[]{0x8000,0x0001,0x7c00,0x7e01,0xfc01};
    std::array<std::uint16_t,512> expected{};
    for(unsigned n=0;n<512;++n) {
        expected[n]=n<std::size(special)?special[n]:std::uint16_t(n*109+17);
        Put(raw,offsets[n],expected[n],2);
    }
    auto decoded=DecodeC512Projection(raw);Require(bool(decoded),"RawStorageBits decoded");
    Require(decoded->ResidualHalfBits().size()==512,"coefficient shape");
    for(unsigned n=0;n<512;++n) {
        Require(decoded->ResidualHalfBits()[n]==expected[n],"RawStorageBits half payload");
        for(unsigned k=0;k<512;++k) Require(decoded->MatrixCodes()[n*512+k]==(k&255),"RawStorageBits FP8 code");
    }
}
template<typename T> concept MatrixGetter=requires(T&& owner){std::forward<T>(owner).MatrixCodes();};
template<typename T> concept HalfGetter=requires(T&& owner){std::forward<T>(owner).ResidualHalfBits();};
void OwnedResultLifetime() {
    static_assert(!std::is_copy_constructible_v<C512ProjectionWeights>);
    static_assert(!std::is_copy_assignable_v<C512ProjectionWeights>);
    static_assert(std::is_nothrow_move_constructible_v<C512ProjectionWeights>);
    static_assert(MatrixGetter<const C512ProjectionWeights&> && HalfGetter<const C512ProjectionWeights&>);
    static_assert(!MatrixGetter<C512ProjectionWeights> && !HalfGetter<C512ProjectionWeights>);
    std::span<const std::uint8_t> matrixView;
    std::span<const std::uint16_t> halfView;
    auto finalOwner=[&] {
        std::vector<std::byte> raw(RecordSize,std::byte{0x38});
        const auto offsets=CoefficientAddresses();Put(raw,offsets[511],0x8000,2);
        auto decoded=DecodeC512Projection(raw);Require(bool(decoded),"OwnedResultLifetime decoded");
        matrixView=decoded->MatrixCodes();halfView=decoded->ResidualHalfBits();
        std::fill(raw.begin(),raw.end(),std::byte{0});
        Require(matrixView.front()==0x38 && halfView[511]==0x8000,"owned independently of modified input");
        return C512ProjectionWeights(std::move(*decoded));
    }();
    Require(matrixView.data()==finalOwner.MatrixCodes().data() && halfView.data()==finalOwner.ResidualHalfBits().data(),"views survive original owner destruction");
    Require(matrixView.back()==0x38 && halfView[511]==0x8000,"OwnedResultLifetime contents");
}
}
int main() {
    ExactRecordLength();ForwardAddressOracle();NonSymmetricPermutation();RawStorageBits();OwnedResultLifetime();
    std::puts("PASS: C512 exact length, exhaustive independent addresses, non-symmetric permutation, raw bits and owned views");
}
