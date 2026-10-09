#include "FSRHostPresentation.h"
#include "FSRSwapChainPolicy.h"
#include "Upscaling/FSRGenerationGuideAdapter.h"
#include "PublicIniSchema.h"
namespace TheosRenderPipeline
{
    using namespace Upscaling;
    struct FsrHostPresentation::State
    {
        FsrPresentation presenter;
        std::shared_ptr<FsrHostResources> resources;
        std::unique_ptr<FsrGenerationGuideAdapter> externalGuides;
        FsrEffectProvider provider{FsrEffect::FrameGeneration,{}};
        FsrGenerationLimits limits{};ColorEncoding encoding{ColorEncoding::Unknown};
        bool created{},feature{},closing{},resizing{},resizeReady{},suspended{},suspendReady{};
    };
    FsrHostPresentation::FsrHostPresentation():state_(std::make_unique<State>()){}
    FsrHostPresentation::~FsrHostPresentation(){if(!Retire())(void)state_.release();}
    Result<Extent> FsrHostPresentation::Create(IDXGIFactory* factory,ID3D11Device* device,
        std::shared_ptr<FsrHostResources> resources,const DXGI_SWAP_CHAIN_DESC& input,const FsrSettings& settings)
    {return CreateInternal(factory,device,std::move(resources),input,settings,false,{},{});}
    Result<Extent> FsrHostPresentation::CreateExternal(IDXGIFactory* factory,ID3D11Device* device,
        std::shared_ptr<FsrHostResources> resources,const DXGI_SWAP_CHAIN_DESC& input,const FsrSettings& settings,
        Extent render,FsrInputPolicy policy)
    {return CreateInternal(factory,device,std::move(resources),input,settings,true,render,policy);}
    Result<Extent> FsrHostPresentation::CreateInternal(IDXGIFactory* factory,ID3D11Device* device,
        std::shared_ptr<FsrHostResources> resources,const DXGI_SWAP_CHAIN_DESC& input,const FsrSettings& settings,
        bool external,Extent externalRender,FsrInputPolicy policy)
    {
        auto invalid=[](const char* text)->Result<Extent>{return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,text});};
        if(state_->created || state_->closing || !factory || !device || !resources)
            return invalid("FSR FG creation requires a fresh presentation owner, factory, producer device and resources");
        if((!external && !ValidFsrSettings(settings)) || !ValidProviderPolicy(settings.generationProviderPolicy))
            return invalid("FSR FG creation rejected invalid provider settings; check [FrameGeneration] FsrProvider");
        if(!IsKnownColorEncoding(settings.sourceColorEncoding)) {
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,
                std::string(external?"DLSS to FSR FG":"FSR upscaling to FSR FG")+" requires explicit SDR encoding in "+
                PublicIni::Reference("FSR","SourceColorEncoding")+" (Linear, Gamma22 or SRGB)"});
        }
        auto descriptor=FsrPresentation::TranslateDescriptor(input);if(!descriptor)return std::unexpected(descriptor.error());
        BackendConfiguration config;config.backend=BackendKind::Fsr;config.generationEnabled=false;config.generationBackend=0;
        config.quality=settings.quality;config.providerPolicy=settings.providerPolicy;config.sharpness=settings.sharpness;
        state_->resources=std::move(resources);state_->encoding=settings.sourceColorEncoding;
        const Extent output{descriptor->BufferDesc.Width,descriptor->BufferDesc.Height};
        Result<Extent> render=externalRender;
        if(external){
            auto sized=state_->resources->PrepareExternalSizing(device,externalRender,output,descriptor->BufferDesc.Format,state_->encoding,policy);
            if(!sized)return std::unexpected(sized.error());
        }else render=state_->resources->PrepareSizing(device,config,output,descriptor->BufferDesc.Format,state_->encoding);
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
            if(!catalog)return std::unexpected(catalog.error());auto selected=SelectFsrEffectProvider(*catalog,FsrEffect::FrameGeneration,settings.generationProviderPolicy);
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
        if(Suspended())return DXGI_STATUS_OCCLUDED;
        return state_->presenter.Present({},UpscaleOutcome::SkippedInvalidInput,{},nullptr,ColorEncoding::Unknown,nullptr,nullptr,false,false,false,interval,flags);
    }
    HRESULT FsrHostPresentation::WaitBeforeProducer()
    {return !state_->closing && !Suspended() && state_->created?state_->presenter.WaitBeforeProducer():E_UNEXPECTED;}
    Result<void> FsrHostPresentation::Suspend()
    {
        if(!state_->created || state_->closing || state_->resizing)return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_UNEXPECTED,"AMD suspension requires a live owner"});
        if(state_->suspended && state_->suspendReady)return {};
        state_->suspended=true;auto result=state_->presenter.Suspend();if(!result)return result;
        state_->suspendReady=true;return {};
    }
    Result<void> FsrHostPresentation::Resume()
    {
        if(!state_->suspended || !state_->suspendReady || state_->closing || state_->resizing)
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_UNEXPECTED,"AMD restoration requires completed suspension"});
        auto result=state_->presenter.Resume();if(!result)return result;
        state_->suspended=state_->suspendReady=false;return {};
    }
    Result<void> FsrHostPresentation::BeforeResize()
    {
        if(!state_->created || state_->closing)return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_UNEXPECTED,"AMD host resize requires a live owner"});
        if(state_->resizing && state_->resizeReady)return {};
        state_->resizing=true;auto retired=state_->presenter.BeforeResize();if(!retired)return retired;
        state_->feature=false;state_->resizeReady=true;state_->suspended=state_->suspendReady=false;return {};
    }
    Result<FsrHostResize> FsrHostPresentation::Resize(const DXGI_SWAP_CHAIN_DESC& input)
    {return ResizeInternal(input,{});}
    Result<FsrHostResize> FsrHostPresentation::ResizeExternal(const DXGI_SWAP_CHAIN_DESC& input,Extent render)
    {return ResizeInternal(input,render);}
    Result<FsrHostResize> FsrHostPresentation::ResizeInternal(const DXGI_SWAP_CHAIN_DESC& input,Extent externalRender)
    {
        if(!state_->resources || (state_->resources->ExternalSource() && (!externalRender.width || !externalRender.height)))
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,"External FSR FG resize requires measured render dimensions"});
        auto descriptor=FsrPresentation::TranslateDescriptor(input);if(!descriptor)return std::unexpected(descriptor.error());
        DXGI_SWAP_CHAIN_DESC previous{};auto* chain=state_->presenter.SwapChain();
        if(!chain || FAILED(chain->GetDesc(&previous)) || FAILED(ValidateFsrResizeFlags(previous.Flags,descriptor->Flags)))
            return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,"AMD immutable swapchain flags cannot change during resize"});
        auto before=BeforeResize();if(!before)return std::unexpected(before.error());
        auto retired=state_->resources->ReleaseSizedAfterRetirement();if(!retired)return std::unexpected(retired.error());
        state_->externalGuides.reset();
        const auto result=chain->ResizeBuffers(2,descriptor->BufferDesc.Width,descriptor->BufferDesc.Height,descriptor->BufferDesc.Format,descriptor->Flags);
        auto after=state_->presenter.AfterResize(result);if(!after)return std::unexpected(after.error());
        state_->resizeReady=false;
        DXGI_SWAP_CHAIN_DESC actual{};auto hr=chain->GetDesc(&actual);
        if(FAILED(hr))return std::unexpected(RuntimeError{ErrorKind::ContextFailure,hr,"AMD resized descriptor query failed"});
        const Extent output{actual.BufferDesc.Width,actual.BufferDesc.Height};
        Result<Extent> render=externalRender;
        if(state_->resources->ExternalSource()){
            if(FAILED(result))externalRender=state_->resources->RenderExtent();
            auto sized=state_->resources->ResizeExternalSizingAfterRetirement(externalRender,output,actual.BufferDesc.Format);
            if(!sized)return std::unexpected(sized.error());render=externalRender;
        }else render=state_->resources->ResizeSizingAfterRetirement(output,actual.BufferDesc.Format);
        if(!render)return std::unexpected(render.error());
        state_->limits={};state_->limits.render=*render;state_->limits.display=output;state_->limits.format=actual.BufferDesc.Format;
        state_->resizing=false;state_->resizeReady=false;return FsrHostResize{*render,result};
    }
    bool FsrHostPresentation::Suspended()const{return state_->resizing || state_->suspended;}
    HRESULT FsrHostPresentation::Present(const UpscaleFrame& frame,UpscaleOutcome outcome,ID3D11Texture2D* ui,
        ID3D11ShaderResourceView* overlay,bool complete,bool menu,bool requested,UINT interval,UINT flags)
    {
        if(flags&DXGI_PRESENT_TEST)return StartupPresent(interval,flags);
        if(!state_->created || state_->closing || Suspended())return E_UNEXPECTED;
        if(outcome==UpscaleOutcome::Temporal && state_->feature && state_->limits.input!=state_->resources->GenerationInputPolicy()) {
            const auto input=state_->resources->GenerationInputPolicy();
            if(frame.camera.depthInverted!=input.depthInverted || frame.camera.depthInfinite!=input.depthInfinite ||
                frame.motionConvention.includesJitter!=input.motionIncludesJitter)return E_INVALIDARG;
            auto changed=state_->presenter.ReconfigureInputPolicy(input,state_->provider);
            if(!changed)return E_FAIL;
            state_->limits.input=input;
        }
        if(state_->resources->ExternalSource()){
            auto ready=state_->resources->CompleteExternalStartup();if(!ready)return E_FAIL;
            if(!state_->externalGuides)state_->externalGuides=std::make_unique<FsrGenerationGuideAdapter>(
                state_->resources->Bridge(),state_->resources->Resources(),state_->resources->Depth11(),state_->resources->Motion11());
            if(outcome==UpscaleOutcome::Temporal){
                auto prepared=state_->externalGuides->Prepare(frame);
                if(!prepared){
                    if(prepared.error().kind!=ErrorKind::InvalidInput)return E_FAIL;
                    outcome=UpscaleOutcome::SkippedInvalidInput;
                }
            }
        }
        if(!state_->resources->GenerationInputsReady())return E_UNEXPECTED;
        if(outcome==UpscaleOutcome::Temporal){
            const auto input=state_->resources->GenerationInputPolicy();
            if(frame.camera.depthInverted!=input.depthInverted || frame.camera.depthInfinite!=input.depthInfinite ||
                frame.motionConvention.includesJitter!=input.motionIncludesJitter)return E_INVALIDARG;
            if(!state_->feature){state_->limits.input=input;auto started=state_->presenter.CompleteStartup(state_->limits,state_->provider);
                if(!started)return E_FAIL;state_->feature=true;}
            else if(state_->limits.input!=input)return E_INVALIDARG;
        }
        return state_->presenter.Present(frame,outcome,state_->resources->Resources(),state_->presenter.SceneTarget11(),
            state_->encoding,ui,overlay,complete,menu,requested,interval,flags);
    }
    Result<void> FsrHostPresentation::Retire()
    {
        state_->closing=true;auto retired=state_->presenter.Retire();if(!retired)return retired;
        state_->externalGuides.reset();
        if(state_->resources){retired=state_->resources->Retire();if(!retired)return retired;}
        state_->feature=false;return {};
    }
    IDXGISwapChain4* FsrHostPresentation::SwapChain()const{return state_->presenter.SwapChain();}
    ID3D11Texture2D* FsrHostPresentation::SceneTarget11()const{return state_->presenter.SceneTarget11();}
    bool FsrHostPresentation::FeatureReady()const{return state_->feature && !state_->closing && !Suspended();}
    Telemetry::OutputCounter FsrHostPresentation::OutputCounter(std::uint64_t observations)const
    {
        auto* chain=state_->presenter.SwapChain();
        return Telemetry::ReadDxgiOutputCounter(chain,reinterpret_cast<std::uintptr_t>(chain),observations,
            state_->created && !state_->closing && !state_->resizing && !Suspended());
    }
    FsrPresentationStatus FsrHostPresentation::Status()const{return state_->presenter.Status();}
    const FsrEffectProvider& FsrHostPresentation::GenerationProvider()const{return state_->provider;}
}
