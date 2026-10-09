#include "PublicIni.h"
#include "NvidiaUpscalerConfiguration.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>
static void Require(bool ok,const char* message){if(!ok){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
int main()
{
    using namespace TheosRenderPipeline;
    for(const auto* quality:{"Native","Quality","Balanced","Performance"}) {
        CSimpleIniA ini;ini.LoadData("[Upscaling]\nUpscaler=XeSS\n[FrameGeneration]\nEnabled=false\n[NeuralRendering]\nEnabled=false\n");
        ini.SetValue("XeSS","Quality",quality);ini.SetValue("XeSS","SourceColorEncoding","Gamma22");
        Require(PublicIni::Decode(ini).empty(),"public XeSS settings decode");
        Require(ini.GetLongValue("Settings","UpscaleType",-1)==5,"XeSS selector preserved");
        Require(!ini.GetBoolValue("FrameGeneration","Enabled",true) && !ini.GetBoolValue("NeuralRendering","Enabled",true),"SR-only startup keeps NR/FG off");
        Require(PublicIni::Encode(ini).empty(),"XeSS settings encode");
        Require(std::string_view(ini.GetValue("Upscaling","Upscaler",""))=="XeSS","XeSS survives save");
        Require(std::string_view(ini.GetValue("XeSS","Quality",""))==quality,"named quality survives save");
        Require(std::string_view(ini.GetValue("DLSS","Quality",""))=="Native","saving XeSS preserves inactive DLAA preference");
    }
    Upscaler::Configuration config;Upscaler::Creation request;request.mode=5;config.Initialize(request);
    Require(config.Startup().mode==5,"XeSS creation does not silently sanitize to DLSS");
    config.BeginSubmission();config.Completed(true);
    request.xess.quality=Upscaling::Quality::Performance;config.Request(request);
    Require(config.NeedsRestart() && !config.NeedsLiveChange(),"XeSS quality changes cannot reach live DLSS feature replacement");
    request.xess.quality=Upscaling::Quality::NativeAA;request.xess.sourceEncoding=Upscaling::ColorEncoding::SRGB;config.Request(request);
    Require(config.NeedsRestart() && !config.NeedsLiveChange(),"XeSS source encoding requires restart");
    CSimpleIniA sharpIni;sharpIni.SetValue("XeSS","Sharpness","0.75");
    auto sharp=Upscaling::ReadXessSettings(sharpIni);Require(bool(sharp),"valid XeSS sharpness");
    CSimpleIniA saved;Upscaling::StoreXessSettings(saved,*sharp);
    Require(saved.GetDoubleValue("XeSS","Sharpness",-1)==0.75,"XeSS sharpness survives settings save");
    config.Request(config.Startup());request=config.Startup();request.xess=*sharp;config.Request(request);
    Require(!config.NeedsRestart() && config.NeedsLiveChange(),"XeSS sharpening applies live without changing SDK creation");
    request.xess.quality=Upscaling::Quality::Performance;config.Request(request);
    Require(config.NeedsRestart() && config.NeedsLiveChange(),"pending quality must not block live sharpening");
    Require(config.LiveCandidate().xess.quality==Upscaling::Quality::NativeAA,"live sharpening preserves allocated XeSS quality");
    for(const auto* invalid:{"nan","inf","-0.1","1.1","0.5junk"}) {
        sharpIni.SetValue("XeSS","Sharpness",invalid);
        Require(!Upscaling::ReadXessSettings(sharpIni),"invalid XeSS strength rejected");
    }
    std::puts("PASS: XeSS public choices and creation selector");
}
