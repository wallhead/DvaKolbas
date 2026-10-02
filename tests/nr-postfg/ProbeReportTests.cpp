#include "PostFgProbeData.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits>
using namespace NrPostFgResearch;
namespace {
int failed{};
void Check(bool condition, const char* test) {
    std::printf("%s %s\n", condition ? "PASS" : "FAIL", test);
    if (!condition) ++failed;
}
// Synthetic valid *schema fixture*, never recorded as a measured GPU receipt.
ProbeReport Fixture() {
    ProbeReport r;
    r.mode = Mode::Sentinel;
    const std::string hash(64, 'a');
    r.runtimes.push_back({"fixture.dll", hash, hash, true});
    r.versionedBoundaryObserved = true;
    r.boundaryDescription = "fixture typed present callback";
    OutputEvidence real;
    real.batchId = 7; real.imageId = 20; real.sources = {6,7};
    real.kind = ImageKind::Real; real.fraction = 1;
    real.colorExtent = {64,32}; real.colorFormat = "RGBA16F";
    real.colorEncoding = "linear"; real.colorIdentity = 101;
    real.commandListIdentity = 201; real.queueIdentity = 301;
    real.hudlessWorldOwned = real.destinationOwned = real.separateUiOwned = real.statesObserved = true;
    real.retirement = RetirementScope::OutputReaders;
    real.fenceIdentity = 401; real.fenceValue = 42; real.fenceCompleted = true;
    real.worldCaptureSha256 = real.finalCaptureSha256 = hash;
    real.distinctOutputSentinelPassed = real.opaqueHudPassed = real.halfAlphaHudPassed = true;
    real.guides.origin = GuideOrigin::RealSource;
    real.guides.sources = real.sources; real.guides.fraction = 1;
    real.guides.extent = real.colorExtent;
    real.guides.depthFormat = "R32F"; real.guides.motionFormat = "RG16F";
    real.guides.depthConvention = "linear metres"; real.guides.motionConvention = "previous to current pixels";
    OutputEvidence generated = real;
    generated.imageId = 21; generated.kind = ImageKind::Generated;
    generated.fraction = generated.guides.fraction = 0.5;
    generated.guides.origin = GuideOrigin::ReconstructedPair;
    generated.guides.recipeId = "fixture-intermediate-pair-v1";
    generated.guides.referenceCaptureSha256 = hash;
    generated.guides.referenceOrigin = ReferenceOrigin::IndependentScene;
    generated.guides.referencePassed = true;
    r.outputs = {generated,real};
    return r;
}
bool HasIssue(const ProbeReport& r, const char* fragment) {
    const auto v = ValidateReport(r);
    return !v.Complete() && std::ranges::any_of(v.issues, [fragment](const auto& s) {
        return s.find(fragment) != std::string::npos;
    });
}
}
int main() {
    Check(ValidateReport(Fixture()).Complete(), "CompleteSchemaFixtureAccepted_NotHardwareEvidence");
    auto r = Fixture(); r.outputs.erase(r.outputs.begin());
    Check(HasIssue(r,"generated coverage"), "NoGeneratedRecordsIsIncomplete");
    r = Fixture(); r.outputs[0].guides.origin = GuideOrigin::RealSource;
    Check(HasIssue(r,"borrowed real guides"), "RealGuidesBorrowedForSyntheticImageAreUnqualified");
    r = Fixture(); r.outputs[0].retirement = RetirementScope::InputOnly;
    Check(HasIssue(r,"output retirement"), "InputCompletionIsNotOutputRetirement");
    r = Fixture(); r.outputs[0].hudlessWorldOwned = false;
    Check(HasIssue(r,"HUD-less"), "ColorPointerWithoutHudlessOwnershipFails");
    r = Fixture(); r.runtimes[0].observedSha256 = std::string(64,'b');
    Check(HasIssue(r,"runtime identity"), "UnmatchedRuntimeRejectsHook");
    r = Fixture(); r.outputs[0].guides.referenceOrigin = ReferenceOrigin::ReconstructionKernel;
    Check(HasIssue(r,"independent reference"), "SyntheticGroundTruthNotDerivedFromGuideKernel");
    r = Fixture(); r.outputs[0].fraction.reset();
    Check(HasIssue(r,"interpolation time"), "UnknownGeneratedTimeIsIncomplete");
    r = Fixture(); r.outputs[0].fraction = std::numeric_limits<double>::quiet_NaN();
    Check(HasIssue(r,"interpolation time"), "NanGeneratedTimeIsIncomplete");
    r = Fixture(); r.outputs[0].guides.fraction = 1;
    Check(HasIssue(r,"guide time"), "WrongGuideTimestampRejectsGeneratedImage");
    r = Fixture(); r.outputs[0].guides.sources = {5,6};
    Check(HasIssue(r,"guide source pair"), "StaleSourcePairRejectsGeneratedImage");
    r = Fixture(); r.outputs[0].fenceCompleted = false;
    Check(HasIssue(r,"output retirement"), "RecordedCommandListDoesNotProveCompletion");
    r = Fixture(); r.outputs[0].halfAlphaHudPassed = false;
    Check(HasIssue(r,"HUD sentinel"), "OpaqueHudAloneDoesNotProveSingleComposition");
    r = Fixture(); r.outputs.push_back(r.outputs[0]);
    Check(HasIssue(r,"duplicate image"), "DuplicateOutputCannotInflateCoverage");
    r = Fixture(); r.expectedGeneratedPerBatch = 3;
    Check(HasIssue(r,"generated coverage"), "MissingMfgOutputsRemainIncomplete");
    r = Fixture(); r.unestablished.push_back("provider queue unknown");
    Check(HasIssue(r,"provider queue unknown"), "MissingContractsStayVisible");
    r = Fixture(); r.schema = 2;
    Check(HasIssue(r,"schema"), "UnknownSchemaRejected");
    r = Fixture(); r.runtimes.clear();
    Check(HasIssue(r,"runtime identity"), "NoModuleIdentityIsIncomplete");
    r = Fixture(); r.outputs[0].guides.referencePassed = false;
    Check(HasIssue(r,"independent reference"), "FailedOcclusionReferenceRejectsRecipe");
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
