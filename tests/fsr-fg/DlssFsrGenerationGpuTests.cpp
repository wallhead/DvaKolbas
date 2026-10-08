#include "DLSSBackend.h"
#include "FrameGen/LoadingScreenUpscaler.h"
#include "FrameGen/SourceNvidiaFrameEvaluator.h"
#include "FrameGen/FSRHostPresentation.h"
#include "NeuralRendering/PostUpscale.h"
#include "NeuralRendering/RuntimeFileLease.h"
#include "PluginPaths.h"
#include "Upscaling/FSRColorConversion.h"
#include <atomic>
#include <set>
#include "PresentationObserver.h"
#include "InteropTestRig.h"
#include "../nr-runtime/GpuProbeGuard.h"
#include <ffx_upscale.h>
#include <dx12/ffx_api_framegeneration_dx12.h>
#include <DirectXPackedVector.h>
#include <fstream>
#include <map>
#include <thread>
#include <bcrypt.h>
#include <iomanip>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
namespace NR=NeuralRendering;
namespace {
void Need(bool value,const char* why){if(!value)throw std::runtime_error(why);}
void Gpu(HRESULT hr){if(FAILED(hr))throw std::runtime_error(std::format("GPU HRESULT 0x{:08X}",unsigned(hr)));}
template<class T>T Value(Result<T> value){if(!value)throw std::runtime_error(value.error().message);return std::move(*value);}
void Accept(Result<void> value){if(!value)throw std::runtime_error(value.error().message);}
template<class T>T Neural(NR::Result<T> value){if(!value)throw std::runtime_error(value.error().message);return std::move(*value);}
void Neural(NR::Result<void> value){if(!value)throw std::runtime_error(value.error().message);}
unsigned srCreates{},srDispatches{},waits{};
PfnFfxCreateContext originalCreate{};PfnFfxDispatch originalDispatch{};
ffxReturnCode_t Create(ffxContext* context,ffxCreateContextDescHeader* header,const ffxAllocationCallbacks* allocator){
    if(header && header->type==FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE)++srCreates;
    return originalCreate(context,header,allocator);
}
ffxReturnCode_t Dispatch(ffxContext* context,const ffxDispatchDescHeader* header){
    if(header && header->type==FFX_API_DISPATCH_DESC_TYPE_UPSCALE)++srDispatches;
    if(header && header->type==FFX_API_DISPATCH_DESC_TYPE_FRAMEGENERATIONSWAPCHAIN_WAIT_FOR_PRESENTS_DX12)++waits;
    return originalDispatch(context,header);
}
ComPtr<ID3D11Texture2D> Texture(ID3D11Device* device,Extent size,DXGI_FORMAT format){
    D3D11_TEXTURE2D_DESC d{};d.Width=size.width;d.Height=size.height;d.MipLevels=d.ArraySize=d.SampleDesc.Count=1;
    d.Format=format;d.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> result;Gpu(device->CreateTexture2D(&d,nullptr,&result));return result;
}
std::vector<uint8_t> Read(ID3D11DeviceContext* context,ID3D11Texture2D* texture){
    ComPtr<ID3D11Device> device;context->GetDevice(&device);D3D11_TEXTURE2D_DESC d{};texture->GetDesc(&d);
    const auto width=d.Width,height=d.Height;d.BindFlags=d.MiscFlags=0;d.Usage=D3D11_USAGE_STAGING;d.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Texture2D> stage;Gpu(device->CreateTexture2D(&d,nullptr,&stage));context->CopyResource(stage.Get(),texture);
    D3D11_MAPPED_SUBRESOURCE mapped{};Gpu(context->Map(stage.Get(),0,D3D11_MAP_READ,0,&mapped));std::vector<uint8_t> bytes(size_t(width)*height*4);
    for(UINT y=0;y<height;++y)std::memcpy(bytes.data()+size_t(y)*width*4,static_cast<const uint8_t*>(mapped.pData)+size_t(y)*mapped.RowPitch,width*4);
    context->Unmap(stage.Get(),0);return bytes;
}
struct Evidence {unsigned sources{},nr{},callbacks{},generatedPixels{},changingGenerated{},uiChecks{},uiFailures{},colorChecks{},changed{},retirements{},retirementFailures{},pendingReaders{},suppressed{},resets{},resetReentries{},styleReentries{},suspensions{},resizes{},policyReentries{};};
struct State {
    InteropFixture::Rig rig;
    HWND window{};
    std::shared_ptr<FsrHostResources> resources;
    FsrHostPresentation presenter;
    std::shared_ptr<NR::RuntimeOwner> nrOwner;
    std::unique_ptr<NR::PostUpscale> post;
    ComPtr<ID3D11Texture2D> world,input,output,depth,motion,ui;
    LoadingScreenUpscaler spatial;
    std::unique_ptr<FgObservation::Capture> capture;
    std::map<uint64_t,std::vector<uint8_t>> enhancedRows;
    std::set<uint64_t> eligible;
    FsrFunctions* functions{};std::weak_ptr<FsrRuntime> observerRuntime;
    PfnFfxConfigure configure{};
    bool created{},retired{};
    explicit State(const std::filesystem::path& plugin):resources(std::make_shared<FsrHostResources>(plugin)){}
    bool Retire(){
        if(retired)return true;
        // The host retires/releases its runtime; keep the observer's hook table
        // alive through reader retirement and restoration.
        const auto retainedRuntime=observerRuntime.lock();
        if(created && !presenter.Retire())return false;
        if(post && !post->Retire())return false;
        if(nrOwner && !nrOwner->Retire())return false;
        if(functions && observerRuntime.expired())return false;
        if(functions){functions->CreateContext=originalCreate;functions->Dispatch=originalDispatch;functions->Configure=configure;functions=nullptr;}
        FgObservation::activeCapture=nullptr;DLSSBackend::GetSingleton()->ReleaseFeature();retired=true;return true;
    }
    ~State(){if(window)DestroyWindow(window);}
};
struct Operations {
    State& state;Evidence& evidence;UpscaleFrame snapshot;NR::SettingsSnapshot settings;bool omitNr{};
    std::vector<uint8_t> final;
    bool ExternalGuideRecoveryEnabled() const { return true; }
    bool RecoverInvalidSourceGuides(const SourceNvidiaFrameInputs& frame) {
        Gpu(state.spatial.Evaluate(state.rig.context11.Get(),frame.input,frame.output));
        snapshot.output=frame.output;snapshot.depth=snapshot.motion=nullptr;snapshot.reset=true;
        final=Read(state.rig.context11.Get(),frame.output);return true;
    }
    void CopyInput(ID3D11DeviceContext* context,const SourceNvidiaFrameInputs& frame){context->CopyResource(frame.input,frame.color);}
    bool EvaluateNeuralBeforeDLSS(SourceNvidiaFrameInputs&){return true;}
    void RenderReShade(const SourceNvidiaFrameInputs&,bool){}
    bool EvaluateDLSS(const SourceNvidiaFrameInputs& f){return DLSSBackend::GetSingleton()->Evaluate(f.input,f.motion,f.depth,f.output,
        snapshot.render.width,snapshot.render.height,0,f.jitterX,f.jitterY,float(snapshot.render.width),float(snapshot.render.height),f.reset);}
    void UpscaleSucceeded(){++evidence.sources;}
    bool EvaluateNeuralAfterDLSS(SourceNvidiaFrameInputs& frame,UpscaleOutcome outcome){
        auto before=Read(state.rig.context11.Get(),frame.output);
        if(!omitNr){NR::PostSrInput input;auto& r=input.resources;
            r.context=state.rig.context11;r.color=frame.output;r.depth=frame.depth;r.motion=frame.motion;
            r.epoch=r.guideEpoch=snapshot.sourceEpoch;r.sourceId=r.guideSourceId=snapshot.sourceId;r.previousSourceId=snapshot.sourceId-1;
            r.presentationTime=double(snapshot.sourceId)/72;r.colorExtent={snapshot.display.width,snapshot.display.height};r.guideExtent={snapshot.render.width,snapshot.render.height};
            r.colorDomain=NR::ColorDomain::SdrBytes;r.motionScaleX=float(snapshot.display.width);r.motionScaleY=float(snapshot.display.height);r.reset=frame.reset;
            auto& m=input.source;m.backend=snapshot.backend;m.outcome=outcome;m.epoch=m.guideEpoch=r.epoch;m.sourceId=m.guideSourceId=r.sourceId;m.previousSourceId=r.previousSourceId;
            m.sourceTime=m.guideTime=r.presentationTime;m.render=m.guides=r.guideExtent;m.display=m.color=r.colorExtent;
            m.colorDomain=r.colorDomain;m.encoding=ColorEncoding::Gamma22;m.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;
            m.depthFormat=DXGI_FORMAT_R32_FLOAT;m.motionFormat=DXGI_FORMAT_R16G16_FLOAT;m.guideOrigin=NR::GuideOrigin::RealSource;m.motion=snapshot.motionConvention;
            auto result=Neural(state.post->Evaluate(input,settings));Neural(state.post->WaitDelivery(result));
            Need(result.evaluated==settings.enabled,"NR enabled preference did not reach the real model");evidence.nr+=result.evaluated;frame.reset|=result.effectiveReset;
        }
        final=Read(state.rig.context11.Get(),frame.output);
        if(settings.enabled&&!omitNr){for(size_t i=0;i<final.size();i+=4)evidence.changed+=std::memcmp(&before[i],&final[i],3)!=0;}
        else Need(final==before,"NR-off mutated the completed NGX image");
        return true;
    }
    GenerationPreparationStatus PrepareGeneration(const SourceNvidiaFrameInputs&,const UpscaleFrame& completed){
        Need(Read(state.rig.context11.Get(),completed.output)==final,"FG did not receive the enhanced real source");
        snapshot=completed;return GenerationPreparationStatus::Succeeded;
    }
};
void VerifyCapture(State& state,Evidence& evidence){
    Need(!state.capture->Failed(),"Present callback observation failed");
    uint64_t previousHash{};unsigned changing{};
    std::set<uint64_t> realIds,generatedIds;
    for(const auto& sample:state.capture->Samples())if(sample.recorded){
        Need(state.enhancedRows.contains(sample.source),"Present observation has no actual enhanced source");
        Need((sample.generated?generatedIds:realIds).insert(sample.source).second,"duplicate source observation");
        if(sample.generated)Need(state.eligible.contains(sample.source),"suppressed source produced generated pixels");
        const auto pitch=state.capture->RowPitch();
        D3D12_RANGE read{0,pitch*4};void* pointer{};Gpu(sample.readback->Map(0,&read,&pointer));auto* p=static_cast<uint8_t*>(pointer);
        const auto close=[](int value,int expected){return std::abs(value-expected)<=2;};
        bool good=p[0]>=253 && p[1]<=2 && p[2]<=2 && p[3]>=253;
        for(unsigned c=0;c<3;++c){const int expected=int(std::round((c==1?128.f:0.f)+p[pitch*2+32+c]*(127.f/255)));good &= close(p[32+c],expected);}
        good &= p[35]>=253;
        // A transparent HUD pixel must preserve the supplied real/generated scene.
        for(unsigned c=0;c<3;++c)good &= close(p[64+c],p[pitch*2+64+c]);
        if(!good)++evidence.uiFailures;
        bool colorGood=true;
        const auto& expected=state.enhancedRows.at(sample.source);
        if(!sample.generated)for(size_t i=0;i<expected.size();i+=4)for(unsigned c=0;c<3;++c){
            const int encoded=int(std::round(255*Value(ConvertColorChannel(expected[i+c]/255.f,ColorEncoding::Gamma22,ColorEncoding::SRGB))));
            colorGood &= close(p[pitch*3+i+c],encoded);
        }
        uint64_t hash=1469598103934665603ull;
        for(UINT x=0;x<state.capture->Width();++x)for(unsigned c=0;c<3;++c)hash=(hash^p[pitch*3+x*4+c])*1099511628211ull;
        D3D12_RANGE noWrite{};sample.readback->Unmap(0,&noWrite);
        Need(good,"native opaque/premultiplied/transparent HUD damaged");
        Need(colorGood,"enhanced real source changed colors during FSR publication");
        evidence.colorChecks+=!sample.generated;++evidence.uiChecks;evidence.generatedPixels+=sample.generated;
        if(sample.generated){if(previousHash && previousHash!=hash)++changing;previousHash=hash;}
    }
    Need(changing>0,"generated scene never changed");evidence.changingGenerated+=changing;
}
void PendingReaderSuspend(State& state,Evidence& evidence){
    auto bridge=state.resources->Bridge();
    ComPtr<ID3D12Fence> gate;Gpu(bridge->Device12()->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)));
    auto* depth=state.resources->Resources().depth;Need(depth!=nullptr,"pending reader requires owned guides");
    auto desc=depth->GetDesc();desc.Flags=D3D12_RESOURCE_FLAG_NONE;
    D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_DEFAULT;
    ComPtr<ID3D12Resource> copied;Gpu(bridge->Device12()->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&copied)));
    ID3D12GraphicsCommandList* list{};Gpu(bridge->Begin(Graphics::InteropWork::FrameGeneration,&list));
    Gpu(Graphics::D3D11D3D12Interop::RecordCopy(list,depth,copied.Get()));
    Gpu(bridge->Queue()->Wait(gate.Get(),1));
    // The CPU release is bounded even on assertion/retirement failure.
    std::atomic<bool> released{};
    std::jthread release([&]{std::this_thread::sleep_for(std::chrono::milliseconds(150));Gpu(gate->Signal(1));released=true;});
    Gpu(bridge->Submit(Graphics::InteropWork::FrameGeneration));
    Need(gate->GetCompletedValue()==0,"reader gate was not pending at retirement admission");
    const auto retainedDepth=state.resources->Depth11();
    auto retired=state.presenter.Suspend();
    if(!retired)++evidence.retirementFailures;
    Accept(std::move(retired));
    Need(released && gate->GetCompletedValue()==1,"suspension returned before the queued guide reader retired");
    Need(state.resources->Depth11()==retainedDepth,"suspension discarded retained guides");
    Accept(state.presenter.Resume());++evidence.pendingReaders;++evidence.suspensions;
}
}
namespace {
    std::string Sha256(const std::filesystem::path& path)
    {
        BCRYPT_ALG_HANDLE algorithm{};BCRYPT_HASH_HANDLE hash{};
        Need(BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0)),"SHA256 algorithm");
        struct Cleanup{BCRYPT_ALG_HANDLE& algorithm;BCRYPT_HASH_HANDLE& hash;std::vector<unsigned char> object;~Cleanup(){if(hash)BCryptDestroyHash(hash);if(algorithm)BCryptCloseAlgorithmProvider(algorithm,0);}}cleanup{algorithm,hash};
        DWORD bytes{},objectSize{};Need(BCRYPT_SUCCESS(BCryptGetProperty(algorithm,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&objectSize),sizeof(objectSize),&bytes,0)),"SHA256 object size");
        cleanup.object.resize(objectSize);std::array<unsigned char,32> digest{};std::array<unsigned char,65536> chunk{};
        Need(BCRYPT_SUCCESS(BCryptCreateHash(algorithm,&hash,cleanup.object.data(),objectSize,nullptr,0,0)),"SHA256 create");
        std::ifstream file(path,std::ios::binary);Need(bool(file),"hash input exists");while(file){file.read(reinterpret_cast<char*>(chunk.data()),chunk.size());const auto count=file.gcount();
            if(count)Need(BCRYPT_SUCCESS(BCryptHashData(hash,chunk.data(),static_cast<ULONG>(count),0)),"SHA256 update");}
        Need(file.eof() && BCRYPT_SUCCESS(BCryptFinishHash(hash,digest.data(),digest.size(),0)),"SHA256 finish");
        std::ostringstream output;output<<std::hex<<std::setfill('0');for(auto byte:digest)output<<std::setw(2)<<unsigned(byte);return output.str();
    }
}

std::string JsonString(const std::string& input){
    std::ostringstream out;out<<'"';for(unsigned char c:input){if(c=='"'||c=='\\')out<<'\\'<<c;else if(c<32)out<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<unsigned(c)<<std::dec;else out<<c;}out<<'"';return out.str();
}
int wmain(int argc,wchar_t** argv){
    std::setvbuf(stdout,nullptr,_IONBF,0);
    std::map<std::wstring,std::filesystem::path> args;bool omitNr{};
    for(int i=1;i<argc;++i){if(std::wstring_view(argv[i])==L"--omit-nr"){omitNr=true;continue;}if(i+1>=argc)return 1;const std::wstring key=argv[i]; ++i; args[key]=argv[i];}
    if(NrRuntimeResearch::GameRunningOrUnknown()){std::puts("REFUSED: keep Skyrim closed for GPU checks");return 1;}
    for(auto key:{L"--quality",L"--nr",L"--core",L"--runtime",L"--output"})if(!args.contains(key)){std::puts("NOT QUALIFIED: missing argument");return 77;}
    const auto quality=args[L"--quality"].string();const int mode=quality=="native"?5:quality=="quality"?2:quality=="performance"?0:-1;
    if(mode<0)return 1;
    const auto report=std::filesystem::absolute(args[L"--output"]);std::filesystem::create_directories(report.parent_path());
    Evidence evidence;std::string failure;std::unique_ptr<State> state;std::string deviceName,fgProvider,mlFgAvailability;
    std::map<std::string,std::string> hashes;
    try {
        const auto plugin=report.parent_path()/("dlss-fsr-runtime-"+quality);
        std::filesystem::create_directories(plugin/"FSR");
        const std::array<const char*,2> names{"amd_fidelityfx_loader_dx12.dll","amd_fidelityfx_framegeneration_dx12.dll"};
        const std::array<const char*,2> expected{TRP_FG_LOADER_SHA,TRP_FG_MODULE_SHA};
        for(size_t i=0;i<names.size();++i){Need(Sha256(args[L"--runtime"]/names[i])==expected[i],"official FSR runtime hash differs from pin");
            std::filesystem::copy_file(args[L"--runtime"]/names[i],plugin/"FSR"/names[i],std::filesystem::copy_options::overwrite_existing);hashes[names[i]]=expected[i];}
        const NR::RuntimeProfile srPin{"dlss-sr-probe","nvngx_dlss.dll","c85f971ce023c9f3492fc7455f0b01a24ba18ea39636407a846902c4360b0b7e",58956400};
        auto srLease=Neural(NR::RuntimeFileLease::Open(PluginPaths::Directory()/"RaZkolbaS/nvngx_dlss.dll",srPin));
        hashes["nr"]=Sha256(args[L"--nr"]);hashes["core"]=Sha256(args[L"--core"]);hashes["dlss"]=srPin.sha256;
        wchar_t executable[MAX_PATH]{};GetModuleFileNameW(nullptr,executable,MAX_PATH);hashes["executable"]=Sha256(executable);
        state=std::make_unique<State>(plugin);
        DXGI_ADAPTER_DESC adapter{};Gpu(state->rig.adapter->GetDesc(&adapter));Need(adapter.VendorId==0x10de,"mixed NGX route needs NVIDIA hardware");
        deviceName=std::format("vendor={:04X} device={:04X}",adapter.VendorId,adapter.DeviceId);
        state->window=CreateWindowExW(0,L"STATIC",L"RaZkolbaS isolated DLSS NR FSR FG",WS_OVERLAPPEDWINDOW,0,0,800,500,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
        Need(state->window!=nullptr,"test HWND unavailable");
        DXGI_SWAP_CHAIN_DESC descriptor{};descriptor.Windowed=TRUE;descriptor.OutputWindow=state->window;descriptor.BufferDesc.Width=640;descriptor.BufferDesc.Height=360;
        descriptor.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;descriptor.SampleDesc.Count=1;descriptor.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;descriptor.BufferCount=1;
        auto renderFor=[mode](Extent display){return mode==5?display:Extent{UINT(display.width*(mode==2?2.f/3:.5f)),UINT(display.height*(mode==2?2.f/3:.5f))};};
        FsrSettings fsr;fsr.quality=Quality::NativeAA;fsr.providerPolicy=ProviderPolicy::MachineLearning; // deliberately inactive INT8/ML SR preference
        fsr.generationProviderPolicy=ProviderPolicy::Analytical;fsr.sourceColorEncoding=ColorEncoding::Gamma22;
        Extent display{640,360},render=renderFor(display);
        Value(state->presenter.CreateExternal(state->rig.factory.Get(),state->rig.device11.Get(),state->resources,descriptor,fsr,render,{}));state->created=true;
        auto* originalChain=state->presenter.SwapChain();fgProvider=state->presenter.GenerationProvider().identity.name;
        const auto catalog=Value(state->resources->Runtime()->EnumerateForEffect(state->resources->Bridge()->Device12(),FsrEffect::FrameGeneration));
        const auto ml=SelectFsrEffectProvider(catalog,FsrEffect::FrameGeneration,ProviderPolicy::MachineLearning);
        mlFgAvailability=ml?ml->identity.name:ml.error().message;
        Need(!GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll") && !state->resources->ContextOwned(),"generation-only path loaded/created FSR SR");
        auto& functions=const_cast<FsrFunctions&>(state->resources->Runtime()->Functions());state->functions=&functions;state->observerRuntime=state->resources->Runtime();
        originalCreate=functions.CreateContext;originalDispatch=functions.Dispatch;state->configure=functions.Configure;
        FgObservation::originalConfigure=functions.Configure;functions.CreateContext=Create;functions.Dispatch=Dispatch;functions.Configure=FgObservation::Configure;
        NR::StageContract contract;contract.device=state->resources->Bridge()->Device12();contract.queue=state->resources->Bridge()->Queue();
        contract.adapterLuid={adapter.AdapterLuid.LowPart,adapter.AdapterLuid.HighPart};
        state->nrOwner=std::make_shared<NR::RuntimeOwner>(NR::RuntimeOwnerPaths{args[L"--nr"],args[L"--core"],plugin/"nr-cache",true});
        Neural(state->nrOwner->Open(NR::RuntimeCatalog()[1],contract.device.Get(),{adapter.VendorId,adapter.DeviceId,adapter.SubSysId,contract.adapterLuid,false}));
        auto* dlss=DLSSBackend::GetSingleton();dlss->SetupDevice(state->rig.device11.Get(),state->rig.context11.Get());
        const auto firstDispatch=dlss->EvalSuccessCount();
        for(unsigned epoch=0;epoch<2;++epoch){
            if(epoch){
                Accept(state->presenter.BeforeResize());Neural(state->post->Retire());state->post.reset();VerifyCapture(*state,evidence);FgObservation::activeCapture=nullptr;
                state->spatial.ResetAfterRetirement();
                ++evidence.retirements;display={768,432};render=renderFor(display);descriptor.BufferDesc.Width=display.width;descriptor.BufferDesc.Height=display.height;
                Value(state->presenter.ResizeExternal(descriptor,render));Need(originalChain==state->presenter.SwapChain(),"resize replaced the presenter");++evidence.resizes;
            }
            contract.colorExtent={display.width,display.height};contract.guideExtent={render.width,render.height};
            state->post=std::make_unique<NR::PostUpscale>();Neural(state->post->Initialize(state->nrOwner,state->rig.device11.Get(),contract,0,nullptr,NR::ColorDomain::SdrBytes,epoch?3:1));
            state->world=Texture(state->rig.device11.Get(),render,DXGI_FORMAT_R8G8B8A8_UNORM);state->input=Texture(state->rig.device11.Get(),render,DXGI_FORMAT_R8G8B8A8_UNORM);
            state->output=Texture(state->rig.device11.Get(),display,DXGI_FORMAT_R8G8B8A8_UNORM);state->ui=Texture(state->rig.device11.Get(),display,DXGI_FORMAT_R8G8B8A8_UNORM);
            state->depth=Texture(state->rig.device11.Get(),render,DXGI_FORMAT_R32_FLOAT);state->motion=Texture(state->rig.device11.Get(),render,DXGI_FORMAT_R16G16_FLOAT);
            Need(dlss->InitUpscale(render.width,render.height,display.width,display.height,DXGI_FORMAT_R8G8B8A8_UNORM,false,true,0,mode),"actual production NGX feature creation failed");
            Need(srLease.Matches(PluginPaths::ModulePath(GetModuleHandleW(L"nvngx_dlss.dll"))),"loaded DLSS differs from held qualified payload");
            state->capture=std::make_unique<FgObservation::Capture>(contract.device.Get(),256,display.width,display.height,true);state->enhancedRows.clear();state->eligible.clear();FgObservation::activeCapture=state->capture.get();
            std::vector<uint32_t> hud(size_t(display.width)*display.height);hud[0]=0xff0000ff;hud[8]=0x80008000;
            state->rig.context11->UpdateSubresource(state->ui.Get(),0,nullptr,hud.data(),display.width*4,0);
            std::vector<uint32_t> colors(size_t(render.width)*render.height);std::vector<float> depths(colors.size(),.5f);
            std::vector<DirectX::PackedVector::HALF> velocities(colors.size()*2);
            
            for(unsigned local=0;local<82;++local){
                Gpu(state->presenter.WaitBeforeProducer());
                const auto source=uint64_t(epoch)*82+local+1;
                for(UINT y=0;y<render.height;++y)for(UINT x=0;x<render.width;++x){const auto i=size_t(y)*render.width+x;
                    const bool object=x>render.width/4+local*2%80 && x<render.width/4+local*2%80+render.width/10;
                    colors[i]=object?0xff1940d0:0xff704025;depths[i]=object?.3f:.6f;
                    velocities[i*2]=DirectX::PackedVector::XMConvertFloatToHalf(object?-2.f/render.width:0.f);velocities[i*2+1]=0;}
                auto* context=state->rig.context11.Get();context->UpdateSubresource(state->world.Get(),0,nullptr,colors.data(),render.width*4,0);
                context->UpdateSubresource(state->depth.Get(),0,nullptr,depths.data(),render.width*4,0);context->UpdateSubresource(state->motion.Get(),0,nullptr,velocities.data(),render.width*4,0);
                UpscaleFrame snapshot;snapshot.backend=mode==5?BackendKind::Dlaa:BackendKind::Dlss;snapshot.sourceId=source;snapshot.sourceEpoch=epoch+1;
                snapshot.render=snapshot.subrect=render;snapshot.display=display;snapshot.deltaMilliseconds=1000.f/72;snapshot.depthFormat=DXGI_FORMAT_R32_FLOAT;snapshot.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
                snapshot.motionConvention={float(render.width),float(render.height),true,false};snapshot.camera.identity=7;snapshot.camera.nearDistance=.1f;snapshot.camera.farDistance=100;
                snapshot.camera.verticalFovRadians=1.04719755f;snapshot.camera.worldUnitsToMeters=1;
                snapshot.camera.view={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};snapshot.camera.projection={1,0,0,0,0,1.7320508f,0,0,0,0,1.001001f,1,0,0,-.1001001f,0};
                SourceNvidiaFrameInputs inputs{};inputs.sourceSnapshot=snapshot;inputs.color=state->world.Get();inputs.input=state->input.Get();inputs.output=state->output.Get();
                inputs.depth=state->depth.Get();inputs.motion=state->motion.Get();inputs.uiColorAndAlpha=state->ui.Get();inputs.hudLessColor=state->output.Get();
                inputs.renderWidth=render.width;inputs.renderHeight=render.height;inputs.outputWidth=display.width;inputs.outputHeight=display.height;
                inputs.motionScaleX=float(render.width);inputs.motionScaleY=float(render.height);inputs.reset=local==0;
                Operations operations{*state,evidence,snapshot,{},omitNr};operations.settings.enabled=local<6||local>=10;operations.settings.placement=NR::Placement::After;
                operations.settings.passes=epoch?3:1;operations.settings.revision=local<32?1:2;operations.settings.tuning.style=local<32?0:1;
                if(local==78)inputs.depth=nullptr;
                if(local==79)inputs.motion=state->output.Get();
                const auto result=SourceNvidiaFrameEvaluator::Evaluate(context,inputs,operations);Need(result.upscaled&&result.prepared,"actual NGX NR source handoff incomplete");
                snapshot=operations.snapshot;
                // Jitter is zero in this scene, so either convention describes
                // the same guides. Exercise actual SDK flag recreation without
                // changing source reconstruction or the NR image.
                snapshot.motionConvention.includesJitter=local==46 || local==47;
                auto policy=state->resources->GenerationInputPolicy();policy.motionIncludesJitter=snapshot.motionConvention.includesJitter;
                Accept(state->resources->EnsureInputPolicy(policy));
                auto* policyDepth=state->resources->Depth11();auto* policyScene=state->presenter.SceneTarget11();
                state->enhancedRows[source]=std::vector<uint8_t>(operations.final.begin()+size_t(display.height/2)*display.width*4,operations.final.begin()+size_t(display.height/2+1)*display.width*4);
                context->CopyResource(state->presenter.SceneTarget11(),state->output.Get());
                const bool requested=local!=16 && local!=17;const bool menu=local==24;
                if(local==20)snapshot.depth=nullptr;
                if(local==21)snapshot.depth=state->world.Get(); // a fresh source with stale/malformed guides
                if(local==23)--snapshot.sourceId; // duplicate guide/source identity must not be admitted
                const auto outcome=(local==28 || local>=78 && local<=79)?UpscaleOutcome::SpatialRecovery:UpscaleOutcome::Temporal;
                const auto hr=state->presenter.Present(snapshot,outcome,state->ui.Get(),nullptr,true,menu,requested,0,0);
                if(local==23){Need(hr==DXGI_ERROR_INVALID_CALL,"duplicate source identity was admitted");++evidence.suppressed;continue;}Gpu(hr);
                const auto status=state->presenter.Status();evidence.callbacks+=status.callback.invocations;
                if(local==46 || local==48) {
                    Need(status.decision.generate && status.decision.reset,"actual guide-policy recreation did not resume with FG reset");
                    Need(state->presenter.SwapChain()==originalChain && state->resources->Depth11()==policyDepth &&
                        state->presenter.SceneTarget11()==policyScene,"actual guide-policy recreation replaced retained buffers");
                    ++evidence.policyReentries;
                }
                if(!requested || menu || local==20 || local==21 || local==28 || local==78 || local==79)Need(!status.decision.generate,"ineligible mixed source generated a frame");
                if(!status.decision.generate)++evidence.suppressed;
                if(status.decision.reset)++evidence.resets;
                // NR requests its own reset on the first valid source after the
                // guide gap; FG suppresses that reset source, then resumes.
                if(local==80 && !omitNr)Need(!status.decision.generate,"NR recovery reset source generated");
                if(local==18 || local==22 || local==25 || local==29 || local==(omitNr?80:81) || (local==33 && !omitNr) || local==41){Need(status.decision.generate && status.decision.reset,"resume did not generate with reset history");++evidence.resetReentries;}
                if(local==32)++evidence.styleReentries;
                if(status.decision.generate)state->eligible.insert(source);
                Need(!state->resources->ContextOwned() && !GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll"),"inactive ML SR preference created an upscaler");
                if(local==40)PendingReaderSuspend(*state,evidence);
            }
        }
        Accept(state->presenter.BeforeResize());Neural(state->post->Retire());state->post.reset();VerifyCapture(*state,evidence);++evidence.retirements;
        Need(dlss->EvalSuccessCount()-firstDispatch==160 && evidence.sources==160,"not every source used actual NGX reconstruction");
        Need(evidence.nr>0 && evidence.changed>0,"no actual NR enhancement observed");
        Need(srCreates==0 && srDispatches==0,"mixed route used FSR SR");
        Need(evidence.callbacks>20 && evidence.generatedPixels>20 && evidence.uiChecks>100,"actual generation/pixel/HUD observations incomplete");
        Need(evidence.resizes==1 && evidence.suspensions==2 && evidence.resetReentries==14 && evidence.pendingReaders==2 && waits>0,"mixed lifecycle observations incomplete");
        Need(evidence.policyReentries==4,"actual FG guide-policy change/restore observations incomplete");
        if(!state->Retire()){++evidence.retirementFailures;throw std::runtime_error("final readers/runtime retirement failed");}++evidence.retirements;state->rig.ValidateDebug();
        std::printf("PASS mixed route quality=%s SR=%u NR=%u FG=%u generatedPixels=%u HUD=%u waits=%u\n",quality.c_str(),evidence.sources,evidence.nr,evidence.callbacks,evidence.generatedPixels,evidence.uiChecks,waits);
    }catch(const std::exception& error){failure=error.what();std::printf("NOT QUALIFIED: %s\n",failure.c_str());if(state&&!state->Retire()){++evidence.retirementFailures;state.release();failure+="; owners quarantined after retirement failure";}}
    std::ofstream out(report);
    out<<"{\"qualified\":"<<(failure.empty()?"true":"false")<<",\"revision\":"<<JsonString(TRP_FG_VALIDATION_REVISION)<<",\"quality\":"<<JsonString(quality)<<",\"device\":"<<JsonString(deviceName)<<",\"fgProvider\":"<<JsonString(fgProvider)<<",\"mlFgAvailability\":"<<JsonString(mlFgAvailability);
    out<<",\"inputPolicyReentries\":"<<evidence.policyReentries;
    out<<",\"dlssDispatches\":"<<evidence.sources<<",\"nrDispatches\":"<<evidence.nr<<",\"fsrSrCreates\":"<<srCreates<<",\"fsrSrDispatches\":"<<srDispatches<<",\"generatedCallbacks\":"<<evidence.callbacks<<",\"generatedPixelReadbacks\":"<<evidence.generatedPixels<<",\"changingGeneratedReadbacks\":"<<evidence.changingGenerated<<",\"uiChecks\":"<<evidence.uiChecks<<",\"uiFailures\":"<<evidence.uiFailures<<",\"colorChecks\":"<<evidence.colorChecks<<",\"readerRetirements\":"<<evidence.retirements<<",\"retirementFailures\":"<<evidence.retirementFailures<<",\"pendingReaderRetirements\":"<<evidence.pendingReaders<<",\"resetReentries\":"<<evidence.resetReentries<<",\"sdkWaits\":"<<waits<<",\"resizes\":"<<evidence.resizes<<",\"suspensions\":"<<evidence.suspensions<<",\"failure\":"<<JsonString(failure)<<",\"hashes\":{";
    bool first=true;for(const auto& [key,value]:hashes){if(!first)out<<',';first=false;out<<JsonString(key)<<':'<<JsonString(value);}out<<"}}\n";out.flush();
    if(!out){std::puts("NOT QUALIFIED: report could not be saved");return 1;}
    return failure.empty()?0:1;
}
