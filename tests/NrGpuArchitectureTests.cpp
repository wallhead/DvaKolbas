#include "NeuralRendering/GpuArchitecture.h"
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <cstdio>
#include <cstring>
using namespace TheosRenderPipeline::NeuralRendering;
namespace {
int failures{},calls{},mode{};
const AdapterLuid target{123,4};
void Check(bool condition,const char* name){std::printf("%s %s\n",condition?"PASS":"FAIL",name);failures+=!condition;}
int __cdecl Enumerate(void** logical,uint32_t* count){++calls;logical[0]=reinterpret_cast<void*>(1);logical[1]=reinterpret_cast<void*>(2);*count=2;return mode==4?-1:0;}
int __cdecl Logical(void* logical,NvLogicalGpuData* data){
    Check(data->version==(sizeof(*data)|(1u<<16)),"PublicLogicalInfoAbiVersion");
    auto& luid=*static_cast<AdapterLuid*>(data->osAdapterId);
    luid=(logical==reinterpret_cast<void*>(2)||mode==1)?target:AdapterLuid{456,7};
    data->physicalGpuCount=mode==5?2:1;data->physicalGpus[0]=logical;return 0;
}
int __cdecl Architecture(void*,NvGpuArchInfo* data){
    if(mode==3 && data->version==(sizeof(*data)|(2u<<16)))return -9;
    Check(data->version==(sizeof(*data)|((mode==3?1u:2u)<<16)),"PublicArchitectureAbiVersion");
    data->architecture=mode==6?0x999:0x1b0;return 0;
}
int __cdecl Name(void*,char* name){std::strcpy(name,mode==2?"NVIDIA GeForce GTX 1660":"NVIDIA GeForce RTX 5090 Laptop GPU");return 0;}
}
int main(int argc,char**) {
    if(argc>1){
        Microsoft::WRL::ComPtr<IDXGIFactory1> factory;
        if(FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))return 1;
        for(UINT i=0;;++i){Microsoft::WRL::ComPtr<IDXGIAdapter1> device;
            if(factory->EnumAdapters1(i,&device)==DXGI_ERROR_NOT_FOUND)break;
            DXGI_ADAPTER_DESC1 desc{};if(FAILED(device->GetDesc1(&desc))||desc.VendorId!=0x10de)continue;
            AdapterIdentity observed{desc.VendorId,desc.DeviceId,desc.SubSysId,{desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart},bool(desc.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)};
            DiscoverGpuArchitecture(observed);
            std::printf("LIVE pci=0x%04X luid=%08X:%08X queried=%d arch=0x%X rtx=%d family=%d\n",observed.deviceId,
                static_cast<uint32_t>(observed.luid.high),observed.luid.low,observed.architecture.queried,
                observed.architecture.id,observed.architecture.rtxProduct,static_cast<int>(ClassifyGpu(observed)));
            return observed.architecture.queried&&ClassifyGpu(observed)!=GpuFamily::Unknown?0:1;
        }return 1;
    }
    const GpuArchitectureApi api{Enumerate,Logical,Architecture,Name};
    AdapterIdentity adapter{0x10de,0xdead,0,target,false};
    DiscoverGpuArchitecture(adapter,api);
    Check(adapter.architecture.queried&&ClassifyGpu(adapter)==GpuFamily::Rtx50,"UnlistedLaptopUsesOnlyMatchingRenderLuid");
    mode=1;DiscoverGpuArchitecture(adapter,api);
    Check(adapter.architecture.queried&&ClassifyGpu(adapter)==GpuFamily::Unknown,"AmbiguousMatchingLogicalGpusRejected");
    mode=2;DiscoverGpuArchitecture(adapter,api);Check(ClassifyGpu(adapter)==GpuFamily::Unknown,"GtxProductRejected");
    mode=3;DiscoverGpuArchitecture(adapter,api);Check(ClassifyGpu(adapter)==GpuFamily::Rtx50,"PublicV1ArchInfoFallback");
    mode=4;DiscoverGpuArchitecture(adapter,api);Check(!adapter.architecture.queried&&ClassifyGpu(adapter)==GpuFamily::Unknown,"QueryFailureCannotInventLaptopEligibility");
    adapter.deviceId=0x2702;DiscoverGpuArchitecture(adapter,api);Check(ClassifyGpu(adapter)==GpuFamily::Rtx40,"UnavailableApiKeepsReviewedDesktopFallback");
    adapter.deviceId=0xdead;mode=5;DiscoverGpuArchitecture(adapter,api);Check(ClassifyGpu(adapter)==GpuFamily::Unknown,"MultiNodeLogicalGpuRejected");
    mode=6;DiscoverGpuArchitecture(adapter,api);Check(ClassifyGpu(adapter)==GpuFamily::Unknown,"FutureArchitectureRejected");
    calls=0;adapter.vendorId=0x1002;DiscoverGpuArchitecture(adapter,api);Check(calls==0&&ClassifyGpu(adapter)==GpuFamily::AmdUnsupported,"AmdDoesNotQueryNvApi");
    calls=0;adapter.vendorId=0x10de;adapter.software=true;DiscoverGpuArchitecture(adapter,api);Check(calls==0&&ClassifyGpu(adapter)==GpuFamily::Unknown,"SoftwareAdapterCannotQueryNvApi");
    return failures?1:0;
}
