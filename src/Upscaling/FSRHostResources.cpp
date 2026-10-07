#include "FSRHostResources.h"
#include "FSRProviderPolicy.h"
#include "FSRCreationFallback.h"
#include "FSRColorContract.h"
#include "FSRPreparedResources.h"
#include "RendererBackendPolicy.h"
#include <dxgi1_4.h>
namespace TheosRenderPipeline::Upscaling
{
    using Microsoft::WRL::ComPtr;
    struct FsrHostResources::State
    {
        std::shared_ptr<FsrRuntime> runtime;std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;
        ComPtr<ID3D12Device> device;ComPtr<ID3D12CommandQueue> queue;
        std::unique_ptr<FsrUpscaler> upscaler;ProviderInfo provider;BackendConfiguration config;Extent render{},output{};
        Graphics::SharedTexture color,depth,motion,native;bool contextOwned{};
        ColorEncoding handoffEncoding{ColorEncoding::Unknown};
        std::string providerDiagnostic;
        bool firstStartupCompleted{},external{},externalReady{};
        uint32_t vendor{};
        FsrInputPolicy externalInput{};
        FsrMlAvailability mlAvailability;
        bool mlShaderSupport{};
        std::string mlShaderDiagnostic;
    };
    static std::unexpected<RuntimeError> Failure(ErrorKind kind,HRESULT code,const char* text){return std::unexpected(RuntimeError{kind,code,text});}
    FsrHostResources::FsrHostResources(std::filesystem::path plugin,DeviceCreator creator):state_(std::make_unique<State>()),pluginDirectory_(std::move(plugin)),deviceCreator_(creator){}
    FsrHostResources::~FsrHostResources(){if(!Retire())(void)state_.release();}
    bool FsrHostResources::FeatureReady()const{return state_->contextOwned && state_->bridge && state_->bridge->Ready();}
    bool FsrHostResources::ContextOwned()const{return state_->contextOwned;}
    std::shared_ptr<Graphics::D3D11D3D12Interop> FsrHostResources::Bridge()const{return state_->bridge;}
    std::shared_ptr<FsrRuntime> FsrHostResources::Runtime()const{return state_->runtime;}
#if defined(TRP_ENABLE_FSR_FG)
    Result<void> FsrHostResources::LoadFrameGeneration()
    {
        if(!state_->runtime)return Failure(ErrorKind::ContextFailure,0,"FG runtime requires completed device and resource sizing");
        auto loaded=state_->external?Result<void>{}:state_->runtime->LoadFrameGeneration(pluginDirectory_);
        if(!loaded)return loaded;
        auto catalog=state_->runtime->EnumerateForEffect(state_->device.Get(),FsrEffect::FrameGeneration);
        if(!catalog) {
            state_->mlAvailability.generation.reset();
            state_->mlAvailability.generationReason="ML FG catalog could not be checked: "+catalog.error().message;
            return {};
        }
        auto selected=SelectFsrEffectProvider(*catalog,FsrEffect::FrameGeneration,ProviderPolicy::MachineLearning);
        state_->mlAvailability.generation=bool(selected) && state_->mlShaderSupport;
        state_->mlAvailability.generationReason=!selected?"ML FG is unavailable in this device's qualified FG catalog; use FSR3 FG or Auto.":
            !state_->mlShaderSupport?state_->mlShaderDiagnostic:
            "ML FG provider is present; real AMD rendering remains experimental.";
        return {};
    }
#endif
    FsrUpscaler* FsrHostResources::Upscaler()const{return state_->upscaler.get();}
    GpuFrameResources FsrHostResources::Resources()const{return {state_->color.texture12.Get(),state_->depth.texture12.Get(),state_->motion.texture12.Get(),state_->native.texture12.Get()};}
    ID3D11Texture2D* FsrHostResources::Color11()const{return state_->color.texture11.Get();}ID3D11Texture2D* FsrHostResources::Depth11()const{return state_->depth.texture11.Get();}
    ID3D11Texture2D* FsrHostResources::Motion11()const{return state_->motion.texture11.Get();}ID3D11Texture2D* FsrHostResources::Output11()const{return state_->native.texture11.Get();}
    const ProviderInfo& FsrHostResources::Provider()const{return state_->provider;}
    const std::string& FsrHostResources::ProviderDiagnostic()const{return state_->providerDiagnostic;}
    const FsrMlAvailability& FsrHostResources::MlAvailability()const{return state_->mlAvailability;}
    ColorEncoding FsrHostResources::HandoffEncoding()const{return state_->handoffEncoding;}
    bool FsrHostResources::ExternalSource()const{return state_->external;}
    bool FsrHostResources::GenerationInputsReady()const
    {return state_->external?state_->externalReady && state_->bridge && state_->bridge->Ready():FeatureReady();}
    FsrInputPolicy FsrHostResources::GenerationInputPolicy()const
    {return state_->external?state_->externalInput:state_->upscaler?state_->upscaler->Limits().input:FsrInputPolicy{};}
    Extent FsrHostResources::RenderExtent()const{return state_->render;}
    Result<void> FsrHostResources::PrepareExternalSizing(ID3D11Device* device11,Extent render,Extent display,
        DXGI_FORMAT format,ColorEncoding encoding,FsrInputPolicy policy)
    {
        if(!device11 || !pluginDirectory_.is_absolute() || state_->bridge || !render.width || !render.height ||
            !display.width || !display.height || render.width>display.width || render.height>display.height)
            return Failure(ErrorKind::InvalidInput,E_INVALIDARG,"External FSR FG requires a fresh owner and fixed measured render/display extents");
        auto handoff=ValidateFsrHandoff(format,encoding);if(!handoff)return std::unexpected(handoff.error());
        auto initialized=InitializeDevice(device11);if(!initialized)return initialized;
        state_->runtime=std::make_shared<FsrRuntime>();
        auto loaded=state_->runtime->LoadGenerationOnly(pluginDirectory_);if(!loaded)return loaded;
        state_->external=true;state_->externalInput=policy;state_->render=render;state_->output=display;
        state_->handoffEncoding=encoding;return {};
    }
    Result<void> FsrHostResources::CompleteExternalStartup()
    {
        if(!state_->external || !state_->runtime || !state_->bridge || !state_->bridge->Ready())
            return Failure(ErrorKind::ContextFailure,E_UNEXPECTED,"External FSR FG requires generation-only device/runtime ownership");
        if(state_->externalReady)return {};
        if(state_->depth.texture11 || state_->motion.texture11)
            return Failure(ErrorKind::ContextFailure,E_UNEXPECTED,"Partial external guide allocation retained; retire before retry");
        auto desc=FsrPreparedTextureDesc(FsrResourceRole::Depth,state_->render);
        auto hr=state_->bridge->CreateSharedTexture(desc,state_->depth);
        if(FAILED(hr))return Failure(ErrorKind::UnsupportedDevice,hr,"External FSR FG shared depth allocation failed");
        desc=FsrPreparedTextureDesc(FsrResourceRole::Motion,state_->render);
        hr=state_->bridge->CreateSharedTexture(desc,state_->motion);
        if(FAILED(hr))return Failure(ErrorKind::UnsupportedDevice,hr,"External FSR FG shared motion allocation failed");
        state_->externalReady=true;return {};
    }
    Result<void> FsrHostResources::ResizeExternalSizingAfterRetirement(Extent render,Extent display,DXGI_FORMAT format)
    {
        if(!state_->external || !state_->runtime || !state_->bridge || !state_->bridge->Ready() ||
            state_->externalReady || state_->depth.texture11 || state_->motion.texture11 ||
            !render.width || !render.height || !display.width || !display.height || render.width>display.width || render.height>display.height)
            return Failure(ErrorKind::InvalidInput,E_UNEXPECTED,"External FSR FG resize requires retired guides and fixed measured extents");
        auto handoff=ValidateFsrHandoff(format,state_->handoffEncoding);if(!handoff)return std::unexpected(handoff.error());
        state_->render=render;state_->output=display;return {};
    }
    Result<void> FsrHostResources::InitializeDevice(ID3D11Device* device11)
    {
        auto fail=[&](HRESULT hr,const char* text)->Result<void>{return Failure(ErrorKind::UnsupportedDevice,hr,text);};
        ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;HRESULT hr=device11->QueryInterface(IID_PPV_ARGS(&dxgi));
        if(FAILED(hr) || FAILED(hr=dxgi->GetAdapter(&adapter)))return fail(hr,"FSR actual adapter query failed");
        hr=deviceCreator_?deviceCreator_(adapter.Get(),D3D_FEATURE_LEVEL_12_0,state_->device.ReleaseAndGetAddressOf()):
            D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&state_->device));
        if(FAILED(hr) || !state_->device)return fail(FAILED(hr)?hr:E_NOINTERFACE,"FSR same-adapter native D3D12 ownership unavailable");
        D3D12_FEATURE_DATA_SHADER_MODEL model{D3D_SHADER_MODEL_6_2};
        if(FAILED(hr=state_->device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&model,sizeof(model))) || model.HighestShaderModel<D3D_SHADER_MODEL_6_2)
            return std::unexpected(RuntimeError{ErrorKind::UnsupportedDevice,hr,
                FsrShaderModelFailure(D3D_SHADER_MODEL_6_2,model.HighestShaderModel,hr)+" before reduced target publication"});
        D3D12_COMMAND_QUEUE_DESC queue{};queue.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
        if(FAILED(hr=state_->device->CreateCommandQueue(&queue,IID_PPV_ARGS(&state_->queue))))return fail(hr,"FSR direct queue creation failed");
        state_->bridge=std::make_shared<Graphics::D3D11D3D12Interop>();
        if(FAILED(hr=state_->bridge->Initialize(device11,state_->device.Get(),state_->queue.Get())))return fail(hr,"FSR shared texture/fence interfaces unsupported");
        // Probe actual sharing/UAV support before committing reduced game buffers.
        for(auto role:{FsrResourceRole::Color,FsrResourceRole::Depth,FsrResourceRole::Motion,FsrResourceRole::Output}) {
            const auto desc=FsrPreparedTextureDesc(role,{4,4});Graphics::SharedTexture probe;
            if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,probe)))return fail(hr,"FSR prepared format cannot share between the actual devices");
            D3D12_FEATURE_DATA_FORMAT_SUPPORT support{desc.Format};
            if(FAILED(hr=state_->device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT,&support,sizeof(support))) ||
                !FsrPreparedFormatSupported(role,support))return fail(hr,"FSR prepared format lacks its required shader/UAV capability");
        }
        DXGI_ADAPTER_DESC description{};
        if(FAILED(hr=adapter->GetDesc(&description)))return fail(hr,"FSR actual adapter description unavailable");
        state_->vendor=description.VendorId;
        D3D12_FEATURE_DATA_SHADER_MODEL mlModel{D3D_SHADER_MODEL_6_6};
        const auto mlHr=state_->device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&mlModel,sizeof(mlModel));
        state_->mlShaderSupport=SUCCEEDED(mlHr) && mlModel.HighestShaderModel>=D3D_SHADER_MODEL_6_6;
        state_->mlShaderDiagnostic=state_->mlShaderSupport?std::string{}:FsrShaderModelFailure(D3D_SHADER_MODEL_6_6,mlModel.HighestShaderModel,mlHr);
        return {};
    }
    Result<Extent> FsrHostResources::PrepareSizing(ID3D11Device* device11,const BackendConfiguration& config,Extent output,DXGI_FORMAT handoffFormat,ColorEncoding handoffEncoding)
    {
        auto backend=TheosRenderPipeline::ResolveBackend(config,true);
        if(!device11 || !pluginDirectory_.is_absolute() || !backend.valid || config.backend!=BackendKind::Fsr || state_->bridge)
            return Failure(ErrorKind::InvalidInput,0,"FSR sizing requires a fresh owner, valid FSR configuration and absolute plugin directory");
        const auto handoff=ValidateFsrHandoff(handoffFormat,handoffEncoding);
        if(!handoff)return std::unexpected(handoff.error());
        auto initialized=InitializeDevice(device11);if(!initialized)return std::unexpected(initialized.error());
        const auto profile=SelectFsrRuntimeProfile(config.providerPolicy,state_->vendor);
        state_->runtime=std::make_shared<FsrRuntime>();auto loaded=state_->runtime->Load(pluginDirectory_,profile);if(!loaded)return std::unexpected(loaded.error());
        auto providers=state_->runtime->Enumerate(state_->device.Get());if(!providers)return std::unexpected(providers.error());
        if(!state_->mlShaderSupport) {
            state_->mlAvailability.upscale=false;
            state_->mlAvailability.upscaleReason=state_->mlShaderDiagnostic;
        } else if(state_->vendor==0x10de && profile!=FsrRuntimeProfile::Int8) {
            auto files=FsrRuntime::CheckInt8Files(pluginDirectory_);
            state_->mlAvailability.upscale=bool(files);
            state_->mlAvailability.upscaleReason=files?"INT8 files verified and SM6.6 available; startup still validates the actual provider/context.":files.error().message;
        } else {
            auto ml=SelectProvider(*providers,ProviderPolicy::MachineLearning,state_->vendor,profile);
            state_->mlAvailability.upscale=bool(ml);
            state_->mlAvailability.upscaleReason=ml?"ML SR provider is present; startup still validates context creation.":
                "ML SR is unavailable in this device's qualified SR catalog; use FSR3 or Auto.";
        }
        auto provider=SelectProvider(*providers,config.providerPolicy,state_->vendor,profile);if(!provider)return std::unexpected(provider.error());
        if(IsFsr4Provider(*provider)) {
            if(!state_->mlShaderSupport) {
                if(config.providerPolicy!=ProviderPolicy::Compatible)return std::unexpected(RuntimeError{ErrorKind::UnsupportedDevice,0,state_->mlShaderDiagnostic+" before reduced target publication"});
                provider=SelectProvider(*providers,ProviderPolicy::Analytical,state_->vendor);
                if(!provider)return std::unexpected(provider.error());
                state_->providerDiagnostic="Auto selected FSR3: shader model 6.6 unavailable";
            }
        }
        state_->provider=*provider;
        if(config.providerPolicy==ProviderPolicy::Compatible && !IsFsr4Provider(*provider) && state_->providerDiagnostic.empty())
            state_->providerDiagnostic="Auto selected FSR3: FSR4 unavailable on this adapter";
        auto render=state_->runtime->QueryRenderExtent(state_->device.Get(),*provider,config.quality,output);if(!render)return std::unexpected(render.error());
        state_->render=*render;state_->output=output;state_->config=config;state_->handoffEncoding=handoffEncoding;return *render;
    }
    Result<void> FsrHostResources::CompleteStartup()
    {
        if(state_->external)return Failure(ErrorKind::InvalidInput,E_UNEXPECTED,"External FG resources cannot initialize an FSR upscaler");
        if(FeatureReady())return {};
        if(!state_->runtime || !state_->bridge || !state_->bridge->Ready() || !state_->render.width)
            return Failure(ErrorKind::ContextFailure,0,"FSR deferred startup has no valid pre-query sizing");
        if(state_->contextOwned || state_->upscaler)return Failure(ErrorKind::ContextFailure,0,"FSR partial startup retained; retirement required before retry");
        HRESULT hr{};auto desc=FsrPreparedTextureDesc(FsrResourceRole::Color,state_->render);
        if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,state_->color)))return Failure(ErrorKind::UnsupportedDevice,hr,"FSR shared color allocation failed");
        desc=FsrPreparedTextureDesc(FsrResourceRole::Depth,state_->render);if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,state_->depth)))return Failure(ErrorKind::UnsupportedDevice,hr,"FSR shared depth allocation failed");
        desc=FsrPreparedTextureDesc(FsrResourceRole::Motion,state_->render);if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,state_->motion)))return Failure(ErrorKind::UnsupportedDevice,hr,"FSR shared motion allocation failed");
        desc=FsrPreparedTextureDesc(FsrResourceRole::Output,state_->output);
        if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,state_->native)))return Failure(ErrorKind::UnsupportedDevice,hr,"FSR native output allocation failed");
        state_->upscaler=std::make_unique<FsrUpscaler>();auto attached=state_->upscaler->SetRetirementBridge(state_->bridge);if(!attached)return attached;
        const auto selected=state_->provider;
        std::string initialError;
        auto created=CreateFsrWithStartupFallback(selected,
            state_->firstStartupCompleted || state_->runtime->Profile()==FsrRuntimeProfile::Int8?
                ProviderPolicy::Analytical:state_->config.providerPolicy,state_->render,
            [&](const ProviderInfo& provider) {
                auto result=state_->upscaler->Initialize(state_->runtime,state_->device.Get(),provider,state_->config.quality,state_->render,state_->output);
                if(!result && initialError.empty())initialError=result.error().message;
                return result;
            },
            [&]{return state_->upscaler->DestroyAfterRetirement();},
            [&]()->Result<std::pair<ProviderInfo,Extent>> {
                auto providers=state_->runtime->Enumerate(state_->device.Get());
                if(!providers)return std::unexpected(providers.error());
                auto provider=SelectProvider(*providers,ProviderPolicy::Analytical);
                if(!provider)return std::unexpected(provider.error());
                auto extent=state_->runtime->QueryRenderExtent(state_->device.Get(),*provider,state_->config.quality,state_->output);
                if(!extent)return std::unexpected(extent.error());
                return std::pair{*provider,*extent};
            },&state_->mlAvailability);
        if(!created)return std::unexpected(created.error());
        state_->provider=*created;
        if(created->id!=selected.id)state_->providerDiagnostic="Auto selected FSR3 after FSR4 startup failed: "+initialError;
        state_->contextOwned=true;state_->firstStartupCompleted=true;return {};
    }
    Result<void> FsrHostResources::EnsureInputPolicy(FsrInputPolicy policy)
    {
        if(!FeatureReady())return Failure(ErrorKind::ContextFailure,0,"FSR context unavailable for measured input conventions");
        if(state_->upscaler->Limits().input==policy)return {};
        // Flags are immutable per context. Retire the actual output readers,
        // preserve fixed allocations/provider/quality, then recreate the feature.
        auto hr=state_->bridge->SignalD3D11(Graphics::InteropWork::SwapChain);
        if(FAILED(hr) || FAILED(hr=state_->bridge->Drain()))return Failure(ErrorKind::RetirementFailure,hr,"FSR measured convention change could not retire readers");
        auto destroyed=state_->upscaler->DestroyAfterRetirement();if(!destroyed)return destroyed;
        state_->contextOwned=false;
        auto configured=state_->upscaler->SetInputPolicy(policy);if(!configured)return configured;
        auto created=state_->upscaler->Initialize(state_->runtime,state_->device.Get(),state_->provider,state_->config.quality,state_->render,state_->output);
        if(!created)return created;state_->contextOwned=true;return {};
    }
    Result<void> FsrHostResources::ReleaseSizedAfterRetirement()
    {
        if(state_->bridge) {
            if(!state_->bridge->Ready())return Failure(ErrorKind::RetirementFailure,state_->bridge->Fault(),"FSR bridge fault; preserve owned resources");
            auto hr=state_->bridge->SignalD3D11(Graphics::InteropWork::SwapChain);
            if(FAILED(hr) || FAILED(hr=state_->bridge->Drain()))return Failure(ErrorKind::RetirementFailure,hr,"FSR final scene/UI readers not retired; preserve ownership");
        }
        if(state_->upscaler){auto destroyed=state_->upscaler->DestroyAfterRetirement();if(!destroyed)return destroyed;}
        state_->upscaler.reset();state_->contextOwned=false;state_->externalReady=false;
        state_->color={};state_->depth={};state_->motion={};state_->native={};return {};
    }
    Result<Extent> FsrHostResources::ResizeSizingAfterRetirement(Extent output,DXGI_FORMAT format)
    {
        if(state_->external || !state_->runtime || !state_->bridge || !state_->bridge->Ready() || state_->upscaler || state_->contextOwned)
            return Failure(ErrorKind::InvalidInput,E_UNEXPECTED,"FSR resize sizing requires retired fixed-size features and retained device/runtime");
        auto handoff=ValidateFsrHandoff(format,state_->handoffEncoding);if(!handoff)return std::unexpected(handoff.error());
        auto render=state_->runtime->QueryRenderExtent(state_->device.Get(),state_->provider,state_->config.quality,output);
        if(!render)return std::unexpected(render.error());
        state_->render=*render;state_->output=output;return *render;
    }
    Result<void> FsrHostResources::Retire()
    {
        auto retired=ReleaseSizedAfterRetirement();if(!retired)return retired;
        state_=std::make_unique<State>();return {};
    }
}
