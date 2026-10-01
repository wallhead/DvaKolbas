#pragma once
#include "CameraMeasurements.h"
namespace BSGraphics { struct State; }
namespace TheosRenderPipeline
{
    Upscaling::Result<Upscaling::CameraMeasurements> CaptureGameCameraMeasurements(BSGraphics::State*,Upscaling::Extent,bool jittered,bool reset);
}
