#include "NvidiaAppSettings.h"
#include <cstdio>
#include <string>
namespace {bool snapshot{},unavailable{};
void Log(const char* text){std::puts(text);const std::string value(text);
    if(value.find("read-only driver settings snapshot: status=0")!=std::string::npos)snapshot=true;
    if(value.find("System32 NVAPI query missing")!=std::string::npos||value.find("NVAPI init=-6;")!=std::string::npos)unavailable=true;
}}
int main(){
    TheosRenderPipeline::NvidiaAppSettings::SetLog(Log);
    TheosRenderPipeline::NvidiaAppSettings::ReportDriverSettings();
    if(unavailable)return 77;
    if(!snapshot){std::puts("FAIL: installed NVIDIA driver must support a read-only DRS snapshot");return 1;}
    std::puts("PASS: installed driver snapshot completed without profile writes");return 0;
}
