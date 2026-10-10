#include "XessGenerationTransport.h"
#include "Upscaling/SdrColorConversion.h"
#include "Upscaling/FSRPresentationColor.h"
#include "FrameGen/D3D11ContextIsolation.h"
#include <array>
namespace TheosRenderPipeline
{
    using namespace Upscaling;
    using namespace Graphics;
    using Microsoft::WRL::ComPtr;
    namespace
    {
        Result<void> Invalid(const char* message)
        { return std::unexpected(RuntimeError{ErrorKind::InvalidInput,E_INVALIDARG,message}); }
        bool Fits(ID3D11Texture2D* texture,Extent extent,ID3D11Device* device,bool exact)
        {
            if (!texture) return false;
            D3D11_TEXTURE2D_DESC desc{};texture->GetDesc(&desc);
            ComPtr<ID3D11Device> owner;texture->GetDevice(&owner);
            return D3D11FrameCopy::SameObject(owner.Get(),device) && desc.MipLevels==1 && desc.ArraySize==1 &&
                desc.SampleDesc.Count==1 && !desc.SampleDesc.Quality &&
                (exact?desc.Width==extent.width && desc.Height==extent.height:desc.Width>=extent.width && desc.Height>=extent.height);
        }
        D3D11_TEXTURE2D_DESC Description(Extent extent,DXGI_FORMAT format,UINT bind)
        {
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width=extent.width;desc.Height=extent.height;desc.MipLevels=desc.ArraySize=1;
            // The established shared-open route requires RTV capability even
            // for copy-only motion and UAV-written depth (FSRPreparedResources).
            desc.SampleDesc.Count=1;desc.Format=format;desc.Usage=D3D11_USAGE_DEFAULT;desc.BindFlags=bind|D3D11_BIND_RENDER_TARGET;
            return desc;
        }
        void Transition(ID3D12GraphicsCommandList* list,const std::array<ID3D12Resource*,4>& resources,bool toCopy)
        {
            std::array<D3D12_RESOURCE_BARRIER,4> barriers{};
            for (std::size_t i=0;i<resources.size();++i) {
                auto& barrier=barriers[i];barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Transition.pResource=resources[i];barrier.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrier.Transition.StateBefore=toCopy?D3D12_RESOURCE_STATE_COMMON:D3D12_RESOURCE_STATE_COPY_SOURCE;
                barrier.Transition.StateAfter=toCopy?D3D12_RESOURCE_STATE_COPY_SOURCE:D3D12_RESOURCE_STATE_COMMON;
            }
            list->ResourceBarrier(static_cast<UINT>(barriers.size()),barriers.data());
        }
    }
    struct XessGenerationTransport::State
    {
        std::shared_ptr<D3D11D3D12Interop> bridge;
        Extent display{},guides{};
        SharedTexture scene,ui,finalImage,depth,motion;
        SdrColorConverter sceneConverter;
        FsrPresentationUiConverter uiConverter,finalComposer;
        ComPtr<ID3D11ShaderResourceView> uiView;
        D3D11FrameCopy::Depth depthCopy;
        D3D11ContextIsolation isolation;
        ComPtr<ID3D12GraphicsCommandList> recording;
        std::uint64_t source{},epoch{};
        bool waited{},uploaded{},tagged{},published{},guidesValid{},closing{};
        HRESULT fault{S_OK};
        Result<void> Check(HRESULT hr,const char* message)
        {
            if (SUCCEEDED(hr)) return {};
            if (SUCCEEDED(fault)) fault=hr;
            const bool lost=hr==DXGI_ERROR_DEVICE_REMOVED || hr==DXGI_ERROR_DEVICE_RESET || hr==DXGI_ERROR_DEVICE_HUNG ||
                (bridge && FAILED(bridge->Device12()->GetDeviceRemovedReason()));
            return std::unexpected(RuntimeError{lost?ErrorKind::DeviceLost:ErrorKind::RetirementFailure,hr,message});
        }
        Result<void> Ready() const
        {
            if (!bridge || !bridge->Ready() || closing || FAILED(fault))
                return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,FAILED(fault)?fault:E_UNEXPECTED,"Intel transport is unavailable; retaining copy owners"});
            return {};
        }
    };
    XessGenerationTransport::XessGenerationTransport()=default;
    XessGenerationTransport::~XessGenerationTransport()
    { if (state_ && !Retire()) (void)state_.release(); }
    Result<void> XessGenerationTransport::Initialize(std::shared_ptr<D3D11D3D12Interop> bridge,Extent display)
    {
        if (state_ || !bridge || !bridge->Ready() || !display.width || !display.height ||
            display.width>D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION || display.height>D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION)
            return Invalid("Intel transport requires ready same-adapter interop and a bounded fixed display extent");
        auto state=std::make_unique<State>();state->bridge=std::move(bridge);state->display=display;
        const auto desc=Description(display,DXGI_FORMAT_R8G8B8A8_UNORM,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET);
        if (auto result=state->Check(state->bridge->CreateSharedTexture(desc,state->scene),"Intel owned scene allocation failed"); !result) return result;
        if (auto result=state->Check(state->bridge->CreateSharedTexture(desc,state->ui),"Intel owned HUD allocation failed"); !result) return result;
        if (auto result=state->Check(state->bridge->CreateSharedTexture(desc,state->finalImage),"Intel composed real-image allocation failed"); !result) return result;
        ComPtr<ID3D11Device> device;state->bridge->Context11()->GetDevice(&device);
        if (auto result=state->Check(device->CreateShaderResourceView(state->ui.texture11.Get(),nullptr,&state->uiView),"Intel HUD composition view failed"); !result) return result;
        state_=std::move(state);return {};
    }
    Result<void> XessGenerationTransport::WaitBeforeProducer()
    {
        if (!state_) return Invalid("Intel transport is not initialized");
        if (auto ready=state_->Ready(); !ready) return ready;
        if (state_->recording) return Invalid("Intel tagging list must be submitted or retired before producer reuse");
        for (const auto work:{InteropWork::FrameGeneration,InteropWork::SwapChain})
            if (auto result=state_->Check(state_->bridge->WaitD3D11(work),"Intel copy/tag fence did not retire before producer writes"); !result) return result;
        state_->waited=true;return {};
    }
    Result<void> XessGenerationTransport::Upload(const UpscaleFrame& frame,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool complete)
    { return UploadInternal(frame,ui,overlay,complete,false); }
    Result<void> XessGenerationTransport::UploadReal(const UpscaleFrame& frame,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool complete)
    {
        auto real=frame;real.sourceId=real.sourceEpoch=0;real.depth=real.motion=nullptr;
        return UploadInternal(real,ui,overlay,complete,true);
    }
    Result<void> XessGenerationTransport::UploadInternal(const UpscaleFrame& frame,ID3D11Texture2D* ui,ID3D11ShaderResourceView* overlay,bool complete,bool realOnly)
    {
        if (!state_) return Invalid("Intel transport is not initialized");
        if (auto ready=state_->Ready(); !ready) return ready;
        if (!state_->waited || !complete || (!realOnly && (!frame.sourceId || !frame.sourceEpoch)) || frame.display!=state_->display ||
            !IsKnownColorEncoding(frame.outputEncoding) || !IsKnownColorEncoding(frame.uiEncoding) ||
            (!realOnly && state_->uploaded && state_->epoch==frame.sourceEpoch && frame.sourceId<=state_->source))
            return Invalid("Intel upload requires a waited fresh completed scene and HUD with explicit output/UI transfers");
        auto* context=state_->bridge->Context11();ComPtr<ID3D11Device> device;context->GetDevice(&device);
        if (!Fits(frame.output,state_->display,device.Get(),true) || !Fits(ui,state_->display,device.Get(),true))
            return Invalid("Intel scene/HUD must match the output extent and rendering D3D11 device");
        const bool guides=frame.depth && frame.motion;
        if (guides && (frame.depthExtent!=frame.motionExtent || !frame.depthExtent.width || !frame.depthExtent.height ||
            (frame.depthExtent!=frame.render && frame.depthExtent!=frame.display) ||
            !Fits(frame.depth,frame.depthExtent,device.Get(),false) || !Fits(frame.motion,frame.motionExtent,device.Get(),false)))
            return Invalid("Intel measured guide regions do not fit the actual producer resources");
        if (guides) {
            D3D11_TEXTURE2D_DESC depth{},motion{};frame.depth->GetDesc(&depth);frame.motion->GetDesc(&motion);
            if (!D3D11FrameCopy::Depth::ReadableSource(depth) || motion.Format!=DXGI_FORMAT_R16G16_FLOAT)
                return Invalid("Intel guides need readable packed/float depth and signed RG16_FLOAT motion");
            if (frame.depthExtent!=state_->guides) {
                // Queued D3D11 waits order writes but do not retire native
                // allocations. Old guide views/resources need CPU proof.
                if (state_->guides.width)
                    if (auto result=state_->Check(state_->bridge->Drain(),"Intel old guide allocation readers did not retire before replacement"); !result) return result;
                SharedTexture newDepth,newMotion;
                if (auto result=state_->Check(state_->bridge->CreateSharedTexture(Description(frame.depthExtent,DXGI_FORMAT_R32_FLOAT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS),newDepth),"Intel depth allocation failed"); !result) return result;
                if (auto result=state_->Check(state_->bridge->CreateSharedTexture(Description(frame.motionExtent,DXGI_FORMAT_R16G16_FLOAT,D3D11_BIND_SHADER_RESOURCE),newMotion),"Intel motion allocation failed"); !result) return result;
                state_->depthCopy.ResetViews();state_->depth=std::move(newDepth);state_->motion=std::move(newMotion);state_->guides=frame.depthExtent;
            }
        }
        state_->waited=false;state_->guidesValid=false;state_->uploaded=false;state_->tagged=false;state_->published=false;
        // SR scene RGB describes opaque world color; its alpha is not HUD
        // coverage and may be zero. Normalize only this owned scene copy.
        auto hr=state_->sceneConverter.Convert(context,frame.output,state_->scene.texture11.Get(),frame.outputEncoding,ColorEncoding::SRGB,SdrAlphaMode::OpaqueScene);
        if (SUCCEEDED(hr)) hr=state_->uiConverter.Convert(context,ui,overlay,state_->ui.texture11.Get(),frame.uiEncoding);
        // Intel's UI policy composites generated images. The application's
        // real backbuffer must already contain its HUD, even with FG disabled.
        // Both prepared inputs are premultiplied sRGB bytes; SRGB -> SRGB in
        // the existing compositor preserves UI + (1-alpha) * scene.
        if (SUCCEEDED(hr)) hr=state_->finalComposer.Convert(context,state_->scene.texture11.Get(),state_->uiView.Get(),state_->finalImage.texture11.Get(),ColorEncoding::SRGB);
        if (SUCCEEDED(hr) && guides) {
            D3D11ContextIsolation::Scope isolation(state_->isolation,context);
            if (!isolation) hr=E_FAIL;
            else {
                hr=state_->depthCopy.Copy(context,frame.depth,state_->depth.texture11.Get(),{frame.depthExtent.width,frame.depthExtent.height});
                if (SUCCEEDED(hr)) hr=D3D11FrameCopy::Color(context,frame.motion,state_->motion.texture11.Get(),{frame.motionExtent.width,frame.motionExtent.height});
            }
        }
        // Seal all queued draws even if a later conversion failed.
        if (auto sealed=state_->Check(state_->bridge->SignalD3D11(InteropWork::FrameGeneration),"Intel producer signal failed"); !sealed) return sealed;
        if (auto result=state_->Check(hr,"Intel owned scene/HUD/guide conversion failed"); !result) return result;
        state_->source=frame.sourceId;state_->epoch=frame.sourceEpoch;state_->uploaded=true;state_->guidesValid=guides;
        return {};
    }
    Result<ID3D12GraphicsCommandList*> XessGenerationTransport::BeginTag()
    {
        if (!state_) return std::unexpected(Invalid("Intel transport is not initialized").error());
        if (auto ready=state_->Ready(); !ready) return std::unexpected(ready.error());
        if (!state_->uploaded || !state_->guidesValid || state_->tagged || state_->recording)
            return std::unexpected(Invalid("Intel tagging requires one fresh owned guide/scene/HUD publication").error());
        ID3D12GraphicsCommandList* list{};
        if (auto result=state_->Check(state_->bridge->Begin(InteropWork::FrameGeneration,&list),"Intel tagging list begin failed"); !result) return std::unexpected(result.error());
        state_->recording=list;return list;
    }
    Result<void> XessGenerationTransport::Tag(ID3D12GraphicsCommandList* list,xefg_swapchain_handle_t context,const XessGenerationFunctions& api,const XessGenerationFrame& frame)
    {
        if (!state_) return Invalid("Intel transport is not initialized");
        if (auto ready=state_->Ready(); !ready) return ready;
        if (!context || !list || list!=state_->recording.Get() || !api.TagFrameResource || !api.TagFrameConstants ||
            !state_->uploaded || !state_->guidesValid || state_->tagged || frame.sourceId!=state_->source || frame.sourceEpoch!=state_->epoch ||
            frame.display!=state_->display || frame.depth!=state_->guides || frame.motion!=state_->guides)
            return Invalid("Intel tags require the exact owned recording and accepted source/epoch/valid regions");
        const std::array<ID3D12Resource*,4> resources{state_->scene.texture12.Get(),state_->depth.texture12.Get(),state_->motion.texture12.Get(),state_->ui.texture12.Get()};
        const std::array<xefg_swapchain_resource_type_t,4> types{XEFG_SWAPCHAIN_RES_HUDLESS_COLOR,XEFG_SWAPCHAIN_RES_DEPTH,XEFG_SWAPCHAIN_RES_MOTION_VECTOR,XEFG_SWAPCHAIN_RES_UI};
        Transition(list,resources,true);
        auto sdk=XEFG_SWAPCHAIN_RESULT_SUCCESS;
        for (std::size_t i=0;i<resources.size();++i) {
            const auto extent=i==1 || i==2?state_->guides:state_->display;
            xefg_swapchain_d3d12_resource_data_t desc{};
            desc.type=types[i];desc.validity=XEFG_SWAPCHAIN_RV_ONLY_NOW;desc.resourceSize={extent.width,extent.height};
            desc.pResource=resources[i];desc.incomingState=D3D12_RESOURCE_STATE_COPY_SOURCE;
            sdk=api.TagFrameResource(context,list,frame.sdkId,&desc);
            if (sdk!=XEFG_SWAPCHAIN_RESULT_SUCCESS) break;
        }
        if (sdk==XEFG_SWAPCHAIN_RESULT_SUCCESS) sdk=api.TagFrameConstants(context,frame.sdkId,&frame.constants);
        Transition(list,resources,false);
        // An SDK failure may follow recorded copies. Submit before reporting it;
        // failed submission never grants slot reuse or releases any owners.
        if (auto result=state_->Check(state_->bridge->Submit(InteropWork::FrameGeneration),"Intel tagging submit failed; retaining recorded copy owners"); !result) return result;
        state_->recording.Reset();
        if (sdk!=XEFG_SWAPCHAIN_RESULT_SUCCESS) {
            state_->fault=E_FAIL;
            return std::unexpected(RuntimeError{sdk==XEFG_SWAPCHAIN_RESULT_ERROR_DEVICE?ErrorKind::DeviceLost:ErrorKind::DispatchFailure,
                sdk,"Intel resource/constants tagging failed; retaining owners"});
        }
        state_->tagged=true;return {};
    }
    Result<void> XessGenerationTransport::PublishTo(ID3D12Resource* backbuffer)
    {
        if (!state_) return Invalid("Intel transport is not initialized");
        if (auto ready=state_->Ready(); !ready) return ready;
        if (!state_->uploaded || state_->recording || !backbuffer) return Invalid("Intel real publication requires completed scene and no unsubmitted tags");
        ComPtr<ID3D12Device> owner;
        if (FAILED(backbuffer->GetDevice(IID_PPV_ARGS(&owner))) || !D3D11FrameCopy::SameObject(owner.Get(),state_->bridge->Device12()))
            return Invalid("Intel output buffer is not on the owned native D3D12 device");
        const auto desc=backbuffer->GetDesc();
        if (desc.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D || desc.Width!=state_->display.width || desc.Height!=state_->display.height ||
            desc.Format!=DXGI_FORMAT_R8G8B8A8_UNORM || desc.MipLevels!=1 || desc.DepthOrArraySize!=1 || desc.SampleDesc.Count!=1 || desc.SampleDesc.Quality)
            return Invalid("Intel output buffer differs from the fixed SDR publication format/extent");
        // Upload signals FG, while the publication allocator is independent.
        if (auto result=state_->Check(state_->bridge->WaitD3D12(InteropWork::FrameGeneration),"Intel producer/tag dependency failed"); !result) return result;
        ID3D12GraphicsCommandList* list{};
        if (auto result=state_->Check(state_->bridge->Begin(InteropWork::SwapChain,&list),"Intel publication list begin failed"); !result) return result;
        if (auto result=state_->Check(D3D11D3D12Interop::RecordCopy(list,state_->finalImage.texture12.Get(),backbuffer),"Intel composed real-image publication copy failed"); !result) return result;
        if (auto result=state_->Check(state_->bridge->Submit(InteropWork::SwapChain),"Intel publication submit failed"); !result) return result;
        state_->published=true;return {};
    }
    Result<void> XessGenerationTransport::WaitPublicationReady()
    {
        if (!state_) return Invalid("Intel transport is not initialized");
        if (auto ready=state_->Ready(); !ready) return ready;
        if (!state_->uploaded || !state_->published || state_->recording)
            return Invalid("Intel source readiness requires an actually submitted current publication");
        return state_->Check(state_->bridge->WaitSubmittedWork(InteropWork::SwapChain),
            "Intel source publication readiness failed; retaining copy owners");
    }
    Result<void> XessGenerationTransport::Retire()
    {
        if (!state_) return {};
        state_->closing=true;
        if (FAILED(state_->fault)) return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,state_->fault,"Intel failed transport retains its complete owner"});
        if (state_->recording) {
            if (auto result=state_->Check(state_->bridge->DiscardUnsubmitted(InteropWork::FrameGeneration),"Intel unsubmitted recording discard failed"); !result) return result;
            state_->recording.Reset();
        }
        if (auto result=state_->Check(state_->bridge->SignalD3D11(InteropWork::FrameGeneration),"Intel final producer signal failed"); !result) return result;
        if (auto result=state_->Check(state_->bridge->Drain(),"Intel producer/tag/publication copies did not retire"); !result) return result;
        state_.reset();return {};
    }
    ID3D12Resource* XessGenerationTransport::Scene() const { return state_?state_->scene.texture12.Get():nullptr; }
    ID3D12Resource* XessGenerationTransport::Ui() const { return state_?state_->ui.texture12.Get():nullptr; }
}
