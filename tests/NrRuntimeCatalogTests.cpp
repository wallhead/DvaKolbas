#include "NeuralRendering/RuntimeCatalog.h"
#include <cstdio>
#include <array>
#include <algorithm>
#include <vector>
using namespace TheosRenderPipeline::NeuralRendering;
namespace {
int failed{};
void Check(bool condition, const char* name) {
    std::printf("%s %s\n", condition?"PASS":"FAIL", name); if (!condition) ++failed;
}
AdapterIdentity Nvidia(uint32_t id) { return {0x10de,id,0xf3041569,{123,4},false}; }
const std::array artifacts{ArtifactIdentity{"rtx50",true,true}, ArtifactIdentity{"rtx40",true,true},
    ArtifactIdentity{"rtx20-30",true,true}};
void Selected(uint32_t device, const char* expected) {
    const auto selected = SelectRuntime(Nvidia(device), "Auto", artifacts);
    Check(selected.profile && selected.profile->id == expected && selected.qualification == Qualification::HardwareNotRun, expected);
}
}
int main() {
    Check(RuntimeCatalog().size()==3, "OnlyThreeRequestedNvidiaProfiles");
    struct Expected { const char* id; const char* sha; uint64_t bytes; uint32_t device; };
    const Expected expected[] {
        {"rtx50","e16bcf15e16e13f527491cdf7845b2fe6521a738d8f7c9c721866a8496e1fc8e",165840496,0x2b85},
        {"rtx40","e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a",165840496,0x2702},
        {"rtx20-30","6dac1b40f0c87af84a8177b18c741e84fb0c914f204c9d87d95916b665ba3af8",309671536,0x1e04}};
    for (const auto& e: expected) {
        const auto selected=SelectRuntime(Nvidia(e.device),e.id,artifacts);
        Check(selected.profile && selected.profile->bytes==e.bytes && selected.profile->sha256==e.sha,e.id);
    }
    Selected(0x1e04,"rtx20-30"); Selected(0x2203,"rtx20-30"); Selected(0x2702,"rtx40"); Selected(0x2f06,"rtx50");
    Check(ClassifyGpu(0x10de,0x2702,false)==GpuFamily::Rtx40,"ExactDesktop4080Super");
    Check(ClassifyGpu(0x1002,0x744c,false)==GpuFamily::AmdUnsupported,"AmdUnsupported");
    Check(ClassifyGpu(0x8086,0x2702,false)==GpuFamily::Unknown,"SameDeviceIdDifferentVendorRejected");
    Check(ClassifyGpu(0x10de,0x2702,true)==GpuFamily::Unknown,"SoftwareAdapterRejected");
    Check(ClassifyGpu(0x10de,0x2701,false)==GpuFamily::Unknown,"NearbyUnreviewedIdRejected");
    Check(ClassifyGpu(0x10de,0x2717,false)==GpuFamily::Unknown,"MobileIdUnqualified");
    auto selected=SelectRuntime(Nvidia(0x2702),"rtx50",artifacts);
    Check(!selected.profile && selected.reason.find("family")!=std::string::npos,"WrongFamilyOverrideRejected");
    selected=SelectRuntime(Nvidia(0x2702),"bogus",artifacts);
    Check(!selected.profile && selected.reason.find("Unknown")!=std::string::npos,"UnknownProfileRejected");
    Check(!SelectRuntime(Nvidia(0x2702),"amd-unsupported",artifacts).profile,"AmdProfileNeverLoadsOnNvidia");
    auto missing=artifacts; missing[1].present=false;
    Check(!SelectRuntime(Nvidia(0x2702),"Auto",missing).profile,"MissingFileDoesNotFallBackToWrongFamily");
    missing=artifacts; missing[1].heldFileVerified=false;
    Check(!SelectRuntime(Nvidia(0x2702),"Auto",missing).profile,"UnleasedFileCannotSelect");
    auto duplicate=std::vector<ArtifactIdentity>(artifacts.begin(),artifacts.end()); duplicate.push_back(artifacts[1]);
    Check(!SelectRuntime(Nvidia(0x2702),"Auto",duplicate).profile,"AmbiguousArtifactRejected");
    auto adapter=Nvidia(0x2702); adapter.luid={};
    Check(!SelectRuntime(adapter,"Auto",artifacts).profile,"MissingRenderLuidRejected");
    adapter=Nvidia(0x2702); auto other=adapter; other.luid.low++;
    Check(!CheckAdapterMatch(adapter,other),"ForeignLuidRejected");
    other=adapter; other.deviceId=0x2782;
    Check(!CheckAdapterMatch(adapter,other),"SameLuidDifferentDeviceRejected");
    Check(bool(CheckAdapterMatch(adapter,adapter)),"SameActualAdapterAccepted");
    RuntimeProfile pathProfile{"fixture","NR/rtx40/nvngx_dlssnr.dll","",0};
    auto path=RuntimePath("C:/absolute/root",pathProfile);
    Check(path && *path==std::filesystem::path("C:/absolute/root/NR/rtx40/nvngx_dlssnr.dll"),"ControlledRelativeProfilePath");
    Check(!RuntimePath("relative",pathProfile),"RelativeRootRejected");
    for (const char* bad : {"../nvngx_dlssnr.dll","NR/../nvngx_dlssnr.dll","C:/foreign/nvngx_dlssnr.dll",
        "NR/./nvngx_dlssnr.dll","NR/other.dll","NR/nvngx_dlssnr.dll:stream"}) {
        pathProfile.relativePath=bad;
        Check(!RuntimePath("C:/absolute/root",pathProfile),bad);
    }
    return failed?1:0;
}
