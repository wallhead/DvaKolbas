#include "NeuralRendering/ImagePacket.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <d3d11.h>
#include <cstdio>
#include <string_view>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
int failed{};
void Check(bool value,const char* name){std::printf("%s %s\n",value?"PASS":"FAIL",name);if(!value)++failed;}
ComPtr<ID3D12Resource> Texture(ID3D12Device* d,DXGI_FORMAT format,bool output=false){
    D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_DESC desc{};desc.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D;desc.Width=64;desc.Height=32;
    desc.DepthOrArraySize=desc.MipLevels=1;desc.SampleDesc.Count=1;desc.Format=format;
    if(output)desc.Flags=D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    ComPtr<ID3D12Resource> r;if(FAILED(d->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&r))))throw 1;
    return r;
}
}
int main(int argc,char** argv){try{
    const bool wrapped=argc==2&&std::string_view(argv[1])=="--require-wrapped";
    if(argc!=1&&!wrapped)return 1;
    if(NrRuntimeResearch::GameRunningOrUnknown()){std::puts("REFUSED: Skyrim running or process inventory unavailable");return 1;}
    ComPtr<IDXGIFactory4> factory; if(FAILED(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory))))return 77;
    // ReShade installs its D3D12 interception after a real D3D11 device exists.
    ComPtr<ID3D11Device> device11;ComPtr<ID3D11DeviceContext> context11;
    if(FAILED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device11,nullptr,&context11)))return 77;
    ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;
    if(FAILED(device11.As(&dxgi))||FAILED(dxgi->GetAdapter(&adapter)))return 1;
    StageContract c;if(FAILED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&c.device))))return 77;
    const auto luid=c.device->GetAdapterLuid();c.adapterLuid={luid.LowPart,luid.HighPart};c.colorExtent=c.guideExtent={64,32};
    D3D12_COMMAND_QUEUE_DESC q{};if(FAILED(c.device->CreateCommandQueue(&q,IID_PPV_ARGS(&c.queue))))return 1;
    FenceDeviceIdentity fences;Check(bool(fences.Initialize(c.device.Get())),"CaptureHostCreatedFenceDeviceIdentity");
    ComPtr<ID3D12CommandAllocator> allocator;ComPtr<ID3D12GraphicsCommandList> list;
    if(FAILED(c.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator))) || FAILED(c.device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list))))return 1;
    ImagePacket p;p.color=Texture(c.device.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT);p.output=Texture(c.device.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT,true);
    p.depth=Texture(c.device.Get(),DXGI_FORMAT_R32_FLOAT);p.motion=Texture(c.device.Get(),DXGI_FORMAT_R16G16_FLOAT);
    p.epoch=p.guideEpoch=1;p.batchId=7;p.imageId=15;p.sourceId=p.guideSourceId=7;p.previousSourceId=6;
    p.interpolationFraction=1;p.presentationTime=2;p.colorExtent=p.guideExtent={64,32};p.colorDomain=ColorDomain::Linear;p.guideOrigin=GuideOrigin::RealSource;
    p.motionScaleX=p.motionScaleY=1;p.colorState=p.depthState=p.motionState=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;p.outputState=D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    Check(bool(ValidateImagePacket(list.Get(),p,c)),"OwnedRealPacketAccepted_ShapeNotTemporalQualification");
    // Explicit SDR-byte packets keep their encoded UNORM values. They must
    // never be admitted as linear FP16 or with a mixed-format output.
    auto sdr=p;sdr.colorDomain=ColorDomain::SdrBytes;
    sdr.color=Texture(c.device.Get(),DXGI_FORMAT_R8G8B8A8_UNORM);
    sdr.output=Texture(c.device.Get(),DXGI_FORMAT_R8G8B8A8_UNORM,true);
    Check(bool(ValidateImagePacket(list.Get(),sdr,c)),"ExplicitSdrBytesPacketAccepted");
    auto mixed=sdr;mixed.colorDomain=ColorDomain::Linear;
    Check(!ValidateImagePacket(list.Get(),mixed,c),"SdrBytesCannotMasqueradeAsLinear");
    mixed=sdr;mixed.output=p.output;
    Check(!ValidateImagePacket(list.Get(),mixed,c),"SdrBytesRejectMixedOutputFormat");
    auto bad=p;bad.guideSourceId=6;Check(!ValidateImagePacket(list.Get(),bad,c),"StaleGuideSourceRejected");
    bad=p;bad.guideEpoch=2;Check(!ValidateImagePacket(list.Get(),bad,c),"StaleGuideEpochRejected");
    bad=p;bad.output=bad.color;Check(!ValidateImagePacket(list.Get(),bad,c),"ColorOutputAliasRejected");
    bad=p;bad.depth.Reset();Check(!ValidateImagePacket(list.Get(),bad,c),"MissingDepthRejected");
    bad=p;bad.colorDomain=ColorDomain::Unknown;Check(!ValidateImagePacket(list.Get(),bad,c),"UnknownColorDomainRejected");
    bad=p;bad.kind=ImageKind::Generated;bad.interpolationFraction=.5;bad.guideOrigin=GuideOrigin::ReconstructedPair;
    Check(!ValidateImagePacket(list.Get(),bad,c),"GeneratedGuideRecipeUnavailableUntilQualified");
    bad=p;bad.guideExtent.width=32;Check(!ValidateImagePacket(list.Get(),bad,c),"MismatchedGuideExtentRejected");
    bad=p;bad.motion=Texture(c.device.Get(),DXGI_FORMAT_R32_FLOAT);Check(!ValidateImagePacket(list.Get(),bad,c),"WrongMotionFormatRejected");
    bad=p;bad.outputState=D3D12_RESOURCE_STATE_COPY_DEST;Check(!ValidateImagePacket(list.Get(),bad,c),"InvalidDeclaredOutputStateRejected");
    auto foreign=c;foreign.adapterLuid.low++;Check(!ValidateImagePacket(list.Get(),p,foreign),"ActualDeviceLuidMismatchRejected");
    bad=p;c.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&bad.producerFence));bad.producerFenceValue=1;
    Check(!ValidateImagePacket(list.Get(),bad,c,&fences),"UnretiredProducerRejected");
    bad.producerFence->Signal(1);Check(bool(ValidateImagePacket(list.Get(),bad,c,&fences)),"CompletedProducerFenceAccepted");
    ComPtr<ID3D12Device> fenceDevice;ComPtr<IUnknown> deviceIdentity,fenceIdentity;
    if(FAILED(bad.producerFence->GetDevice(IID_PPV_ARGS(&fenceDevice)))||FAILED(c.device.As(&deviceIdentity))||FAILED(fenceDevice.As(&fenceIdentity)))return 1;
    std::printf("NR_DEVICE host=%p fenceOwner=%p wrapped=%u\n",deviceIdentity.Get(),fenceIdentity.Get(),deviceIdentity!=fenceIdentity);
    Check(!wrapped||deviceIdentity!=fenceIdentity,"WrappedRegressionActuallyUsesDifferentFenceDeviceIdentity");
    ComPtr<IDXGIAdapter> warp;ComPtr<ID3D12Device> foreignDevice;ComPtr<ID3D12Fence> foreignFence;
    if(FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)))||FAILED(D3D12CreateDevice(warp.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&foreignDevice)))||FAILED(foreignDevice->CreateFence(1,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&foreignFence))))return 1;
    bad.producerFence=foreignFence;auto rejected=ValidateImagePacket(list.Get(),bad,c,&fences);
    Check(!rejected&&rejected.error().kind==ErrorKind::IdentityMismatch,"CompletedForeignProducerFenceRejectedBeforeRecording");
    FenceDeviceIdentity foreignAnchor;Check(bool(foreignAnchor.Initialize(foreignDevice.Get())),"CaptureIndependentForeignFenceAnchor");
    rejected=ValidateImagePacket(list.Get(),bad,c,&foreignAnchor);
    Check(!rejected&&rejected.error().kind==ErrorKind::IdentityMismatch,"ForeignAnchorCannotAdmitForeignFenceToHostPacket");
    Check(!ValidateImagePacket(nullptr,p,c),"MissingRecordingListRejected");
    list->Close();return failed?1:0;
}catch(...){return 1;}}
