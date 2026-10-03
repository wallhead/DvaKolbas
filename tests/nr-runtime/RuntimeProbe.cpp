#include "NeuralRendering/CallerIdentityShim.h"
#include "NeuralRendering/RuntimeOwner.h"
#include "NeuralRendering/RuntimeParameters.h"
#include "NeuralRendering/Stage.h"
#include "RuntimeProbeReport.h"
#include "ProbeBuildIdentity.h"
#include "GpuProbeGuard.h"
#include <nvsdk_ngx_params.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <DirectXPackedVector.h>
#include <bcrypt.h>
#include <TlHelp32.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
constexpr UINT width=640,height=360;
NrRuntimeResearch::ProbeReport report;
std::filesystem::path reportPath;
std::vector<std::string> outputHashes;
std::vector<std::string> rgbHashes;
AdapterIdentity adapterIdentity;
uint64_t shimRva{};
bool delayedGpuReaderVerified{};
void WriteReport() {
    std::filesystem::create_directories(std::filesystem::absolute(reportPath).parent_path());
    std::ofstream out(reportPath);
    const auto issues=NrRuntimeResearch::Validate(report);
    out<<"{\n\"schema\":1,\"result\":"<<std::quoted(issues.empty()?"PASS":"INCOMPLETE")
        <<",\"sourceRevision\":"<<std::quoted(NrRuntimeResearch::buildRevision)
        <<",\"cleanSource\":"<<(NrRuntimeResearch::buildClean?"true":"false")
        <<",\"compiledSourceSha256\":"<<NrRuntimeResearch::compiledSourcesJson
        <<",\"profile\":"<<std::quoted(report.profile)<<",\"failure\":"<<std::quoted(report.failure)
        <<",\"runtimeSha256\":"<<std::quoted(report.runtimeSha256)<<",\"coreSha256\":"<<std::quoted(report.coreSha256)
        <<",\"vendorId\":"<<adapterIdentity.vendorId<<",\"deviceId\":"<<adapterIdentity.deviceId
        <<",\"adapterLuidLow\":"<<adapterIdentity.luid.low<<",\"adapterLuidHigh\":"<<adapterIdentity.luid.high
        <<",\"standaloneAdapterOnly\":true,\"callerShim\":"<<(report.shimRequested?"true":"false")
        <<",\"shimSlotRva\":"<<shimRva<<",\"rawInit\":"<<report.init<<",\"rawCreate\":"<<report.create
        <<",\"rawEvaluate\":"<<report.evaluate<<",\"rawRelease\":"<<report.release
        <<",\"rawDestroyParameters\":"<<report.destroyParameters<<",\"rawShutdown\":"<<report.shutdown
        <<",\"requestedFrames\":"<<report.requestedFrames<<",\"readbackFrames\":"<<report.readbackFrames
        <<",\"distinctOutputHashes\":"<<report.distinctOutputHashes<<",\"outputPixels\":"<<report.outputPixels
        <<",\"spatiallyVariedFrames\":"<<report.spatiallyVariedFrames
        <<",\"sharedStage\":true,\"sourceAlphaPreservedPixels\":"<<report.sourceAlphaPreservedPixels
        <<",\"delayedGpuReaderVerified\":"<<(delayedGpuReaderVerified?"true":"false")
        <<",\"finitePixels\":"<<report.finitePixels<<",\"overwrittenPixels\":"<<report.overwrittenPixels
        <<",\"changedFromInputPixels\":"<<report.changedFromInputPixels<<",\"allocations\":"<<report.allocations
        <<",\"releases\":"<<report.releases<<",\"runtimeHeldAndMatched\":"<<(report.runtimeHeldAndMatched?"true":"false")
        <<",\"coreHeldAndMatched\":"<<(report.coreHeldAndMatched?"true":"false")
        <<",\"outputReadersRetired\":"<<(report.outputReadersRetired?"true":"false")
        <<",\"shimRestored\":"<<(report.shimRestored?"true":"false")
        <<",\"colorFormat\":\"RGBA16F\",\"guideScenario\":\"static depth/zero motion; changing color pattern, not temporal scene qualification\""
        <<",\"ui\":\"null; no HUD composition in this NR runtime probe\",\"outputSha256\":[";
    for(size_t i=0;i<outputHashes.size();++i){if(i)out<<',';out<<std::quoted(outputHashes[i]);}
    out<<"],\"temporalRgbSha256\":[";
    for(size_t i=0;i<rgbHashes.size();++i){if(i)out<<',';out<<std::quoted(rgbHashes[i]);}
    out<<"],\"unestablished\":[";
    for(size_t i=0;i<issues.size();++i){if(i)out<<',';out<<std::quoted(issues[i]);}out<<"]\n}\n";
    if(!out)throw std::runtime_error("cannot save probe report");
}
[[noreturn]] void Stop(const std::string& message,UINT code=1) {
    report.failure=message;
    std::fprintf(stderr,"NR_PROBE_STOP %s\n",message.c_str());
    try{WriteReport();}catch(...){std::fputs("report write failed\n",stderr);}
    // Research child only: never unwind feature/module/resource owners after a
    // failed operation or unconfirmed recording/submission. No destructor retry.
    ExitProcess(code);
}
void Need(bool condition,const char* message){if(!condition)Stop(message);}
void Gpu(HRESULT code,const char* message){if(FAILED(code)){std::ostringstream text;text<<message<<" HRESULT=0x"<<std::hex<<static_cast<uint32_t>(code);Stop(text.str());}}
template<class T>T Value(Result<T> value){if(!value)Stop(value.error().message);return std::move(*value);}
ComPtr<ID3D12Resource> Texture(ID3D12Device* device,DXGI_FORMAT format,bool uav=false){
    D3D12_RESOURCE_DESC d{};d.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D;d.Width=width;d.Height=height;
    d.DepthOrArraySize=d.MipLevels=1;d.Format=format;d.SampleDesc.Count=1;d.Flags=uav?D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS:D3D12_RESOURCE_FLAG_NONE;
    D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_DEFAULT;ComPtr<ID3D12Resource> r;
    Gpu(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&r)),"texture create");return r;
}
ComPtr<ID3D12Resource> Buffer(ID3D12Device* device,uint64_t bytes,D3D12_HEAP_TYPE type){
    D3D12_RESOURCE_DESC d{};d.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;d.Width=bytes;d.Height=1;d.DepthOrArraySize=d.MipLevels=1;d.SampleDesc.Count=1;d.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    D3D12_HEAP_PROPERTIES heap{};heap.Type=type;ComPtr<ID3D12Resource> r;
    Gpu(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&d,type==D3D12_HEAP_TYPE_UPLOAD?D3D12_RESOURCE_STATE_GENERIC_READ:D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&r)),"buffer create");return r;
}
void Barrier(ID3D12GraphicsCommandList* list,ID3D12Resource* r,D3D12_RESOURCE_STATES from,D3D12_RESOURCE_STATES to){
    D3D12_RESOURCE_BARRIER b{};b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;b.Transition={r,D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,from,to};list->ResourceBarrier(1,&b);
}
struct Transfer {ComPtr<ID3D12Resource> buffer;D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;};
Transfer MakeTransfer(ID3D12Device* device,ID3D12Resource* texture,D3D12_HEAP_TYPE heap){
    const auto desc=texture->GetDesc();Transfer t;uint64_t bytes{};device->GetCopyableFootprints(&desc,0,1,0,&t.footprint,nullptr,nullptr,&bytes);t.buffer=Buffer(device,bytes,heap);return t;
}
void Upload(ID3D12GraphicsCommandList* list,ID3D12Resource* texture,const Transfer& t,const void* pixels,size_t rowBytes){
    unsigned char* mapped{};Gpu(t.buffer->Map(0,nullptr,reinterpret_cast<void**>(&mapped)),"upload map");
    for(UINT y=0;y<height;++y)std::memcpy(mapped+t.footprint.Offset+size_t(y)*t.footprint.Footprint.RowPitch,static_cast<const unsigned char*>(pixels)+size_t(y)*rowBytes,rowBytes);
    t.buffer->Unmap(0,nullptr);D3D12_TEXTURE_COPY_LOCATION dst{},src{};dst.pResource=texture;dst.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    src.pResource=t.buffer.Get();src.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;src.PlacedFootprint=t.footprint;list->CopyTextureRegion(&dst,0,0,0,&src,nullptr);
}
std::string Hash(const std::vector<uint16_t>& pixels){
    std::array<unsigned char,32> digest{};
    Need(BCryptHash(BCRYPT_SHA256_ALG_HANDLE,nullptr,0,reinterpret_cast<PUCHAR>(const_cast<uint16_t*>(pixels.data())),static_cast<ULONG>(pixels.size()*2),digest.data(),32)>=0,"readback SHA256");
    std::ostringstream out;out<<std::hex<<std::setfill('0');for(auto byte:digest)out<<std::setw(2)<<unsigned(byte);return out.str();
}
}
int wmain(int argc,wchar_t** argv){
    std::filesystem::path dll,core;std::string profileId;bool shimRequested=false;reportPath="nr-runtime-probe.json";report.requestedFrames=30;
    try {
        for(int i=1;i<argc;++i){Need(i+1<argc,"option needs value");std::wstring_view key=argv[i];const auto value=argv[++i];
            if(key==L"--dll")dll=value;else if(key==L"--core")core=value;else if(key==L"--output")reportPath=value;
            else if(key==L"--profile")profileId=std::filesystem::path(value).string();
            else if(key==L"--caller-shim"){Need(std::wstring_view(value)==L"on"||std::wstring_view(value)==L"off","shim needs on/off");shimRequested=std::wstring_view(value)==L"on";}
            else if(key==L"--frames")report.requestedFrames=std::stoul(value);else Stop("unknown option");}
        report.profile=profileId;report.shimRequested=shimRequested;report.requireSourceAlpha=true;
        Need(!NrRuntimeResearch::GameRunningOrUnknown(),"Skyrim running or process inventory unavailable; GPU probe refused");
        Need(report.requestedFrames>=2 && report.requestedFrames<=240,"frame count outside bounded probe range");
        const RuntimeProfile* profile{};for(const auto& p:RuntimeCatalog())if(p.id==profileId)profile=&p;Need(profile!=nullptr,"profile missing/unknown");
        Need(!shimRequested || profile->compatibility==CompatibilityPolicy::CallerIdentityProbeRequired,"signed profile has no caller-shim justification");
        auto runtimeLease=Value(RuntimeFileLease::Open(dll,*profile));report.runtimeSha256=runtimeLease.Sha256();
        ComPtr<IDXGIFactory6> factory;Gpu(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)),"factory");ComPtr<IDXGIAdapter1> adapter;
        for(UINT index=0;;++index){ComPtr<IDXGIAdapter1> candidate;const auto r=factory->EnumAdapterByGpuPreference(index,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&candidate));
            if(r==DXGI_ERROR_NOT_FOUND)break;Gpu(r,"adapter enumeration");DXGI_ADAPTER_DESC1 d{};Gpu(candidate->GetDesc1(&d),"adapter description");
            AdapterIdentity id{d.VendorId,d.DeviceId,d.SubSysId,{d.AdapterLuid.LowPart,d.AdapterLuid.HighPart},bool(d.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)};
            const ArtifactIdentity artifact{profileId,true,true};const auto selected=SelectRuntime(id,profileId,{&artifact,1});
            if(selected.profile){adapter=candidate;adapterIdentity=id;break;}}
        Need(adapter.Get()!=nullptr,"no eligible standalone adapter");ComPtr<ID3D12Device> device;Gpu(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device)),"device");
        const auto luid=device->GetAdapterLuid();Need(luid.LowPart==adapterIdentity.luid.low && luid.HighPart==adapterIdentity.luid.high,"device LUID mismatch");
        auto coreLease=Value(RuntimeFileLease::Open(core,QualifiedProbeDriverCore()));report.coreSha256=coreLease.Sha256();
        auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{dll,core,std::filesystem::absolute(reportPath).parent_path()/"cache",shimRequested});
        const auto opened=owner->Open(*profile,device.Get(),adapterIdentity);report.init=owner->LastInitResult();shimRva=owner->ShimSlotRva();
        std::printf("INIT=0x%08x shim=%u\n",report.init,shimRequested);
        if(!opened)Stop(opened.error().message,report.init==0xbad00002?5:1);
        report.runtimeHeldAndMatched=report.coreHeldAndMatched=true;
        ComPtr<ID3D12CommandQueue> queue;D3D12_COMMAND_QUEUE_DESC q{};Gpu(device->CreateCommandQueue(&q,IID_PPV_ARGS(&queue)),"queue");
        ComPtr<ID3D12CommandAllocator> allocator;Gpu(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator)),"allocator");
        ComPtr<ID3D12GraphicsCommandList> list;Gpu(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list)),"list");
        ComPtr<ID3D12Fence> fence;Gpu(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&fence)),"fence");
        const auto event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Need(event!=nullptr,"fence event");uint64_t fenceValue{};
        Stage stage;StageContract stageContract{device,queue,adapterIdentity.luid,{width,height},{width,height}};
        const auto initialized=stage.Initialize(owner,stageContract);report.create=stage.Diagnostics().create;
        if(!initialized)Stop(initialized.error().message);
        Gpu(list->Close(),"initial empty list close");
        const auto submit=[&](const EvaluationTicket& ticket){Gpu(list->Close(),"list close");ID3D12CommandList* l[]={list.Get()};queue->ExecuteCommandLists(1,l);
            const auto marked=stage.MarkSubmitted(ticket,fence.Get(),++fenceValue);if(!marked)Stop(marked.error().message);
            Gpu(fence->SetEventOnCompletion(fenceValue,event),"fence event arm");Need(WaitForSingleObject(event,15000)==WAIT_OBJECT_0 && fence->GetCompletedValue()==fenceValue,"output fence incomplete");Gpu(device->GetDeviceRemovedReason(),"device removed");};
        auto color=Texture(device.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT),motion=Texture(device.Get(),DXGI_FORMAT_R16G16_FLOAT),depth=Texture(device.Get(),DXGI_FORMAT_R32_FLOAT),output=Texture(device.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT,true);
        auto colorUpload=MakeTransfer(device.Get(),color.Get(),D3D12_HEAP_TYPE_UPLOAD),motionUpload=MakeTransfer(device.Get(),motion.Get(),D3D12_HEAP_TYPE_UPLOAD),depthUpload=MakeTransfer(device.Get(),depth.Get(),D3D12_HEAP_TYPE_UPLOAD),outputUpload=MakeTransfer(device.Get(),output.Get(),D3D12_HEAP_TYPE_UPLOAD),readback=MakeTransfer(device.Get(),output.Get(),D3D12_HEAP_TYPE_READBACK);
        std::vector<uint16_t> colors(width*height*4),motions(width*height*2),sentinel(width*height*4,DirectX::PackedVector::XMConvertFloatToHalf(-.25f)),pixels(colors.size());std::vector<float> depths(width*height,.5f);
        SettingsSnapshot settings;settings.revision=1;settings.enabled=true;
        std::set<std::string> distinct;
        for(UINT frame=0;frame<report.requestedFrames;++frame){
            for(UINT y=0;y<height;++y)for(UINT x=0;x<width;++x){const size_t i=(size_t(y)*width+x)*4;const float a=((x/8+y/8+frame)&1)?.1f:.9f;
                colors[i]=DirectX::PackedVector::XMConvertFloatToHalf(a);colors[i+1]=DirectX::PackedVector::XMConvertFloatToHalf(float(x)/width);colors[i+2]=DirectX::PackedVector::XMConvertFloatToHalf(float(y)/height + .002f*frame);colors[i+3]=DirectX::PackedVector::XMConvertFloatToHalf(float((x+frame)%5)/4);}
            Gpu(allocator->Reset(),"allocator reset");Gpu(list->Reset(allocator.Get(),nullptr),"list reset");
            if(frame){Barrier(list.Get(),color.Get(),D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_COPY_DEST);Barrier(list.Get(),motion.Get(),D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_COPY_DEST);Barrier(list.Get(),depth.Get(),D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_COPY_DEST);Barrier(list.Get(),output.Get(),D3D12_RESOURCE_STATE_COPY_SOURCE,D3D12_RESOURCE_STATE_COPY_DEST);}
            Upload(list.Get(),color.Get(),colorUpload,colors.data(),width*8);Upload(list.Get(),motion.Get(),motionUpload,motions.data(),width*4);Upload(list.Get(),depth.Get(),depthUpload,depths.data(),width*4);Upload(list.Get(),output.Get(),outputUpload,sentinel.data(),width*8);
            Barrier(list.Get(),color.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);Barrier(list.Get(),motion.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);Barrier(list.Get(),depth.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);Barrier(list.Get(),output.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            ImagePacket packet;packet.color=color;packet.output=output;packet.depth=depth;packet.motion=motion;
            packet.epoch=packet.guideEpoch=1;packet.sourceId=packet.guideSourceId=packet.batchId=packet.imageId=frame+1;packet.previousSourceId=frame;
            packet.kind=ImageKind::Real;packet.interpolationFraction=1;packet.presentationTime=double(frame)/60;
            packet.colorExtent=packet.guideExtent={width,height};packet.colorDomain=ColorDomain::Linear;packet.guideOrigin=GuideOrigin::RealSource;
            packet.motionScaleX=packet.motionScaleY=1;packet.reset=frame==0;
            packet.colorState=packet.depthState=packet.motionState=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;packet.outputState=D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            auto ticket=Value(stage.Record(list.Get(),packet,settings));report.evaluate=stage.Diagnostics().evaluate;
            Barrier(list.Get(),output.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_COPY_SOURCE);D3D12_TEXTURE_COPY_LOCATION dst{},src{};dst.pResource=readback.buffer.Get();dst.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;dst.PlacedFootprint=readback.footprint;src.pResource=output.Get();src.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;list->CopyTextureRegion(&dst,0,0,0,&src,nullptr);submit(ticket);
            unsigned char* mapped{};Gpu(readback.buffer->Map(0,nullptr,reinterpret_cast<void**>(&mapped)),"readback map");for(UINT y=0;y<height;++y)std::memcpy(pixels.data()+size_t(y)*width*4,mapped+readback.footprint.Offset+size_t(y)*readback.footprint.Footprint.RowPitch,width*8);D3D12_RANGE noWrite{};readback.buffer->Unmap(0,&noWrite);
            for(size_t i=3;i<pixels.size();i+=4)if(pixels[i]==colors[i])++report.sourceAlphaPreservedPixels;
            std::array<float,3> minimum{INFINITY,INFINITY,INFINITY},maximum{-INFINITY,-INFINITY,-INFINITY};
            for(size_t i=0;i<pixels.size();i+=4){bool finite=true,changed=false;for(size_t c=0;c<3;++c){const float value=DirectX::PackedVector::XMConvertHalfToFloat(pixels[i+c]);finite&=std::isfinite(value);minimum[c]=std::min(minimum[c],value);maximum[c]=std::max(maximum[c],value);changed|=pixels[i+c]!=colors[i+c];}++report.outputPixels;if(finite)++report.finitePixels;if(NrRuntimeResearch::AllRgbOverwritten(std::span<const uint16_t,4>{pixels.data()+i,4},std::span<const uint16_t,4>{sentinel.data()+i,4}))++report.overwrittenPixels;if(changed)++report.changedFromInputPixels;}
            // Require visible spatial contrast in every measured image, not
            // merely different hashes from changing uniform or alpha values.
            if(maximum[0]-minimum[0]>.01f || maximum[1]-minimum[1]>.01f || maximum[2]-minimum[2]>.01f)
                ++report.spatiallyVariedFrames;
            outputHashes.push_back(Hash(pixels));const auto rgbDigest=Hash(NrRuntimeResearch::RgbForTemporalHash(pixels));
            rgbHashes.push_back(rgbDigest);distinct.insert(rgbDigest);++report.readbackFrames;
            if(frame==0){
                // A real second-queue output copy stays blocked behind a gate.
                // Input completion alone must not release the stage's readers.
                ComPtr<ID3D12CommandQueue> consumer;Gpu(device->CreateCommandQueue(&q,IID_PPV_ARGS(&consumer)),"consumer queue");
                ComPtr<ID3D12CommandAllocator> consumerAllocator;ComPtr<ID3D12GraphicsCommandList> consumerList;
                Gpu(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&consumerAllocator)),"consumer allocator");
                Gpu(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,consumerAllocator.Get(),nullptr,IID_PPV_ARGS(&consumerList)),"consumer list");
                ComPtr<ID3D12Fence> gate,done;Gpu(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"consumer gate");Gpu(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&done)),"consumer done");
                auto delayedReadback=MakeTransfer(device.Get(),output.Get(),D3D12_HEAP_TYPE_READBACK);
                D3D12_TEXTURE_COPY_LOCATION copy=dst;copy.pResource=delayedReadback.buffer.Get();copy.PlacedFootprint=delayedReadback.footprint;
                consumerList->CopyTextureRegion(&copy,0,0,0,&src,nullptr);Gpu(consumerList->Close(),"consumer close");
                Gpu(consumer->Wait(fence.Get(),fenceValue),"consumer source wait");Gpu(consumer->Wait(gate.Get(),1),"consumer delayed wait");
                ID3D12CommandList* copyLists[]={consumerList.Get()};consumer->ExecuteCommandLists(1,copyLists);Gpu(consumer->Signal(done.Get(),1),"consumer completion signal");
                auto tracked=stage.TrackReader(ticket,done.Get(),1);if(!tracked)Stop(tracked.error().message);
                const auto early=stage.RetireTicket(ticket);Need(!early&&early.error().kind==ErrorKind::Retirement,"delayed output reader retired early");
                Need(!stage.Retire(),"feature released with delayed output reader");
                Gpu(gate->Signal(1),"consumer gate release");Gpu(done->SetEventOnCompletion(1,event),"consumer event");
                Need(WaitForSingleObject(event,15000)==WAIT_OBJECT_0&&done->GetCompletedValue()==1,"consumer retirement incomplete");
                delayedGpuReaderVerified=true;
            }
            const auto ticketRetired=stage.RetireTicket(ticket);if(!ticketRetired)Stop(ticketRetired.error().message);
        }
        report.distinctOutputHashes=static_cast<uint32_t>(distinct.size());report.outputReadersRetired=true;
        const auto stageRetired=stage.Retire();const auto diagnostics=stage.Diagnostics();
        report.release=diagnostics.release;report.destroyParameters=diagnostics.destroyParameters;
        report.allocations=diagnostics.allocations;report.releases=diagnostics.releases;
        if(!stageRetired)Stop(stageRetired.error().message);
        const auto retired=owner->Retire();report.shutdown=owner->LastShutdownResult();if(!retired)Stop(retired.error().message);
        report.shimRestored=shimRequested;
        WriteReport();
        const auto issues=NrRuntimeResearch::Validate(report);for(const auto& issue:issues)std::fprintf(stderr,"UNQUALIFIED %s\n",issue.c_str());
        CloseHandle(event);
        std::printf("frames=%u distinct=%u pixels=%llu alloc/release=%u/%u\n",report.readbackFrames,report.distinctOutputHashes,report.outputPixels,report.allocations,report.releases);
        return issues.empty()?0:1;
    }catch(const std::exception& e){Stop(e.what());}
}
