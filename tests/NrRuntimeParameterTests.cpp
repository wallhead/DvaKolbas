#include "NeuralRendering/RuntimeParameters.h"
#include "FrameGen/NeuralRenderingRuntimeContract.h"
#include <map>
#include <variant>
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
struct Parameters {
    std::map<std::string,std::variant<int,unsigned,float>> values;
    template<class T>void Set(const char* key,T v){values[key]=v;}
};
int main(){
    int failed{};const auto check=[&](bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);if(!ok)++failed;};
    Parameters p;DirectCreationContract c{"rtx40",{640,360},{640,360},0};
    check(bool(WriteDirectCreationParameters(p,c)),"PinnedDirectProfileCreationAccepted");
    const auto typed=[&]<class T>(const char* key,T expected){const auto it=p.values.find(key);return it!=p.values.end()&&std::holds_alternative<T>(it->second)&&std::get<T>(it->second)==expected;};
    check(typed("DLSSNR.Hint.Render.Preset",0u),"DirectPresetIsUnsignedNotLegacyInt");
    check(typed("PerfQualityValue",2u),"DirectQualityIsUnsigned");
    check(typed("DLSSNR.Upscaling",0),"DirectUpscalingIsSignedInt");
    check(typed("DLSS.Feature.Create.Flags",0x42),"DirectFlagsAreSignedInt");
    check(typed("DLSSNR.ScalingRatio",1.f),"DirectScaleUsesMeasuredNativePolicy");
    check(typed("DLSSNR.Output.Width",640u)&&typed("DLSSNR.InputHeight",360u),"AllObservedCreationExtentAliasesWritten");
    auto bad=c;bad.profileId="legacy";Parameters untouched;check(!WriteDirectCreationParameters(untouched,bad)&&untouched.values.empty(),"LegacyNameCannotSelectCommunityDirectPolicy");
    bad=c;bad.preset=99;check(!WriteDirectCreationParameters(untouched,bad)&&untouched.values.empty(),"BadPresetRejectedBeforeParameterMutation");
    bad=c;bad.guideExtent.width=0;check(!WriteDirectCreationParameters(untouched,bad)&&untouched.values.empty(),"MissingGuideExtentRejectedBeforeMutation");
    bad=c;bad.colorExtent.width=UINT32_MAX;check(!WriteDirectCreationParameters(untouched,bad)&&untouched.values.empty(),"UnboundedExtentRejectedBeforeMutation");
    Parameters legacy;WriteCreationParameters(legacy,MakeFeatureContract(RuntimeBuild::Legacy,640,360,320,180));
    check(std::get<int>(legacy.values.at("DLSSNR.Hint.Render.Preset"))==1&&std::get<float>(legacy.values.at("DLSSNR.ScalingRatio"))==.5f,"ExistingLegacyPresetAndScaleRemainIntact");
    return failed?1:0;
}
