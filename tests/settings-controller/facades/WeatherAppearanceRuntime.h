#pragma once
#include "WeatherAppearance.h"
namespace TheosRenderPipeline::Appearance {
class Runtime {
    Settings settings_;
public:
    static Runtime& Get(){static Runtime v;return v;}
    Settings Configuration()const{return settings_;}
    void Configure(Settings v){settings_=std::move(v);}
};
}
