#include "RendererBackendPolicy.h"
#include "FrameGen/PresentationPolicy.h"
#include "FrameGen/XessGenerationStartupPolicy.h"
#include <cstdio>
#include <cstdlib>
using namespace TheosRenderPipeline;
using namespace Upscaling;
static void Require(bool ok,const char* reason) { if(!ok){std::fprintf(stderr,"FAIL: %s\n",reason);std::exit(1);} }
int main()
{
    static_assert(static_cast<int>(PresentationKind::Ordinary)==0 && static_cast<int>(PresentationKind::Nvidia)==1 && static_cast<int>(PresentationKind::Fsr)==2 && static_cast<int>(PresentationKind::Xess)==3);
    BackendConfiguration config;config.generationBackend=3;
    for(auto mode:{BackendKind::Dlss,BackendKind::Dlaa,BackendKind::Fsr,BackendKind::Xess}) {
        config.backend=mode;config.adapterVendorId=0x10de;
        auto decision=ResolveBackend(config,true,true);
        Require(decision.valid && static_cast<int>(decision.presentation)==3,"explicit Intel presenter routes independently of SR");
        config.generationEnabled=false;Require(ResolveBackend(config,true,true).valid,"FG off retains Intel presenter for live reentry");
    }
    config.backend=BackendKind::Fsr;
    Require(!ResolveBackend(config,true,true,false).valid,"explicit Intel request requires built support");
    for(auto vendor:{0x10deu,0x1002u,0x8086u}) {
        config.adapterVendorId=vendor;Require(ResolveBackend(config,true,true).valid,"cross-vendor routing without pretending device qualification");
    }
    config.hdr=true;Require(!ResolveBackend(config,true,true).valid,"Intel HDR unavailable");config.hdr=false;
    config.dynamicResolution=true;Require(!ResolveBackend(config,true,true).valid,"Intel fixed source sizing");config.dynamicResolution=false;
    config.backend=BackendKind::External;Require(!ResolveBackend(config,true,true).valid,"external source ownership not accepted");
    config.backend=BackendKind::Fsr;config.neuralRendering=true;config.communityNeural=false;
    Require(!ResolveBackend(config,true,true).valid,"Intel refuses legacy NR");
    config.generationBackend=2;config.neuralRendering=false;Require(ResolveBackend(config,true,true).valid,"existing FSR owner unaffected");
    config.backend=BackendKind::Dlss;config.generationBackend=1;Require(ResolveBackend(config,true,true).valid,"existing NVIDIA owner unaffected");
    XessGenerationStartupCapabilities caps;
    Require(!ValidateXessGenerationStartup(PresentationKind::Nvidia,caps) && !ValidateXessGenerationStartup(PresentationKind::Fsr,caps),"missing optional Intel files cannot break another owner");
    Require(ValidateXessGenerationStartup(PresentationKind::Xess,caps),"explicit Intel request needs both runtime files");
    caps.runtimePairPresent=true;Require(ValidateXessGenerationStartup(PresentationKind::Xess,caps),"explicit Intel needs native device");
    caps.nativeDevice=true;Require(ValidateXessGenerationStartup(PresentationKind::Xess,caps),"native device must share actual LUID");
    caps.sameAdapter=true;Require(ValidateXessGenerationStartup(PresentationKind::Xess,caps),"non-Intel requires measured SM 6.4");
    caps.shaderModel64=true;Require(ValidateXessGenerationStartup(PresentationKind::Xess,caps),"engine boundaries must be verified");
    caps.engineHooks=true;Require(!ValidateXessGenerationStartup(PresentationKind::Xess,caps),"complete observed prerequisites accepted");
    caps.intelAdapter=true;caps.shaderModel64=false;Require(!ValidateXessGenerationStartup(PresentationKind::Xess,caps),"Intel uses actual SDK device qualification");
    struct Operations {
        unsigned ordinary{},nvidia{},fsr{},intel{},retired{};
        HRESULT CreateOrdinary(){++ordinary;return S_OK;} HRESULT CreateNvidia(){++nvidia;return S_OK;}
        HRESULT CreateFsr(){++fsr;return S_OK;} HRESULT CreateXess(){++intel;return E_FAIL;}
        bool OrdinaryReady(){return false;} bool NvidiaReady(){return false;} bool FsrReady(){return false;} bool XessReady(){return true;}
        HRESULT RetireOrdinary(){return E_FAIL;} HRESULT RetireNvidia(){return E_FAIL;} HRESULT RetireFsr(){return E_FAIL;}
        HRESULT RetireXess(){++retired;return S_OK;}
    } operations;
    BackendDecision intel{BackendKind::Dlss,PresentationKind::Xess,true,false,{}};
    Require(CreatePresentation(intel,operations)==E_FAIL && operations.intel==1 && !operations.ordinary && !operations.nvidia && !operations.fsr,"partial Intel creation never falls through to another owner");
    Require(PresentationReady(intel,operations) && RetirePresentation(intel,operations)==S_OK && operations.retired==1,"Intel readiness and retirement route to its owner");
    std::puts("PASS: independent Intel routing; physical device readiness remains separate");
}
