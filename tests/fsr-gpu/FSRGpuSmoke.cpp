#include "Upscaling/FSRUpscaler.h"
#include "Upscaling/FSRProviderPolicy.h"
#include "InteropTestRig.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <unordered_set>
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
static ID3D12Device* allocatorDevice{};
static std::unordered_set<ID3D12Resource*> liveResources;
static std::unordered_set<ID3D12Heap*> liveHeaps;
static std::size_t peakResources{},peakHeaps{},allocatedResources{},allocatedHeaps{};
static ffxReturnCode_t AllocateResource(uint32_t,D3D12_RESOURCE_STATES state,const D3D12_HEAP_PROPERTIES* heap,
    const D3D12_RESOURCE_DESC* desc,const FfxApiResourceDescription*,const D3D12_CLEAR_VALUE* clear,ID3D12Resource** resource)
{
    if(FAILED(allocatorDevice->CreateCommittedResource(heap,D3D12_HEAP_FLAG_NONE,desc,state,clear,IID_PPV_ARGS(resource))))return FFX_API_RETURN_ERROR_MEMORY;
    liveResources.insert(*resource);++allocatedResources;peakResources=std::max(peakResources,liveResources.size());return FFX_API_RETURN_OK;
}
static ffxReturnCode_t FreeResource(uint32_t,ID3D12Resource* resource)
{ Require(liveResources.erase(resource)==1,"SDK resource released exactly once");resource->Release();return FFX_API_RETURN_OK; }
static ffxReturnCode_t AllocateHeap(uint32_t,const D3D12_HEAP_DESC* desc,bool,ID3D12Heap** heap,uint64_t* offset)
{
    if(FAILED(allocatorDevice->CreateHeap(desc,IID_PPV_ARGS(heap))))return FFX_API_RETURN_ERROR_MEMORY;
    *offset=0;liveHeaps.insert(*heap);++allocatedHeaps;peakHeaps=std::max(peakHeaps,liveHeaps.size());return FFX_API_RETURN_OK;
}
static ffxReturnCode_t FreeHeap(uint32_t,ID3D12Heap* heap,uint64_t,uint64_t)
{ Require(liveHeaps.erase(heap)==1,"SDK heap released exactly once");heap->Release();return FFX_API_RETURN_OK; }
template<class T> static T Value(Result<T> result)
{ if(!result){std::fprintf(stderr,"FSR code=%lld: %s\n",static_cast<long long>(result.error().nativeResult),result.error().message.c_str());std::exit(1);}return std::move(*result); }
static void Accepted(Result<void> result)
{ if(!result){std::fprintf(stderr,"FSR code=%lld: %s\n",static_cast<long long>(result.error().nativeResult),result.error().message.c_str());std::exit(1);} }

// Jittered projection of textured planes, with independent camera/object
// translation. Motion is unjittered previous-current screen pixels. Depth uses
// the same physical near/far planes as the dispatch; no fabricated guides.
static constexpr char shaderSource[]=R"(
cbuffer Frame : register(b0) { uint2 size; float2 jitter; float cameraX; float previousCameraX;
    float objectX; float previousObjectX; float nearZ; float farZ; uint inverted; uint unused; };
RWTexture2D<float4> color : register(u0);RWTexture2D<float> depth : register(u1);RWTexture2D<float2> motion : register(u2);
[numthreads(8,8,1)] void main(uint3 id : SV_DispatchThreadID) {
    if(any(id.xy>=size)) return;
    float2 uv=(float2(id.xy)+0.5-jitter)/float2(size);float2 world=uv+float2(cameraX,0);
    bool foreground=abs(world.x-(0.5+objectX))<0.12 && abs(world.y-0.5)<0.2;float z=foreground?1.5:3.0;
    float d=farZ/(farZ-nearZ)-(farZ*nearZ/(farZ-nearZ))/z;depth[id.xy]=inverted?1-d:d;
    float shift=cameraX-previousCameraX-(foreground?(objectX-previousObjectX):0);motion[id.xy]=float2(shift*size.x,0);
    float checker=fmod(floor(world.x*48)+floor(world.y*24),2);
    color[id.xy]=float4(foreground?float3(0.8,0.15+0.1*checker,0.05):float3(0.08+0.12*checker,0.15+0.15*checker,0.25+0.2*checker),1);
})";
struct Constants { UINT width,height;float jitterX,jitterY,cameraX,previousCameraX,objectX,previousObjectX,nearZ,farZ;UINT inverted,unused; };
static std::uint64_t Pixels(Rig& rig,ID3D11Texture2D* staging,Extent size)
{
    D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(staging,0,D3D11_MAP_READ,0,&mapped),"read native output");
    std::uint64_t hash=1469598103934665603ull;double energy{};float min=100,max=-100;
    for(UINT y=0;y<size.height;++y) {
        auto* row=reinterpret_cast<const DirectX::PackedVector::HALF*>(static_cast<const unsigned char*>(mapped.pData)+y*mapped.RowPitch);
        for(UINT x=0;x<size.width;++x)for(UINT c=0;c<3;++c){auto half=row[x*4+c];auto v=DirectX::PackedVector::XMConvertHalfToFloat(half);
            Require(std::isfinite(v)&&std::abs(v)<128,"native FSR output finite and bounded");energy+=std::abs(v);min=std::min(min,v);max=std::max(max,v);hash=(hash^half)*1099511628211ull;}
    }
    rig.context11->Unmap(staging,0);Require(energy>double(size.width)*size.height*0.01 && max-min>0.05f,"native output is nonempty textured geometry");return hash;
}
int main(int argc,char** argv)
{
    unsigned frames=1000,recreate=25;std::filesystem::path runtimeDirectory,report="out/validation/fsr-gpu.json";
    for(int i=1;i<argc;++i){std::string argument=argv[i];Require(i+1<argc,"smoke option needs value");std::string value=argv[++i];
        if(argument=="--frames")frames=std::stoul(value);else if(argument=="--recreate")recreate=std::stoul(value);else if(argument=="--output")report=value;
        else if(argument=="--runtime")runtimeDirectory=std::filesystem::absolute(value);else if(argument=="--debug")Require(value=="auto","debug auto only");else Require(false,"unknown smoke option");}
    Require(recreate>0 && frames>=recreate*4,"at least four frames per context");
    if(runtimeDirectory.empty()||!std::filesystem::exists(runtimeDirectory/"amd_fidelityfx_loader_dx12.dll")){std::puts("SKIPPED: pinned runtime unavailable");return 77;}
    auto plugin=std::filesystem::absolute(report).parent_path()/"runtime-plugin";std::filesystem::create_directories(plugin/"FSR");
    for(auto name:{"amd_fidelityfx_loader_dx12.dll","amd_fidelityfx_upscaler_dx12.dll"})std::filesystem::copy_file(runtimeDirectory/name,plugin/"FSR"/name,std::filesystem::copy_options::overwrite_existing);
    auto runtime=std::make_shared<FsrRuntime>();Accepted(runtime->Load(plugin));Rig rig(true);allocatorDevice=rig.device12.Get();
    auto provider=Value(SelectProvider(Value(runtime->Enumerate(rig.device12.Get())),ProviderPolicy::Analytical));
    ComPtr<IDXGIAdapter3> memoryAdapter;Check(rig.adapter.As(&memoryAdapter),"adapter memory observation");
    auto memory=[&](){DXGI_QUERY_VIDEO_MEMORY_INFO info{};Check(memoryAdapter->QueryVideoMemoryInfo(0,DXGI_MEMORY_SEGMENT_GROUP_LOCAL,&info),"GPU memory observation");return info.CurrentUsage;};
    auto initialMemory=memory();UINT64 peakMemory=initialMemory,minRetiredMemory=UINT64_MAX,maxRetiredMemory{};
    ComPtr<ID3DBlob> code,errors;auto hr=D3DCompile(shaderSource,sizeof(shaderSource)-1,nullptr,nullptr,nullptr,"main","cs_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&code,&errors);
    if(FAILED(hr)&&errors)std::fprintf(stderr,"shader: %s\n",static_cast<const char*>(errors->GetBufferPointer()));Check(hr,"controlled geometry shader");
    ComPtr<ID3D11ComputeShader> shader;Check(rig.device11->CreateComputeShader(code->GetBufferPointer(),code->GetBufferSize(),nullptr,&shader),"geometry shader");
    D3D11_BUFFER_DESC cb{};cb.ByteWidth=sizeof(Constants);cb.Usage=D3D11_USAGE_DEFAULT;cb.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
    ComPtr<ID3D11Buffer> constants;Check(rig.device11->CreateBuffer(&cb,nullptr,&constants),"geometry constants");
    unsigned dispatched{},readbacks{},changed{},inflightVerified{};std::uint64_t previousHash{};std::size_t firstPeakResources{},firstPeakHeaps{};
    for(unsigned cycle=0;cycle<recreate;++cycle){
        {
            auto bridge=std::make_shared<Interop>();rig.Initialize(*bridge);
            Extent display{cycle%4==0?321u:641u,cycle%4==0?181u:361u};auto quality=static_cast<Quality>(cycle%4);
            auto render=Value(runtime->QueryRenderExtent(rig.device12.Get(),provider,quality,display));
            FsrUpscaler fsr;Accepted(fsr.SetRetirementBridge(bridge));bool inverted=cycle%2!=0;Accepted(fsr.SetInputPolicy({inverted,false,false,true}));
            ffxCreateBackendDX12AllocationCallbacksDesc allocation{};allocation.header.type=FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12_ALLOCATION_CALLBACKS;
            allocation.pfnFfxResourceAllocator=AllocateResource;allocation.pfnFfxResourceDeallocator=FreeResource;allocation.pfnFfxHeapAllocator=AllocateHeap;allocation.pfnFfxHeapDeallocator=FreeHeap;
            Accepted(fsr.SetAllocationCallbacks(allocation));Accepted(fsr.Initialize(runtime,rig.device12.Get(),provider,quality,render,display));Require(Value(fsr.ActualProvider()).id==provider.id,"actual analytical provider retained");
            SharedTexture color,depth,motion,output;auto desc=rig.Description();desc.Width=render.width;desc.Height=render.height;
            // Opening this shared D3D12 UAV on D3D11 requires RT capability on
            // the tested device; shader production still uses only its UAV.
            desc.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_RENDER_TARGET;
            desc.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;Check(bridge->CreateSharedTexture(desc,color),"shared color");desc.Format=DXGI_FORMAT_R32_FLOAT;Check(bridge->CreateSharedTexture(desc,depth),"shared depth");
            desc.Format=DXGI_FORMAT_R16G16_FLOAT;Check(bridge->CreateSharedTexture(desc,motion),"shared motion");desc.Width=display.width;desc.Height=display.height;desc.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;
            Check(bridge->CreateSharedTexture(desc,output),"native output");ComPtr<ID3D11UnorderedAccessView> views[3];ID3D11Texture2D* inputs[]{color.texture11.Get(),depth.texture11.Get(),motion.texture11.Get()};
            for(unsigned i=0;i<3;++i)Check(rig.device11->CreateUnorderedAccessView(inputs[i],nullptr,&views[i]),"controlled input UAV");
            desc.BindFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;desc.Usage=D3D11_USAGE_STAGING;ComPtr<ID3D11Texture2D> staging;Check(rig.device11->CreateTexture2D(&desc,nullptr,&staging),"native readback");
            GpuFrameResources gpu{color.texture12.Get(),depth.texture12.Get(),motion.texture12.Get(),output.texture12.Get()};UpscaleFrame frame;frame.backend=BackendKind::Fsr;frame.render=frame.subrect=render;frame.display=display;
            frame.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;frame.colorIsLinear=true;frame.deltaMilliseconds=16;frame.sharpness=0.25f;
            frame.motionConvention={1,1,true,false};frame.camera.identity=1;frame.camera.nearDistance=0.1f;frame.camera.farDistance=100;frame.camera.verticalFovRadians=1;frame.camera.depthInverted=inverted;
            DirectX::XMFLOAT4X4 projection,view;DirectX::XMStoreFloat4x4(&projection,DirectX::XMMatrixPerspectiveFovLH(1,float(display.width)/display.height,0.1f,100));
            if(inverted)for(unsigned row=0;row<4;++row)projection.m[row][2]=projection.m[row][3]-projection.m[row][2];std::memcpy(frame.camera.projection.data(),&projection,sizeof(projection));
            ComPtr<ID3D12Fence> gate;Check(rig.device12->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&gate)),"in-flight gate");Check(rig.queue->Wait(gate.Get(),1),"hold three real dispatches");
            unsigned count=frames/recreate+(cycle<frames%recreate?1:0);
            for(unsigned local=0;local<count;++local){
                frame.sourceId=++dispatched;frame.reset=local==0;auto jitter=Value(fsr.QueryJitter(frame.sourceId));frame.jitterX=jitter[0];frame.jitterY=jitter[1];
                float cameraX=std::sin(float(frame.sourceId)*0.02f)*0.04f,objectX=std::sin(float(frame.sourceId)*0.04f)*0.12f;
                float previousCameraX=local?std::sin(float(frame.sourceId-1)*0.02f)*0.04f:cameraX,previousObjectX=local?std::sin(float(frame.sourceId-1)*0.04f)*0.12f:objectX;
                frame.camera.position={cameraX,0,0};DirectX::XMStoreFloat4x4(&view,DirectX::XMMatrixTranslation(-cameraX,0,0));std::memcpy(frame.camera.view.data(),&view,sizeof(view));
                Constants values{render.width,render.height,jitter[0],jitter[1],cameraX,previousCameraX,objectX,previousObjectX,0.1f,100,inverted?1u:0u,0};
                rig.context11->UpdateSubresource(constants.Get(),0,nullptr,&values,0,0);ID3D11Buffer* buffer=constants.Get();rig.context11->CSSetConstantBuffers(0,1,&buffer);
                ID3D11UnorderedAccessView* uavs[]{views[0].Get(),views[1].Get(),views[2].Get()};rig.context11->CSSetUnorderedAccessViews(0,3,uavs,nullptr);rig.context11->CSSetShader(shader.Get(),nullptr,0);
                rig.context11->Dispatch((render.width+7)/8,(render.height+7)/8,1);ID3D11UnorderedAccessView* empty[3]{};rig.context11->CSSetUnorderedAccessViews(0,3,empty,nullptr);
                Check(bridge->SignalProducer(),"submit D3D11 geometry");ID3D12GraphicsCommandList* list{};Check(bridge->Begin(&list),"FSR slot");Accepted(fsr.Dispatch(list,gpu,frame));Check(bridge->Submit(),"submit real FSR");Check(bridge->WaitConsumer(),"native dependency");
                if(local==2){Require(bridge->CurrentSlot(Work::Upscaling)==0 && gate->GetCompletedValue()==0,"three submissions outstanding before reuse");++inflightVerified;Check(gate->Signal(1),"release independent test gate");}
                if((local+1)%16==0||local+1==count){rig.context11->CopyResource(staging.Get(),output.texture11.Get());Check(bridge->Drain(),"retire native reader");auto hash=Pixels(rig,staging.Get(),display);if(previousHash&&hash!=previousHash)++changed;previousHash=hash;++readbacks;}
            }
            peakMemory=std::max(peakMemory,memory());Accepted(fsr.DestroyAfterRetirement());Require(liveResources.empty()&&liveHeaps.empty(),"every instrumented SDK allocation destroyed after retirement");
        }
        auto retired=memory();minRetiredMemory=std::min(minRetiredMemory,retired);maxRetiredMemory=std::max(maxRetiredMemory,retired);
        if(cycle==3){firstPeakResources=peakResources;firstPeakHeaps=peakHeaps;}if(cycle>=4)Require(peakResources<=firstPeakResources&&peakHeaps<=firstPeakHeaps,"SDK resource peaks bounded across recreation");
    }
    Require(changed>recreate && readbacks>=recreate && inflightVerified==recreate,"changing output and multiple in-flight frames in every context");
    std::printf("Allocation observations: resources=%zu heaps=%zu peakResources=%zu peakHeaps=%zu retiredMemoryRange=%llu\n",
        allocatedResources,allocatedHeaps,peakResources,peakHeaps,static_cast<unsigned long long>(maxRetiredMemory-minRetiredMemory));
    Require(allocatedResources>0||allocatedHeaps>0,"official allocation callbacks observed SDK allocations");Require(maxRetiredMemory-minRetiredMemory<64ull*1024*1024,"retired GPU memory growth bounded");rig.ValidateDebug();
    auto luid=rig.device12->GetAdapterLuid();std::filesystem::create_directories(report.parent_path());std::ofstream json(report);
    json<<"{\n\"result\":\"PASS\",\n\"frames\":"<<dispatched<<",\"recreations\":"<<recreate<<",\"readbacks\":"<<readbacks<<",\"changingSamples\":"<<changed<<",\"inflightContexts\":"<<inflightVerified
        <<",\n\"providerId\":"<<provider.id<<",\"providerName\":\""<<provider.name<<"\",\"adapterLuidHigh\":"<<luid.HighPart<<",\"adapterLuidLow\":"<<luid.LowPart
        <<",\n\"d3d11DebugValidated\":"<<(rig.messages11?"true":"false")<<",\"d3d12DebugValidated\":"<<(rig.messages12?"true":"false")
        <<",\n\"sdkCommittedResourcesCreated\":"<<allocatedResources<<",\"sdkHeapsCreated\":"<<allocatedHeaps<<",\"peakSdkCommittedResources\":"<<peakResources<<",\"peakSdkHeaps\":"<<peakHeaps
        <<",\"remainingSdkCommittedResources\":"<<liveResources.size()<<",\"remainingSdkHeaps\":"<<liveHeaps.size()
        <<",\n\"initialLocalGpuBytes\":"<<initialMemory<<",\"peakLocalGpuBytes\":"<<peakMemory<<",\"minRetiredLocalGpuBytes\":"<<minRetiredMemory<<",\"maxRetiredLocalGpuBytes\":"<<maxRetiredMemory
        <<",\n\"skyrimGameplayTested\":false,\"crossVendorTested\":false,\"frameGenerationTested\":false\n}\n";
    Require(bool(json),"write validation report");std::printf("PASS: real FSR %u frames, %u recreations, %u changing samples; SDK live resources/heaps zero\n",dispatched,recreate,changed);
}
