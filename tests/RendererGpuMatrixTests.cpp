#include <SimpleIni.h>
#include "RendererGpuSupport.h"
#include "RendererStartupValidation.h"
#include "PublicIni.h"
#include "FrameGen/SourceFrameGeneration.h"
#include <cstdio>
#include <array>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>
using namespace TheosRenderPipeline;
namespace NR=TheosRenderPipeline::NeuralRendering;
int failures{},routes{};
void Check(bool yes,const char* message){if(!yes){std::printf("FAIL %s\n",message);++failures;}}
struct Card {const wchar_t* name;uint32_t vendor,device,arch;bool rtx;bool requiredD3d12{true};};
// Synthetic IDs exercise unknown/laptop fallback; known desktop IDs and GTX1070
// are explicit regressions. Architecture and feature support are injected facts,
// not claims of physical qualification for each named model.
const Card cards[]{
 {L"NVIDIA TITAN RTX",0x10de,0x1e02,0x160,true},
 {L"Quadro RTX 8000",0x10de,0xde10,0x160,true},
 {L"NVIDIA GeForce RTX 2080",0x10de,0x1e82,0x160,true},
 {L"NVIDIA GeForce RTX 2060 Laptop GPU",0x10de,0xde01,0x160,true},
 {L"NVIDIA GeForce RTX 3080",0x10de,0x2206,0x170,true},
 {L"NVIDIA GeForce RTX 3050 Laptop GPU",0x10de,0xde02,0x170,true},
 {L"NVIDIA GeForce RTX 4080",0x10de,0x2704,0x190,true},
 {L"NVIDIA GeForce RTX 4060 Laptop GPU",0x10de,0xde03,0x190,true},
 {L"NVIDIA GeForce RTX 5090",0x10de,0x2b85,0x1b0,true},
 {L"NVIDIA GeForce RTX 5090 Laptop GPU",0x10de,0xde04,0x1b0,true},
 {L"NVIDIA GeForce GTX 1070",0x10de,0x1b81,0x130,false},
 {L"NVIDIA GeForce GTX 1060",0x10de,0xde05,0x130,false},
 {L"NVIDIA GeForce GTX 1050 Ti Laptop GPU",0x10de,0xde06,0x130,false},
 {L"NVIDIA GeForce GTX 1650 Laptop GPU",0x10de,0xde07,0x160,false},
 {L"NVIDIA GeForce GTX 1660 SUPER",0x10de,0xde08,0x160,false},
 {L"NVIDIA GeForce GTX 980",0x10de,0xde09,0x120,false},
 {L"NVIDIA GeForce GTX 780",0x10de,0xde0a,0xf0,false,false},
 {L"NVIDIA GeForce GTX 750 Ti",0x10de,0xde0e,0x110,false,false},
 {L"NVIDIA GeForce GTX 480",0x10de,0xde0f,0xc0,false,false},
 {L"NVIDIA GeForce MX150",0x10de,0xde0b,0x130,false},
 {L"NVIDIA Quadro P2000",0x10de,0xde0c,0x130,false},
 {L"NVIDIA RTX A4000",0x10de,0xde0d,0x170,true},
 {L"AMD Radeon RX 9070",0x1002,0xad01,0,false},
 {L"AMD Radeon RX 7900",0x1002,0xad02,0,false},
 {L"AMD Radeon RX 6700",0x1002,0xad03,0,false},
 {L"AMD Radeon RX 5700",0x1002,0xad04,0,false},
 {L"AMD Radeon RX 590",0x1002,0xad05,0,false},
 {L"AMD Radeon 780M",0x1002,0xad06,0,false},
 {L"Intel Arc A770",0x8086,0xac01,0,false},
 {L"Intel Iris Xe",0x8086,0xac02,0,false}};
int main(int argc,char** argv){
 if(argc<2 || argc>3)return 1;
 for(const auto& card:cards){
  NR::AdapterIdentity adapter{card.vendor,card.device,0,{123,4},false};
  if(card.vendor==0x10de)adapter.architecture={adapter.luid,card.arch,card.rtx,true};
  const auto expected=!card.requiredD3d12?RendererGpuChoice::Unsupported:card.rtx?RendererGpuChoice::NvidiaRtx:RendererGpuChoice::FsrOnly;
  for(bool nvapi:{true,false}){
   auto observation=adapter;if(!nvapi)observation.architecture={};
   const auto choice=ClassifyRendererGpu(observation,card.name,true,card.requiredD3d12);
   Check(choice==expected,"GPU policy uses actual architecture/product and DXGI fallback");
   if(choice==RendererGpuChoice::Unsupported)continue;
   for(const auto quality:{"Native","Quality","Performance"})
   for(int scenario=0;scenario<4;++scenario){
    CSimpleIniA ini;Check(ini.LoadFile(argv[1])>=0,"package loads");
    ini.SetValue("FSR","Quality",quality);
    if(scenario==1){ini.SetValue("FrameGeneration","Backend","NVIDIA");ini.SetBoolValue("FrameGeneration","Enabled",true);ini.SetBoolValue("NeuralRendering","Enabled",true);}
    if(scenario==2){ini.SetValue("Upscaling","Upscaler","FSR");ini.SetValue("FrameGeneration","Backend","FSR");ini.SetBoolValue("FrameGeneration","Enabled",true);}
    if(scenario==3){ini.SetValue("Upscaling","Upscaler","FSR");ini.SetValue("FSR","Provider","FSR4");ini.SetValue("FrameGeneration","FsrProvider","FSR4");}
    Check(PublicIni::Decode(ini).empty(),"named settings decode");
    const bool only=choice==RendererGpuChoice::FsrOnly;
    Check(ValidateRendererStartup(ini,card.vendor,true,true,only).empty(),"in-memory startup normalization admits supported route");
    auto& prefs=*SourceFrameGeneration::GetSingleton();prefs.LoadStartupPreferences(ini);
    if(only){Check(ini.GetLongValue("Settings","UpscaleType",-1)==FSR&&prefs.settings.generationBackend==2&&!prefs.settings.sourceDLSSG.neuralEnabled,"FSR-only devices cannot enter NVIDIA/NR owner");}
    if(only&&card.vendor==0x10de){Check(!prefs.settings.sourceDLSSGMFGUnlock,"non-RTX avoids NVIDIA compatibility probing");Check(std::string_view(ini.GetValue("FSR","ProviderPolicy",""))=="Analytical","non-RTX retains qualified FSR3 baseline");}
    if(scenario==0)Check(!prefs.settings.enabled,"default FG remains off");
    if(scenario==2&&only)Check(prefs.settings.enabled,"explicit FSR FG request remains explicit");
    if(scenario==1&&only)Check(!prefs.settings.enabled,"saved NVIDIA FG never silently becomes enabled FSR FG");
    if(only){
     const auto setting=Upscaling::ReadFsrSettings(ini);
     Check(setting && std::string_view(Upscaling::QualityName(setting->quality))==(std::string_view(quality)=="Native"?"NativeAA":quality),"FSR fallback preserves selected render scale");
    }
    ++routes;
   }
  }
  Check(ClassifyRendererGpu(adapter,card.name,true,false)==RendererGpuChoice::Unsupported,"no D3D12 yields unsupported hardware");
  Check(ClassifyRendererGpu(adapter,card.name,false,true)==RendererGpuChoice::Unsupported,"no required D3D11 feature level yields unsupported hardware");
  adapter.software=true;
  Check(ClassifyRendererGpu(adapter,card.name)==RendererGpuChoice::Unsupported,"software adapter rejected");
  std::printf("CASE vendor=0x%04X arch=0x%X RTX=%d expected=%s\n",card.vendor,card.arch,card.rtx,!card.requiredD3d12?"Unsupported-required-D3D12":card.rtx?"NVIDIA":"FSR-only");
 }
 NR::AdapterIdentity unknown{0x10de,0xffff,0,{123,4},false};
 Check(ClassifyRendererGpu(unknown,L"")==RendererGpuChoice::FsrOnly,"unidentified NVIDIA cannot default into RTX-only owner");
 unknown.architecture={{456,7},0x190,true,true};
 Check(ClassifyRendererGpu(unknown,L"NVIDIA GeForce RTX 4090")==RendererGpuChoice::FsrOnly,"foreign architecture evidence does not grant RTX ownership");
 unknown.luid={};Check(ClassifyRendererGpu(unknown,L"RTX 4090")==RendererGpuChoice::Unsupported,"missing renderer identity rejected");
 std::printf("RESULT cards=%zu simulated_routes=%d failures=%d; no physical vendor runtime creation\n",std::size(cards),routes,failures);
 if(argc==3 && std::string_view(argv[2])=="--live") {
  Microsoft::WRL::ComPtr<IDXGIFactory1> factory;
  Check(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))),"live DXGI factory");
  int hardware=0;
  if(factory)for(UINT i=0;;++i){
   Microsoft::WRL::ComPtr<IDXGIAdapter1> gpu;
   if(factory->EnumAdapters1(i,&gpu)==DXGI_ERROR_NOT_FOUND)break;
   DXGI_ADAPTER_DESC1 desc{};
   if(!gpu || FAILED(gpu->GetDesc1(&desc)) || (desc.Flags&DXGI_ADAPTER_FLAG_SOFTWARE))continue;
   ++hardware;
   NR::AdapterIdentity actual{desc.VendorId,desc.DeviceId,desc.SubSysId,{desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart},false};
   NR::DiscoverGpuArchitecture(actual);
   const auto support=D3D12CreateDevice(gpu.Get(),D3D_FEATURE_LEVEL_12_0,__uuidof(ID3D12Device),nullptr);
   const auto route=ClassifyRendererGpu(actual,desc.Description,true,SUCCEEDED(support));
   std::printf("LIVE name=%ls vendor=0x%X device=0x%X arch=0x%X queried=%d RTX=%d D3D12=0x%X route=%d; capability probe only\n",desc.Description,actual.vendorId,actual.deviceId,actual.architecture.id,actual.architecture.queried,actual.architecture.rtxProduct,static_cast<unsigned>(support),static_cast<int>(route));
   Check(route!=RendererGpuChoice::Unsupported,"local hardware meets baseline");
   if(actual.architecture.queried && actual.architecture.rtxProduct)Check(route==RendererGpuChoice::NvidiaRtx,"actual RTX retains NVIDIA route");
  }
  Check(hardware>0,"live hardware adapter present");
 }
 return failures?1:0;
}
