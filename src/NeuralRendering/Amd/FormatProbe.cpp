#include "FormatProbe.h"
#include "FormatCodecBytecode.h"
#include <wrl/client.h>
#include <algorithm>
#include <atomic>
#include <cstring>
#include <limits>
#include <new>
namespace TheosRenderPipeline::NeuralRendering::Amd {
using Microsoft::WRL::ComPtr;
struct ProbeJobData {
    std::shared_ptr<int> owner;
    std::size_t count{};
    ComPtr<ID3D12Resource> upload,input,output,readback;
    ComPtr<ID3D12CommandAllocator> allocator;
    ComPtr<ID3D12GraphicsCommandList> list;
    ComPtr<ID3D12Fence> fence;
};
struct ProbeState {
    ComPtr<ID3D12Device> device;ComPtr<ID3D12CommandQueue> queue;
    ComPtr<ID3D12RootSignature> root;ComPtr<ID3D12PipelineState> pipeline;
    std::shared_ptr<int> owner=std::make_shared<int>();
    std::vector<std::shared_ptr<ProbeJobData>> pending;
    bool signalFailed{},injectRetirementFailure{};
    ProbeState* retainedNext{};
};
namespace {
// Intentionally retained until process termination if retirement cannot be proved.
// No allocation is needed on this failure path, and the whole context survives.
std::atomic<ProbeState*> retainedHead{};
std::atomic<std::size_t> retainedCount{};
struct Failure { ProbeError error; };
void Check(HRESULT hr) {if(FAILED(hr)) throw Failure{ProbeError::Api};}
std::expected<void,ProbeError> Wait(const ProbeState& state,const ProbeJobData& job,std::chrono::steady_clock::time_point deadline) {
    if(state.signalFailed || state.injectRetirementFailure) return std::unexpected(ProbeError::Api);
    if(!job.count) return {};
    for(;;) {
        auto value=job.fence->GetCompletedValue();
        if(value==std::numeric_limits<UINT64>::max() || FAILED(state.device->GetDeviceRemovedReason())) return std::unexpected(ProbeError::DeviceRemoved);
        if(value>=1) return {};
        if(std::chrono::steady_clock::now()>=deadline) return std::unexpected(ProbeError::Timeout);
        Sleep(1);
    }
}
ComPtr<ID3D12Resource> Buffer(ID3D12Device* device,UINT64 bytes,D3D12_HEAP_TYPE heap,D3D12_RESOURCE_STATES initial,bool uav=false) {
    D3D12_HEAP_PROPERTIES properties{};properties.Type=heap;
    D3D12_RESOURCE_DESC desc{};desc.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;desc.Width=bytes;desc.Height=1;desc.DepthOrArraySize=1;desc.MipLevels=1;desc.SampleDesc.Count=1;desc.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags=uav?D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS:D3D12_RESOURCE_FLAG_NONE;
    ComPtr<ID3D12Resource> result;Check(device->CreateCommittedResource(&properties,D3D12_HEAP_FLAG_NONE,&desc,initial,nullptr,IID_PPV_ARGS(&result)));return result;
}
void Barrier(ID3D12GraphicsCommandList* list,ID3D12Resource* resource,D3D12_RESOURCE_STATES before,D3D12_RESOURCE_STATES after) {
    D3D12_RESOURCE_BARRIER barrier{};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition={resource,D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,before,after};list->ResourceBarrier(1,&barrier);
}
}
FormatProbe::FormatProbe(std::unique_ptr<ProbeState> state):state_(std::move(state)){}
FormatProbe::FormatProbe(FormatProbe&&) noexcept=default;
FormatProbe& FormatProbe::operator=(FormatProbe&& other) noexcept {if(this!=&other){Shutdown();state_=std::move(other.state_);}return *this;}
FormatProbe::~FormatProbe(){Shutdown();}
void FormatProbe::Shutdown() noexcept {
    if(!state_) return;
    if(!Drain(std::chrono::milliseconds(1000))) {
        auto state=state_.release();state->retainedNext=retainedHead.load();
        while(!retainedHead.compare_exchange_weak(state->retainedNext,state)) {}
        ++retainedCount;
    }
    else state_.reset();
}
std::expected<FormatProbe,ProbeError> FormatProbe::Create(ID3D12Device* device,ID3D12CommandQueue* queue) {
    if(!device) return std::unexpected(ProbeError::InvalidDevice);
    if(!queue || queue->GetDesc().Type!=D3D12_COMMAND_LIST_TYPE_DIRECT) return std::unexpected(ProbeError::InvalidQueue);
    ComPtr<IUnknown> a,b;
    if(FAILED(device->QueryInterface(IID_PPV_ARGS(&a))) || FAILED(queue->GetDevice(IID_PPV_ARGS(&b))) || a.Get()!=b.Get()) return std::unexpected(ProbeError::InvalidQueue);
    try {
        auto state=std::make_unique<ProbeState>();state->device=device;state->queue=queue;
        D3D12_FEATURE_DATA_SHADER_MODEL capability{D3D_SHADER_MODEL_5_1};
        Check(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&capability,sizeof(capability)));
        if(capability.HighestShaderModel<D3D_SHADER_MODEL_5_1) return std::unexpected(ProbeError::InvalidDevice);
        D3D12_ROOT_PARAMETER parameters[3]{};
        parameters[0].ParameterType=D3D12_ROOT_PARAMETER_TYPE_SRV;parameters[0].Descriptor.ShaderRegister=0;
        parameters[1].ParameterType=D3D12_ROOT_PARAMETER_TYPE_UAV;parameters[1].Descriptor.ShaderRegister=0;
        parameters[2].ParameterType=D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;parameters[2].Constants={0,0,2};
        D3D12_ROOT_SIGNATURE_DESC rootDesc{};rootDesc.NumParameters=3;rootDesc.pParameters=parameters;
        ComPtr<ID3DBlob> blob,errors;Check(D3D12SerializeRootSignature(&rootDesc,D3D_ROOT_SIGNATURE_VERSION_1,&blob,&errors));
        Check(device->CreateRootSignature(0,blob->GetBufferPointer(),blob->GetBufferSize(),IID_PPV_ARGS(&state->root)));
        D3D12_COMPUTE_PIPELINE_STATE_DESC pipeline{};pipeline.pRootSignature=state->root.Get();pipeline.CS={AmdNrShader::FormatCodec,sizeof(AmdNrShader::FormatCodec)};
        Check(device->CreateComputePipelineState(&pipeline,IID_PPV_ARGS(&state->pipeline)));
        return FormatProbe(std::move(state));
    } catch(const Failure& e){return std::unexpected(e.error);}catch(const std::bad_alloc&){return std::unexpected(ProbeError::Memory);}
}
std::expected<FormatProbeJob,ProbeError> FormatProbe::Submit(FormatOperation operation,std::span<const std::uint32_t> input) {
    if(!state_) return std::unexpected(ProbeError::InvalidDevice);
    if(operation!=FormatOperation::Float32ToHalf && operation!=FormatOperation::HalfToFloat32 && operation!=FormatOperation::E4m3ToHalf) return std::unexpected(ProbeError::InvalidOperation);
    if(input.size()>65535u*64) return std::unexpected(ProbeError::Count);
    if(state_->signalFailed || state_->injectRetirementFailure) return std::unexpected(ProbeError::Api);
    try {
        auto job=std::make_shared<ProbeJobData>();job->owner=state_->owner;job->count=input.size();
        if(input.empty()) return FormatProbeJob(std::move(job));
        auto& pending=state_->pending;
        std::erase_if(pending,[&](const auto& p){return bool(Wait(*state_,*p,std::chrono::steady_clock::now()));});
        if(pending.size()>=64) return std::unexpected(ProbeError::Busy);
        auto device=state_->device.Get();const auto bytes=UINT64(input.size())*4;
        job->upload=Buffer(device,bytes,D3D12_HEAP_TYPE_UPLOAD,D3D12_RESOURCE_STATE_GENERIC_READ);
        job->input=Buffer(device,bytes,D3D12_HEAP_TYPE_DEFAULT,D3D12_RESOURCE_STATE_COPY_DEST);
        job->output=Buffer(device,bytes,D3D12_HEAP_TYPE_DEFAULT,D3D12_RESOURCE_STATE_UNORDERED_ACCESS,true);
        job->readback=Buffer(device,bytes,D3D12_HEAP_TYPE_READBACK,D3D12_RESOURCE_STATE_COPY_DEST);
        void* mapped{};D3D12_RANGE empty{};Check(job->upload->Map(0,&empty,&mapped));std::memcpy(mapped,input.data(),std::size_t(bytes));
        D3D12_RANGE written{0,std::size_t(bytes)};job->upload->Unmap(0,&written);
        Check(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&job->allocator)));
        Check(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,job->allocator.Get(),state_->pipeline.Get(),IID_PPV_ARGS(&job->list)));
        auto list=job->list.Get();list->CopyBufferRegion(job->input.Get(),0,job->upload.Get(),0,bytes);
        Barrier(list,job->input.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        list->SetComputeRootSignature(state_->root.Get());list->SetComputeRootShaderResourceView(0,job->input->GetGPUVirtualAddress());list->SetComputeRootUnorderedAccessView(1,job->output->GetGPUVirtualAddress());
        const UINT constants[]{UINT(input.size()),UINT(operation)};list->SetComputeRoot32BitConstants(2,2,constants,0);list->Dispatch(UINT((input.size()+63)/64),1,1);
        Barrier(list,job->output.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_COPY_SOURCE);
        list->CopyBufferRegion(job->readback.Get(),0,job->output.Get(),0,bytes);Check(list->Close());
        Check(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&job->fence)));
        // All potentially allocating work is complete before GPU submission.
        pending.push_back(job);ID3D12CommandList* lists[]{list};state_->queue->ExecuteCommandLists(1,lists);
        if(FAILED(state_->queue->Signal(job->fence.Get(),1))) {state_->signalFailed=true;return std::unexpected(ProbeError::Api);}
        return FormatProbeJob(std::move(job));
    }catch(const Failure& e){return std::unexpected(e.error);}catch(const std::bad_alloc&){return std::unexpected(ProbeError::Memory);}
}
std::expected<std::vector<std::uint32_t>,ProbeError> FormatProbe::Readback(FormatProbeJob& handle,std::chrono::milliseconds timeout) {
    if(!state_ || !handle.data_ || handle.data_->owner!=state_->owner) return std::unexpected(ProbeError::InvalidJob);
    if(timeout.count()<0 || timeout>std::chrono::hours(24)) return std::unexpected(ProbeError::Timeout);
    auto& job=*handle.data_;auto wait=Wait(*state_,job,std::chrono::steady_clock::now()+timeout);
    if(!wait) return std::unexpected(wait.error());
    try {
        std::vector<std::uint32_t> result(job.count);if(!job.count) return result;
        void* mapped{};D3D12_RANGE read{0,job.count*4};Check(job.readback->Map(0,&read,&mapped));
        std::memcpy(result.data(),mapped,read.End);D3D12_RANGE empty{};job.readback->Unmap(0,&empty);return result;
    }catch(const Failure& e){return std::unexpected(e.error);}catch(const std::bad_alloc&){return std::unexpected(ProbeError::Memory);}
}
std::expected<void,ProbeError> FormatProbe::Drain(std::chrono::milliseconds timeout) {
    if(!state_) return std::unexpected(ProbeError::InvalidDevice);
    if(timeout.count()<0 || timeout>std::chrono::hours(24)) return std::unexpected(ProbeError::Timeout);
    if(state_->signalFailed || state_->injectRetirementFailure) return std::unexpected(ProbeError::Api);
    auto deadline=std::chrono::steady_clock::now()+timeout;
    for(const auto& job:state_->pending) {auto waited=Wait(*state_,*job,deadline);if(!waited) return waited;}
    state_->pending.clear();return {};
}
struct FormatProbeTestAccess {
    static void FailRetirement(FormatProbe& probe);
    static std::size_t RetainedContexts();
    static std::size_t RetainedJobs();
};
void FormatProbeTestAccess::FailRetirement(FormatProbe& probe){probe.state_->injectRetirementFailure=true;}
std::size_t FormatProbeTestAccess::RetainedContexts(){return retainedCount.load();}
std::size_t FormatProbeTestAccess::RetainedJobs(){auto state=retainedHead.load();return state?state->pending.size():0;}
}
