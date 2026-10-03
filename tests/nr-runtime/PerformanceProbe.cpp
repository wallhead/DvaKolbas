#include "NeuralRendering/PreparedBeforeUpscale.h"
#include "NeuralRendering/RuntimeFileLease.h"
#include "NeuralRendering/PerformanceMetrics.h"
#include "ProbeBuildIdentity.h"
#include "GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <fstream>
#include <iomanip>
#include <set>
#include <cstdio>
#include <mmsystem.h>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
struct Options {
    std::string profile{"rtx40"};std::filesystem::path dll,core,output;
    unsigned width{2560},height{1440},frames{300},warmup{120},timerPeriodMs{1};bool enabled{true},instrumentation{true},readback{};
} options;
struct WallSample {uint64_t id{},nanoseconds{};};
PerformanceMetrics metrics;std::vector<WallSample> wall;
std::string runtimeHash,failure;
StageDiagnostics diagnostics;AdapterIdentity adapterIdentity;
uint32_t rawInit{},rawShutdown{};uint64_t alphaPixels{},changedPixels{},expectedAlpha{};
uint64_t shimSlotRva{};bool shimRestored{};
std::set<uint64_t> hashes;bool retired{},timingComplete{};
void Write(){
    if(options.output.empty())return;std::filesystem::create_directories(options.output.parent_path());std::ofstream out(options.output);
    out<<std::setprecision(12)<<"{\n\"schema\":1,\"scope\":\"standalone prepared Before NR transaction; no FSR/FG/Skyrim FPS\","
        <<"\"sourceRevision\":"<<std::quoted(NrRuntimeResearch::buildRevision)<<",\"cleanSource\":"<<(NrRuntimeResearch::buildClean?"true":"false")
        <<",\"compiledSourceSha256\":"<<NrRuntimeResearch::compiledSourcesJson
        <<",\"profile\":"<<std::quoted(options.profile)<<",\"runtimeSha256\":"<<std::quoted(runtimeHash)
        <<",\"driverCoreSha256\":"<<std::quoted(std::string(QualifiedProbeDriverCore().sha256))
        <<",\"adapterVendor\":"<<adapterIdentity.vendorId<<",\"adapterDevice\":"<<adapterIdentity.deviceId
        <<",\"adapterLuidLow\":"<<adapterIdentity.luid.low<<",\"adapterLuidHigh\":"<<adapterIdentity.luid.high
        <<",\"width\":"<<options.width<<",\"height\":"<<options.height
        <<",\"placement\":\"Before\",\"encoding\":\"Gamma22-to-linear-FP16\",\"passes\":1,\"preset\":0,\"style\":0,\"intensity\":1,\"localTone\":0,\"localStructure\":1,\"inputScale\":1,\"resolve\":\"Auto\",\"hdr\":false"
        <<",\"scene\":\"two static checker/ramp images; constant depth 0.5 and zero motion\",\"resetSchedule\":\"first source; optional correctness-only off/on\""
        <<",\"nrEnabled\":"<<(options.enabled?"true":"false")<<",\"instrumentation\":"<<(options.instrumentation?"true":"false")
        <<",\"readbacks\":"<<(options.readback?"true":"false")<<",\"debugLayer\":false,\"warmup\":"<<options.warmup<<",\"requestedSamples\":"<<options.frames
        <<",\"timerPeriodMs\":"<<options.timerPeriodMs
        <<",\"rawInit\":"<<rawInit<<",\"rawShutdown\":"<<rawShutdown<<",\"retired\":"<<(retired?"true":"false")
        <<",\"shimSlotRva\":"<<shimSlotRva<<",\"shimRestored\":"<<(shimRestored?"true":"false")
        <<",\"recorded\":"<<diagnostics.recorded<<",\"gpuTiming11Available\":"<<(diagnostics.gpuTiming11Available?"true":"false")
        <<",\"gpuTiming12Available\":"<<(diagnostics.gpuTiming12Available?"true":"false")<<",\"gpuTimingDropped\":"<<diagnostics.gpuTimingDropped
        <<",\"alphaPixels\":"<<alphaPixels<<",\"expectedAlphaPixels\":"<<expectedAlpha<<",\"changedRgbPixels\":"<<changedPixels<<",\"distinctOutputHashes\":"<<hashes.size()
        <<",\"timingComplete\":"<<(timingComplete?"true":"false")<<",\"failure\":"<<std::quoted(failure);
    const auto snapshot=metrics.Snapshot();out<<",\"droppedFrames\":"<<snapshot.droppedFrames<<",\"droppedIntervals\":"<<snapshot.droppedIntervals<<",\"samples\":[";
    size_t count{};for(const auto& sample:wall){if(sample.id<=options.warmup)continue;if(count++)out<<',';
        out<<"{\"source\":"<<sample.id<<",\"wallMilliseconds\":"<<double(sample.nanoseconds)/1e6;
        const PerformanceFrame* frame{};for(const auto& f:snapshot.frames)if(f.sourceId==sample.id){frame=&f;break;}
        if(frame){out<<",\"waitCalls\":"<<frame->waitCalls<<",\"blockingCalls\":"<<frame->blockingCalls<<",\"blockMilliseconds\":"<<double(frame->blockNanoseconds)/1e6
            <<",\"flushes\":"<<frame->flushes<<",\"descriptorCreations\":"<<frame->descriptorCreations<<",\"submitted\":"<<frame->submitted<<",\"completed\":"<<frame->completed
            <<",\"cpuUnionMilliseconds\":"<<double(frame->cpuUnionNanoseconds)/1e6<<",\"cpuMilliseconds\":{";
            for(size_t p=0;p<size_t(CpuPhase::Count);++p){if(p)out<<',';out<<std::quoted(std::string(PhaseName(CpuPhase(p))))<<':'<<double(frame->cpuNanoseconds[p])/1e6;}
            out<<"},\"waitMilliseconds\":{";for(size_t p=0;p<size_t(CpuPhase::Count);++p){if(p)out<<',';out<<std::quoted(std::string(PhaseName(CpuPhase(p))))<<':'<<double(frame->waitNanoseconds[p])/1e6;}
            out<<"},\"gpuMilliseconds\":{";for(size_t p=0;p<size_t(GpuPhase::Count);++p){if(p)out<<',';out<<std::quoted(std::string(PhaseName(GpuPhase(p))))<<':';if(frame->gpuMilliseconds[p])out<<*frame->gpuMilliseconds[p];else out<<"null";}out<<'}';}
        out<<'}';}out<<"]\n}\n";
}
[[noreturn]] void Stop(const std::string& text){failure=text;std::fprintf(stderr,"PERF_PROBE_STOP %s\n",text.c_str());try{Write();}catch(...){}ExitProcess(1);}
void Need(bool value,const char* text){if(!value)Stop(text);}
void Gpu(HRESULT hr,const char* text){if(FAILED(hr)){char v[32];std::snprintf(v,sizeof(v)," (0x%08x)",unsigned(hr));Stop(std::string(text)+v);}}
template<class T>T Value(Result<T> r){if(!r)Stop(r.error().message);return std::move(*r);}
void Value(Result<void> r){if(!r)Stop(r.error().message);}
bool Boolean(std::wstring_view value){Need(value==L"on"||value==L"off","Boolean option needs on/off");return value==L"on";}
unsigned Number(const wchar_t* text){size_t consumed{};const std::wstring value(text);const auto number=std::stoull(value,&consumed);
    Need(consumed==value.size()&&number<=UINT_MAX&&value.front()!=L'-',"invalid bounded number");return unsigned(number);}
}
int wmain(int argc,wchar_t** argv){try{
    for(int i=1;i<argc;++i){Need(i+1<argc,"option needs a value");const std::wstring_view key=argv[i];const auto* value=argv[++i];
        if(key==L"--profile")options.profile=std::filesystem::path(value).string();else if(key==L"--dll")options.dll=value;
        else if(key==L"--core")options.core=value;else if(key==L"--output")options.output=std::filesystem::absolute(value);
        else if(key==L"--frames")options.frames=Number(value);else if(key==L"--warmup")options.warmup=Number(value);
        else if(key==L"--timer-period-ms")options.timerPeriodMs=Number(value);
        else if(key==L"--width")options.width=Number(value);else if(key==L"--height")options.height=Number(value);
        else if(key==L"--enabled")options.enabled=Boolean(value);else if(key==L"--instrumentation")options.instrumentation=Boolean(value);
        else if(key==L"--readback")options.readback=Boolean(value);else Stop("unknown option");}
    Need(!options.output.empty(),"output required");Need(!NrRuntimeResearch::GameRunningOrUnknown(),"Skyrim running or process inventory unavailable; refused");
    Need(options.frames>=2&&options.frames<=1000&&options.warmup<=240&&options.width>=16&&options.height>=16&&options.width<=3840&&options.height<=2160,"bounded workload violated");
    Need(options.timerPeriodMs<=1,"timer period must be 0/system or 1 ms");if(options.timerPeriodMs)Need(timeBeginPeriod(options.timerPeriodMs)==TIMERR_NOERROR,"cannot request probe-local timer period");
    const RuntimeProfile* profile{};for(const auto& p:RuntimeCatalog())if(p.id==options.profile){profile=&p;break;}Need(profile!=nullptr,"unknown exact research profile");
    auto lease=Value(RuntimeFileLease::Open(options.dll,*profile));runtimeHash=lease.Sha256();auto core=Value(RuntimeFileLease::Open(options.core,QualifiedProbeDriverCore()));
    ComPtr<IDXGIFactory6> factory;Gpu(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)),"factory");ComPtr<IDXGIAdapter1> adapter;
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> candidate;const auto result=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));if(result==DXGI_ERROR_NOT_FOUND)break;Gpu(result,"enumerate adapter");
        DXGI_ADAPTER_DESC1 d{};Gpu(candidate->GetDesc1(&d),"adapter descriptor");AdapterIdentity id{d.VendorId,d.DeviceId,d.SubSysId,{d.AdapterLuid.LowPart,d.AdapterLuid.HighPart},bool(d.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)};
        const ArtifactIdentity artifact{profile->id,true,true};if(SelectRuntime(id,profile->id,{&artifact,1}).profile){adapter=candidate;adapterIdentity=id;break;}}
    Need(bool(adapter),"eligible adapter unavailable");StageContract contract;contract.colorExtent=contract.guideExtent={options.width,options.height};contract.adapterLuid=adapterIdentity.luid;
    Gpu(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&contract.device)),"NR device");D3D12_COMMAND_QUEUE_DESC q{};Gpu(contract.device->CreateCommandQueue(&q,IID_PPV_ARGS(&contract.queue)),"NR queue");
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;Gpu(D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context),"D3D11 device");
    const bool shim=profile->compatibility==CompatibilityPolicy::CallerIdentityProbeRequired;
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{options.dll,options.core,options.output.parent_path()/"cache",shim});
    auto opened=owner->Open(*profile,contract.device.Get(),adapterIdentity);rawInit=owner->LastInitResult();if(!opened)Stop(opened.error().message);
    shimSlotRva=owner->ShimSlotRva();Need(!shim||shimSlotRva!=0,"shim profile has no owned slot");
    metrics.Enable(options.instrumentation);PreparedBeforeUpscale prepared;Value(prepared.Initialize(owner,device.Get(),contract,0,&metrics));
    const auto texture=[&](DXGI_FORMAT format,UINT bind,bool staging=false){D3D11_TEXTURE2D_DESC d{};d.Width=options.width;d.Height=options.height;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=format;d.BindFlags=bind;d.Usage=staging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;d.CPUAccessFlags=staging?D3D11_CPU_ACCESS_READ:0;ComPtr<ID3D11Texture2D> result;Gpu(device->CreateTexture2D(&d,nullptr,&result),"source texture");return result;};
    auto color=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE);
    auto depth=texture(DXGI_FORMAT_R32_TYPELESS,D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE);auto motion=texture(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);
    ComPtr<ID3D11Texture2D> readback;if(options.readback)readback=texture(DXGI_FORMAT_R8G8B8A8_UNORM,0,true);
    ComPtr<ID3D11DepthStencilView> dsv;D3D11_DEPTH_STENCIL_VIEW_DESC ds{};ds.Format=DXGI_FORMAT_D32_FLOAT;ds.ViewDimension=D3D11_DSV_DIMENSION_TEXTURE2D;Gpu(device->CreateDepthStencilView(depth.Get(),&ds,&dsv),"source DSV");context->ClearDepthStencilView(dsv.Get(),D3D11_CLEAR_DEPTH,.5f,0);
    const auto pixels=size_t(options.width)*options.height;std::vector<uint16_t> velocities(pixels*2);context->UpdateSubresource(motion.Get(),0,nullptr,velocities.data(),options.width*4,0);
    std::array<std::vector<unsigned char>,2> pattern;
    for(size_t f=0;f<pattern.size();++f){pattern[f].resize(pixels*4);for(UINT y=0;y<options.height;++y)for(UINT x=0;x<options.width;++x){const auto p=(size_t(y)*options.width+x)*4;pattern[f][p]=((x/8+y/8+f)%2)?48:208;pattern[f][p+1]=(x+f)%256;pattern[f][p+2]=(y+f)%256;pattern[f][p+3]=(x+y+f)%256;}}
    BeforeInput input;input.context=context;input.color=color;input.depth=depth;input.motion=motion;input.colorExtent=input.guideExtent=contract.colorExtent;input.epoch=input.guideEpoch=1;input.motionScaleX=float(options.width);input.motionScaleY=float(options.height);
    SettingsSnapshot settings;settings.revision=1;settings.tuning.localToneStrength=0;
    for(UINT frame=0;frame<options.frames+options.warmup;++frame){const auto& data=pattern[frame%pattern.size()];context->UpdateSubresource(color.Get(),0,nullptr,data.data(),options.width*4,0);
        input.sourceId=input.guideSourceId=frame+1;input.previousSourceId=frame;input.presentationTime=double(frame+1)/60.;settings.enabled=options.enabled&&!(options.readback&&frame==10);
        if(options.readback&&(frame==10||frame==11))++settings.revision;
        metrics.BeginFrame(input.sourceId,settings.enabled);const auto begin=PerformanceNow();auto evaluated=prepared.Evaluate(input,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,settings);const auto end=PerformanceNow();
        wall.push_back({input.sourceId,end-begin});metrics.EndFrame();if(!evaluated)Stop(evaluated.error().message);Need(evaluated->evaluated==settings.enabled,"NR enable state mismatch");
        if(options.readback){context->CopyResource(readback.Get(),color.Get());D3D11_MAPPED_SUBRESOURCE mapped{};Gpu(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped),"correctness readback");uint64_t hash=1469598103934665603ull;
            for(UINT y=0;y<options.height;++y){const auto* row=static_cast<const unsigned char*>(mapped.pData)+size_t(y)*mapped.RowPitch;for(UINT x=0;x<options.width;++x){const auto p=(size_t(y)*options.width+x)*4;alphaPixels+=row[x*4+3]==data[p+3];if(row[x*4]!=data[p]||row[x*4+1]!=data[p+1]||row[x*4+2]!=data[p+2]){Need(settings.enabled,"disabled source changed");++changedPixels;}
                for(unsigned c=0;c<4;++c){hash^=row[x*4+c];hash*=1099511628211ull;}}}context->Unmap(readback.Get(),0);hashes.insert(hash);expectedAlpha+=pixels;}
    }
    Value(prepared.Retire());diagnostics=prepared.Diagnostics();Value(owner->Retire());rawShutdown=owner->LastShutdownResult();retired=true;shimRestored=shim;
    if(options.readback){Need(alphaPixels==expectedAlpha,"source alpha corrupted");Need(hashes.size()>1,"output constant");if(options.enabled)Need(changedPixels>0,"no changed NR RGB pixels");}
    timingComplete=!options.instrumentation||!options.enabled;
    if(options.instrumentation&&options.enabled){const auto s=metrics.Snapshot();timingComplete=s.droppedFrames==0&&s.droppedIntervals==0&&diagnostics.gpuTimingDropped==0;
        for(const auto& f:s.frames)if(f.sourceId>options.warmup&&f.nrEnabled)timingComplete&=f.waitCalls>0&&f.descriptorCreations==1&&f.submitted==1&&f.completed==1&&
            f.gpuMilliseconds[size_t(GpuPhase::Vendor)].has_value()&&f.gpuMilliseconds[size_t(GpuPhase::Alpha)].has_value()&&f.gpuMilliseconds[size_t(GpuPhase::PrepareColor)].has_value()&&
            f.gpuMilliseconds[size_t(GpuPhase::PrepareGuides)].has_value()&&f.gpuMilliseconds[size_t(GpuPhase::InputCopy)].has_value()&&f.gpuMilliseconds[size_t(GpuPhase::Delivery)].has_value()&&f.gpuMilliseconds[size_t(GpuPhase::Encode)].has_value();}
    Write();if(options.timerPeriodMs)timeEndPeriod(options.timerPeriodMs);std::printf("PERF samples=%u recorded=%llu timingComplete=%u retired=%u\n",options.frames,diagnostics.recorded,timingComplete,retired);return timingComplete?0:2;
}catch(const std::exception& error){Stop(error.what());}}
