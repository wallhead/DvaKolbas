#pragma once
#include "RuntimeCatalog.h"
#include <Windows.h>
namespace TheosRenderPipeline::NeuralRendering {
// Keeps the verified file write/delete-denied until its eventual runtime owner
// safely unloads. Open alone does not execute or qualify an NR binary.
class RuntimeFileLease {
public:
    RuntimeFileLease() = default;
    ~RuntimeFileLease();
    RuntimeFileLease(const RuntimeFileLease&) = delete;
    RuntimeFileLease& operator=(const RuntimeFileLease&) = delete;
    RuntimeFileLease(RuntimeFileLease&&) noexcept;
    RuntimeFileLease& operator=(RuntimeFileLease&&) noexcept;
    static Result<RuntimeFileLease> Open(const std::filesystem::path&, const RuntimeProfile&);
    bool Matches(const std::filesystem::path& loadedPath) const noexcept;
    const std::filesystem::path& Path() const noexcept { return path_; }
    const std::string& Sha256() const noexcept { return sha256_; }
    uint64_t Bytes() const noexcept { return bytes_; }
    bool Valid() const noexcept { return file_ != INVALID_HANDLE_VALUE; }
private:
    HANDLE file_{INVALID_HANDLE_VALUE};
    std::filesystem::path path_;
    std::string sha256_;
    uint64_t bytes_{};
};
}
