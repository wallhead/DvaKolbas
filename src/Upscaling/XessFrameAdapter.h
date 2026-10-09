#pragma once
#include "XessRuntime.h"
#include <optional>
namespace TheosRenderPipeline::Upscaling
{
    enum class XessMotionGuide { Unknown, Undilated, Dilated };
    struct XessInputPolicy
    {
        ColorEncoding sourceEncoding{ColorEncoding::Unknown};
        MotionConvention motion{};
        Extent motionExtent{},depthExtent{};
        bool depthInverted{};
        XessMotionGuide motionGuide{XessMotionGuide::Unknown};
        std::optional<std::uint64_t> historySourceEpoch;
    };
    struct XessFrameParameters
    {
        Extent input{},output{};
        float jitterX{},jitterY{},motionScaleX{},motionScaleY{},exposureScale{1};
        uint32_t flags{},resetHistory{};
        uint64_t sourceId{},sourceEpoch{};
    };
    struct XessJitter
    {
        float generatedX{},generatedY{},sampleX{},sampleY{},projectionX{},projectionY{};
        uint32_t phaseCount{};
    };
    Result<XessFrameParameters> BuildXessFrameParameters(const UpscaleFrame&,const XessInputPolicy&);
    Result<xess_quality_settings_t> XessQuality(Quality);
    Result<Extent> QueryXessRenderExtent(const XessFunctions&,xess_context_handle_t,Quality,Extent);
    Result<XessJitter> XessJitterForGame(float,float,Extent);
    Result<XessJitter> GenerateXessJitter(uint64_t,Extent,Extent);
}
