#include "C512Projection.h"
#include <array>
#include <new>
#include <utility>
#include <vector>

namespace TheosRenderPipeline::NeuralRendering::Amd {
struct C512ProjectionWeights::Storage {
    std::vector<std::uint8_t> matrixCodes;
    std::array<std::uint16_t,Channels> residualHalfBits{};
};
C512ProjectionWeights::C512ProjectionWeights(std::unique_ptr<Storage> storage) noexcept:storage_(std::move(storage)) {}
C512ProjectionWeights::~C512ProjectionWeights()=default;
C512ProjectionWeights::C512ProjectionWeights(C512ProjectionWeights&&) noexcept=default;
std::span<const std::uint8_t> C512ProjectionWeights::MatrixCodes() const & noexcept {
    return storage_?std::span<const std::uint8_t>(storage_->matrixCodes):std::span<const std::uint8_t>{};
}
std::span<const std::uint16_t> C512ProjectionWeights::ResidualHalfBits() const & noexcept {
    return storage_?std::span<const std::uint16_t>(storage_->residualHalfBits):std::span<const std::uint16_t>{};
}
namespace {
constexpr std::size_t BankBytes=512*512, RecordBytes=BankBytes+512*2;
constexpr std::array<unsigned,9> OutputBits{6,3,7,8,9,10,11,12,13};
constexpr std::array<unsigned,9> InputBits{0,1,4,5,2,14,15,16,17};
constexpr std::array<unsigned,9> CoefficientBits{0,3,1,2,4,5,6,7,8};
std::size_t Deposit(std::size_t value,const std::array<unsigned,9>& positions) noexcept {
    std::size_t offset{};
    for(unsigned bit=0;bit<positions.size();++bit) offset|=((value>>bit)&1)<<positions[bit];
    return offset;
}
}
std::expected<C512ProjectionWeights,ProjectionError> DecodeC512Projection(std::span<const std::byte> raw) {
    if(raw.size()!=RecordBytes) return std::unexpected(ProjectionError::RecordSize);
    try {
        auto storage=std::make_unique<C512ProjectionWeights::Storage>();
        storage->matrixCodes.resize(BankBytes);
        std::array<std::size_t,512> inputOffsets{};
        for(std::size_t k=0;k<512;++k) inputOffsets[k]=Deposit(k,InputBits);
        for(std::size_t n=0;n<512;++n) {
            const auto outputOffset=Deposit(n,OutputBits);
            for(std::size_t k=0;k<512;++k)
                storage->matrixCodes[n*512+k]=std::to_integer<std::uint8_t>(raw[outputOffset|inputOffsets[k]]);
            const auto offset=BankBytes+2*Deposit(n,CoefficientBits);
            storage->residualHalfBits[n]=std::uint16_t(std::to_integer<unsigned>(raw[offset])|
                (std::to_integer<unsigned>(raw[offset+1])<<8));
        }
        return C512ProjectionWeights(std::move(storage));
    } catch(const std::bad_alloc&) {return std::unexpected(ProjectionError::Memory);}
}
}
