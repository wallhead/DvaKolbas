#include "RendererBackendPolicy.h"
#include "RendererGpuPolicy.h"
#include "NvidiaUpscalerConfiguration.h"
#include "PublicIni.h"
#include "RendererStartupValidation.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>
static void Require(bool ok,const char* message){if(!ok){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
int main()
{
    using namespace TheosRenderPipeline;using namespace Upscaling;
    for(unsigned vendor:{0x10deu,0x1002u,0x8086u}) {
        BackendConfiguration request;request.backend=BackendKind::Xess;request.adapterVendorId=vendor;
        request.generationBackend=0;request.generationEnabled=false;
        const auto result=ResolveBackend(request,true,true);
        Require(result.valid && result.backend==BackendKind::Xess && result.presentation==PresentationKind::Ordinary,"XeSS SR selects ordinary presenter on each vendor");
        CSimpleIniA ini;ini.SetLongValue("Settings","UpscaleType",5);ini.SetLongValue("FrameGeneration","Backend",0);
        ini.SetBoolValue("FrameGeneration","Enabled",false);ini.SetBoolValue("NeuralRendering","Enabled",false);
        ApplyRendererGpuPolicy(ini,vendor,true,vendor!=0x10de);
        Require(ini.GetLongValue("Settings","UpscaleType",-1)==5,"non-NVIDIA explicit XeSS request retained");
        CSimpleIniA named;named.LoadData("[Upscaling]\nUpscaler=XeSS\n[FrameGeneration]\nEnabled=false\n[NeuralRendering]\nEnabled=false\n");
        Require(PublicIni::Decode(named).empty() && ValidateRendererStartup(named,vendor,true,true,vendor!=0x10de).empty(),"named XeSS startup validates with NR/FG off on each vendor");
        named.SetBoolValue("FrameGeneration","Enabled",true);
        Require(!ValidateRendererStartup(named,vendor,true,true,vendor!=0x10de).empty(),"XeSS SR-only gate reports unsupported FG request");
    }
    BackendConfiguration ordinary;ordinary.backend=BackendKind::Fsr;ordinary.generationBackend=0;ordinary.generationEnabled=false;
    Require(ResolveBackend(ordinary,true,true).valid && !GetModuleHandleW(L"libxess.dll"),"unrequested XeSS never loads its optional dispatcher");
    Upscaler::Configuration config;Upscaler::Creation requested;requested.mode=Xess;config.Initialize(requested);
    auto fallback=requested;fallback.mode=FSR;
    Require(config.UseStartupFallback(fallback),"fallback accepted before resource commitment");
    Require(config.Requested().mode==Xess && config.Startup().mode==FSR && !config.NeedsRestart(),"requested XeSS preserved separately from effective FSR fallback");
    config.BeginSubmission();config.Completed(true);
    Require(!config.UseStartupFallback(requested),"provider fallback refused after successful feature creation");
    std::puts("PASS: XeSS vendor-independent SR routing and startup fallback boundary");
}
