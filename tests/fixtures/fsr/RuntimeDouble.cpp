#include <ffx_upscale.h>
#include <dx12/ffx_api_dx12.h>
#include <cstdio>
#include <cstring>
#include <wrl/client.h>
#include <vector>
#ifdef TRP_ENABLE_FSR_FG
#include <ffx_framegeneration.h>
#include <dx12/ffx_api_framegeneration_dx12.h>
#endif

static unsigned mode{}, enumerations{}, destructions{};
static uint64_t queryId{}, createId{};
static char name[64]{"fixture analytical FSR 3.1.5"};
struct FixtureContext { std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> heaps; uint64_t provider{17}; };
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
{
#ifdef TRP_ENABLE_FSR_FG
    if (desc->type == FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATION) {
        bool backend{}, version{}; uint64_t identity{};
        for (auto* next = desc->pNext; next; next = next->pNext) {
            if (next->type == FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12) backend = reinterpret_cast<const ffxCreateBackendDX12Desc*>(next)->device != nullptr;
            if (next->type == FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATION_VERSION) version = reinterpret_cast<const ffxCreateContextDescFrameGenerationVersion*>(next)->version == FFX_FRAMEGENERATION_VERSION;
            if (next->type == FFX_API_DESC_TYPE_OVERRIDE_VERSION) identity = reinterpret_cast<const ffxOverrideVersion*>(next)->versionId;
        }
        if (!backend || !version || identity != 17726168133342859270ull) return FFX_API_RETURN_ERROR_PARAMETER;
        auto* created = new FixtureContext; created->provider = identity; *context = created; return FFX_API_RETURN_OK;
    }
#endif
    if (!ReadChain(desc, createId)) return FFX_API_RETURN_ERROR_PARAMETER; *context = new FixtureContext; return FFX_API_RETURN_OK;
}
#endif
#if FSR_MISSING_EXPORT != 2
extern "C" __declspec(dllexport) ffxReturnCode_t ffxDestroyContext(ffxContext* context, const ffxAllocationCallbacks*)
{ ++destructions; delete static_cast<FixtureContext*>(*context); *context = nullptr; return FFX_API_RETURN_OK; }
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
        if (!desc.device) return FFX_API_RETURN_ERROR_PARAMETER;
        uint64_t providerId = 17; const char* providerName = name;
        if (desc.createDescType != FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE) {
#ifdef TRP_ENABLE_FSR_FG
            if (desc.createDescType == FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATION) { providerId = 17726168133342859270ull; providerName = "3.1.6"; }
            else if (desc.createDescType == FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_NEW_DX12) { providerId = 17752306900579389447ull; providerName = "3.1.7"; }
            else
#endif
                return FFX_API_RETURN_ERROR_PARAMETER;
        }
        ++enumerations;
        uint64_t count = mode == 1 ? 0 : mode == 3 && enumerations > 1 ? 2 : mode == 5 ? *desc.outputCount + 1 : mode == 6 ? 129 : 1;
        uint64_t capacity = *desc.outputCount;
        *desc.outputCount = count;
        if (desc.versionIds && capacity >= count) {
            for (uint64_t i = 0; i < count; ++i) { desc.versionIds[i] = providerId + i; desc.versionNames[i] = mode == 7 ? nullptr : i ? "fixture compatible" : providerName; }
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
        const auto provider = static_cast<FixtureContext*>(*context)->provider;
        desc.versionId = mode == 4 ? 99 : provider;
        desc.versionName = mode == 15 ? "4.0.1" : provider == 17 ? name : "3.1.6";
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
extern "C" __declspec(dllexport) ffxReturnCode_t ffxDispatch(ffxContext* context, const ffxDispatchDescHeader* header)
{
    if(mode==9) {
        // A deterministic successful vendor seam. Record an actual GPU write,
        // keeping descriptors alive until the caller proves context retirement.
        if(!context || !*context || header->type!=FFX_API_DISPATCH_DESC_TYPE_UPSCALE)return FFX_API_RETURN_ERROR_PARAMETER;
        const auto* desc=reinterpret_cast<const ffxDispatchDescUpscale*>(header);
        auto* list=static_cast<ID3D12GraphicsCommandList*>(desc->commandList);auto* output=static_cast<ID3D12Resource*>(desc->output.resource);
        Microsoft::WRL::ComPtr<ID3D12Device> device;
        if(!list || !output || FAILED(output->GetDevice(IID_PPV_ARGS(&device))))return FFX_API_RETURN_ERROR_PARAMETER;
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};heapDesc.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;heapDesc.NumDescriptors=1;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cpuHeap,gpuHeap;
        if(FAILED(device->CreateDescriptorHeap(&heapDesc,IID_PPV_ARGS(&cpuHeap))))return FFX_API_RETURN_ERROR;
        heapDesc.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if(FAILED(device->CreateDescriptorHeap(&heapDesc,IID_PPV_ARGS(&gpuHeap))))return FFX_API_RETURN_ERROR;
        device->CreateUnorderedAccessView(output,nullptr,nullptr,cpuHeap->GetCPUDescriptorHandleForHeapStart());
        device->CopyDescriptorsSimple(1,gpuHeap->GetCPUDescriptorHandleForHeapStart(),cpuHeap->GetCPUDescriptorHandleForHeapStart(),heapDesc.Type);
        ID3D12DescriptorHeap* heaps[]{gpuHeap.Get()};list->SetDescriptorHeaps(1,heaps);
        const float color[]{0.005f,0.25f,0.75f,0.5f};
        list->ClearUnorderedAccessViewFloat(gpuHeap->GetGPUDescriptorHandleForHeapStart(),cpuHeap->GetCPUDescriptorHandleForHeapStart(),output,color,0,nullptr);
        auto& owned=static_cast<FixtureContext*>(*context)->heaps;owned.push_back(cpuHeap);owned.push_back(gpuHeap);
        return FFX_API_RETURN_OK;
    }
    if(mode==8) {
        auto* desc=reinterpret_cast<const ffxDispatchDescUpscale*>(header);
        auto* list=static_cast<ID3D12GraphicsCommandList*>(desc->commandList);
        D3D12_RESOURCE_BARRIER barrier{};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource=static_cast<ID3D12Resource*>(desc->output.resource);list->ResourceBarrier(1,&barrier);
        return FFX_API_RETURN_ERROR;
    }
    return FFX_API_RETURN_OK;
}
#endif
