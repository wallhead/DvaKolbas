#include "FSRGenerationGuideAdapter.h"
#include "FrameGen/D3D11ContextIsolation.h"
#include <cmath>
namespace TheosRenderPipeline::Upscaling
{
    using Microsoft::WRL::ComPtr;
    struct FsrGenerationGuideAdapter::State
    {
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;
        ComPtr<ID3D11Texture2D> depth,motion;
        ComPtr<ID3D12Resource> depth12,motion12;
        D3D11FrameCopy::Depth depthCopy;
        D3D11ContextIsolation isolation;
        uint64_t source{},epoch{};bool published{};
    };
    FsrGenerationGuideAdapter::FsrGenerationGuideAdapter(std::shared_ptr<Graphics::D3D11D3D12Interop> bridge,
        GpuFrameResources resources,ID3D11Texture2D* depth,ID3D11Texture2D* motion):state_(std::make_unique<State>())
    {
        state_->bridge=std::move(bridge);state_->depth=depth;state_->motion=motion;
        state_->depth12=resources.depth;state_->motion12=resources.motion;
    }
    FsrGenerationGuideAdapter::~FsrGenerationGuideAdapter()
    { if(state_->bridge && FAILED(state_->bridge->Drain()))(void)state_.release(); }
    Result<void> FsrGenerationGuideAdapter::Prepare(const UpscaleFrame& frame)
    {
        auto fail=[](ErrorKind kind,HRESULT hr,const char* text)->Result<void>{return std::unexpected(RuntimeError{kind,hr,text});};
        if(!state_->bridge || !state_->bridge->Ready() || !state_->depth || !state_->motion ||
            !state_->depth12 || !state_->motion12 || !frame.depth || !frame.motion || !frame.sourceId ||
            (state_->published && frame.sourceEpoch==state_->epoch && frame.sourceId<=state_->source) ||
            !std::isfinite(frame.jitterX) || !std::isfinite(frame.jitterY) ||
            !std::isfinite(frame.motionConvention.scaleX) || !std::isfinite(frame.motionConvention.scaleY))
            return fail(ErrorKind::InvalidInput,E_INVALIDARG,"FSR FG external guides require a fresh source and finite measured conventions");
        auto* context=state_->bridge->Context11();
        D3D11_TEXTURE2D_DESC depth{},motion{},target{};
        frame.depth->GetDesc(&depth);frame.motion->GetDesc(&motion);state_->depth->GetDesc(&target);
        if(frame.render!=Extent{target.Width,target.Height} || depth.Width!=target.Width || depth.Height!=target.Height ||
            motion.Width!=target.Width || motion.Height!=target.Height || motion.Format!=DXGI_FORMAT_R16G16_FLOAT ||
            !D3D11FrameCopy::ValidResources(context,frame.depth,state_->depth.Get()) ||
            !D3D11FrameCopy::ValidResources(context,frame.motion,state_->motion.Get()))
            return fail(ErrorKind::InvalidInput,E_INVALIDARG,"FSR FG external depth/motion must match the current render size and producer device");
        auto hr=state_->bridge->WaitD3D11(Graphics::InteropWork::FrameGeneration);
        if(FAILED(hr))return fail(ErrorKind::RetirementFailure,hr,"FSR FG guide readers could not retire before producer reuse");
        D3D11ContextIsolation::Scope scope(state_->isolation,context);
        if(!scope)return fail(ErrorKind::ContextFailure,E_FAIL,"FSR FG guide producer state isolation failed");
        hr=state_->depthCopy.Copy(context,frame.depth,state_->depth.Get(),{frame.render.width,frame.render.height});
        if(SUCCEEDED(hr))hr=D3D11FrameCopy::Color(context,frame.motion,state_->motion.Get(),{frame.render.width,frame.render.height});
        if(FAILED(hr))return fail(ErrorKind::InvalidInput,hr,"FSR FG external guide conversion failed");
        hr=state_->bridge->SignalD3D11(Graphics::InteropWork::FrameGeneration);
        if(FAILED(hr))return fail(ErrorKind::DeviceLost,hr,"FSR FG external guide producer submission failed");
        state_->source=frame.sourceId;state_->epoch=frame.sourceEpoch;state_->published=true;return {};
    }
}
