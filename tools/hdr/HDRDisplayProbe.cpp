// Read-only display inventory using the same HDR detection as the renderer.
// Does not change Windows HDR, create a swapchain or launch Skyrim.
#include "FrameGen/SourceDLSSGHDROutput.h"
#include <iostream>
#include <string>
#include <cwchar>
#include <vector>

namespace HDR = TheosRenderPipeline::SourceDLSSG;
using Microsoft::WRL::ComPtr;

std::string Utf8(const wchar_t* value)
{
    const auto length = static_cast<int>(std::wcslen(value));
    const int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value, length, nullptr, 0, nullptr, nullptr);
    if (!size) return {};
    std::string result(size, '\0');
    if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value, length, result.data(), size, nullptr, nullptr)) return {};
    return result;
}

std::string JsonString(const wchar_t* value)
{
    std::string result{"\""};
    constexpr char digits[] = "0123456789abcdef";
    for (const unsigned char c : Utf8(value)) {
        if (c == '"' || c == '\\') { result += '\\'; result += static_cast<char>(c); }
        else if (c < 0x20) {
            result += "\\u00"; result += digits[c >> 4]; result += digits[c & 15];
        } else result += static_cast<char>(c);
    }
    result += '"';
    return result;
}

struct Capability
{
    std::wstring name;
    bool advancedKnown{}, advancedSupported{}, hdrKnown{}, hdrSupported{};
    LONG queryResult{ERROR_NOT_FOUND};
};

Capability QueryCapability(const wchar_t* display)
{
    Capability result;
    UINT32 pathCount{}, modeCount{};
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount)) return result;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount, modes.data(), nullptr)) return result;
    for (UINT32 i = 0; i < pathCount; ++i) {
        const auto& path = paths[i];
        DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
        source.header = {DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME, sizeof(source), path.sourceInfo.adapterId, path.sourceInfo.id};
        if (DisplayConfigGetDeviceInfo(&source.header) || std::wcscmp(source.viewGdiDeviceName, display)) continue;
        DISPLAYCONFIG_TARGET_DEVICE_NAME target{};
        target.header = {DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME, sizeof(target), path.targetInfo.adapterId, path.targetInfo.id};
        if (!DisplayConfigGetDeviceInfo(&target.header)) result.name = target.monitorFriendlyDeviceName;
        DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO_2 modern{};
        modern.header = {DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO_2, sizeof(modern), path.targetInfo.adapterId, path.targetInfo.id};
        result.queryResult = DisplayConfigGetDeviceInfo(&modern.header);
        if (result.queryResult == ERROR_SUCCESS) {
            result.advancedKnown = result.hdrKnown = true;
            result.advancedSupported = modern.advancedColorSupported;
            result.hdrSupported = modern.highDynamicRangeSupported;
        } else {
            DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO legacy{};
            legacy.header = {DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO, sizeof(legacy), path.targetInfo.adapterId, path.targetInfo.id};
            if (!DisplayConfigGetDeviceInfo(&legacy.header)) {
                result.advancedKnown = true;
                result.advancedSupported = legacy.advancedColorSupported;
                // Legacy advanced-color support can include WCG; only absence
                // rules out HDR. A positive result leaves HDR support unknown.
                result.hdrKnown = !result.advancedSupported;
            }
        }
        return result;
    }
    return result;
}

int main()
{
    ComPtr<IDXGIFactory1> factory;
    const auto created = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(created)) {
        std::cerr << "CreateDXGIFactory1 failed: " << std::hex << static_cast<unsigned>(created) << '\n';
        return 1;
    }
    std::cout << "{\"readOnly\":true,\"displays\":[";
    unsigned count{};
    for (UINT a = 0;; ++a) {
        ComPtr<IDXGIAdapter1> adapter;
        const auto enumerated = factory->EnumAdapters1(a, &adapter);
        if (enumerated == DXGI_ERROR_NOT_FOUND) break;
        if (FAILED(enumerated)) return 2;
        DXGI_ADAPTER_DESC1 gpu{};
        if (FAILED(adapter->GetDesc1(&gpu))) return 2;
        for (UINT o = 0;; ++o) {
            ComPtr<IDXGIOutput> output;
            const auto found = adapter->EnumOutputs(o, &output);
            if (found == DXGI_ERROR_NOT_FOUND) break;
            if (FAILED(found)) return 2;
            ComPtr<IDXGIOutput6> output6;
            DXGI_OUTPUT_DESC1 desc{};
            if (FAILED(output.As(&output6)) || FAILED(output6->GetDesc1(&desc))) continue;
            if (!desc.AttachedToDesktop) continue;
            const auto& rect = desc.DesktopCoordinates;
            HWND window = CreateWindowExW(0, L"STATIC", L"RaZkolbaS HDR probe", WS_POPUP,
                rect.left + (rect.right-rect.left)/2, rect.top + (rect.bottom-rect.top)/2,
                1, 1, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
            if (!window) return 2;
            const auto detected = HDR::QueryDisplayHDR(factory.Get(), window);
            DestroyWindow(window);
            const auto white = HDR::QuerySDRWhiteNits(desc.DeviceName);
            const auto capability = QueryCapability(desc.DeviceName);
            if (count++) std::cout << ',';
            std::cout << "{\"adapter\":" << JsonString(gpu.Description)
                      << ",\"vendorId\":" << gpu.VendorId
                      << ",\"display\":" << JsonString(desc.DeviceName)
                      << ",\"monitorName\":" << JsonString(capability.name.c_str())
                      << ",\"hdrSupportKnown\":" << (capability.hdrKnown ? "true" : "false")
                      << ",\"hdrSupported\":" << (capability.hdrKnown ? (capability.hdrSupported ? "true" : "false") : "null")
                      << ",\"advancedColorKnown\":" << (capability.advancedKnown ? "true" : "false")
                      << ",\"advancedColorSupported\":" << (capability.advancedSupported ? "true" : "false")
                      << ",\"hdrCapabilityQueryResult\":" << capability.queryResult
                      << ",\"colorSpace\":" << static_cast<unsigned>(desc.ColorSpace)
                      << ",\"bitsPerColor\":" << desc.BitsPerColor
                      << ",\"windowsHdrKnown\":" << (detected.known ? "true" : "false")
                      << ",\"windowsHdrActive\":" << (detected.active ? "true" : "false")
                      << ",\"maxLuminanceNits\":" << desc.MaxLuminance
                      << ",\"minLuminanceNits\":" << desc.MinLuminance
                      << ",\"maxFullFrameLuminanceNits\":" << desc.MaxFullFrameLuminance
                      << ",\"windowsSdrWhiteNits\":" << white << '}';
        }
    }
    std::cout << "]}\n";
    return count ? 0 : 2;
}
