// Source inclusion keeps fault injection and private lifecycle snapshots inside
// this executable; GPU modes use real devices and a pinned, initialized runtime.
// The product exposes no fault injection interface.
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
#define Transition(...) StageTransition(__VA_ARGS__)
#include "../src/NeuralRendering/Stage.cpp"
#undef Transition
#undef Fail
#define Fail BeforeFail
#include "../src/NeuralRendering/BeforeUpscale.cpp"
#undef Fail
#define Fail PreparedFail
#include "../src/NeuralRendering/PreparedBeforeUpscale.cpp"
#undef Fail
#include "../src/NeuralRendering/PostUpscale.cpp"
#define Fail HostFail
#include "../src/NeuralRendering/BeforeHost.cpp"
#undef Fail
#include "nr-runtime/NrTestComStubs.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <cstdio>
#include <cstring>
using namespace TheosRenderPipeline::NeuralRendering;
namespace {
int failures{},allocates{},destroys{},creates{},shutdowns{};
BeforeHost* injectedHost{};
std::weak_ptr<PreparedBeforeUpscale::State> failedPreparation;
void Check(bool ok,const char* message){std::printf("%s %s\n",ok?"PASS":"FAIL",message);failures+=!ok;}
enum class Fault { Alpha, Allocate, AllocatePartial, Abi, Allocator, List, Fence, VendorCreate, SecondVendorCreate, Destroy, NullFeature, Shutdown };
Fault fault;
struct Parameters final : NVSDK_NGX_Parameter {
#define PARAM(T) void Set(const char*,T)override{} NVSDK_NGX_Result Get(const char*,T* out)const override{*out={};return NVSDK_NGX_Result_Success;}
    PARAM(unsigned long long) PARAM(float) PARAM(double) PARAM(unsigned int)
    PARAM(ID3D11Resource*) PARAM(ID3D12Resource*) PARAM(void*)
#undef PARAM
    void Set(const char*,int)override{}
    NVSDK_NGX_Result Get(const char*,int* out)const override{*out=fault==Fault::Abi||fault==Fault::Destroy?1:0;return NVSDK_NGX_Result_Success;}
    void Reset()override{}
} parameters;
uint32_t __cdecl AllocateParameters(NVSDK_NGX_Parameter** out){
    if(injectedHost)failedPreparation=injectedHost->state_->post?injectedHost->state_->post->bridge_.state_:injectedHost->state_->prepared->state_;
    ++allocates;if(fault==Fault::Allocate){*out=nullptr;return 0xbad00002;}*out=&parameters;return fault==Fault::AllocatePartial?0xbad00002:1;
}
uint32_t __cdecl DestroyParameters(NVSDK_NGX_Parameter*){++destroys;return fault==Fault::Destroy?0xbad00002:1;}
uint32_t __cdecl CreateFeature(ID3D12GraphicsCommandList*,uint32_t,NVSDK_NGX_Parameter*,void** out){++creates;if(fault==Fault::SecondVendorCreate&&creates==1){*out=reinterpret_cast<void*>(0x123);return 1;}*out=nullptr;return fault==Fault::NullFeature?1:0xbad00002;}
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
    Check(!stage->Diagnostics().terminal,"Confirmed pre-create rollback is not terminal");
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
// These regressions catch the wrappers converting a proven Stage rollback into
// terminal retention, or the host returning fatal/retrying after safe failure.
RuntimeExports::ShutdownFn realShutdown;
uint32_t __cdecl CountRealShutdown(ID3D12Device* device){++shutdowns;return fault==Fault::Shutdown?0xbad00002:realShutdown(device);}
int GpuFailure(int argc,char** argv){
    if(argc!=6||NrRuntimeResearch::GameRunningOrUnknown()){std::puts("GPU fixture arguments invalid or Skyrim/process enumeration blocked");return 1;}
    const std::string_view layer=argv[3],injected=argv[5];const bool after=std::string_view(argv[4])=="after";
    std::printf("GPU fixture layer=%s placement=%s fault=%s\n",argv[3],argv[4],argv[5]);
    if(injected=="allocate")fault=Fault::Allocate;
    else if(injected=="partial")fault=Fault::AllocatePartial;
    else if(injected=="abi"||injected=="shutdown")fault=injected=="abi"?Fault::Abi:Fault::Shutdown;
    else if(injected=="destroy")fault=Fault::Destroy;
    else if(injected=="create")fault=Fault::VendorCreate;
    else if(injected=="second-create")fault=Fault::SecondVendorCreate;
    else if(injected=="null-feature")fault=Fault::NullFeature;
    else return 1;
    ComPtr<IDXGIFactory6> factory;if(FAILED(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory))))return 77;
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 d{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> candidate;auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));if(hr==DXGI_ERROR_NOT_FOUND)break;if(FAILED(hr)||FAILED(candidate->GetDesc1(&d)))return 1;if(d.VendorId==0x10de&&d.DeviceId==0x2702){adapter=candidate;break;}}
    if(!adapter)return 77;
    StageContract c;c.colorExtent=c.guideExtent={64,32};c.adapterLuid={d.AdapterLuid.LowPart,d.AdapterLuid.HighPart};
    if(FAILED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&c.device))))return 77;
    D3D12_COMMAND_QUEUE_DESC q{};if(FAILED(c.device->CreateCommandQueue(&q,IID_PPV_ARGS(&c.queue))))return 1;
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    const auto device11Result=D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context);if(FAILED(device11Result)){std::printf("D3D11 setup failed: %08x\n",unsigned(device11Result));return 77;}
    StartupSettings startup;startup.community=true;startup.runtimeRoot=std::filesystem::absolute(argv[1]);startup.driverCore=std::filesystem::absolute(argv[2]);startup.sourceEncoding=TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22;startup.sdrBytesTrial=true;
    BeforeHost host;auto inspected=host.Inspect(device.Get(),startup,std::filesystem::absolute("nr-initialization-failure-cache"),c.device.Get());
    if(!inspected){std::puts(inspected.error().message.c_str());return 1;}
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{host.state_->nrFile,startup.driverCore,std::filesystem::absolute("nr-initialization-failure-cache"),true});
    auto opened=owner->Open(*host.state_->profile,c.device.Get(),host.state_->adapter);if(!opened){std::puts(opened.error().message.c_str());return 1;}
    // Replace only external operations below the real lifecycle owner. Genuine
    // init, devices, interop allocations, callback claims and shutdown stay real.
    owner->state_->exports.allocate=AllocateParameters;owner->state_->exports.destroy=DestroyParameters;owner->state_->exports.create=CreateFeature;
    realShutdown=owner->state_->exports.shutdown;owner->state_->exports.shutdown=CountRealShutdown;
    const bool safe=fault==Fault::Allocate||fault==Fault::AllocatePartial||fault==Fault::Abi;
    const unsigned passes=fault==Fault::SecondVendorCreate?3:1;
    if(layer=="wrapper"){
        auto wrapper=std::make_unique<PreparedBeforeUpscale>();auto& prepared=*wrapper;
        std::weak_ptr<PreparedBeforeUpscale::State> lifetime=prepared.state_;
        auto failed=prepared.Initialize(owner,device.Get(),c,0,nullptr,ColorDomain::SdrBytes,after?Placement::After:Placement::Before,passes);
        Check(!failed,"Injected wrapper initialization fails");
        if(safe){
            Check(!prepared.Diagnostics().terminal&&!prepared.state_->uncertain&&!prepared.state_->retainedSelf,"Safe wrapper rollback releases self-retention and terminal state");
            Check(!prepared.state_->bridge.state_->uncertain&&prepared.InitializationRolledBackBeforeCreate()&&owner->state_->clients==0&&!activeAllocation,"Safe wrapper preserves stage rollback proof and releases bridge retention");
            SettingsSnapshot off;auto bypass=prepared.Evaluate({},startup.sourceEncoding,off);Check(bypass&&!bypass->evaluated,"Safe failed wrapper permits disabled passthrough");
            Check(bool(prepared.Retire())&&bool(owner->Retire()),"Safe wrapper permits genuine runtime retirement");
        }else{
            Check(prepared.Diagnostics().terminal&&prepared.state_->uncertain&&prepared.state_->retainedSelf,"Uncertain wrapper failure retains every owner");
            Check(!prepared.Retire()&&!owner->Retire(),"Uncertain wrapper failure forbids retirement retry");
        }
        wrapper.reset();Check(lifetime.expired()==safe,"Wrapper destruction frees proven-safe preparation and retains uncertain preparation");
    }else if(layer=="host"){
        host.state_->owner=owner;host.state_->uncertain=true;host.state_->contract=c;
        // Simulate a menu restart after the previous preparation retired safely.
        host.state_->active=injected!="partial";host.state_->recorded=injected=="partial"?0:7;
        std::vector<unsigned char> bytes(64*32*4,83);ComPtr<ID3D11Texture2D> color;
        D3D11_TEXTURE2D_DESC t{};t.Width=64;t.Height=32;t.MipLevels=t.ArraySize=t.SampleDesc.Count=1;t.Format=DXGI_FORMAT_R8G8B8A8_UNORM;t.Usage=D3D11_USAGE_DEFAULT;t.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA pixels{bytes.data(),64*4,0};if(FAILED(device->CreateTexture2D(&t,&pixels,&color)))return 1;
        BeforeInput input;input.context=context;input.color=color;input.depth=color;input.motion=color;input.colorExtent=input.guideExtent={64,32};input.epoch=input.guideEpoch=input.sourceId=input.guideSourceId=1;
        PostSrInput post;post.resources=input;auto& m=post.source;m.backend=TheosRenderPipeline::Upscaling::BackendKind::Fsr;m.outcome=TheosRenderPipeline::Upscaling::UpscaleOutcome::Temporal;m.epoch=m.guideEpoch=m.sourceId=m.guideSourceId=1;m.render=m.display=m.color=m.guides={64,32};m.colorDomain=ColorDomain::SdrBytes;m.encoding=startup.sourceEncoding;m.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;m.depthFormat=DXGI_FORMAT_R32_FLOAT;m.motionFormat=DXGI_FORMAT_R16G16_FLOAT;m.guideOrigin=GuideOrigin::RealSource;m.motion.currentToPrevious=true;m.motion.scaleX=64;m.motion.scaleY=32;
        SettingsSnapshot enabled;enabled.enabled=true;enabled.placement=after?Placement::After:Placement::Before;enabled.passes=int(passes);
        auto run=[&]{return after?host.EvaluatePost(post,enabled):host.Evaluate(input,enabled);};
        // A shutdown failure also begins with the safe ABI rejection, then fails
        // genuine runtime retirement. It must not disable the host as healthy.
        if(fault==Fault::Shutdown)owner->state_->exports.allocate=+[](NVSDK_NGX_Parameter** out)->uint32_t{++allocates;*out=nullptr;return 0xbad00002;};
        injectedHost=&host;auto result=run();injectedHost=nullptr;
        if(safe){
            const bool expectedReset=injected=="partial"?false:true;
            Check(result&&!result->evaluated&&result->effectiveReset==expectedReset&&!host.Terminal()&&!host.Available()&&!host.Active(),"Safe host failure disables NR and preserves initial/restart source reset");
            Check(!host.state_->HasPreparation()&&!host.state_->owner&&!host.state_->uncertain,"Safe host rollback releases preparations and runtime");
            Check(failedPreparation.expired(),"Safe host destroys the failed preparation without a self-retention cycle");
            if(injected=="allocate"||injected=="partial")Check(host.Status().find("3134193666")!=std::string::npos,"Latched unavailable status retains the vendor native failure code");
            const auto allocations=allocates,creationCalls=creates,shutdownCalls=shutdowns;
            for(int i=0;i<3;++i){auto bypass=run();Check(bypass&&!bypass->evaluated&&!bypass->effectiveReset,"Latched unavailable host passes later sources without forced resets");}
            Check(allocates==allocations&&creates==creationCalls&&shutdowns==shutdownCalls,"Latched unavailable host never retries initialization");
            Check(bool(host.Retire())&&!host.Terminal(),"Safe failed host retires normally");
        }else{
            Check(!result&&host.Terminal()&&host.state_->HasPreparation()&&host.state_->uncertain,"Uncertain host initialization remains terminal and retained");
            const auto calls=creates;Check(!run()&&creates==calls&&!host.Retire(),"Terminal host never retries feature creation or teardown");
        }
        ComPtr<ID3D11Texture2D> readback;t.Usage=D3D11_USAGE_STAGING;t.BindFlags=0;t.CPUAccessFlags=D3D11_CPU_ACCESS_READ;if(FAILED(device->CreateTexture2D(&t,nullptr,&readback)))return 1;context->CopyResource(readback.Get(),color.Get());D3D11_MAPPED_SUBRESOURCE mapped{};if(FAILED(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped)))return 1;
        bool unchanged=true;for(UINT y=0;y<32;++y)unchanged&=std::memcmp(static_cast<unsigned char*>(mapped.pData)+y*mapped.RowPitch,bytes.data()+y*64*4,64*4)==0;context->Unmap(readback.Get(),0);Check(unchanged,"Initialization failure leaves real source color bytes unchanged");
    }else return 1;
    return failures?1:0;
}
int main(int argc,char** argv){
    setvbuf(stdout,nullptr,_IONBF,0);
    if(argc>1)return GpuFailure(argc,argv);
    EarlyFailure(Fault::Alpha,"Alpha construction failure injected");
    EarlyFailure(Fault::Allocate,"Parameter allocation failure injected");
    EarlyFailure(Fault::AllocatePartial,"Partial parameter allocation failure injected");
    EarlyFailure(Fault::Abi,"Parameter ABI rejection injected");
    EarlyFailure(Fault::Allocator,"Command allocator failure injected");
    EarlyFailure(Fault::List,"Command list failure injected");
    EarlyFailure(Fault::Fence,"Creation fence failure injected");
    fault=Fault::Allocate;ComPtr<Device> lostDevice;lostDevice.Attach(new Device);lostDevice->removed=true;
    ComPtr<Queue> lostQueue;lostQueue.Attach(new Queue);lostQueue->device=lostDevice.Get();auto lostOwner=Owner(lostDevice.Get());
    StageContract lostContract{lostDevice,lostQueue,{123,0},{64,32},{64,32}};
    {Stage lostStage;auto failed=lostStage.Initialize(lostOwner,lostContract);
        Check(!failed&&lostStage.Diagnostics().terminal&&!lostStage.InitializationRolledBackBeforeCreate(),"Device loss before creation never proves healthy rollback");
        Check(!lostStage.Retire(),"Removed-device stage forbids teardown retry");}
    Check(lostOwner->state_->clients==1&&activeAllocation&&!lostOwner->Retire(),"Removed-device early failure retains runtime client and allocator claim");
    activeAllocation=nullptr;
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
    fault=Fault::SecondVendorCreate;creates=allocates=destroys=0;
    auto chainedOwner=Owner(device.Get());
    {Stage chain;auto failed=chain.Initialize(chainedOwner,c,0,nullptr,3);
        Check(!failed&&creates==2&&allocates==3&&destroys==0,"Second pass creation failure retains every feature creation parameter");
        Check(chain.state_->features[0].handle==reinterpret_cast<void*>(0x123)&&chain.Diagnostics().terminal,"Partial chain retains earlier vendor feature handle");
        Check(!chain.Retire(),"Partial chain failure never releases uncertain vendor recordings");}
    Check(chainedOwner->state_->clients==1&&activeAllocation&&!chainedOwner->Retire()&&queue->executions==0,"Partial chain destructor retains client and global allocator ownership without submission");
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
