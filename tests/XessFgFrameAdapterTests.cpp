#include "FrameGen/XessGenerationFrameAdapter.h"
#include "XessFgFrameFixture.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
static void Require(bool value,const char* reason) { if (!value) { std::fprintf(stderr,"FAIL: %s\n",reason);std::exit(1); } }
static void Invalid(const UpscaleFrame& frame,const char* reason)
{ const auto result=AdaptXessGenerationFrame(frame,17);Require(!result && result.error().kind==ErrorKind::InvalidInput,reason); }
int main()
{
    auto frame=XessFgFrame();
    auto result=AdaptXessGenerationFrame(frame,17);
    Require(result && result->render==Extent{1600,900} && result->display==Extent{2560,1440} && result->depth==frame.render && result->motion==frame.render,"non-square scaled valid regions retained");
    Require(result->sdkId==17 && result->sourceId==1 && result->sourceEpoch==7,"SDK/source identities preserved");
    Require(result->constants.viewMatrix[12]==3 && result->constants.projectionMatrix[11]==1,"unjittered row-major matrices preserved without transpose");
    Require(result->constants.motionVectorScaleX==1600 && result->constants.motionVectorScaleY==900,"UV displacement scales in guide pixels, never display pixels");
    frame.motionConvention={1,1,false,true,true};frame.camera.depthInverted=true;
    frame.jitterX=-.25f;frame.jitterY=.125f;
    result=AdaptXessGenerationFrame(frame,0);
    Require(result && result->sdkId==0 && result->constants.motionVectorScaleX==-1 && result->constants.motionVectorScaleY==-1,"NDC and reversed temporal direction declared, zero SDK ID legal");
    Require(result->initFlags==(XEFG_SWAPCHAIN_INIT_FLAG_USE_NDC_VELOCITY|XEFG_SWAPCHAIN_INIT_FLAG_JITTERED_MV|XEFG_SWAPCHAIN_INIT_FLAG_INVERTED_DEPTH),"immutable convention flags");
    Require(result->constants.jitterOffsetX==-.25f && result->constants.jitterOffsetY==.125f,"sample jitter forwarded once");
    frame=XessFgFrame();frame.motionExtent=frame.display;
    Invalid(frame,"depth and motion valid regions must agree");
    frame.depthExtent=frame.display;
    Invalid(frame,"high-resolution motion must declare dilation");
    frame.motionDilated=true;
    Require(AdaptXessGenerationFrame(frame,17)->initFlags==XEFG_SWAPCHAIN_INIT_FLAG_HIGH_RES_MV,"actual high-resolution motion flag");
    frame=XessFgFrame();frame.render=frame.subrect=frame.depthExtent=frame.motionExtent=frame.display;
    Require(bool(AdaptXessGenerationFrame(frame,17)),"Native input accepted");
    frame.motionConvention={1,1,true,false};
    Require(AdaptXessGenerationFrame(frame,17)->constants.motionVectorScaleX==1,"pixel displacement needs no normalized conversion");
    frame.deltaMilliseconds=0;Require(AdaptXessGenerationFrame(frame,17)->constants.frameRenderTime==0,"unavailable measured render time accepted");
    frame.reset=true;Require(AdaptXessGenerationFrame(frame,17)->constants.resetHistory==1,"reset forwarded");
    frame=XessFgFrame();frame.depthExtent={};Invalid(frame,"missing measured guide extent rejected");
    frame=XessFgFrame();frame.motionExtent={1599,900};Invalid(frame,"fallback guide scale forbidden");
    frame=XessFgFrame();frame.subrect.height=899;Invalid(frame,"invalid subrect rejected");
    frame=XessFgFrame();frame.render.width=0;Invalid(frame,"zero extent rejected");
    frame=XessFgFrame();frame.jitterY=.51f;Invalid(frame,"half-pixel bound enforced");
    frame=XessFgFrame();frame.motionConvention.scaleX=0;Invalid(frame,"zero motion scale rejected");
    frame=XessFgFrame();frame.camera.view[0]=std::numeric_limits<float>::quiet_NaN();Invalid(frame,"NaN camera rejected");
    frame=XessFgFrame();frame.camera.projection[8]=.0001f;Invalid(frame,"jittered/off-axis projection not qualified");
    frame=XessFgFrame();frame.camera.view.fill(0);Invalid(frame,"degenerate camera rejected");
    frame=XessFgFrame();frame.deltaMilliseconds=-1;Invalid(frame,"negative time rejected");
    frame=XessFgFrame();frame.deltaMilliseconds=std::numeric_limits<float>::infinity();Invalid(frame,"infinite time rejected");
    std::puts("PASS: Intel FG frame/motion/depth/matrix/identity contracts");
}
