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
inline bool ValidDirectExtents(ImageExtent color,ImageExtent guides){
    return color.width && color.height && color.width<=16384 && color.height<=16384 &&
        guides.width && guides.height && guides.width<=color.width && guides.height<=color.height;
}
template<class Parameters>Result<void> WriteDirectSubrectParameters(Parameters& p,ImageExtent color,ImageExtent guides){
    if(!ValidDirectExtents(color,guides))
        return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR direct color/guide subrect extent invalid"});
    for(const char* plane:{"Color","MVec","Depth","Output"}){
        const auto extent=std::string_view(plane)=="MVec"||std::string_view(plane)=="Depth"?guides:color;
        const auto prefix=std::string("DLSSNR.")+plane+"Subrect";
        p.Set((prefix+"BaseX").c_str(),0u);p.Set((prefix+"BaseY").c_str(),0u);
        p.Set((prefix+"Width").c_str(),extent.width);p.Set((prefix+"Height").c_str(),extent.height);
    }
    return {};
}
template<class Parameters>Result<void> WriteDirectCreationParameters(Parameters& p,const DirectCreationContract& c){
    bool known=false;for(const auto& profile:RuntimeCatalog())known|=profile.id==c.profileId;
    if(!known || !ValidDirectExtents(c.colorExtent,c.guideExtent) || c.preset>1)
        return std::unexpected(Error{ErrorKind::InvalidInput,0,"NR direct profile/preset/color guide extent invalid"});
    for(const char* k:{"Width","OutWidth","DLSSNR.Width","DLSSNR.InputWidth","DLSSNR.OutputWidth","DLSSNR.Output.Width"})p.Set(k,c.colorExtent.width);
    for(const char* k:{"Height","OutHeight","DLSSNR.Height","DLSSNR.InputHeight","DLSSNR.OutputHeight","DLSSNR.Output.Height"})p.Set(k,c.colorExtent.height);
    p.Set("DLSSNR.ScalingRatio",1.f);p.Set("DLSSNR.Scale",1.f);
    p.Set("DLSSNR.Hint.Render.Preset",c.preset);p.Set("PerfQualityValue",2u);
    p.Set("DLSSNR.Upscaling",0);p.Set("DLSS.Feature.Create.Flags",0x42);
    p.Set("CreationNodeMask",1u);p.Set("VisibilityNodeMask",1u);return {};
}
}
