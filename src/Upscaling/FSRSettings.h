#pragma once
#include "UpscalerBackend.h"
#include "FSRColorContract.h"
#include "RendererBackendPolicy.h"
#include "RendererSettingsAction.h"
#include <charconv>
#include <cmath>
#include <string_view>
namespace TheosRenderPipeline::Upscaling {
struct FsrSettings {
    Quality quality{Quality::Quality};
    ProviderPolicy providerPolicy{ProviderPolicy::Analytical};
    float sharpness{};
    ColorEncoding sourceColorEncoding{ColorEncoding::Unknown};
    bool operator==(const FsrSettings&) const = default;
};
inline const char* QualityName(Quality value) {
    switch(value){case Quality::Quality:return "Quality";case Quality::Balanced:return "Balanced";case Quality::Performance:return "Performance";case Quality::NativeAA:return "NativeAA";}
    return "Invalid";
}
inline const char* ProviderPolicyName(ProviderPolicy value){return value==ProviderPolicy::Analytical?"Analytical":value==ProviderPolicy::Compatible?"Compatible":value==ProviderPolicy::MachineLearning?"MachineLearning":"Invalid";}
inline bool ValidFsrSettings(const FsrSettings& value) {
    return value.quality>=Quality::Quality && value.quality<=Quality::NativeAA &&
        ValidProviderPolicy(value.providerPolicy) &&
        std::isfinite(value.sharpness) && value.sharpness>=0 && value.sharpness<=1 &&
        (value.sourceColorEncoding==ColorEncoding::Unknown || IsKnownColorEncoding(value.sourceColorEncoding));
}
template<class Ini> Result<FsrSettings> ReadFsrSettings(const Ini& ini) {
    auto invalid=[](const char* reason)->Result<FsrSettings>{return std::unexpected(RuntimeError{ErrorKind::InvalidInput,0,reason});};
    FsrSettings result;
    const std::string_view quality=ini.GetValue("FSR","Quality","Quality");
    bool found=false;
    for(auto value:{Quality::Quality,Quality::Balanced,Quality::Performance,Quality::NativeAA})if(quality==QualityName(value)){result.quality=value;found=true;break;}
    if(!found)return invalid("[FSR] Quality must be Quality, Balanced, Performance or NativeAA.");
    const std::string_view policy=ini.GetValue("FSR","ProviderPolicy","Analytical");
    if(policy=="Analytical")result.providerPolicy=ProviderPolicy::Analytical;
    else if(policy=="Compatible")result.providerPolicy=ProviderPolicy::Compatible;
    else if(policy=="MachineLearning")result.providerPolicy=ProviderPolicy::MachineLearning;
    else return invalid("[FSR] ProviderPolicy must be Analytical, Compatible (Auto) or MachineLearning (FSR4).");
    const std::string_view sharpness=ini.GetValue("FSR","Sharpness","0");
    const auto parsed=std::from_chars(sharpness.data(),sharpness.data()+sharpness.size(),result.sharpness);
    if(parsed.ec!=std::errc{} || parsed.ptr!=sharpness.data()+sharpness.size() || !ValidFsrSettings(result))return invalid("[FSR] Sharpness must be a finite number from 0 to 1.");
    const std::string_view encoding=ini.GetValue("FSR","SourceColorEncoding","Unknown");
    found=false;
    for(auto value:{ColorEncoding::Unknown,ColorEncoding::Linear,ColorEncoding::Gamma22,ColorEncoding::SRGB})
        if(encoding==ColorEncodingName(value)){result.sourceColorEncoding=value;found=true;break;}
    if(!found)return invalid("[FSR] SourceColorEncoding must be Unknown, Linear, Gamma22 or SRGB.");
    return result;
}
template<class Ini> void StoreFsrSettings(Ini& ini,const FsrSettings& value) {
    ini.SetValue("FSR","Quality",QualityName(value.quality));
    ini.SetValue("FSR","ProviderPolicy",ProviderPolicyName(value.providerPolicy));
    ini.SetDoubleValue("FSR","Sharpness",value.sharpness);
    ini.SetValue("FSR","SourceColorEncoding",ColorEncodingName(value.sourceColorEncoding));
}
inline bool FsrChangeRequiresRestart(const BackendConfiguration& old,const BackendConfiguration& next) {
    return old.backend!=next.backend || old.quality!=next.quality || old.providerPolicy!=next.providerPolicy ||
        old.enabled!=next.enabled || old.generationBackend!=next.generationBackend || old.hdr!=next.hdr || old.dynamicResolution!=next.dynamicResolution;
}
inline SettingsActionStatus DescribeFsrStatus(const BackendConfiguration& requested,const BackendDecision& active,const ProviderInfo* provider,bool restartRequired,const RuntimeError* error) {
    if(error)return {error->message,SettingsStatusKind::Error};
    if(restartRequired)return {"Upscaler allocation awaiting restart; requested settings are not active.",SettingsStatusKind::Pending};
    if(requested.backend!=BackendKind::Fsr)return {"FSR is not requested.",SettingsStatusKind::Neutral};
    if(active.backend!=BackendKind::Fsr || !active.valid || !provider)return {active.diagnostic.empty()?"FSR requested; waiting for a successful temporal frame.":active.diagnostic,SettingsStatusKind::Pending};
    return {"FSR active | "+provider->name+(active.presentation==PresentationKind::Fsr?" | AMD presenter":" | ordinary presentation"),SettingsStatusKind::Success};
}
}
