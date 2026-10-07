#include "FSRUpscaler.h"
#include "FSRProviderPolicy.h"
#include <dx12/ffx_api_dx12.h>
#include <dxgi1_4.h>
#include <cmath>
#include <format>

namespace TheosRenderPipeline::Upscaling
{
    using Microsoft::WRL::ComPtr;
    struct FsrUpscaler::State
    {
        std::shared_ptr<FsrRuntime> runtime;
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;
        ComPtr<ID3D12Device> device;
        ffxContext context{};
        FsrContextLimits limits{};
        ProviderInfo actual{};
        ffxCreateBackendDX12AllocationCallbacksDesc allocation{};
        std::array<ComPtr<ID3D12Resource>,7> resources;
        std::uint64_t sourceId{};
        int32_t phaseCount{};
        bool dispatched{}, acceptedSource{}, poisoned{};
    };
    static std::unexpected<RuntimeError> Error(ErrorKind kind, std::int64_t result,const char* message)
    { return std::unexpected(RuntimeError{kind,result,message}); }
    FsrUpscaler::FsrUpscaler():state_(std::make_unique<State>()){}
    FsrUpscaler::~FsrUpscaler()
    {
        if (!DestroyAfterRetirement()) {
            // Keep context, runtime modules, bridge and every borrowed GPU resource
            // together. A timeout or failed context destroy never frees live work.
            (void)state_.release();
        }
    }
    Result<void> FsrUpscaler::SetInputPolicy(FsrInputPolicy policy)
    {
        if (state_->context || !policy.colorIsLinear) return Error(ErrorKind::InvalidInput,0,"FSR input policy must be linear SDR and set before creation");
        state_->limits.input=policy; return {};
    }
    Result<void> FsrUpscaler::SetRetirementBridge(std::shared_ptr<Graphics::D3D11D3D12Interop> bridge)
    {
        if(state_->dispatched || !bridge || !bridge->Ready()) return Error(ErrorKind::InvalidInput,0,"FSR requires a ready retirement bridge before recording work");
        if(state_->device && state_->device.Get()!=bridge->Device12()) return Error(ErrorKind::UnsupportedDevice,0,"FSR context and bridge devices differ");
        state_->bridge=std::move(bridge); return {};
    }
    Result<void> FsrUpscaler::SetAllocationCallbacks(const ffxCreateBackendDX12AllocationCallbacksDesc& callbacks)
    {
        if(state_->context || callbacks.header.type!=FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12_ALLOCATION_CALLBACKS ||
            callbacks.header.pNext || bool(callbacks.pfnFfxResourceAllocator)!=bool(callbacks.pfnFfxResourceDeallocator) ||
            bool(callbacks.pfnFfxHeapAllocator)!=bool(callbacks.pfnFfxHeapDeallocator))
            return Error(ErrorKind::InvalidInput,0,"FSR allocation callbacks must be paired and configured before context creation");
        state_->allocation=callbacks; return {};
    }
    Result<void> FsrUpscaler::Initialize(std::shared_ptr<FsrRuntime> runtime,ID3D12Device* device,const ProviderInfo& provider,Quality quality,Extent render,Extent output)
    {
        if(state_->context || !runtime || !runtime->Functions().CreateContext || !device || (state_->bridge && state_->bridge->Device12()!=device))
            return Error(ErrorKind::InvalidInput,0,"FSR creation requires a loaded runtime and matching actual device");
        auto extent=runtime->QueryRenderExtent(device,provider,quality,output);
        if(!extent) return std::unexpected(extent.error());
        if(*extent!=render) return Error(ErrorKind::InvalidInput,0,"FSR render dimensions differ from selected provider query");
        auto hr=device->GetDeviceRemovedReason(); if(FAILED(hr)) return Error(ErrorKind::DeviceLost,hr,"FSR device removed before creation");
        const auto minimum=IsFsr4Provider(provider)?D3D_SHADER_MODEL_6_6:D3D_SHADER_MODEL_6_2;
        D3D12_FEATURE_DATA_SHADER_MODEL model{minimum};
        if(FAILED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&model,sizeof(model))) || model.HighestShaderModel<minimum)
            return Error(ErrorKind::UnsupportedDevice,0,"FSR provider shader model unavailable (FSR3 requires 6.2; FSR4 requires 6.6)");
        if(IsFsr4Provider(provider)) {
            ComPtr<IDXGIFactory4> factory;ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 description{};
            if(FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))) ||
                FAILED(factory->EnumAdapterByLuid(device->GetAdapterLuid(),IID_PPV_ARGS(&adapter))) ||
                FAILED(adapter->GetDesc1(&description)) || description.VendorId!=0x1002)
                return Error(ErrorKind::UnsupportedDevice,0,"Official FSR4 requires a supported AMD adapter; use Auto or FSR3 on other GPUs");
        }
        for(auto format:{state_->limits.colorFormat,state_->limits.depthFormat,state_->limits.motionFormat}) {
            D3D12_FEATURE_DATA_FORMAT_SUPPORT support{format};
            if(FAILED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT,&support,sizeof(support))) ||
                !(support.Support1&D3D12_FORMAT_SUPPORT1_SHADER_LOAD) ||
                (format==state_->limits.colorFormat && !(support.Support2&D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE)))
                return Error(ErrorKind::UnsupportedDevice,0,"FSR prepared formats lack shader/UAV support");
        }
        state_->runtime=std::move(runtime); state_->device=device;
        state_->poisoned=true;
        state_->limits.render=render; state_->limits.output=output;
        auto& policy=state_->limits.input;
        ffxCreateContextDescUpscale create{}; create.header.type=FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE;
        // This pinned C API uses bit 9 for debug visualization. AIO's older
        // FSR3 RCAS-compensation bit is not a flag in this descriptor ABI.
        create.flags=FFX_UPSCALE_ENABLE_AUTO_EXPOSURE |
            (policy.depthInverted?FFX_UPSCALE_ENABLE_DEPTH_INVERTED:0) |
            (policy.depthInfinite?FFX_UPSCALE_ENABLE_DEPTH_INFINITE:0) |
            (policy.motionIncludesJitter?FFX_UPSCALE_ENABLE_MOTION_VECTORS_JITTER_CANCELLATION:0);
        create.maxRenderSize={render.width,render.height}; create.maxUpscaleSize={output.width,output.height};
        ffxCreateBackendDX12Desc backend{{FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12,nullptr},device};
        ffxCreateContextDescUpscaleVersion version{{FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE_VERSION,nullptr},FFX_UPSCALER_VERSION};
        ffxOverrideVersion override{{FFX_API_DESC_TYPE_OVERRIDE_VERSION,nullptr},provider.id};
        create.header.pNext=&backend.header; backend.header.pNext=&version.header; version.header.pNext=&override.header;
        if(state_->allocation.header.type) { backend.header.pNext=&state_->allocation.header; state_->allocation.header.pNext=&version.header; }
        auto result=state_->runtime->Functions().CreateContext(&state_->context,&create.header,nullptr);
        if(result!=FFX_API_RETURN_OK) return Error(ErrorKind::ContextFailure,result,"FSR context creation failed");
        auto verified=state_->runtime->VerifyActualProvider(state_->context,provider);
        auto actual=state_->runtime->QueryActualProvider(state_->context);
        if(!verified || !actual || actual->id!=provider.id) {
            auto error=!verified?verified.error():actual?RuntimeError{ErrorKind::ContextFailure,0,"FSR actual provider differs from selected provider"}:actual.error();
            state_->poisoned=true; (void)DestroyAfterRetirement(); return std::unexpected(error);
        }
        state_->actual=*actual;
        ffxQueryDescUpscaleGetResourceRequirements requirements{};
        requirements.header.type=FFX_API_QUERY_DESC_TYPE_UPSCALE_GET_RESOURCE_REQUIREMENTS;
        result=state_->runtime->Functions().Query(&state_->context,&requirements.header);
        // The producer supplies color/depth/motion and uses SDK auto exposure.
        // Required masks or new unknown inputs cannot be silently omitted.
        // The pinned header defines EXPOSURE as a texture OR the auto-exposure
        // creation flag. FSR3 reports it as required even with that flag set.
        const std::uint64_t supplied=FFX_API_QUERY_RESOURCE_INPUT_COLOR|FFX_API_QUERY_RESOURCE_INPUT_DEPTH|FFX_API_QUERY_RESOURCE_INPUT_MV |
            ((create.flags&FFX_UPSCALE_ENABLE_AUTO_EXPOSURE)?FFX_API_QUERY_RESOURCE_INPUT_EXPOSURE:0);
        if(result!=FFX_API_RETURN_OK || (requirements.required_resources&~supplied)) {
            (void)DestroyAfterRetirement();
            return std::unexpected(RuntimeError{ErrorKind::IncompatibleAbi,result,std::format(
                "FSR provider resource requirements unavailable or unsupported: required=0x{:X} optional=0x{:X} supplied=0x{:X}",
                requirements.required_resources,requirements.optional_resources,supplied)});
        }
        state_->limits.requiredResources=requirements.required_resources;
        state_->limits.optionalResources=requirements.optional_resources;
        ffxQueryDescUpscaleGetJitterPhaseCount phase{{FFX_API_QUERY_DESC_TYPE_UPSCALE_GETJITTERPHASECOUNT,nullptr},render.width,output.width,&state_->phaseCount};
        result=state_->runtime->Functions().Query(&state_->context,&phase.header);
        if(result!=FFX_API_RETURN_OK || state_->phaseCount<=0 || state_->phaseCount>1048576) {
            state_->poisoned=true; (void)DestroyAfterRetirement(); return Error(ErrorKind::IncompatibleAbi,result,"FSR jitter phase query failed");
        }
        state_->poisoned=false;
        return {};
    }
    Result<Extent> FsrUpscaler::RenderExtent() const
    { if(!state_->context || state_->poisoned) return Error(ErrorKind::ContextFailure,0,"FSR context unavailable"); return state_->limits.render; }
    Result<ProviderInfo> FsrUpscaler::ActualProvider() const
    { if(!state_->context || state_->poisoned) return Error(ErrorKind::ContextFailure,0,"FSR context unavailable"); return state_->actual; }
    const FsrContextLimits& FsrUpscaler::Limits() const { return state_->limits; }
    Result<std::array<float,2>> FsrUpscaler::QueryJitter(std::uint64_t sourceId)
    {
        if(!state_->context || state_->poisoned || state_->phaseCount<=0) return Error(ErrorKind::ContextFailure,0,"FSR jitter requires an initialized context");
        std::array<float,2> offset{};
        ffxQueryDescUpscaleGetJitterOffset jitter{{FFX_API_QUERY_DESC_TYPE_UPSCALE_GETJITTEROFFSET,nullptr},
            static_cast<int32_t>(sourceId%static_cast<std::uint64_t>(state_->phaseCount)),state_->phaseCount,&offset[0],&offset[1]};
        const auto code=state_->runtime->Functions().Query(&state_->context,&jitter.header);
        if(code!=FFX_API_RETURN_OK || !std::isfinite(offset[0]) || !std::isfinite(offset[1]) || std::abs(offset[0])>1 || std::abs(offset[1])>1)
            return Error(ErrorKind::IncompatibleAbi,code,"FSR provider returned invalid jitter");
        return offset;
    }
    Result<void> FsrUpscaler::Dispatch(ID3D12GraphicsCommandList* list,const GpuFrameResources& resources,const UpscaleFrame& frame)
    {
        if(!list || !state_->context || state_->poisoned || !state_->bridge || !state_->bridge->Ready())
            return Error(ErrorKind::ContextFailure,0,"FSR dispatch requires a live context and retirement bridge");
        if(state_->acceptedSource && frame.sourceId<=state_->sourceId)
            return Error(ErrorKind::InvalidInput,0,"FSR source identity must advance once per rendered frame");
        auto mapped=BuildFsrDispatch(resources,frame,state_->limits); if(!mapped) return std::unexpected(mapped.error());
        ComPtr<ID3D12Device> commandDevice; ComPtr<ID3D12Device> resourceDevice;
        if(FAILED(list->GetDevice(IID_PPV_ARGS(&commandDevice))) || FAILED(resources.color->GetDevice(IID_PPV_ARGS(&resourceDevice))) ||
            commandDevice.Get()!=state_->device.Get() || resourceDevice.Get()!=state_->device.Get())
            return std::unexpected(RuntimeError{ErrorKind::UnsupportedDevice,0,std::format("FSR command list/resources belong to another device (host={}, command={}, color={})",
                static_cast<void*>(state_->device.Get()),static_cast<void*>(commandDevice.Get()),static_cast<void*>(resourceDevice.Get()))});
        auto hr=state_->device->GetDeviceRemovedReason(); if(FAILED(hr)) return Error(ErrorKind::DeviceLost,hr,"FSR device removed before dispatch");
        auto all=resources.All();
        for(std::size_t i=0;i<all.size();++i) {
            if(state_->dispatched && state_->resources[i].Get()!=all[i]) return Error(ErrorKind::InvalidInput,0,"FSR resource identities changed without context retirement");
        }
        // Retain before calling the vendor: a failed dispatch may have recorded
        // partial work. Fixed allocations avoid per-frame ownership allocation.
        for(std::size_t i=0;i<all.size();++i) state_->resources[i]=all[i];
        state_->dispatched=true;
        std::array<D3D12_RESOURCE_BARRIER,7> barriers{}; UINT count{};
        for(auto* resource:all) if(resource) {
            auto& b=barriers[count++]; b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            b.Transition={resource,D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,D3D12_RESOURCE_STATE_COMMON,
                resource==resources.output?D3D12_RESOURCE_STATE_UNORDERED_ACCESS:D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE};
        }
        list->ResourceBarrier(count,barriers.data()); mapped->commandList=list;
        const auto result=state_->runtime->Functions().Dispatch(&state_->context,&mapped->header);
        if(result!=FFX_API_RETURN_OK) {
            state_->poisoned=true;
            return Error(ErrorKind::DispatchFailure,result,"FSR dispatch failed; discard unsubmitted commands and retain context until retirement");
        }
        // The SDK restores externally registered resources to their declared
        // incoming states; make the interop COMMON boundary explicit afterward.
        for(UINT i=0;i<count;++i) std::swap(barriers[i].Transition.StateBefore,barriers[i].Transition.StateAfter);
        list->ResourceBarrier(count,barriers.data());
        state_->sourceId=frame.sourceId; state_->acceptedSource=true; return {};
    }
    Result<void> FsrUpscaler::DestroyAfterRetirement()
    {
        if(state_->dispatched) {
            if(!state_->bridge) return Error(ErrorKind::RetirementFailure,0,"FSR submitted work has no retirement owner");
            const auto hr=state_->bridge->Drain();
            if(FAILED(hr)) return Error(hr==DXGI_ERROR_DEVICE_REMOVED?ErrorKind::DeviceLost:ErrorKind::RetirementFailure,hr,"FSR GPU retirement failed; retain context/runtime/resources");
        }
        if(state_->context) {
            const auto code=state_->runtime->Functions().DestroyContext(&state_->context,nullptr);
            if(code!=FFX_API_RETURN_OK) { state_->poisoned=true; return Error(ErrorKind::ContextFailure,code,"FSR context destruction failed; retain runtime"); }
            state_->context=nullptr;
        }
        for(auto& resource:state_->resources) resource.Reset();
        state_->runtime.reset(); state_->device.Reset(); state_->actual={}; state_->phaseCount=0;
        state_->dispatched=state_->acceptedSource=state_->poisoned=false; return {};
    }
}
