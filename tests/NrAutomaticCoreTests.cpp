#include "NeuralRendering/BeforeHost.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
int main(int argc,char** argv) {
    if(argc!=2)return 1;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    const auto hr=D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,nullptr);
    if(FAILED(hr)||!GetModuleHandleW(L"nvwgf2umx.dll"))return 77;
    StartupSettings startup;startup.community=true;startup.runtimeRoot=std::filesystem::absolute(argv[1]);
    startup.sourceEncoding=TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22;startup.sdrBytesTrial=true;
    BeforeHost automatic;
    const auto result=automatic.Inspect(device.Get(),startup,std::filesystem::absolute("nr-auto-core-cache"));
    if(!result){std::printf("FAIL BlankCoreUsesActiveNvidiaDriver: %s\n",result.error().message.c_str());return 1;}
    if(!automatic.Available())return 1;
    std::puts("PASS BlankCoreUsesActiveNvidiaDriver");
    startup.driverCore=std::filesystem::absolute("deliberately-missing-driver/_nvngx.dll");
    BeforeHost explicitOverride;
    const auto missing=explicitOverride.Inspect(device.Get(),startup,std::filesystem::absolute("nr-auto-core-cache"));
    if(missing||missing.error().nativeCode!=ERROR_PATH_NOT_FOUND||
       missing.error().message.find(startup.driverCore.string())==std::string::npos)return 1;
    std::puts("PASS ExplicitMissingCoreIsReportedWithoutSilentFallback");
    return 0;
}
