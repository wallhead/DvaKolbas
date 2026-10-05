#include "NeuralRendering/Amd/WeightArchive.h"
#include "TestSupport.h"
#include <limits>
#include <random>
#include <type_traits>
using namespace TheosRenderPipeline::NeuralRendering::Amd;
using namespace AmdNrTest;
int main() {
    auto valid=Fixture();const auto result=ParseArchive(valid);
    Require(bool(result),"ValidTwoRecordArchive"); const auto& index=*result;
    Require(index.payloadStart==61 && index.fileSize==66,"absolute payload/EOF");
    Require(index.records.size()==2 && index.records[0].name=="first" && index.records[1].name=="second","names");
    Require(index.records[0].offset==0 && index.records[0].size==3 && index.records[1].offset==3 && index.records[1].size==2,"ranges");
    for (std::size_t i=0;i<valid.size();++i) Require(!ParseArchive(std::span(valid).first(i)),"TruncationAtEveryByte");
    auto bad=valid;bad[0]=std::byte{};Require(!ParseArchive(bad),"Magic");
    bad=valid;Put(bad,8,0,4);Require(!ParseArchive(bad),"ZeroCount");
    bad=valid;Put(bad,8,4097,4);Require(!ParseArchive(bad),"CountBudget");
    Require(!ParseArchive(valid,ArchiveLimits{65,4096}),"ByteBudget");
    bad=valid;bad[16]=std::byte{};Require(!ParseArchive(bad),"EmptyName");
    for (auto byte:{0,10,128,255}) {bad=valid;bad[17]=std::byte(byte);Require(!ParseArchive(bad),"InvalidNameBytes");}
    const TestRecord duplicate[]{{"same",{std::byte{1}}},{"same",{std::byte{2}}}};
    Require(!ParseArchive(MakeArchive(duplicate)),"DuplicateName");
    bad=valid;Put(bad,30,0,8);Require(!ParseArchive(bad),"ZeroRecordLength");
    bad=valid;Put(bad,45,2,8);Require(!ParseArchive(bad),"Overlap");
    bad=valid;Put(bad,45,4,8);Require(!ParseArchive(bad),"Gap");
    bad=valid;bad.push_back(std::byte{});Require(!ParseArchive(bad),"TrailingBytes");
    bad=valid;Put(bad,12,15,4);Require(!ParseArchive(bad),"PayloadOffsetBeforeDirectory");
    bad=valid;Put(bad,12,62,4);Require(!ParseArchive(bad),"DirectoryClosure");
    bad=valid;Put(bad,45,std::numeric_limits<std::uint64_t>::max(),8);Require(!ParseArchive(bad),"RecordRangeOverflow");
    bad=valid;Put(bad,53,std::numeric_limits<std::uint64_t>::max(),8);Require(!ParseArchive(bad),"RecordSizeOverflow");
    // Directory order is independent from physical payload ordering.
    bad=valid;Put(bad,22,2,8);Put(bad,45,0,8);Require(bool(ParseArchive(bad)),"UnsortedContiguousDirectory");
    std::mt19937 rng(12345);for (unsigned i=0;i<4000;++i) {
        bad=valid;bad[rng()%61]=std::byte(rng()&255);auto r=ParseArchive(bad);
        if (r) for (const auto& record:r->records) Require(record.offset<=5 && record.size<=5-record.offset,"mutation accepted only bounded records");
    }
    const auto file=std::filesystem::temp_directory_path()/"TRP AMD NR archive fixture.bin";
    Write(file,valid);auto owned=ReadArchive(file);Require(bool(owned),"owned file");
    auto view=RecordBytes(*owned,1);Require(bool(view) && view->size()==2 && (*view)[0]==std::byte{4},"record span");
    Require(!RecordBytes(*owned,2),"RecordIndexOutOfRange");
    ArchiveFile moved=std::move(*owned);Require((*view)[1]==std::byte{5} && moved.Index().records.size()==2,"move lifetime");
    Require(!RecordBytes(*owned,0),"moved-from record rejected");
    Require(!ReadArchive(file,ArchiveLimits{65,4096}),"file budget before allocation");
    std::filesystem::remove(file);Require(!ReadArchive(file),"missing file");
    static_assert(!std::is_copy_constructible_v<ArchiveFile>);
    std::puts("PASS: archive accounting, malformed inputs, mutation bounds, owned spans and file errors");
}
