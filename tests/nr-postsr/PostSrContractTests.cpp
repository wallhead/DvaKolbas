#include "NeuralRendering/PostSrContract.h"
#include <cstdio>
#include <limits>
using namespace TheosRenderPipeline;
using namespace NeuralRendering;
namespace {
int failed{}, assertions{};
void Check(bool value,const char* name){++assertions;std::printf("%s %s\n",value?"PASS":"FAIL",name);if(!value)++failed;}
PostSrSourceContract Native() {
    PostSrSourceContract c;
    c.backend=Upscaling::BackendKind::Fsr;c.outcome=Upscaling::UpscaleOutcome::Temporal;
    c.epoch=c.guideEpoch=3;c.sourceId=c.guideSourceId=20;c.previousSourceId=19;
    c.sourceTime=c.guideTime=1;c.render=c.display=c.color=c.guides={640,360};
    c.colorDomain=ColorDomain::SdrBytes;c.encoding=Upscaling::ColorEncoding::Gamma22;
    c.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;c.depthFormat=DXGI_FORMAT_R32_FLOAT;c.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
    c.guideOrigin=GuideOrigin::RealSource;c.motion={640,360,true,false};return c;
}
}
int main(){
    auto c=Native();auto result=ValidatePostSrSourceContract(c);
    Check(result && result->extent==c.display && result->motionScaleX==640 && result->motionScaleY==360,"NativeRealSourceRetainsExplicitMotionUnits");
    c.backend=Upscaling::BackendKind::Dlss;Check(bool(ValidatePostSrSourceContract(c)),"DlssRealSourceAcceptedWithoutFgRequirement");
    c.backend=Upscaling::BackendKind::Dlaa;Check(bool(ValidatePostSrSourceContract(c)),"DlaaRealSourceAcceptedWithoutFgRequirement");
    c=Native();c.encoding=Upscaling::ColorEncoding::SRGB;Check(bool(ValidatePostSrSourceContract(c)),"ExplicitSrgbBytesAccepted");
    c=Native();c.colorDomain=ColorDomain::Linear;c.encoding=Upscaling::ColorEncoding::Linear;c.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;
    Check(bool(ValidatePostSrSourceContract(c)),"ExplicitLinearFormatAccepted_NotVisualQualification");
    c=Native();c.sourceId=0;Check(!ValidatePostSrSourceContract(c),"RejectMissingSourceIdentity");
    c=Native();c.previousSourceId=c.sourceId;Check(!ValidatePostSrSourceContract(c),"RejectUnorderedSourceIdentity");
    c=Native();c.guideSourceId--;Check(!ValidatePostSrSourceContract(c),"RejectStaleRealGuideSource");
    c=Native();c.guideEpoch++;Check(!ValidatePostSrSourceContract(c),"RejectStaleRealGuideEpoch");
    c=Native();c.epoch=c.guideEpoch=0;Check(!ValidatePostSrSourceContract(c),"RejectMissingEpoch");
    c=Native();c.kind=ImageKind::Generated;Check(!ValidatePostSrSourceContract(c),"RejectGeneratedImageInRealSourceStage");
    c=Native();c.guideOrigin=GuideOrigin::ReconstructedPair;Check(!ValidatePostSrSourceContract(c),"RejectGeneratedRecipeForRealSource");
    c=Native();c.guideTime+=.001;Check(!ValidatePostSrSourceContract(c),"RejectWrongTimeRealGuides");
    c=Native();c.sourceTime=c.guideTime=std::numeric_limits<double>::quiet_NaN();Check(!ValidatePostSrSourceContract(c),"RejectUnknownSourceTime");
    c=Native();c.outcome=Upscaling::UpscaleOutcome::SpatialRecovery;Check(!ValidatePostSrSourceContract(c),"SpatialRecoveryNotTemporalNrSource");
    c=Native();c.backend=Upscaling::BackendKind::External;Check(!ValidatePostSrSourceContract(c),"RejectExternalUnownedUpscaler");
    c=Native();c.guides.width=320;Check(!ValidatePostSrSourceContract(c),"RejectGuideColorExtentMismatch");
    c=Native();c.render={320,180};c.guides=c.render;result=ValidatePostSrSourceContract(c);
    Check(result && result->extent==c.render && result->motionScaleX==1280 && result->motionScaleY==720,"ReducedGuidesRetainedWithDisplayPixelMotion");
    c.motion={1,1,true,false};result=ValidatePostSrSourceContract(c);
    Check(result && result->motionScaleX==2 && result->motionScaleY==2,"RenderPixelVectorsScaledOnceToDisplayPixels");
    c.motionScaleDomain=MotionScaleDomain::DisplayPixels;result=ValidatePostSrSourceContract(c);
    Check(result && result->motionScaleX==1 && result->motionScaleY==1,"AlreadyDisplayPixelVectorsNotScaledTwice");
    c=Native();c.render=c.guides={427,203};c.motion={427,203,true,false};result=ValidatePostSrSourceContract(c);
    Check(result && std::abs(result->motionScaleX-640)<.001 && std::abs(result->motionScaleY-360)<.001,"NonIntegralPerAxisRatioNormalizedMotion");
    c=Native();c.render=c.guides={641,360};Check(!ValidatePostSrSourceContract(c),"GuidesLargerThanDisplayRejected");
    c=Native();c.render=c.guides={320,180};c.motion.scaleX=std::numeric_limits<float>::max();
    Check(!ValidatePostSrSourceContract(c),"MotionScaleOverflowRejected");
    c=Native();c.color.width=0;Check(!ValidatePostSrSourceContract(c),"RejectEmptyColorExtent");
    c=Native();c.render=c.guides=c.color=c.display={0xffffffffu,0xffffffffu};Check(!ValidatePostSrSourceContract(c),"RejectOversizedExtentBeforeAllocation");
    c=Native();c.encoding=Upscaling::ColorEncoding::Linear;Check(!ValidatePostSrSourceContract(c),"EncodedBytesCannotBeClaimedLinear");
    c=Native();c.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;Check(!ValidatePostSrSourceContract(c),"RejectMixedColorDomainFormat");
    c=Native();c.colorDomain=ColorDomain::Unknown;Check(!ValidatePostSrSourceContract(c),"RejectUnknownColorDomain");
    c=Native();c.depthFormat=DXGI_FORMAT_R16_FLOAT;Check(!ValidatePostSrSourceContract(c),"RejectUnpreparedDepthFormat");
    c=Native();c.motionFormat=DXGI_FORMAT_R32_FLOAT;Check(!ValidatePostSrSourceContract(c),"RejectUnpreparedMotionFormat");
    c=Native();c.motion.currentToPrevious=false;Check(!ValidatePostSrSourceContract(c),"RejectUnconvertedReverseMotion");
    c=Native();c.motion.includesJitter=true;Check(!ValidatePostSrSourceContract(c),"RejectUnremovedMotionJitter");
    c=Native();c.motion.scaleX=0;Check(!ValidatePostSrSourceContract(c),"RejectZeroMotionScale");
    c=Native();c.motion.scaleY=std::numeric_limits<float>::infinity();Check(!ValidatePostSrSourceContract(c),"RejectNonfiniteMotionScale");
    std::printf("RESULT assertions=%d failed=%d\n",assertions,failed);return failed?1:0;
}
