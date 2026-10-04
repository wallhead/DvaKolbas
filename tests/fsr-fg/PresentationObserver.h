#pragma once
#include <d3dcompiler.h>
#include <dx12/ffx_api_dx12.h>
#include <atomic>
#include <vector>
#include <stdexcept>
// Test-only public Present callback. It composes and captures on the supplied
// D3D12 list; it never touches D3D11, game state or the SDK session mutex. Its
// pixels validate this callback mode, not AMD's automatic texture compositor.
namespace FgObservation
{
    using Microsoft::WRL::ComPtr;
    inline void Check(HRESULT hr){if(FAILED(hr))throw std::runtime_error("D3D12 observer setup failed");}
    inline D3D12_RESOURCE_STATES State(uint32_t state)
    {
        constexpr uint32_t supported=FFX_API_RESOURCE_STATE_COMMON|FFX_API_RESOURCE_STATE_PRESENT|FFX_API_RESOURCE_STATE_UNORDERED_ACCESS|
            FFX_API_RESOURCE_STATE_COMPUTE_READ|FFX_API_RESOURCE_STATE_PIXEL_READ|FFX_API_RESOURCE_STATE_COPY_SRC|FFX_API_RESOURCE_STATE_COPY_DEST|FFX_API_RESOURCE_STATE_RENDER_TARGET;
        if(!state || state&~supported)throw std::runtime_error("unknown public resource state");
        unsigned native{};
        if(state&FFX_API_RESOURCE_STATE_UNORDERED_ACCESS)native|=D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        if(state&FFX_API_RESOURCE_STATE_COMPUTE_READ)native|=D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        if(state&FFX_API_RESOURCE_STATE_PIXEL_READ)native|=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        if(state&FFX_API_RESOURCE_STATE_COPY_SRC)native|=D3D12_RESOURCE_STATE_COPY_SOURCE;
        if(state&FFX_API_RESOURCE_STATE_COPY_DEST)native|=D3D12_RESOURCE_STATE_COPY_DEST;
        if(state&FFX_API_RESOURCE_STATE_RENDER_TARGET)native|=D3D12_RESOURCE_STATE_RENDER_TARGET;
        return static_cast<D3D12_RESOURCE_STATES>(native);
    }
    inline void Barrier(ID3D12GraphicsCommandList* list,ID3D12Resource* resource,D3D12_RESOURCE_STATES from,D3D12_RESOURCE_STATES to)
    {
        if(from==to)return;D3D12_RESOURCE_BARRIER b{};b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;b.Transition.pResource=resource;
        b.Transition.StateBefore=from;b.Transition.StateAfter=to;b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;list->ResourceBarrier(1,&b);
    }
    struct Sample
    {
        ComPtr<ID3D12Resource> readback;uint64_t source{};bool generated{};bool recorded{};
    };
    class Capture
    {
    public:
        Capture(ID3D12Device* device,unsigned capacity,UINT width,UINT height,bool captureScene=false):device_(device),samples_(capacity),width_(width),height_(height),captureScene_(captureScene)
        {
            constexpr char program[]=R"(
Texture2D<float4> scene:register(t0);Texture2D<float4> ui:register(t1);
float4 vs(uint id:SV_VertexID):SV_Position{float2 p=float2((id<<1)&2,id&2);return float4(p*float2(2,-2)+float2(-1,1),0,1);}
// Match pinned FrameInterpolationSwapchainUiComposition.hlsl: generated scene
// alpha can carry interpolation data; the opaque HWND compositor uses RGB only.
float4 ps(float4 p:SV_Position):SV_Target{int3 xy=int3(int2(p.xy),0);float4 h=ui.Load(xy);return float4(h.rgb+scene.Load(xy).rgb*(1-h.a),1);}
)";
            D3D12_DESCRIPTOR_RANGE range{};range.RangeType=D3D12_DESCRIPTOR_RANGE_TYPE_SRV;range.NumDescriptors=2;
            D3D12_ROOT_PARAMETER parameter{};parameter.ParameterType=D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;parameter.ShaderVisibility=D3D12_SHADER_VISIBILITY_PIXEL;
            parameter.DescriptorTable={1,&range};D3D12_ROOT_SIGNATURE_DESC root{};root.NumParameters=1;root.pParameters=&parameter;root.Flags=D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
            ComPtr<ID3DBlob> blob;Check(D3D12SerializeRootSignature(&root,D3D_ROOT_SIGNATURE_VERSION_1,&blob,nullptr));
            Check(device_->CreateRootSignature(0,blob->GetBufferPointer(),blob->GetBufferSize(),IID_PPV_ARGS(&root_)));
            ComPtr<ID3DBlob> vertex,pixel;Check(D3DCompile(program,sizeof(program)-1,"FgObserver",nullptr,nullptr,"vs","vs_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&vertex,nullptr));
            Check(D3DCompile(program,sizeof(program)-1,"FgObserver",nullptr,nullptr,"ps","ps_5_0",D3DCOMPILE_ENABLE_STRICTNESS,0,&pixel,nullptr));
            D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};pso.pRootSignature=root_.Get();pso.VS={vertex->GetBufferPointer(),vertex->GetBufferSize()};pso.PS={pixel->GetBufferPointer(),pixel->GetBufferSize()};
            pso.BlendState.RenderTarget[0].RenderTargetWriteMask=D3D12_COLOR_WRITE_ENABLE_ALL;pso.SampleMask=UINT_MAX;pso.RasterizerState.FillMode=D3D12_FILL_MODE_SOLID;
            auto& blend=pso.BlendState.RenderTarget[0];blend.SrcBlend=blend.SrcBlendAlpha=D3D12_BLEND_ONE;blend.DestBlend=blend.DestBlendAlpha=D3D12_BLEND_ZERO;
            blend.BlendOp=blend.BlendOpAlpha=D3D12_BLEND_OP_ADD;blend.LogicOp=D3D12_LOGIC_OP_NOOP;
            pso.RasterizerState.CullMode=D3D12_CULL_MODE_NONE;pso.RasterizerState.DepthClipEnable=TRUE;pso.DepthStencilState.DepthEnable=FALSE;pso.DepthStencilState.StencilEnable=FALSE;
            pso.DepthStencilState.DepthFunc=D3D12_COMPARISON_FUNC_ALWAYS;pso.DepthStencilState.FrontFace={D3D12_STENCIL_OP_KEEP,D3D12_STENCIL_OP_KEEP,D3D12_STENCIL_OP_KEEP,D3D12_COMPARISON_FUNC_ALWAYS};pso.DepthStencilState.BackFace=pso.DepthStencilState.FrontFace;
            pso.PrimitiveTopologyType=D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;pso.NumRenderTargets=1;pso.RTVFormats[0]=DXGI_FORMAT_R8G8B8A8_UNORM;pso.SampleDesc.Count=1;
            Check(device_->CreateGraphicsPipelineState(&pso,IID_PPV_ARGS(&pipeline_)));
            D3D12_DESCRIPTOR_HEAP_DESC heap{};heap.NumDescriptors=capacity*2;heap.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;heap.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            Check(device_->CreateDescriptorHeap(&heap,IID_PPV_ARGS(&views_)));heap.NumDescriptors=capacity;heap.Type=D3D12_DESCRIPTOR_HEAP_TYPE_RTV;heap.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            Check(device_->CreateDescriptorHeap(&heap,IID_PPV_ARGS(&targets_)));
            viewStride_=device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);targetStride_=device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            // Each stored row is also a separate placed footprint. Satisfy both
            // the 256-byte pitch and 512-byte placement rules on every device.
            rowPitch_=((width*4+D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT-1)/D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT)*D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
            D3D12_HEAP_PROPERTIES properties{};properties.Type=D3D12_HEAP_TYPE_READBACK;D3D12_RESOURCE_DESC desc{};desc.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;
            desc.Width=rowPitch_*(captureScene_?4:2);desc.Height=1;desc.DepthOrArraySize=desc.MipLevels=1;desc.SampleDesc.Count=1;desc.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            for(auto& sample:samples_)Check(device_->CreateCommittedResource(&properties,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&sample.readback)));
        }
        static ffxReturnCode_t Present(ffxCallbackDescFrameGenerationPresent* params,void* opaque)noexcept
        {
            if(!opaque || !params)return FFX_API_RETURN_ERROR_PARAMETER;auto& capture=*static_cast<Capture*>(opaque);
            try{return capture.Record(*params);}catch(...){capture.failure_=true;return FFX_API_RETURN_ERROR;}
        }
        unsigned Count()const{return std::min(next_.load(),unsigned(samples_.size()));}
        bool Failed()const{return failure_.load();}
        const std::vector<Sample>& Samples()const{return samples_;}
        UINT RowPitch()const{return rowPitch_;}
        UINT Width()const{return width_;}
    private:
        ffxReturnCode_t Record(const ffxCallbackDescFrameGenerationPresent& params)
        {
            const auto index=next_.fetch_add(1);if(index>=samples_.size())throw std::runtime_error("observer capacity exceeded");
            auto* list=static_cast<ID3D12GraphicsCommandList*>(params.commandList);auto* scene=static_cast<ID3D12Resource*>(params.currentBackBuffer.resource);
            auto* ui=static_cast<ID3D12Resource*>(params.currentUI.resource);auto* output=static_cast<ID3D12Resource*>(params.outputSwapChainBuffer.resource);
            if(!list || !scene || !ui || !output || !params.frameID)throw std::runtime_error("incomplete observation");
            auto* extra=params.header.pNext;bool premultiplied{};for(;extra;extra=extra->pNext)if(extra->type==FFX_API_CALLBACK_DESC_TYPE_FRAMEGENERATION_PRESENT_PREMUL_ALPHA)premultiplied=reinterpret_cast<const ffxCallbackDescFrameGenerationPresentPremulAlpha*>(extra)->usePremulAlpha;
            if(!premultiplied)throw std::runtime_error("premultiplied UI flag missing");
            for(auto* resource:{scene,ui,output}){const auto d=resource->GetDesc();if(d.Width!=width_ || d.Height!=height_ || d.Format!=DXGI_FORMAT_R8G8B8A8_UNORM || d.SampleDesc.Count!=1)throw std::runtime_error("observer dimensions/format mismatch");}
            const auto sceneState=State(params.currentBackBuffer.state),uiState=State(params.currentUI.state),outputState=State(params.outputSwapChainBuffer.state);
            Barrier(list,scene,sceneState,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);Barrier(list,ui,uiState,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);Barrier(list,output,outputState,D3D12_RESOURCE_STATE_RENDER_TARGET);
            auto view=views_->GetCPUDescriptorHandleForHeapStart();view.ptr+=index*2*viewStride_;D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
            srv.Format=DXGI_FORMAT_R8G8B8A8_UNORM;srv.Shader4ComponentMapping=D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;srv.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D;srv.Texture2D.MipLevels=1;
            device_->CreateShaderResourceView(scene,&srv,view);view.ptr+=viewStride_;device_->CreateShaderResourceView(ui,&srv,view);
            auto target=targets_->GetCPUDescriptorHandleForHeapStart();target.ptr+=index*targetStride_;device_->CreateRenderTargetView(output,nullptr,target);
            auto gpu=views_->GetGPUDescriptorHandleForHeapStart();gpu.ptr+=index*2*viewStride_;ID3D12DescriptorHeap* heaps[]{views_.Get()};list->SetDescriptorHeaps(1,heaps);
            list->SetGraphicsRootSignature(root_.Get());list->SetPipelineState(pipeline_.Get());list->SetGraphicsRootDescriptorTable(0,gpu);
            list->OMSetRenderTargets(1,&target,FALSE,nullptr);const D3D12_VIEWPORT viewport{0,0,float(width_),float(height_),0,1};const D3D12_RECT scissor{0,0,LONG(width_),LONG(height_)};
            list->RSSetViewports(1,&viewport);list->RSSetScissorRects(1,&scissor);list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);list->DrawInstanced(3,1,0,0);
            Barrier(list,scene,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,sceneState);Barrier(list,ui,D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,uiState);Barrier(list,output,D3D12_RESOURCE_STATE_RENDER_TARGET,D3D12_RESOURCE_STATE_COPY_SOURCE);
            auto& sample=samples_[index];D3D12_TEXTURE_COPY_LOCATION source{};source.pResource=output;source.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            D3D12_TEXTURE_COPY_LOCATION destination{};destination.pResource=sample.readback.Get();destination.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            destination.PlacedFootprint.Footprint={DXGI_FORMAT_R8G8B8A8_UNORM,width_,1,1,rowPitch_};
            D3D12_BOX row{0,0,0,width_,1,1};list->CopyTextureRegion(&destination,0,0,0,&source,&row);
            destination.PlacedFootprint.Offset=rowPitch_;row.top=height_/2;row.bottom=row.top+1;list->CopyTextureRegion(&destination,0,0,0,&source,&row);
            if(captureScene_){
                // Independent raw scene samples let callers validate UI blending
                // without assuming generated RGB matches a real source.
                Barrier(list,scene,sceneState,D3D12_RESOURCE_STATE_COPY_SOURCE);
                source.pResource=scene;destination.PlacedFootprint.Offset=rowPitch_*2;
                row.top=0;row.bottom=1;list->CopyTextureRegion(&destination,0,0,0,&source,&row);
                destination.PlacedFootprint.Offset=rowPitch_*3;row.top=height_/2;row.bottom=row.top+1;
                list->CopyTextureRegion(&destination,0,0,0,&source,&row);
                Barrier(list,scene,D3D12_RESOURCE_STATE_COPY_SOURCE,sceneState);
            }
            Barrier(list,output,D3D12_RESOURCE_STATE_COPY_SOURCE,outputState);
            sample.source=params.frameID;sample.generated=params.isGeneratedFrame;sample.recorded=true;return FFX_API_RETURN_OK;
        }
        ComPtr<ID3D12Device> device_;ComPtr<ID3D12RootSignature> root_;ComPtr<ID3D12PipelineState> pipeline_;ComPtr<ID3D12DescriptorHeap> views_,targets_;
        std::vector<Sample> samples_;UINT width_{},height_{},rowPitch_{},viewStride_{},targetStride_{};bool captureScene_{};std::atomic<unsigned> next_{};std::atomic<bool> failure_{};
    };
    inline Capture* activeCapture{};
    inline PfnFfxConfigure originalConfigure{};
    inline ffxReturnCode_t Configure(ffxContext* context,const ffxConfigureDescHeader* header)
    {
        if(activeCapture && header && header->type==FFX_API_CONFIGURE_DESC_TYPE_FRAMEGENERATION){
            auto copy=*reinterpret_cast<const ffxConfigureDescFrameGeneration*>(header);
            if(copy.frameGenerationCallback){copy.presentCallback=Capture::Present;copy.presentCallbackUserContext=activeCapture;return originalConfigure(context,&copy.header);}
        }
        return originalConfigure(context,header);
    }
}
