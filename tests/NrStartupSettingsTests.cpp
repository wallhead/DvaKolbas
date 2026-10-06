#include "NeuralRendering/StartupSettings.h"
#include "NeuralRendering/Error.h"
#include <map>
#include <string>
#include <cstdio>
#include <Windows.h>
struct Ini {std::map<std::string,std::string> v;
const char* GetValue(const char*,const char* key,const char* fallback)const{auto i=v.find(key);return i==v.end()?fallback:i->second.c_str();}
bool GetBoolValue(const char*,const char* key,bool fallback)const{auto i=v.find(key);return i==v.end()?fallback:i->second=="true";}
};
int main(){using namespace TheosRenderPipeline::NeuralRendering;Ini i;auto s=LoadStartupSettings(i);int failed{};
auto check=[&](bool v,const char* n){std::printf("%s %s\n",v?"PASS":"FAIL",n);failed+=!v;};
check(!s.community&&s.profile=="Auto"&&s.sourceEncoding==TheosRenderPipeline::Upscaling::ColorEncoding::Unknown,"OldIniKeepsLegacyNrAndUnknownDomain");
check(!s.sdrBytesTrial,"SdrByteTrialIsOptIn");
i.v={{"CommunityRuntime","true"},{"Profile","rtx40"},{"DriverCore","C:/pinned/_nvngx.dll"},{"SourceColorEncoding","Gamma22"}};s=LoadStartupSettings(i);
check(s.community&&s.profile=="rtx40"&&s.sourceEncoding==TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,"ExplicitCommunityStartupParsed");
i.v["SdrBytesTrial"]="true";check(LoadStartupSettings(i).sdrBytesTrial,"ExplicitSdrByteTrialParsed");
check(s.Resolve("C:/plugin").runtimeRoot==std::filesystem::path("C:/plugin"),"CatalogPathsResolveFromControlledPluginRoot");
auto core=s.Resolve("C:/plugin").ResolveDriverCore();
check(core&&*core==std::filesystem::path("C:/pinned/_nvngx.dll"),"ExplicitCoreOverrideIsPreserved");
auto automatic=s;automatic.driverCore.clear();core=automatic.ResolveDriverCore();
check(!core&&core.error().nativeCode!=0,"DiscoveryWithoutLoadedNvidiaDriverReportsNativeError");
automatic.driverCore="relative/_nvngx.dll";core=automatic.ResolveDriverCore();
check(!core&&core.error().kind==ErrorKind::InvalidInput,"UnresolvedRelativeCoreOverrideCannotReachLoader");
std::wstring windows(32768,L'\0');auto length=GetWindowsDirectoryW(windows.data(),static_cast<UINT>(windows.size()));
check(length&&length<windows.size(),"WindowsDirectoryAvailableForMissingDriverStoreFixture");windows.resize(length);
automatic.driverCore=std::filesystem::path(windows)/L"System32/DriverStore/FileRepository"/
    (L"raz-missing-"+std::to_wstring(GetCurrentProcessId()))/L"_nvngx.dll";
check(!std::filesystem::exists(automatic.driverCore),"MissingDriverStoreFixtureIsAbsent");
core=automatic.ResolveDriverCore();
check(!core&&core.error().kind==ErrorKind::Unsupported&&core.error().message.find("missing DriverStore override")!=std::string::npos,
    "MissingDriverStoreOverrideUsesDiscoveryAndReportsRecoveryContext");
i.v["SourceColorEncoding"]="garbage";check(LoadStartupSettings(i).sourceEncoding==TheosRenderPipeline::Upscaling::ColorEncoding::Unknown,"InvalidEncodingCannotGuessGamma");
const auto fixture=std::filesystem::temp_directory_path()/(L"RazNrRoot-"+std::to_wstring(GetCurrentProcessId()));
const auto renamed=fixture/L"RaZkolbaS";
std::filesystem::create_directories(renamed/L"NR");
StartupSettings legacy;legacy.runtimeRoot="TheosRenderPipeline";
check(legacy.Resolve(renamed).runtimeRoot==renamed,"LegacyCommunityRootRecoversRenamedResources");
legacy.runtimeRoot="TheosRenderPipeline/NR";
check(legacy.Resolve(renamed).runtimeRoot==renamed/L"NR","LegacyCommunitySubdirectoryRecoversRenamedResources");
std::filesystem::create_directories(fixture/L"TheosRenderPipeline/NR");
check(legacy.Resolve(renamed).runtimeRoot==fixture/L"TheosRenderPipeline/NR","ExistingLegacyCommunityRootIsPreserved");
legacy.runtimeRoot=fixture/L"TheosRenderPipeline";
check(legacy.Resolve(renamed).runtimeRoot==legacy.runtimeRoot,"AbsoluteCommunityOverrideIsPreserved");
legacy.runtimeRoot="custom";
check(legacy.Resolve(renamed).runtimeRoot==renamed/L"custom","CustomRelativeCommunityOverrideIsPreserved");
std::filesystem::remove(fixture/L"TheosRenderPipeline/NR");
std::filesystem::remove(fixture/L"TheosRenderPipeline");
legacy.runtimeRoot="TheosRenderPipeline/missing";
check(legacy.Resolve(renamed).runtimeRoot==renamed/L"TheosRenderPipeline/missing","MissingRenamedCommunityResourcesDoNotInventFallback");
std::filesystem::remove(renamed/L"NR");std::filesystem::remove(renamed);std::filesystem::remove(fixture);
return failed?1:0;}
