#include "NeuralRendering/PreparedBeforeUpscale.h"
#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRFrameAdapter.h"
#include "nr-runtime/FsrPerformanceRecorder.h"
#include "nr-runtime/GpuProbeGuard.h"
#include <DirectXMath.h>
#include <dxgi1_6.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <DirectXPackedVector.h>
using namespace TheosRenderPipeline;
using Microsoft::WRL::ComPtr;
namespace NR=NeuralRendering;namespace SR=Upscaling;
namespace {
void Need(bool value,const char* text){if(!value){std::printf("FAIL %s\n",text);ExitProcess(1);}}
void Gpu(HRESULT hr){Need(SUCCEEDED(hr),"GPU operation");}
template<class T>auto Take(T value){if(!value){std::puts(value.error().message.c_str());ExitProcess(1);}return std::move(*value);}
template<class P>auto EvaluateNr(P& p,const NR::BeforeInput& input,const NR::SettingsSnapshot& settings,NR::PreparedFsrInput& lease){
    if constexpr(requires{p.Evaluate(input,SR::ColorEncoding::Gamma22,settings,&lease);})return p.Evaluate(input,SR::ColorEncoding::Gamma22,settings,&lease);
    else return p.Evaluate(input,SR::ColorEncoding::Gamma22,settings);
}
template<class A>auto EvaluateFsr(A& a,const SR::UpscaleFrame& frame,NR::PreparedFsrInput& lease){
    if constexpr(requires{a.EvaluatePrepared(frame,lease);})return lease.Valid()?a.EvaluatePrepared(frame,lease):a.Evaluate(frame);
    else return a.Evaluate(frame);
}
template<class A>void RejectMismatch(A& a,const SR::UpscaleFrame& frame,NR::PreparedFsrInput& lease){if constexpr(requires{a.EvaluatePrepared(frame,lease);}){auto wrong=frame;++wrong.sourceId;auto rejected=a.EvaluatePrepared(wrong,lease);Need(rejected&&*rejected==SR::UpscaleOutcome::SkippedInvalidInput&&lease.Valid(),"SourceIdentityMismatchRejectedBeforeFsrReader");}}
}
int wmain(int argc,wchar_t** argv){
    if(argc!=5&&argc!=6)return 77;if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
    const bool quality=std::wstring_view(argv[4])==L"quality",gated=std::wstring_view(argv[4])==L"gated",abandon=std::wstring_view(argv[4])==L"abandon",recovery=std::wstring_view(argv[4])==L"recovery",sdr=std::wstring_view(argv[4])==L"sdr-bytes";setvbuf(stdout,nullptr,_IONBF,0);
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;Gpu(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device,nullptr,&context));
    ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;Gpu(device.As(&dxgi));Gpu(dxgi->GetAdapter(&adapter));DXGI_ADAPTER_DESC desc{};Gpu(adapter->GetDesc(&desc));if(desc.VendorId!=0x10de||desc.DeviceId!=0x2702)return 77;
    SR::FsrHostResources host(argv[3]);SR::BackendConfiguration config;config.backend=SR::BackendKind::Fsr;config.quality=quality?SR::Quality::Quality:SR::Quality::NativeAA;config.generationEnabled=false;config.generationBackend=0;
    auto size=Take(host.PrepareSizing(device.Get(),config,quality?SR::Extent{480,270}:SR::Extent{320,180},DXGI_FORMAT_R8G8B8A8_UNORM,SR::ColorEncoding::Gamma22));Need(size==SR::Extent{320,180},"prepared test render size");Need(bool(host.CompleteStartup()),"FSR startup");
    NR::StageContract contract;contract.device=host.Bridge()->Device12();D3D12_COMMAND_QUEUE_DESC q{};Gpu(contract.device->CreateCommandQueue(&q,IID_PPV_ARGS(&contract.queue)));contract.adapterLuid={desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart};contract.colorExtent=contract.guideExtent={320,180};
    if(argc==6){ComPtr<ID3D12Fence> probe;ComPtr<ID3D12Device> native;ComPtr<IUnknown> wrapper,anchor;Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&probe)));Gpu(probe->GetDevice(IID_PPV_ARGS(&native)));Gpu(contract.device.As(&wrapper));Gpu(native.As(&anchor));Need(wrapper.Get()!=anchor.Get(),"ActualReShadeWrappedDeviceRequired");}
    const NR::RuntimeProfile* profile{};for(const auto& entry:NR::RuntimeCatalog())if(entry.id=="rtx40")profile=&entry;Need(profile!=nullptr,"pinned RTX40 profile");
    auto owner=std::make_shared<NR::RuntimeOwner>(NR::RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("prepared-fsr-cache"),true});auto opened=owner->Open(*profile,contract.device.Get(),{desc.VendorId,desc.DeviceId,desc.SubSysId,contract.adapterLuid,false});if(!opened)std::puts(opened.error().message.c_str());Need(bool(opened),"NR startup");
    NR::PerformanceMetrics metrics;metrics.Enable(true);NrRuntimeResearch::FsrPerformanceRecorder timing(metrics);Gpu(timing.Initialize(device.Get(),contract.device.Get(),host.Bridge()->Queue()));host.Bridge()->SetPerformanceSink(timing.InteropSink());
    NR::PreparedBeforeUpscale prepared;Need(bool(prepared.Initialize(owner,device.Get(),contract,0,&metrics,sdr?NR::ColorDomain::SdrBytes:NR::ColorDomain::Linear)),"prepared startup");
    SR::FsrFrameAdapter fsr(*host.Upscaler(),host.Bridge(),host.Resources(),host.Color11(),host.Depth11(),host.Motion11(),host.Output11(),SR::ColorEncoding::Gamma22,&timing);
    const auto texture=[&](DXGI_FORMAT format,UINT bind,UINT w=320,UINT h=180,bool staging=false){D3D11_TEXTURE2D_DESC d{};d.Width=w;d.Height=h;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=format;d.BindFlags=bind;d.Usage=staging?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;d.CPUAccessFlags=staging?D3D11_CPU_ACCESS_READ:0;ComPtr<ID3D11Texture2D> t;Gpu(device->CreateTexture2D(&d,nullptr,&t));return t;};
    NR::BeforeInput input;input.context=context;input.color=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);input.depth=texture(DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE);input.motion=texture(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE);input.epoch=input.guideEpoch=7;input.colorExtent=input.guideExtent={320,180};input.motionScaleX=320;input.motionScaleY=180;
    std::vector<float> depth(320*180,.5f);std::vector<uint16_t> motion(320*180*2);context->UpdateSubresource(input.depth.Get(),0,nullptr,depth.data(),320*4,0);context->UpdateSubresource(input.motion.Get(),0,nullptr,motion.data(),320*4,0);
    auto output=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET,quality?480:320,quality?270:180);auto readback=texture(DXGI_FORMAT_R8G8B8A8_UNORM,0,320,180,true);
    auto linearReadback=texture(DXGI_FORMAT_R16G16B16A16_FLOAT,0,320,180,true),encoded=texture(DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET),decoded=texture(DXGI_FORMAT_R16G16B16A16_FLOAT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET),decodedReadback=texture(DXGI_FORMAT_R16G16B16A16_FLOAT,0,320,180,true);
    auto alphaReference=texture(DXGI_FORMAT_R16G16B16A16_FLOAT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET),alphaReadback=texture(DXGI_FORMAT_R16G16B16A16_FLOAT,0,320,180,true);
    SR::FsrColorConverter encodeReference,decodeReference,sourceReference;uint64_t gamutSamples{},linearAlpha{},outsideGamut{};double maxQuantization{};
    SR::UpscaleFrame frame;frame.backend=SR::BackendKind::Fsr;frame.input=frame.color=input.color.Get();frame.depth=input.depth.Get();frame.motion=input.motion.Get();frame.output=output.Get();frame.render=frame.subrect={320,180};frame.display=quality?SR::Extent{480,270}:frame.render;frame.deltaMilliseconds=1000.f/60;frame.motionConvention={320,180,true,false};frame.camera.identity=1;frame.camera.nearDistance=.1f;frame.camera.farDistance=100;frame.camera.verticalFovRadians=1;
    DirectX::XMFLOAT4X4 view,projection;DirectX::XMStoreFloat4x4(&view,DirectX::XMMatrixIdentity());DirectX::XMStoreFloat4x4(&projection,DirectX::XMMatrixPerspectiveFovLH(1,320.f/180,.1f,100));std::memcpy(frame.camera.view.data(),&view,64);std::memcpy(frame.camera.projection.data(),&projection,64);
    frame.sourceEpoch=7;NR::SettingsSnapshot settings;settings.enabled=true;settings.revision=1;settings.tuning.localToneStrength=sdr?1:0;
    std::vector<unsigned char> pixels(320*180*4);uint64_t exact{},sdrChanged{};
    for(unsigned source=1;source<=18;++source){for(unsigned y=0;y<180;++y)for(unsigned x=0;x<320;++x){auto p=(y*320+x)*4;pixels[p]=(x+source)%256;pixels[p+1]=x<64?x%8:x>256?248+x%8:(y+source)%256;pixels[p+2]=(y+source)%256;pixels[p+3]=(x+y+source)%256;}context->UpdateSubresource(input.color.Get(),0,nullptr,pixels.data(),320*4,0);input.sourceId=input.guideSourceId=frame.sourceId=source;input.previousSourceId=source-1;input.presentationTime=source/60.;settings.enabled=source!=10;if(source==10||source==11)++settings.revision;
        metrics.BeginFrame(source,settings.enabled);NR::PreparedFsrInput lease;auto nr=Take(sdr?prepared.Evaluate(input,SR::ColorEncoding::Gamma22,settings):EvaluateNr(prepared,input,settings,lease));frame.reset=nr.effectiveReset;
        if(sdr)Need(!lease.Valid()&&nr.evaluated==settings.enabled,"EncodedNrUsesOrdinaryFsrDecodeWithoutLinearLease");
        if(!settings.enabled)Need(metrics.Snapshot().frames.back().flushes==0,"AlreadyRetiredNrOffDoesNotFlushGpu");
        if(source==1&&abandon){Need(lease.Valid(),"owned lease exists");{auto dropped=std::move(lease);Need(dropped.Valid()&&!lease.Valid(),"MoveOnlyLeaseTransfersOwner");}Need(prepared.Diagnostics().terminal&&!prepared.CollectCompleted()&&!prepared.Retire()&&!owner->Retire(),"AbandonedLeaseQuarantinesResourceAndRuntimeOwners");host.Bridge()->SetPerformanceSink({});Need(bool(host.Retire()),"independent FSR retires after abandoned NR lease");std::puts("PASS abandoned unconsumed reader retains owners");return 0;}
        if(source==1&&!sdr)RejectMismatch(fsr,frame,lease);
        std::thread release;ComPtr<ID3D12Fence> gate;
        if((source==1&&gated)||(source==2&&recovery)){
            context->Flush();HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);Need(event!=nullptr,"producer event");Gpu(lease.ProducerFence()->SetEventOnCompletion(lease.ProducerValue(),event));Need(WaitForSingleObject(event,2000)==WAIT_OBJECT_0,"NR producer genuinely complete");CloseHandle(event);
            Gpu(contract.device->CreateFence(0,D3D12_FENCE_FLAG_SHARED,IID_PPV_ARGS(&gate)));HANDLE shared{};Gpu(contract.device->CreateSharedHandle(gate.Get(),nullptr,GENERIC_ALL,nullptr,&shared));ComPtr<ID3D11Device5> d5;ComPtr<ID3D11Fence> g11;ComPtr<ID3D11DeviceContext4> c4;Gpu(device.As(&d5));auto hr=d5->OpenSharedFence(shared,IID_PPV_ARGS(&g11));CloseHandle(shared);Gpu(hr);Gpu(context.As(&c4));Gpu(c4->Wait(g11.Get(),1));release=std::thread([gate]{std::this_thread::sleep_for(std::chrono::milliseconds(750));Gpu(gate->Signal(1));});
        }
        if(recovery){Gpu(encodeReference.Convert(context.Get(),lease.Color(),encoded.Get(),SR::ColorEncoding::Linear,SR::ColorEncoding::Gamma22));auto module=GetModuleHandleW((std::filesystem::absolute(argv[3])/"FSR/amd_fidelityfx_loader_dx12.dll").c_str());auto mode=reinterpret_cast<void(*)(unsigned)>(GetProcAddress(module,"FixtureMode"));Need(mode!=nullptr,"recovery fixture export");mode(source==1?8:0);}
        // Queue reference reads while the lease is still owned. FSR's actual
        // input-copy/draw reader signal then covers these earlier D3D11 reads.
        if(settings.enabled&&!recovery&&!sdr){Gpu(encodeReference.Convert(context.Get(),lease.Color(),encoded.Get(),SR::ColorEncoding::Linear,SR::ColorEncoding::Gamma22));Gpu(decodeReference.Convert(context.Get(),encoded.Get(),decoded.Get(),SR::ColorEncoding::Gamma22,SR::ColorEncoding::Linear));Gpu(sourceReference.Convert(context.Get(),input.color.Get(),alphaReference.Get(),SR::ColorEncoding::Gamma22,SR::ColorEncoding::Linear));context->CopyResource(alphaReadback.Get(),alphaReference.Get());context->CopyResource(linearReadback.Get(),lease.Color());context->CopyResource(decodedReadback.Get(),decoded.Get());}
        auto result=Take(EvaluateFsr(fsr,frame,lease));Need(result==(recovery?SR::UpscaleOutcome::SpatialRecovery:SR::UpscaleOutcome::Temporal),"direct FSR temporal or persistent spatial recovery");Need(!lease.Valid(),"SuccessfulFsrDeliveryConsumesPreparedLease");
        Need(!lease.Context()&&!lease.Color()&&!lease.Depth()&&!lease.Motion()&&!lease.ProducerFence()&&!lease.ProducerValue()&&!lease.Width()&&!lease.Height()&&!lease.SourceId()&&!lease.Epoch()&&!lease.Reset(),"ConsumedPreparedLeaseCannotExposeReusedImages");metrics.EndFrame();
        if(source==2&&recovery){auto collected=prepared.CollectCompleted();Need(collected&&*collected==0&&prepared.Diagnostics().pendingTickets==1&&gate->GetCompletedValue()==0,"PendingSpatialDrawReaderRetainsNrSlot");auto begun=std::chrono::steady_clock::now();Gpu(host.Bridge()->Drain());Need(std::chrono::steady_clock::now()-begun>std::chrono::milliseconds(250),"SpatialRecoveryDrawWaitsForItsGenuineReader");release.join();}
        if(recovery){
            context->CopyResource(readback.Get(),output.Get());auto reference=texture(DXGI_FORMAT_R8G8B8A8_UNORM,0,320,180,true);context->CopyResource(reference.Get(),encoded.Get());D3D11_MAPPED_SUBRESOURCE actual{},expected{};Gpu(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&actual));Gpu(context->Map(reference.Get(),0,D3D11_MAP_READ,0,&expected));for(unsigned y=0;y<180;++y)for(unsigned x=0;x<320*4;++x)Need(std::abs(int(static_cast<unsigned char*>(actual.pData)[y*actual.RowPitch+x])-int(static_cast<unsigned char*>(expected.pData)[y*expected.RowPitch+x]))<=1,"BothRecoveryFramesDeliverEnhancedNrColorAndAlpha");context->Unmap(readback.Get(),0);context->Unmap(reference.Get(),0);
            if(source==4){Need(!prepared.Diagnostics().terminal,"RecoveryDoesNotFaultNrOnNextFrame");Need(bool(prepared.Retire())&&bool(owner->Retire()),"recovery NR readers genuinely retire");auto bridge=host.Bridge();Need(bool(host.Retire()),"poisoned FSR context retires");bridge->SetPerformanceSink({});std::puts("PASS four-frame direct NR FSR dispatch failure remains spatial without abandoned leases or intermediate drains");return 0;}
            continue;
        }
        if(source==1&&gated){auto collected=prepared.CollectCompleted();Need(collected&&*collected==0&&prepared.Diagnostics().pendingTickets==1&&gate->GetCompletedValue()==0,"PendingFsrReaderRetainsNrSlot");auto begun=std::chrono::steady_clock::now();Gpu(host.Bridge()->Drain());Need(std::chrono::steady_clock::now()-begun>std::chrono::milliseconds(250),"ActualFsrInputCopiesWaitForTheirReaderGate");release.join();}
        auto snapshot=metrics.Snapshot();const auto& observed=snapshot.frames.back();if(settings.enabled&&!sdr)Need(observed.cpuNanoseconds[size_t(NR::CpuPhase::Encode)]==0&&observed.cpuNanoseconds[size_t(NR::CpuPhase::FsrPrepareColor)]==0&&observed.cpuNanoseconds[size_t(NR::CpuPhase::FsrPrepareGuides)]==0,"PreparedNrFsrSkipsEncodeDecode");
        if(sdr&&settings.enabled)Need(observed.cpuNanoseconds[size_t(NR::CpuPhase::FsrPrepareColor)]>0,"EncodedNrActuallyRunsOrdinaryFsrColorPreparation");
        if(settings.enabled&&!sdr){
            D3D11_MAPPED_SUBRESOURCE raw{},roundtrip{},original{};Gpu(context->Map(linearReadback.Get(),0,D3D11_MAP_READ,0,&raw));Gpu(context->Map(decodedReadback.Get(),0,D3D11_MAP_READ,0,&roundtrip));Gpu(context->Map(alphaReadback.Get(),0,D3D11_MAP_READ,0,&original));
            for(unsigned y=0;y<180;++y)for(unsigned x=0;x<320;++x){auto* a=reinterpret_cast<const uint16_t*>(static_cast<const unsigned char*>(raw.pData)+y*raw.RowPitch)+x*4;auto* b=reinterpret_cast<const uint16_t*>(static_cast<const unsigned char*>(roundtrip.pData)+y*roundtrip.RowPitch)+x*4;auto* baseline=reinterpret_cast<const uint16_t*>(static_cast<const unsigned char*>(original.pData)+y*original.RowPitch)+x*4;Need(a[3]==baseline[3]&&std::abs(DirectX::PackedVector::XMConvertHalfToFloat(a[3])-pixels[(y*320+x)*4+3]/255.f)<.0005f,"EveryLinearNrSourceAlphaPreserved");++linearAlpha;for(unsigned c=0;c<3;++c){float value=DirectX::PackedVector::XMConvertHalfToFloat(a[c]),quantized=DirectX::PackedVector::XMConvertHalfToFloat(b[c]);Need(std::isfinite(value)&&std::isfinite(quantized),"FiniteColorRampsNearBlackAndHighlights");if(value>=0&&value<=1){++gamutSamples;maxQuantization=std::max(maxQuantization,double(std::abs(value-quantized)));Need(std::abs(value-quantized)<=.012f,"RoundtripPrecisionWithinUnormGammaBound");}else ++outsideGamut;}}
            context->Unmap(linearReadback.Get(),0);context->Unmap(decodedReadback.Get(),0);context->Unmap(alphaReadback.Get(),0);
        }
        context->CopyResource(readback.Get(),input.color.Get());D3D11_MAPPED_SUBRESOURCE mapped{};Gpu(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped));for(unsigned y=0;y<180;++y)for(unsigned x=0;x<320;++x){auto* row=static_cast<const unsigned char*>(mapped.pData)+y*mapped.RowPitch;for(unsigned c=0;c<4;++c){const bool same=row[x*4+c]==pixels[(y*320+x)*4+c];if(!sdr||!settings.enabled||c==3)Need(same,"SourceAlphaAndDisabledSourceUnchanged");if(sdr&&settings.enabled&&c<3&&!same)++sdrChanged;}exact+=row[x*4+3]==pixels[(y*320+x)*4+3];}context->Unmap(readback.Get(),0);
    }
    Need(exact==1036800&&(sdr?sdrChanged>0:linearAlpha==979200&&gamutSamples>979200),"AllSourceAlphaValuesAndColorSamples");std::printf("PRECISION gamut=%llu outside=%llu maxLinearUnormRoundtripError=%.8f alpha=%llu sdrChanged=%llu\n",gamutSamples,outsideGamut,maxQuantization,linearAlpha,sdrChanged);auto bridge=host.Bridge();Need(bool(host.Retire()),"FSR retirement");Need(bool(prepared.Retire())&&bool(owner->Retire()),"NR retirement");bridge->SetPerformanceSink({});std::puts("PASS NR FSR NativeAA/Quality, identity, ramps/near-black/highlights/off and exact source alpha");return 0;
}
