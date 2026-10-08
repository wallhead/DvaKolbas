#pragma once
#include "Error.h"
#include <filesystem>
#include <span>
#include <string_view>
namespace TheosRenderPipeline::NeuralRendering {
enum class GpuFamily { Unknown, Rtx20, Rtx30, Rtx40, Rtx50, AmdUnsupported };
enum class CompatibilityPolicy { SignedDirect, CallerIdentityProbeRequired };
// Catalog eligibility and exact bytes do not establish evaluated GPU output.
enum class Qualification { Unavailable, HardwareNotRun };
struct AdapterLuid {
    uint32_t low{}; int32_t high{};
    bool operator==(const AdapterLuid&) const = default;
    bool Valid() const noexcept { return low || high; }
};
struct GpuArchitecture {
    AdapterLuid luid;
    uint32_t id{};
    bool rtxProduct{}, queried{};
    // NR needs one physical GPU. Renderer routing may use independent DXGI
    // evidence when a same-LUID logical mapping is linked or ambiguous.
    bool mappingAmbiguous{};
};
struct AdapterIdentity {
    uint32_t vendorId{}, deviceId{}, subsystemId{};
    AdapterLuid luid;
    bool software{};
    GpuArchitecture architecture;
};
struct RuntimeProfile {
    std::string_view id, relativePath, sha256;
    uint64_t bytes{};
    GpuFamily primaryFamily{GpuFamily::Unknown};
    bool includeRtx30{};
    CompatibilityPolicy compatibility{CompatibilityPolicy::SignedDirect};
};
struct ArtifactIdentity { std::string_view profileId; bool present{}, heldFileVerified{}; };
struct Selection {
    const RuntimeProfile* profile{};
    std::string reason;
    Qualification qualification{Qualification::Unavailable};
};
std::span<const RuntimeProfile> RuntimeCatalog() noexcept;
GpuFamily ClassifyGpu(uint32_t vendorId, uint32_t deviceId, bool software) noexcept;
GpuFamily ClassifyGpu(const AdapterIdentity&) noexcept;
void DiscoverGpuArchitecture(AdapterIdentity&);
Selection SelectRuntime(const AdapterIdentity&, std::string_view requested, std::span<const ArtifactIdentity>);
Result<void> CheckAdapterMatch(const AdapterIdentity& renderer, const AdapterIdentity& nr);
Result<std::filesystem::path> RuntimePath(const std::filesystem::path& absoluteRoot, const RuntimeProfile&);
}
