#include "PostFgProbeData.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
namespace NrPostFgResearch {
namespace {
bool Sha256(const std::string& s) {
    return s.size() == 64 && std::ranges::all_of(s, [](unsigned char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    });
}
bool ValidExtent(Extent e) { return e.width && e.height; }
bool ValidPair(SourcePair p) { return p.previous && p.current > p.previous; }
bool Fraction(std::optional<double> f, ImageKind kind) {
    return f && std::isfinite(*f) && (kind == ImageKind::Real ? *f == 1 : *f > 0 && *f < 1);
}
struct Batch {
    SourcePair sources;
    uint32_t real{}, generated{};
    std::set<double> fractions;
};
}
ReportValidation ValidateReport(const ProbeReport& report) {
    ReportValidation result;
    auto issue = [&](std::string message) { result.issues.push_back(std::move(message)); };
    if (report.schema != 1) issue("unsupported schema");
    if (report.provider != Provider::Fsr && report.provider != Provider::DlssG) issue("unknown provider");
    if (report.mode != Mode::Trace && report.mode != Mode::Sentinel) issue("unknown probe mode");
    if (report.runtimes.empty()) issue("runtime identity is missing");
    std::set<std::string> modules;
    for (const auto& runtime : report.runtimes) {
        if (runtime.module.empty() || !modules.insert(runtime.module).second ||
            !Sha256(runtime.expectedSha256) || runtime.expectedSha256 != runtime.observedSha256 ||
            !runtime.loadedFileMatchesLease) issue("runtime identity is unverified: " + runtime.module);
    }
    if (!report.versionedBoundaryObserved || report.boundaryDescription.empty()) issue("versioned provider boundary missing");
    for (const auto& missing : report.unestablished) issue("unestablished: " + missing);
    if (report.outputs.empty()) issue("output records missing");
    if (!report.expectedGeneratedPerBatch) issue("generated coverage target missing");
    std::set<uint64_t> images;
    std::map<uint64_t, Batch> batches;
    for (const auto& output : report.outputs) {
        const std::string prefix = "image " + std::to_string(output.imageId) + ": ";
        if (!output.imageId || !images.insert(output.imageId).second) issue(prefix + "duplicate image or missing identity");
        if (!output.batchId || !ValidPair(output.sources)) issue(prefix + "invalid batch/source pair");
        auto [it, inserted] = batches.try_emplace(output.batchId, Batch{output.sources});
        auto& batch = it->second;
        if (!inserted && batch.sources != output.sources) issue(prefix + "mixed source pairs in batch");
        if (output.kind == ImageKind::Real) ++batch.real;
        else if (output.kind == ImageKind::Generated) ++batch.generated;
        else issue(prefix + "unknown image kind");
        if (!Fraction(output.fraction, output.kind)) issue(prefix + "interpolation time is unknown/invalid");
        else if (!batch.fractions.insert(*output.fraction).second) issue(prefix + "duplicate interpolation time in batch");
        if (!ValidExtent(output.colorExtent) || output.colorFormat.empty() || output.colorEncoding.empty() ||
            !output.colorIdentity || !output.commandListIdentity || !output.queueIdentity || !output.statesObserved)
            issue(prefix + "color/command ownership or state incomplete");
        if (!output.hudlessWorldOwned || !output.destinationOwned || !output.separateUiOwned)
            issue(prefix + "HUD-less world/destination/separate UI ownership missing");
        if (output.retirement != RetirementScope::OutputReaders || !output.fenceIdentity ||
            !output.fenceValue || !output.fenceCompleted) issue(prefix + "output retirement is unproven");
        if (!Sha256(output.worldCaptureSha256) || !Sha256(output.finalCaptureSha256)) issue(prefix + "pixel readbacks missing");
        if (!output.opaqueHudPassed || !output.halfAlphaHudPassed) issue(prefix + "HUD sentinel did not pass");
        if (report.mode == Mode::Sentinel && !output.distinctOutputSentinelPassed) issue(prefix + "distinct output sentinel missing");
        const auto& guides = output.guides;
        if (!ValidExtent(guides.extent) || guides.depthFormat.empty() || guides.motionFormat.empty() ||
            guides.depthConvention.empty() || guides.motionConvention.empty()) issue(prefix + "guide format/convention missing");
        if (guides.sources != output.sources) issue(prefix + "guide source pair mismatch");
        if (!Fraction(guides.fraction, output.kind) || !Fraction(output.fraction, output.kind) ||
            *guides.fraction != *output.fraction) issue(prefix + "guide time mismatch");
        if (output.kind == ImageKind::Generated) {
            if (guides.origin == GuideOrigin::RealSource) issue(prefix + "borrowed real guides are unqualified");
            else if (guides.origin != GuideOrigin::ProviderOutput && guides.origin != GuideOrigin::ReconstructedPair)
                issue(prefix + "generated guide provenance missing");
            if (guides.recipeId.empty() || !Sha256(guides.referenceCaptureSha256) ||
                guides.referenceOrigin != ReferenceOrigin::IndependentScene || !guides.referencePassed)
                issue(prefix + "independent reference did not qualify generated guides");
        } else if (guides.origin != GuideOrigin::RealSource && guides.origin != GuideOrigin::ProviderOutput)
            issue(prefix + "real guide provenance missing");
    }
    for (const auto& [id, batch] : batches) {
        if (batch.real != 1 || batch.generated != report.expectedGeneratedPerBatch)
            issue("batch " + std::to_string(id) + ": real/generated coverage incomplete");
    }
    return result;
}
}
