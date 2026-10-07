#include "NvidiaAppSettings.h"
#include <cstdio>
#include <string>
namespace {bool snapshot{},unavailable{},global{},base{},catalog{},configuration{};
void Log(const char* text){std::puts(text);const std::string value(text);
    if(value.find("read-only driver settings snapshot: status=0")!=std::string::npos)snapshot=true;
    if(value.find("driver profile lookup: scope=global status=0")!=std::string::npos)global=true;
    if(value.find("driver profile lookup: scope=base status=0")!=std::string::npos)base=true;
    if(value.find("driver setting ID enumeration: status=0")!=std::string::npos)catalog=true;
    if(value.find("Smooth Motion DX11 configured=")!=std::string::npos&&
        value.find("does not establish active interpolation")!=std::string::npos)configuration=true;
    if(value.find("System32 NVAPI query missing")!=std::string::npos||value.find("NVAPI init=-6;")!=std::string::npos)unavailable=true;
}}
int main(){
    TheosRenderPipeline::NvidiaAppSettings::SetLog(Log);
    TheosRenderPipeline::NvidiaAppSettings::ReportDriverSettings();
    if(unavailable)return 77;
    if(!snapshot){std::puts("FAIL: installed NVIDIA driver must support a read-only DRS snapshot");return 1;}
    if(!global||!base||!catalog||!configuration){std::puts("FAIL: installed NVIDIA driver profile/catalog diagnostics incomplete");return 1;}
    std::puts("PASS: installed driver global/base/catalog diagnostics completed without profile writes");return 0;
}
