#include "NeuralRendering/MotionDiagnosticStats.h"
#include <DirectXPackedVector.h>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <limits>

using namespace TheosRenderPipeline::NeuralRendering;
namespace {
int failures{};
void Check(bool good,const char* name){std::printf("%s %s\n",good?"PASS":"FAIL",name);failures+=!good;}
bool Near(double actual,double expected){return std::abs(actual-expected)<1e-6;}
}
int main(){
    const auto ultrawide=DiagnosticReadbackRegion(5120,1440);
    Check(ultrawide && ultrawide->width==512 && ultrawide->height==512 &&
          ultrawide->x==2304 && ultrawide->y==464,"UltrawideProbeHasBoundedCenteredReadback");
    const auto small=DiagnosticReadbackRegion(65,37);
    Check(small && small->x==0 && small->y==0 && small->width==65 && small->height==37,
          "SmallProbeRetainsCompleteSource");
    Check(!DiagnosticReadbackRegion(0,37) && !DiagnosticReadbackRegion(65,16385),
          "ProbeRejectsEmptyAndImpossibleExtent");
    const std::array<std::uint8_t,16> before{64,128,192,255, 192,64,32,255, 9,9,9,9, 9,9,9,9};
    const std::array<std::uint8_t,16> after {128,128,192,255, 128,64,32,255, 9,9,9,9, 9,9,9,9};
    const auto rgb=CompareRgba8(before.data(),after.data(),16,16,2,1,1);
    Check(rgb && rgb->samples==2 && Near(rgb->before[0],128) && Near(rgb->after[0],128),"ColorReadbackUsesActiveExtentAndPitch");
    Check(rgb && Near(rgb->signedChange[0],0) && Near(rgb->absoluteChange[0],64),"SignedAndAbsoluteColorChangeStayDistinct");
    std::array<std::uint16_t,8> motion{DirectX::PackedVector::XMConvertFloatToHalf(.25f),0,
        DirectX::PackedVector::XMConvertFloatToHalf(-.5f),0,0,0,0,0};
    const auto guide=SummarizeMotionRg16(reinterpret_cast<const std::uint8_t*>(motion.data()),16,2,1,2,1,1);
    Check(guide && guide->samples==2 && Near(guide->meanXpixels,-.25) && Near(guide->meanMagnitudePixels,.75),
        "MotionReadbackUsesNormalizedSignedHalfAndPixelScale");
    motion[2]=DirectX::PackedVector::XMConvertFloatToHalf(std::numeric_limits<float>::infinity());
    const auto bad=SummarizeMotionRg16(reinterpret_cast<const std::uint8_t*>(motion.data()),16,2,1,2,1,1);
    Check(bad && bad->nonFinite==1 && bad->samples==1,"NonFiniteMotionIsCountedAndExcluded");
    Check(!CompareRgba8(before.data(),after.data(),7,8,2,1,1) &&
          !SummarizeMotionRg16(reinterpret_cast<const std::uint8_t*>(motion.data()),7,2,1,2,1,1),
          "ShortReadbackPitchRejected");
    return failures?1:0;
}
