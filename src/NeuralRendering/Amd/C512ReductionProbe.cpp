#include "C512ReductionProbe.h"
#include "C512ReductionBytecode.h"
#include <new>
namespace TheosRenderPipeline::NeuralRendering::Amd {
std::expected<C512ReductionProbe,ProbeError> C512ReductionProbe::Create(ID3D12Device* device,ID3D12CommandQueue* queue) {
    auto transport=FormatProbe::CreateCompute(device,queue,AmdNrShader::C512Reduction);
    if(!transport) return std::unexpected(transport.error());return C512ReductionProbe(std::move(*transport));
}
std::expected<FormatProbeJob,ProbeError> C512ReductionProbe::Submit(const C512TensorLayout& source,const C512TensorLayout& destination,
    std::span<const std::uint16_t> input,std::span<const std::uint8_t> initial) {
    if(!transport_.state_) return std::unexpected(ProbeError::InvalidDevice);
    const auto count=source.ByteCount(),bytes=destination.ByteCount();
    if(count>C512ReductionMaxSourceValues || bytes>C512ReductionMaxDestinationBytes ||
        input.size()!=count || initial.size()!=bytes) return std::unexpected(ProbeError::Count);
    try {
        const auto s=source.Dimensions(),d=destination.Dimensions();
        std::vector<std::uint32_t> payload(4+count/2+bytes/4);
        payload[0]=s.width;payload[1]=s.height;payload[2]=d.width;payload[3]=d.height;
        for(std::size_t i=0;i<count;++i) payload[4+i/2]|=std::uint32_t(input[i])<<(16*(i%2));
        const auto base=4+count/2;
        for(std::size_t i=0;i<bytes;++i) payload[base+i/4]|=std::uint32_t(initial[i])<<(8*(i%4));
        return transport_.SubmitCompute(payload,bytes/4,bytes/4,{std::uint32_t(bytes/4),std::uint32_t(count)});
    } catch(const std::bad_alloc&) {return std::unexpected(ProbeError::Memory);}
}
std::expected<std::vector<std::uint8_t>,ProbeError> C512ReductionProbe::Readback(FormatProbeJob& job,std::chrono::milliseconds timeout) {
    auto words=transport_.Readback(job,timeout);if(!words) return std::unexpected(words.error());
    try {
        std::vector<std::uint8_t> bytes(words->size()*4);
        for(std::size_t i=0;i<bytes.size();++i) bytes[i]=std::uint8_t((*words)[i/4]>>(8*(i%4)));
        return bytes;
    } catch(const std::bad_alloc&) {return std::unexpected(ProbeError::Memory);}
}
}
