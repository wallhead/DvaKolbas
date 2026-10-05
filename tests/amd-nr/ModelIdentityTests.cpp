#include "NeuralRendering/Amd/ModelIdentity.h"
#include "TestSupport.h"
#include <array>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using namespace AmdNrTest;
static std::string Hex(const Sha256Digest& d) {
    std::string s;const char* hex="0123456789abcdef";
    for(auto b:d){auto v=std::to_integer<unsigned>(b);s+=hex[v>>4];s+=hex[v&15];}return s;
}
int main() {
    Require(Hex(Sha256({}).value())=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855","empty SHA256");
    const char abc[]="abc";
    Require(Hex(Sha256(std::as_bytes(std::span(abc,3))).value())=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad","abc SHA256");
    const auto path=std::filesystem::temp_directory_path()/"TRP AMD NR identity fixture.bin";
    Write(path,Fixture());auto file=ReadArchive(path);Require(bool(file),"synthetic archive read");
    Require(IdentifyModel(*file)->status==ModelStatus::UnsupportedArchive,"valid archive unsupported");
    const struct {const char* name;std::uint64_t size;} records[]{
#include "NeuralRendering/Amd/KnownModelRecords.inc"
    };
    std::vector<TestRecord> synthetic;
    for(auto& r:records) synthetic.push_back({r.name,std::vector<std::byte>(std::size_t(r.size))});
    auto bytes=MakeArchive(synthetic);Require(bytes.size()==147689451,"known structural fixture size");
    Write(path,bytes);auto sameStructure=ReadArchive(path);Require(bool(sameStructure),"metadata-only fixture read");
    Require(IdentifyModel(*sameStructure)->status==ModelStatus::UnsupportedArchive,"names lengths size cannot bypass digest");
    std::filesystem::remove(path);std::puts("PASS: AMD NR identity");
}
