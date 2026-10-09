#include "ReShadeIntegration.h"
#include "FrameGen/OrdinaryPresentation.h"
#include "FrameGen/PresentationDevice.h"
#include "Graphics/D3D11D3D12Interop.h"
#include "Upscaling/FSRColorConversion.h"
#include "Upscaling/FSRPreparedResources.h"
#if defined(TRP_TEST_XESS_OWNER)
#include "Upscaling/XessHostResources.h"
#include "Upscaling/SdrSharpeningPass.h"
#endif
#if defined(TRP_ENABLE_FSR_FG)
#include "FrameGen/FSRHostPresentation.h"
#include "FrameGen/NativeUICompletion.h"
#include "Upscaling/FSRFrameAdapter.h"
#include "Upscaling/FSRPresentationColor.h"
#endif
#if defined(TRP_TEST_NR_FSR)
#include "NeuralRendering/BeforeHost.h"
#include "nr-runtime/GpuProbeGuard.h"
#endif
#include <reshade/reshade_events.hpp>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string_view>

using Microsoft::WRL::ComPtr;
using TheosRenderPipeline::ReShadeIntegration;
namespace api = reshade::api;
static api::effect_runtime* owned{};
static unsigned draws{}, automaticDraws{}, automaticRuntimes{};
static bool creatingOrdinary{};
static api::effect_runtime* automaticOrdinary{};
// Both placements exceed ReShade 6.8's 160x120 minimum runtime size.
static constexpr UINT outputWidth = 512, outputHeight = 256;
static UINT renderWidth = 256, renderHeight = 128;
static bool nrNative{};
static UINT expectedDepthWidth{}, expectedDepthHeight{};
static bool depthExtentMatched{};
static std::filesystem::path nrRoot,nrCore;
static void Init(api::effect_runtime* runtime)
{ if (creatingOrdinary) { automaticOrdinary = runtime; ++automaticRuntimes; }
  else if (runtime->get_device()->get_api() == api::device_api::d3d11) { owned = runtime; } else { ++automaticRuntimes; } }
static void Destroy(api::effect_runtime* runtime)
{ if (runtime == automaticOrdinary) { automaticOrdinary = nullptr; --automaticRuntimes; } }
static void Draw(api::effect_runtime* runtime, api::command_list*, api::resource_view, api::resource_view)
{
    if (runtime != owned) { ++automaticDraws; return; }
    ++draws;
    const auto variable = runtime->find_texture_variable("Probe.fx", "Depth");
    api::resource_view view{}; runtime->get_texture_binding(variable, &view, nullptr);
    const auto desc = runtime->get_device()->get_resource_desc(runtime->get_device()->get_resource_from_view(view));
    depthExtentMatched = desc.texture.width == expectedDepthWidth && desc.texture.height == expectedDepthHeight && desc.texture.format == api::format::r32_float;
}
static void Require(bool ok, const char* why)
{ if (!ok) { std::fprintf(stderr, "FAIL: %s; stage=%s\n", why, ReShadeIntegration::Get().Status().c_str()); std::exit(1); } }
static void Check(HRESULT hr, const char* why) { Require(SUCCEEDED(hr), why); }
struct Surface
{
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11RenderTargetView> rtv;
    Surface(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        D3D11_TEXTURE2D_DESC desc{}; desc.Width = w; desc.Height = h;
        desc.MipLevels = desc.ArraySize = desc.SampleDesc.Count = 1;
        desc.Format = format; desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        Check(device->CreateTexture2D(&desc, nullptr, &texture), "texture");
        Check(device->CreateRenderTargetView(texture.Get(), nullptr, &rtv), "RTV");
    }
    void Paint(ID3D11DeviceContext* context, const std::array<float, 4>& color)
    { context->ClearRenderTargetView(rtv.Get(), color.data()); }
};
static std::array<unsigned char, 4> Pixel(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture, UINT x = 0, UINT y = 0)
{
    D3D11_TEXTURE2D_DESC desc{}; texture->GetDesc(&desc);
    desc.BindFlags = 0; desc.Usage = D3D11_USAGE_STAGING; desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Texture2D> staging;
    Check(device->CreateTexture2D(&desc, nullptr, &staging), "staging");
    context->CopyResource(staging.Get(), texture);
    D3D11_MAPPED_SUBRESOURCE mapped{}; Check(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped), "readback");
    auto* p = static_cast<const unsigned char*>(mapped.pData) + y * mapped.RowPitch + x * 4;
    std::array<unsigned char, 4> value{p[0], p[1], p[2], p[3]}; context->Unmap(staging.Get(), 0); return value;
}

#if defined(TRP_ENABLE_FSR_FG)
static void FsrReShadeHost(HWND window,IDXGIFactory* factory,ID3D11Device* device,ID3D11DeviceContext* context)
{
    using namespace TheosRenderPipeline;using namespace Upscaling;
    auto& effects=ReShadeIntegration::Get();
    wchar_t executable[32768]{};Require(GetModuleFileNameW(nullptr,executable,32768)!=0,"AMD fixture root");
    auto resources=std::make_shared<FsrHostResources>(std::filesystem::path(executable).parent_path(),
        +[](IUnknown* adapter,D3D_FEATURE_LEVEL level,ID3D12Device** output)->HRESULT {
            return ReShadeIntegration::Get().CreateSourceDevice(adapter,level,output,true);
        });
    FsrSettings settings;settings.quality=nrNative?Quality::NativeAA:Quality::Performance;settings.sourceColorEncoding=nrNative?ColorEncoding::Gamma22:ColorEncoding::SRGB;
    const UINT largeWidth=nrNative?640:320,largeHeight=nrNative?360:180;
    DXGI_SWAP_CHAIN_DESC desc{};desc.BufferDesc.Width=outputWidth;desc.BufferDesc.Height=outputHeight;
    desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.BufferCount=2;desc.SampleDesc.Count=1;
    desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.OutputWindow=window;desc.Windowed=TRUE;
    desc.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH|DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING|DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    FsrHostPresentation host;auto extent=host.Create(factory,device,resources,desc,settings);
    if(!extent)std::printf("AMD startup failure: %s native=%lld\n",extent.error().message.c_str(),extent.error().nativeResult);
    Require(extent && *extent==Extent{renderWidth,renderHeight},"actual AMD host reduced sizing on original ReShade producer");
    Require(bool(resources->CompleteStartup()),"real analytical SR context");
#if defined(TRP_TEST_NR_FSR)
    // Catches lazy NR creating a second ReShade device proxy which changes
    // GetDevice identity on an already prepared native FSR/FG host.
    namespace NR=NeuralRendering;
    std::unique_ptr<NR::BeforeHost> neural;
    NR::PreparedFsrInput nrPrepared;
    unsigned nrEvaluations{},nrBypasses{},nrDirect{};std::uint64_t nrRevision{};bool nrEnabled{};
    auto retireNeural=[&]{if(neural){Require(bool(neural->Retire()),"NR retires before AMD readers/device");neural.reset();}};
    auto evaluateNeural=[&](UpscaleFrame& f,bool enabled,bool direct){
        if(nrRoot.empty())return;
        if(!neural){
            neural=std::make_unique<NR::BeforeHost>();NR::StartupSettings startup;
            startup.community=true;startup.runtimeRoot=nrRoot;startup.driverCore=nrCore;startup.sourceEncoding=settings.sourceColorEncoding;
            auto inspected=neural->Inspect(device,startup,std::filesystem::absolute("nr-fsr-cache"),resources->Bridge()->Device12());
            if(!inspected)std::fprintf(stderr,"NR inspection: %s\n",inspected.error().message.c_str());
            Require(bool(inspected),"combined NR renderer inspection");
            const auto luid=resources->Bridge()->Device12()->GetAdapterLuid();
            std::printf("NR retained presenter profile=%s luid=%lu:%ld device=%p\n",std::string(neural->ProfileId()).c_str(),luid.LowPart,luid.HighPart,resources->Bridge()->Device12());
        }
        NR::BeforeInput input;input.context=context;input.color=f.input;input.depth=f.depth;input.motion=f.motion;
        input.colorExtent=input.guideExtent={f.render.width,f.render.height};input.epoch=input.guideEpoch=1;
        input.sourceId=input.guideSourceId=f.sourceId;input.previousSourceId=f.sourceId-1;input.presentationTime=double(f.sourceId)/72;
        input.motionScaleX=float(f.render.width);input.motionScaleY=float(f.render.height);input.reset=f.reset;
        NR::SettingsSnapshot snapshot;snapshot.enabled=enabled;
        if(!nrRevision || enabled!=nrEnabled){++nrRevision;nrEnabled=enabled;}
        snapshot.revision=nrRevision;
        f.sourceEpoch=input.epoch;
        auto result=neural->Evaluate(input,snapshot,direct?&nrPrepared:nullptr);
        if(!result)std::fprintf(stderr,"NR source=%llu: %s\n",static_cast<unsigned long long>(f.sourceId),result.error().message.c_str());
        Require(result && result->evaluated==enabled,"one real NR pass before SR only when enabled");
        f.reset|=result->effectiveReset;enabled?++nrEvaluations:++nrBypasses;
        if(nrPrepared.Valid())++nrDirect;
    };
#endif
    Require(automaticRuntimes==0,"AMD presenter has no automatic ReShade runtime");
    Surface source(device,renderWidth,renderHeight),input(device,renderWidth,renderHeight),output(device,outputWidth,outputHeight);
    Surface depth(device,renderWidth,renderHeight,DXGI_FORMAT_R32_FLOAT),motion(device,renderWidth,renderHeight,DXGI_FORMAT_R16G16_FLOAT),hud(device,outputWidth,outputHeight);
    ComPtr<ID3D11Device> producerDevice,viewDevice,textureDevice;context->GetDevice(&producerDevice);
    hud.rtv->GetDevice(&viewDevice);hud.texture->GetDevice(&textureDevice);
    std::printf("HUD ownership producer=%p viewDevice=%p textureDevice=%p sameView=%d sameTexture=%d\n",producerDevice.Get(),viewDevice.Get(),textureDevice.Get(),
        D3D11FrameCopy::SameObject(producerDevice.Get(),viewDevice.Get()),D3D11FrameCopy::SameObject(producerDevice.Get(),textureDevice.Get()));
    const std::array<float,4> hudSentinel{0,1,0,1};hud.Paint(context,hudSentinel);
    Require(PrepareFsrPresentUi(context,hud.rtv.Get(),true) && Pixel(device,context,hud.texture.Get())==std::array<unsigned char,4>{0,255,0,255},
        "real ReShade completed HUD retained");
    Require(PrepareFsrPresentUi(context,hud.rtv.Get(),false),"RealReShadeHudProducerPreparation");
    Require(Pixel(device,context,hud.texture.Get())==std::array<unsigned char,4>{0,0,0,0},"real ReShade empty HUD cleared without alpha residue");
    Surface foreground(device,outputWidth,outputHeight),publication(device,outputWidth,outputHeight);
    ComPtr<ID3D11ShaderResourceView> foregroundView;
    Check(device->CreateShaderResourceView(foreground.texture.Get(),nullptr,&foregroundView),"real ReShade foreground view");
    foreground.Paint(context,{.5f,0,0,.5f});hud.Paint(context,hudSentinel);
    const auto foregroundPixel=Pixel(device,context,foreground.texture.Get());
    std::printf("Foreground producer pixel=%u,%u,%u,%u\n",foregroundPixel[0],foregroundPixel[1],foregroundPixel[2],foregroundPixel[3]);
    Require(std::abs(int(foregroundPixel[0])-128)<=1 && foregroundPixel[1]==0 && foregroundPixel[2]==0 && std::abs(int(foregroundPixel[3])-128)<=1,
        "real ReShade foreground is half-alpha premultiplied red");
    FsrPresentationUiConverter uiConverter;
    Check(uiConverter.Convert(context,hud.texture.Get(),foregroundView.Get(),publication.texture.Get(),ColorEncoding::SRGB),
        "RealReShadeForegroundResourceOwnership");
    const auto mergedPixel=Pixel(device,context,publication.texture.Get());
    Require(std::abs(int(mergedPixel[0])-128)<=1 && std::abs(int(mergedPixel[1])-127)<=1 && mergedPixel[2]==0 && mergedPixel[3]==255,
        "real ReShade half-alpha foreground blended once over opaque HUD");
    const std::array<float,4> scene{.25f,.5f,.25f,1},ui{0,.25f,0,.5f};
    depth.Paint(context,{.5f,0,0,0});motion.Paint(context,{0,0,0,0});
    auto makeAdapter=[&]{return std::make_unique<FsrFrameAdapter>(*resources->Upscaler(),resources->Bridge(),resources->Resources(),resources->Color11(),resources->Depth11(),
        resources->Motion11(),resources->Output11(),resources->HandoffEncoding());};
    auto adapter=makeAdapter();auto* originalChain=host.SwapChain();const auto originalBridge=resources->Bridge();
    DXGI_SWAP_CHAIN_DESC actualDesc{};Check(originalChain->GetDesc(&actualDesc),"real AMD requested flags");
    Require(actualDesc.Flags==desc.Flags && actualDesc.Windowed,"real AMD preserves windowed Display Tweaks flags");
    Check(originalChain->SetMaximumFrameLatency(1),"real AMD maximum latency");
    Check(originalChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709),"real AMD SDR state");
    const auto cachedWaitable=originalChain->GetFrameLatencyWaitableObject();Require(cachedWaitable!=nullptr,"real AMD cached waitable handle");
    // Compile the real source-stage probe before counting source transactions.
    for(unsigned i=0;i<300 && !draws;++i){output.Paint(context,scene);hud.Paint(context,ui);
        Check(effects.Render(output.texture.Get(),depth.texture.Get(),{outputWidth,outputHeight},{renderWidth,renderHeight},false),"AMD ReShade shader warmup");
        Check(effects.FinishUI(hud.texture.Get()),"AMD ReShade GUI warmup");effects.PresentCompleted();Sleep(10);}
    Require(owned && draws,"actual AMD source effect compiled");owned->open_overlay(false,api::input_source::none);
    unsigned sourceTransactions{},upscales{},callbacks{},uiChecks{},disabledSources{},spatialSources{},resizeChecks{},phase{},largerSources{},largerCallbacks{},suspendChecks{};
    std::uint64_t sourceId{};
    UpscaleFrame frame;frame.backend=BackendKind::Fsr;frame.color=source.texture.Get();frame.input=input.texture.Get();frame.output=output.texture.Get();
    frame.depth=depth.texture.Get();frame.motion=motion.texture.Get();frame.render=frame.subrect={renderWidth,renderHeight};frame.display={outputWidth,outputHeight};
    frame.deltaMilliseconds=1000.f/72;frame.motionConvention={float(renderWidth),float(renderHeight),true,false};
    frame.camera.identity=7;frame.camera.nearDistance=.1f;frame.camera.farDistance=100;
    frame.camera.verticalFovRadians=1.04719755f;frame.camera.worldUnitsToMeters=1;
    frame.camera.view={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    frame.camera.projection={1,0,0,0,0,1.7320508f,0,0,0,0,1.001001f,1,0,0,-.1001001f,0};
    for(bool before:{false,true,false}) {
        effects.SetBeforeUpscaling(before);const FrameExtent effectExtent=before?FrameExtent{renderWidth,renderHeight}:FrameExtent{outputWidth,outputHeight};
        expectedDepthWidth=effectExtent.width;expectedDepthHeight=effectExtent.height;
        unsigned stable{};const auto callbacksBeforePhase=callbacks;bool resetGeneration{};
        for(unsigned i=0;i<32;++i){
            Check(host.WaitBeforeProducer(),"AMD guide wait before SR writes");
            source.Paint(context,scene);context->CopyResource(input.texture.Get(),source.texture.Get());hud.Paint(context,ui);
            frame.sourceId=++sourceId;frame.reset=i==0;auto outcome=UpscaleOutcome::Temporal;
            const bool menu=i==31,requested=i!=23;
#if defined(TRP_TEST_NR_FSR)
            evaluateNeural(frame,!menu && i!=11,!before);
#endif
            const auto drawCount=draws,automaticCount=automaticDraws;
            Check(effects.Render(input.texture.Get(),menu?nullptr:depth.texture.Get(),{renderWidth,renderHeight},{renderWidth,renderHeight},true),"AMD before effects stage");
#if defined(TRP_TEST_NR_FSR)
            auto evaluated=menu?adapter->Spatial(frame):nrPrepared.Valid()?adapter->EvaluatePrepared(frame,nrPrepared):adapter->Evaluate(frame);
#else
            auto evaluated=menu?adapter->Spatial(frame):adapter->Evaluate(frame);
#endif
            if(adapter->LastError())std::fprintf(stderr,"AMD SR source=%llu failure: %s\n",static_cast<unsigned long long>(frame.sourceId),adapter->LastError()->message.c_str());
            Require(evaluated && (*evaluated==UpscaleOutcome::Temporal || *evaluated==UpscaleOutcome::SpatialRecovery),"exactly one real SR/spatial pass");
            outcome=*evaluated;++upscales;
            Check(effects.Render(output.texture.Get(),menu?nullptr:depth.texture.Get(),{outputWidth,outputHeight},{renderWidth,renderHeight},false),"AMD after effects stage");
            Check(effects.FinishUI(hud.texture.Get()),"AMD completed GUI before SDK handoff");
            const auto hudPixel=Pixel(device,context,hud.texture.Get(),outputWidth-1,outputHeight-1);
            const auto scenePixel=Pixel(device,context,output.texture.Get());
            if(!menu && draws==drawCount+1 && depthExtentMatched &&
                (!nrRoot.empty() || (scenePixel[0]>=94 && scenePixel[0]<=98 && scenePixel[1]>=126 && scenePixel[1]<=130)))++stable;
            context->CopyResource(host.SceneTarget11(),output.texture.Get());
            auto generationFrame=frame;generationFrame.depthFormat=DXGI_FORMAT_R32_FLOAT;generationFrame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
            generationFrame.colorIsLinear=true;generationFrame.reset |= adapter->LastTemporalReset();
            Check(host.Present(generationFrame,outcome,hud.texture.Get(),foregroundView.Get(),true,menu,requested,0,0),"actual AMD source Present with completed foreground");
            const auto status=host.Status();callbacks+=status.callback.invocations;++sourceTransactions;
            if(status.decision.generate)resetGeneration|=status.decision.reset;
            Require(draws<=drawCount+1,"AMD SDK presents cannot run another source effects pass");
            Require(automaticDraws==automaticCount && automaticRuntimes==0,"AMD output workers run no ReShade effect/input runtime");
            Require(Pixel(device,context,hud.texture.Get(),outputWidth-1,outputHeight-1)==hudPixel,"AMD preserves completed premultiplied HUD pixels");++uiChecks;
            const auto foregroundAfter=Pixel(device,context,foreground.texture.Get());
            if(foregroundAfter!=foregroundPixel)std::printf("Foreground changed source=%llu before=%u,%u,%u,%u after=%u,%u,%u,%u\n",
                static_cast<unsigned long long>(generationFrame.sourceId),foregroundPixel[0],foregroundPixel[1],foregroundPixel[2],foregroundPixel[3],
                foregroundAfter[0],foregroundAfter[1],foregroundAfter[2],foregroundAfter[3]);
            Require(foregroundAfter==foregroundPixel,"AMD foreground publication preserves producer pixels");
            if(!requested){++disabledSources;Require(!status.decision.generate && !status.callback.invocations,"FG off keeps same chain with real UI");}
            if(menu){++spatialSources;Require(!status.decision.generate && !status.callback.invocations,"spatial menu cannot generate");}
            effects.PresentCompleted();Sleep(10);
        }
        Require(stable>=3,"AMD before/after placement has exactly one probe and correct depth");
        Require(callbacks>callbacksBeforePhase && resetGeneration,"real generation resumes with reset in each resized phase");
        Require(WaitForSingleObject(cachedWaitable,2000)==WAIT_OBJECT_0,"cached AMD waitable still signals from active chain");
        if(++phase<3){
            // Exhaust old permits: a stale handle detached by context replacement
            // must not pass merely because it was signaled before resize.
#if defined(TRP_TEST_NR_FSR)
            retireNeural();
#endif
            adapter.reset();Require(bool(host.BeforeResize()),"real AMD quiescence before resize");
            while(WaitForSingleObject(cachedWaitable,0)==WAIT_OBJECT_0){}
            effects.ResetAfterRetirement();
            auto enlarged=desc;enlarged.BufferDesc.Width=640;enlarged.BufferDesc.Height=360;
            auto resize=host.Resize(enlarged);
            if(!resize)std::fprintf(stderr,"real resize: %s native=%lld\n",resize.error().message.c_str(),resize.error().nativeResult);
            else std::printf("real resize result=0x%08X render=%ux%u\n",static_cast<unsigned>(resize->result),resize->render.width,resize->render.height);
            Require(resize && SUCCEEDED(resize->result) && resize->render==Extent{largeWidth,largeHeight},"real AMD resize to larger extent");
            Require(bool(resources->CompleteStartup()),"real SR feature at larger extent");
            {
                Surface largeSource(device,largeWidth,largeHeight),largeInput(device,largeWidth,largeHeight),largeOutput(device,640,360);
                Surface largeDepth(device,largeWidth,largeHeight,DXGI_FORMAT_R32_FLOAT),largeMotion(device,largeWidth,largeHeight,DXGI_FORMAT_R16G16_FLOAT),largeHud(device,640,360);
                largeDepth.Paint(context,{.5f,0,0,0});largeMotion.Paint(context,{0,0,0,0});adapter=makeAdapter();
                effects.Configure(device,context,{640,360});effects.SetBeforeUpscaling(false);
                expectedDepthWidth=640;expectedDepthHeight=360;
                largeHud.Paint(context,ui);Check(effects.FinishUI(largeHud.texture.Get()),"larger ReShade GUI initialization");effects.PresentCompleted();
                Require(owned!=nullptr,"larger source manual runtime");owned->open_overlay(false,api::input_source::none);
                auto largeFrame=frame;largeFrame.color=largeSource.texture.Get();largeFrame.input=largeInput.texture.Get();largeFrame.output=largeOutput.texture.Get();
                largeFrame.depth=largeDepth.texture.Get();largeFrame.motion=largeMotion.texture.Get();
                largeFrame.render=largeFrame.subrect={largeWidth,largeHeight};largeFrame.display={640,360};largeFrame.motionConvention={float(largeWidth),float(largeHeight),true,false};
                const auto beforeLargeCallbacks=largerCallbacks;bool generatedAfterRestore{};
                for(unsigned sample=0;sample<40;++sample){
                    if(sample==18){
                        auto* retainedScene=host.SceneTarget11();auto* retainedSr=resources->Upscaler();
                        Require(bool(host.Suspend()),"real AMD suspension with a live generation feature");
                        Require(host.StartupPresent(0,DXGI_PRESENT_TEST)==DXGI_STATUS_OCCLUDED,"real suspended test Present is suppressed");
                        Require(bool(host.Resume()) && host.SceneTarget11()==retainedScene && resources->Upscaler()==retainedSr,
                            "real restoration without ResizeBuffers retains producer and SR feature");++suspendChecks;
                    }
                    Check(host.WaitBeforeProducer(),"larger guide producer wait");largeSource.Paint(context,scene);
                    context->CopyResource(largeInput.texture.Get(),largeSource.texture.Get());largeHud.Paint(context,ui);
                    largeFrame.sourceId=++sourceId;largeFrame.reset=sample==0;
#if defined(TRP_TEST_NR_FSR)
                    evaluateNeural(largeFrame,true,true);
#endif
#if defined(TRP_TEST_NR_FSR)
                    auto evaluated=nrPrepared.Valid()?adapter->EvaluatePrepared(largeFrame,nrPrepared):adapter->Evaluate(largeFrame);
#else
                    auto evaluated=adapter->Evaluate(largeFrame);
#endif
                    Require(evaluated && *evaluated==UpscaleOutcome::Temporal,"actual temporal SR dispatch at larger extent");
                    Check(effects.Render(largeOutput.texture.Get(),largeDepth.texture.Get(),{640,360},{largeWidth,largeHeight},false),"larger ReShade effects");
                    Check(effects.FinishUI(largeHud.texture.Get()),"larger completed UI");
                    const auto sentinel=Pixel(device,context,largeHud.texture.Get(),639,359);
                    context->CopyResource(host.SceneTarget11(),largeOutput.texture.Get());
                    auto generatedFrame=largeFrame;generatedFrame.depthFormat=DXGI_FORMAT_R32_FLOAT;generatedFrame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
                    generatedFrame.colorIsLinear=true;generatedFrame.reset|=adapter->LastTemporalReset();
                    Check(host.Present(generatedFrame,*evaluated,largeHud.texture.Get(),nullptr,true,false,true,0,0),"larger actual source/FG/UI transaction");
                    const auto largerStatus=host.Status();largerCallbacks+=largerStatus.callback.invocations;++largerSources;
                    if(sample>=18 && largerStatus.decision.generate && largerStatus.decision.reset)generatedAfterRestore=true;
                    Require(largerStatus.callback.configuredId==largeFrame.sourceId && (!largerStatus.decision.prepare || largerStatus.callback.preparedId==largeFrame.sourceId),
                        "larger configured/prepared/source correlation");
                    Require(Pixel(device,context,largeHud.texture.Get(),639,359)==sentinel && automaticRuntimes==0,"larger UI and ReShade ownership intact");
                    effects.PresentCompleted();Sleep(10);
                }
                Require(largerCallbacks>beforeLargeCallbacks && generatedAfterRestore,"generation at larger extent and after suspension reset");
                Require(WaitForSingleObject(cachedWaitable,2000)==WAIT_OBJECT_0,"cached handle signals at larger generated extent");
#if defined(TRP_TEST_NR_FSR)
                retireNeural();
#endif
                adapter.reset();Require(bool(host.BeforeResize()),"larger SDK/SR/UI readers retired");
                while(WaitForSingleObject(cachedWaitable,0)==WAIT_OBJECT_0){}
                effects.ResetAfterRetirement();
            }
            resize=host.Resize(desc);Require(resize && SUCCEEDED(resize->result) && resize->render==Extent{renderWidth,renderHeight},"real AMD resize back to source fixture extent");
            Require(host.SwapChain()==originalChain && resources->Bridge()==originalBridge,"actual ReShade/AMD chain and native device survive resize");
            UINT latency{};Check(originalChain->GetMaximumFrameLatency(&latency),"real AMD latency after resize");
            Require(latency==1,"real AMD maximum latency preserved across resize");
            Require(bool(resources->CompleteStartup()),"real SR feature restored");adapter=makeAdapter();
            effects.Configure(device,context,{outputWidth,outputHeight});
            effects.SetBeforeUpscaling(false); // The warmup below explicitly renders the output stage.
            const auto beforeWarmup=draws;
            for(unsigned warm=0;warm<300 && draws==beforeWarmup;++warm){output.Paint(context,scene);hud.Paint(context,ui);
                Check(effects.Render(output.texture.Get(),depth.texture.Get(),{outputWidth,outputHeight},{renderWidth,renderHeight},false),"resized ReShade shader warmup");
                Check(effects.FinishUI(hud.texture.Get()),"resized ReShade GUI warmup");effects.PresentCompleted();Sleep(10);}
            Require(owned && draws>beforeWarmup,"ReShade manual runtime recreated on retained AMD device");owned->open_overlay(false,api::input_source::none);
            ++resizeChecks;
        }
    }
    Require(sourceTransactions==96 && upscales==96 && uiChecks==96 && callbacks>0 && disabledSources==3 && spatialSources==3,"actual host source/effects/FG/UI observations");
    for(auto name:{L"sl.interposer.dll",L"sl.dlss.dll",L"sl.dlss_g.dll",L"nvngx_dlss.dll",L"nvngx_dlssg.dll"})Require(!GetModuleHandleW(name),"AMD fixture loads no NVIDIA runtime");
#if defined(TRP_TEST_NR_FSR)
    retireNeural();
    if(!nrRoot.empty()){
        Require(nrEvaluations==170 && nrBypasses==6,"combined real NR sources and live/menu bypasses");
        Require(nrDirect==140,"DirectNrFsrFgRetainsBeforeReShadeFallback");
        std::printf("PASS: combined NR/FSR/FG/ReShade nrEvaluations=%u nrBypasses=%u\n",nrEvaluations,nrBypasses);
    }
#endif
    owned->get_command_queue()->wait_idle();Require(bool(host.Retire()),"AMD async readers retire before SR owner release");
    effects.ResetAfterRetirement();DestroyWindow(window);
    CloseHandle(cachedWaitable);
    Require(resizeChecks==2 && largerSources==80 && suspendChecks==2,"larger/restored resize and suspension cycles observed");
    std::printf("PASS: actual AMD/ReShade host sources=%u upscales=%u generationCallbacks=%u completedUi=%u liveOff=%u spatial=%u resizeCycles=%u largerSources=%u largerCallbacks=%u suspendRestore=%u cachedWaitableSignaled=true; physical cadence unobserved\n",
        sourceTransactions,upscales,callbacks,uiChecks,disabledSources,spatialSources,resizeChecks,largerSources,largerCallbacks,suspendChecks);
}
#endif

template<class Effects> static HRESULT RequireNativeDevice(Effects& effects,IUnknown* adapter,ID3D12Device** out) {
 if constexpr(requires{effects.CreateSourceDevice(adapter,D3D_FEATURE_LEVEL_12_0,out,true);})
     return effects.CreateSourceDevice(adapter,D3D_FEATURE_LEVEL_12_0,out,true);
 else return E_NOTIMPL;
}
int main(int argc, char** argv)
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    std::setvbuf(stdout,nullptr,_IONBF,0);
    const bool unresolvedRoute=(argc==3 || argc==4) && std::string_view(argv[1])=="--unresolved-reshade";
    nrNative=argc==4 && std::string_view(argv[1])=="--fsr-reshade-nr-native";
    const bool nrRoute=nrNative || (argc==4 && std::string_view(argv[1])=="--fsr-reshade-nr");
    if(nrNative){renderWidth=outputWidth;renderHeight=outputHeight;}
    const bool fsrRoute=nrRoute || (argc==2 && std::string_view(argv[1])=="--fsr-reshade");
#if defined(TRP_TEST_NR_FSR)
    if(nrRoute){
        Require(!NrRuntimeResearch::GameRunningOrUnknown(),"Skyrim closed before combined GPU probe");
        nrRoot=std::filesystem::absolute(std::filesystem::u8path(argv[2]));nrCore=std::filesystem::u8path(argv[3]);
    }
#else
    Require(!nrRoute,"combined NR not built");
#endif
    const bool ordinaryRoute = argc == 2 && std::string_view(argv[1]) == "--ordinary-reshade";
    const bool xessFirstRoute = argc == 3 && std::string_view(argv[1]) == "--xess-first";
    const bool requireReShade = fsrRoute || ordinaryRoute || xessFirstRoute || (argc == 2 && std::string_view(argv[1]) == "--require-reshade");
    Require(argc == 1 || requireReShade || unresolvedRoute, "supported arguments");
    WNDCLASSW wc{}; wc.hInstance = GetModuleHandleW(nullptr); wc.lpfnWndProc = DefWindowProcW; wc.lpszClassName = L"TRPReShadeFixture";
    RegisterClassW(&wc);
    HWND window = CreateWindowW(wc.lpszClassName, L"TRP offline ReShade fixture", WS_POPUP, 0, 0, outputWidth, outputHeight, nullptr, nullptr, wc.hInstance, nullptr);
    Require(window != nullptr, "hidden fixture window");
    ComPtr<IDXGIFactory4> factory; Check(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "factory");
    ComPtr<ID3D11Device> device; ComPtr<ID3D11DeviceContext> context;
    Check(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
        &device, nullptr, &context), "D3D11 device");
    if(unresolvedRoute) {
        auto module=LoadLibraryW(std::filesystem::path(argv[2]).c_str());Require(module!=nullptr,"unresolved public-API fixture module");
        auto& effects=ReShadeIntegration::Get();effects.Discover(window);
        ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;Check(device.As(&dxgi),"unresolved adapter query");Check(dxgi->GetAdapter(&adapter),"unresolved adapter");
        ComPtr<ID3D12Device> strict;
        Require(effects.CreateSourceDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,&strict,true)==E_NOINTERFACE && !strict,
            "recognized unresolved ReShade injector rejects AMD ownership before publication");
        ComPtr<ID3D12Device> legacy;Check(effects.CreateSourceDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,&legacy),"legacy non-strict creation remains available");
#if defined(TRP_TEST_XESS_OWNER)
        Require(argc==4,"unresolved XeSS test requires official runtime root");
        TheosRenderPipeline::Upscaling::XessHostResources sr(std::filesystem::absolute(argv[3]),
            +[](IUnknown* adapter,D3D_FEATURE_LEVEL level,ID3D12Device** out)->HRESULT {
                return ReShadeIntegration::Get().CreateSourceDevice(adapter,level,out,false);
            });
        const auto srExtent=sr.Initialize(device.Get(),TheosRenderPipeline::Upscaling::Quality::NativeAA,{outputWidth,outputHeight},
            TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22);
        Require(bool(srExtent),"known injector without usable addon API retains actual XeSS SR-only startup");
        Require(bool(sr.Retire()),"unresolved injector XeSS owner retires safely");
#endif
        FreeLibrary(module);DestroyWindow(window);std::puts("PASS: unresolved native ownership rejected; legacy creation preserved");return 0;
    }
    auto& effects = ReShadeIntegration::Get(); effects.Discover(window); effects.Configure(device.Get(), context.Get(), {outputWidth, outputHeight});
#if defined(TRP_TEST_XESS_OWNER)
    if(xessFirstRoute) {
        using namespace TheosRenderPipeline::Upscaling;
        XessHostResources source(std::filesystem::absolute(argv[2]),
            +[](IUnknown* adapter,D3D_FEATURE_LEVEL level,ID3D12Device** output)->HRESULT {
                return ReShadeIntegration::Get().CreateSourceDevice(adapter,level,output,true);
            });
        const auto extent=source.Initialize(device.Get(),Quality::NativeAA,{outputWidth,outputHeight},ColorEncoding::Gamma22);
        Require(bool(extent),"actual XeSS first-device startup");
        wchar_t executable[32768]{};Require(GetModuleFileNameW(nullptr,executable,32768)!=0,"FSR fixture root");
        FsrHostResources fg(std::filesystem::path(executable).parent_path(),
            +[](IUnknown* adapter,D3D_FEATURE_LEVEL level,ID3D12Device** output)->HRESULT {
                return ReShadeIntegration::Get().CreateSourceDevice(adapter,level,output,true);
            });
        const auto sized=fg.PrepareExternalSizing(device.Get(),*extent,{outputWidth,outputHeight},DXGI_FORMAT_R8G8B8A8_UNORM,ColorEncoding::Gamma22,{});
        if(!sized)std::printf("XeSS-first FSR failure: %s native=%lld\n",sized.error().message.c_str(),sized.error().nativeResult);
        Require(bool(sized),"FSR acquires native ownership after actual XeSS initialization");
        Require(bool(fg.CompleteExternalStartup()),"actual external FG guide allocations");
        Require(TheosRenderPipeline::D3D11FrameCopy::SameObject(source.Bridge()->Device12(),fg.Bridge()->Device12()),
            "XeSS and FSR owners use the same native adapter device identity");
        Surface sampleColor(device.Get(),extent->width,extent->height);
        Surface sampleDepth(device.Get(),extent->width,extent->height,DXGI_FORMAT_R32_FLOAT);
        Surface sampleMotion(device.Get(),extent->width,extent->height,DXGI_FORMAT_R16G16_FLOAT);
        Surface delivered(device.Get(),outputWidth,outputHeight);
        sampleColor.Paint(context.Get(),{.25f,.5f,.25f,1});sampleDepth.Paint(context.Get(),{.5f,0,0,0});sampleMotion.Paint(context.Get(),{0,0,0,0});
        FsrColorConverter encode;SdrSharpeningPass sharp;
        Check(sharp.Initialize(device.Get(),TRP_TEST_RCAS_SHADER),"actual ReShade source sharpening initialization");
        for(std::uint64_t id=1;id<=2;++id) {
            Require(bool(source.PrepareInput(sampleColor.texture.Get(),sampleDepth.texture.Get(),sampleMotion.texture.Get())),"real XeSS input preparation on native owner");
            UpscaleFrame frame{};frame.backend=BackendKind::Xess;frame.render=frame.subrect=*extent;frame.display={outputWidth,outputHeight};
            frame.sourceId=id;frame.sourceEpoch=1;frame.reset=id==1;frame.depth=sampleDepth.texture.Get();frame.motion=sampleMotion.texture.Get();
            frame.colorIsLinear=true;frame.colorFormat=DXGI_FORMAT_R16G16B16A16_FLOAT;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
            frame.motionConvention={float(extent->width),float(extent->height),true,false};
            auto bridge=source.Bridge();Check(bridge->SignalProducer(),"native XeSS producer");ID3D12GraphicsCommandList* list{};
            Check(bridge->Begin(&list),"native XeSS recording");Require(bool(source.Upscaler()->Dispatch(list,source.Resources(),frame)),"actual XeSS execute with ReShade native ownership");
            Check(bridge->Submit(),"native XeSS submission");Check(bridge->WaitConsumer(),"native XeSS delivery");
            Check(encode.Convert(context.Get(),source.Output11(),delivered.texture.Get(),ColorEncoding::Linear,ColorEncoding::Gamma22),"native XeSS reconstructed SDR delivery");
            Check(sharp.Apply(context.Get(),delivered.texture.Get(),id==1?0.f:1.f),"actual ReShade XeSS output sharpening off/on");
            const auto pixel=Pixel(device.Get(),context.Get(),delivered.texture.Get(),outputWidth/2,outputHeight/2);
            Require(std::abs(int(pixel[0])-64)<=3 && std::abs(int(pixel[1])-128)<=3 && std::abs(int(pixel[2])-64)<=3,
                "actual retained-native XeSS reconstruction preserves scene colors");
        }
        ComPtr<ID3D12Fence> fence;ComPtr<ID3D12Device> native;
        Check(source.Bridge()->Device12()->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&fence)),"native XeSS fence");
        Check(fence->GetDevice(IID_PPV_ARGS(&native)),"fence native owner");
        Require(TheosRenderPipeline::D3D11FrameCopy::SameObject(source.Bridge()->Device12(),native.Get()),
            "retained source handle matches a native child GetDevice identity");
        effects.ResetAfterRetirement(); // Sized/runtime reset must not lose the native anchor while SDK owners remain live.
        ComPtr<IDXGIDevice> dxgi;ComPtr<IDXGIAdapter> adapter;
        Check(device.As(&dxgi),"repeat adapter query");Check(dxgi->GetAdapter(&adapter),"repeat adapter");
        ComPtr<ID3D12Device> repeated;Check(effects.CreateSourceDevice(adapter.Get(),D3D_FEATURE_LEVEL_12_0,&repeated,true),"retained device after runtime reset");
        Require(TheosRenderPipeline::D3D11FrameCopy::SameObject(repeated.Get(),native.Get()),"reset preserves live native anchor");
        ComPtr<ID3D12Device> unsupported;
        Require(FAILED(effects.CreateSourceDevice(adapter.Get(),static_cast<D3D_FEATURE_LEVEL>(0xffff),&unsupported,true)) && !unsupported,
            "retained native anchor never bypasses unsupported feature level checks");
        Require(bool(fg.Retire()) && bool(source.Retire()),"both owners retire their own queues and resources");
        effects.ResetAfterRetirement();DestroyWindow(window);
        std::puts("PASS: actual XeSS-first / ReShade / FSR owner startup, native identity, two SDK executions, color delivery and retirement");return 0;
    }
#else
    Require(!xessFirstRoute,"XeSS-first fixture requires compiled XeSS ownership");
#endif
    effects.SetBeforeUpscaling(false);
    Surface color(device.Get(), outputWidth, outputHeight), earlyColor(device.Get(), renderWidth, renderHeight), depth(device.Get(), renderWidth, renderHeight, DXGI_FORMAT_R32_FLOAT), ui(device.Get(), outputWidth, outputHeight);
    const std::array<float, 4> scene{0.25f, 0.5f, 0.25f, 1};
    color.Paint(context.Get(), scene); ui.Paint(context.Get(), scene); depth.Paint(context.Get(), {0.5f, 0, 0, 0});
    // Device creation is required even when the optional injector is absent.
    ComPtr<IDXGIDevice> dxgi; ComPtr<IDXGIAdapter> adapter; Check(device.As(&dxgi), "DXGI device"); Check(dxgi->GetAdapter(&adapter), "adapter");
    Require(effects.CreateSourceDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, nullptr) == E_POINTER, "null device output rejected");
    ComPtr<ID3D12Device> device12;
    if(!fsrRoute)Check(ordinaryRoute ? D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device12)) :
        RequireNativeDevice(effects,adapter.Get(), &device12), "D3D12 device");
    Require(fsrRoute || device12.Get() != nullptr, "successful creation returns a device");
    D3D12_COMMAND_QUEUE_DESC queueDesc{}; ComPtr<ID3D12CommandQueue> queue; if(!fsrRoute)Check(device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&queue)), "queue");
    if (!requireReShade) {
        Require(effects.Status() == "ReShade not loaded", "absence fixture must not load an injector");
        ComPtr<ID3D12Fence> ready; Check(device12->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&ready)), "absent runtime fence");
        ComPtr<ID3D12Device> fenceDevice; Check(ready->GetDevice(IID_PPV_ARGS(&fenceDevice)), "absent fence device");
        Require(TheosRenderPipeline::D3D11FrameCopy::SameObject(device12.Get(), fenceDevice.Get()), "absent runtime preserves native device identity");
        Check(queue->Signal(ready.Get(), 1), "absent queue signal");
        HANDLE complete = CreateEventW(nullptr, FALSE, FALSE, nullptr); Require(complete != nullptr, "absent fence event");
        Check(ready->SetEventOnCompletion(1, complete), "absent fence completion");
        Require(WaitForSingleObject(complete, 30000) == WAIT_OBJECT_0, "absent queue completes work"); CloseHandle(complete);
        Require(effects.Render(color.texture.Get(), depth.texture.Get(), {outputWidth, outputHeight}, {renderWidth, renderHeight}, false) == S_FALSE, "absent stage is inert");
        Require(effects.FinishUI(ui.texture.Get()) == S_FALSE, "absent GUI is inert");
        ReShadeIntegration::ScreenshotRequest shot;
        Require(!effects.TakeScreenshotRequest(shot), "absent ReShade queues no screenshot replacement");
        Require(Pixel(device.Get(), context.Get(), color.texture.Get())[0] == 64, "absence leaves pixels unchanged");
        effects.ResetAfterRetirement(); DestroyWindow(window); std::puts("PASS: absent ReShade creates a working native device and leaves effects inert"); return 0;
    }

    auto module = GetModuleHandleW(L"dxgi.dll");
    const auto reg = reinterpret_cast<void (*)(reshade::addon_event, void*)>(GetProcAddress(module, "ReShadeRegisterEvent"));
    Require(reg != nullptr, "actual ReShade exports");
    reg(reshade::addon_event::init_effect_runtime, reinterpret_cast<void*>(&Init));
    reg(reshade::addon_event::destroy_effect_runtime, reinterpret_cast<void*>(&Destroy));
    reg(reshade::addon_event::reshade_begin_effects, reinterpret_cast<void*>(&Draw));

    if(fsrRoute) {
#if defined(TRP_ENABLE_FSR_FG)
        FsrReShadeHost(window,factory.Get(),device.Get(),context.Get());return 0;
#else
        Require(false,"FSR FG not built");
#endif
    }
    // The production device factory keeps this D3D12 output chain free of
    // automatic effect/input runtimes. It presents four times per source frame.
    DXGI_SWAP_CHAIN_DESC1 desc{}; desc.Width = outputWidth; desc.Height = outputHeight; desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1; desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; desc.BufferCount = 2; desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    TheosRenderPipeline::OrdinaryPresentation ordinary;
    ComPtr<IDXGISwapChain> output;
    if (ordinaryRoute) {
        DXGI_SWAP_CHAIN_DESC ordinaryDesc{}; ordinaryDesc.BufferDesc.Width=outputWidth; ordinaryDesc.BufferDesc.Height=outputHeight;
        ordinaryDesc.BufferDesc.Format=desc.Format; ordinaryDesc.SampleDesc.Count=1;
        ordinaryDesc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT; ordinaryDesc.BufferCount=2;
        ordinaryDesc.OutputWindow=window; ordinaryDesc.Windowed=TRUE;
        creatingOrdinary=true;
        Check(ordinary.CreateSwapChain(factory.Get(),device.Get(),ordinaryDesc,&output,&IDXGIFactory::CreateSwapChain), "production ordinary swapchain");
        creatingOrdinary=false;
    } else {
        ComPtr<IDXGISwapChain1> chain;
        Check(factory->CreateSwapChainForHwnd(queue.Get(), window, &desc, nullptr, nullptr, &chain), "output swapchain");
        Check(chain.As(&output), "output interface");
    }

    Require(automaticRuntimes == 0, "native output creates no automatic ReShade runtime");
    if(ordinaryRoute) {
        TheosRenderPipeline::Graphics::D3D11D3D12Interop bridge;
        ComPtr<ID3D11Device> presenterDevice;
        Check(TheosRenderPipeline::AcquirePresentationDevice(output.Get(),device.Get(),true,presenterDevice),"ordinary source device acquisition");
        Check(bridge.Initialize(presenterDevice.Get(),device12.Get(),queue.Get()),"ordinary FSR sharing bridge");
        ComPtr<ID3D11Device> contextOwner,resourceOwner;
        bridge.Context11()->GetDevice(&contextOwner);color.texture->GetDevice(&resourceOwner);
        std::printf("FSR ownership: immediate=%p bridge=%p contextDevice=%p resourceDevice=%p same=%d\n",
            context.Get(),bridge.Context11(),contextOwner.Get(),resourceOwner.Get(),
            TheosRenderPipeline::D3D11FrameCopy::SameObject(contextOwner.Get(),resourceOwner.Get()));
        TheosRenderPipeline::Upscaling::FsrColorConverter spatial;
        const auto converted=spatial.Convert(bridge.Context11(),color.texture.Get(),ui.texture.Get(),
            TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22);
        std::printf("FSR spatial conversion: result=0x%08lX stage=%s\n",static_cast<unsigned long>(converted),spatial.FailureStage());
        Check(converted,"ordinary ReShade FSR spatial handoff");
        const auto pixel=Pixel(device.Get(),context.Get(),ui.texture.Get());
        std::printf("FSR spatial pixels: %u %u %u %u\n",pixel[0],pixel[1],pixel[2],pixel[3]);
        const auto matchesScene=[](const std::array<unsigned char,4>& value) {
            constexpr std::array<int,4> expected{64,128,64,255};
            for(std::size_t i=0;i<value.size();++i)if(std::abs(int(value[i])-expected[i])>1)return false;
            return true; // Gamma decode/encode permits one RGBA8 quantization step.
        };
        Require(matchesScene(pixel),"ordinary wrapped FSR spatial pixels");
        ComPtr<ID3D11Device> nativeDevice;
        Check(TheosRenderPipeline::AcquirePresentationDevice(output.Get(),device.Get(),false,nativeDevice),"indexed presenter retains swapchain device selection");
        Require(!TheosRenderPipeline::D3D11FrameCopy::SameObject(nativeDevice.Get(),presenterDevice.Get()),
            "real ReShade native chain exposes a distinct device identity");
        auto desc=TheosRenderPipeline::Upscaling::FsrPreparedTextureDesc(TheosRenderPipeline::Upscaling::FsrResourceRole::Color,{outputWidth,outputHeight});
        TheosRenderPipeline::Graphics::SharedTexture linear;
        Check(bridge.CreateSharedTexture(desc,linear),"wrapped FSR prepared shared color");
        TheosRenderPipeline::Upscaling::FsrColorConverter decode,encode;
        Check(decode.Convert(bridge.Context11(),color.texture.Get(),linear.texture11.Get(),
            TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,TheosRenderPipeline::Upscaling::ColorEncoding::Linear),"wrapped FSR input decode");
        Check(encode.Convert(bridge.Context11(),linear.texture11.Get(),ui.texture.Get(),
            TheosRenderPipeline::Upscaling::ColorEncoding::Linear,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22),"wrapped FSR output encode");
        Require(matchesScene(Pixel(device.Get(),context.Get(),ui.texture.Get())),"wrapped shared color retains round-trip pixels");
        ComPtr<ID3D11Device> foreignDevice;ComPtr<ID3D11DeviceContext> foreignContext;
        Check(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,
            &foreignDevice,nullptr,&foreignContext),"foreign D3D11 device on same adapter");
        Surface foreign(foreignDevice.Get(),outputWidth,outputHeight);
        Require(spatial.Convert(bridge.Context11(),foreign.texture.Get(),ui.texture.Get(),
            TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22,TheosRenderPipeline::Upscaling::ColorEncoding::Gamma22)==E_INVALIDARG,
            "genuinely foreign D3D11 resource remains rejected");
        Check(bridge.Drain(),"ordinary FSR bridge retirement");
    }
    ComPtr<ID3D12Device> foreign; ComPtr<ID3D12Fence> foreignFence;
    Check(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&foreign)), "unrelated D3D12 creation");
    Check(foreign->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&foreignFence)), "unrelated fence");
    ComPtr<ID3D12Device> foreignNative; Check(foreignFence->GetDevice(IID_PPV_ARGS(&foreignNative)), "unrelated native identity");
    Require(!TheosRenderPipeline::D3D11FrameCopy::SameObject(foreign.Get(), foreignNative.Get()), "unrelated D3D12 device retains ReShade wrapping");
    foreignFence.Reset(); foreignNative.Reset(); foreign.Reset();
    for (unsigned frame = 0; frame < 300 && !draws; ++frame) {
        color.Paint(context.Get(), scene); ui.Paint(context.Get(), scene);
        Check(effects.Render(color.texture.Get(), depth.texture.Get(), {outputWidth, outputHeight}, {renderWidth, renderHeight}, false), "warm-up effect stage");
        Check(effects.FinishUI(ui.texture.Get()), "warm-up runtime update");
        effects.PresentCompleted(); Sleep(10);
    }
    Require(owned && draws, "actual effect compiled and executed");
    owned->open_overlay(false, api::input_source::none);
    // Validate both placements and live switching. The shader adds 1/8 to red
    // and reads our depth into green; a duplicate render adds another 1/8.
    for (bool before : {false, true, false}) {
        effects.SetBeforeUpscaling(before);
        auto& target = before ? earlyColor : color;
        const TheosRenderPipeline::FrameExtent extent = before ? TheosRenderPipeline::FrameExtent{renderWidth, renderHeight} : TheosRenderPipeline::FrameExtent{outputWidth, outputHeight};
        expectedDepthWidth = extent.width; expectedDepthHeight = extent.height;
        bool validated{}; unsigned stableFrames{};
        for (unsigned frame = 0; frame < 300 && !validated; ++frame) {
            target.Paint(context.Get(), scene); ui.Paint(context.Get(), scene);
            const auto originalUI=Pixel(device.Get(),context.Get(),ui.texture.Get(),outputWidth-1,outputHeight-1);
            const auto initial = draws;
            Require(effects.Render(target.texture.Get(), depth.texture.Get(), extent, {renderWidth, renderHeight}, !before) == S_FALSE, "unselected placement is inert");
            Check(effects.Render(target.texture.Get(), depth.texture.Get(), extent, {renderWidth, renderHeight}, before), "selected placement");
            Require(effects.Render(target.texture.Get(), depth.texture.Get(), extent, {renderWidth, renderHeight}, before) == S_FALSE, "source-frame deduplication");
            const auto uiBeforeGui=Pixel(device.Get(),context.Get(),ui.texture.Get(),outputWidth-1,outputHeight-1);
            Require(uiBeforeGui==originalUI, "effects leave native UI sentinel unchanged");
            Check(effects.FinishUI(ui.texture.Get()), "GUI update");
            Require(effects.FinishUI(ui.texture.Get()) == S_FALSE, "GUI update deduplicated");
            if (draws != initial) {
                const auto pixel = Pixel(device.Get(), context.Get(), target.texture.Get(), extent.width - 1, extent.height - 1);
                std::printf("placement=%s pixel=%u,%u,%u,%u draws=%u\n", before ? "before" : "after", pixel[0], pixel[1], pixel[2], pixel[3], draws - initial);
                Require(depthExtentMatched, "DEPTH uses the selected stage extent and R32_FLOAT");
                const bool correct = pixel[0] >= 95 && pixel[0] <= 97 && pixel[1] >= 127 && pixel[1] <= 129 && pixel[2] >= 190 && pixel[2] <= 192;
                stableFrames = correct ? stableFrames + 1 : 0;
                Require(draws - initial == 1, "one effect invocation per source frame");
                validated = stableFrames >= 3;
            } else { stableFrames = 0; }
            const auto outputDraws = automaticDraws;
            const auto uiBeforePresent=Pixel(device.Get(),context.Get(),ui.texture.Get(),outputWidth-1,outputHeight-1);
            for (unsigned i = 0; i < 4; ++i) { Check(output->Present(0, 0), "downstream output Present"); }
            Require(automaticRuntimes == 0, "output remains free of automatic runtimes");
            Require(automaticDraws == outputDraws, "actual output runtime records no effect draws");
            const auto uiPixel=Pixel(device.Get(),context.Get(),ui.texture.Get(),outputWidth-1,outputHeight-1);
            Require(uiPixel==uiBeforePresent, "downstream presents preserve completed native UI");
            effects.PresentCompleted(); Sleep(10);
        }
        Require(validated, "placement switch completed shader reload");
    }
    PostMessageW(window, WM_KEYDOWN, VK_F8, 1);
    MSG message{};
    while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) { DispatchMessageW(&message); }
    const auto keyBefore = owned->last_key_pressed();
    for (unsigned i = 0; i < 4; ++i) { Check(output->Present(0, 0), "output Present before source input"); }
    std::printf("pending F8 before output=%u after output=%u\n", keyBefore, owned->last_key_pressed());
    Require(keyBefore == VK_F8 && owned->last_key_pressed() == VK_F8, "output frames must not consume source overlay input");
    Check(effects.FinishUI(ui.texture.Get()), "F8 input update"); effects.PresentCompleted();
    Require(!owned->get_effects_state(), "configured F8 toggles effects off");
    PostMessageW(window, WM_KEYUP, VK_F8, (1u << 31) | (1u << 30) | 1);
    while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) { DispatchMessageW(&message); }
    Check(effects.FinishUI(ui.texture.Get()), "key release update"); effects.PresentCompleted();
    auto pressHome = [&] {
        PostMessageW(window, WM_KEYDOWN, VK_HOME, 1);
        while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) { DispatchMessageW(&message); }
        for (unsigned i = 0; i < 4; ++i) { Check(output->Present(0, 0), "output before Home"); }
        Check(effects.FinishUI(ui.texture.Get()), "Home input update"); effects.PresentCompleted();
        PostMessageW(window, WM_KEYUP, VK_HOME, (1u << 31) | (1u << 30) | 1);
        while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) { DispatchMessageW(&message); }
        Check(effects.FinishUI(ui.texture.Get()), "Home release update"); effects.PresentCompleted();
    };
    pressHome(); Require(effects.OverlayOpen(), "configured Home opens the owned overlay");
    pressHome(); Require(!effects.OverlayOpen(), "configured Home closes the owned overlay");
    // The owned runtime saves its UI-layer back buffer; the saved path must
    // reach the presenter's queue exactly once for final-frame replacement.
    ReShadeIntegration::ScreenshotRequest shot;
    Require(!effects.TakeScreenshotRequest(shot), "no screenshot request before the key");
    PostMessageW(window, WM_KEYDOWN, VK_F9, 1);
    while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) { DispatchMessageW(&message); }
    for (unsigned i = 0; i < 4; ++i) { Check(output->Present(0, 0), "output before screenshot key"); }
    Check(effects.FinishUI(ui.texture.Get()), "screenshot key update"); effects.PresentCompleted();
    PostMessageW(window, WM_KEYUP, VK_F9, (1u << 31) | (1u << 30) | 1);
    while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) { DispatchMessageW(&message); }
    bool requested{};
    for (unsigned i = 0; i < 1000 && !requested; ++i) {
        Check(effects.FinishUI(ui.texture.Get()), "screenshot save update");
        requested = effects.TakeScreenshotRequest(shot);
        effects.PresentCompleted();
        if (!requested) { Sleep(10); }
    }
    Require(requested, "owned runtime screenshot is queued after ReShade saves it");
    const auto shotPath = std::filesystem::u8path(shot.path);
    std::printf("screenshot request=%s quality=%d\n", shot.path.c_str(), shot.jpegQuality);
    Require(std::filesystem::exists(shotPath) && shotPath.extension() == ".png" && shotPath.stem() == "TRPProbe",
        "request names ReShade's saved file");
    Require(shot.jpegQuality == 90, "unset JPEG quality uses ReShade's default");
    Require(shot.replaceAllowed, "closed-overlay screenshot permits final-frame replacement");
    for (unsigned i = 0; i < 20; ++i) { Check(effects.FinishUI(ui.texture.Get()), "post-screenshot update"); effects.PresentCompleted(); Sleep(5); }
    Require(!effects.TakeScreenshotRequest(shot), "one key press queues one replacement");
    std::filesystem::remove(shotPath);
    // A normal ReShade screenshot deliberately excludes its GUI. Its file must
    // survive when our only available final frame includes the open overlay.
    owned->open_overlay(true, api::input_source::none);
    Require(effects.OverlayOpen(), "open overlay for screenshot preservation");
    PostMessageW(window, WM_KEYDOWN, VK_F9, 1);
    while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) { DispatchMessageW(&message); }
    Check(effects.FinishUI(ui.texture.Get()), "open-overlay screenshot key"); effects.PresentCompleted();
    PostMessageW(window, WM_KEYUP, VK_F9, (1u << 31) | (1u << 30) | 1);
    while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) { DispatchMessageW(&message); }
    requested = false;
    for (unsigned i = 0; i < 1000 && !requested; ++i) {
        Check(effects.FinishUI(ui.texture.Get()), "open-overlay screenshot update");
        requested = effects.TakeScreenshotRequest(shot);
        effects.PresentCompleted();
        if (!requested) { Sleep(10); }
    }
    Require(requested && !shot.replaceAllowed, "GUI-inclusive frame must not overwrite a normal screenshot");
    Require(std::filesystem::exists(std::filesystem::u8path(shot.path)), "ReShade original still exists for rejected replacement");
    std::filesystem::remove(std::filesystem::u8path(shot.path));
    owned->open_overlay(false, api::input_source::none);
    owned->set_effects_state(true);
    effects.SetBeforeUpscaling(false);
    color.Paint(context.Get(), scene);
    const auto beforeToggle = draws;
    owned->set_effects_state(false);
    Check(effects.Render(color.texture.Get(), depth.texture.Get(), {outputWidth, outputHeight}, {renderWidth, renderHeight}, false), "disabled effects");
    Require(Pixel(device.Get(), context.Get(), color.texture.Get())[0] == 64 && draws == beforeToggle, "effects toggle preserves world pixels");
    Check(effects.FinishUI(ui.texture.Get()), "effects-off GUI update"); effects.PresentCompleted();
    owned->set_effects_state(true);
    color.Paint(context.Get(), scene);
    ID3D11RenderTargetView* sentinel = ui.rtv.Get(); context->OMSetRenderTargets(1, &sentinel, nullptr);
    const D3D11_VIEWPORT viewport{3, 4, 27, 19, 0.125f, 0.75f}; context->RSSetViewports(1, &viewport);
    Check(effects.Render(color.texture.Get(), nullptr, {outputWidth, outputHeight}, {}, false), "menu frame clears depth semantic");
    ComPtr<ID3D11RenderTargetView> restored; context->OMGetRenderTargets(1, &restored, nullptr);
    D3D11_VIEWPORT restoredViewport{}; UINT count = 1; context->RSGetViewports(&count, &restoredViewport);
    Require(restored.Get() == sentinel && restoredViewport.TopLeftX == 3 && restoredViewport.Width == 27 && restoredViewport.MaxDepth == 0.75f, "effect stage restores producer MRT/viewport state");
    const auto menuPixel = Pixel(device.Get(), context.Get(), color.texture.Get());
    std::printf("menu pixel=%u,%u,%u,%u draws=%u\n", menuPixel[0], menuPixel[1], menuPixel[2], menuPixel[3], draws);
    Require(menuPixel[0] >= 95 && menuPixel[0] <= 97 && menuPixel[1] == 0, "menu does not reuse stale world depth");
    Check(effects.FinishUI(ui.texture.Get()), "menu GUI update"); effects.PresentCompleted();
    context->ClearState();
    Require(owned->open_overlay(true, api::input_source::none), "open ReShade overlay");
    Require(effects.OverlayOpen(), "overlay capture opens");
    Require(owned->open_overlay(false, api::input_source::none), "close ReShade overlay");
    Require(!effects.OverlayOpen(), "overlay capture releases");
    owned->get_command_queue()->wait_idle();
    Check(ordinary.Retire(), "ordinary final readers retired");
    effects.ResetAfterRetirement();
    Require(!effects.OverlayOpen(), "retirement releases capture");
    effects.Configure(device.Get(), context.Get(), {outputWidth, outputHeight});
    Check(effects.FinishUI(ui.texture.Get()), "runtime recreated after retirement");
    owned->get_command_queue()->wait_idle(); effects.ResetAfterRetirement();
    std::puts("PASS: actual ReShade placement, depth, source/output ownership, overlay and runtime lifecycle");
    // The output chain owns asynchronous D3D12 work; wait before releasing it.
    ComPtr<ID3D12Fence> fence; Check(device12->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "retirement fence");
    Check(queue->Signal(fence.Get(), 1), "retirement signal");
    HANDLE event = CreateEventW(nullptr, FALSE, FALSE, nullptr); Check(fence->SetEventOnCompletion(1, event), "retirement event");
    Require(WaitForSingleObject(event, 30000) == WAIT_OBJECT_0, "output retirement"); CloseHandle(event);
    output.Reset(); DestroyWindow(window);
}
