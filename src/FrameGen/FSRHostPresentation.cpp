#include "FSRHostPresentation.h"
namespace TheosRenderPipeline
{
    using namespace Upscaling;
    struct FsrHostPresentation::State
    {
        FsrPresentation presenter;
        std::shared_ptr<FsrHostResources> resources;
        FsrEffectProvider provider{FsrEffect::FrameGeneration,{}};
        FsrGenerationLimits limits{};ColorEncoding encoding{ColorEncoding::Unknown};
        bool created{},feature{},closing{};
    };
    FsrHostPresentation::FsrHostPresentation():state_(std::make_unique<State>()){}
    FsrHostPresentation::~FsrHostPresentation(){if(!Retire())(void)state_.release();}
    Result<Extent> FsrHostPresentation::Create(IDXGIFactory* factory,ID3D11Device* device,
        std::shared_ptr<FsrHostResources> resources,const DXGI_SWAP_CHAIN_DESC& input,const FsrSettings& settings)
    {
        auto invalid=[](const char* text)->Result<Extent>{return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,text});};
        if(state_->created || state_->closing || !factory || !device || !resources || !ValidFsrSettings(settings) ||
            settings.providerPolicy!=ProviderPolicy::Analytical || !IsKnownColorEncoding(settings.sourceColorEncoding))
            return invalid("AMD host requires a fresh owner, analytical SR and explicit native SDR encoding");
        auto descriptor=FsrPresentation::TranslateDescriptor(input);if(!descriptor)return std::unexpected(descriptor.error());
        BackendConfiguration config;config.backend=BackendKind::Fsr;config.generationEnabled=false;config.generationBackend=0;
        config.quality=settings.quality;config.providerPolicy=settings.providerPolicy;config.sharpness=settings.sharpness;
        state_->resources=std::move(resources);state_->encoding=settings.sourceColorEncoding;
        const Extent output{descriptor->BufferDesc.Width,descriptor->BufferDesc.Height};
        auto render=state_->resources->PrepareSizing(device,config,output,descriptor->BufferDesc.Format,state_->encoding);
        if(!render)return std::unexpected(render.error());
        auto runtime=state_->resources->Runtime();
        // The absolute plugin directory belongs to the SR owner; it performs
        // the optional load without creating a second runtime or changing DLL search.
        auto loaded=state_->resources->LoadFrameGeneration();if(!loaded)return std::unexpected(loaded.error());
        FsrEffectProvider swap{FsrEffect::FrameGenerationSwapChain,{}};
        {
            auto lock=state_->presenter.Session()->Lock();
            if(!lock.Owns(*state_->presenter.Session()))return invalid("AMD provider discovery session unavailable");
            auto catalog=runtime->EnumerateForEffect(state_->resources->Bridge()->Device12(),FsrEffect::FrameGeneration);
            if(!catalog)return std::unexpected(catalog.error());auto selected=SelectFsrEffectProvider(*catalog,FsrEffect::FrameGeneration);
            if(!selected)return std::unexpected(selected.error());state_->provider=*selected;
            catalog=runtime->EnumerateForEffect(state_->resources->Bridge()->Device12(),FsrEffect::FrameGenerationSwapChain);
            if(!catalog)return std::unexpected(catalog.error());selected=SelectFsrEffectProvider(*catalog,FsrEffect::FrameGenerationSwapChain);
            if(!selected)return std::unexpected(selected.error());swap=*selected;
        }
        auto created=state_->presenter.Create(factory,runtime,state_->resources->Bridge(),*descriptor,swap);
        if(!created)return std::unexpected(created.error());
        state_->limits.render=*render;state_->limits.display=output;state_->limits.format=descriptor->BufferDesc.Format;
        state_->created=true;return *render;
    }
    HRESULT FsrHostPresentation::StartupPresent(UINT interval,UINT flags)
    {
        if(!state_->created || state_->closing)return E_UNEXPECTED;
        return state_->presenter.Present({},UpscaleOutcome::SkippedInvalidInput,{},nullptr,ColorEncoding::Unknown,nullptr,nullptr,false,false,false,interval,flags);
    }
    HRESULT FsrHostPresentation::WaitBeforeProducer()
    {return !state_->closing && state_->created?state_->presenter.WaitBeforeProducer():E_UNEXPECTED;}
    HRESULT FsrHostPresentation::Present(const UpscaleFrame& frame,UpscaleOutcome outcome,ID3D11Texture2D* ui,
        ID3D11ShaderResourceView* overlay,bool complete,bool menu,bool requested,UINT interval,UINT flags)
    {
        if(flags&DXGI_PRESENT_TEST)return StartupPresent(interval,flags);
        if(!state_->created || state_->closing || !state_->resources->FeatureReady())return E_UNEXPECTED;
        if(outcome==UpscaleOutcome::Temporal){
            const auto input=state_->resources->Upscaler()->Limits().input;
            if(frame.camera.depthInverted!=input.depthInverted || frame.camera.depthInfinite!=input.depthInfinite ||
                frame.motionConvention.includesJitter!=input.motionIncludesJitter)return E_INVALIDARG;
            if(!state_->feature){state_->limits.input=input;auto started=state_->presenter.CompleteStartup(state_->limits,state_->provider);
                if(!started)return E_FAIL;state_->feature=true;}
            else if(state_->limits.input!=input)return E_INVALIDARG; // An immutable convention change needs a new owner.
        }
        return state_->presenter.Present(frame,outcome,state_->resources->Resources(),state_->presenter.SceneTarget11(),
            state_->encoding,ui,overlay,complete,menu,requested,interval,flags);
    }
    Result<void> FsrHostPresentation::Retire()
    {
        state_->closing=true;auto retired=state_->presenter.Retire();if(!retired)return retired;
        if(state_->resources){retired=state_->resources->Retire();if(!retired)return retired;}
        state_->feature=false;return {};
    }
    IDXGISwapChain4* FsrHostPresentation::SwapChain()const{return state_->presenter.SwapChain();}
    ID3D11Texture2D* FsrHostPresentation::SceneTarget11()const{return state_->presenter.SceneTarget11();}
    bool FsrHostPresentation::FeatureReady()const{return state_->feature && !state_->closing;}
    FsrPresentationStatus FsrHostPresentation::Status()const{return state_->presenter.Status();}
}
