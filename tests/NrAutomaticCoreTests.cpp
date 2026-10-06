#include "NeuralRendering/BeforeHost.h"
#include "NeuralRendering/RuntimeFileLease.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <cstdio>
#include <fstream>
#include <vector>
#include <cstring>
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
    startup.driverCore=automatic.DriverCorePath().parent_path().parent_path()/
        (L"raz-missing-"+std::to_wstring(GetCurrentProcessId()))/L"_nvngx.dll";
    if(std::filesystem::exists(startup.driverCore))return 1;
    BeforeHost recovered;
    const auto fallback=recovered.Inspect(device.Get(),startup,std::filesystem::absolute("nr-auto-core-cache"));
    if(!fallback||!recovered.Available()||recovered.DriverCorePath()!=automatic.DriverCorePath()) {
        std::printf("FAIL MissingDriverStoreOverrideRecoversQualifiedActiveCore: %s\n",fallback?"wrong core":fallback.error().message.c_str());return 1;
    }
    std::puts("PASS MissingDriverStoreOverrideRecoversQualifiedActiveCore");
    startup.driverCore=automatic.DriverCorePath();
    const auto existing=startup.ResolveDriverCore();
    if(!existing||*existing!=startup.driverCore)return 1;
    std::puts("PASS ExistingDriverStoreOverrideIsPreserved");
    startup.driverCore=std::filesystem::absolute("deliberately-missing-driver/_nvngx.dll");
    BeforeHost explicitOverride;
    const auto missing=explicitOverride.Inspect(device.Get(),startup,std::filesystem::absolute("nr-auto-core-cache"));
    if(missing||missing.error().nativeCode!=ERROR_PATH_NOT_FOUND||
       missing.error().message.find(startup.driverCore.string())==std::string::npos)return 1;
    std::puts("PASS ExplicitMissingCoreIsReportedWithoutSilentFallback");
    // PE CheckSum is excluded from Authenticode/catalog hashing. Changing it
    // produces a different whole-file SHA without changing trusted code or ABI.
    const auto root=std::filesystem::absolute("nr-trusted-core-"+std::to_string(GetCurrentProcessId()));
    std::filesystem::create_directory(root);
    const auto alternate=root/"_nvngx.dll";
    {
        std::ifstream input(automatic.DriverCorePath(),std::ios::binary);
        std::vector<char> bytes((std::istreambuf_iterator<char>(input)),{});
        IMAGE_DOS_HEADER dos{};
        if(bytes.size()<sizeof(dos))return 1;
        std::memcpy(&dos,bytes.data(),sizeof(dos));
        const auto checksum=static_cast<size_t>(dos.e_lfanew)+offsetof(IMAGE_NT_HEADERS64,OptionalHeader)+offsetof(IMAGE_OPTIONAL_HEADER64,CheckSum);
        if(checksum>=bytes.size())return 1;
        bytes[checksum]^=1;
        std::ofstream output(alternate,std::ios::binary);output.write(bytes.data(),bytes.size());
        if(!output)return 1;
    }
    bool accepted{};
    {
        startup.driverCore=alternate;
        BeforeHost different;
        const auto inspected=different.Inspect(device.Get(),startup,std::filesystem::absolute("nr-auto-core-cache"));
        accepted=bool(inspected)&&different.Available();
        std::printf("%s TrustedDriverCoreWithDifferentWholeFileShaAccepted%s%s\n",accepted?"PASS":"FAIL",
            inspected?"":": ",inspected?"":inspected.error().message.c_str());
        auto held=RuntimeFileLease::OpenDriverCore(alternate);
        if(!held || !held->Matches(alternate) || held->Sha256()=="66767018c36b3bab46398dade3adf173daa3730fda75965689ea848c9bc4e79b")return 1;
        const auto writer=CreateFileW(alternate.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
        const auto error=GetLastError();
        if(writer!=INVALID_HANDLE_VALUE){CloseHandle(writer);return 1;}
        if(error!=ERROR_SHARING_VIOLATION)return 1;
        std::puts("PASS TrustedCoreRetainsDigestIdentityAndWriteProtection");
    }
    // Changing code, unlike CheckSum, invalidates the catalog member digest.
    {
        std::fstream damaged(alternate,std::ios::binary|std::ios::in|std::ios::out);
        damaged.seekg(0x1000);char byte{};damaged.read(&byte,1);byte^=1;
        damaged.seekp(0x1000);damaged.write(&byte,1);if(!damaged)return 1;
    }
    const auto corrupt=RuntimeFileLease::OpenDriverCore(alternate);
    if(corrupt || corrupt.error().message.find("signature or installed catalog")==std::string::npos)return 1;
    std::puts("PASS ModifiedDriverCodeRejectedByWindowsTrust");
    std::filesystem::remove(alternate);std::filesystem::remove(root);
    if(!accepted)return 1;
    return 0;
}
