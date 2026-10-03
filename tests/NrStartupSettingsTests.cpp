#include "NeuralRendering/StartupSettings.h"
#include <map>
#include <string>
#include <cstdio>
struct Ini {std::map<std::string,std::string> v;
const char* GetValue(const char*,const char* key,const char* fallback)const{auto i=v.find(key);return i==v.end()?fallback:i->second.c_str();}
bool GetBoolValue(const char*,const char* key,bool fallback)const{auto i=v.find(key);return i==v.end()?fallback:i->second=="true";}
};
int main(){using namespace TheosRenderPipeline::NeuralRendering;Ini i;auto s=LoadStartupSettings(i);int failed{};
auto check=[&](bool v,const char* n){std::printf("%s %s\n",v?"PASS":"FAIL",n);failed+=!v;};
check(!s.community&&s.profile=="Auto"&&s.sourceEncoding==TheosRenderPipeline::Upscaling::ColorEncoding::Unknown,"OldIniKeepsLegacyNrAndUnknownDomain");
i.v={{"CommunityRuntime","true"},{"Profile","rtx40"},{"DriverCore","C:/pinned/_nvngx.dll"},{"SourceColorEncoding","Gamma22"}};s=LoadStartupSettings(i);
check(s.community&&s.profile=="rtx40"&&s.sourceEncoding==TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,"ExplicitCommunityStartupParsed");
check(s.Resolve("C:/plugin").runtimeRoot==std::filesystem::path("C:/plugin"),"CatalogPathsResolveFromControlledPluginRoot");
i.v["SourceColorEncoding"]="garbage";check(LoadStartupSettings(i).sourceEncoding==TheosRenderPipeline::Upscaling::ColorEncoding::Unknown,"InvalidEncodingCannotGuessGamma");
return failed?1:0;}
