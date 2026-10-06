#include "NeuralRendering/BeforeUpscale.h"
#include "NeuralRendering/RuntimeFileLease.h"
#include "nr-runtime/GpuProbeGuard.h"
#include "nr-runtime/ObservedQueue.h"
#include "ProbeBuildIdentity.h"
#include <dxgi1_6.h>
#include <DirectXPackedVector.h>
#include <cstdio>
#include <vector>
#include <cmath>
#include <bcrypt.h>
#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <set>
using namespace TheosRenderPipeline::NeuralRendering;
using Microsoft::WRL::ComPtr;
namespace {
int failures{};
void Check(bool ok,const char* name){std::printf("%s %s\n",ok?"PASS":"FAIL",name);if(!ok)++failures;}
void Need(HRESULT hr){if(FAILED(hr))throw hr;}
constexpr UINT width=320,height=180;
std::string Hash(const std::vector<uint16_t>& pixels){
    std::array<unsigned char,32> digest{};
    if(BCryptHash(BCRYPT_SHA256_ALG_HANDLE,nullptr,0,reinterpret_cast<PUCHAR>(const_cast<uint16_t*>(pixels.data())),ULONG(pixels.size()*2),digest.data(),32)<0)throw 1;
    std::ostringstream text;text<<std::hex<<std::setfill('0');for(auto b:digest)text<<std::setw(2)<<unsigned(b);return text.str();
}
ComPtr<ID3D11Texture2D> Texture(ID3D11Device* d,DXGI_FORMAT format,bool readback=false){
    D3D11_TEXTURE2D_DESC desc{};desc.Width=width;desc.Height=height;desc.Format=format;
    desc.MipLevels=desc.ArraySize=desc.SampleDesc.Count=1;
    desc.Usage=readback?D3D11_USAGE_STAGING:D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags=readback?D3D11_CPU_ACCESS_READ:0;
    desc.BindFlags=readback?0:D3D11_BIND_SHADER_RESOURCE|(format==DXGI_FORMAT_R16G16B16A16_FLOAT?D3D11_BIND_RENDER_TARGET:0);
    ComPtr<ID3D11Texture2D> t;Need(d->CreateTexture2D(&desc,nullptr,&t));return t;
}
}
int wmain(int argc,wchar_t** argv){try{
    setvbuf(stdout,nullptr,_IONBF,0);
    if((argc!=3&&argc!=4)||!std::filesystem::exists(argv[1])||!std::filesystem::exists(argv[2]))return 77;
    if(NrRuntimeResearch::GameRunningOrUnknown())return 1;
    BeforeUpscale bridge;SettingsSnapshot settings;settings.revision=1;
    const unsigned passes=argc==4&&std::wstring_view(argv[3])==L"chain-two"?2:argc==4&&std::wstring_view(argv[3])==L"chain-three"?3:1;
    settings.passes=int(passes);
    settings.additionalTuning[0]={1,.75f,.5f,1.25f,-1.f,true,false};
    settings.additionalTuning[1]={2,1.25f,1.5f,.75f,.5f,false,false};
    auto disabled=bridge.Evaluate({},settings);
    Check(disabled&&!disabled->evaluated&&bridge.Diagnostics().recorded==0,"DisabledBeforeNeedsNoRuntimeOrGpuWork");
    settings.enabled=true;
    Check(!bridge.Evaluate({},settings),"EnabledBeforeRejectsMissingContract");
    ComPtr<IDXGIFactory6> factory;Need(CreateDXGIFactory2(0,IID_PPV_ARGS(&factory)));
    ComPtr<IDXGIAdapter1> adapter;DXGI_ADAPTER_DESC1 desc{};
    for(UINT i=0;;++i){ComPtr<IDXGIAdapter1> a;const auto hr=factory->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&a));if(hr==DXGI_ERROR_NOT_FOUND)break;Need(hr);Need(a->GetDesc1(&desc));if(desc.VendorId==0x10de&&desc.DeviceId==0x2702){adapter=a;break;}}
    if(!adapter)return 77;
    StageContract contract;contract.colorExtent=contract.guideExtent={width,height};
    Need(D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&contract.device)));
    D3D12_COMMAND_QUEUE_DESC queue{};Need(contract.device->CreateCommandQueue(&queue,IID_PPV_ARGS(&contract.queue)));
    auto* observedQueue=new ObservedQueue(contract.queue.Get());contract.queue.Attach(observedQueue);
    contract.adapterLuid={desc.AdapterLuid.LowPart,desc.AdapterLuid.HighPart};
    ComPtr<ID3D11Device> device11;ComPtr<ID3D11DeviceContext> context;
    Need(D3D11CreateDevice(adapter.Get(),D3D_DRIVER_TYPE_UNKNOWN,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&device11,nullptr,&context));
    auto owner=std::make_shared<RuntimeOwner>(RuntimeOwnerPaths{argv[1],argv[2],std::filesystem::absolute("nr-before-cache"),true});
    AdapterIdentity id{desc.VendorId,desc.DeviceId,desc.SubSysId,contract.adapterLuid,false};
    auto opened=owner->Open(RuntimeCatalog()[1],contract.device.Get(),id);if(!opened){std::puts(opened.error().message.c_str());return 1;}
    auto initialized=bridge.Initialize(owner,device11.Get(),contract,0,nullptr,nullptr,ColorDomain::Linear,Placement::Before,passes);if(!initialized){std::printf("%s native=0x%08x\n",initialized.error().message.c_str(),unsigned(initialized.error().nativeCode));return 1;}
    Check(bridge.Diagnostics().create==1,"BeforeOwnsExactlyOneSharedStageFeature");
    auto color=Texture(device11.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT);
    auto depth=Texture(device11.Get(),DXGI_FORMAT_R32_FLOAT);
    auto motion=Texture(device11.Get(),DXGI_FORMAT_R16G16_FLOAT);
    auto readback=Texture(device11.Get(),DXGI_FORMAT_R16G16B16A16_FLOAT,true);
    std::vector<float> depths(width*height,.5f);std::vector<uint16_t> motions(width*height*2);
    context->UpdateSubresource(depth.Get(),0,nullptr,depths.data(),width*4,0);
    context->UpdateSubresource(motion.Get(),0,nullptr,motions.data(),width*4,0);
    BeforeInput input;input.context=context;input.color=color;input.depth=depth;input.motion=motion;
    input.colorExtent=input.guideExtent={width,height};input.colorDomain=ColorDomain::Linear;
    input.epoch=input.guideEpoch=1;input.motionScaleX=float(width);input.motionScaleY=float(height);
    input.sourceId=input.guideSourceId=1;input.presentationTime=1./60;
    auto unknown=input;unknown.colorDomain=ColorDomain::Unknown;
    Check(!bridge.Evaluate(unknown,settings)&&bridge.Diagnostics().evaluate==0,"UnknownSourceEncodingRejectedBeforeVendorWork");
    auto stale=input;stale.guideSourceId=2;
    Check(!bridge.Evaluate(stale,settings)&&bridge.Diagnostics().evaluate==0,"StaleGuidesRejectedBeforeVendorWork");
    D3D11_TEXTURE2D_DESC noTargetDesc{};color->GetDesc(&noTargetDesc);noTargetDesc.BindFlags=D3D11_BIND_SHADER_RESOURCE;
    auto noTarget=input;Need(device11->CreateTexture2D(&noTargetDesc,nullptr,&noTarget.color));
    if(argc==4&&std::wstring_view(argv[3])==L"copy-without-rt"){
        auto direct=bridge.Evaluate(noTarget,settings);
        Check(direct&&direct->evaluated,"NativeNrDeliveryCanCopyIntoSourceWithoutRenderTargetBinding");
        if(!direct)return 1;
        Check(bool(bridge.WaitDelivery(*direct))&&bool(bridge.Retire())&&bool(owner->Retire()),"CopyOnlyDeliveryRetiresGenuineReaders");
        return failures?1:0;
    }
    auto late=settings;late.placement=Placement::After;
    Check(!bridge.Evaluate(input,late)&&bridge.Diagnostics().evaluate==0,"AfterCannotSilentlyRunBefore");
    auto ratio=settings;ratio.reconstruction.method=ResolveMethod::Ratio;
    Check(!bridge.Evaluate(input,ratio)&&bridge.Diagnostics().evaluate==0,"NativeBeforeRejectsUnpreparedRatioBeforeGpuWork");
    if(failures)return 1;
    if(argc==4&&std::wstring_view(argv[3])==L"wait-failure"){
        observedQueue->failWait=true;
        auto failed=bridge.Evaluate(input,settings);
        const auto state=bridge.Diagnostics();
        Check(!failed&&observedQueue->waits==1&&state.evaluate==0&&state.recorded==0&&state.terminal,
            "WholeBeforeQueueWaitFailureRecordsNoVendorEvaluation");
        Check(!bridge.Retire()&&!owner->Retire(),"FailedBeforeWaitRetainsUncertainOwners");
        return failures?1:0;
    }
    uint64_t alphaPixels{},finitePixels{},changedPixels{},bypassPreservedPixels{};uint32_t evaluated{},bypassed{},resumedReset{};
    std::vector<std::string> sourceHashes,outputHashes,rgbHashes;
    std::vector<uint16_t> pixels(width*height*4);
    for(UINT frame=0;frame<240;++frame){
        for(UINT y=0;y<height;++y)for(UINT x=0;x<width;++x){const size_t p=(size_t(y)*width+x)*4;
            const float v=((x/8+y/8+frame)%2)?.2f:.8f;
            pixels[p]=DirectX::PackedVector::XMConvertFloatToHalf(v);
            pixels[p+1]=DirectX::PackedVector::XMConvertFloatToHalf(.1f+.6f*float((x+frame)%width)/width);
            pixels[p+2]=DirectX::PackedVector::XMConvertFloatToHalf(.1f+.6f*float((y+frame)%height)/height);
            pixels[p+3]=DirectX::PackedVector::XMConvertFloatToHalf(float((x+y+frame)%5)*.25f);}
        context->UpdateSubresource(color.Get(),0,nullptr,pixels.data(),width*8,0);
        sourceHashes.push_back(Hash(pixels));
        input.sourceId=input.guideSourceId=frame+1;input.previousSourceId=frame;
        input.presentationTime=double(frame+1)/60.;input.reset=frame==120;
        settings.enabled=!(frame>=80&&frame<88);settings.revision=frame<80?1:frame<88?2:3;
        if(passes>1)settings.additionalTuning[passes-2].style=int((frame/20)%3);
        const auto producerWaitsBefore=observedQueue->waits;
        auto result=bridge.Evaluate(input,settings);
        if(!result){std::printf("frame %u: %s\n",frame,result.error().message.c_str());return 1;}
        Check(observedQueue->waits-producerWaitsBefore==(result->evaluated?1u:0u),"ExactlyOneQueueWaitForEachBeforeProducerHandoff");
        if(result->evaluated)++evaluated;else ++bypassed;
        if((frame==0||frame==88||frame==120)&&result->effectiveReset)++resumedReset;
        if(result->evaluated&&!bridge.WaitDelivery(*result))return 1;
        // An SR consumer on this immediate context can now read the modified
        // color. This is a native bridge readback, not an SR/FG proof.
        context->CopyResource(readback.Get(),color.Get());D3D11_MAPPED_SUBRESOURCE mapped{};
        Need(context->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped));
        std::vector<uint16_t> measured(width*height*4),rgbPixels;rgbPixels.reserve(width*height*3);
        for(UINT y=0;y<height;++y){auto row=reinterpret_cast<const uint16_t*>(static_cast<const unsigned char*>(mapped.pData)+size_t(y)*mapped.RowPitch);
            for(UINT x=0;x<width;++x){const size_t p=(size_t(y)*width+x)*4;const auto* rgb=row+x*4;
                std::copy_n(rgb,4,measured.data()+p);rgbPixels.insert(rgbPixels.end(),rgb,rgb+3);
                if(rgb[3]==pixels[p+3])++alphaPixels;
                if(std::isfinite(DirectX::PackedVector::XMConvertHalfToFloat(rgb[0]))&&std::isfinite(DirectX::PackedVector::XMConvertHalfToFloat(rgb[1]))&&std::isfinite(DirectX::PackedVector::XMConvertHalfToFloat(rgb[2])))++finitePixels;
                if(result->evaluated&&(rgb[0]!=pixels[p]||rgb[1]!=pixels[p+1]||rgb[2]!=pixels[p+2]))++changedPixels;
                if(!result->evaluated&&std::equal(rgb,rgb+4,pixels.data()+p))++bypassPreservedPixels;}}
        context->Unmap(readback.Get(),0);
        outputHashes.push_back(Hash(measured));if(result->evaluated)rgbHashes.push_back(Hash(rgbPixels));
        if(frame==0)Check(!bridge.Evaluate(input,settings)&&bridge.Diagnostics().evaluate==1,"DuplicateSourceRejectedWithoutSecondEvaluation");
    }
    Check(evaluated==232&&bypassed==8&&bridge.Diagnostics().recorded==232&&bridge.Diagnostics().evaluatedPasses==232*passes,"NativeBridge240FramesWithLiveOffOnAndAllRequestedNrPasses");
    Check(resumedReset==3,"FirstReenabledAndExplicitResetAreEffective");
    Check(alphaPixels==uint64_t(width)*height*240,"ExactSourceAlphaPreservedAcrossD3D11RoundTrip");
    Check(finitePixels==alphaPixels&&changedPixels>uint64_t(width)*height,"FiniteNrModifiedRgbReturnedToD3D11");
    Check(bypassPreservedPixels==uint64_t(width)*height*8,"DisabledNrLeavesEverySourcePixelUnchanged");
    const std::set<std::string> distinct(rgbHashes.begin(),rgbHashes.end());
    Check(distinct.size()==232,"EveryEvaluatedNativeFrameHasDistinctRgbOutput");
    const auto pooled=bridge.Diagnostics();
    Check(pooled.bridgeSlots==3&&pooled.descriptorOwners==3*passes&&pooled.parameterOwners==3*passes&&!pooled.pendingTickets,"ThreeRetainedBridgeSlotsReusePerPassDescriptorsAndParameters");
    const auto retired=bridge.Retire();Check(bool(retired),"BeforeRetiresStageAfterD3D11Readers");
    Check(bool(owner->Retire()),"BeforeRuntimeShutsDownAfterFeatureRetirement");
    const auto diagnostics=bridge.Diagnostics();
    Check(diagnostics.release==1&&diagnostics.destroyParameters==1&&diagnostics.allocations==diagnostics.releases,"BeforeFeatureAndCallbackAllocationsRetired");
    const auto receipt=passes>1?std::filesystem::path(passes==2?"nr-before-chain-two.json":"nr-before-chain-three.json"):argc==4?std::filesystem::path(argv[3]):std::filesystem::path("nr-before-probe.json");
    std::filesystem::create_directories(std::filesystem::absolute(receipt).parent_path());std::ofstream out(receipt);
    auto coreEvidence=RuntimeFileLease::OpenDriverCore(argv[2]);if(!coreEvidence)return 1;
    out<<"{\n\"schema\":1,\"result\":"<<std::quoted(failures?"FAIL":"PASS")
        <<",\"sourceRevision\":"<<std::quoted(NrRuntimeResearch::buildRevision)<<",\"cleanSource\":"<<(NrRuntimeResearch::buildClean?"true":"false")
        <<",\"compiledSourceSha256\":"<<NrRuntimeResearch::compiledSourcesJson
        <<",\"scope\":\"standalone native D3D11-NR-D3D11 bridge, no SR/FG/game/UI qualification\""
        <<",\"guideScenario\":\"static depth and zero motion with changing color\""
        <<",\"vendorId\":"<<desc.VendorId<<",\"deviceId\":"<<desc.DeviceId<<",\"adapterLuidLow\":"<<desc.AdapterLuid.LowPart<<",\"adapterLuidHigh\":"<<desc.AdapterLuid.HighPart
        <<",\"runtimeSha256\":"<<std::quoted(std::string(RuntimeCatalog()[1].sha256))<<",\"coreSha256\":"<<std::quoted(coreEvidence->Sha256())
        <<",\"width\":"<<width<<",\"height\":"<<height<<",\"frames\":240,\"evaluated\":"<<evaluated<<",\"bypassed\":"<<bypassed
        <<",\"sourceAlphaPreservedPixels\":"<<alphaPixels<<",\"finitePixels\":"<<finitePixels<<",\"changedFromInputPixels\":"<<changedPixels
        <<",\"bypassPreservedPixels\":"<<bypassPreservedPixels<<",\"distinctEvaluatedRgbHashes\":"<<distinct.size()<<",\"effectiveResetChecks\":"<<resumedReset
        <<",\"allocations\":"<<diagnostics.allocations<<",\"releases\":"<<diagnostics.releases<<",\"rawInit\":"<<owner->LastInitResult()
        <<",\"rawCreate\":"<<diagnostics.create<<",\"rawEvaluate\":"<<diagnostics.evaluate<<",\"rawRelease\":"<<diagnostics.release<<",\"rawDestroyParameters\":"<<diagnostics.destroyParameters<<",\"rawShutdown\":"<<owner->LastShutdownResult();
    auto hashes=[&](const char* key,const auto& values){out<<",\""<<key<<"\":[";for(size_t i=0;i<values.size();++i){if(i)out<<',';out<<std::quoted(values[i]);}out<<']';};
    hashes("sourceRgbaSha256",sourceHashes);hashes("outputRgbaSha256",outputHashes);hashes("evaluatedRgbSha256",rgbHashes);out<<"\n}\n";if(!out)throw 1;
    std::printf("BEFORE_BRIDGE evaluated=%u bypassed=%u alpha=%llu changed=%llu\n",evaluated,bypassed,(unsigned long long)alphaPixels,(unsigned long long)changedPixels);
    return failures?1:0;
}catch(...){return 1;}}
