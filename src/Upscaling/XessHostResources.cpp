#include "XessHostResources.h"
#include "SdrColorConversion.h"
#include "FrameGen/D3D11FrameCopy.h"
#include <dxgi1_4.h>
namespace TheosRenderPipeline::Upscaling
{
    using Microsoft::WRL::ComPtr;
    struct XessHostResources::State
    {
        ComPtr<ID3D12Device> device;ComPtr<ID3D12CommandQueue> queue;
        std::shared_ptr<XessRuntime> runtime;
        std::shared_ptr<Graphics::D3D11D3D12Interop> bridge;
        std::unique_ptr<XessUpscaler> upscaler;
        Graphics::SharedTexture color,depth,motion,output;
        SdrColorConverter decode;D3D11FrameCopy::Depth depthCopy;
        ColorEncoding encoding{ColorEncoding::Unknown};Extent render{},display{};
        bool ready{};
    };
    static std::unexpected<RuntimeError> Failure(ErrorKind kind,HRESULT native,const char* text)
    { return std::unexpected(RuntimeError{kind,native,text}); }
    XessHostResources::XessHostResources(std::filesystem::path path,DeviceCreator creator):state_(std::make_unique<State>()),pluginDirectory_(std::move(path)),deviceCreator_(creator){}
    XessHostResources::~XessHostResources(){if(!Retire())(void)state_.release();}
    Result<Extent> XessHostResources::Initialize(ID3D11Device* device11,Quality quality,Extent display,ColorEncoding encoding,bool inverted)
    {
        if(state_->device || !device11 || !pluginDirectory_.is_absolute() || !IsKnownColorEncoding(encoding))return Failure(ErrorKind::InvalidInput,E_INVALIDARG,"XeSS host initialization requires a fresh owner and explicit SDR encoding");
        auto fail=[&](RuntimeError error)->Result<Extent>{const auto retired=Retire();return std::unexpected(retired?error:retired.error());};
        ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;DXGI_ADAPTER_DESC description{};
        auto hr=device11->QueryInterface(IID_PPV_ARGS(&dxgi));
        if(FAILED(hr) || FAILED(hr=dxgi->GetAdapter(&adapter)) || FAILED(hr=adapter->GetDesc(&description)))return Failure(ErrorKind::UnsupportedDevice,hr,"XeSS actual D3D11 adapter query failed");
        hr=deviceCreator_?deviceCreator_(adapter.Get(),D3D_FEATURE_LEVEL_12_0,state_->device.ReleaseAndGetAddressOf()):
            D3D12CreateDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,IID_PPV_ARGS(&state_->device));
        if(FAILED(hr) || !state_->device)return fail({ErrorKind::UnsupportedDevice,FAILED(hr)?hr:E_NOINTERFACE,"XeSS native same-adapter D3D12 device creation failed"});
        const auto match=ValidateXessAdapter(description.AdapterLuid,state_->device->GetAdapterLuid());if(!match)return fail(match.error());
        D3D12_COMMAND_QUEUE_DESC queue{};queue.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
        if(FAILED(hr=state_->device->CreateCommandQueue(&queue,IID_PPV_ARGS(&state_->queue))))return fail({ErrorKind::UnsupportedDevice,hr,"XeSS direct queue creation failed"});
        state_->bridge=std::make_shared<Graphics::D3D11D3D12Interop>();
        if(FAILED(hr=state_->bridge->Initialize(device11,state_->device.Get(),state_->queue.Get())))return fail({ErrorKind::UnsupportedDevice,hr,"XeSS actual shared-device/fence bridge unavailable"});
        state_->runtime=std::make_shared<XessRuntime>();const auto loaded=state_->runtime->Load(pluginDirectory_);if(!loaded)return fail(loaded.error());
        state_->upscaler=std::make_unique<XessUpscaler>();
        XessInputPolicy policy{};policy.sourceEncoding=encoding;policy.depthInverted=inverted;policy.motionGuide=XessMotionGuide::Undilated;
        const auto extent=state_->upscaler->Initialize(state_->runtime,state_->device.Get(),quality,display,policy);if(!extent)return fail(extent.error());
        state_->render=*extent;state_->display=display;state_->encoding=encoding;
        // This host's explicit producer contract is unjittered current-to-previous UV motion.
        policy.motion={static_cast<float>(extent->width),static_cast<float>(extent->height),true,false};
        policy.motionExtent=policy.depthExtent=*extent;
        const auto configured=state_->upscaler->ConfigureGuides(policy);if(!configured)return fail(configured.error());
        auto create=[&](Graphics::SharedTexture& texture,Extent size,DXGI_FORMAT format,bool uav)->HRESULT {
            D3D11_TEXTURE2D_DESC desc{};desc.Width=size.width;desc.Height=size.height;desc.MipLevels=desc.ArraySize=1;desc.SampleDesc.Count=1;
            desc.Usage=D3D11_USAGE_DEFAULT;desc.Format=format;desc.BindFlags=D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE|(uav?D3D11_BIND_UNORDERED_ACCESS:0);
            return state_->bridge->CreateSharedTexture(desc,texture);
        };
        if(FAILED(hr=create(state_->color,*extent,DXGI_FORMAT_R16G16B16A16_FLOAT,false)) || FAILED(hr=create(state_->depth,*extent,DXGI_FORMAT_R32_FLOAT,true)) ||
            FAILED(hr=create(state_->motion,*extent,DXGI_FORMAT_R16G16_FLOAT,false)) || FAILED(hr=create(state_->output,display,DXGI_FORMAT_R16G16B16A16_FLOAT,true)))
            return fail({ErrorKind::UnsupportedDevice,hr,"XeSS shared prepared texture allocation failed"});
        state_->ready=true;return *extent;
    }
    Result<void> XessHostResources::PrepareInput(ID3D11Texture2D* color,ID3D11Texture2D* depth,ID3D11Texture2D* motion)
    {
        if(!state_->ready || !color || !depth || !motion || !state_->bridge->Ready())return Failure(ErrorKind::InvalidInput,E_INVALIDARG,"XeSS prepared input resources unavailable");
        for(auto* texture:{color,depth,motion}){
            D3D11_TEXTURE2D_DESC desc{};texture->GetDesc(&desc);
            if(desc.Width!=state_->render.width || desc.Height!=state_->render.height)return Failure(ErrorKind::InvalidInput,E_INVALIDARG,"XeSS source guide/colour extent mismatch");
        }
        auto* context=state_->bridge->Context11();
        D3D11ContextIsolation isolation;D3D11ContextIsolation::Scope scope(isolation,context);
        if(!scope)return Failure(ErrorKind::ContextFailure,E_FAIL,"XeSS guide-copy context isolation failed");
        // The strict ordinary bridge copy checks producer/dispatch ownership before any writes.
        auto hr=state_->bridge->CopyInput(motion,state_->motion);
        if(FAILED(hr))return Failure(ErrorKind::InvalidInput,hr,"XeSS motion copy or producer ownership admission failed");
        hr=state_->decode.Convert(context,color,state_->color.texture11.Get(),state_->encoding,ColorEncoding::Linear);
        if(FAILED(hr))return Failure(ErrorKind::InvalidInput,hr,"XeSS explicit source-to-linear colour conversion failed");
        hr=state_->depthCopy.Copy(context,depth,state_->depth.texture11.Get(),{state_->render.width,state_->render.height});
        if(FAILED(hr))return Failure(ErrorKind::InvalidInput,hr,"XeSS undilated depth/motion copy failed");
        return {};
    }
    Result<void> XessHostResources::Retire()
    {
        if(state_->upscaler){const auto retired=state_->upscaler->DestroyAfterRetirement();if(!retired)return retired;}
        if(state_->bridge){const auto hr=state_->bridge->Drain();if(FAILED(hr))return Failure(ErrorKind::RetirementFailure,hr,"XeSS producer/consumer drain failed; retain resources");}
        *state_=State{};return {};
    }
    Extent XessHostResources::RenderExtent()const { return state_->render; }
    XessGpuResources XessHostResources::Resources()const { return {state_->color.texture12.Get(),state_->depth.texture12.Get(),state_->motion.texture12.Get(),state_->output.texture12.Get(),state_->bridge}; }
    std::shared_ptr<Graphics::D3D11D3D12Interop> XessHostResources::Bridge()const { return state_->bridge; }
    XessUpscaler* XessHostResources::Upscaler()const { return state_->upscaler.get(); }
    ID3D11Texture2D* XessHostResources::Output11()const { return state_->output.texture11.Get(); }
}
