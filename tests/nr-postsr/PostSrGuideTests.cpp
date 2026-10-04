#include "../nr-postfg/SyntheticScene.h"
#include <cstdio>
using namespace NrPostFgResearch;
namespace {
int assertions{},failed{};
void Check(bool value,const char* name){++assertions;std::printf("%s %s\n",value?"PASS":"FAIL",name);if(!value)++failed;}
ReferenceFrame AtExtent(Scene scene,unsigned width,unsigned height,double time,double history){
    const double sx=double(width)/8,sy=double(height)/4;
    scene.cameraVelocityX*=sx;scene.cameraVelocityY*=sy;
    for(auto& b:scene.boxes){b.left*=sx;b.width*=sx;b.velocityX*=sx;b.top*=sy;b.height*=sy;b.velocityY*=sy;}
    return Rasterize(scene,width,height,time,history);
}
// Research-only nearest-neighbour candidate; never a qualified shipping recipe.
ReferenceFrame Nearest(const ReferenceFrame& input,unsigned width,unsigned height){
    ReferenceFrame result{width,height,input.time,input.historyTime,{}};
    for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x){
        const auto ix=unsigned((uint64_t(2)*x+1)*input.width/(uint64_t(2)*width));
        const auto iy=unsigned((uint64_t(2)*y+1)*input.height/(uint64_t(2)*height));
        auto p=input.pixels.at(size_t(iy)*input.width+ix);
        p.currentToHistoryPixelsX*=float(width)/input.width;p.currentToHistoryPixelsY*=float(height)/input.height;
        result.pixels.push_back(p);
    }
    return result;
}
unsigned Differences(const ReferenceFrame& a,const ReferenceFrame& b){
    unsigned mismatches{};
    for(size_t i=0;i<a.pixels.size();++i){const auto& x=a.pixels[i];const auto& y=b.pixels.at(i);
        mismatches+=x.depthMetres!=y.depthMetres||x.currentToHistoryPixelsX!=y.currentToHistoryPixelsX||x.currentToHistoryPixelsY!=y.currentToHistoryPixelsY;}
    return mismatches;
}
}
int main(){
    Scene s;s.cameraVelocityX=2;s.boxes={{2,0,2,4,0,0,2,{1,0,0,1},1}};
    const auto native=AtExtent(s,8,4,1,0),nativeCopy=Nearest(native,8,4);
    Check(Differences(nativeCopy,native)==0,"NativeGuideCopyMatchesIndependentRealGeometry");
    const auto small=AtExtent(s,4,2,1,0),candidate=Nearest(small,8,4);
    Check(Differences(candidate,native)==0,"AlignedReducedSceneScalesMotionToDisplayPixels");
    Check(small.pixels.back().currentToHistoryPixelsX==1 && candidate.pixels.back().currentToHistoryPixelsX==2,"RenderPixelMotionCannotBeCopiedUnscaled");
    s.cameraVelocityX=0;s.boxes={{2,0,1,4,0,0,2,{1,0,0,1},1}};
    const auto thinNative=AtExtent(s,8,4,1,0),thinSmall=AtExtent(s,4,2,1,0);
    const auto thinMismatch=Differences(Nearest(thinSmall,8,4),thinNative);
    Check(thinMismatch==4,"ThinSurfaceLostAtRenderExtentCannotBeRecoveredByNearestUpsample");
    std::printf("OBSERVATION reducedThinSurfaceGuideMismatches=%u\n",thinMismatch);
    s.boxes={{1,0,2,4,0,0,2,{1,0,0,1},1}};
    const auto edgeMismatch=Differences(Nearest(AtExtent(s,4,2,1,0),8,4),AtExtent(s,8,4,1,0));
    Check(edgeMismatch>0,"UnalignedDepthEdgesDoNotQualifyNearestGuideRecipe");
    std::printf("OBSERVATION reducedUnalignedEdgeGuideMismatches=%u\n",edgeMismatch);
    std::printf("RESULT assertions=%d failed=%d\n",assertions,failed);return failed?1:0;
}
