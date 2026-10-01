#include <PCH.h>
#include "GameCameraMeasurements.h"
#include "SourceCameraSelection.h"
#include "../RE/BSGraphics.h"
namespace TheosRenderPipeline
{
    Upscaling::Result<Upscaling::CameraMeasurements> CaptureGameCameraMeasurements(BSGraphics::State* state,Upscaling::Extent extent,bool jittered,bool reset)
    {
        auto invalid=[](const char* why)->Upscaling::Result<Upscaling::CameraMeasurements>{return std::unexpected(Upscaling::RuntimeError{Upscaling::ErrorKind::InvalidInput,0,why});};
        auto* player=RE::PlayerCamera::GetSingleton();
        if(!state || !extent.width || !extent.height || !player || !player->cameraRoot)return invalid("Player camera/graphics state unavailable");
        // Shared retained-camera selection used by the NVIDIA capture as well;
        // this path produces measurements and never asks a vendor for constants.
        const auto selection=SourceDLSSG::SelectOwnedCameraView(RE::NiPointer<RE::NiAVObject>(player->cameraRoot),state->GetRuntimeData().kCameraDataCacheA,jittered,
            [](RE::NiAVObject* object){return netimmerse_cast<RE::NiCamera*>(object)!=nullptr;});
        if(selection.ambiguous || !selection.entry)return invalid("No unique retained player view matching the jitter convention");
        DirectX::XMFLOAT4X4 projection,view;DirectX::XMStoreFloat4x4(&projection,selection.entry->CamViewData.m_ProjMatrixUnjittered);DirectX::XMStoreFloat4x4(&view,selection.entry->CamViewData.m_ViewMat);
        const auto* camera=static_cast<const RE::NiCamera*>(selection.camera.get());const auto& position=camera->world.translate;
        if(projection._11<=0 || std::abs(projection._22/projection._11-float(extent.width)/extent.height)>0.02f)return invalid("Camera projection disagrees with raster extent");
        return MeasureCamera(projection,view,{position.x,position.y,position.z},camera->viewFrustum.fNear,camera->viewFrustum.fFar,
            reinterpret_cast<std::uintptr_t>(camera),reset,RE::bhkWorld::GetWorldScale());
    }
}
