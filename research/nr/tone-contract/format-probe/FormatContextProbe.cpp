
#include "NeuralRendering/RuntimeOwner.h"
#include "NeuralRendering/RuntimeFileLease.h"
#include "NeuralRendering/Stage.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXPackedVector.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <vector>
#include <array>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
constexpr UINT width=640,height=360,frames=160;
[[noreturn]] void Stop(const std::string& s){std::fprintf(stderr,"STOP %s\n",s.c_str());ExitProcess(1);}
void Need(bool b,const char* s){if(!b)Stop(s);}
void Gpu(HRESULT r,const char* s){if(FAILED(r)){char b[256];sprintf_s(b,"%s HRESULT=%08x",s,unsigned(r));Stop(b);}}
template<class T>T Value(Result<T> r){if(!r)Stop(r.error().message);return std::move(*r);}
void Done(Result<void> r){if(!r)Stop(r.error().message);}
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
// Fixed surface; only its surroundings change between four 40-source phases.
void Source(std::vector<unsigned char>& pixels,unsigned phase){
    for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x){
        const auto i=(size_t(y)*width+x)*4;
        const bool surface=x>=160&&x<480&&y>=80&&y<280;
        const auto texture=std::sin(x*.075)+std::cos(y*.061)+.3*std::sin((x+y)*.29);
        if(surface){
            pixels[i]=static_cast<unsigned char>(std::clamp(110+30*texture,16.,239.));
            pixels[i+1]=static_cast<unsigned char>(std::clamp(100+25*texture,16.,239.));
            pixels[i+2]=static_cast<unsigned char>(std::clamp(85+20*texture,16.,239.));
        }else{
            const unsigned char backgrounds[4][3]={{16,16,16},{235,235,235},{25,70,230},{16,16,16}};
            for(unsigned c=0;c<3;++c)pixels[i+c]=backgrounds[phase][c];
        }
        pixels[i+3]=static_cast<unsigned char>(64+(x+y)%192);
    }
}
void Ppm(const std::filesystem::path& path,const std::vector<unsigned char>& pixels){
    std::ofstream file(path,std::ios::binary);file<<"P6\n"<<width<<" "<<height<<"\n255\n";
    for(size_t i=0;i<pixels.size();i+=4)file.write(reinterpret_cast<const char*>(pixels.data()+i),3);
    if(!file)throw HRESULT(E_FAIL);
}
}
int wmain(int argc,wchar_t** argv){try{
 Need(argc==7,"args: dll core output profile style unorm8|encoded16|linear16");
 Need(!NrRuntimeResearch::GameRunningOrUnknown(),"Skyrim running or inventory unknown");
 const std::string profileId=std::filesystem::path(argv[4]).string();
 const std::wstring mode=argv[6];const bool unorm=mode==L"unorm8",linear=mode==L"linear16";
 Need(unorm||linear||mode==L"encoded16","invalid mode");
 const auto dir=std::filesystem::absolute(argv[3]);std::filesystem::create_directories(dir);
 const RuntimeProfile* profile=nullptr;for(const auto& p:RuntimeCatalog())if(p.id==profileId)profile=&p;
 Need(profile!=nullptr,"unknown profile");
 const auto runtime=Value(RuntimeFileLease::Open(argv[1],*profile));
 const auto core=Value(RuntimeFileLease::Open(argv[2],QualifiedProbeDriverCore()));
 ComPtr<IDXGIFactory6> factory;Gpu(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)),"factory");
 ComPtr<IDXGIAdapter1> adapter;AdapterIdentity id;
 for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> a;const auto h=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&a));if(h==DXGI_ERROR_NOT_FOUND)break;Gpu(h,"enumerate");DXGI_ADAPTER_DESC1 d{};Gpu(a->GetDesc1(&d),"desc");if(d.VendorId==0x10de&&d.DeviceId==0x2702){adapter=a;id={d.VendorId,d.DeviceId,d.SubSysId,{d.AdapterLuid.LowPart,d.AdapterLuid.HighPart},false};break;}}
 Need(adapter!=nullptr,"qualified RTX4080 SUPER missing");
 ComPtr<ID3D12Device> device;Gpu(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&device)),"device");
 const bool shim=profile->compatibility==CompatibilityPolicy::CallerIdentityProbeRequired;
 auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],dir/"cache",shim});Done(owner->Open(*profile,device.Get(),id));
 ComPtr<ID3D12CommandQueue> queue;D3D12_COMMAND_QUEUE_DESC q{};Gpu(device->CreateCommandQueue(&q,IID_PPV_ARGS(&queue)),"queue");
 ComPtr<ID3D12CommandAllocator> allocator;Gpu(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator)),"allocator");
 ComPtr<ID3D12GraphicsCommandList> list;Gpu(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list)),"list");Gpu(list->Close(),"initial close");
 ComPtr<ID3D12Fence> fence;Gpu(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&fence)),"fence");
 const auto event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Need(event!=nullptr,"event");
 const auto format=unorm?DXGI_FORMAT_R8G8B8A8_UNORM:DXGI_FORMAT_R16G16B16A16_FLOAT;
 D3D12_FEATURE_DATA_FORMAT_SUPPORT support{format};Gpu(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT,&support,sizeof(support)),"support");
 const auto needed=D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD|D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE;Need((support.Support2&needed)==needed,"format typed UAV support missing");
 Stage stage;Done(stage.Initialize(owner,{device,queue,id.luid,{width,height},{width,height}}));
 auto color=Texture(device.Get(),format),output=Texture(device.Get(),format,true),motion=Texture(device.Get(),DXGI_FORMAT_R16G16_FLOAT),depth=Texture(device.Get(),DXGI_FORMAT_R32_FLOAT);
 auto cu=MakeTransfer(device.Get(),color.Get(),D3D12_HEAP_TYPE_UPLOAD),mu=MakeTransfer(device.Get(),motion.Get(),D3D12_HEAP_TYPE_UPLOAD),du=MakeTransfer(device.Get(),depth.Get(),D3D12_HEAP_TYPE_UPLOAD),ou=MakeTransfer(device.Get(),output.Get(),D3D12_HEAP_TYPE_UPLOAD),rb=MakeTransfer(device.Get(),output.Get(),D3D12_HEAP_TYPE_READBACK);
 std::vector<unsigned char> source(width*height*4),result(source.size()),previous,packed(source.size()*(unorm?1:2)),read(packed.size()),zero(packed.size());
 std::vector<uint16_t> mv(width*height*2);std::vector<float> depths(width*height,.5f);
 SettingsSnapshot settings;settings.enabled=true;settings.revision=1;settings.tuning.style=std::stoi(argv[5]);settings.stableColors=false;
 std::ofstream csv(dir/"frames.csv");csv<<"source,meanR,meanG,meanB,sourceR,sourceG,sourceB,meanRgbCorrection,warpedMeanRgbDifference\n";
 double settled[4][3]{};uint64_t alphaCount{};
 for(UINT f=0;f<frames;++f){
  Source(source,f/40);
  if(unorm)packed=source;else for(size_t i=0;i<source.size();++i){float v=float(source[i])/255.f;if(linear&&i%4!=3)v=std::pow(v,2.2f);const auto half=DirectX::PackedVector::XMConvertFloatToHalf(v);std::memcpy(packed.data()+2*i,&half,2);}
  Gpu(allocator->Reset(),"reset allocator");Gpu(list->Reset(allocator.Get(),nullptr),"reset list");
  if(f){for(auto* r:{color.Get(),motion.Get(),depth.Get()})Barrier(list.Get(),r,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_COPY_DEST);Barrier(list.Get(),output.Get(),D3D12_RESOURCE_STATE_COPY_SOURCE,D3D12_RESOURCE_STATE_COPY_DEST);}
  Upload(list.Get(),color.Get(),cu,packed.data(),width*(unorm?4:8));Upload(list.Get(),motion.Get(),mu,mv.data(),width*4);Upload(list.Get(),depth.Get(),du,depths.data(),width*4);Upload(list.Get(),output.Get(),ou,zero.data(),width*(unorm?4:8));
  for(auto* r:{color.Get(),motion.Get(),depth.Get()})Barrier(list.Get(),r,D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);Barrier(list.Get(),output.Get(),D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
  ImagePacket p;p.color=color;p.output=output;p.motion=motion;p.depth=depth;p.epoch=p.guideEpoch=1;p.sourceId=p.guideSourceId=p.batchId=p.imageId=f+1;p.previousSourceId=f;p.kind=ImageKind::Real;p.interpolationFraction=1;p.presentationTime=double(f+1)/60;p.colorExtent=p.guideExtent={width,height};p.colorDomain=ColorDomain::Linear;p.guideOrigin=GuideOrigin::RealSource;p.motionScaleX=width;p.motionScaleY=height;p.reset=f==0;
  p.colorState=p.depthState=p.motionState=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;p.outputState=D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
  const auto ticket=Value(stage.Record(list.Get(),p,settings));
  Barrier(list.Get(),output.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS,D3D12_RESOURCE_STATE_COPY_SOURCE);
  D3D12_TEXTURE_COPY_LOCATION dst{},src{};dst.pResource=rb.buffer.Get();dst.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;dst.PlacedFootprint=rb.footprint;src.pResource=output.Get();src.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;list->CopyTextureRegion(&dst,0,0,0,&src,nullptr);
  Gpu(list->Close(),"close");ID3D12CommandList* ls[]={list.Get()};queue->ExecuteCommandLists(1,ls);Done(stage.MarkSubmitted(ticket,fence.Get(),f+1));Gpu(fence->SetEventOnCompletion(f+1,event),"event arm");Need(WaitForSingleObject(event,15000)==WAIT_OBJECT_0&&fence->GetCompletedValue()==f+1,"fence incomplete");Gpu(device->GetDeviceRemovedReason(),"device removed");
  unsigned char* mapped{};Gpu(rb.buffer->Map(0,nullptr,reinterpret_cast<void**>(&mapped)),"readback");for(UINT y=0;y<height;++y)std::memcpy(read.data()+size_t(y)*width*(unorm?4:8),mapped+rb.footprint.Offset+size_t(y)*rb.footprint.Footprint.RowPitch,width*(unorm?4:8));D3D12_RANGE noWrite{};rb.buffer->Unmap(0,&noWrite);
  if(unorm)result=read;else for(size_t i=0;i<result.size();++i){uint16_t h;std::memcpy(&h,read.data()+2*i,2);float v=DirectX::PackedVector::XMConvertHalfToFloat(h);Need(std::isfinite(v),"nonfinite output");if(linear&&i%4!=3)v=std::pow(std::max(0.f,v),1.f/2.2f);result[i]=static_cast<unsigned char>(std::clamp(std::lround(v*255),0l,255l));}
  for(size_t i=3;i<result.size();i+=4){Need(result[i]==source[i],"alpha changed");++alphaCount;}
  double means[3]{},input[3]{},correction{},difference{};constexpr double samples=200*120;
  for(UINT y=120;y<240;++y)for(UINT x=220;x<420;++x){const auto i=(size_t(y)*width+x)*4;for(UINT c=0;c<3;++c){means[c]+=result[i+c];input[c]+=source[i+c];correction+=std::abs(int(result[i+c])-int(source[i+c]));if(f)difference+=std::abs(int(result[i+c])-int(previous[i+c]));}}
  csv<<f+1;for(auto v:means)csv<<','<<v/samples;for(auto v:input)csv<<','<<v/samples;csv<<','<<correction/(samples*3)<<','<<difference/(samples*3)<<'\n';
  if(f%40>=20)for(UINT c=0;c<3;++c)settled[f/40][c]+=means[c]/samples/20;
  if(f%40==39){Ppm(dir/("source-"+std::to_string(f+1)+".ppm"),source);Ppm(dir/("nr-"+std::to_string(f+1)+".ppm"),result);std::ofstream raw(dir/("output-"+std::to_string(f+1)+".bin"),std::ios::binary);raw.write(reinterpret_cast<const char*>(read.data()),read.size());Need(bool(raw),"raw save");}
  previous=result;Done(stage.RetireTicket(ticket));
 }
 Done(stage.Retire());Done(owner->Retire());Need(bool(csv),"csv save");CloseHandle(event);
 double drift{};for(UINT c=0;c<3;++c){double low=settled[0][c],high=low;for(const auto& p:settled){low=std::min(low,p[c]);high=std::max(high,p[c]);}drift=std::max(drift,high-low);}
 const auto d=stage.Diagnostics();Need(d.recorded==frames,"incomplete evals");
 printf("OBSERVED mode=%ls style=%d sources=%u alpha=%llu drift=%.6f init=%08x create=%08x eval=%08x release=%08x shutdown=%08x retirement=complete\n",mode.c_str(),settings.tuning.style,frames,alphaCount,drift,owner->LastInitResult(),d.create,d.evaluate,d.release,owner->LastShutdownResult());return 0;
}catch(const std::exception& e){Stop(e.what());}}
