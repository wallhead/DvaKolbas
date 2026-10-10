#include "PublicIni.h"
#include "RendererSettingsEdits.h"
#include "RendererStartupValidation.h"
#include "FrameGen/SourceFrameGeneration.h"
#include <SimpleIni.h>
#include <cstdio>
#include <cstdlib>
using namespace TheosRenderPipeline;
static void Require(bool ok,const char* reason) { if(!ok){std::fprintf(stderr,"FAIL: %s\n",reason);std::exit(1);} }
int main()
{
    for (const char* mode:{"DLSS","FSR","XeSS"}) {
        CSimpleIniA ini;ini.SetValue("Upscaling","Upscaler",mode);ini.SetValue("FrameGeneration","Backend","XeSS");
        ini.SetBoolValue("FrameGeneration","Enabled",true);
        Require(PublicIni::Decode(ini).empty(),"explicit Intel FG decodes independently of SR");
        Require(ini.GetLongValue("FrameGeneration","Backend",-1)==3 && ini.GetBoolValue("FrameGeneration","Enabled",false),"Intel owner/enable request survive decoding");
        auto* generation=SourceFrameGeneration::GetSingleton();generation->LoadStartupPreferences(ini);generation->StoreInterpolationPreference(ini);
        Require(generation->settings.generationBackend==3,"startup readers preserve Intel owner");
        Require(PublicIni::Encode(ini).empty() && std::string_view(ini.GetValue("FrameGeneration","Backend",""))=="XeSS","Intel choice round trips as a named option");
        Require(PublicIni::Decode(ini).empty() && ini.GetLongValue("FrameGeneration","Backend",-1)==3,"Intel preference survives restart");
        Require(!ValidateRendererConfiguration(ini,true,true),"named Intel startup configuration validates");
        if(std::string_view(mode)=="FSR") {
            ApplyRendererGpuPolicy(ini,0x1002,true);
            Require(ini.GetLongValue("FrameGeneration","Backend",-1)==3 && ini.GetBoolValue("FrameGeneration","Enabled",false) && ini.GetLongValue("FrameGeneration","BackendPreference",-1)==3,"AMD startup retains explicit Intel FG request");
            ini.SetBoolValue("HDROutput","Enabled",true);ApplyRendererGpuPolicy(ini,0x1002,true);
            Require(ValidateRendererConfiguration(ini,true,true),"AMD Intel HDR request must be rejected rather than erased");
        }
    }
    RendererSettingsDraft draft;draft.valid=true;draft.upscaleType=DLAA;draft.generationBackend=3;
    draft.generationBackendPreference=static_cast<GenerationBackendPreference>(3);
    for(auto mode:{FSR,Xess,DLSS,DLAA}) {
        SetRendererUpscaleMode(draft,mode);Require(draft.generationBackend==3,"SR changes retain explicit Intel backend");
        auto prepared=PrepareRendererStartupDraft(draft,true);Require(prepared.generationBackend==3,"startup draft retains Intel backend");
    }
    RendererSettingsCapabilities caps{true,true,true,false};caps.communityNeural=true;caps.fsrBuilt=caps.fsrFgBuilt=true;
    Require(ValidateRendererSettings(draft,caps)!=nullptr,"unbuilt Intel menu request rejected");
    caps.xessFgBuilt=true;caps.adapterVendorId=0x10de;
    Require(!ValidateRendererSettings(draft,caps),"built Intel choice passes normal DLAA settings validation");
    caps.externalWorld=true;Require(ValidateRendererSettings(draft,caps),"external presentation rejected");caps.externalWorld=false;
    draft.sourceDLSSG.hdrOutput.enabled=true;Require(ValidateRendererSettings(draft,caps),"Intel HDR request rejected explicitly");draft.sourceDLSSG.hdrOutput.enabled=false;
    auto before=draft,after=draft,current=draft;current.generationBackend=1;
    after.generationBackend=2;after.generationBackendPreference=GenerationBackendPreference::Fsr;after.generationEnabled=false;
    auto live=ProjectRendererLiveEdits(before,after,current);
    Require(live.generationBackend==1 && live.generationEnabled==current.generationEnabled,"pending backend choice cannot mutate active owner or live FG request");
    std::puts("PASS: independent Intel settings, roundtrip, source changes and unavailable request");
}
