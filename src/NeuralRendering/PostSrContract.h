#pragma once
#include "ImagePacket.h"
#include "Upscaling/UpscalerBackend.h"
#include <cmath>

namespace TheosRenderPipeline::NeuralRendering {
// Metadata admission only; this does not prove texture ownership, GPU producer
// order or that FG consumed the enhanced source. Those remain adapter gates.
struct PostSrSourceContract {
    Upscaling::BackendKind backend{Upscaling::BackendKind::External};
    Upscaling::UpscaleOutcome outcome{Upscaling::UpscaleOutcome::Fatal};
    ImageKind kind{ImageKind::Real};
    uint64_t epoch{}, sourceId{}, previousSourceId{}, guideEpoch{}, guideSourceId{};
    double sourceTime{}, guideTime{};
    ImageExtent render, display, color, guides;
    ColorDomain colorDomain{ColorDomain::Unknown};
    Upscaling::ColorEncoding encoding{Upscaling::ColorEncoding::Unknown};
    DXGI_FORMAT colorFormat{DXGI_FORMAT_UNKNOWN}, depthFormat{DXGI_FORMAT_UNKNOWN}, motionFormat{DXGI_FORMAT_UNKNOWN};
    GuideOrigin guideOrigin{GuideOrigin::Unknown};
    bool depthInverted{};
    Upscaling::MotionConvention motion;
};
struct PostSrGuidePlan {
    ImageExtent extent;
    float motionScaleX{}, motionScaleY{};
};
inline Result<PostSrGuidePlan> ValidatePostSrSourceContract(const PostSrSourceContract& source) {
    const auto fail=[](ErrorKind kind,const char* message)->Result<PostSrGuidePlan>{
        return std::unexpected(Error{kind,0,message});
    };
    if(source.backend!=Upscaling::BackendKind::Dlss && source.backend!=Upscaling::BackendKind::Dlaa &&
        source.backend!=Upscaling::BackendKind::Fsr)
        return fail(ErrorKind::Unsupported,"NR post-SR source backend is not owned");
    if(source.outcome!=Upscaling::UpscaleOutcome::Temporal)
        return fail(ErrorKind::Unsupported,"NR post-SR requires a completed temporal real source");
    if(source.kind!=ImageKind::Real || source.guideOrigin!=GuideOrigin::RealSource)
        return fail(ErrorKind::Unsupported,"NR post-SR accepts real-source guides only");
    if(!source.epoch || !source.sourceId || source.previousSourceId>=source.sourceId ||
        source.guideEpoch!=source.epoch || source.guideSourceId!=source.sourceId ||
        !std::isfinite(source.sourceTime) || !std::isfinite(source.guideTime) || source.sourceTime!=source.guideTime)
        return fail(ErrorKind::InvalidInput,"NR post-SR real source/guide identity or time is stale");
    const auto valid=[](ImageExtent extent){return extent.width && extent.height &&
        extent.width<=16384 && extent.height<=16384;};
    if(!valid(source.render) || !valid(source.display) || source.color!=source.display || source.guides!=source.render)
        return fail(ErrorKind::InvalidInput,"NR post-SR real color/guide extent invalid");
    if(source.render!=source.display)
        return fail(ErrorKind::Unsupported,"NR post-SR reduced-resolution guide recipe is unqualified; use Native AA");
    if(NrColorFormat(source.colorDomain)==DXGI_FORMAT_UNKNOWN || source.colorFormat!=NrColorFormat(source.colorDomain) ||
        (source.colorDomain==ColorDomain::Linear && source.encoding!=Upscaling::ColorEncoding::Linear) ||
        (source.colorDomain==ColorDomain::SdrBytes && source.encoding!=Upscaling::ColorEncoding::Gamma22 &&
            source.encoding!=Upscaling::ColorEncoding::SRGB))
        return fail(ErrorKind::Unsupported,"NR post-SR color domain/format/transfer contract unqualified");
    if(source.depthFormat!=DXGI_FORMAT_R32_FLOAT || source.motionFormat!=DXGI_FORMAT_R16G16_FLOAT ||
        !source.motion.currentToPrevious || source.motion.includesJitter ||
        !std::isfinite(source.motion.scaleX) || !std::isfinite(source.motion.scaleY) ||
        source.motion.scaleX<=0 || source.motion.scaleY<=0)
        return fail(ErrorKind::InvalidInput,"NR post-SR real guide formats/direction/jitter/units invalid");
    return PostSrGuidePlan{source.display, source.motion.scaleX, source.motion.scaleY};
}
}
