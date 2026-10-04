#include "PCH.h"
#include "FrameGen/NvidiaHost.h"
#include "FrameGen/SourceDLSSGBackend.h"
#include "DLSSBackend.h"
#include "CommunityShaderIntegration.h"
#include <stdexcept>
using namespace TheosRenderPipeline;
void Require(bool v,const char* message){if(!v)throw std::runtime_error(message);}
void Init(NvidiaHost& host,int mode=DLAA){
    Upscaler::Creation c{mode,4,11,false,true};c.fsr.quality=Upscaling::Quality::NativeAA;c.fsr.sourceColorEncoding=Upscaling::ColorEncoding::Gamma22;
    host.configuration.Initialize(c);host.configuration.BeginSubmission();host.configuration.Completed(true);
    host.failure=S_OK;host.lifecycleFailures=0;host.startup=true;host.fsr=mode==FSR;
    host.nativeUIPass_={};host.frameGenerationStateKnown_=true;host.resetNextEvaluation_=false;
    auto& b=SourceDLSSG::Backend::Get();b.events.clear();b.retire=[] {return true;};b.resume=[] {return true;};
    DLSSBackend::GetSingleton()->create=[] {return true;};CommunityShaders::active=false;host.AdoptEffectiveSourceUpscalerSettings();
}
int main(int argc,char** argv){try{
    if(argc>2||(argc==2&&std::string_view(argv[1])!="--omit-deferred"))return 1;
    NvidiaHost host;Init(host);auto& backend=SourceDLSSG::Backend::Get();auto& dlss=*DLSSBackend::GetSingleton();
    const bool omit=argc==2&&std::string_view(argv[1])=="--omit-deferred";
    auto request=host.configuration.Requested();request.preset=12;host.RequestSourceUpscalerSettings(request);
    Require(host.configuration.NeedsLiveChange()&&RenderPipeline::GetSingleton()->mDLSSPreset==11&&backend.events.empty(),"request stays deferred with active feature unchanged");
    host.nativeUIPass_.active=true;host.ApplySourceUpscalerSettingsAfterPresent();
    Require(backend.events.empty(),"active UI pass prevents feature replacement");
    host.nativeUIPass_={false,true};host.ApplySourceUpscalerSettingsAfterPresent();
    Require(backend.events.empty(),"early evaluated UI/source prevents feature replacement");
    host.nativeUIPass_={};if(!omit)host.ApplySourceUpscalerSettingsAfterPresent();
    Require(backend.events==std::vector<std::string>{"retire","create","resume"},"actual deferred host must retire, create and resume in order");
    Require(!host.configuration.NeedsLiveChange()&&host.configuration.Effective().preset==12&&RenderPipeline::GetSingleton()->mDLSSPreset==12&&!host.frameGenerationStateKnown_&&host.resetNextEvaluation_,"successful replacement publishes effective settings and resets source/FG history");
    Require(dlss.last.quality==5&&dlss.last.format==DXGI_FORMAT_R8G8B8A8_UNORM&&dlss.last.renderWidth==320&&dlss.last.outputWidth==320&&dlss.last.renderHeight==180&&dlss.last.outputHeight==180&&dlss.last.preset==12&&!dlss.last.sharpening&&dlss.last.autoExposure,"DLAA allocation, format and live options reach creation boundary");
    host.ApplySourceUpscalerSettingsAfterPresent();Require(backend.events.size()==3,"completed feature request cannot be recreated twice");
    for(int failure=0;failure<3;++failure){
        Init(host);request=host.configuration.Requested();request.preset=12;host.RequestSourceUpscalerSettings(request);
        backend.retire=[=]{return failure!=0;};dlss.create=[=]{return failure!=1;};backend.resume=[=]{return failure!=2;};
        host.ApplySourceUpscalerSettingsAfterPresent();
        Require(host.configuration.Failed()&&FAILED(host.FailureResult())&&host.lifecycleFailures==1,"retire/create/resume failure faults host and configuration");
        Require(backend.events.size()==unsigned(failure+1)&&host.configuration.Effective().preset==11&&RenderPipeline::GetSingleton()->mDLSSPreset==11,"uncertain feature cannot publish requested settings or call subsequent boundaries");
        host.ApplySourceUpscalerSettingsAfterPresent();Require(backend.events.size()==unsigned(failure+1)&&host.lifecycleFailures==1,"terminal lifecycle failure is not retried");
    }
    Init(host);request=host.configuration.Requested();request.mode=FSR;request.fsr.sharpness=.3f;host.RequestSourceUpscalerSettings(request);host.ApplySourceUpscalerSettingsAfterPresent();
    Require(host.configuration.NeedsRestart()&&host.configuration.Effective().mode==DLAA&&backend.events.empty(),"provider-only change cannot replace active NVIDIA allocation");
    Init(host,FSR);request=host.configuration.Requested();request.fsr.sharpness=.4f;request.fsr.quality=Upscaling::Quality::Quality;host.RequestSourceUpscalerSettings(request);host.ApplySourceUpscalerSettingsAfterPresent();
    Require(backend.events.empty()&&host.configuration.NeedsRestart()&&host.configuration.Effective().fsr.quality==Upscaling::Quality::NativeAA&&host.configuration.Effective().fsr.sharpness==.4f,"FSR updates dispatch sharpness only, retaining allocation and vendor ownership");
    Init(host);CommunityShaders::active=true;request=host.configuration.Requested();request.preset=12;host.RequestSourceUpscalerSettings(request);
    Require(!host.configuration.NeedsLiveChange(),"external source ownership prevents TRP request mutation");
    std::cout<<"PASS deferred production host success, UI deferral, three terminal failures and provider staging\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
