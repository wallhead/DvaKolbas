#include "PairGuideReconstruction.h"
#include "SyntheticScene.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
using namespace NrPostFgResearch;
namespace {
int failed{}, assertions{};
void Check(bool ok, const char* name) {
    ++assertions;
    std::printf("%s %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok) ++failed;
}
PairGuideFrame Endpoint(const Scene& scene, uint64_t id, double time) {
    const auto reference = Rasterize(scene, 8, 4, time, time - 1);
    PairGuideFrame frame{1, id, 8, 4, time, time - 1, {}};
    for (const auto& p : reference.pixels)
        frame.pixels.push_back({p.depthMetres, p.currentToHistoryPixelsX, p.currentToHistoryPixelsY});
    return frame;
}
unsigned Mismatches(const ReconstructedGuides& guides, const ReferenceFrame& reference) {
    unsigned count{};
    for (size_t i = 0; i < guides.pixels.size(); ++i) {
        const auto& p = guides.pixels[i]; const auto& r = reference.pixels[i];
        if (guides.covered[i] && (p.depthMetres != r.depthMetres ||
            std::abs(p.currentToHistoryPixelsX-r.currentToHistoryPixelsX) > 1e-6f ||
            std::abs(p.currentToHistoryPixelsY-r.currentToHistoryPixelsY) > 1e-6f)) ++count;
    }
    return count;
}
template<class F> bool Rejects(F action) {
    try { action(); } catch (const std::invalid_argument&) { return true; }
    return false;
}
}
int main() {
    Scene scene;
    scene.boxes.push_back({0,1,2,2,4,0,2,{.8f,.2f,.1f,1},1});
    auto previous = Endpoint(scene,10,0), current = Endpoint(scene,11,1);
    GuideTarget target{1,10,11,.5,0};
    auto result = ReconstructPairGuides(previous,current,target);
    auto reference = Rasterize(scene,8,4,.5,0);
    Check(result.time == reference.time, "ReconstructionUsesIntermediateTimestamp");
    Check(std::ranges::all_of(result.covered, [](bool value){return value;}), "VisibleMovingBoxCoverage");
    Check(Mismatches(result,reference)==0, "MovingSurfaceAndDisocclusionMatchIndependentGeometry");
    ReconstructedGuides borrowed{8,4,1,0,current.pixels,std::vector<bool>(32,true)};
    Check(Mismatches(borrowed,reference)>0, "BorrowingCurrentRealGuidesIsIncorrect");
    target.historyTime=.25;
    result=ReconstructPairGuides(previous,current,target);
    Check(Mismatches(result,Rasterize(scene,8,4,.5,.25))==0, "MotionUsesOutputHistoryInterval");
    scene.boxes.push_back({3,1,1,2,0,0,1,{.1f,.9f,.1f,1},2});
    previous=Endpoint(scene,10,0);current=Endpoint(scene,11,1);target.historyTime=0;
    result=ReconstructPairGuides(previous,current,target);
    Check(Mismatches(result,Rasterize(scene,8,4,.5,0))==0, "NearestVisibleSurfaceWinsSplatCollision");
    scene.cameraVelocityX=4;
    previous=Endpoint(scene,10,0);current=Endpoint(scene,11,1);
    result=ReconstructPairGuides(previous,current,target);
    Check(Mismatches(result,Rasterize(scene,8,4,.5,0))==0, "CameraPanAndObjectMotionCancellation");
    Check(result.covered[8] && result.pixels[8].currentToHistoryPixelsX==0,
        "CameraMatchedObjectHasZeroMotion");
    Scene pan;pan.cameraVelocityY=6;
    previous=Endpoint(pan,10,0);current=Endpoint(pan,11,1);
    result=ReconstructPairGuides(previous,current,target);
    Check(Mismatches(result,Rasterize(pan,8,4,.5,0))==0, "VerticalPanGuidesMatchWhereCovered");
    Check(result.covered[0] && !result.covered[8] && !result.covered[16] && result.covered[31],
        "OffscreenEndpointGapsRemainExplicitlyUncovered");

    Scene subpixel;
    subpixel.boxes.push_back({0,1,1,2,1,0,2,{1,1,1,1},1});
    result=ReconstructPairGuides(Endpoint(subpixel,10,0),Endpoint(subpixel,11,1),target);
    const auto subpixelMismatches=Mismatches(result,Rasterize(subpixel,8,4,.5,0));
    Check(subpixelMismatches>0, "PointSplatsDoNotQualifySubpixelSurfaceBoundaries");
    std::printf("OBSERVATION subpixelCoveredGuideMismatches=%u\n",subpixelMismatches);

    // Two scenes have identical endpoint guide buffers; their intermediate
    // depth differs. An endpoint-only method cannot distinguish these scenes.
    Scene hidden;
    hidden.boxes={{0,0,2,4,0,0,1,{1,0,0,1},1}, {6,0,2,4,0,0,1,{0,1,0,1},2}};
    Scene revealed=hidden;
    revealed.boxes.push_back({0,1,2,2,6,0,2,{0,0,1,1},3});
    const auto emptyPrevious=Endpoint(hidden,10,0), emptyCurrent=Endpoint(hidden,11,1);
    previous=Endpoint(revealed,10,0);current=Endpoint(revealed,11,1);
    Check(previous.pixels==emptyPrevious.pixels && current.pixels==emptyCurrent.pixels,
        "HiddenSurfaceScenesHaveIdenticalEndpointGuides");
    const auto absentReference=Rasterize(hidden,8,4,.5,0);
    reference=Rasterize(revealed,8,4,.5,0);
    Check(absentReference.pixels[11].depthMetres==10 && reference.pixels[11].depthMetres==2,
        "IndependentIntermediateGeometryDistinguishesHiddenSurface");
    result=ReconstructPairGuides(previous,current,target);
    const auto hiddenMismatches=Mismatches(result,reference);
    Check(hiddenMismatches==4, "HiddenSurfaceCannotBeRecoveredFromEndpointGuides");
    Check(std::ranges::all_of(result.covered, [](bool value){return value;}) && hiddenMismatches>0,
        "FullSplatCoverageDoesNotQualifyRecipe");
    Check(result.pixels==ReconstructPairGuides(emptyPrevious,emptyCurrent,target).pixels,
        "SameEndpointInputsCannotSelectDifferentIntermediateSurfaces");
    std::printf("OBSERVATION hiddenSurfacePixels=4 coveredGuideMismatches=%u\n",hiddenMismatches);

    const auto validPrevious=previous, validCurrent=current;const auto validTarget=target;
    target.previousSourceId=9;
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectStaleSourcePair");
    target=validTarget;current.epoch=2;
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectMixedEpochs");
    current=validCurrent;current.width=4;
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectExtentMismatch");
    current=validCurrent;current.pixels.pop_back();
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectIncompleteGuides");
    current=validCurrent;target.fraction=std::numeric_limits<double>::quiet_NaN();
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectUnknownFraction");
    target=validTarget;target.historyTime=.75;
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectFutureOutputHistory");
    target=validTarget;current.historyTime=current.time;
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectZeroSourceHistoryInterval");
    current=validCurrent;current.time=previous.time;
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectUnorderedSourceTimes");
    current=validCurrent;previous.pixels[0].depthMetres=-1;
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectInvalidDepth");
    previous=validPrevious;current.pixels[0].currentToHistoryPixelsY=std::numeric_limits<float>::infinity();
    Check(Rejects([&]{ReconstructPairGuides(previous,current,target);}), "RejectNonfiniteMotion");
    std::printf("RESULT assertions=%d failed=%d\n",assertions,failed);
    return failed?1:0;
}
