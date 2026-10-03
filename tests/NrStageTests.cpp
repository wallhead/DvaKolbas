#include "NeuralRendering/Stage.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <dxgi1_6.h>
#include <cstdio>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
int failed{};void Check(bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);if(!ok)++failed;}
uint32_t __cdecl FailEvaluate(ID3D12GraphicsCommandList*,void*,NVSDK_NGX_Parameter*,void*){return 0xbad00002;}
ComPtr<ID3D12Resource> Texture(ID3D12Device* device,DXGI_FORMAT format,bool output=false){
    D3D12_RESOURCE_DESC d{};d.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D;d.Width=64;d.Height=32;d.DepthOrArraySize=d.MipLevels=1;d.SampleDesc.Count=1;d.Format=format;
    d.Flags=output?D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS:D3D12_RESOURCE_FLAG_NONE;D3D12_HEAP_PROPERTIES heap{};heap.Type=D3D12_HEAP_TYPE_DEFAULT;
    ComPtr<ID3D12Resource> r;if(FAILED(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&r))))throw 1;return r;
}
}
int wmain(int argc,wchar_t** argv){try{
    setvbuf(stdout,nullptr,_IONBF,0);
    if(argc!=4||!std::filesystem::exists(argv[1])||!std::filesystem::exists(argv[2]))return 77;
    if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
    ComPtr<IDXGIFactory6> factory;if(FAILED(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory))))return 77;
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 d{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> a;auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&a));if(hr==DXGI_ERROR_NOT_FOUND)break;if(FAILED(hr))return 1;
        a->GetDesc1(&d);if(d.VendorId==0x10de&&d.DeviceId==0x2702){adapter=a;break;}}
    if(!adapter)return 77;StageContract c;c.colorExtent=c.guideExtent={64,32};
    if(FAILED(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&c.device))))return 77;
    c.adapterLuid={d.AdapterLuid.LowPart,d.AdapterLuid.HighPart};D3D12_COMMAND_QUEUE_DESC queue{};
    if(FAILED(c.device->CreateCommandQueue(&queue,IID_PPV_ARGS(&c.queue))))return 1;
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-stage-cache"),true});
    const AdapterIdentity id{d.VendorId,d.DeviceId,d.SubSysId,c.adapterLuid,false};
    auto opened=owner->Open(RuntimeCatalog()[1],c.device.Get(),id);if(!opened){std::puts(opened.error().message.c_str());return 1;}
    auto stage=std::make_unique<Stage>();auto initialized=stage->Initialize(owner,c);
    Check(bool(initialized)&&stage->Diagnostics().create==1,"SharedStageCreatesDirectFeatureOnRetainedOwner");
    if(!initialized)return 1;Check(!owner->Retire(),"StageClientPreventsRuntimeShutdown");
    ComPtr<ID3D12CommandAllocator> allocator;ComPtr<ID3D12GraphicsCommandList> list;c.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator));
    c.device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list));
    ImagePacket p;p.color=Texture(c.device.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT);p.output=Texture(c.device.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT,true);
    p.depth=Texture(c.device.Get(),DXGI_FORMAT_R32_FLOAT);p.motion=Texture(c.device.Get(),DXGI_FORMAT_R16G16_FLOAT);
    p.epoch=p.guideEpoch=1;p.sourceId=p.guideSourceId=p.batchId=p.imageId=1;p.presentationTime=1;p.interpolationFraction=1;
    p.kind=ImageKind::Real;p.guideOrigin=GuideOrigin::RealSource;p.colorDomain=ColorDomain::Linear;p.colorExtent=p.guideExtent={64,32};p.motionScaleX=p.motionScaleY=1;
    p.colorState=p.depthState=p.motionState=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;p.outputState=D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    SettingsSnapshot s;s.revision=1;s.enabled=true;auto disabled=s;disabled.enabled=false;
    Check(!stage->Record(list.Get(),p,disabled)&&stage->Diagnostics().evaluate==0,"DisabledNrRecordsNoVendorWork");
    auto bad=p;bad.guideSourceId=2;Check(!stage->Record(list.Get(),bad,s)&&stage->Diagnostics().evaluate==0,"InvalidPacketRejectedBeforeVendorWork");
    auto ratio=s;ratio.reconstruction.method=ResolveMethod::Ratio;
    Check(!stage->Record(list.Get(),p,ratio)&&stage->Diagnostics().evaluate==0,"CoreStageCannotSilentlyIgnoreRequestedRatioResolve");
    if(failed)return 1;
    if(std::wstring_view(argv[3])==L"record-failure"){
        const_cast<RuntimeExports&>(owner->Exports()).evaluate=&FailEvaluate;
        Check(!stage->Record(list.Get(),p,s)&&stage->Diagnostics().terminal,"FailedVendorRecordingIsTerminal");
        Check(!stage->Retire(),"FailedRecordingCannotReleaseFeature");
    }else{
        auto ticket=stage->Record(list.Get(),p,s);Check(ticket&&ticket->Output()==p.output.Get()&&stage->Diagnostics().evaluate==1,"SharedStageRecordsExactlyOneRealEvaluation");
        ComPtr<ID3D12Fence> completed;c.device->CreateFence(1,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&completed));
        if(ticket)Check(!stage->MarkSubmitted(*ticket,completed.Get(),1),"AlreadyCompletedSubmissionFenceRejected");
        if(ticket)Check(!stage->RetireTicket(*ticket),"UnsubmittedRecordingIsNotRetirement");
        Check(!stage->Retire(),"UnsubmittedTicketPreventsFeatureRelease");
    }
    stage.reset();Check(!owner->Retire(),"DestructorRetainsUncertainStageClientAndResources");
    return failed?1:0;
}catch(...){return 1;}}
