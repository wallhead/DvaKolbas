#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// Research-only evidence validation. A complete report is NOT a hardware
// qualification: its assertions must still be backed by the actual captures.
namespace NrPostFgResearch {
enum class Provider { Fsr, DlssG };
enum class Mode { Trace, Sentinel };
enum class ImageKind { Real, Generated };
enum class GuideOrigin { Missing, RealSource, ProviderOutput, ReconstructedPair };
enum class RetirementScope { Missing, InputOnly, OutputReaders };
enum class ReferenceOrigin { Missing, IndependentScene, ReconstructionKernel };
struct Extent {
    uint32_t width{}, height{};
    bool operator==(const Extent&) const = default;
};
struct RuntimeIdentity {
    std::string module, expectedSha256, observedSha256;
    bool loadedFileMatchesLease{};
};
struct SourcePair {
    uint64_t previous{}, current{};
    bool operator==(const SourcePair&) const = default;
};
struct GuideEvidence {
    GuideOrigin origin{GuideOrigin::Missing};
    SourcePair sources;
    std::optional<double> fraction;
    Extent extent;
    std::string depthFormat, motionFormat, depthConvention, motionConvention;
    std::string recipeId, referenceCaptureSha256;
    ReferenceOrigin referenceOrigin{ReferenceOrigin::Missing};
    bool referencePassed{};
};
struct OutputEvidence {
    uint64_t batchId{}, imageId{};
    SourcePair sources;
    ImageKind kind{ImageKind::Real};
    std::optional<double> fraction;
    Extent colorExtent;
    std::string colorFormat, colorEncoding;
    uint64_t colorIdentity{}, commandListIdentity{}, queueIdentity{};
    bool hudlessWorldOwned{}, destinationOwned{}, separateUiOwned{}, statesObserved{};
    RetirementScope retirement{RetirementScope::Missing};
    uint64_t fenceIdentity{}, fenceValue{};
    bool fenceCompleted{};
    std::string worldCaptureSha256, finalCaptureSha256;
    bool distinctOutputSentinelPassed{}, opaqueHudPassed{}, halfAlphaHudPassed{};
    GuideEvidence guides;
};
struct ProbeOptions {
    Provider provider{Provider::Fsr};
    Mode mode{Mode::Trace};
    std::string runtimeDirectory, outputPath;
    uint32_t adapterLuidLow{};
    int32_t adapterLuidHigh{};
    uint32_t sourceFrames{240};
};
struct ProbeReport {
    uint32_t schema{1};
    Provider provider{Provider::Fsr};
    Mode mode{Mode::Trace};
    std::vector<RuntimeIdentity> runtimes;
    bool versionedBoundaryObserved{};
    std::string boundaryDescription;
    uint32_t expectedGeneratedPerBatch{1};
    std::vector<OutputEvidence> outputs;
    std::vector<std::string> unestablished;
};
struct ReportValidation {
    std::vector<std::string> issues;
    bool Complete() const noexcept { return issues.empty(); }
};
ReportValidation ValidateReport(const ProbeReport& report);
}
