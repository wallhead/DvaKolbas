#include "SyntheticScene.h"
#include <cstdio>
using namespace NrPostFgResearch;
int main(){
    int failed{};const auto check=[&](bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);if(!ok)++failed;};
    Scene scene;
    scene.boxes.push_back({1,1,2,2,2,0,2,{.8f,.2f,.1f,1},1});
    auto real=Rasterize(scene,8,4,0,0);auto half=Rasterize(scene,8,4,.5,0);auto next=Rasterize(scene,8,4,1,.5);
    const auto pixel=[](const ReferenceFrame& f,unsigned x,unsigned y)->const auto&{return f.pixels.at(size_t(y)*f.width+x);};
    check(pixel(real,1,1).objectId==1 && pixel(half,1,1).objectId==0,"IntermediateGeometryDisoccludesOldForegroundPixel");
    check(pixel(half,2,1).objectId==1 && pixel(half,2,1).depthMetres==2,"IntermediateGeometryRasterizedAtHalfTime");
    check(pixel(half,2,1).currentToHistoryPixelsX==-1 && pixel(next,3,1).currentToHistoryPixelsX==-1,"HistoryMotionUsesActualImageInterval");
    check(pixel(half,1,1).depthMetres==10 && pixel(half,1,1).color==scene.background,"DisoccludedBackgroundComesFromGeometry");
    scene.boxes.push_back({3,1,1,2,0,0,1,{.1f,.9f,.1f,1},2});
    half=Rasterize(scene,8,4,.5,0);
    check(pixel(half,3,1).objectId==2 && pixel(half,3,1).depthMetres==1 && pixel(half,3,1).currentToHistoryPixelsX==0,"NearestSurfaceResolvesOcclusionIndependently");
    scene.cameraVelocityX=2;
    half=Rasterize(scene,8,4,.5,0);
    check(pixel(half,1,1).objectId==1 && pixel(half,1,1).currentToHistoryPixelsX==0,"ObjectAndCameraMotionCanCancel");
    check(pixel(half,7,3).objectId==0 && pixel(half,7,3).currentToHistoryPixelsX==1,"CameraMotionAffectsBackgroundGuides");
    bool rejected=false;try{(void)Rasterize(scene,0,4,.5,0);}catch(const std::invalid_argument&){rejected=true;}
    check(rejected,"InvalidReferenceExtentRejected");
    return failed?1:0;
}
