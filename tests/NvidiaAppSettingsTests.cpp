#include "NvidiaAppSettingsPolicy.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>
using namespace TheosRenderPipeline::NvidiaAppSettings;
void Require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int main(){try {
    DrsSetting setting{};setting.version=sizeof(setting)|0x10000;setting.id=0x10e41e03;
    setting.current=1;setting.location=2;setting.predefined[0]=99;
    auto expected=setting;expected.current=0;
    Require(FilterSetting(0,setting.id,&setting),"FG runtime override must yield to application settings");
    Require(!std::memcmp(&expected,&setting,sizeof(setting)),"only returned current value changes; metadata preserved");
    setting.id=0x10e41df3;setting.current=0xffffff;
    Require(FilterSetting(0,setting.id,&setting)&&setting.current==0,"forced SR preset disabled");
    setting.id=0x10afb768;setting.current=4;
    Require(FilterSetting(0,setting.id,&setting)&&setting.current==3,"SR mode uses snippet-controlled default");
    setting.id=0x10e41e06;setting.current=1;
    Require(FilterSetting(0,setting.id,&setting)&&setting.current==0,"Streamline runtime override disabled");
    setting.id=0x10e41e04;setting.current=1;
    Require(FilterSetting(0,setting.id,&setting)&&setting.current==0,"NR remains application controlled");
    setting.id=0x12345678;setting.current=42;expected=setting;
    Require(!FilterSetting(0,setting.id,&setting)&&!std::memcmp(&expected,&setting,sizeof(setting)),"unrelated driver options unchanged");
    setting.id=0x10e41e03;setting.current=1;expected=setting;
    Require(!FilterSetting(-160,setting.id,&setting)&&!std::memcmp(&expected,&setting,sizeof(setting)),"missing settings and errors preserved");
    Require(!FilterSetting(0,0x10e41e04,&setting),"mismatched setting identity rejected");
    setting.type=1;Require(!FilterSetting(0,setting.id,&setting),"non-DWORD setting rejected");
    setting.type=0;setting.version=16|0x10000;Require(!FilterSetting(0,setting.id,&setting),"short ABI rejected");
    Require(!FilterSetting(0,setting.id,nullptr),"null output rejected");
    std::puts("PASS: NVIDIA override reads yield to application settings; unrelated settings, errors and metadata preserved");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
