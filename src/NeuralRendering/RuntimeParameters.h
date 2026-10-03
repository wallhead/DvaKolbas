#pragma once
#include "ImagePacket.h"
#include <array>
namespace TheosRenderPipeline::NeuralRendering {
inline constexpr uint64_t kDirectNrAppId=0x0876232c;
inline constexpr uint32_t kDirectNrApiVersion=0x15,kDirectNrFeatureId=0x12;
struct DirectCreationContract {
    std::string_view profileId;
    ImageExtent colorExtent,guideExtent;
    unsigned preset{};
};
template<class Parameters>Result<void> WriteDirectCreationParameters(Parameters& p,const DirectCreationContract& c){
    bool known=false;for(const auto& profile:RuntimeCatalog())known|=profile.id==c.profileId;
    if(!known || !c.colorExtent.width || !c.colorExtent.height || c.colorExtent.width>16384 ||
        c.colorExtent.height>16384 || c.guideExtent!=c.colorExtent || c.preset>1)
        return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR direct profile/preset/native guide extent invalid"});
    for(const char* k:{"Width","OutWidth","DLSSNR.Width","DLSSNR.InputWidth","DLSSNR.OutputWidth","DLSSNR.Output.Width"})p.Set(k,c.colorExtent.width);
    for(const char* k:{"Height","OutHeight","DLSSNR.Height","DLSSNR.InputHeight","DLSSNR.OutputHeight","DLSSNR.Output.Height"})p.Set(k,c.colorExtent.height);
    p.Set("DLSSNR.ScalingRatio",1.f);p.Set("DLSSNR.Scale",1.f);
    p.Set("DLSSNR.Hint.Render.Preset",c.preset);p.Set("PerfQualityValue",2u);
    p.Set("DLSSNR.Upscaling",0);p.Set("DLSS.Feature.Create.Flags",0x42);
    p.Set("CreationNodeMask",1u);p.Set("VisibilityNodeMask",1u);return {};
}
}
