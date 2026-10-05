#include "NeuralRendering/Amd/C512ProjectionArchive.h"
#include "TestSupport.h"
#include <limits>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using namespace AmdNrTest;

int main() {
    unsigned supported{};
    for(std::uint32_t block=0;block<=71;++block) for(std::uint32_t layer=0;layer<=4;++layer) {
        const bool expected=((block>=23 && block<=30)||(block>=40 && block<=47)) && (layer==1 || layer==3);
        Require(IsC512ProjectionSelection(block,layer)==expected,"SelectionBounds");
        supported+=IsC512ProjectionSelection(block,layer)?1:0;
    }
    Require(supported==32,"exact supported selection count");
    const auto largest=std::numeric_limits<std::uint32_t>::max();
    Require(!IsC512ProjectionSelection(largest,1) && !IsC512ProjectionSelection(23,largest),"SelectionBounds overflow");
    const TestRecord records[]{{"block23.layer1.layer",std::vector<std::byte>(263168)}};
    const auto file=std::filesystem::temp_directory_path()/"TRP AMD NR projection archive fixture.bin";
    Write(file,MakeArchive(records));auto owned=ReadArchive(file);Require(bool(owned),"unknown fixture parsed");
    auto result=ReadKnownC512Projection(*owned,23,1);
    Require(!result && result.error()==ProjectionError::UnsupportedArchive,"UnknownArchive must not trust name or size");
    auto invalidBlock=ReadKnownC512Projection(*owned,22,1);
    Require(!invalidBlock && invalidBlock.error()==ProjectionError::UnsupportedSelection,"unsupported block before identity");
    auto invalidLayer=ReadKnownC512Projection(*owned,23,2);
    Require(!invalidLayer && invalidLayer.error()==ProjectionError::UnsupportedSelection,"unsupported layer before identity");
    std::filesystem::remove(file);
    std::puts("PASS: 32 selection pairs, boundaries and unknown archive rejection");
}
