#pragma once
#include "IniLayout.h"
#include "Error.h"
#include "Upscaling/FSRColorContract.h"
#include <filesystem>
namespace TheosRenderPipeline::NeuralRendering {
struct StartupSettings {
    bool community{};
    // Opt-in comparison with the user-confirmed stable Raz SDR byte contract.
    bool sdrBytesTrial{};
    std::string profile{"Auto"};
    std::filesystem::path runtimeRoot,driverCore;
    Upscaling::ColorEncoding sourceEncoding{Upscaling::ColorEncoding::Unknown};
    // Called after rendering-device creation. Missing OS DriverStore overrides
    // from older packages use the active driver; custom/existing overrides stay
    // explicit. Discovery never skips the subsequent driver-core trust checks.
    Result<std::filesystem::path> ResolveDriverCore() const;
    StartupSettings Resolve(const std::filesystem::path& pluginRoot)const{
        auto result=*this;result.runtimeRoot=runtimeRoot.empty()?pluginRoot:runtimeRoot.is_absolute()?runtimeRoot:pluginRoot/runtimeRoot;
        if(!result.driverCore.empty()&&!result.driverCore.is_absolute())result.driverCore=pluginRoot/result.driverCore;
        return result;
    }
};
template<class Ini> StartupSettings LoadStartupSettings(const Ini& source){
    const IniLayout::ReadView ini(source);
    StartupSettings result;result.community=ini.GetBoolValue("NeuralRendering","CommunityRuntime",false);
    result.sdrBytesTrial=ini.GetBoolValue("NeuralRendering","SdrBytesTrial",false);
    result.profile=ini.GetValue("NeuralRendering","Profile","Auto");
    result.runtimeRoot=ini.GetValue("NeuralRendering","RuntimeRoot","");result.driverCore=ini.GetValue("NeuralRendering","DriverCore","");
    const std::string_view encoding=ini.GetValue("NeuralRendering","SourceColorEncoding","Unknown");
    for(auto value:{Upscaling::ColorEncoding::Linear,Upscaling::ColorEncoding::Gamma22,Upscaling::ColorEncoding::SRGB})
        if(encoding==Upscaling::ColorEncodingName(value))result.sourceEncoding=value;
    return result;
}
}
