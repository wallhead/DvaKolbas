// Source inclusion keeps fault injection and private lifecycle snapshots inside
// this CPU-only executable; the product exposes no fault injection interface.
#include "NeuralRendering/RuntimeCatalog.h"
#include "NeuralRendering/ImagePacket.h"
#include "NeuralRendering/TicketOwnership.h"
#include <memory>
#include <optional>
#define private public
#include "NeuralRendering/RuntimeOwner.h"
#include "NeuralRendering/Stage.h"
#include "NeuralRendering/BeforeHost.h"
#undef private
#include "../src/NeuralRendering/RuntimeOwner.cpp"
#define Fail StageFail
#include "../src/NeuralRendering/Stage.cpp"
#undef Fail
#define Fail HostFail
#include "../src/NeuralRendering/BeforeHost.cpp"
#undef Fail
#include "nr-runtime/NrTestComStubs.h"
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
namespace {
int failures{},allocates{},destroys{},creates{},shutdowns{};
void Check(bool ok,const char* message){std::printf("%s %s\n",ok?"PASS":"FAIL",message);failures+=!ok;}
enum class Fault { Alpha, Allocate, AllocatePartial, Abi, Allocator, List, Fence, VendorCreate, Destroy };
Fault fault;
struct Parameters final : NVSDK_NGX_Parameter {
#define PARAM(T) void Set(const char*,T)override{} NVSDK_NGX_Result Get(const char*,T* out)const override{*out={};return NVSDK_NGX_Result_Success;}
    PARAM(unsigned long long) PARAM(float) PARAM(double) PARAM(unsigned int)
    PARAM(ID3D11Resource*) PARAM(ID3D12Resource*) PARAM(void*)
#undef PARAM
    void Set(const char*,int)override{}
    NVSDK_NGX_Result Get(const char*,int* out)const override{*out=fault==Fault::Abi?1:0;return NVSDK_NGX_Result_Success;}
    void Reset()override{}
} parameters;
uint32_t __cdecl AllocateParameters(NVSDK_NGX_Parameter** out){++allocates;if(fault==Fault::Allocate){*out=nullptr;return 0xbad00002;}*out=&parameters;return fault==Fault::AllocatePartial?0xbad00002:1;}
uint32_t __cdecl DestroyParameters(NVSDK_NGX_Parameter*){++destroys;return fault==Fault::Destroy?0xbad00002:1;}
uint32_t __cdecl CreateFeature(ID3D12GraphicsCommandList*,uint32_t,NVSDK_NGX_Parameter*,void**){++creates;return 0xbad00002;}
uint32_t __cdecl Shutdown(ID3D12Device*){++shutdowns;return 1;}
int inits{};
uint32_t __cdecl RejectInit(uint64_t,const wchar_t*,ID3D12Device*,uint32_t,const void*){++inits;return 0xbad00002;}
struct Device final : NrTestDeviceStub {
    UINT fenceCalls{};bool removed{};
    LUID STDMETHODCALLTYPE GetAdapterLuid()override{return {123,0};}
    HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason()override{return removed?DXGI_ERROR_DEVICE_REMOVED:S_OK;}
    template<class T>HRESULT Child(void** out){auto* child=new T;child->device=this;*out=child;return S_OK;}
    HRESULT STDMETHODCALLTYPE CreateFence(UINT64,D3D12_FENCE_FLAGS,REFIID,void** out)override{++fenceCalls;return fault==Fault::Fence&&fenceCalls%2==0?E_FAIL:Child<NrTestFenceStub>(out);}
    HRESULT STDMETHODCALLTYPE CheckFeatureSupport(D3D12_FEATURE,void* data,UINT)override{if(fault==Fault::Alpha)return E_FAIL;auto& f=*static_cast<D3D12_FEATURE_DATA_FORMAT_SUPPORT*>(data);f.Support2=D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD|D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE;return S_OK;}
    HRESULT STDMETHODCALLTYPE CreateRootSignature(UINT,const void*,SIZE_T,REFIID,void** out)override{return Child<NrTestRootSignatureStub>(out);}
    HRESULT STDMETHODCALLTYPE CreateComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC*,REFIID,void** out)override{return Child<NrTestPipelineStateStub>(out);}
    HRESULT STDMETHODCALLTYPE CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE,REFIID,void** out)override{return fault==Fault::Allocator||fault==Fault::Destroy?E_FAIL:Child<NrTestCommandAllocatorStub>(out);}
    HRESULT STDMETHODCALLTYPE CreateCommandList(UINT,D3D12_COMMAND_LIST_TYPE,ID3D12CommandAllocator*,ID3D12PipelineState*,REFIID,void** out)override{return fault==Fault::List?E_FAIL:Child<NrTestGraphicsCommandListStub>(out);}
};
struct Queue final : NrTestCommandQueueStub {
    UINT executions{};
    D3D12_COMMAND_QUEUE_DESC STDMETHODCALLTYPE GetDesc()override{D3D12_COMMAND_QUEUE_DESC d{};d.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;return d;}
    void STDMETHODCALLTYPE ExecuteCommandLists(UINT,ID3D12CommandList* const*)override{++executions;}
};
std::shared_ptr<RuntimeOwner> Owner(Device* device){
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{});
    owner->state_->device=device;owner->state_->phase=Phase::Ready;owner->state_->profileId="rtx40";
    owner->state_->exports.allocate=AllocateParameters;owner->state_->exports.destroy=DestroyParameters;
    owner->state_->exports.create=CreateFeature;owner->state_->exports.shutdown=Shutdown;
    return owner;
}
void EarlyFailure(Fault injected,const char* name){
    fault=injected;allocates=destroys=creates=0;
    ComPtr<Device> device;device.Attach(new Device);ComPtr<Queue> queue;queue.Attach(new Queue);queue->device=device.Get();
    auto owner=Owner(device.Get());StageContract c{device,queue,{123,0},{64,32},{64,32}};
    auto stage=std::make_unique<Stage>();auto result=stage->Initialize(owner,c);
    Check(!result,name);Check(creates==0&&queue->executions==0,"Early failure never enters vendor creation or submits GPU work");
    stage.reset();Check(owner->state_->clients==0,"Early failure releases retained runtime client");
    Check(activeAllocation==nullptr,"Early failure releases process allocator claim");
    Check(destroys==(injected==Fault::Alpha||injected==Fault::Allocate?0:1),"Early parameter owner is destroyed exactly once");
    {Stage fresh;auto retry=fresh.Initialize(owner,c);Check(!retry&&retry.error().kind!=ErrorKind::Conflict,"Fresh stage can claim callbacks after safe early rollback");}
    Check(owner->state_->clients==0&&activeAllocation==nullptr,"Fresh failed stage also releases its independent ownership");
    // Keep a broken baseline from contaminating later independent cases.
    activeAllocation=nullptr;owner->state_->clients=0;
    Check(bool(owner->Retire()),"Runtime can retire after safe early failure");
}
}
int main(){
    EarlyFailure(Fault::Alpha,"Alpha construction failure injected");
    EarlyFailure(Fault::Allocate,"Parameter allocation failure injected");
    EarlyFailure(Fault::AllocatePartial,"Partial parameter allocation failure injected");
    EarlyFailure(Fault::Abi,"Parameter ABI rejection injected");
    EarlyFailure(Fault::Allocator,"Command allocator failure injected");
    EarlyFailure(Fault::List,"Command list failure injected");
    EarlyFailure(Fault::Fence,"Creation fence failure injected");
    fault=Fault::Destroy;ComPtr<Device> cleanupDevice;cleanupDevice.Attach(new Device);ComPtr<Queue> cleanupQueue;cleanupQueue.Attach(new Queue);cleanupQueue->device=cleanupDevice.Get();
    auto cleanupOwner=Owner(cleanupDevice.Get());StageContract cleanupContract{cleanupDevice,cleanupQueue,{123,0},{64,32},{64,32}};
    {Stage cleanup;Check(!cleanup.Initialize(cleanupOwner,cleanupContract),"Early parameter destruction failure injected");}
    Check(cleanupOwner->state_->clients==1&&activeAllocation&&!cleanupOwner->Retire(),"Failed early cleanup retains uncertain client and callback claim");
    activeAllocation=nullptr;
    fault=Fault::VendorCreate;ComPtr<Device> device;device.Attach(new Device);ComPtr<Queue> queue;queue.Attach(new Queue);queue->device=device.Get();
    auto owner=Owner(device.Get());StageContract c{device,queue,{123,0},{64,32},{64,32}};
    {Stage stage;Check(!stage.Initialize(owner,c)&&creates==1,"Vendor create failure injected");}
    Check(owner->state_->clients==1&&activeAllocation,"Create call retains uncertain runtime client and callback owner");
    Check(!owner->Retire()&&queue->executions==0,"Unsubmitted vendor creation remains quarantined");
    {Stage competitor;auto conflict=competitor.Initialize(owner,c);Check(!conflict&&conflict.error().kind==ErrorKind::Conflict&&owner->state_->clients==1,"Uncertain vendor create blocks a second stage without leaking its extra client");}
    activeAllocation=nullptr;
    auto partial=Owner(device.Get());partial->state_->exports.init=RejectInit;partial->state_->attempted=true;
    auto rejected=partial->state_->InitializeRuntime();
    Check(!rejected&&inits==1&&partial->OpenDisposition()==RuntimeOpenDisposition::InitializationQuarantined,"Init_Ext rejection is typed as session-unavailable quarantine");
    Check(!partial->Open(RuntimeCatalog()[1],device.Get(),{}),"Rejected initialization cannot retry");
    BeforeHost host;host.state_->owner=partial;host.state_->contract.device=device;host.state_->uncertain=true;
    Check(bool(host.state_->DisableAfterOpenFailure(rejected.error())),"Healthy unsubmitted initialization admits source fallback");
    BeforeInput input;input.reset=true;SettingsSnapshot settings;settings.enabled=true;
    auto passthrough=host.Evaluate(input,settings);
    Check(passthrough&&!passthrough->evaluated&&passthrough->effectiveReset&&!host.Terminal(),"Unavailable initialization passes source through");
    const auto calls=shutdowns;
    Check(bool(host.Retire())&&!host.Terminal(),"Session initialization quarantine does not make later host retirement fatal");
    Check(shutdowns==calls&&partial->state_->phase==Phase::Quarantined&&partial->state_->device,"Partial runtime remains retained without shutdown");
    Check(bool(host.Retire())&&inits==1&&shutdowns==calls,"Later host retirement neither retries init nor tears down partial ownership");
    auto blocked=Owner(device.Get());blocked->state_->phase=Phase::Quarantined;
    BeforeHost terminal;terminal.state_->owner=blocked;terminal.state_->contract.device=device;terminal.state_->uncertain=true;
    Check(!terminal.state_->DisableAfterOpenFailure(rejected.error())&&terminal.Terminal(),"Non-initialization quarantine remains fatal");
    device->removed=true;
    BeforeHost removed;removed.state_->owner=partial;removed.state_->contract.device=device;removed.state_->uncertain=true;
    Check(!removed.state_->DisableAfterOpenFailure(rejected.error())&&removed.Terminal(),"Device removal forbids initialization fallback");
    device->removed=false;
    BeforeHost worked;worked.state_->owner=partial;worked.state_->contract.device=device;worked.state_->uncertain=true;worked.state_->recorded=1;
    Check(!worked.state_->DisableAfterOpenFailure(rejected.error())&&worked.Terminal(),"Prior NR frame work forbids initialization fallback");
    BeforeHost laterRemoved;laterRemoved.state_->owner=partial;laterRemoved.state_->contract.device=device;laterRemoved.state_->uncertain=true;
    Check(bool(laterRemoved.state_->DisableAfterOpenFailure(rejected.error())),"Healthy device initially admits session bypass");
    device->removed=true;
    Check(!laterRemoved.Evaluate(input,settings)&&laterRemoved.Terminal(),"Later device removal cannot be hidden by a latched session bypass");
    device->removed=false;
    auto retained=Owner(device.Get());retained->state_->exports.init=RejectInit;retained->state_->attempted=true;
    auto retentionFailure=retained->state_->InitializeRuntime();std::weak_ptr<RuntimeOwner> retainedOwner=retained;
    auto lifetime=std::make_unique<BeforeHost>();lifetime->state_->owner=retained;lifetime->state_->contract.device=device;lifetime->state_->uncertain=true;
    Check(bool(lifetime->state_->DisableAfterOpenFailure(retentionFailure.error()))&&bool(lifetime->Retire()),"Retained initialization owner admits session retirement");
    retained.reset();lifetime.reset();
    Check(!retainedOwner.expired(),"Host destruction retains partial initialization owner until process exit");
    return failures?1:0;
}
