#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <charconv>
#include <bit>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
namespace AmdNrTools {
using Microsoft::WRL::ComPtr;
inline void CheckDeviceApi(HRESULT hr) {if(FAILED(hr)) throw std::runtime_error("D3D12/DXGI API failure");}
inline LUID ParseLuid(std::wstring_view text) {
    auto colon=text.find(L':');if(colon==text.npos || text.find(L':',colon+1)!=text.npos) throw std::runtime_error("LUID must be high-hex:low-hex");
    auto part=[](std::wstring_view text) {
        std::string s;for(auto c:text){if(c>127) throw std::runtime_error("invalid LUID");s+=char(c);}
        std::uint32_t result{};auto [end,error]=std::from_chars(s.data(),s.data()+s.size(),result,16);
        if(error!=std::errc{} || end!=s.data()+s.size() || s.empty()) throw std::runtime_error("invalid LUID");return result;
    };
    return {part(text.substr(colon+1)),std::bit_cast<LONG>(part(text.substr(0,colon)))};
}
inline ComPtr<IDXGIFactory4> Factory() {ComPtr<IDXGIFactory4> factory;CheckDeviceApi(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));return factory;}
inline void Describe(IDXGIAdapter1* adapter) {
    DXGI_ADAPTER_DESC1 desc{};CheckDeviceApi(adapter->GetDesc1(&desc));
    std::cout<<std::hex<<std::setfill('0')<<"vendor="<<std::setw(4)<<desc.VendorId<<" device="<<std::setw(4)<<desc.DeviceId<<" luid="<<std::setw(8)<<std::uint32_t(desc.AdapterLuid.HighPart)<<':'<<std::setw(8)<<desc.AdapterLuid.LowPart<<std::dec;
    LARGE_INTEGER driver{};if(SUCCEEDED(adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice),&driver))) {
        auto v=std::uint64_t(driver.QuadPart);std::cout<<" driver="<<(v>>48)<<'.'<<((v>>32)&65535)<<'.'<<((v>>16)&65535)<<'.'<<(v&65535);
    }else std::cout<<" driver=unavailable";
    std::cout<<'\n';std::wcout<<L"adapter="<<desc.Description<<L'\n';
}
inline ComPtr<IDXGIAdapter1> SelectAdapter(bool warp,LUID luid={}) {
    auto factory=Factory();ComPtr<IDXGIAdapter1> adapter;
    if(warp) CheckDeviceApi(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
    else if(FAILED(factory->EnumAdapterByLuid(luid,IID_PPV_ARGS(&adapter)))) throw std::runtime_error("requested adapter LUID unavailable");
    return adapter;
}
inline void ListAdapters() {
    auto factory=Factory();
    for(UINT i=0;;++i) {ComPtr<IDXGIAdapter1> adapter;auto hr=factory->EnumAdapters1(i,&adapter);if(hr==DXGI_ERROR_NOT_FOUND) break;CheckDeviceApi(hr);Describe(adapter.Get());}
}
}
