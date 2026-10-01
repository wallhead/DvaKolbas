#include <ffx_upscale.h>
#include <dx12/ffx_api_dx12.h>
#include <cstdio>
#include <cstring>

static unsigned mode{}, enumerations{}, destructions{};
static uint64_t queryId{}, createId{};
static char name[64]{"fixture analytical FSR 3.1.5"};
extern "C" __declspec(dllexport) void FixtureMode(unsigned value)
{ mode = value; enumerations = 0; std::strcpy(name, "fixture analytical FSR 3.1.5"); }
extern "C" __declspec(dllexport) uint64_t FixtureQueryId() { return queryId; }
extern "C" __declspec(dllexport) uint64_t FixtureCreateId() { return createId; }
extern "C" __declspec(dllexport) unsigned FixtureDestroyCount() { return destructions; }
static bool ReadChain(const ffxApiHeader* header, uint64_t& id)
{
    bool device{}, version{};
    for (auto* next = header->pNext; next; next = next->pNext) {
        if (next->type == FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12)
            device = reinterpret_cast<const ffxCreateBackendDX12Desc*>(next)->device != nullptr;
        if (next->type == FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE_VERSION)
            version = reinterpret_cast<const ffxCreateContextDescUpscaleVersion*>(next)->version == FFX_UPSCALER_VERSION;
        if (next->type == FFX_API_DESC_TYPE_OVERRIDE_VERSION)
            id = reinterpret_cast<const ffxOverrideVersion*>(next)->versionId;
    }
    return device && version && id == 17;
}
#if FSR_MISSING_EXPORT != 1
extern "C" __declspec(dllexport) ffxReturnCode_t ffxCreateContext(ffxContext* context, ffxCreateContextDescHeader* desc, const ffxAllocationCallbacks*)
{ if (!ReadChain(desc, createId)) return FFX_API_RETURN_ERROR_PARAMETER; *context = reinterpret_cast<void*>(1); return FFX_API_RETURN_OK; }
#endif
#if FSR_MISSING_EXPORT != 2
extern "C" __declspec(dllexport) ffxReturnCode_t ffxDestroyContext(ffxContext* context, const ffxAllocationCallbacks*)
{ ++destructions; *context = nullptr; return FFX_API_RETURN_OK; }
#endif
#if FSR_MISSING_EXPORT != 3
extern "C" __declspec(dllexport) ffxReturnCode_t ffxConfigure(ffxContext*, const ffxConfigureDescHeader*) { return FFX_API_RETURN_OK; }
#endif
#if FSR_MISSING_EXPORT != 4
extern "C" __declspec(dllexport) ffxReturnCode_t ffxQuery(ffxContext* context, ffxQueryDescHeader* header)
{
    if (mode == 2) return FFX_API_RETURN_PROVIDER_NO_SUPPORT_NEW_DESCTYPE;
    if (header->type == FFX_API_QUERY_DESC_TYPE_GET_VERSIONS) {
        auto& desc = *reinterpret_cast<ffxQueryDescGetVersions*>(header);
        if (!desc.device || desc.createDescType != FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE) return FFX_API_RETURN_ERROR_PARAMETER;
        ++enumerations;
        uint64_t count = mode == 1 ? 0 : mode == 3 && enumerations > 1 ? 2 : mode == 5 ? *desc.outputCount + 1 : mode == 6 ? 129 : 1;
        uint64_t capacity = *desc.outputCount;
        *desc.outputCount = count;
        if (desc.versionIds && capacity >= count) {
            for (uint64_t i = 0; i < count; ++i) { desc.versionIds[i] = 17 + i; desc.versionNames[i] = mode == 7 ? nullptr : i ? "fixture compatible" : name; }
        }
        return FFX_API_RETURN_OK;
    }
    if (header->type == FFX_API_QUERY_DESC_TYPE_UPSCALE_GETRENDERRESOLUTIONFROMQUALITYMODE) {
        if (!ReadChain(header, queryId)) return FFX_API_RETURN_ERROR_PARAMETER;
        auto& desc = *reinterpret_cast<ffxQueryDescUpscaleGetRenderResolutionFromQualityMode*>(header);
        *desc.pOutRenderWidth = desc.displayWidth / (desc.qualityMode == FFX_UPSCALE_QUALITY_MODE_NATIVEAA ? 1 : 2);
        *desc.pOutRenderHeight = desc.displayHeight / (desc.qualityMode == FFX_UPSCALE_QUALITY_MODE_NATIVEAA ? 1 : 2);
        std::strcpy(name, "overwritten shared SDK string");
        return FFX_API_RETURN_OK;
    }
    if (header->type == FFX_API_QUERY_DESC_TYPE_GET_PROVIDER_VERSION && context && *context) {
        auto& desc = *reinterpret_cast<ffxQueryGetProviderVersion*>(header);
        desc.versionId = mode == 4 ? 99 : 17; desc.versionName = name;
        return FFX_API_RETURN_OK;
    }
    if (header->type == FFX_API_QUERY_DESC_TYPE_UPSCALE_GETJITTERPHASECOUNT && context && *context) {
        auto& desc=*reinterpret_cast<ffxQueryDescUpscaleGetJitterPhaseCount*>(header);
        *desc.pOutPhaseCount=32; return FFX_API_RETURN_OK;
    }
    if (header->type == FFX_API_QUERY_DESC_TYPE_UPSCALE_GETJITTEROFFSET && context && *context) {
        auto& desc=*reinterpret_cast<ffxQueryDescUpscaleGetJitterOffset*>(header);
        *desc.pOutX=float(desc.index)/float(desc.phaseCount)-0.5f; *desc.pOutY=-*desc.pOutX; return FFX_API_RETURN_OK;
    }
    return FFX_API_RETURN_ERROR_UNKNOWN_DESCTYPE;
}
#endif
#if FSR_MISSING_EXPORT != 5
extern "C" __declspec(dllexport) ffxReturnCode_t ffxDispatch(ffxContext*, const ffxDispatchDescHeader*) { return FFX_API_RETURN_OK; }
#endif
