#pragma once
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace TheosRenderPipeline::NeuralRendering::Amd {
struct ArchiveRecord { std::string name; std::uint64_t offset{}, size{}; };
struct ArchiveIndex {
    std::uint32_t payloadStart{};
    std::uint64_t fileSize{};
    std::vector<ArchiveRecord> records;
};
struct ArchiveLimits { std::uint64_t maxBytes{268435456}; std::uint32_t maxRecords{4096}; };
enum class ArchiveErrorCode { Header, Budget, Directory, Name, Range, Coverage, File, Memory, RecordIndex };
struct ArchiveError { ArchiveErrorCode code; std::uint64_t offset{}; const char* message; };
std::expected<ArchiveIndex, ArchiveError> ParseArchive(std::span<const std::byte>, ArchiveLimits = {});

// Immutable snapshot. Views remain valid while this owner (or its move destination) lives.
class ArchiveFile {
public:
    ArchiveFile(ArchiveFile&&) noexcept;
    ArchiveFile(const ArchiveFile&) = delete;
    ArchiveFile& operator=(const ArchiveFile&) = delete;
    ArchiveFile& operator=(ArchiveFile&&) = delete;
    std::span<const std::byte> Bytes() const & noexcept { return bytes_; }
    std::span<const std::byte> Bytes() const && = delete;
    const ArchiveIndex& Index() const & noexcept { return index_; }
    const ArchiveIndex& Index() const && = delete;
private:
    ArchiveFile(std::vector<std::byte>&&, ArchiveIndex&&) noexcept;
    std::vector<std::byte> bytes_;
    ArchiveIndex index_;
    friend std::expected<ArchiveFile, ArchiveError> ReadArchive(const std::filesystem::path&, ArchiveLimits);
};
std::expected<ArchiveFile, ArchiveError> ReadArchive(const std::filesystem::path&, ArchiveLimits = {});
std::expected<std::span<const std::byte>, ArchiveError> RecordBytes(const ArchiveFile&, std::size_t);
std::expected<std::span<const std::byte>, ArchiveError> RecordBytes(const ArchiveFile&&, std::size_t) = delete;
}
