#include "NeuralRendering/BeforeHost.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
int wmain(int argc,wchar_t** argv){
    if(argc!=3||NrRuntimeResearch::GameRunningOrUnknown())return 1;
    BeforeHost host;SettingsSnapshot off;off.revision=1;
    auto result=host.Evaluate({},off);if(!result||result->evaluated||GetModuleHandleW(L"nvngx_dlssnr.dll"))return 1;
    BeforeHost absent;SettingsSnapshot requested;requested.enabled=true;requested.revision=1;
    auto unavailable=absent.Evaluate({},requested);
    if(!unavailable || unavailable->effectiveReset){std::puts("FAIL UnavailableNrMustNotResetSourceHistoryEveryFrame");return 1;}
    std::puts("PASS UnavailableNrMustNotResetSourceHistoryEveryFrame");
    ComPtr<IDXGIFactory6> factory;if(FAILED(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory))))return 77;
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 d{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> a;auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&a));if(hr==DXGI_ERROR_NOT_FOUND)break;if(FAILED(hr)||FAILED(a->GetDesc1(&d)))return 1;if(d.VendorId==0x10de&&d.DeviceId==0x2702){adapter=a;break;}}
    if(!adapter)return 77;ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    if(FAILED(D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context)))return 77;
    StartupSettings settings;settings.community=true;settings.runtimeRoot=std::filesystem::absolute(argv[1]);settings.driverCore=argv[2];settings.sourceEncoding=TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22;
    ComPtr<IDXGIAdapter> warp;ComPtr<ID3D12Device> foreign;
    if(FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)))||FAILED(D3D12CreateDevice(warp.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&foreign))))return 1;
    BeforeHost wrongAdapter;auto rejected=wrongAdapter.Inspect(device.Get(),settings,std::filesystem::absolute("nr-host-cache"),foreign.Get());
    if(rejected||rejected.error().kind!=ErrorKind::IdentityMismatch||wrongAdapter.Available()||GetModuleHandleW(L"nvngx_dlssnr.dll"))return 1;
    std::puts("PASS ForeignPresenterRejectedBeforeVendorAdmission");
    auto inspect=host.Inspect(device.Get(),settings,std::filesystem::absolute("nr-host-cache"));
    if(!inspect){std::puts(inspect.error().message.c_str());return 1;}
    if(!host.Available()||host.ProfileId()!="rtx40"||host.Recorded()!=0||GetModuleHandleW(L"nvngx_dlssnr.dll"))return 1;
    BeforeInput invalid;invalid.context=context;SettingsSnapshot enabled=off;enabled.enabled=true;
    auto admission=host.Evaluate(invalid,enabled);
    if(!admission||admission->evaluated||host.Terminal()||GetModuleHandleW(L"nvngx_dlssnr.dll"))return 1;
    std::puts("PASS BeforeHostUsesActualAdapterAndHeldFilesWithoutVendorInitOnMissingGuides");
    if(!host.Retire())return 1;return 0;
}
