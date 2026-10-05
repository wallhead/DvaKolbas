#include "NeuralRendering/Stage.h"
#include "nr-runtime/GpuProbeGuard.h"
#include "nr-runtime/ObservedQueue.h"
#include <dxgi1_6.h>
#include <cstdio>
#include <nvsdk_ngx_params.h>
#include <array>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
int failed{};void Check(bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);if(!ok)++failed;}
uint32_t __cdecl FailEvaluate(ID3D12GraphicsCommandList*,void*,NVSDK_NGX_Parameter*,void*){return 0xbad00002;}
decltype(RuntimeExports::evaluate) nativeEvaluate{};
struct PassObservation {void* feature{};ID3D12Resource* color{},*output{};float intensity{},tone{},structure{},skin{};unsigned style{};int reset{},ui{};};
std::array<PassObservation,3> observations;unsigned observationCount{};
unsigned failOnPass{};
uint32_t __cdecl ObserveEvaluate(ID3D12GraphicsCommandList* list,void* feature,NVSDK_NGX_Parameter* parameters,void* callback){
    if(observationCount>=observations.size())return 0xbad00002;
    auto& o=observations[observationCount++];o.feature=feature;
    parameters->Get("DLSSNR.Color",&o.color);parameters->Get("DLSSNR.Output",&o.output);
    parameters->Get("DLSSNR.Intensity",&o.intensity);parameters->Get("DLSSNR.LocalToneStrength",&o.tone);
    parameters->Get("DLSSNR.LocalStructureStrength",&o.structure);parameters->Get("DLSSNR.SkinStructureStrength",&o.skin);
    parameters->Get("DLSSNR.Style",&o.style);parameters->Get("DLSSNR.Reset",&o.reset);parameters->Get("DLSSNR.UICorrection",&o.ui);
    if(failOnPass&&observationCount==failOnPass)return 0xbad00002;
    return nativeEvaluate(list,feature,parameters,callback);
}
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
    const bool admission=std::wstring_view(argv[3])==L"queued-admission";
    ObservedQueue* observed{};
    if(admission){observed=new ObservedQueue(c.queue.Get());c.queue.Attach(observed);}
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-stage-cache"),true});
    const AdapterIdentity id{d.VendorId,d.DeviceId,d.SubSysId,c.adapterLuid,false};
    auto opened=owner->Open(RuntimeCatalog()[1],c.device.Get(),id);if(!opened){std::puts(opened.error().message.c_str());return 1;}
    const bool partialFailure=std::wstring_view(argv[3])==L"chain-failure";
    const unsigned passes=std::wstring_view(argv[3])==L"chain-two"?2:(std::wstring_view(argv[3])==L"chain-three"||partialFailure)?3:1;
    auto stage=std::make_unique<Stage>();auto initialized=stage->Initialize(owner,c,0,nullptr,passes);
    Check(bool(initialized)&&stage->Diagnostics().create==1,"SharedStageCreatesDirectFeatureOnRetainedOwner");
    if(!initialized)return 1;Check(!owner->Retire(),"StageClientPreventsRuntimeShutdown");
    ComPtr<ID3D12CommandAllocator> allocator;ComPtr<ID3D12GraphicsCommandList> list;c.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&allocator));
    c.device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,allocator.Get(),nullptr,IID_PPV_ARGS(&list));
    ImagePacket p;p.color=Texture(c.device.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT);p.output=Texture(c.device.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT,true);
    p.depth=Texture(c.device.Get(),DXGI_FORMAT_R32_FLOAT);p.motion=Texture(c.device.Get(),DXGI_FORMAT_R16G16_FLOAT);
    p.epoch=p.guideEpoch=1;p.sourceId=p.guideSourceId=p.batchId=p.imageId=1;p.presentationTime=1;p.interpolationFraction=1;
    p.kind=ImageKind::Real;p.guideOrigin=GuideOrigin::RealSource;p.colorDomain=ColorDomain::Linear;p.colorExtent=p.guideExtent={64,32};p.motionScaleX=p.motionScaleY=1;
    p.colorState=p.depthState=p.motionState=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;p.outputState=D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    SettingsSnapshot s;s.revision=1;s.enabled=true;s.passes=int(passes);auto disabled=s;disabled.enabled=false;
    Check(!stage->Record(list.Get(),p,disabled)&&stage->Diagnostics().evaluate==0,"DisabledNrRecordsNoVendorWork");
    auto bad=p;bad.guideSourceId=2;Check(!stage->Record(list.Get(),bad,s)&&stage->Diagnostics().evaluate==0,"InvalidPacketRejectedBeforeVendorWork");
    auto ratio=s;ratio.reconstruction.method=ResolveMethod::Ratio;
    Check(!stage->Record(list.Get(),p,ratio)&&stage->Diagnostics().evaluate==0,"CoreStageCannotSilentlyIgnoreRequestedRatioResolve");
    if(admission){
        c.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&p.producerFence));p.producerFenceValue=1;
        auto rejected=stage->Record(list.Get(),p,s);
        Check(!rejected&&rejected.error().kind==ErrorKind::Retirement&&observed->waits==0,"PendingProducerStillRejectedByStrictRecord");
        auto stale=p;stale.guideSourceId=2;
        Check(!stage->RecordQueued(list.Get(),stale,s)&&observed->waits==0,"InvalidIdentityNeverQueued");
        Check(!stage->RecordQueued(list.Get(),p,disabled)&&observed->waits==0,"DisabledPendingProducerNeverQueued");
        ComPtr<IDXGIAdapter> warp;ComPtr<ID3D12Device> foreign;auto foreignPacket=p;
        if(FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)))||FAILED(D3D12CreateDevice(warp.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&foreign))))return 1;
        foreignPacket.producerFence.Reset();if(FAILED(foreign->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&foreignPacket.producerFence))))return 1;
        rejected=stage->RecordQueued(list.Get(),foreignPacket,s);
        Check(!rejected&&rejected.error().kind==ErrorKind::IdentityMismatch&&observed->waits==0,"ForeignFenceNeverQueued");
        observed->failWait=true;rejected=stage->RecordQueued(list.Get(),p,s);
        Check(!rejected&&observed->waits==1&&stage->Diagnostics().evaluate==0&&stage->Diagnostics().recorded==0&&stage->Diagnostics().terminal,"QueueWaitFailureRecordsNoNr");
        Check(!stage->RecordQueued(list.Get(),p,s)&&observed->waits==1,"FailedQueueWaitCannotRetryTerminalStage");
        return failed?1:0;
    }
    if(failed)return 1;
    if(passes>1){
        s.tuning={0,.25f,.5f,.75f,-1.f,true,true};
        s.additionalTuning[0]={1,.75f,1.f,1.25f,.5f,false,true};
        s.additionalTuning[1]={2,1.25f,1.5f,1.75f,1.f,true,true};
        auto mismatch=s;mismatch.passes=1;
        Check(!stage->Record(list.Get(),p,mismatch),"LatchedPassCountMismatchRecordsNoWork");
        nativeEvaluate=owner->Exports().evaluate;const_cast<RuntimeExports&>(owner->Exports()).evaluate=&ObserveEvaluate;
        if(partialFailure)failOnPass=3;
        auto ticket=stage->Record(list.Get(),p,s);
        if(partialFailure){
            const auto held=stage->Diagnostics();
            Check(!ticket&&held.terminal&&held.evaluatedPasses==2&&held.recorded==0&&held.pendingTickets==1&&held.parameterOwners==3,"FailureAfterTwoNativePassesRetainsAllChainRecordingOwners");
            Check(!stage->Retire(),"PartialChainEvaluationCannotRetireVendorFeatures");
            stage.reset();Check(!owner->Retire(),"PartialChainDestructorRetainsClientAndAllocatorOwnership");return failed?1:0;
        }
        Check(ticket&&ticket->Output()==p.output.Get()&&observationCount==passes&&stage->Diagnostics().evaluatedPasses==passes,"AllPassesShareOneFinalOutputTicket");
        bool chained=observationCount==passes&&observations[0].color==p.color.Get()&&observations[passes-1].output==p.output.Get();
        bool independent=true;
        for(unsigned i=0;i<passes;++i){
            if(i)chained&=observations[i].color==observations[i-1].output&&observations[i].feature!=observations[i-1].feature;
            const auto& o=observations[i];
            independent&=o.style==i&&o.intensity==.25f+.5f*i&&o.tone==.5f+.5f*i&&o.structure==.75f+.5f*i&&o.skin==(i==0?-1.f:i==1?.5f:1.f)&&o.reset==1&&o.ui==0;
        }
        Check(chained,"EachPassUsesPreviousOutputAndAnIndependentVendorFeature");
        Check(independent,"EachPassReceivesItsOwnTuningResetAndNoUiCorrection");
        Check(!stage->Retire(),"UnsubmittedChainRetainsAllVendorFeaturesAndIntermediateReaders");
        stage.reset();Check(!owner->Retire(),"ChainDestructorRetainsUncertainRuntimeOwnership");return failed?1:0;
    }
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
