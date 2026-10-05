#include "C512ProjectionArchive.h"
#include "ModelIdentity.h"
#include <new>
#include <string>

namespace TheosRenderPipeline::NeuralRendering::Amd {
bool IsC512ProjectionSelection(std::uint32_t block,std::uint32_t layer) noexcept {
    return ((block>=23 && block<=30)||(block>=40 && block<=47)) && (layer==1 || layer==3);
}
std::expected<C512ProjectionWeights,ProjectionError> ReadKnownC512Projection(
    const ArchiveFile& file,std::uint32_t block,std::uint32_t layer) {
    if(!IsC512ProjectionSelection(block,layer)) return std::unexpected(ProjectionError::UnsupportedSelection);
    try {
        auto identification=IdentifyModel(file);
        if(!identification) return std::unexpected(identification.error()==IdentityError::Memory?ProjectionError::Memory:ProjectionError::Crypto);
        if(identification->status!=ModelStatus::KnownArchiveIncompleteSchema) return std::unexpected(ProjectionError::UnsupportedArchive);
        const auto name="block"+std::to_string(block)+".layer"+std::to_string(layer)+".layer";
        const auto& records=file.Index().records;
        for(std::size_t i=0;i<records.size();++i) if(records[i].name==name) {
            auto raw=RecordBytes(file,i);
            if(!raw) return std::unexpected(ProjectionError::Archive);
            return DecodeC512Projection(*raw);
        }
        return std::unexpected(ProjectionError::Archive);
    } catch(const std::bad_alloc&) {return std::unexpected(ProjectionError::Memory);}
}
}
