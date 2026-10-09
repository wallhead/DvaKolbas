#include "Upscaling/XessFrameAdapter.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
using namespace TheosRenderPipeline::Upscaling;
static void Require(bool ok,const char* message) { if(!ok){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);} }
static bool Close(float a,float b) { return std::abs(a-b)<0.0000001f; }
static UpscaleFrame Frame()
{
    UpscaleFrame frame{};frame.backend=BackendKind::Xess;
    frame.render=frame.subrect={1600,900};frame.display={2560,1440};
    frame.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
    frame.sourceId=3;frame.sourceEpoch=7;frame.jitterX=-.25f;frame.jitterY=.125f;frame.colorIsLinear=true;
    frame.motionConvention={1600,900,true,false};return frame;
}
static XessInputPolicy Policy()
{
    XessInputPolicy policy{};policy.sourceEncoding=ColorEncoding::Gamma22;policy.motion={1600,900,true,false};
    policy.motionExtent=policy.depthExtent={1600,900};policy.motionGuide=XessMotionGuide::Undilated;policy.historySourceEpoch=7;
    return policy;
}
static void Invalid(Result<XessFrameParameters> result,const char* why)
{ Require(!result && result.error().kind==ErrorKind::InvalidInput,why); }
static xess_result_t Query(xess_context_handle_t context,const xess_2d_t* output,xess_quality_settings_t quality,xess_2d_t* optimal,xess_2d_t* minimum,xess_2d_t* maximum)
{
    Require(context==reinterpret_cast<xess_context_handle_t>(1) && output->x==2560 && output->y==1440 && quality==XESS_QUALITY_SETTING_QUALITY,"sizing query uses actual context/output/quality");
    *optimal={1506,848};*minimum={1506,848};*maximum={2560,1440};return XESS_RESULT_SUCCESS;
}
int main()
{
    auto frame=Frame();auto policy=Policy();
    policy.motionExtent.width=1599;Invalid(BuildXessFrameParameters(frame,policy),"RejectMismatchedGuides: motion size mismatch");
    policy=Policy();policy.depthExtent.height=899;Invalid(BuildXessFrameParameters(frame,policy),"RejectMismatchedGuides: depth size mismatch");
    policy=Policy();policy.sourceEncoding=ColorEncoding::Unknown;Invalid(BuildXessFrameParameters(frame,policy),"RejectUnknownEncoding");
    policy=Policy();policy.motionGuide=XessMotionGuide::Unknown;Invalid(BuildXessFrameParameters(frame,policy),"unknown guide provenance rejected");
    policy.motionGuide=XessMotionGuide::Dilated;Invalid(BuildXessFrameParameters(frame,policy),"dilated low-res guide rejected");
    policy=Policy();const auto valid=BuildXessFrameParameters(frame,policy);
    Require(valid && valid->input==Extent{1600,900} && valid->output==Extent{2560,1440} && Close(valid->motionScaleX,1600) && Close(valid->motionScaleY,900),"InputPixelMotionAtNonSquareExtent");
    Require(valid->jitterX==-.25f && valid->jitterY==.125f && !valid->resetHistory,"stored game sample jitter forwarded once, without second negation");
    for(const auto quality:{Quality::NativeAA,Quality::Quality,Quality::Balanced,Quality::Performance}) {
        const auto mapped=XessQuality(quality);
        const auto expected=quality==Quality::NativeAA?106:quality==Quality::Quality?103:quality==Quality::Balanced?102:101;
        Require(mapped && static_cast<int>(*mapped)==expected,"NativeUsesAa and named SDK qualities");
    }
    Require(!XessQuality(static_cast<Quality>(99)),"unknown quality rejected");
    policy.historySourceEpoch=6;Require(BuildXessFrameParameters(frame,policy)->resetHistory,"SourceEpochResetsHistory");
    policy.historySourceEpoch.reset();Require(BuildXessFrameParameters(frame,policy)->resetHistory,"first source epoch resets history");
    policy=Policy();frame.camera.reset=true;Require(BuildXessFrameParameters(frame,policy)->resetHistory,"camera cut resets history");
    frame=Frame();policy.depthInverted=true;Invalid(BuildXessFrameParameters(frame,policy),"depth convention mismatch rejected");
    frame.camera.depthInverted=true;frame.motionConvention.includesJitter=policy.motion.includesJitter=true;
    const auto flags=BuildXessFrameParameters(frame,policy);
    Require(flags && flags->flags==(XESS_INIT_FLAG_LDR_INPUT_COLOR|XESS_INIT_FLAG_INVERTED_DEPTH|XESS_INIT_FLAG_JITTERED_MV),"measured depth/jitter conventions set flags; no autoexposure or high-res motion");
    frame=Frame();policy=Policy();frame.colorIsLinear=false;Invalid(BuildXessFrameParameters(frame,policy),"undecoded source never reaches SDK");
    frame=Frame();frame.jitterX=std::numeric_limits<float>::quiet_NaN();Invalid(BuildXessFrameParameters(frame,policy),"non-finite sample rejected");
    frame=Frame();frame.jitterY=.51f;Invalid(BuildXessFrameParameters(frame,policy),"sample outside half-pixel range rejected");
    frame=Frame();policy.motion.scaleX=0;Invalid(BuildXessFrameParameters(frame,policy),"zero motion scale rejected");
    frame=Frame();policy=Policy();frame.motionConvention.scaleY=1440;Invalid(BuildXessFrameParameters(frame,policy),"frame does not match measured input-motion convention");
    const auto aio=XessJitterForGame(.25f,-.125f,{1600,900});
    Require(aio && Close(aio->sampleX,-.25f) && Close(aio->sampleY,.125f) && Close(aio->projectionX,-.0003125f) && Close(aio->projectionY,-.0002777778f),"AIO19 reference forwarding and normalized hook values");
    Require(Close(aio->projectionX,2*aio->sampleX/1600) && Close(aio->projectionY,-2*aio->sampleY/900),"ProjectionAndSampleJitterAgree with actual RaZkolbaS hook convention");
    const auto native=GenerateXessJitter(0,{1920,1080},{1920,1080});
    Require(native && native->phaseCount==8 && Close(native->generatedX,0) && Close(native->generatedY,-1.f/6.f),"Native Halton jitter sequence");
    const auto scaled=GenerateXessJitter(0,{1000,562},{2560,1440});
    Require(scaled && scaled->phaseCount==53,"sequence ceil(8*(2560/1000)^2), not an FSR constant");
    const auto repeated=GenerateXessJitter(53,{1000,562},{2560,1440});
    Require(repeated && repeated->generatedX==scaled->generatedX && repeated->generatedY==scaled->generatedY,"queried-scale period controls repeat");
    Require(!GenerateXessJitter(0,{0,1080},{1920,1080}),"zero render extent rejects jitter");
    XessFunctions api{};api.GetOptimalInputResolution=Query;
    const auto extent=QueryXessRenderExtent(api,reinterpret_cast<xess_context_handle_t>(1),Quality::Quality,{2560,1440});
    Require(extent && *extent==Extent{1506,848},"SDK sizing governs resources rather than FSR ratios");
    Require(!QueryXessRenderExtent(api,nullptr,Quality::Quality,{2560,1440}),"no sizing without live context");
    std::puts("PASS: XeSS frame/guide/jitter/quality/history/SDK-sizing contracts");
}
