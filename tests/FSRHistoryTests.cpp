#include "Upscaling/FSRHistoryPolicy.h"
#include "InteropTestRig.h"
#include <limits>
using namespace TheosRenderPipeline::Upscaling;
using InteropFixture::Require;
int main()
{
    FsrHistoryPolicy history;CameraMeasurements camera;
    camera.identity=1;camera.nearDistance=0.1f;camera.farDistance=100;camera.verticalFovRadians=1;
    camera.view={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};camera.projection=camera.view;
    Extent extent{960,540};
    auto first=history.Accept(1,camera,extent,false,false);Require(first.valid && first.reset,"first source resets");
    auto normal=history.Accept(2,camera,extent,false,false);Require(normal.valid && !normal.reset,"normal source does not repeat reset");
    Require(!history.Accept(2,camera,extent,false,false).valid,"duplicate ID rejected");
    Require(history.Accept(3,camera,extent,false,false).reset,"InvalidFrameResetsReentry");
    Require(!history.Accept(4,camera,extent,true,true).valid,"loading source deliberately skipped");
    Require(history.Accept(5,camera,extent,false,false).reset,"LoadingExitResetsOnce");
    Require(!history.Accept(6,camera,extent,false,false).reset,"loading reset consumed once");
    camera.identity=2;Require(history.Accept(7,camera,extent,false,false).reset,"camera owner change resets");
    camera.position[0]=100;Require(history.Accept(8,camera,extent,false,false).reset,"camera jump resets");
    history.Invalidate();Require(history.Accept(9,camera,extent,false,false).reset,"new game/save load invalidation");
    camera.position[0]=std::numeric_limits<float>::quiet_NaN();Require(!history.Accept(10,camera,extent,false,false).valid,"nonfinite camera rejected");
    camera.position[0]=100;Require(history.Accept(11,camera,extent,false,false).reset,"skipped invalid source resets reentry");
    Require(!history.Accept(12,camera,extent,false,false).reset,"paused simulation still accepts advancing source IDs");
    camera.depthInfinite=true;camera.farDistance=std::numeric_limits<float>::infinity();
    auto infinite=history.Accept(13,camera,extent,false,false);
    Require(infinite.valid && infinite.reset,"finite to infinite depth is accepted and resets once");
    auto continued=history.Accept(14,camera,extent,false,false);
    Require(continued.valid && !continued.reset,"stable infinite depth does not reset repeatedly");
    camera.farDistance=100;
    Require(!history.Accept(15,camera,extent,false,false).valid,"infinite convention rejects finite far distance");
    camera.depthInfinite=false;camera.farDistance=std::numeric_limits<float>::infinity();
    Require(!history.Accept(16,camera,extent,false,false).valid,"finite convention rejects infinity");
    camera.depthInfinite=true;camera.farDistance=-std::numeric_limits<float>::infinity();
    Require(!history.Accept(17,camera,extent,false,false).valid,"negative infinity is never a far plane");
    camera.depthInfinite=false;camera.farDistance=100;
    auto finite=history.Accept(18,camera,extent,false,false);
    Require(finite.valid && finite.reset,"return to finite depth resets history");
    std::puts("PASS: source identity, camera discontinuities, loading and invalid-frame reentry");
}
