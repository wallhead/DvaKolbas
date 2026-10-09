#include "XessFrameAdapter.h"
#include <cmath>
#include <limits>
namespace TheosRenderPipeline::Upscaling
{
    namespace
    {
        RuntimeError Invalid(const char* message) { return {ErrorKind::InvalidInput,0,message}; }
        bool Valid(Extent extent) { return extent.width && extent.height; }
        bool ValidPair(Extent input,Extent output)
        { return Valid(input) && Valid(output) && input.width<=output.width && input.height<=output.height; }
        bool Sample(float value) { return std::isfinite(value) && std::abs(value)<=.5f; }
        float Halton(uint32_t index,uint32_t base)
        {
            double result=0,fraction=1;
            while (index) { fraction/=base;result+=fraction*(index%base);index/=base; }
            return static_cast<float>(result);
        }
    }
    Result<XessFrameParameters> BuildXessFrameParameters(const UpscaleFrame& frame,const XessInputPolicy& policy)
    {
        if (frame.backend!=BackendKind::Xess || !ValidPair(frame.render,frame.display) || frame.subrect!=frame.render)
            return std::unexpected(Invalid("XeSS requires valid fixed input/output extents and a full input subrect"));
        if (policy.sourceEncoding!=ColorEncoding::Linear && policy.sourceEncoding!=ColorEncoding::Gamma22 && policy.sourceEncoding!=ColorEncoding::SRGB)
            return std::unexpected(Invalid("XeSS source colour encoding is unknown"));
        if (!frame.colorIsLinear || frame.colorFormat!=DXGI_FORMAT_R16G16B16A16_FLOAT || frame.depthFormat!=DXGI_FORMAT_R32_FLOAT || frame.motionFormat!=DXGI_FORMAT_R16G16_FLOAT)
            return std::unexpected(Invalid("XeSS requires prepared linear FP16 colour, R32_FLOAT depth and signed RG16_FLOAT motion"));
        if (policy.motionExtent!=frame.render || policy.depthExtent!=frame.render || policy.motionGuide!=XessMotionGuide::Undilated)
            return std::unexpected(Invalid("XeSS low-resolution guides must match input size and be explicitly undilated"));
        if (frame.camera.depthInverted!=policy.depthInverted || frame.motionConvention.currentToPrevious!=policy.motion.currentToPrevious || frame.motionConvention.includesJitter!=policy.motion.includesJitter ||
            frame.motionConvention.scaleX!=policy.motion.scaleX || frame.motionConvention.scaleY!=policy.motion.scaleY ||
            !std::isfinite(policy.motion.scaleX) || !std::isfinite(policy.motion.scaleY) || policy.motion.scaleX==0 || policy.motion.scaleY==0)
            return std::unexpected(Invalid("XeSS measured depth/motion conventions do not match this input"));
        if (!Sample(frame.jitterX) || !Sample(frame.jitterY) || !std::isfinite(frame.preExposure) || frame.preExposure!=1 || frame.exposure || frame.reactive || frame.transparencyComposition || !frame.sourceId || !frame.sourceEpoch)
            return std::unexpected(Invalid("XeSS SDR frame requires valid half-pixel jitter, exposure 1, source identity and no unsupported masks"));
        XessFrameParameters result{};
        result.input=frame.render;result.output=frame.display;
        result.jitterX=frame.jitterX;result.jitterY=frame.jitterY;
        const auto direction=policy.motion.currentToPrevious?1.f:-1.f;
        result.motionScaleX=direction*policy.motion.scaleX;result.motionScaleY=direction*policy.motion.scaleY;
        result.flags=XESS_INIT_FLAG_LDR_INPUT_COLOR;
        if (policy.depthInverted) result.flags|=XESS_INIT_FLAG_INVERTED_DEPTH;
        if (policy.motion.includesJitter) result.flags|=XESS_INIT_FLAG_JITTERED_MV;
        result.resetHistory=frame.reset || frame.camera.reset || !policy.historySourceEpoch || *policy.historySourceEpoch!=frame.sourceEpoch;
        result.sourceId=frame.sourceId;result.sourceEpoch=frame.sourceEpoch;
        return result;
    }
    Result<xess_quality_settings_t> XessQuality(Quality quality)
    {
        switch(quality) {
        case Quality::NativeAA:return XESS_QUALITY_SETTING_AA;
        case Quality::Quality:return XESS_QUALITY_SETTING_QUALITY;
        case Quality::Balanced:return XESS_QUALITY_SETTING_BALANCED;
        case Quality::Performance:return XESS_QUALITY_SETTING_PERFORMANCE;
        }
        return std::unexpected(Invalid("XeSS render scale must be Native, Quality, Balanced or Performance"));
    }
    Result<Extent> QueryXessRenderExtent(const XessFunctions& api,xess_context_handle_t context,Quality quality,Extent output)
    {
        const auto setting=XessQuality(quality);
        if (!setting) return std::unexpected(setting.error());
        if (!context || !api.GetOptimalInputResolution || !Valid(output)) return std::unexpected(Invalid("XeSS sizing requires a live context and nonzero output"));
        const xess_2d_t size{output.width,output.height};xess_2d_t optimal{},minimum{},maximum{};
        const auto result=api.GetOptimalInputResolution(context,&size,*setting,&optimal,&minimum,&maximum);
        if (result!=XESS_RESULT_SUCCESS)
            return std::unexpected(RuntimeError{result==XESS_RESULT_ERROR_UNSUPPORTED_DEVICE?ErrorKind::UnsupportedDevice:ErrorKind::ContextFailure,result,"xessGetOptimalInputResolution failed"});
        const Extent input{optimal.x,optimal.y};
        if (!ValidPair(input,output) || !minimum.x || !minimum.y || minimum.x>optimal.x || minimum.y>optimal.y || maximum.x<optimal.x || maximum.y<optimal.y || maximum.x>output.width || maximum.y>output.height ||
            (quality==Quality::NativeAA && input!=output))
            return std::unexpected(Invalid("XeSS SDK returned invalid input-resolution bounds"));
        return input;
    }
    Result<XessJitter> XessJitterForGame(float x,float y,Extent render)
    {
        if (!Valid(render) || !Sample(x) || !Sample(y)) return std::unexpected(Invalid("XeSS jitter requires valid input size and finite half-pixel samples"));
        XessJitter result{};result.generatedX=x;result.generatedY=y;
        // UpscalerHooks stores -x/-y and sets projection jitter to -2*x/W,+2*y/H.
        // SDK's sample offsets describe that same projection; never negate again at dispatch.
        result.sampleX=-x;result.sampleY=-y;
        result.projectionX=-2*x/render.width;result.projectionY=2*y/render.height;
        return result;
    }
    Result<XessJitter> GenerateXessJitter(uint64_t sourceIndex,Extent render,Extent output)
    {
        if (!ValidPair(render,output)) return std::unexpected(Invalid("XeSS jitter requires valid SDK-sized input/output"));
        const double ratio=static_cast<double>(output.width)/render.width;
        const double count=std::ceil(8*ratio*ratio);
        if (count>std::numeric_limits<uint32_t>::max()) return std::unexpected(Invalid("XeSS jitter sequence size overflow"));
        const auto phase=static_cast<uint32_t>(count);
        const auto index=static_cast<uint32_t>(sourceIndex%phase)+1;
        auto jitter=XessJitterForGame(Halton(index,2)-.5f,Halton(index,3)-.5f,render);
        if (jitter) jitter->phaseCount=phase;
        return jitter;
    }
}
