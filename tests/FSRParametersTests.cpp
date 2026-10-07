#include "Upscaling/FSRParameters.h"
#include "Upscaling/FSRUpscaler.h"
#include "InteropTestRig.h"
#include <DirectXMath.h>
#include <limits>
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;

int main(int argc, char** argv)
{
    Require(argc == 2, "fixture runtime supplied");
    Rig rig; auto bridge = std::make_shared<Interop>(); rig.Initialize(*bridge);
    SharedTexture color, depth, motion, output;
    auto desc = rig.Description(); desc.Width = 960; desc.Height = 540;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    Check(bridge->CreateSharedTexture(desc, color), "color allocation");
    desc.Format = DXGI_FORMAT_R32_FLOAT; Check(bridge->CreateSharedTexture(desc, depth), "depth allocation");
    desc.Format = DXGI_FORMAT_R16G16_FLOAT; Check(bridge->CreateSharedTexture(desc, motion), "motion allocation");
    desc.Width = 1921; desc.Height = 1081; desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS; Check(bridge->CreateSharedTexture(desc, output), "output allocation");
    GpuFrameResources resources{color.texture12.Get(), depth.texture12.Get(), motion.texture12.Get(), output.texture12.Get()};
    UpscaleFrame frame; frame.backend = BackendKind::Fsr;
    frame.render = frame.subrect = {960,540}; frame.display = {1921,1081};
    frame.colorFormat = DXGI_FORMAT_R16G16B16A16_FLOAT; frame.depthFormat = DXGI_FORMAT_R32_FLOAT;
    frame.motionFormat = DXGI_FORMAT_R16G16_FLOAT; frame.colorIsLinear = true;
    frame.sourceId = 109; frame.deltaMilliseconds = 16; frame.sharpness = 0.6f;
    frame.jitterX = 0.25f; frame.jitterY = -0.5f; frame.motionConvention = {960,-540,true,false};
    frame.camera.identity = 1; frame.camera.nearDistance = 0.1f; frame.camera.farDistance = 100;
    frame.camera.verticalFovRadians = 1; frame.camera.worldUnitsToMeters = 0.0142875f;
    DirectX::XMFLOAT4X4 view, projection;
    DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveFovLH(1,2,0.1f,100));
    std::memcpy(frame.camera.view.data(), &view, sizeof(view));
    std::memcpy(frame.camera.projection.data(), &projection, sizeof(projection));
    FsrContextLimits limits{frame.render, frame.display};
    auto mapped = BuildFsrDispatch(resources, frame, limits);
    Require(mapped && mapped->frameTimeDelta == 16 && mapped->motionVectorScale.x == 960 && mapped->motionVectorScale.y == -540,
        "FsrParameterMapping: milliseconds and explicit pixel motion scales preserved");
    Require(mapped->enableSharpening && mapped->sharpness == 0.6f && !mapped->reactive.resource && !mapped->transparencyAndComposition.resource,
        "SharpenExactlyOnce: one SDK sharpening request and absent masks");
    Require(mapped->cameraNear == 0.1f && mapped->cameraFar == 100 && mapped->viewSpaceToMetersFactor == 0.0142875f,
        "physical camera distances and world scale preserved");
    for (float bad : {0.0f, -1.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity()}) {
        auto changed = frame; changed.deltaMilliseconds = bad; auto invalid = BuildFsrDispatch(resources, changed, limits);
        Require(!invalid && invalid.error().kind == ErrorKind::InvalidInput, "SourceTimeAndIdentity: invalid time rejected");
    }
    auto rejected = [&](auto change, const char* why) { auto copy=frame; change(copy); Require(!BuildFsrDispatch(resources,copy,limits),why); };
    rejected([](auto& f){f.subrect.width++;}, "subrect bounds rejected");
    rejected([](auto& f){f.camera.verticalFovRadians=0;}, "invalid FOV rejected");
    rejected([](auto& f){f.camera.position[0]=std::numeric_limits<float>::quiet_NaN();}, "nonfinite camera rejected");
    rejected([](auto& f){f.camera.projection.fill(0);}, "missing physical projection rejected");
    rejected([](auto& f){f.camera.view.fill(0);}, "degenerate camera basis rejected");
    rejected([](auto& f){f.motionConvention.currentToPrevious=false;}, "unknown motion direction rejected");
    rejected([](auto& f){f.motionConvention.includesJitter=true;}, "mismatched jitter convention rejected");
    rejected([](auto& f){f.camera.depthInverted=true;}, "mismatched depth convention rejected");
    rejected([](auto& f){f.colorIsLinear=false;}, "color encoding not inferred from format");
    rejected([](auto& f){f.sharpness=1.1f;}, "sharpening range rejected");
    rejected([](auto& f){f.jitterX=std::numeric_limits<float>::infinity();}, "nonfinite jitter rejected");
    auto missing=resources; missing.motion=nullptr; Require(!BuildFsrDispatch(missing,frame,limits), "missing guides never fabricated");
    auto alias=resources; alias.output=resources.color; Require(!BuildFsrDispatch(alias,frame,limits), "output cannot alias input");
    auto runtime=std::make_shared<FsrRuntime>(); Require(bool(runtime->Load(std::filesystem::absolute(argv[1]))), "fixture runtime loaded");
    auto dll=GetModuleHandleW((std::filesystem::absolute(argv[1])/"FSR/amd_fidelityfx_loader_dx12.dll").c_str());
    auto mode=reinterpret_cast<void(*)(unsigned)>(GetProcAddress(dll,"FixtureMode"));
    auto queryId=reinterpret_cast<uint64_t(*)()>(GetProcAddress(dll,"FixtureQueryId"));
    auto createId=reinterpret_cast<uint64_t(*)()>(GetProcAddress(dll,"FixtureCreateId"));
    ProviderInfo provider{17,"fixture analytical FSR 3.1.5"};
    for(unsigned failureMode:{31u,32u,33u}) {
        mode(failureMode);FsrUpscaler incompatibleResources;
        Require(!incompatibleResources.Initialize(runtime,rig.device12.Get(),provider,Quality::Performance,frame.render,frame.display),
            "failed resource query, missing required mask or unknown mandatory input must reject startup");
        Require(!incompatibleResources.ActualProvider(),"resource-contract failure cannot report an active provider");
        Require(bool(incompatibleResources.DestroyAfterRetirement()),"failed resource-contract context cleans up before reuse");
    }
    mode(0);
    FsrUpscaler fsr; fsr.SetRetirementBridge(bridge);
    Require(bool(fsr.Initialize(runtime,rig.device12.Get(),provider,Quality::Performance,frame.render,frame.display)), "context initialized");
    Require(fsr.RenderExtent() && *fsr.RenderExtent()==Extent{960,540} && queryId()==createId(), "OddResolutionFromProvider: sizing and create agree");
    Require(fsr.ActualProvider() && fsr.ActualProvider()->id==provider.id, "ActualProviderReported: actual ID retained");
    auto jitter=fsr.QueryJitter(109); auto repeated=fsr.QueryJitter(109); auto next=fsr.QueryJitter(110);
    Require(jitter && repeated && next && *jitter==*repeated && *jitter!=*next, "source identity selects jitter, independent of query count");
    Check(bridge->SignalProducer(),"fixture producer"); ID3D12GraphicsCommandList* list{}; Check(bridge->Begin(&list),"fixture command recording");
    Require(bool(fsr.Dispatch(list,resources,frame)),"fixture records dispatch");
    Require(!fsr.Dispatch(list,resources,frame),"duplicate source identity rejected");
    Require(!fsr.DestroyAfterRetirement(),"recorded but unsubmitted dispatch cannot release its context");
    Check(bridge->Submit(),"fixture dispatch submitted"); Check(bridge->WaitConsumer(),"fixture consumer wait");
    Require(bool(fsr.DestroyAfterRetirement()), "context retired before unload");
    mode(4); FsrUpscaler mismatch; Require(!mismatch.Initialize(runtime,rig.device12.Get(),provider,Quality::Performance,frame.render,frame.display), "actual provider mismatch rejects initialization");
    mode(0); FsrUpscaler wrongSize; Require(!wrongSize.Initialize(runtime,rig.device12.Get(),provider,Quality::Performance,{961,540},frame.display), "caller cannot invent odd render extent");
    auto destructionCount=reinterpret_cast<unsigned(*)()>(GetProcAddress(dll,"FixtureDestroyCount"));
    auto heldBridge=std::make_shared<Interop>();rig.Initialize(*heldBridge);heldBridge->SetRetirementWaitPolicy({1,10});
    ComPtr<ID3D12Fence> reader12;ComPtr<ID3D11Fence> reader11;rig.SharedGate(reader12,reader11);
    std::weak_ptr<FsrRuntime> retainedRuntime=runtime;
    unsigned beforeDestroy{};
    {
        FsrUpscaler stalled;Require(bool(stalled.SetRetirementBridge(heldBridge)),"stalled owner bridge");
        Require(bool(stalled.Initialize(runtime,rig.device12.Get(),provider,Quality::Performance,frame.render,frame.display)),"stalled owner context");
        Check(heldBridge->SignalProducer(),"stalled producer");Check(heldBridge->Begin(&list),"stalled record");
        frame.sourceId++;Require(bool(stalled.Dispatch(list,resources,frame)),"stalled fixture dispatch");
        Check(heldBridge->Submit(),"stalled submit");Check(heldBridge->WaitConsumer(),"stalled wait");
        Check(rig.context4->Wait(reader11.Get(),1),"delay actual output reader");
        beforeDestroy=destructionCount();Require(!stalled.DestroyAfterRetirement(),"retirement failure retains context");
        Require(destructionCount()==beforeDestroy,"no SDK destruction before final reader retires");runtime.reset();
    }
    Require(!retainedRuntime.expired() && destructionCount()==beforeDestroy,"safe abandonment retains SDK modules/context after owner destruction");
    Check(reader12->Signal(1),"release independent reader gate after abandonment observation");Check(heldBridge->Drain(),"retire controlled test work");
    rig.ValidateDebug(); std::puts("PASS: FSR mapping, source time/jitter, provider sizing, context identity and sharpening");
}
