#pragma once
#include "RendererSettings.h"
namespace TheosRenderPipeline::Overlay {struct Layout;}
class RenderPipeline {
public:
    static RenderPipeline* GetSingleton(){static RenderPipeline v;return &v;}
    int mUpscaleType{DLAA},mQualityLevel{4},mDLSSPreset{11};
    bool mDlssNativeScale{true};
    std::uint32_t mAdapterVendorId{};
    TheosRenderPipeline::Upscaling::FsrSettings mFsrSettings;
    bool mDynamicResolutionRequested{},mAutoExposure{true},mSharpening{},mEnableJitter{true},mNativeUI{true};
    bool mReShadeBeforeUpscaling{},mWheelerLateOverlayBridge{true};float mSharpness{};
    std::atomic_bool mRequestLoadingArtwork{true};unsigned saves{};bool saveResult{true};
    bool SaveINI(const TheosRenderPipeline::Overlay::Layout*){++saves;return saveResult;}
};
