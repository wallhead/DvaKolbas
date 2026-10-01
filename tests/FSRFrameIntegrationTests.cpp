#include "Upscaling/FSRFrameAdapter.h"
#include "Upscaling/FSRHostResources.h"
#include "FrameGen/SourceFrameEvaluator.h"
#include "FrameGen/SourceFrameCoordinator.h"
#include "InteropTestRig.h"
#include <DirectXMath.h>
#include <string>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
struct Order
{
    std::string order;unsigned temporal{},effects{},ack{};bool before{};UpscaleOutcome outcome{UpscaleOutcome::Temporal};
    void CopyInput(ID3D11DeviceContext*,const UpscaleFrame&){order+='C';}
    bool EvaluateOptionalPreUpscale(UpscaleFrame&){return true;}
    void RenderReShade(const UpscaleFrame&,bool stage){if(stage==before){++effects;order+='E';}}
    Result<UpscaleOutcome> EvaluateUpscaler(const UpscaleFrame&){++temporal;order+='S';return outcome;}
    void UpscaleSucceeded(){++ack;}
    GenerationPreparationStatus PrepareGeneration(const UpscaleFrame&){return GenerationPreparationStatus::NotRequested;}
};
struct CompletedUI
{
    bool completed{};unsigned reads{};
    bool NativeUIAvailable(){return true;}bool ComposeNativeUI(){Require(completed,"CompletedUiOnly: no unfinished foreground reads");++reads;return true;}
    void DisableRuntime(){}void CompositionFailed(){Require(false,"UI composition unexpectedly failed");}
};
struct FaultBridge:Interop{void SimulateFault(HRESULT result){fault_=result;}};
int main(int argc,char** argv)
{
    Require(argc==2,"fixture root supplied");Rig rig;
    NativeUIPass pass;SourceFrameCoordinator coordinator(pass);CompletedUI ui;
    pass.Frame().EvaluationSucceeded();pass.Frame().EnteredUI();
    Require(!coordinator.FinishForPresent(ui) && ui.reads==0,"UI pointer availability alone does not mean composition completed");
    pass.Frame().PreparingPresent();ui.completed=true;Require(coordinator.FinishForPresent(ui) && ui.reads==1,"compose completed UI at final Present boundary");
    Require(!coordinator.FinishForPresent(ui) && ui.reads==1,"UI completion cannot be consumed twice");
    for(bool before:{false,true}){Order operations;operations.before=before;UpscaleFrame frame;frame.backend=BackendKind::Fsr;
        auto result=SourceFrameEvaluator::Evaluate(rig.context11.Get(),frame,operations);
        Require(result.outcome==UpscaleOutcome::Temporal && operations.temporal==1 && operations.effects==1 && operations.order==(before?"CES":"CSE"),"SourceFrameOrdering: one SR and selected effect position");
        frame.backend=BackendKind::External;SourceFrameEvaluator::Evaluate(rig.context11.Get(),frame,operations);Require(operations.temporal==1,"CsDoesNotUpscaleTwice");
        frame.backend=BackendKind::Fsr;operations.outcome=UpscaleOutcome::SpatialRecovery;SourceFrameEvaluator::Evaluate(rig.context11.Get(),frame,operations);Require(operations.ack==1,"spatial recovery cannot acknowledge temporal history");
    }
    FsrHostResources host(std::filesystem::absolute(argv[1]));BackendConfiguration config;config.backend=BackendKind::Fsr;config.generationEnabled=false;config.generationBackend=0;
    auto render=host.PrepareSizing(rig.device11.Get(),config,{65,37});Require(bool(render),"frame sizing");Require(bool(host.CompleteStartup()),"frame context");
    FsrFrameAdapter adapter(*host.Upscaler(),host.Bridge(),host.Resources(),host.Color11(),host.Depth11(),host.Motion11(),host.Output11(),ColorEncoding::Gamma22);
    auto desc=rig.Description();desc.Width=render->width;desc.Height=render->height;desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D> input,output,depth,motion;Check(rig.device11->CreateTexture2D(&desc,nullptr,&input),"game color");desc.Width=65;desc.Height=37;Check(rig.device11->CreateTexture2D(&desc,nullptr,&output),"native handoff");
    desc.Width=render->width;desc.Height=render->height;desc.Format=DXGI_FORMAT_R32_FLOAT;Check(rig.device11->CreateTexture2D(&desc,nullptr,&depth),"real depth input");desc.Format=DXGI_FORMAT_R16G16_FLOAT;Check(rig.device11->CreateTexture2D(&desc,nullptr,&motion),"real motion input");
    ComPtr<ID3D11RenderTargetView> rtv;Check(rig.device11->CreateRenderTargetView(input.Get(),nullptr,&rtv),"color patch RTV");float patch[4]{0.25f,0.5f,0.75f,1};rig.context11->ClearRenderTargetView(rtv.Get(),patch);
    UpscaleFrame frame;frame.backend=BackendKind::Fsr;frame.input=frame.color=input.Get();frame.output=output.Get();frame.depth=depth.Get();frame.motion=motion.Get();frame.render=frame.subrect=*render;frame.display={65,37};
    frame.deltaMilliseconds=16;frame.sourceId=1;frame.motionConvention={float(render->width),float(render->height),true,false};frame.camera.identity=1;frame.camera.nearDistance=0.1f;frame.camera.farDistance=100;frame.camera.verticalFovRadians=1;
    DirectX::XMFLOAT4X4 view,proj;DirectX::XMStoreFloat4x4(&view,DirectX::XMMatrixIdentity());DirectX::XMStoreFloat4x4(&proj,DirectX::XMMatrixPerspectiveFovLH(1,65.0f/37,0.1f,100));std::memcpy(frame.camera.view.data(),&view,64);std::memcpy(frame.camera.projection.data(),&proj,64);
    auto dll=GetModuleHandleW((std::filesystem::absolute(argv[1])/"FSR/amd_fidelityfx_loader_dx12.dll").c_str());auto mode=reinterpret_cast<void(*)(unsigned)>(GetProcAddress(dll,"FixtureMode"));mode(8);
    auto slot=host.Bridge()->CurrentSlot(Work::Upscaling);auto recovered=adapter.Evaluate(frame);Require(recovered && *recovered==UpscaleOutcome::SpatialRecovery,"SpatialRecoveryNativeOutput after vendor dispatch failure");
    Require(host.Bridge()->CurrentSlot(Work::Upscaling)==slot,"failed unsubmitted vendor commands discarded without a fake submission");
    desc.Width=65;desc.Height=37;desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.BindFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;desc.Usage=D3D11_USAGE_STAGING;ComPtr<ID3D11Texture2D> readback;Check(rig.device11->CreateTexture2D(&desc,nullptr,&readback),"recovery readback");
    rig.context11->CopyResource(readback.Get(),output.Get());D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped),"recover pixels");auto* bytes=static_cast<unsigned char*>(mapped.pData);
    Require(std::abs(int(bytes[0])-64)<=1 && std::abs(int(bytes[1])-128)<=1 && std::abs(int(bytes[2])-191)<=1,"native recovery contains correctly encoded source patch");rig.context11->Unmap(readback.Get(),0);
    mode(0);frame.sourceId=2;auto next=adapter.Evaluate(frame);Require(next && *next==UpscaleOutcome::SpatialRecovery,"failed context remains honest spatial recovery until restart");
    frame.deltaMilliseconds=0;auto invalid=adapter.Evaluate(frame);Require(invalid && *invalid==UpscaleOutcome::SkippedInvalidInput,"invalid source timing is skipped");
    auto faultBridge=std::make_shared<FaultBridge>();rig.Initialize(*faultBridge);
    FsrFrameAdapter faulted(*host.Upscaler(),faultBridge,host.Resources(),host.Color11(),host.Depth11(),host.Motion11(),host.Output11(),ColorEncoding::Gamma22);
    frame.deltaMilliseconds=16;faultBridge->SimulateFault(DXGI_ERROR_DEVICE_REMOVED);auto removed=faulted.Evaluate(frame);
    Require(!removed && removed.error().kind==ErrorKind::DeviceLost,"simulated device loss is fatal, never relabeled spatial output");
    Require(bool(host.Retire()),"retire integration feature");rig.ValidateDebug();std::puts("PASS: frame ordering, external ownership, native recovery and discarded failed commands");
}
