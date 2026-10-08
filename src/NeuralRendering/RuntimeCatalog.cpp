#include "RuntimeCatalog.h"
#include <algorithm>
#include <array>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
// Fallback when NVAPI architecture discovery is unavailable. Exact desktop PCI IDs independently cross-checked against this host's signed
// driver INF; see research/nr/runtime-catalog/driver-id-evidence.json. No ranges,
// device-ID ranges or laptop-family assumptions. Successful NVAPI observations
// take precedence and must agree with any known PCI family.
constexpr std::array rtx20{0x1e04u,0x1e07u,0x1e81u,0x1e82u,0x1e84u,0x1e87u,
    0x1e89u,0x1ec2u,0x1ec7u,0x1f02u,0x1f03u,0x1f06u,0x1f07u,0x1f08u,0x1f42u,0x1f47u};
constexpr std::array rtx30{0x2203u,0x2204u,0x2206u,0x2207u,0x2208u,0x220au,0x2216u,
    0x2414u,0x2482u,0x2484u,0x2486u,0x2487u,0x2488u,0x2489u,0x24c7u,0x24c9u,
    0x2503u,0x2504u,0x2507u,0x2508u,0x2544u,0x2582u,0x2584u};
constexpr std::array rtx40{0x2684u,0x2685u,0x2689u,0x2702u,0x2704u,0x2705u,0x2709u,
    0x2782u,0x2783u,0x2786u,0x2788u,0x2803u,0x2805u,0x2808u,0x2882u};
constexpr std::array rtx50{0x2b85u,0x2b87u,0x2b8cu,0x2c02u,0x2c05u,0x2c09u,0x2d04u,
    0x2d05u,0x2d83u,0x2f04u,0x2f06u};
static_assert(std::ranges::is_sorted(rtx20) && std::ranges::is_sorted(rtx30) &&
    std::ranges::is_sorted(rtx40) && std::ranges::is_sorted(rtx50));
constexpr std::array profiles{
    // Separate hardware eligibility/receipts, one shared compatibility payload.
    RuntimeProfile{"rtx50","NR/rtx40/nvngx_dlssnr.dll",
        "e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a",165840496,
        GpuFamily::Rtx50,false,CompatibilityPolicy::CallerIdentityProbeRequired},
    RuntimeProfile{"rtx40","NR/rtx40/nvngx_dlssnr.dll",
        "e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a",165840496,
        GpuFamily::Rtx40,false,CompatibilityPolicy::CallerIdentityProbeRequired},
    RuntimeProfile{"rtx20-30","NR/rtx20-30/nvngx_dlssnr.dll",
        "6dac1b40f0c87af84a8177b18c741e84fb0c914f204c9d87d95916b665ba3af8",309671536,
        GpuFamily::Rtx20,true,CompatibilityPolicy::CallerIdentityProbeRequired}
};
bool Eligible(const RuntimeProfile& profile, GpuFamily family) {
    return profile.primaryFamily==family || (profile.includeRtx30 && family==GpuFamily::Rtx30);
}
std::unexpected<Error> InvalidPath() {
    return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR root/profile path is not controlled"});
}
}
std::span<const RuntimeProfile> RuntimeCatalog() noexcept { return profiles; }
GpuFamily ClassifyGpu(uint32_t vendor, uint32_t device, bool software) noexcept {
    if (software) return GpuFamily::Unknown;
    if (vendor==0x1002) return GpuFamily::AmdUnsupported;
    if (vendor!=0x10de) return GpuFamily::Unknown;
    if (std::binary_search(rtx20.begin(),rtx20.end(),device)) return GpuFamily::Rtx20;
    if (std::binary_search(rtx30.begin(),rtx30.end(),device)) return GpuFamily::Rtx30;
    if (std::binary_search(rtx40.begin(),rtx40.end(),device)) return GpuFamily::Rtx40;
    if (std::binary_search(rtx50.begin(),rtx50.end(),device)) return GpuFamily::Rtx50;
    return GpuFamily::Unknown;
}
GpuFamily ClassifyGpu(const AdapterIdentity& adapter) noexcept {
    const auto fallback=ClassifyGpu(adapter.vendorId,adapter.deviceId,adapter.software);
    if(adapter.software || adapter.vendorId!=0x10de) return fallback;
    const auto& evidence=adapter.architecture;
    if(!evidence.queried) return fallback;
    if(!adapter.luid.Valid() || evidence.luid!=adapter.luid || !evidence.rtxProduct) return GpuFamily::Unknown;
    GpuFamily family{GpuFamily::Unknown};
    switch(evidence.id) {
    case 0x160: family=GpuFamily::Rtx20; break; // Turing (RTX only; excludes GTX 16)
    case 0x170: family=GpuFamily::Rtx30; break; // Ampere
    case 0x190: family=GpuFamily::Rtx40; break; // Ada
    case 0x1b0: family=GpuFamily::Rtx50; break; // Blackwell
    }
    if(fallback!=GpuFamily::Unknown && fallback!=family) return GpuFamily::Unknown;
    return family;
}
Selection SelectRuntime(const AdapterIdentity& adapter, std::string_view requested,
    std::span<const ArtifactIdentity> artifacts) {
    const auto family=ClassifyGpu(adapter);
    if (family==GpuFamily::AmdUnsupported || requested=="amd-unsupported")
        return {nullptr,"AMD NR is unsupported"};
    if (family==GpuFamily::Unknown) return {nullptr,"NR renderer GPU ID is unqualified"};
    if (!adapter.luid.Valid()) return {nullptr,"Actual render adapter LUID is missing"};
    const RuntimeProfile* profile{};
    if (requested=="Auto") {
        for (const auto& p: profiles) if (Eligible(p,family)) { profile=&p; break; }
    } else {
        for (const auto& p: profiles) if (p.id==requested) { profile=&p; break; }
    }
    if (!profile) return {nullptr,"Unknown NR runtime profile"};
    if (!Eligible(*profile,family)) return {nullptr,"NR profile does not match the renderer GPU family"};
    const ArtifactIdentity* artifact{};
    for (const auto& a: artifacts) if (a.profileId==profile->id) {
        if (artifact) return {nullptr,"Duplicate NR profile artifact identity"};
        artifact=&a;
    }
    if (!artifact || !artifact->present) return {nullptr,"Selected NR profile file is missing"};
    if (!artifact->heldFileVerified) return {nullptr,"Selected NR file has no verified held-file identity"};
    return {profile,"Exact eligible NR profile selected; GPU output qualification pending",Qualification::HardwareNotRun};
}
Result<void> CheckAdapterMatch(const AdapterIdentity& renderer, const AdapterIdentity& nr) {
    if (renderer.software || nr.software || !renderer.luid.Valid() || !nr.luid.Valid() ||
        renderer.luid!=nr.luid || renderer.vendorId!=nr.vendorId || renderer.deviceId!=nr.deviceId ||
        renderer.subsystemId!=nr.subsystemId)
        return std::unexpected(Error{ErrorKind::IdentityMismatch,0,"NR device does not belong to the actual renderer adapter"});
    return {};
}
Result<std::filesystem::path> RuntimePath(const std::filesystem::path& root, const RuntimeProfile& profile) {
    if (root.empty() || !root.is_absolute() || profile.relativePath.empty()) return InvalidPath();
    const std::filesystem::path relative{profile.relativePath};
    if (relative.is_absolute() || relative.has_root_name() || relative.has_root_directory() ||
        relative.filename()!=L"nvngx_dlssnr.dll" || profile.relativePath.find(':')!=std::string_view::npos)
        return InvalidPath();
    for (const auto& component: relative) if (component==L".." || component==L"." || component.empty()) return InvalidPath();
    return (root/relative).lexically_normal();
}
}
