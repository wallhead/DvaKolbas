#pragma once
#include "NvidiaUpscalerConfiguration.h"
#include "RenderPipeline.h"
#include <d3d11.h>
#include "FrameGen/SourceDLSSGBackend.h"
class NvidiaHost {
public:
    static NvidiaHost* GetSingleton(){static NvidiaHost v;return &v;}
    bool startup{true},dedicatedUI{true},available{true},terminal{},fsr{},fsrFg{};
    struct RecordedConfiguration: TheosRenderPipeline::Upscaler::Configuration {
        unsigned requests{};
        void Request(TheosRenderPipeline::Upscaler::Creation v){++requests;Configuration::Request(v);}
    } sourceUpscalerSettings_;
    RecordedConfiguration& configuration{sourceUpscalerSettings_};
    unsigned& requests{sourceUpscalerSettings_.requests};
    struct UiPass {bool active{},early{};bool Active()const{return active;}bool HasEarlyEvaluation()const{return early;}} nativeUIPass_;
    struct Texture {void GetDesc(D3D11_TEXTURE2D_DESC* d){*d={};d->Format=DXGI_FORMAT_R8G8B8A8_UNORM;}} input;
    struct Targets {Texture* input{};Texture* UpscaleInput(){return input;}} gameTargets_{&input};
    UINT renderWidth_{320},renderHeight_{180},outputWidth_{320},outputHeight_{180};
    bool frameGenerationStateKnown_{true},resetNextEvaluation_{};
    HRESULT failure{S_OK};unsigned lifecycleFailures{};
    NvidiaHost(){configuration.Initialize({DLAA,4,11,false,true});configuration.BeginSubmission();configuration.Completed(true);}
    bool StartupConfigured()const{return startup;}
    bool DedicatedUITextureMode()const{return dedicatedUI;}
    bool CommunityNeuralAvailable()const{return available;}
    bool CommunityNeuralTerminal()const{return terminal;}
    bool FsrActive()const{return StartupConfigured()&&configuration.Startup().mode==FSR;}
    bool XessActive()const{return StartupConfigured()&&configuration.Startup().mode==Xess;}
    bool OrdinarySourceActive()const{return (FsrActive()||XessActive())&&!FsrFgActive();}
    bool FsrFgActive()const{return StartupConfigured()&&fsrFg;}
    bool XessFgActive()const{return false;}
    bool NativeGenerationActive()const{return FsrFgActive();}
    const auto& SourceUpscalerSettings()const{return configuration;}
    HRESULT FailureResult()const{return failure;}
    void FailLifecycle(HRESULT value,const char*){failure=value;++lifecycleFailures;}
    TheosRenderPipeline::Upscaling::Result<void> QuiesceActivePresentation(){
        if(FsrFgActive())return {};
        if(TheosRenderPipeline::SourceDLSSG::Backend::Get().Quiesce())return {};
        return std::unexpected(TheosRenderPipeline::Upscaling::RuntimeError{});
    }
    TheosRenderPipeline::Upscaling::Result<void> ResumeActivePresentation(){
        if(FsrFgActive())return {};
        if(TheosRenderPipeline::SourceDLSSG::Backend::Get().ResumeAfterResize())return {};
        return std::unexpected(TheosRenderPipeline::Upscaling::RuntimeError{});
    }
    // All three implementations are copied unchanged from production.
    void AdoptEffectiveSourceUpscalerSettings() const;
    void RequestSourceUpscalerSettings(TheosRenderPipeline::Upscaler::Creation request);
    void ApplySourceUpscalerSettingsAfterPresent();
};
