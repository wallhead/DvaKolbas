#include "XessUpscaler.h"
#include "FSRColorContract.h"
#include <vector>
#include <utility>
#include <format>
namespace TheosRenderPipeline::Upscaling
{
    using Microsoft::WRL::ComPtr;
    struct XessUpscaler::State
    {
        std::shared_ptr<XessRuntime> runtime;
        ComPtr<ID3D12Device> device;
        xess_context_handle_t context{};
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;
        std::array<ComPtr<ID3D12Resource>,4> resources;
        struct Reader { ComPtr<ID3D12Fence> fence;uint64_t value{}; };
        std::vector<Reader> readers;
        XessInputPolicy policy{};Extent render{},output{};
        uint32_t requestedFlags{},effectiveFlags{};
        uint64_t sourceId{},sourceEpoch{};
        DWORD thread{};
        bool ready{},recorded{},accepted{},guidesConfigured{},poisoned{};
    };
    namespace
    {
        std::unexpected<RuntimeError> Failure(ErrorKind kind,int64_t native,const char* text)
        { return std::unexpected(RuntimeError{kind,native,text}); }
        uint32_t Flags(const XessInputPolicy& policy)
        { return XESS_INIT_FLAG_LDR_INPUT_COLOR|(policy.depthInverted?XESS_INIT_FLAG_INVERTED_DEPTH:0)|(policy.motion.includesJitter?XESS_INIT_FLAG_JITTERED_MV:0); }
        bool Texture(ID3D12Resource* texture,Extent size,DXGI_FORMAT format,bool output)
        {
            if(!texture)return false;
            const auto desc=texture->GetDesc();
            return desc.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && desc.Width==size.width && desc.Height==size.height && desc.DepthOrArraySize==1 && desc.MipLevels==1 && desc.SampleDesc.Count==1 && desc.Format==format && (!output || (desc.Flags&D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));
        }
    }
    XessUpscaler::XessUpscaler():state_(std::make_unique<State>()){}
    XessUpscaler::~XessUpscaler()
    {
        if(!DestroyAfterRetirement()) (void)state_.release(); // retain complete ownership on failed retirement
    }
    Result<Extent> XessUpscaler::Initialize(std::shared_ptr<XessRuntime> runtime,ID3D12Device* device,Quality quality,Extent output,XessInputPolicy policy)
    {
        if(state_->context || !runtime || !runtime->Functions().CreateContext || !device || !IsKnownColorEncoding(policy.sourceEncoding) || policy.motionGuide!=XessMotionGuide::Undilated)
            return Failure(ErrorKind::InvalidInput,0,"XeSS initialization requires a fresh owner, loaded runtime, device and declared SDR/undilated-guide contract");
        const auto setting=XessQuality(quality);if(!setting)return std::unexpected(setting.error());
        if(!output.width || !output.height)return Failure(ErrorKind::InvalidInput,0,"XeSS output extent is empty");
        const auto removed=device->GetDeviceRemovedReason();if(FAILED(removed))return Failure(ErrorKind::DeviceLost,removed,"XeSS device removed before initialization");
        state_->runtime=std::move(runtime);state_->device=device;state_->thread=GetCurrentThreadId();state_->poisoned=true;
        const auto& api=state_->runtime->Functions();
        auto fail=[&](xess_result_t code,const char* stage)->Result<Extent> {
            const auto cleanup=DestroyAfterRetirement();
            if(!cleanup)return std::unexpected(cleanup.error());
            return Failure(code==XESS_RESULT_ERROR_UNSUPPORTED_DEVICE || code==XESS_RESULT_ERROR_UNSUPPORTED_DRIVER?ErrorKind::UnsupportedDevice:ErrorKind::ContextFailure,code,stage);
        };
        auto code=api.CreateContext(device,&state_->context);
        if(code!=XESS_RESULT_SUCCESS || !state_->context)return fail(code,"xessD3D12CreateContext failed");
        const auto render=QueryXessRenderExtent(api,state_->context,quality,output);
        if(!render){const auto cleanup=DestroyAfterRetirement();return std::unexpected(cleanup?render.error():cleanup.error());}
        state_->render=*render;state_->output=output;state_->policy=policy;state_->requestedFlags=Flags(policy);
        code=api.SetJitterScale(state_->context,1,1);if(code!=XESS_RESULT_SUCCESS)return fail(code,"xessSetJitterScale failed");
        code=api.BuildPipelines(state_->context,nullptr,true,state_->requestedFlags);if(code!=XESS_RESULT_SUCCESS)return fail(code,"xessD3D12BuildPipelines failed");
        xess_d3d12_init_params_t parameters{};
        parameters.outputResolution={output.width,output.height};parameters.qualitySetting=*setting;parameters.initFlags=state_->requestedFlags;
        code=api.Init(state_->context,&parameters);if(code!=XESS_RESULT_SUCCESS)return fail(code,"xessD3D12Init failed");
        xess_d3d12_init_params_t actual{};code=api.GetInitParams(state_->context,&actual);
        if(code!=XESS_RESULT_SUCCESS)return fail(code,"xessD3D12GetInitParams failed");
        xess_version_t version{};code=api.GetVersion(&version);if(code!=XESS_RESULT_SUCCESS)return fail(code,"xessGetVersion failed after initialization");
        const bool ldrRemoved=version.major==2 && version.minor==0 && version.patch==2 && actual.initFlags==(state_->requestedFlags & ~XESS_INIT_FLAG_LDR_INPUT_COLOR);
        if(actual.initFlags!=state_->requestedFlags && !ldrRemoved) {
            const auto message=std::format("XeSS dispatcher {}.{}.{} effective flags {} differ from requested flags {}; runtime flag behavior is not qualified (LDR stripping is qualified only for 2.0.2)",
                version.major,version.minor,version.patch,actual.initFlags,state_->requestedFlags);
            const auto cleanup=DestroyAfterRetirement();
            if(!cleanup)return std::unexpected(cleanup.error());
            return Failure(ErrorKind::IncompatibleAbi,XESS_RESULT_ERROR_INVALID_ARGUMENT,message.c_str());
        }
        if(actual.outputResolution.x!=output.width || actual.outputResolution.y!=output.height || actual.qualitySetting!=*setting)
            return fail(XESS_RESULT_ERROR_INVALID_ARGUMENT,"XeSS effective initialization parameters do not match the validated request");
        state_->effectiveFlags=actual.initFlags;state_->poisoned=false;state_->ready=true;
        return *render;
    }
    Result<void> XessUpscaler::ConfigureGuides(XessInputPolicy policy)
    {
        if(!state_->ready || state_->poisoned || state_->recorded || state_->thread!=GetCurrentThreadId() || Flags(policy)!=state_->requestedFlags || policy.sourceEncoding!=state_->policy.sourceEncoding)
            return Failure(ErrorKind::InvalidInput,0,"XeSS guides must be configured on the owner thread before execution, without changing initialized flags/encoding");
        UpscaleFrame frame{};frame.backend=BackendKind::Xess;frame.render=frame.subrect=state_->render;frame.display=state_->output;
        frame.colorIsLinear=true;frame.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
        frame.motionConvention=policy.motion;frame.camera.depthInverted=policy.depthInverted;frame.sourceId=frame.sourceEpoch=1;
        const auto validated=BuildXessFrameParameters(frame,policy);if(!validated)return std::unexpected(validated.error());
        const auto code=state_->runtime->Functions().SetVelocityScale(state_->context,validated->motionScaleX,validated->motionScaleY);
        if(code!=XESS_RESULT_SUCCESS)return Failure(ErrorKind::ContextFailure,code,"xessSetVelocityScale failed");
        state_->policy=policy;state_->guidesConfigured=true;return {};
    }
    Result<std::array<float,2>> XessUpscaler::QueryJitter(uint64_t sourceId)
    {
        if(!state_->ready || state_->poisoned || state_->thread!=GetCurrentThreadId())return Failure(ErrorKind::ContextFailure,0,"XeSS jitter requires initialized context on its owner thread");
        const auto jitter=GenerateXessJitter(sourceId,state_->render,state_->output);if(!jitter)return std::unexpected(jitter.error());
        return std::array<float,2>{jitter->generatedX,jitter->generatedY};
    }
    bool XessUpscaler::OnOwnerThread() const { return !state_->thread || state_->thread==GetCurrentThreadId(); }
    DWORD XessUpscaler::OwnerThread() const { return state_->thread; }
    XessFrameAdmission XessUpscaler::AdmitFrame(uint64_t sourceId,uint64_t sourceEpoch) const
    {
        // Do not inspect mutable temporal history on the foreign thread.
        if(!OnOwnerThread())return XessFrameAdmission::OffOwnerThread;
        if(state_->ready && !state_->poisoned && state_->accepted && sourceEpoch==state_->sourceEpoch && sourceId<=state_->sourceId)
            return XessFrameAdmission::DuplicateSource;
        return XessFrameAdmission::Ready;
    }
    Result<void> XessUpscaler::Dispatch(ID3D12GraphicsCommandList* list,const XessGpuResources& gpu,const UpscaleFrame& frame)
    {
        if(!state_->ready || state_->poisoned || !state_->guidesConfigured || state_->thread!=GetCurrentThreadId() || !list || !gpu.bridge || !gpu.bridge->Ready() || gpu.bridge->Device12()!=state_->device.Get() || list->GetType()!=D3D12_COMMAND_LIST_TYPE_DIRECT)
            return Failure(ErrorKind::InvalidInput,0,"XeSS dispatch requires configured context, same-device direct list and live retirement bridge");
        if(state_->accepted && frame.sourceEpoch==state_->sourceEpoch && frame.sourceId<=state_->sourceId)
            return Failure(ErrorKind::InvalidInput,0,"XeSS source ID must advance once per accepted source frame");
        auto policy=state_->policy;policy.historySourceEpoch=state_->accepted?std::optional<uint64_t>{state_->sourceEpoch}:std::nullopt;
        const auto parameters=BuildXessFrameParameters(frame,policy);if(!parameters)return std::unexpected(parameters.error());
        if(frame.render!=state_->render || frame.display!=state_->output)return Failure(ErrorKind::InvalidInput,0,"XeSS extent changed without context retirement");
        if(!Texture(gpu.color,state_->render,DXGI_FORMAT_R16G16B16A16_FLOAT,false) || !Texture(gpu.depth,state_->render,DXGI_FORMAT_R32_FLOAT,false) || !Texture(gpu.motion,state_->render,DXGI_FORMAT_R16G16_FLOAT,false) || !Texture(gpu.output,state_->output,DXGI_FORMAT_R16G16B16A16_FLOAT,true))
            return Failure(ErrorKind::InvalidInput,0,"XeSS texture formats, extents or UAV flags do not match the frame contract");
        const auto all=gpu.All();ComPtr<ID3D12Device> commandDevice;
        if(FAILED(list->GetDevice(IID_PPV_ARGS(&commandDevice))) || commandDevice.Get()!=state_->device.Get())return Failure(ErrorKind::UnsupportedDevice,0,"XeSS command list belongs to another device");
        for(size_t i=0;i<all.size();++i) {
            ComPtr<ID3D12Device> resourceDevice;
            if(FAILED(all[i]->GetDevice(IID_PPV_ARGS(&resourceDevice))) || resourceDevice.Get()!=state_->device.Get())return Failure(ErrorKind::UnsupportedDevice,0,"XeSS resource belongs to another device");
            for(size_t j=0;j<i;++j)if(all[i]==all[j])return Failure(ErrorKind::InvalidInput,0,"XeSS input/output resources must not alias");
            if(state_->recorded && state_->resources[i].Get()!=all[i])return Failure(ErrorKind::InvalidInput,0,"XeSS resource identity changed without retirement");
        }
        if(state_->bridge && state_->bridge!=gpu.bridge)return Failure(ErrorKind::InvalidInput,0,"XeSS retirement owner changed without retirement");
        const auto removed=state_->device->GetDeviceRemovedReason();if(FAILED(removed))return Failure(ErrorKind::DeviceLost,removed,"XeSS device removed before execute");
        state_->bridge=gpu.bridge;
        for(size_t i=0;i<all.size();++i)state_->resources[i]=all[i];
        state_->recorded=true; // retain even if vendor records partial work and fails
        std::array<D3D12_RESOURCE_BARRIER,4> barriers{};
        for(size_t i=0;i<all.size();++i) {
            auto& barrier=barriers[i];barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition={all[i],D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,D3D12_RESOURCE_STATE_COMMON,i==3?D3D12_RESOURCE_STATE_UNORDERED_ACCESS:D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE};
        }
        list->ResourceBarrier(static_cast<UINT>(barriers.size()),barriers.data());
        xess_d3d12_execute_params_t execute{};execute.pColorTexture=gpu.color;execute.pDepthTexture=gpu.depth;execute.pVelocityTexture=gpu.motion;execute.pOutputTexture=gpu.output;
        execute.jitterOffsetX=parameters->jitterX;execute.jitterOffsetY=parameters->jitterY;execute.exposureScale=1;
        execute.resetHistory=parameters->resetHistory;execute.inputWidth=state_->render.width;execute.inputHeight=state_->render.height;
        const auto code=state_->runtime->Functions().Execute(state_->context,list,&execute);
        if(code!=XESS_RESULT_SUCCESS){state_->poisoned=true;return Failure(ErrorKind::DispatchFailure,code,"xessD3D12Execute failed; discard unsubmitted list and retain until retirement");}
        for(auto& barrier:barriers)std::swap(barrier.Transition.StateBefore,barrier.Transition.StateAfter);
        list->ResourceBarrier(static_cast<UINT>(barriers.size()),barriers.data());
        state_->accepted=true;state_->sourceId=frame.sourceId;state_->sourceEpoch=frame.sourceEpoch;
        return {};
    }
    Result<void> XessUpscaler::TrackReader(ID3D12Fence* fence,uint64_t value)
    {
        if(!state_->context || !fence || !value || state_->thread!=GetCurrentThreadId())return Failure(ErrorKind::InvalidInput,0,"XeSS reader hold requires an owned context and real fence value");
        ComPtr<ID3D12Device> device;if(FAILED(fence->GetDevice(IID_PPV_ARGS(&device))) || device.Get()!=state_->device.Get())return Failure(ErrorKind::UnsupportedDevice,0,"XeSS reader fence belongs to another device");
        for(auto& reader:state_->readers)if(reader.fence.Get()==fence){reader.value=std::max(reader.value,value);return {};}
        state_->readers.push_back({fence,value});return {};
    }
    Result<void> XessUpscaler::DestroyAfterRetirement()
    {
        if(state_->thread && state_->thread!=GetCurrentThreadId())return Failure(ErrorKind::RetirementFailure,0,"XeSS context destruction must run on its owner thread");
        for(const auto& reader:state_->readers) {
            const auto completed=reader.fence->GetCompletedValue();
            if(completed==UINT64_MAX)return Failure(ErrorKind::DeviceLost,DXGI_ERROR_DEVICE_REMOVED,"XeSS reader device removed; retain resources");
            if(completed<reader.value)return Failure(ErrorKind::RetirementFailure,0,"XeSS downstream reader still pending; retain and retry retirement");
        }
        if(state_->recorded) {
            if(!state_->bridge)return Failure(ErrorKind::RetirementFailure,0,"XeSS recorded work has no retirement bridge");
            const auto hr=state_->bridge->Drain();if(FAILED(hr))return Failure(ErrorKind::RetirementFailure,hr,"XeSS GPU drain failed; retain context/runtime/resources");
        }
        if(state_->context) {
            const auto code=state_->runtime->Functions().DestroyContext(state_->context);
            if(code!=XESS_RESULT_SUCCESS){state_->poisoned=true;return Failure(ErrorKind::ContextFailure,code,"xessDestroyContext failed; retain runtime for retry");}
            state_->context=nullptr;
        }
        *state_=State{};return {};
    }
    uint32_t XessUpscaler::RequestedFlags() const { return state_->requestedFlags; }
    uint32_t XessUpscaler::EffectiveFlags() const { return state_->effectiveFlags; }
}
