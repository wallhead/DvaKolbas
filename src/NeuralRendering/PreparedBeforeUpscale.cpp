#include "PreparedBeforeUpscale.h"
#include "PerformanceQueries.h"
#include "History.h"
#include "Upscaling/FSRColorConversion.h"
#include "FrameGen/D3D11FrameCopy.h"
#include <chrono>
namespace TheosRenderPipeline::NeuralRendering {
namespace {
using Microsoft::WRL::ComPtr;
std::unexpected<Error> Fail(ErrorKind kind,const char* text,int64_t hr=0){return std::unexpected(Error{kind,hr,text});}
}
struct PreparedBeforeUpscale::State {
    PerformanceMetrics* metrics{};std::unique_ptr<PerformanceQueries> timing;
    BeforeUpscale bridge;History history;
    ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
    ComPtr<ID3D11Texture2D> color,depth,motion;ComPtr<ID3D11Query> reader;
    Upscaling::FsrColorConverter decode,encode;
    D3D11FrameCopy::Depth depthCopy;D3D11ContextIsolation isolation;
    ImageExtent extent;unsigned preset{};BeforeInput held;
    bool attempted{},ready{},uncertain{},terminal{};
    Result<void> Gpu(HRESULT hr,const char* text){if(FAILED(hr)){terminal=true;return Fail(ErrorKind::Runtime,text,hr);}return {};}
    Result<void> FinishReaders(){
        PerformanceScope performance(metrics,CpuPhase::PreparedWait);
        context->End(reader.Get());context->Flush();if(metrics)metrics->RecordFlush();const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(20);
        uint64_t blocked{};bool slept=false;
        for(;;){BOOL done{};const auto hr=context->GetData(reader.Get(),&done,sizeof(done),D3D11_ASYNC_GETDATA_DONOTFLUSH);
            if(FAILED(hr))return Gpu(hr,"NR preparation reader query failed");if(hr==S_OK&&done){if(metrics)metrics->RecordWait(CpuPhase::PreparedWait,slept,blocked);return Gpu(device->GetDeviceRemovedReason(),"NR preparation device removed");}
            if(std::chrono::steady_clock::now()>=deadline){terminal=true;return Fail(ErrorKind::Retirement,"NR preparation copy-back reader completion unconfirmed");}
            const auto begin=metrics?PerformanceNow():0;Sleep(1);if(metrics){slept=true;blocked+=PerformanceNow()-begin;}}
    }
    bool Shape(ID3D11Texture2D* texture,D3D11_TEXTURE2D_DESC& d)const{
        if(!texture)return false;ComPtr<ID3D11Device> actual;texture->GetDevice(&actual);texture->GetDesc(&d);
        return D3D11FrameCopy::SameObject(actual.Get(),device.Get())&&d.Width==extent.width&&d.Height==extent.height&&
            d.MipLevels==1&&d.ArraySize==1&&d.SampleDesc.Count==1&&!d.SampleDesc.Quality&&d.Usage==D3D11_USAGE_DEFAULT;
    }
};
PreparedBeforeUpscale::PreparedBeforeUpscale():state_(std::make_unique<State>()){}
PreparedBeforeUpscale::~PreparedBeforeUpscale(){if(state_->uncertain)state_.release();}
Result<void> PreparedBeforeUpscale::Initialize(std::shared_ptr<RuntimeOwner> owner,ID3D11Device* device,const StageContract& c,unsigned preset,PerformanceMetrics* metrics){
    auto& s=*state_;if(s.attempted)return Fail(ErrorKind::Conflict,"NR source preparation initialization already attempted");s.attempted=true;
    if(!device||c.colorExtent!=c.guideExtent||!c.colorExtent.width||!c.colorExtent.height||preset>1)return Fail(ErrorKind::InvalidInput,"NR source preparation native contract invalid");
    s.device=device;device->GetImmediateContext(&s.context);s.extent=c.colorExtent;s.preset=preset;
    if(metrics&&metrics->Enabled()){s.metrics=metrics;s.timing=std::make_unique<PerformanceQueries>(metrics);s.timing->Initialize11(device);}
    auto make=[&](DXGI_FORMAT format,UINT bind,ComPtr<ID3D11Texture2D>& out){D3D11_TEXTURE2D_DESC d{};d.Width=s.extent.width;d.Height=s.extent.height;d.ArraySize=d.MipLevels=d.SampleDesc.Count=1;d.Format=format;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=bind;return s.Gpu(device->CreateTexture2D(&d,nullptr,&out),"NR prepared source allocation failed");};
    auto r=make(DXGI_FORMAT_R16G16B16A16_FLOAT,D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE,s.color);if(!r)return r;
    r=make(DXGI_FORMAT_R32_FLOAT,D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE,s.depth);if(!r)return r;
    r=make(DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE,s.motion);if(!r)return r;
    D3D11_QUERY_DESC query{D3D11_QUERY_EVENT,0};r=s.Gpu(device->CreateQuery(&query,&s.reader),"NR prepared output reader query creation failed");if(!r)return r;
    s.uncertain=true;r=s.bridge.Initialize(std::move(owner),device,c,preset,s.metrics,s.timing.get());if(!r){s.terminal=true;return r;}s.ready=true;return {};
}
Result<BeforeResult> PreparedBeforeUpscale::Evaluate(const BeforeInput& input,Upscaling::ColorEncoding encoding,const SettingsSnapshot& settings){
    auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Runtime,"NR source preparation terminal; ownership retained");
    PerformanceScope performance(s.metrics,CpuPhase::Total);
    if(!settings.enabled){s.history.ResetNext();return s.bridge.Evaluate(input,settings);}
    if(!s.ready)return Fail(ErrorKind::InvalidInput,"NR source preparation not initialized");
    if(!Upscaling::IsKnownColorEncoding(encoding))return Fail(ErrorKind::InvalidInput,"NR source encoding unknown; explicit Linear/Gamma22/SRGB required");
    if(settings.placement!=Placement::Before||settings.reconstruction.preset!=s.preset||settings.reconstruction.inputScale!=1||
        settings.reconstruction.method>ResolveMethod::Ratio||EffectiveResolve(settings.reconstruction)!=ResolveMethod::Auto||
        settings.reconstruction.colorIsHDR||settings.reconstruction.producerColor||settings.reconstruction.fusedPreparation||settings.reconstruction.peripheralCompression)
        return Fail(ErrorKind::Unsupported,"NR first source adapter supports one native SDR Before pass");
    D3D11_TEXTURE2D_DESC color{},depth{},motion{};
    if(input.colorExtent!=s.extent||input.guideExtent!=s.extent||input.guideEpoch!=input.epoch||input.guideSourceId!=input.sourceId||
        !input.context||!D3D11FrameCopy::SameObject(input.context.Get(),s.context.Get())||input.context->GetType()!=D3D11_DEVICE_CONTEXT_IMMEDIATE||
        !s.Shape(input.color.Get(),color)||!s.Shape(input.depth.Get(),depth)||!s.Shape(input.motion.Get(),motion)||
        !Upscaling::SupportsFsrHandoffFormat(color.Format)||(color.BindFlags&(D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET))!=(D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET)||
        !(depth.BindFlags&D3D11_BIND_SHADER_RESOURCE)||(depth.Format!=DXGI_FORMAT_R32_TYPELESS&&depth.Format!=DXGI_FORMAT_R32_FLOAT&&depth.Format!=DXGI_FORMAT_R24G8_TYPELESS&&depth.Format!=DXGI_FORMAT_R32G8X24_TYPELESS)||motion.Format!=DXGI_FORMAT_R16G16_FLOAT)
        return Fail(ErrorKind::InvalidInput,"NR native source color/depth/motion ownership or shape invalid");
    ImagePacket p;p.epoch=input.epoch;p.sourceId=input.sourceId;p.previousSourceId=input.previousSourceId;p.imageId=p.batchId=input.sourceId;p.presentationTime=input.presentationTime;p.reset=input.reset;
    auto history=s.history.Check(p,settings);if(!history)return std::unexpected(history.error());
    s.held=input;
    if(s.timing){s.timing->Begin11(input.context.Get(),input.sourceId);s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareColor,true);}
    auto r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareColor,[&]{return s.decode.Convert(input.context.Get(),input.color.Get(),s.color.Get(),encoding,Upscaling::ColorEncoding::Linear);}),"NR explicit SDR source decoding failed");if(!r)return std::unexpected(r.error());
    if(s.timing){s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareColor,false);s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareGuides,true);}
    {
        D3D11ContextIsolation::Scope scope(s.isolation,input.context.Get());if(!scope){s.terminal=true;return Fail(ErrorKind::Runtime,"NR guide preparation state isolation failed");}
        r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareGuides,[&]{return s.depthCopy.Copy(input.context.Get(),input.depth.Get(),s.depth.Get(),{s.extent.width,s.extent.height});}),"NR source depth preparation failed");if(!r)return std::unexpected(r.error());
        r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::PrepareGuides,[&]{return D3D11FrameCopy::Color(input.context.Get(),input.motion.Get(),s.motion.Get(),{s.extent.width,s.extent.height});}),"NR source motion preparation failed");if(!r)return std::unexpected(r.error());
    }
    if(s.timing)s.timing->Stamp11(input.context.Get(),GpuPhase::PrepareGuides,false);
    auto linear=input;linear.color=s.color;linear.depth=s.depth;linear.motion=s.motion;linear.colorDomain=ColorDomain::Linear;linear.reset=history->Reset();
    auto result=s.bridge.Evaluate(linear,settings);if(!result){s.terminal=true;return std::unexpected(result.error());}
    if(s.timing)s.timing->Stamp11(input.context.Get(),GpuPhase::Encode,true);
    r=s.Gpu(MeasurePerformance(s.metrics,CpuPhase::Encode,[&]{return s.encode.Convert(input.context.Get(),s.color.Get(),input.color.Get(),Upscaling::ColorEncoding::Linear,encoding);}),"NR explicit SDR source delivery failed");if(!r)return std::unexpected(r.error());
    if(s.timing){s.timing->Stamp11(input.context.Get(),GpuPhase::Encode,false);s.timing->End11(input.context.Get());}
    r=s.FinishReaders();if(!r)return std::unexpected(r.error());r=s.history.CommitRecorded(*history);if(!r){s.terminal=true;return std::unexpected(r.error());}
    s.held={};return *result;
}
Result<void> PreparedBeforeUpscale::Retire(){auto& s=*state_;if(s.terminal)return Fail(ErrorKind::Retirement,"NR preparation terminal; no teardown retry");if(!s.ready)return {};
    auto r=s.FinishReaders();if(!r)return r;if(s.timing)s.timing->Collect11(s.context.Get());r=s.bridge.Retire();if(!r){s.terminal=true;return r;}s.ready=false;s.uncertain=false;return {};}
StageDiagnostics PreparedBeforeUpscale::Diagnostics()const{auto d=state_->bridge.Diagnostics();if(state_->timing){d.gpuTiming11Available=state_->timing->Available11();d.gpuTimingDropped+=state_->timing->Dropped();}return d;}
}
