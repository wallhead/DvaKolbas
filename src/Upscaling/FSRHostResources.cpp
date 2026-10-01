#include "FSRHostResources.h"
#include "FSRProviderPolicy.h"
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
    };
    static std::unexpected<RuntimeError> Failure(ErrorKind kind,HRESULT code,const char* text){return std::unexpected(RuntimeError{kind,code,text});}
    FsrHostResources::FsrHostResources(std::filesystem::path plugin):state_(std::make_unique<State>()),pluginDirectory_(std::move(plugin)){}
    FsrHostResources::~FsrHostResources(){if(!Retire())(void)state_.release();}
    bool FsrHostResources::FeatureReady()const{return state_->contextOwned && state_->bridge && state_->bridge->Ready();}
    bool FsrHostResources::ContextOwned()const{return state_->contextOwned;}
    std::shared_ptr<Graphics::D3D11D3D12Interop> FsrHostResources::Bridge()const{return state_->bridge;}
    FsrUpscaler* FsrHostResources::Upscaler()const{return state_->upscaler.get();}
    GpuFrameResources FsrHostResources::Resources()const{return {state_->color.texture12.Get(),state_->depth.texture12.Get(),state_->motion.texture12.Get(),state_->native.texture12.Get()};}
    ID3D11Texture2D* FsrHostResources::Color11()const{return state_->color.texture11.Get();}ID3D11Texture2D* FsrHostResources::Depth11()const{return state_->depth.texture11.Get();}
    ID3D11Texture2D* FsrHostResources::Motion11()const{return state_->motion.texture11.Get();}ID3D11Texture2D* FsrHostResources::Output11()const{return state_->native.texture11.Get();}
    const ProviderInfo& FsrHostResources::Provider()const{return state_->provider;}
    Result<Extent> FsrHostResources::PrepareSizing(ID3D11Device* device11,const BackendConfiguration& config,Extent output)
    {
        auto backend=TheosRenderPipeline::ResolveBackend(config,true);
        if(!device11 || !pluginDirectory_.is_absolute() || !backend.valid || config.backend!=BackendKind::Fsr || state_->bridge)
            return Failure(ErrorKind::InvalidInput,0,"FSR sizing requires a fresh owner, valid FSR configuration and absolute plugin directory");
        auto fail=[&](HRESULT hr,const char* text)->Result<Extent>{return Failure(ErrorKind::UnsupportedDevice,hr,text);};
        ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;HRESULT hr=device11->QueryInterface(IID_PPV_ARGS(&dxgi));
        if(FAILED(hr) || FAILED(hr=dxgi->GetAdapter(&adapter)))return fail(hr,"FSR actual adapter query failed");
        if(FAILED(hr=D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&state_->device))))return fail(hr,"FSR same-adapter D3D12 creation failed");
        D3D12_FEATURE_DATA_SHADER_MODEL model{D3D_SHADER_MODEL_6_0};
        if(FAILED(hr=state_->device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL,&model,sizeof(model))) || model.HighestShaderModel<D3D_SHADER_MODEL_6_0)
            return fail(hr,"FSR requires shader model 6.0 before reduced target publication");
        D3D12_COMMAND_QUEUE_DESC queue{};queue.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
        if(FAILED(hr=state_->device->CreateCommandQueue(&queue,IID_PPV_ARGS(&state_->queue))))return fail(hr,"FSR direct queue creation failed");
        state_->bridge=std::make_shared<Graphics::D3D11D3D12Interop>();
        if(FAILED(hr=state_->bridge->Initialize(device11,state_->device.Get(),state_->queue.Get())))return fail(hr,"FSR shared texture/fence interfaces unsupported");
        // Probe actual sharing/UAV support before committing reduced game buffers.
        D3D11_TEXTURE2D_DESC desc{};desc.Width=desc.Height=4;desc.MipLevels=desc.ArraySize=1;desc.SampleDesc.Count=1;
        desc.Usage=D3D11_USAGE_DEFAULT;desc.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS;
        for(auto format:{DXGI_FORMAT_R16G16B16A16_FLOAT,DXGI_FORMAT_R32_FLOAT,DXGI_FORMAT_R16G16_FLOAT}) {
            desc.Format=format;Graphics::SharedTexture probe;
            if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,probe)))return fail(hr,"FSR prepared format cannot share between the actual devices");
            D3D12_FEATURE_DATA_FORMAT_SUPPORT support{format};
            if(FAILED(hr=state_->device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT,&support,sizeof(support))) ||
                !(support.Support1&D3D12_FORMAT_SUPPORT1_SHADER_LOAD) || !(support.Support2&D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE))return fail(hr,"FSR prepared format lacks shader/UAV capability");
        }
        state_->runtime=std::make_shared<FsrRuntime>();auto loaded=state_->runtime->Load(pluginDirectory_);if(!loaded)return std::unexpected(loaded.error());
        auto providers=state_->runtime->Enumerate(state_->device.Get());if(!providers)return std::unexpected(providers.error());
        auto provider=SelectProvider(*providers,config.providerPolicy);if(!provider)return std::unexpected(provider.error());state_->provider=*provider;
        auto render=state_->runtime->QueryRenderExtent(state_->device.Get(),*provider,config.quality,output);if(!render)return std::unexpected(render.error());
        state_->render=*render;state_->output=output;state_->config=config;return *render;
    }
    Result<void> FsrHostResources::CompleteStartup()
    {
        if(FeatureReady())return {};
        if(!state_->runtime || !state_->bridge || !state_->bridge->Ready() || !state_->render.width)
            return Failure(ErrorKind::ContextFailure,0,"FSR deferred startup has no valid pre-query sizing");
        if(state_->contextOwned || state_->upscaler)return Failure(ErrorKind::ContextFailure,0,"FSR partial startup retained; retirement required before retry");
        D3D11_TEXTURE2D_DESC desc{};desc.Width=state_->render.width;desc.Height=state_->render.height;desc.MipLevels=desc.ArraySize=1;desc.SampleDesc.Count=1;
        desc.Usage=D3D11_USAGE_DEFAULT;desc.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS;
        HRESULT hr{};desc.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;
        if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,state_->color)))return Failure(ErrorKind::UnsupportedDevice,hr,"FSR shared color allocation failed");
        desc.Format=DXGI_FORMAT_R32_FLOAT;if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,state_->depth)))return Failure(ErrorKind::UnsupportedDevice,hr,"FSR shared depth allocation failed");
        desc.Format=DXGI_FORMAT_R16G16_FLOAT;if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,state_->motion)))return Failure(ErrorKind::UnsupportedDevice,hr,"FSR shared motion allocation failed");
        desc.Width=state_->output.width;desc.Height=state_->output.height;desc.Format=DXGI_FORMAT_R16G16B16A16_FLOAT;
        if(FAILED(hr=state_->bridge->CreateSharedTexture(desc,state_->native)))return Failure(ErrorKind::UnsupportedDevice,hr,"FSR native output allocation failed");
        state_->upscaler=std::make_unique<FsrUpscaler>();auto attached=state_->upscaler->SetRetirementBridge(state_->bridge);if(!attached)return attached;
        auto created=state_->upscaler->Initialize(state_->runtime,state_->device.Get(),state_->provider,state_->config.quality,state_->render,state_->output);
        if(!created)return created;state_->contextOwned=true;return {};
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
    Result<void> FsrHostResources::Retire()
    {
        if(state_->bridge) {
            if(!state_->bridge->Ready())return Failure(ErrorKind::RetirementFailure,state_->bridge->Fault(),"FSR bridge fault; preserve owned resources");
            auto hr=state_->bridge->SignalD3D11(Graphics::InteropWork::SwapChain);
            if(FAILED(hr) || FAILED(hr=state_->bridge->Drain()))return Failure(ErrorKind::RetirementFailure,hr,"FSR final scene/UI readers not retired; preserve ownership");
        }
        if(state_->upscaler){auto destroyed=state_->upscaler->DestroyAfterRetirement();if(!destroyed)return destroyed;}
        state_=std::make_unique<State>();return {};
    }
}
