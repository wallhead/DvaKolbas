#pragma once
#include "UpscalerBackend.h"
#include "CameraDepthPolicy.h"
#include <cmath>
namespace TheosRenderPipeline::Upscaling
{
    struct HistoryDecision { bool valid{},reset{}; };
    class FsrHistoryPolicy
    {
    public:
        HistoryDecision Accept(std::uint64_t sourceId,const CameraMeasurements& camera,Extent extent,bool transition,bool skipped)
        {
            bool finite=true;
            for(float value:camera.view)finite &= std::isfinite(value);
            for(float value:camera.projection)finite &= std::isfinite(value);
            for(float value:camera.position)finite &= std::isfinite(value);
            if(skipped || transition || !sourceId || (seen_ && sourceId<=lastSource_) || !extent.width || !extent.height ||
                !camera.identity || !finite || !ValidCameraDepthRange(camera.nearDistance,camera.farDistance,camera.depthInfinite) || !std::isfinite(camera.verticalFovRadians) ||
                camera.verticalFovRadians<=0 || camera.verticalFovRadians>=3.141593f || !std::isfinite(camera.worldUnitsToMeters) || camera.worldUnitsToMeters<=0) {
                Invalidate();return {};
            }
            bool discontinuity=!seen_ || invalidated_ || camera.reset;
            if(seen_) {
                discontinuity |= sourceId!=lastSource_+1 || camera.identity!=lastCamera_.identity || extent!=lastExtent_ ||
                    camera.depthInverted!=lastCamera_.depthInverted || camera.depthInfinite!=lastCamera_.depthInfinite ||
                    std::abs(camera.verticalFovRadians-lastCamera_.verticalFovRadians)>0.01f;
                float distanceSquared{};
                for(unsigned i=0;i<3;++i){float delta=(camera.position[i]-lastCamera_.position[i])*camera.worldUnitsToMeters;distanceSquared+=delta*delta;}
                discontinuity |= distanceSquared>4;
                for(unsigned axis=0;axis<3;++axis) {
                    float dot{};for(unsigned j=0;j<3;++j)dot+=camera.view[axis*4+j]*lastCamera_.view[axis*4+j];
                    discontinuity |= dot<0.5f;
                }
            }
            seen_=true;invalidated_=false;lastSource_=sourceId;lastCamera_=camera;lastExtent_=extent;
            return {true,discontinuity};
        }
        void Invalidate(){invalidated_=true;}
    private:
        CameraMeasurements lastCamera_;Extent lastExtent_;std::uint64_t lastSource_{};bool seen_{},invalidated_{true};
    };
}
