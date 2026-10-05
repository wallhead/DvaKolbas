#include "WeightArchive.h"
#include <algorithm>
#include <fstream>
#include <limits>
#include <new>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace TheosRenderPipeline::NeuralRendering::Amd {
namespace {
auto Fail(ArchiveErrorCode code, std::uint64_t pos, const char* message) {
    return std::unexpected(ArchiveError{code,pos,message});
}
std::uint64_t Little(std::span<const std::byte> bytes,std::size_t p,unsigned n) {
    std::uint64_t value{};
    for (unsigned i=0;i<n;++i) value|=std::uint64_t(std::to_integer<unsigned>(bytes[p+i]))<<(8*i);
    return value;
}
}
std::expected<ArchiveIndex,ArchiveError> ParseArchive(std::span<const std::byte> bytes,ArchiveLimits limits) {
    try {
        if (bytes.size()>limits.maxBytes) return Fail(ArchiveErrorCode::Budget,0,"archive exceeds byte budget");
        constexpr char magic[]="DLSSNRW1";
        if (bytes.size()<16) return Fail(ArchiveErrorCode::Header,0,"truncated archive header");
        for (unsigned i=0;i<8;++i) if (bytes[i]!=std::byte(magic[i])) return Fail(ArchiveErrorCode::Header,i,"unknown archive magic");
        const auto count=Little(bytes,8,4),base=Little(bytes,12,4);
        if (!count || count>limits.maxRecords) return Fail(ArchiveErrorCode::Budget,8,"invalid record count");
        if (base<16 || base>bytes.size() || count>(base-16)/18) return Fail(ArchiveErrorCode::Directory,12,"invalid directory boundary");
        ArchiveIndex index{static_cast<std::uint32_t>(base),bytes.size(),{}};
        index.records.reserve(static_cast<std::size_t>(count));
        std::unordered_set<std::string> names;
        std::vector<std::pair<std::uint64_t,std::uint64_t>> ranges;
        ranges.reserve(static_cast<std::size_t>(count));
        std::size_t p=16;const auto payloadBytes=bytes.size()-base;
        for (std::uint64_t i=0;i<count;++i) {
            if (p>=base) return Fail(ArchiveErrorCode::Directory,p,"truncated record");
            const auto length=std::to_integer<unsigned>(bytes[p++]);
            if (!length) return Fail(ArchiveErrorCode::Name,p-1,"empty record name");
            if (length>base-p || base-p-length<16) return Fail(ArchiveErrorCode::Directory,p,"truncated record fields");
            std::string name;name.reserve(length);
            for (unsigned j=0;j<length;++j) {
                const auto c=std::to_integer<unsigned>(bytes[p+j]);
                if (c<32 || c>126) return Fail(ArchiveErrorCode::Name,p+j,"record name must be printable ASCII");
                name.push_back(static_cast<char>(c));
            }
            p+=length;
            if (!names.insert(name).second) return Fail(ArchiveErrorCode::Name,p-length,"duplicate record name");
            const auto offset=Little(bytes,p,8),size=Little(bytes,p+8,8);p+=16;
            if (!size || offset>payloadBytes || size>payloadBytes-offset) return Fail(ArchiveErrorCode::Range,p-16,"record outside payload");
            ranges.emplace_back(offset,size);
            index.records.push_back({std::move(name),offset,size});
        }
        if (p!=base) return Fail(ArchiveErrorCode::Directory,p,"directory does not close at payload start");
        std::sort(ranges.begin(),ranges.end());std::uint64_t cursor{};
        for (const auto& [offset,size]:ranges) {
            if (offset!=cursor) return Fail(ArchiveErrorCode::Coverage,base+offset,"payload gap or overlap");
            cursor+=size; // Each range was already bounded by payloadBytes.
        }
        if (cursor!=payloadBytes) return Fail(ArchiveErrorCode::Coverage,base+cursor,"trailing payload bytes");
        return index;
    } catch (const std::bad_alloc&) { return Fail(ArchiveErrorCode::Memory,0,"archive allocation failed"); }
      catch (const std::length_error&) { return Fail(ArchiveErrorCode::Memory,0,"archive allocation size unsupported"); }
}
ArchiveFile::ArchiveFile(std::vector<std::byte>&& bytes,ArchiveIndex&& index) noexcept : bytes_(std::move(bytes)),index_(std::move(index)) {}
ArchiveFile::ArchiveFile(ArchiveFile&& other) noexcept : bytes_(std::move(other.bytes_)),index_(std::move(other.index_)) { other.index_={}; }
std::expected<ArchiveFile,ArchiveError> ReadArchive(const std::filesystem::path& path,ArchiveLimits limits) {
    try {
        std::ifstream stream(path,std::ios::binary|std::ios::ate);
        if (!stream) return Fail(ArchiveErrorCode::File,0,"cannot open archive");
        const auto position=stream.tellg();
        if (position<0) return Fail(ArchiveErrorCode::File,0,"cannot measure archive");
        const auto size=static_cast<std::uint64_t>(position);
        if (size>limits.maxBytes || size>std::numeric_limits<std::size_t>::max() || size>static_cast<std::uint64_t>(std::numeric_limits<std::streamsize>::max()))
            return Fail(ArchiveErrorCode::Budget,0,"archive exceeds file budget");
        std::vector<std::byte> bytes(static_cast<std::size_t>(size));
        stream.seekg(0);
        if (size) stream.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(size));
        if (!stream) return Fail(ArchiveErrorCode::File,0,"archive read failed or file changed size");
        const auto next=stream.peek();
        if (next!=std::char_traits<char>::eof() || stream.bad()) return Fail(ArchiveErrorCode::File,0,"archive changed size or read failed");
        auto index=ParseArchive(bytes,limits);
        if (!index) return std::unexpected(index.error());
        return ArchiveFile(std::move(bytes),std::move(*index));
    } catch (const std::bad_alloc&) { return Fail(ArchiveErrorCode::Memory,0,"archive file allocation failed"); }
      catch (const std::length_error&) { return Fail(ArchiveErrorCode::Memory,0,"archive file too large"); }
      catch (const std::exception&) { return Fail(ArchiveErrorCode::File,0,"archive file operation failed"); }
}
std::expected<std::span<const std::byte>,ArchiveError> RecordBytes(const ArchiveFile& file,std::size_t recordIndex) {
    const auto& index=file.Index();const auto bytes=file.Bytes();
    if (recordIndex>=index.records.size()) return Fail(ArchiveErrorCode::RecordIndex,recordIndex,"record index outside archive");
    const auto& record=index.records[recordIndex];
    if (index.payloadStart>bytes.size() || record.offset>bytes.size()-index.payloadStart ||
        record.size>bytes.size()-index.payloadStart-record.offset) return Fail(ArchiveErrorCode::Range,recordIndex,"record owner unavailable");
    return bytes.subspan(index.payloadStart+static_cast<std::size_t>(record.offset),static_cast<std::size_t>(record.size));
}
}
