#pragma once
#include "UpscalerBackend.h"
#include "FSRColorContract.h"
#include <string_view>
namespace TheosRenderPipeline::Upscaling {
struct XessSettings {
    Quality quality{Quality::NativeAA};
    ColorEncoding sourceEncoding{ColorEncoding::Gamma22};
    bool operator==(const XessSettings&)const=default;
};
inline const char* XessQualityName(Quality value) {
    switch(value){case Quality::NativeAA:return "NativeAA";case Quality::Quality:return "Quality";case Quality::Balanced:return "Balanced";case Quality::Performance:return "Performance";}
    return "Invalid";
}
inline bool ValidXessSettings(const XessSettings& value) {
    return value.quality>=Quality::Quality && value.quality<=Quality::NativeAA && IsKnownColorEncoding(value.sourceEncoding);
}
template<class Ini> Result<XessSettings> ReadXessSettings(const Ini& ini) {
    XessSettings value;bool found{};
    const std::string_view quality=ini.GetValue("XeSS","Quality","NativeAA");
    for(auto option:{Quality::NativeAA,Quality::Quality,Quality::Balanced,Quality::Performance})
        if(quality==XessQualityName(option)){value.quality=option;found=true;break;}
    if(!found)return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,"[XeSS] Quality must be Native, Quality, Balanced or Performance."});
    found=false;const std::string_view encoding=ini.GetValue("XeSS","SourceColorEncoding","Gamma22");
    for(auto option:{ColorEncoding::Linear,ColorEncoding::Gamma22,ColorEncoding::SRGB})
        if(encoding==ColorEncodingName(option)){value.sourceEncoding=option;found=true;break;}
    if(!found)return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,"[XeSS] SourceColorEncoding must be Linear, Gamma22 or SRGB."});
    return value;
}
template<class Ini> void StoreXessSettings(Ini& ini,const XessSettings& value) {
    ini.SetValue("XeSS","Quality",XessQualityName(value.quality));
    ini.SetValue("XeSS","SourceColorEncoding",ColorEncodingName(value.sourceEncoding));
}
}
