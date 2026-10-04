#pragma once
#include "NvidiaUpscalerConfiguration.h"
#include "RenderPipeline.h"
class NvidiaHost {
public:
    static NvidiaHost* GetSingleton(){static NvidiaHost v;return &v;}
    bool startup{true},dedicatedUI{true},available{true},terminal{},fsr{},fsrFg{};
    unsigned requests{};
    TheosRenderPipeline::Upscaler::Configuration configuration;
    NvidiaHost(){configuration.Initialize({DLAA,4,11,false,true});configuration.BeginSubmission();configuration.Completed(true);}
    bool StartupConfigured()const{return startup;}
    bool DedicatedUITextureMode()const{return dedicatedUI;}
    bool CommunityNeuralAvailable()const{return available;}
    bool CommunityNeuralTerminal()const{return terminal;}
    bool FsrActive()const{return fsr;}
    bool FsrFgActive()const{return fsrFg;}
    const auto& SourceUpscalerSettings()const{return configuration;}
    // Host boundary facade: preserves the production requested/effective contract.
    // No GPU retirement or presenter replacement is simulated by this function.
    void RequestSourceUpscalerSettings(TheosRenderPipeline::Upscaler::Creation request){
        ++requests;configuration.Request(request);const auto& effective=configuration.Effective();
        auto& p=*RenderPipeline::GetSingleton();p.mUpscaleType=effective.mode;p.mQualityLevel=effective.quality;
        p.mDLSSPreset=effective.preset;p.mAutoExposure=effective.autoExposure;p.mSharpening=effective.sharpening;p.mFsrSettings=effective.fsr;
    }
};
