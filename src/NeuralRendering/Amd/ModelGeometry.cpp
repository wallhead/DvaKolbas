#include "ModelGeometry.h"
#include <algorithm>
#include <limits>
namespace TheosRenderPipeline::NeuralRendering::Amd {
std::expected<ModelGeometry,GeometryError> MakeGeometry(Extent input,ExtentMode mode,bool disableExtraHeight) noexcept {
    if(mode!=ExtentMode::Default && mode!=ExtentMode::Step8 && mode!=ExtentMode::Step128) return std::unexpected(GeometryError::InvalidMode);
    if(!input.width || !input.height) return std::unexpected(GeometryError::ZeroDimension);
    const auto round=[](std::uint64_t value,std::uint64_t step){return (value+step-1)/step*step;};
    const unsigned step=mode==ExtentMode::Default?64:mode==ExtentMode::Step8?8:128;
    auto w=round(input.width,step),h=round(input.height,step);
    if(mode==ExtentMode::Default) {
        w=std::max(w,std::uint64_t{320});h=std::max(h,std::uint64_t{320});
        if(!disableExtraHeight && w%256==0 && h%256==0) h+=64;
    }
    constexpr auto max=std::uint64_t(std::numeric_limits<std::int32_t>::max());
    if(w>max || h>max) return std::unexpected(GeometryError::DimensionOverflow);
    ModelGeometry result{input,{std::uint32_t(w),std::uint32_t(h)},{}};
    for(unsigned i=0;i<6;++i) {
        if(i==0 || mode==ExtentMode::Step128) {w/=2;h/=2;}
        else {w=round((w+1)/2,4);h=round((h+1)/2,4);}
        result.levels[i]={{std::uint32_t(w),std::uint32_t(h)},32u<<i};
    }
    return result;
}
}
