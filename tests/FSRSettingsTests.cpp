#include "Upscaling/FSRSettings.h"
#include "RendererSettings.h"
#include "NvidiaUpscalerConfiguration.h"
#include <SimpleIni.h>
#include <iostream>
#include <stdexcept>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
void Require(bool v,const char* why){if(!v)throw std::runtime_error(why);}
int main(int argc,char** argv) {
 try {
  CSimpleIniA ini;
  Require(ReadFsrSettings(ini).has_value(),"absent keys retain defaults");
  Require(ReadFsrSettings(ini)->sourceColorEncoding==ColorEncoding::Unknown,"missing transfer remains unknown instead of guessing");
  for(auto quality:{Quality::Quality,Quality::Balanced,Quality::Performance,Quality::NativeAA})
   for(auto policy:{ProviderPolicy::Analytical,ProviderPolicy::Compatible}) {
    FsrSettings settings{quality,policy,0.42f,ColorEncoding::SRGB};StoreFsrSettings(ini,settings);
    const auto loaded=ReadFsrSettings(ini);Require(loaded && *loaded==settings,"FSR settings round trip");
  }
  for(auto encoding:{ColorEncoding::Unknown,ColorEncoding::Linear,ColorEncoding::Gamma22,ColorEncoding::SRGB}) {
    FsrSettings settings{Quality::Quality,ProviderPolicy::Analytical,0,encoding};StoreFsrSettings(ini,settings);
    Require(ReadFsrSettings(ini) && ReadFsrSettings(ini)->sourceColorEncoding==encoding,"explicit color encoding persists independently of format");
  }
  ini.SetValue("FSR","SourceColorEncoding","Guess");Require(!ReadFsrSettings(ini),"unknown encoding spelling rejected");
  ini.SetValue("FSR","SourceColorEncoding","Gamma22");
  ini.SetValue("FSR","Sharpness","nan");Require(!ReadFsrSettings(ini),"nonfinite value rejected");
  ini.SetValue("FSR","Sharpness","0.4junk");Require(!ReadFsrSettings(ini),"partial numeric value rejected");
  ini.SetValue("FSR","Sharpness","0.4");ini.SetValue("FSR","Quality","UltraPerformance");Require(!ReadFsrSettings(ini),"unknown quality rejected");
  BackendConfiguration old,next;next.backend=BackendKind::Fsr;next.generationEnabled=false;next.generationBackend=0;
  Require(FsrChangeRequiresRestart(old,next),"backend change requires restart");
  BackendDecision active{BackendKind::Dlss,PresentationKind::Nvidia,true,false,{}};
  Require(DescribeFsrStatus(next,active,nullptr,true,nullptr).kind==SettingsStatusKind::Pending,"request is not active");
  old=next;next.quality=Quality::NativeAA;Require(FsrChangeRequiresRestart(old,next),"quality changes allocation");
  next=old;next.providerPolicy=ProviderPolicy::Compatible;Require(FsrChangeRequiresRestart(old,next),"provider change needs restart");
  next=old;next.sharpness=.8f;Require(!FsrChangeRequiresRestart(old,next),"sharpness can change live");
  active={BackendKind::Fsr,PresentationKind::Ordinary,false,false,"Waiting for first temporal frame"};
  Require(DescribeFsrStatus(next,active,nullptr,false,nullptr).kind!=SettingsStatusKind::Success,"uninitialized is not active");
  active.valid=true;ProviderInfo provider{42,"test provider"};Require(DescribeFsrStatus(next,active,&provider,false,nullptr).kind==SettingsStatusKind::Success,"successful temporal frame is active");
  RuntimeError error{ErrorKind::DispatchFailure,1,"spatial recovery until restart"};Require(DescribeFsrStatus(next,active,&provider,false,&error).kind==SettingsStatusKind::Error,"failure overrides stale active state");
  RendererSettingsDraft draft;draft.valid=true;draft.upscaleType=FSR;draft.generationEnabled=false;draft.generationBackend=0;draft.sourceDLSSG.neuralEnabled=false;draft.sourceDLSSG.hdrOutput.enabled=false;
  RendererSettingsCapabilities caps{true,false,true,false,true,true};
  const auto original=draft;
  Require(ValidateRendererSettings(draft,caps),"FSR cannot apply an unknown source encoding");
  draft.fsr.sourceColorEncoding=ColorEncoding::Gamma22;
  const auto known=draft;
  Require(!ValidateRendererSettings(draft,caps),"FSR coherent request accepted");
  draft.generationEnabled=true;Require(ValidateRendererSettings(draft,caps),"FSR generation explicitly unavailable");
  draft=known;draft.sourceDLSSG.neuralEnabled=true;Require(ValidateRendererSettings(draft,caps),"FSR NR unavailable");
  draft=known;draft.sourceDLSSG.hdrOutput.enabled=true;Require(ValidateRendererSettings(draft,caps),"FSR HDR unavailable");
  draft=known;draft.dynamicResolution=true;Require(ValidateRendererSettings(draft,caps),"FSR dynamic resolution unavailable");
  draft=original;draft.fsr.quality=Quality::Performance;Require(CountRendererSettingsChanges(draft,original)==1,"one draft tracks FSR edits");
  draft=original;Require(CountRendererSettingsChanges(draft,original)==0,"discard restores original draft");
  Upscaler::Configuration configuration;Upscaler::Creation startup{4};configuration.Initialize(startup);configuration.BeginSubmission();configuration.Completed(true);
  auto requested=startup;requested.fsr.quality=Quality::Performance;configuration.Request(requested);
  Require(configuration.NeedsRestart() && configuration.Effective()==startup,"Apply retains active allocation");
  configuration.Saved();Require(configuration.Persisted()==requested,"Save stores requested allocation");
  requested.fsr.sharpness=.6f;configuration.Request(requested);Require(configuration.LiveCandidate().fsr.quality==startup.fsr.quality,"live candidate retains startup quality");
  configuration.Initialize(startup);configuration.BeginSubmission();configuration.Completed(true);requested=startup;requested.fsr.sourceColorEncoding=ColorEncoding::SRGB;configuration.Request(requested);
  Require(configuration.NeedsRestart() && configuration.LiveCandidate().fsr.sourceColorEncoding==startup.fsr.sourceColorEncoding,
      "encoding changes require restart and cannot alter an active frame adapter");
  if(argc==3){CSimpleIniA legacy,example;Require(legacy.LoadFile(argv[1])>=0 && example.LoadFile(argv[2])>=0,"package INIs readable");Require(legacy.GetLongValue("Settings","UpscaleType",-1)==0,"default NVIDIA selection preserved");Require(!ValidateRendererConfiguration(example,true) && ReadFsrSettings(example),"example is valid FSR only");}
  std::cout<<"PASS: SettingsLifecycle UnsupportedCombinationReason RequestedIsNotActive\n";
  return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
