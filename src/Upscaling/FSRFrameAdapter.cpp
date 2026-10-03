#include "FSRFrameAdapter.h"
#include "FrameGen/D3D11FrameCopy.h"
#include "NeuralRendering/PreparedFsrInput.h"
#include <cmath>
#include <optional>
namespace TheosRenderPipeline::Upscaling
{
    using Microsoft::WRL::ComPtr;
    namespace {
        struct CpuObservation {
            FsrPerformanceObserver* observer;FsrCpuPhase phase;
            CpuObservation(FsrPerformanceObserver* p,FsrCpuPhase v):observer(p),phase(v){if(observer)observer->Cpu(phase,true);}
            ~CpuObservation(){if(observer)observer->Cpu(phase,false);}
        };
        template<class Operation>decltype(auto) Observe(FsrPerformanceObserver* p,FsrCpuPhase phase,Operation&& operation){
            CpuObservation scope(p,phase);return std::forward<Operation>(operation)();
        }
        struct Query11Observation {
            FsrPerformanceObserver* observer;ID3D11DeviceContext* context;
            Query11Observation(FsrPerformanceObserver* p,ID3D11DeviceContext* c,uint64_t source):observer(p),context(c){if(observer)observer->Begin11(context,source);}
            ~Query11Observation(){if(observer)observer->End11(context);}
        };
    }
    struct FsrFrameAdapter::State
    {
        FsrUpscaler* upscaler{};std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;GpuFrameResources resources;
        ComPtr<ID3D11Texture2D> color,depth,motion,output;
        ColorEncoding encoding{ColorEncoding::Unknown};FsrColorConverter decode,encode,spatial;
        D3D11FrameCopy::Depth depthCopy;D3D11ContextIsolation isolation;FsrHistoryPolicy history;
        std::optional<RuntimeError> error;bool recovery{},lastReset{};
        FsrPerformanceObserver* performance{};
    };
    FsrFrameAdapter::FsrFrameAdapter(FsrUpscaler& upscaler,std::shared_ptr<Graphics::D3D11D3D12Interop> bridge,GpuFrameResources resources,
        ID3D11Texture2D* color,ID3D11Texture2D* depth,ID3D11Texture2D* motion,ID3D11Texture2D* output,ColorEncoding encoding,FsrPerformanceObserver* performance):state_(std::make_unique<State>())
    {
        state_->upscaler=&upscaler;state_->bridge=std::move(bridge);state_->resources=resources;
        state_->color=color;state_->depth=depth;state_->motion=motion;state_->output=output;state_->encoding=encoding;
        state_->performance=performance;
    }
    FsrFrameAdapter::~FsrFrameAdapter(){if(state_->bridge && FAILED(state_->bridge->Drain()))(void)state_.release();}
    void FsrFrameAdapter::InvalidateHistory(){state_->history.Invalidate();}
    const RuntimeError* FsrFrameAdapter::LastError()const{return state_->error?&*state_->error:nullptr;}
    bool FsrFrameAdapter::LastTemporalReset()const{return state_->lastReset;}
    static bool NativeImage(const UpscaleFrame& frame)
    {
        if(!frame.input || !frame.output || !frame.render.width || !frame.render.height || !frame.display.width || !frame.display.height)return false;
        D3D11_TEXTURE2D_DESC in{},out{};frame.input->GetDesc(&in);frame.output->GetDesc(&out);
        return in.Width==frame.render.width && in.Height==frame.render.height && out.Width==frame.display.width && out.Height==frame.display.height;
    }
    Result<UpscaleOutcome> FsrFrameAdapter::Spatial(const UpscaleFrame& frame)
    {
        state_->history.Invalidate();state_->lastReset=false;
        if(!state_->bridge || !state_->bridge->Ready()) {
            state_->error=RuntimeError{ErrorKind::RetirementFailure,state_->bridge?state_->bridge->Fault():E_POINTER,"FSR bridge is unavailable; spatial recovery cannot continue"};
            return std::unexpected(*state_->error);
        }
        if(!NativeImage(frame)) {
            state_->error=RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,"FSR spatial input/output extent is invalid"};
            return UpscaleOutcome::SkippedInvalidInput;
        }
        auto hr=state_->spatial.Convert(state_->bridge->Context11(),frame.input,frame.output,frame.colorIsLinear?ColorEncoding::Linear:state_->encoding,state_->encoding);
        if(FAILED(hr)) {
            state_->error=RuntimeError{FAILED(state_->bridge->Device12()->GetDeviceRemovedReason())?ErrorKind::DeviceLost:ErrorKind::InvalidInput,hr,
                std::string("FSR native spatial conversion failed at ")+state_->spatial.FailureStage()};
            return std::unexpected(*state_->error);
        }
        return UpscaleOutcome::SpatialRecovery;
    }
    Result<UpscaleOutcome> FsrFrameAdapter::Evaluate(const UpscaleFrame& frame)
    {return EvaluateInternal(frame,nullptr);}
    Result<UpscaleOutcome> FsrFrameAdapter::EvaluatePrepared(const UpscaleFrame& frame,NeuralRendering::PreparedFsrInput& input)
    {return EvaluateInternal(frame,&input);}
    Result<UpscaleOutcome> FsrFrameAdapter::EvaluateInternal(const UpscaleFrame& frame,NeuralRendering::PreparedFsrInput* linear)
    {
        CpuObservation total(state_->performance,FsrCpuPhase::Total);
        state_->lastReset=false;
        if(!state_->recovery)state_->error.reset();
        auto invalid=[&]()->Result<UpscaleOutcome>{state_->history.Invalidate();return UpscaleOutcome::SkippedInvalidInput;};
        auto fatal=[&](HRESULT hr,const char* text)->Result<UpscaleOutcome>{state_->history.Invalidate();const bool removed=hr==DXGI_ERROR_DEVICE_REMOVED || hr==DXGI_ERROR_DEVICE_RESET || hr==DXGI_ERROR_DEVICE_HUNG || FAILED(state_->bridge->Device12()->GetDeviceRemovedReason());state_->error=RuntimeError{removed?ErrorKind::DeviceLost:ErrorKind::RetirementFailure,hr,text};return std::unexpected(*state_->error);};
        if(frame.backend!=BackendKind::Fsr || !frame.sourceId || !std::isfinite(frame.deltaMilliseconds) || frame.deltaMilliseconds<=0 || !NativeImage(frame))return invalid();
        if(!state_->bridge)return std::unexpected(RuntimeError{ErrorKind::ContextFailure,0,"FSR frame has no bridge"});
        if(!state_->bridge->Ready())return fatal(state_->bridge->Fault(),"FSR bridge fault; stop rendering");
        if(linear){
            if(!linear->Valid()||linear->SourceId()!=frame.sourceId||linear->Epoch()!=frame.sourceEpoch||!linear->ProducerFence()||!linear->ProducerValue()||linear->Width()!=frame.render.width||linear->Height()!=frame.render.height||!D3D11FrameCopy::SameObject(linear->Context(),state_->bridge->Context11()))return invalid();
            const auto valid=[&](ID3D11Texture2D* texture,DXGI_FORMAT format){if(!texture)return false;D3D11_TEXTURE2D_DESC desc{};texture->GetDesc(&desc);ComPtr<ID3D11Device> actual,expected;texture->GetDevice(&actual);state_->bridge->Context11()->GetDevice(&expected);return D3D11FrameCopy::SameObject(actual.Get(),expected.Get())&&desc.Format==format&&desc.Width==frame.render.width&&desc.Height==frame.render.height&&desc.SampleDesc.Count==1&&desc.ArraySize==1&&desc.MipLevels==1;};
            if(!valid(linear->Color(),DXGI_FORMAT_R16G16B16A16_FLOAT)||!valid(linear->Depth(),DXGI_FORMAT_R32_FLOAT)||!valid(linear->Motion(),DXGI_FORMAT_R16G16_FLOAT))return invalid();
            if(state_->recovery){
                auto spatialFrame=frame;spatialFrame.input=spatialFrame.color=linear->Color();spatialFrame.colorIsLinear=true;
                auto result=Spatial(spatialFrame);if(!result||*result!=UpscaleOutcome::SpatialRecovery)return result;
                // Spatial recovery draws directly from the leased NR color;
                // its genuine final reader is this draw, not an FSR dispatch.
                ComPtr<ID3D12Fence> drawn;uint64_t value{};auto hr=state_->bridge->SignalReader(&drawn,&value);if(FAILED(hr))return fatal(hr,"FSR spatial NR reader submission failed");
                auto tracked=linear->TrackReader(drawn.Get(),value);if(!tracked)return fatal(E_FAIL,"FSR spatial NR reader ownership rejected");return result;
            }
        }
        if(state_->recovery)return Spatial(frame);
        if(frame.exposure || frame.reactive || frame.transparencyComposition) {
            state_->error=RuntimeError{ErrorKind::InvalidInput,0,"FSR external exposure/reactive/transparency guides are unsupported by this SR adapter; using spatial recovery"};
            return invalid();
        }
        if(!frame.depth || !frame.motion)return invalid();
        D3D11_TEXTURE2D_DESC depth{},motion{};frame.depth->GetDesc(&depth);frame.motion->GetDesc(&motion);
        if(depth.Width!=frame.render.width || depth.Height!=frame.render.height || motion.Width!=frame.render.width || motion.Height!=frame.render.height || motion.Format!=DXGI_FORMAT_R16G16_FLOAT)return invalid();
        UpscaleFrame prepared=frame;prepared.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;prepared.depthFormat=DXGI_FORMAT_R32_FLOAT;prepared.motionFormat=DXGI_FORMAT_R16G16_FLOAT;prepared.colorIsLinear=true;
        auto parameters=BuildFsrDispatch(state_->resources,prepared,state_->upscaler->Limits());if(!parameters){state_->error=parameters.error();return invalid();}
        auto decision=state_->history.Accept(frame.sourceId,frame.camera,frame.render,false,false);if(!decision.valid)return invalid();
        prepared.reset=frame.reset || decision.reset || (linear&&linear->Reset());state_->lastReset=prepared.reset;
        auto* context=state_->bridge->Context11();HRESULT hr{};
        auto* performance=state_->performance;
        Query11Observation query11(performance,context,frame.sourceId);
        if(performance)performance->Stamp11(context,FsrGpuPhase::Prepare,true);
        const auto inputEncoding=frame.colorIsLinear?ColorEncoding::Linear:state_->encoding;
        if(linear){
            D3D11ContextIsolation::Scope scope(state_->isolation,context);if(!scope)return fatal(E_FAIL,"FSR prepared copy state isolation failed");
            hr=D3D11FrameCopy::Color(context,linear->Color(),state_->color.Get(),{frame.render.width,frame.render.height});
            if(SUCCEEDED(hr))hr=D3D11FrameCopy::Color(context,linear->Depth(),state_->depth.Get(),{frame.render.width,frame.render.height});
            if(SUCCEEDED(hr))hr=D3D11FrameCopy::Color(context,linear->Motion(),state_->motion.Get(),{frame.render.width,frame.render.height});
            if(FAILED(hr))return fatal(hr,"FSR prepared NR input copies failed");
        } else if(frame.input!=state_->color.Get()) {
            hr=Observe(performance,FsrCpuPhase::PrepareColor,[&]{return state_->decode.Convert(context,frame.input,state_->color.Get(),inputEncoding,ColorEncoding::Linear);});
            if(FAILED(hr)){if(FAILED(state_->bridge->Device12()->GetDeviceRemovedReason()))return fatal(hr,"FSR device removed during input conversion");state_->error=RuntimeError{ErrorKind::InvalidInput,hr,"FSR explicit input color conversion failed"};return invalid();}
        } else if(inputEncoding!=ColorEncoding::Linear)return invalid();
        if(!linear){
            CpuObservation guides(performance,FsrCpuPhase::PrepareGuides);
            D3D11ContextIsolation::Scope scope(state_->isolation,context);if(!scope)return fatal(E_FAIL,"FSR producer state isolation failed");
            if(frame.depth!=state_->depth.Get())hr=state_->depthCopy.Copy(context,frame.depth,state_->depth.Get(),{frame.render.width,frame.render.height});
            if(SUCCEEDED(hr) && frame.motion!=state_->motion.Get())hr=D3D11FrameCopy::Color(context,frame.motion,state_->motion.Get(),{frame.render.width,frame.render.height});
            if(FAILED(hr)){state_->error=RuntimeError{ErrorKind::InvalidInput,hr,"FSR prepared depth/motion conversion failed"};return invalid();}
        }
        if(performance)performance->Stamp11(context,FsrGpuPhase::Prepare,false);
        if(FAILED(hr=Observe(performance,FsrCpuPhase::ProducerSignal,[&]{return state_->bridge->SignalProducer();})))return fatal(hr,"FSR producer submission failed");
        if(linear){
            // The last reader of the leased textures is their D3D11 input copy,
            // not dispatch: dispatch uses FSR's independently retained images.
            ComPtr<ID3D12Fence> copied;uint64_t value{};if(FAILED(hr=state_->bridge->ProducerDependency(&copied,&value)))return fatal(hr,"FSR prepared input reader dependency missing");
            auto retained=linear->TrackReader(copied.Get(),value);if(!retained)return fatal(E_FAIL,"FSR prepared input reader ownership rejected");
        }
        ID3D12GraphicsCommandList* list{};if(FAILED(hr=Observe(performance,FsrCpuPhase::Begin,[&]{return state_->bridge->Begin(&list);})))return fatal(hr,"FSR command slot unavailable");
        if(performance){performance->Begin12(list,frame.sourceId);performance->Stamp12(list,FsrGpuPhase::Dispatch,true);}
        auto dispatch=Observe(performance,FsrCpuPhase::Record,[&]{return state_->upscaler->Dispatch(list,state_->resources,prepared);});
        if(performance)performance->Stamp12(list,FsrGpuPhase::Dispatch,false);
        if(!dispatch) {
            state_->error=dispatch.error();state_->history.Invalidate();
            hr=state_->bridge->DiscardRecording();if(SUCCEEDED(hr)&&performance)performance->DiscardUnsubmitted12();
            if(FAILED(hr) || FAILED(hr=state_->bridge->Drain()))return fatal(hr,"FSR failed dispatch could not retire safely");
            if(dispatch.error().kind==ErrorKind::DeviceLost || dispatch.error().kind==ErrorKind::RetirementFailure || dispatch.error().kind==ErrorKind::ContextFailure)return std::unexpected(dispatch.error());
            if(dispatch.error().kind!=ErrorKind::DispatchFailure)return invalid();
            // The vendor may have changed CPU-side history while recording.
            // Discard its unsubmitted list and keep this session spatial until
            // restart, retaining the poisoned context until normal retirement.
            state_->recovery=true;
            // Prepared input was already copied and its reader registered.
            // Recover from that enhanced linear copy on the failing frame too.
            auto spatialFrame=frame;if(linear){spatialFrame.input=spatialFrame.color=state_->color.Get();spatialFrame.colorIsLinear=true;}return Spatial(spatialFrame);
        }
        if(performance)performance->Resolve12(list);
        if(FAILED(hr=Observe(performance,FsrCpuPhase::Submit,[&]{return state_->bridge->Submit();})) || FAILED(hr=state_->bridge->WaitConsumer()))return fatal(hr,"FSR dispatch/consumer dependency failed");
        if(performance)performance->Stamp11(context,FsrGpuPhase::Delivery,true);
        hr=Observe(performance,FsrCpuPhase::Delivery,[&]{return state_->encode.Convert(context,state_->output.Get(),frame.output,ColorEncoding::Linear,state_->encoding);});
        if(performance)performance->Stamp11(context,FsrGpuPhase::Delivery,false);
        if(FAILED(hr))return fatal(hr,"FSR native output delivery failed");
        state_->error.reset();return UpscaleOutcome::Temporal;
    }
}
