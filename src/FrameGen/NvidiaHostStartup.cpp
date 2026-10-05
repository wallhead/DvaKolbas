#include "DLSSBackend.h"
#include "RenderPipeline.h"
#include "GameSwapChain.h"
#include "NativeInput.h"
#include "NvidiaHost.h"
#include "CommunityShaderIntegration.h"
#include "SourceDLSSGBackend.h"
#include "SourceDLSSGCamera.h"
#include "SourceFrameGeneration.h"
#include "NeuralRenderingMode.h"
#include "RendererBackendPolicy.h"
#include "PresentationPolicy.h"
#include "PresentationDevice.h"
#include "FSRSwapChainPolicy.h"
#include "PluginPaths.h"
#include "NeuralRendering/SourcePolicy.h"
#include <PCH.h>

HRESULT NvidiaHost::CreateSwapChain(IDXGIFactory* a_factory, ID3D11Device* a_device, DXGI_SWAP_CHAIN_DESC* a_desc, IDXGISwapChain** a_swapChain,
    TheosRenderPipeline::OriginalCreateSwapChain original)
{
    if (FAILED(FailureResult())) { return FailureResult(); }
    if (!a_factory || !a_device || !a_desc || !a_swapChain)
    {
        status_ = "NVIDIA DLSS-G swapchain inputs are incomplete";
        return E_INVALIDARG;
    }
    const auto* upscalerSettings = RenderPipeline::GetSingleton();
    sourceUpscalerSettings_.Initialize(
        {upscalerSettings->mUpscaleType, upscalerSettings->mQualityLevel, upscalerSettings->mDLSSPreset,
         upscalerSettings->mSharpening, upscalerSettings->mAutoExposure, upscalerSettings->mFsrSettings});
    const auto& generation=SourceFrameGeneration::GetSingleton()->settings;
    TheosRenderPipeline::Upscaling::BackendConfiguration requested;
    requested.adapterVendorId=upscalerSettings->mAdapterVendorId;
    requested.backend=upscalerSettings->mUpscaleType==FSR?TheosRenderPipeline::Upscaling::BackendKind::Fsr:
        upscalerSettings->mUpscaleType==DLAA?TheosRenderPipeline::Upscaling::BackendKind::Dlaa:TheosRenderPipeline::Upscaling::BackendKind::Dlss;
    requested.generationEnabled=generation.enabled;requested.generationBackend=generation.generationBackend;
    requested.quality=upscalerSettings->mFsrSettings.quality;requested.providerPolicy=upscalerSettings->mFsrSettings.providerPolicy;
    requested.sharpness=upscalerSettings->mFsrSettings.sharpness;requested.dynamicResolution=upscalerSettings->mDynamicResolutionRequested;
    requested.neuralRendering=generation.sourceDLSSG.neuralEnabled;
#if !defined(TRP_NO_NEURAL_RENDERING)
    requested.communityNeural=generation.neuralStartup.community;
#endif
    requested.hdr=generation.sourceDLSSG.hdrOutput.enabled;
#if defined(TRP_ENABLE_FSR)
    #if defined(TRP_ENABLE_FSR_FG)
    backendDecision_=TheosRenderPipeline::ResolveBackend(requested,true,true);
#else
    backendDecision_=TheosRenderPipeline::ResolveBackend(requested,true);
#endif
    if(FsrActive())fsrResources_=std::make_shared<TheosRenderPipeline::Upscaling::FsrHostResources>(TheosRenderPipeline::PluginPaths::Directory());
#else
    backendDecision_=TheosRenderPipeline::ResolveBackend(requested,false);
#endif
    if (FsrFgActive() && (TheosRenderPipeline::CommunityShaders::Active() || !upscalerSettings->mNativeUI || generation.nativeUICompositionMode != 0)) {
        status_ = "FSR FG requires TRP-owned upscaling and dedicated NativeUI=true, composition mode 0";
        return E_INVALIDARG;
    }
    if(!backendDecision_.valid){status_=backendDecision_.diagnostic;return E_INVALIDARG;}
    logger::info("[SourceUpscaler] startup size authority=QualityLevel mode={} "
                 "quality={}",
                 upscalerSettings->mUpscaleType, upscalerSettings->mQualityLevel);

    splitSourceRuntimeFailureLogged_ = false;

    // Preserve the tested two-buffer native presentation contract. The outer
    // wrapper publishes one stable render-sized buffer independently of it.
    outputWindow_ = a_desc->OutputWindow;
    TheosRenderPipeline::ReShadeIntegration::Get().Discover(outputWindow_);
    logger::info("[ReShade] {}", TheosRenderPipeline::ReShadeIntegration::Get().Status());
    const auto requestedBufferCount = a_desc->BufferCount;
    a_desc->BufferCount = 2;
    logger::info("[NvidiaHost] normalized swapchain buffer count requested={} proxy=2", requestedBufferCount);

    // Start generation disabled
    // before proxy creation so an undocumented backend default cannot affect the
    // first proxied Present while frame inputs are still unavailable.
    SetRuntimeEnabled(false);

    auto createNvidia=[&]()->HRESULT {
    const auto& settings = SourceFrameGeneration::GetSingleton()->settings;
    auto& backend = TheosRenderPipeline::SourceDLSSG::Backend::Get();
    backend.ConfigureReflex(static_cast<sl::ReflexMode>(settings.sourceDLSSG.reflexMode));
    backend.ConfigureUIRecomposition(settings.sourceDLSSG.uiRecomposition);
    backend.ConfigureOutputFPSLimit(settings.sourceDLSSG.outputFPSLimit);
    backend.ConfigureGeneration(settings.sourceDLSSG.generation);
    backend.ConfigureMFGUnlock(settings.sourceDLSSGMFGUnlock);
    backend.ConfigureHDROutput(settings.sourceDLSSG.hdrOutput);
    TheosRenderPipeline::SourceDLSSG::NeuralOptions options;
    options.runtimePath = settings.neuralRenderingRuntimePath;
    options.enabled = !settings.neuralStartup.community && settings.sourceDLSSG.neuralEnabled && !options.runtimePath.empty() &&
        TheosRenderPipeline::SupportsNeuralRenderingMode(upscalerSettings->mUpscaleType, TheosRenderPipeline::CommunityShaders::Active());
    options.tuning = settings.sourceDLSSG.neuralTuning;
    options.reconstruction = settings.sourceDLSSG.neuralReconstruction;
    options.secondPass = settings.sourceDLSSG.neuralSecondPass;
    options.beforeUpscaling = settings.sourceDLSSG.neuralBeforeUpscaling;
    options.passes = settings.sourceDLSSG.neuralPasses;
    options.combat = settings.sourceDLSSG.neuralCombat;
    backend.ConfigureNeuralRendering(std::move(options));

    return TheosRenderPipeline::SourceDLSSG::Backend::Get().CreateSwapChain(
        a_factory, a_device, *a_desc, SourceFrameGeneration::GetSingleton()->settings.sourceDLSSGStreamlineDirectory, a_swapChain);
    };
    auto createOrdinary=[&](){return ordinaryPresentation_.CreateSwapChain(a_factory,a_device,*a_desc,a_swapChain,original);};
    auto createFsr=[&]()->HRESULT {
#if defined(TRP_ENABLE_FSR_FG)
        return CreateFsrPresenter(a_factory,a_device,*a_desc,a_swapChain);
#else
        return E_NOTIMPL;
#endif
    };
    TheosRenderPipeline::PresentationCreation creation{createNvidia,createOrdinary,createFsr};
    const auto result=TheosRenderPipeline::CreatePresentation(backendDecision_,creation);
    if (FAILED(result) || !*a_swapChain)
    {
        if(!FsrFgActive())status_ = FsrActive()?"Ordinary D3D11 swapchain creation failed":TheosRenderPipeline::SourceDLSSG::Backend::Get().Status();
        return FAILED(result) ? result : E_FAIL;
    }
    innerSwapChain_ = *a_swapChain;
    nativeUIContexts_.ResetAfterRetirement();
    device_.Reset();
    context_.Reset();
    const auto deviceResult = TheosRenderPipeline::AcquirePresentationDevice(innerSwapChain_,a_device,FsrActive(),device_);
    if (FAILED(deviceResult) || !device_)
    {
        status_ = std::format("Renderer source D3D11 device selection failed (0x{:08X})", static_cast<std::uint32_t>(deviceResult));
        return E_FAIL;
    }
    device_->GetImmediateContext(&context_);
    if (!CreateGameFacingResources(*a_swapChain))
    {
        status_ = FsrActive() ? "FSR ordinary presentation game-facing buffer creation failed" :
            "NVIDIA DLSS-G stable game-facing buffer creation failed";
        (*a_swapChain)->Release();
        *a_swapChain = nullptr;
        return E_FAIL;
    }

    // Transfer the backend-created swapchain into an outer wrapper before the
    // factory hook returns. D3D11CreateDeviceAndSwapChain can cache GetBuffer(0)
    // during its own call, so installing a hook after it returns is too late.
    nativeUIPass_.Frame().NewSession();
    auto* outer = new (std::nothrow) GameSwapChain(*a_swapChain, this);
    if (!outer)
    {
        status_ = "NVIDIA DLSS-G game-facing swapchain wrapper allocation failed";
        (*a_swapChain)->Release();
        *a_swapChain = nullptr;
        return E_OUTOFMEMORY;
    }
    (*a_swapChain)->Release();
    outerSwapChain_ = outer;
    *a_swapChain = outer;
    proxyActive_ = true;
    // Proxy construction can replace or reinitialize the live presentation
    // state. Do not trust the pre-proxy cache across that boundary. Re-issue the
    // disable against the completed proxy before its first game-facing Present.
    frameGenerationStateKnown_ = false;
    SetRuntimeEnabled(false);
    status_ = FsrFgActive()?"AMD presenter active; FSR feature deferred until device creation returns":FsrActive()?"Ordinary presenter active; FSR feature deferred until device creation returns":"NVIDIA DLSS-G proxy active; waiting for complete frame inputs";
    logger::info("[NvidiaHost] source swapchain active format={}", static_cast<std::uint32_t>(a_desc->BufferDesc.Format));
    logger::info("[NvidiaHost] outer stable-buffer swapchain returned during "
                 "factory creation");
    return S_OK;
}

bool NvidiaHost::CreateGameFacingResources(IDXGISwapChain* a_swapChain)
{
    EndNativeUIPass();
    gameTargets_.ResetGameFacingAfterRetirement();
    nativeUIPass_.ResetEvaluation();
    if (!FsrFgActive()) {
        bool retainFsrDevice{};
#if defined(TRP_ENABLE_FSR)
        retainFsrDevice=fsrSizingRetainedForResize_;
#endif
        ReleaseSourceUpscaler(retainFsrDevice);
    }
    sourceUpscalerInitializationPending_ = false;
    presentation_.ResetAfterRetirement();
    if (!a_swapChain || !device_ || !context_)
    {
        return false;
    }

    HRESULT cacheResult{};
#if defined(TRP_ENABLE_FSR_FG)
    if (FsrFgActive()) { cacheResult = presentation_.CacheSceneAfterRetirement(fsrPresentation_->SceneTarget11()); }
    else
#endif
    cacheResult = presentation_.CacheAfterRetirement(a_swapChain,
        FsrActive() ? TheosRenderPipeline::PresentationBufferAccess::D3D11Current :
            TheosRenderPipeline::PresentationBufferAccess::Indexed);
    if (FAILED(cacheResult))
    {
        logger::error("[CoreHost] inner buffer cache failed result=0x{:08X}", static_cast<std::uint32_t>(cacheResult));
        return false;
    }

    D3D11_TEXTURE2D_DESC outputDesc{};
    presentation_.Buffers().front()->GetDesc(&outputDesc);
    outputWidth_ = outputDesc.Width;
    outputHeight_ = outputDesc.Height;
    TheosRenderPipeline::NativeInput::Publish(outputWidth_, outputHeight_);
    auto* settings = RenderPipeline::GetSingleton();
    int queriedRenderWidth = 0;
    int queriedRenderHeight = 0;
    const auto sizeQuery = [](int width, int height, int quality, int* renderWidth, int* renderHeight) {
        if (TheosRenderPipeline::CommunityShaders::Active()) {
            *renderWidth = width; *renderHeight = height; return true;
        }
        return TheosRenderPipeline::SourceDLSSG::QueryRenderSize(width, height, quality, renderWidth, renderHeight);
    };
    bool sized{};
#if defined(TRP_ENABLE_FSR)
    if (FsrFgActive()) { queriedRenderWidth=renderWidth_;queriedRenderHeight=renderHeight_;sized=true; }
    else if(FsrActive() && !TheosRenderPipeline::CommunityShaders::Active()) {
        TheosRenderPipeline::Upscaling::BackendConfiguration config;
        config.backend=TheosRenderPipeline::Upscaling::BackendKind::Fsr;config.generationEnabled=false;config.generationBackend=0;
        config.quality=sourceUpscalerSettings_.Startup().fsr.quality;config.providerPolicy=sourceUpscalerSettings_.Startup().fsr.providerPolicy;
        config.sharpness=sourceUpscalerSettings_.Startup().fsr.sharpness;
        const auto encoding=sourceUpscalerSettings_.Startup().fsr.sourceColorEncoding;
        auto render=fsrSizingRetainedForResize_ ?
            fsrResources_->ResizeSizingAfterRetirement({outputWidth_,outputHeight_},outputDesc.Format) :
            fsrResources_->PrepareSizing(device_.Get(),config,{outputWidth_,outputHeight_},outputDesc.Format,encoding);
        if(!render){status_=render.error().message;logger::error("[FSR] {}",status_);return false;}
        logger::info("[FSR startup] source/output format={} sourceColorEncoding={} SDR-only contract; installed producer calibration required",
            static_cast<unsigned>(outputDesc.Format),TheosRenderPipeline::Upscaling::ColorEncodingName(encoding));
        queriedRenderWidth=render->width;queriedRenderHeight=render->height;sized=true;
    } else
#endif
    sized=sizeQuery(static_cast<int>(outputWidth_),static_cast<int>(outputHeight_),sourceUpscalerSettings_.Startup().AllocationQuality(),&queriedRenderWidth,&queriedRenderHeight);
    if (!sized ||
        queriedRenderWidth <= 0 || queriedRenderHeight <= 0 || queriedRenderWidth > static_cast<int>(outputWidth_) ||
        queriedRenderHeight > static_cast<int>(outputHeight_))
    {
        logger::error("[NvidiaHost] QueryRenderSize rejected output={}x{} "
                      "quality={} result={}x{}",
                      outputWidth_, outputHeight_, settings->mQualityLevel, queriedRenderWidth, queriedRenderHeight);
        presentation_.ResetAfterRetirement();
        return false;
    }
    renderWidth_ = static_cast<UINT>(queriedRenderWidth);
    renderHeight_ = static_cast<UINT>(queriedRenderHeight);

    const auto gameFacingDesc = TheosRenderPipeline::GameFacingTargets::GameFacingDesc(outputDesc, renderWidth_, renderHeight_);
    const auto createResult = gameTargets_.CreateGameFacingAfterRetirement(device_.Get(), outputDesc, renderWidth_, renderHeight_);
    if (FAILED(createResult) || !gameTargets_.GameFacing())
    {
        logger::error("[NvidiaHost] stable game-facing texture creation failed "
                      "{}x{} format={} result=0x{:08X}",
                      gameFacingDesc.Width, gameFacingDesc.Height, static_cast<std::uint32_t>(gameFacingDesc.Format),
                      static_cast<std::uint32_t>(createResult));
        ReleaseSourceUpscaler();
        presentation_.ResetAfterRetirement();
        return false;
    }

    if (TheosRenderPipeline::CommunityShaders::Active()) {
        evaluationFailureLogged_ = false;
        sourceUpscalerInitializationPending_ = true;
        logger::info("[CS Adapter] native game-facing buffer prepared {}x{}; CS owns its upscaling resources", outputWidth_, outputHeight_);
        return true;
    }
    const auto inputDesc = TheosRenderPipeline::GameFacingTargets::UpscaleInputDesc(outputDesc, renderWidth_, renderHeight_);
    const auto inputResult = gameTargets_.CreateUpscaleInputAfterRetirement(device_.Get(), outputDesc, renderWidth_, renderHeight_);
    if (FAILED(inputResult) || !gameTargets_.UpscaleInput())
    {
        logger::error("[NvidiaHost] source-upscale input creation failed {}x{} "
                      "format={} result=0x{:08X}",
                      inputDesc.Width, inputDesc.Height, static_cast<std::uint32_t>(inputDesc.Format), static_cast<std::uint32_t>(inputResult));
        gameTargets_.ResetGameFacingAfterRetirement();
        ReleaseSourceUpscaler();
        presentation_.ResetAfterRetirement();
        return false;
    }

    evaluationFailureLogged_ = false;
    sourceUpscalerInitializationPending_ = true;
    logger::info("[NvidiaHost] outer resources prepared render={}x{} "
                 "output={}x{} format={} innerBuffers={}; source upscaler "
                 "deferred until D3D11 device creation returns",
                 gameFacingDesc.Width, gameFacingDesc.Height, outputWidth_, outputHeight_, static_cast<std::uint32_t>(gameFacingDesc.Format),
                 presentation_.Buffers().size());
    return true;
}

bool NvidiaHost::CompleteStartupAfterDeviceCreation()
{
    if (FAILED(FailureResult())) { return false; }
    if (!proxyActive_)
    {
        return false;
    }
    if (upscalerReady_)
    {
        return true;
    }
    if (!sourceUpscalerInitializationPending_ || presentation_.Buffers().empty())
    {
        return false;
    }

    // Preserve the tested startup boundary: publish the stable render-sized
    // buffer during factory creation, then initialize DLSS only after the
    // original D3D11 creation call and its internal Present have returned.
    sourceUpscalerInitializationPending_ = false;
    D3D11_TEXTURE2D_DESC outputDesc{};
    presentation_.Buffers().front()->GetDesc(&outputDesc);
    const auto expectedRenderWidth = renderWidth_;
    const auto expectedRenderHeight = renderHeight_;
    if (!InitializeSourceUpscaler(outputDesc))
    {
        status_ = std::format("Source DLSS startup failed: {}", status_);
        logger::error("[NvidiaHost] {}", status_);
        return false;
    }
    if (renderWidth_ != expectedRenderWidth || renderHeight_ != expectedRenderHeight)
    {
        logger::error("[NvidiaHost] deferred source upscaler extent mismatch "
                      "actual={}x{} prepared={}x{}",
                      renderWidth_, renderHeight_, expectedRenderWidth, expectedRenderHeight);
        // Retain the initialized feature until teardown proves retirement.
        renderWidth_ = expectedRenderWidth;
        renderHeight_ = expectedRenderHeight;
        status_ = "NVIDIA source upscaler disagreed with the prepared render-size "
                  "contract";
        return false;
    }
    if(FsrActive()){warmupPresentsRemaining_=0;SetRuntimeEnabled(false);}else ArmFrameGenerationWarmup();
    TheosRenderPipeline::ReShadeIntegration::Get().Configure(device_.Get(), context_.Get(), {outputWidth_, outputHeight_});
    logger::info("[NvidiaHost] source upscaler initialized after D3D11 startup Present");
#if !defined(TRP_NO_NEURAL_RENDERING)
    InspectCommunityNeural();
#endif
#if defined(TRP_ENABLE_FSR)
    fsrSizingRetainedForResize_=false;
#endif
    return true;
}

bool NvidiaHost::InitializeSourceUpscaler(const D3D11_TEXTURE2D_DESC& a_outputDesc)
{
    if (TheosRenderPipeline::CommunityShaders::Active()) {
        upscalerReady_ = true;
        splitSourceDLSSActive_ = false;
        status_ = FsrActive()?"CS owns upscaling; ordinary presenter ready":"CS owns upscaling; NVIDIA frame adapter initialized";
        return true;
    }
    if (!device_ || !context_ || renderWidth_ == 0 || renderHeight_ == 0)
    {
        status_ = "TheosRenderPipeline DLSS split source prerequisites are incomplete";
        return false;
    }

    // Allocate the native handoff target before creating the DLSS feature.
    // The source backend imports the completed D3D11 output for presentation.
    const auto outputResult = gameTargets_.CreateUpscaleOutputAfterRetirement(device_.Get(), a_outputDesc);
    const auto& allocation = gameTargets_.LastOutputAllocation();
    logger::info("[NvidiaHost] handoff allocation format={} supportHr=0x{:08X} support=0x{:X} uavAttempted={} uavHr=0x{:08X} plainAttempted={} plainHr=0x{:08X} deviceStatusChecked={} deviceRemovedReason=0x{:08X} result=0x{:08X}",
                 static_cast<std::uint32_t>(a_outputDesc.Format), static_cast<std::uint32_t>(allocation.formatSupportResult),
                 allocation.formatSupport, allocation.unorderedAccessAttempted, static_cast<std::uint32_t>(allocation.unorderedAccessResult),
                 allocation.plainAttempted, static_cast<std::uint32_t>(allocation.plainResult), allocation.deviceStatusChecked,
                 static_cast<std::uint32_t>(allocation.deviceRemovedReason), static_cast<std::uint32_t>(outputResult));
    if (FAILED(outputResult) || !gameTargets_.UpscaleOutput())
    {
        status_ = std::format("TheosRenderPipeline DLSS native handoff texture creation failed (0x{:08X})", static_cast<std::uint32_t>(outputResult));
        return false;
    }
    {
        // Whether the direct-output routes can be eligible at all is decided
        // here, once, and is otherwise only observable as a per-session
        // rejection after the user enables them.
        D3D11_TEXTURE2D_DESC handoff{};
        gameTargets_.UpscaleOutput()->GetDesc(&handoff);
        logger::info("[NvidiaHost] native handoff target {}x{} format={} bind=0x{:X}; direct-output routes {}",
                     handoff.Width, handoff.Height, static_cast<std::uint32_t>(handoff.Format), handoff.BindFlags,
                     (handoff.BindFlags & D3D11_BIND_UNORDERED_ACCESS) ? "can be requested" : "unavailable on allocated target; using copy route");
    }

#if defined(TRP_ENABLE_FSR)
    if(FsrActive()) {
        auto created=fsrResources_->CompleteStartup();
        if(!created){status_=created.error().message;return false;}
        upscalerReady_=true;splitSourceDLSSActive_=false;
        sourceUpscalerSettings_.BeginSubmission();sourceUpscalerSettings_.Completed(true);AdoptEffectiveSourceUpscalerSettings();
        if(!CreateNativeUIExtractionResources(a_outputDesc)){status_="FSR native UI resource creation failed";return false;}
        if (FsrFgActive() && (!nativeUI_.Dedicated() || !nativeUI_.Available())) {
            status_="FSR FG dedicated UI initialization failed";return false;
        }
        status_=FsrFgActive()?"FSR ready on AMD presenter; FG awaits measured camera and completed UI":"FSR context ready on ordinary D3D11 presenter; waiting for validated source frames";
        const auto& settings=sourceUpscalerSettings_.Effective().fsr;
        logger::info("[FSR startup] provider={} quality={} policy={} sourceColorEncoding={} render={}x{} output={}x{} presentation={} frameGeneration=context-deferred",
            fsrResources_->Provider().name,TheosRenderPipeline::Upscaling::QualityName(settings.quality),
            TheosRenderPipeline::Upscaling::ProviderPolicyName(settings.providerPolicy),
            TheosRenderPipeline::Upscaling::ColorEncodingName(settings.sourceColorEncoding),
            renderWidth_,renderHeight_,outputWidth_,outputHeight_,FsrFgActive()?"AMD-D3D12":"ordinary-D3D11");
        return true;
    }
#endif
    auto* dlss = DLSSBackend::GetSingleton();
    dlss->SetupDevice(device_.Get(), context_.Get());
    const auto creation = sourceUpscalerSettings_.BeginSubmission();
    if (!dlss->InitUpscale(static_cast<int>(renderWidth_), static_cast<int>(renderHeight_), static_cast<int>(a_outputDesc.Width),
                           static_cast<int>(a_outputDesc.Height), a_outputDesc.Format, creation.sharpening, creation.autoExposure, creation.preset,
                           creation.AllocationQuality()))
    {
        sourceUpscalerSettings_.Completed(false);
        status_ = "TheosRenderPipeline direct DLSS feature initialization failed";
        gameTargets_.ResetUpscaleOutputAfterRetirement();
        return false;
    }
    // These flags also track feature ownership for teardown. A subsequent
    // contract failure must retain the initialized feature until retirement.
    splitSourceDLSSActive_ = true;
    upscalerReady_ = true;
    if (dlss->RenderWidth() != static_cast<int>(renderWidth_) || dlss->RenderHeight() != static_cast<int>(renderHeight_))
    {
        sourceUpscalerSettings_.Completed(false);
        status_ = "TheosRenderPipeline direct DLSS disagreed with the prepared render extent";
        return false;
    }

    sourceUpscalerSettings_.Completed(true);
    AdoptEffectiveSourceUpscalerSettings();
    if (!CreateNativeUIExtractionResources(a_outputDesc))
    {
        logger::warn("[NvidiaHost] explicit native UI extraction unavailable on "
                     "split source; retaining HUD-less-only fallback");
    }
    status_ = "Source DLSS and NVIDIA frame-generation path ready";
    logger::info("[NvidiaHost] split source initialized owner=TheosRenderPipeline-DLSS render={}x{} "
                 "output={}x{} format={} quality={} evaluator=SourceNvidiaFrameEvaluator",
                 renderWidth_, renderHeight_, a_outputDesc.Width, a_outputDesc.Height, static_cast<std::uint32_t>(a_outputDesc.Format),
                 creation.AllocationQuality());
    return true;
}

#if defined(TRP_ENABLE_FSR_FG)
HRESULT NvidiaHost::CreateFsrPresenter(IDXGIFactory* factory,ID3D11Device* producer,const DXGI_SWAP_CHAIN_DESC& descriptor,IDXGISwapChain** output)
{
    if (!output) return E_POINTER;*output=nullptr;
    using namespace TheosRenderPipeline;
    logger::info("[FSR startup] requested extent={}x{} windowed={} flags=0x{:08X} swapEffect={} buffers={}",
        descriptor.BufferDesc.Width,descriptor.BufferDesc.Height,descriptor.Windowed!=FALSE,descriptor.Flags,
        static_cast<unsigned>(descriptor.SwapEffect),descriptor.BufferCount);
    fsrResources_=std::make_shared<Upscaling::FsrHostResources>(PluginPaths::Directory(),
        +[](IUnknown* adapter,D3D_FEATURE_LEVEL level,ID3D12Device** device)->HRESULT {
            return ReShadeIntegration::Get().CreateSourceDevice(adapter,level,device,true);
        });
    fsrPresentation_=std::make_unique<FsrHostPresentation>();
    auto extent=fsrPresentation_->Create(factory,producer,fsrResources_,descriptor,sourceUpscalerSettings_.Startup().fsr);
    if (!extent) {
        status_=extent.error().message;
        logger::error("[FSR startup] rejected flags=0x{:08X} native=0x{:08X} reason={}",descriptor.Flags,
            static_cast<std::uint32_t>(extent.error().nativeResult),status_);
        return E_FAIL;
    }
    fsrFactory_=factory;fsrDescriptor_=descriptor;
    fsrDescriptor_.BufferCount=2;
    renderWidth_=extent->width;renderHeight_=extent->height;
    *output=fsrPresentation_->SwapChain();(*output)->AddRef();
    return S_OK;
}
#endif

HRESULT NvidiaHost::ResizeFsrSwapChain(GameSwapChain& outer,UINT count,UINT width,UINT height,DXGI_FORMAT format,UINT flags,
    const UINT* masks,IUnknown* const* queues)
{
#if defined(TRP_ENABLE_FSR_FG)
    if (!FsrFgActive() || &outer!=outerSwapChain_ || !fsrPresentation_ || FAILED(FailureResult())) return E_UNEXPECTED;
    auto candidate=fsrDescriptor_;candidate.BufferCount=2;candidate.BufferDesc.Width=width;candidate.BufferDesc.Height=height;
    if (format!=DXGI_FORMAT_UNKNOWN)candidate.BufferDesc.Format=format;candidate.Flags=flags;
    auto valid=TheosRenderPipeline::ValidateFsrResize(count,masks,queues);if(FAILED(valid))return valid;
    valid=TheosRenderPipeline::ValidateFsrResizeFlags(fsrDescriptor_.Flags,flags);if(FAILED(valid))return valid;
    auto translated=TheosRenderPipeline::FsrPresentation::TranslateResizeDescriptor(candidate);
    if(!translated)return E_INVALIDARG;
    if(!*translated){
        const bool wasSuspended=FsrPresentSuspended();auto stopped=fsrPresentation_->Suspend();
        if(!stopped){status_=stopped.error().message;return FailLifecycle(E_FAIL,"AMD suspension retirement");}
        EndNativeUIPass();nativeUIPass_.ResetEvaluation();frameGenerationEnabled_=false;
        fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();resetNextEvaluation_=true;
        if(!wasSuspended)logger::info("[FSR resize] suspended empty client extent; AMD chain and game buffers retained");
        return S_OK;
    }
    const auto admission=TheosRenderPipeline::NeuralRendering::RetireBeforeSourceResize(
        [&]()->HRESULT {
#if !defined(TRP_NO_NEURAL_RENDERING)
            if (!RetireCommunityNeural()) return FailureResult();
#endif
            return S_OK;
        },
        [&]()->HRESULT {const auto before=fsrPresentation_->BeforeResize();
            if(!before){status_=before.error().message;return FailLifecycle(E_FAIL,"AMD resize retirement");}return S_OK;});
    if (FAILED(admission)) return admission;
    frameGenerationEnabled_=false;fsrSourcePending_=fsrUiComplete_=false;fsrForeground_.Reset();
    resetNextEvaluation_=true;
    EndNativeUIPass();context_->ClearState();context_->Flush();
    gameTargets_.ResetGameFacingAfterRetirement();ReleaseSourceUpscaler(true);presentation_.ResetAfterRetirement();
    auto resized=fsrPresentation_->Resize(**translated);
    if(!resized){status_=resized.error().message;return FailLifecycle(E_FAIL,"AMD resize reconstruction");}
    renderWidth_=resized->render.width;renderHeight_=resized->render.height;
    auto hr=innerSwapChain_->GetDesc(&fsrDescriptor_);
    if(FAILED(hr) || !CreateGameFacingResources(innerSwapChain_) || !CompleteStartupAfterDeviceCreation())
        return FailLifecycle(FAILED(hr)?hr:E_FAIL,"AMD resized source resources");
    logger::info("[FSR resize] same AMD chain result=0x{:08X} render={}x{} output={}x{} temporalReset=true",
        static_cast<std::uint32_t>(resized->result),renderWidth_,renderHeight_,outputWidth_,outputHeight_);
    return resized->result;
#else
    (void)outer;(void)count;(void)width;(void)height;(void)format;(void)flags;(void)masks;(void)queues;return E_NOTIMPL;
#endif
}
