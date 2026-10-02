#include "FrameGen/FSRPresentation.h"
#include "Upscaling/FSRPreparedResources.h"
#include "InteropTestRig.h"
#include "PresentationObserver.h"
#include <bcrypt.h>
#include <dxgi1_6.h>
#include <DirectXPackedVector.h>
#include <array>
#include <atomic>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <cmath>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using Microsoft::WRL::ComPtr;
namespace
{
    void Need(bool condition,const char* reason){if(!condition)throw std::runtime_error(reason);}
    void Gpu(HRESULT hr,const char* reason){if(FAILED(hr)){std::ostringstream text;text<<reason<<" HRESULT=0x"<<std::hex<<static_cast<unsigned long>(hr);throw std::runtime_error(text.str());}}
    void Accepted(Result<void> result){if(!result)throw std::runtime_error(result.error().message+" native="+std::to_string(result.error().nativeResult));}
    template<class T>T Value(Result<T> result){if(!result)throw std::runtime_error(result.error().message);return std::move(*result);}
    struct DebugMessage{uint32_t type{};std::array<wchar_t,512> text{};std::atomic<bool> ready{};};
    struct Collector
    {
        std::array<DebugMessage,64> messages{};std::atomic<unsigned> count{};
        static std::atomic<Collector*> active;
        static void Message(uint32_t type,const wchar_t* text)
        {
            auto* collector=active.load(std::memory_order_acquire);if(!collector)return;const auto index=collector->count.fetch_add(1,std::memory_order_relaxed);
            if(index>=collector->messages.size())return;auto& message=collector->messages[index];message.type=type;
            if(text)for(std::size_t i=0;i+1<message.text.size() && text[i];++i)message.text[i]=text[i];
            message.ready.store(true,std::memory_order_release);
        }
    };
    std::atomic<Collector*> Collector::active{};
    std::string JsonString(const std::string& value)
    {
        std::ostringstream output;output<<'"';
        for(unsigned char c:value){if(c=='"' || c=='\\')output<<'\\'<<char(c);
            else if(c<32)output<<"\\u00"<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(c);
            else output<<char(c);}
        output<<'"';return output.str();
    }
    std::string Sha256(const std::filesystem::path& path)
    {
        BCRYPT_ALG_HANDLE algorithm{};BCRYPT_HASH_HANDLE hash{};
        Need(BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0)),"SHA256 algorithm");
        struct Cleanup{BCRYPT_ALG_HANDLE& algorithm;BCRYPT_HASH_HANDLE& hash;~Cleanup(){if(hash)BCryptDestroyHash(hash);if(algorithm)BCryptCloseAlgorithmProvider(algorithm,0);}}cleanup{algorithm,hash};
        DWORD bytes{},objectSize{};Need(BCRYPT_SUCCESS(BCryptGetProperty(algorithm,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&objectSize),sizeof(objectSize),&bytes,0)),"SHA256 object size");
        std::vector<unsigned char> object(objectSize);std::array<unsigned char,32> digest{};std::array<unsigned char,65536> chunk{};
        Need(BCRYPT_SUCCESS(BCryptCreateHash(algorithm,&hash,object.data(),objectSize,nullptr,0,0)),"SHA256 create");
        std::ifstream file(path,std::ios::binary);Need(bool(file),"hash input exists");while(file){file.read(reinterpret_cast<char*>(chunk.data()),chunk.size());const auto count=file.gcount();
            if(count)Need(BCRYPT_SUCCESS(BCryptHashData(hash,chunk.data(),static_cast<ULONG>(count),0)),"SHA256 update");}
        Need(file.eof() && BCRYPT_SUCCESS(BCryptFinishHash(hash,digest.data(),digest.size(),0)),"SHA256 finish");
        std::ostringstream output;output<<std::hex<<std::setfill('0');for(auto byte:digest)output<<std::setw(2)<<unsigned(byte);return output.str();
    }
    bool NvidiaAbsent()
    {
        for(auto name:{L"sl.interposer.dll",L"sl.dlss.dll",L"sl.dlss_g.dll",L"nvngx_dlss.dll",L"nvngx_dlssg.dll"})if(GetModuleHandleW(name))return false;
        return true;
    }
    struct Evidence
    {
        unsigned sources{},callbacks{},prepared{},suppressed{},reentries{},retired{},generatedPixels{},presentObservations{};
        bool debug11{},debug12{},nvidiaFree{},automaticUiPending{true},physicalCadencePending{true};
        UINT64 memoryMin{UINT64_MAX},memoryMax{};std::string failure,sourceHash;
        std::string observerHash;unsigned automaticSources{},observerSources{},renderedReadbacks{},changingGenerated{},uiSentinels{};
    };
    void WriteReport(const std::filesystem::path& path,const Evidence& e,const Collector& collector,unsigned frames,unsigned cycles)
    {
        std::filesystem::create_directories(std::filesystem::absolute(path).parent_path());std::ofstream out(path);
        out<<"{\n\"result\":"<<JsonString(e.failure.empty()?"PASS":"FAIL")<<",\"failure\":"<<JsonString(e.failure)
            <<",\n\"sourceRevision\":\""<<TRP_FG_VALIDATION_REVISION<<"\",\"fixtureSourceSha256\":"<<std::quoted(e.sourceHash)
            <<",\"sdkCommit\":\"60f4ea81909200d8542eca14dccb2628b763a9a3\",\"fgModuleSha256\":\""<<TRP_FG_MODULE_SHA<<"\""
            <<",\"observerSourceSha256\":"<<std::quoted(e.observerHash)
            <<",\n\"requestedSourcesPerMode\":"<<frames<<",\"requestedRecreationsPerMode\":"<<cycles<<",\"validationModes\":2,\"submittedSources\":"<<e.sources
            <<",\"prepareSubmissions\":"<<e.prepared<<",\"generationCallbacks\":"<<e.callbacks<<",\"generatedPixelReadbacks\":"<<e.generatedPixels
            <<",\"presentCallbackObservations\":"<<e.presentObservations<<",\"suppressedSources\":"<<e.suppressed<<",\"resetReentries\":"<<e.reentries<<",\"retiredContexts\":"<<e.retired
            <<",\"automaticModeSources\":"<<e.automaticSources<<",\"observerModeSources\":"<<e.observerSources<<",\"renderedPixelReadbacks\":"<<e.renderedReadbacks<<",\"changingGeneratedSamples\":"<<e.changingGenerated<<",\"callbackModeUiSentinels\":"<<e.uiSentinels
            <<",\n\"nvidiaRuntimeAbsent\":"<<(e.nvidiaFree?"true":"false")<<",\"d3d11DebugAvailable\":"<<(e.debug11?"true":"false")<<",\"d3d12DebugAvailable\":"<<(e.debug12?"true":"false")
            <<",\"sdkDebugMessages\":"<<collector.count.load()<<",\"sdkDebugCheckingEnabled\":true,\"automaticUiAppearance\":\"pending visible acceptance\",\"physicalCadence\":\"pending visible acceptance\""
            <<",\"retiredMemoryRangeBytes\":"<<(e.memoryMin==UINT64_MAX?0:e.memoryMax-e.memoryMin)<<",\n\"messages\":[";
        const auto count=std::min(collector.count.load(),unsigned(collector.messages.size()));for(unsigned i=0;i<count;++i){if(i)out<<',';std::string message;
            if(!collector.messages[i].ready.load(std::memory_order_acquire)){out<<"{\"pending\":true}";continue;}
            for(auto character:collector.messages[i].text){if(!character)break;message+=character<128?char(character):'?';}out<<"{\"type\":"<<collector.messages[i].type<<",\"text\":"<<JsonString(message)<<'}';}
        out<<"]\n}\n";Need(bool(out),"validation report saved");
    }
}
int main(int argc,char** argv)
{
    unsigned frames=1000,cycles=25;std::filesystem::path runtimeDirectory,report="fsr-fg-gpu.json";Evidence evidence;static Collector collector;Collector::active=&collector;
    try{
        for(int i=1;i<argc;++i){Need(i+1<argc,"option needs value");std::string key=argv[i],value=argv[++i];
            if(key=="--frames")frames=std::stoul(value);else if(key=="--recreate")cycles=std::stoul(value);else if(key=="--runtime")runtimeDirectory=std::filesystem::absolute(value);else if(key=="--output")report=value;
            else if(key=="--debug")Need(value=="auto","only debug auto supported");else Need(false,"unknown option");}
        Need(cycles && frames>=cycles*36,"at least 36 sources per cycle for suppression/reentry");
        if(!std::filesystem::exists(runtimeDirectory/"amd_fidelityfx_framegeneration_dx12.dll")){std::puts("SKIPPED: pinned FG runtime missing");return 77;}
        evidence.sourceHash=Sha256(TRP_FG_VALIDATION_SOURCE);evidence.observerHash=Sha256(TRP_FG_OBSERVER_SOURCE);evidence.nvidiaFree=NvidiaAbsent();Need(evidence.nvidiaFree,"NVIDIA upscaling/generation runtimes must be absent");
        const auto plugin=std::filesystem::absolute(report).parent_path()/"fg-runtime-plugin";std::filesystem::create_directories(plugin/"FSR");
        const std::array<const char*,3> files{"amd_fidelityfx_loader_dx12.dll","amd_fidelityfx_upscaler_dx12.dll","amd_fidelityfx_framegeneration_dx12.dll"};
        const std::array<const char*,3> hashes{TRP_FG_LOADER_SHA,TRP_FG_UPSCALER_SHA,TRP_FG_MODULE_SHA};
        for(unsigned i=0;i<files.size();++i){Need(Sha256(runtimeDirectory/files[i])==hashes[i],"runtime SHA256 differs from pinned SDK");std::filesystem::copy_file(runtimeDirectory/files[i],plugin/"FSR"/files[i],std::filesystem::copy_options::overwrite_existing);}
        auto runtime=std::make_shared<FsrRuntime>();Accepted(runtime->Load(plugin));Accepted(runtime->LoadFrameGeneration(plugin));
        ffxConfigureDescGlobalDebug1 debug{{FFX_API_CONFIGURE_DESC_TYPE_GLOBALDEBUG1,nullptr},Collector::Message,FFX_API_CONFIGURE_GLOBALDEBUG_LEVEL_ERRORS|FFX_API_CONFIGURE_GLOBALDEBUG_LEVEL_WARNINGS};
        const auto loaderDebug=runtime->Functions().Configure(nullptr,&debug.header);
        std::printf("GlobalDebug1 loader return=%u\n",unsigned(loaderDebug));
        // Global diagnostics belong to each effect DLL's public API instance.
        // The small provider-routing loader may reject context-free configure.
        for(auto moduleName:{L"amd_fidelityfx_upscaler_dx12.dll",L"amd_fidelityfx_framegeneration_dx12.dll"}){
            auto configure=reinterpret_cast<PfnFfxConfigure>(GetProcAddress(GetModuleHandleW(moduleName),"ffxConfigure"));Need(configure!=nullptr,"effect public Configure export");
            const auto result=configure(nullptr,&debug.header);std::printf("GlobalDebug1 effect return=%u\n",unsigned(result));
            Need(result==FFX_API_RETURN_OK,"effect SDK global debug collector enabled");}
        InteropFixture::Rig rig(true);evidence.debug11=bool(rig.messages11);evidence.debug12=bool(rig.messages12);
        auto fg=Value(SelectFsrEffectProvider(Value(runtime->EnumerateForEffect(rig.device12.Get(),FsrEffect::FrameGeneration)),FsrEffect::FrameGeneration));
        auto sw=Value(SelectFsrEffectProvider(Value(runtime->EnumerateForEffect(rig.device12.Get(),FsrEffect::FrameGenerationSwapChain)),FsrEffect::FrameGenerationSwapChain));
        ComPtr<IDXGIAdapter3> memoryAdapter;Gpu(rig.adapter.As(&memoryAdapter),"GPU memory adapter");
        auto memory=[&]{DXGI_QUERY_VIDEO_MEMORY_INFO info{};Gpu(memoryAdapter->QueryVideoMemoryInfo(0,DXGI_MEMORY_SEGMENT_GROUP_LOCAL,&info),"GPU memory query");return info.CurrentUsage;};
        HWND window=CreateWindowExW(0,L"STATIC",L"TRP actual FSR generation fixture",WS_OVERLAPPEDWINDOW,0,0,640,360,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);Need(window!=nullptr,"hidden fixture HWND");
        struct Window{HWND value;~Window(){DestroyWindow(value);}}windowOwner{window};
        // This fixture alone intercepts the public Configure descriptor to add
        // a public Present callback. Production generation routing is unchanged.
        // No observer field/default/API is added to the release integration.
        auto& functions=const_cast<FsrFunctions&>(runtime->Functions());FgObservation::originalConfigure=functions.Configure;functions.Configure=FgObservation::Configure;
        struct Restore{FsrFunctions& functions;~Restore(){functions.Configure=FgObservation::originalConfigure;FgObservation::activeCapture=nullptr;}}restore{functions};
        for(unsigned observationMode=0;observationMode<2;++observationMode){
        for(unsigned cycle=0;cycle<cycles;++cycle){
            {
                auto bridge=std::make_shared<Graphics::D3D11D3D12Interop>();rig.Initialize(*bridge);FsrGenerationLimits limits;limits.render={320,180};limits.display={640,360};limits.debugChecking=true;
                auto d=FsrPreparedTextureDesc(FsrResourceRole::Depth,limits.render);
                Graphics::SharedTexture depth,motion;Gpu(bridge->CreateSharedTexture(d,depth),"controlled depth");d=FsrPreparedTextureDesc(FsrResourceRole::Motion,limits.render);Gpu(bridge->CreateSharedTexture(d,motion),"controlled motion");
                d.Width=limits.display.width;d.Height=limits.display.height;d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;d.BindFlags=D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
                ComPtr<ID3D11Texture2D> ui;Gpu(rig.device11->CreateTexture2D(&d,nullptr,&ui),"completed native UI");
                std::vector<uint32_t> scenePixels(d.Width*d.Height,0xff40261a),uiPixels(scenePixels.size());
                uiPixels[0]=0xff0000ff;uiPixels[8]=0x80008000;rig.context11->UpdateSubresource(ui.Get(),0,nullptr,uiPixels.data(),d.Width*4,0);
                std::vector<float> depths(limits.render.width*limits.render.height);
                std::vector<DirectX::PackedVector::HALF> motions(depths.size()*2);
                FsrPresentation presenter;DXGI_SWAP_CHAIN_DESC desc{};desc.OutputWindow=window;desc.Windowed=TRUE;desc.BufferDesc.Width=d.Width;desc.BufferDesc.Height=d.Height;desc.BufferDesc.Format=d.Format;desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=1;
                Accepted(presenter.Create(rig.factory.Get(),runtime,bridge,desc,sw));Gpu(presenter.Present({},UpscaleOutcome::SkippedInvalidInput,{},nullptr,ColorEncoding::Unknown,nullptr,nullptr,false,false,false,0,0),"feature-less startup Present");Accepted(presenter.CompleteStartup(limits,fg));
                const auto count=frames/cycles+(cycle<frames%cycles?1:0);std::unique_ptr<FgObservation::Capture> capture;
                if(observationMode){capture=std::make_unique<FgObservation::Capture>(rig.device12.Get(),count*2+8,limits.display.width,limits.display.height);FgObservation::activeCapture=capture.get();}
                std::unordered_set<uint64_t> sourceIds,eligibleIds;
                UpscaleFrame frame;frame.backend=BackendKind::Fsr;frame.render=frame.subrect=limits.render;frame.display=limits.display;frame.depth=depth.texture11.Get();frame.motion=motion.texture11.Get();frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;
                frame.deltaMilliseconds=1000.0f/72;frame.motionConvention={float(limits.render.width),float(limits.render.height),true,false};
                frame.camera.identity=7;frame.camera.nearDistance=.1f;frame.camera.farDistance=100;frame.camera.verticalFovRadians=1.04719755f;frame.camera.worldUnitsToMeters=1;
                frame.camera.view={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};frame.camera.projection={1,0,0,0,0,1.7320508f,0,0,0,0,1.001001f,1,0,0,-.1001001f,0};
                GpuFrameResources guides;guides.depth=depth.texture12.Get();guides.motion=motion.texture12.Get();
                for(unsigned local=0;local<count;++local){
                    Gpu(presenter.WaitBeforeProducer(),"wait before guide overwrite");frame.sourceId=static_cast<uint64_t>(evidence.sources)+1;
                    const float objectShift=float(local)*2/limits.display.width;const float previousShift=local?float(local-1)*2/limits.display.width:objectShift;
                    for(UINT y=0;y<limits.display.height;++y)for(UINT x=0;x<limits.display.width;++x){const float u=(float(x)+.5f)/limits.display.width;
                        const bool foreground=std::abs(u-(.4f+objectShift))<.04f && y>100 && y<260;scenePixels[y*limits.display.width+x]=foreground?0xff0d26cc:0xff40261a;}
                    for(UINT y=0;y<limits.render.height;++y)for(UINT x=0;x<limits.render.width;++x){const float u=(float(x)+.5f)/limits.render.width;const bool foreground=std::abs(u-(.4f+objectShift))<.04f && y>50 && y<130;
                        const float z=foreground?1.5f:3.f;const auto index=y*limits.render.width+x;
                        depths[index]=100.f/99.9f-(100.f*.1f/99.9f)/z;
                        motions[index*2]=DirectX::PackedVector::XMConvertFloatToHalf(foreground?previousShift-objectShift:0);motions[index*2+1]=0;}
                    rig.context11->UpdateSubresource(presenter.SceneTarget11(),0,nullptr,scenePixels.data(),limits.display.width*4,0);
                    rig.context11->UpdateSubresource(depth.texture11.Get(),0,nullptr,depths.data(),limits.render.width*4,0);
                    rig.context11->UpdateSubresource(motion.texture11.Get(),0,nullptr,motions.data(),limits.render.width*4,0);Gpu(bridge->SignalD3D11(Graphics::InteropWork::FrameGeneration),"guide producer submission");
                    bool requested=true,menu=false;frame.camera.reset=false;frame.depth=depth.texture11.Get();frame.deltaMilliseconds=1000.f/72;
                    if(local==19){switch(cycle%5){case 0:requested=false;break;case 1:menu=true;break;case 2:frame.depth=nullptr;break;case 3:frame.camera.reset=true;break;case 4:frame.deltaMilliseconds=100;break;}}
                    Gpu(presenter.Present(frame,UpscaleOutcome::Temporal,guides,presenter.SceneTarget11(),ColorEncoding::SRGB,ui.Get(),nullptr,true,menu,requested,0,0),"production FG source handoff");
                    const auto status=presenter.Status();Need(status.sourceId==frame.sourceId && status.submitted,"source ID and submission observed");
                    Need(status.callback.invocations==(status.decision.generate?1u:0u),"one SDK generation callback exactly on eligible sources");
                    ++evidence.sources;evidence.callbacks+=status.callback.invocations;if(status.decision.prepare)++evidence.prepared;else ++evidence.suppressed;
                    sourceIds.insert(frame.sourceId);if(status.decision.generate)eligibleIds.insert(frame.sourceId);
                    if(observationMode)++evidence.observerSources;else ++evidence.automaticSources;
                    if(status.decision.generate && status.decision.reset)++evidence.reentries;
                }
                const auto retirement=presenter.Retire();if(!retirement){(void)capture.release();Accepted(retirement);}++evidence.retired;FgObservation::activeCapture=nullptr;
                if(capture){
                    Need(!capture->Failed(),"public Present observer rejected a descriptor or recording");unsigned generatedThisCycle{},changingThisCycle{};uint64_t previous{};
                    std::unordered_set<uint64_t> renderedIds,generatedIds;
                    for(unsigned index=0;index<capture->Count();++index){const auto& sample=capture->Samples()[index];Need(sample.recorded && sourceIds.contains(sample.source),"observed Present ID belongs to a submitted source");
                        auto& ids=sample.generated?generatedIds:renderedIds;Need(ids.insert(sample.source).second,"one observation of each kind per source");
                        if(sample.generated)Need(eligibleIds.contains(sample.source),"generated presentation belongs to an enabled prepared source");
                        D3D12_RANGE read{0,capture->RowPitch()*2};void* mapped{};Gpu(sample.readback->Map(0,&read,&mapped),"retired observation readback");auto* bytes=static_cast<const unsigned char*>(mapped);
                        const auto close=[](int value,int expected){return std::abs(value-expected)<=2;};
                        Need(bytes[0]>=253 && bytes[1]<=2 && bytes[2]<=2 && bytes[3]>=253,"opaque UI sentinel on rendered/generated output");
                        if(!(close(bytes[32],13) && close(bytes[33],147) && close(bytes[34],32) && bytes[35]>=253))
                            std::fprintf(stderr,"UI sample source=%llu generated=%u RGBA=%u,%u,%u,%u\n",static_cast<unsigned long long>(sample.source),sample.generated,bytes[32],bytes[33],bytes[34],bytes[35]);
                        Need(close(bytes[32],13) && close(bytes[33],147) && close(bytes[34],32) && bytes[35]>=253,"half-alpha premultiplied UI sentinel on rendered/generated output");
                        uint64_t hash=1469598103934665603ull;unsigned minimum=255,maximum{};const auto* row=bytes+capture->RowPitch();
                        for(UINT x=0;x<capture->Width();++x)for(UINT channel=0;channel<3;++channel){const auto value=row[x*4+channel];Need(std::isfinite(float(value)/255),"native UNORM generated samples finite");hash=(hash^value)*1099511628211ull;minimum=std::min(minimum,unsigned(value));maximum=std::max(maximum,unsigned(value));}
                        Need(maximum-minimum>40,"observed scene contains nonempty moving foreground contrast");D3D12_RANGE noWrite{};sample.readback->Unmap(0,&noWrite);
                        ++evidence.presentObservations;++evidence.uiSentinels;
                        if(sample.generated){++evidence.generatedPixels;++generatedThisCycle;if(previous && previous!=hash){++evidence.changingGenerated;++changingThisCycle;}previous=hash;}else ++evidence.renderedReadbacks;
                    }
                    Need(generatedThisCycle>=2 && changingThisCycle>=1,"changing generated pixels in every observed context");
                }
            }
            const auto usage=memory();evidence.memoryMin=std::min(evidence.memoryMin,usage);evidence.memoryMax=std::max(evidence.memoryMax,usage);
        }
        }
        evidence.nvidiaFree=NvidiaAbsent();Need(evidence.nvidiaFree,"NVIDIA runtime appeared during FSR-only generation");rig.ValidateDebug();
        Need(!collector.count.load(),"unexpected SDK debug warning/error (see report)");Need(evidence.automaticSources==frames && evidence.observerSources==frames && evidence.retired==cycles*2,"all automatic/observer source submissions and context retirements observed");
        Need(evidence.callbacks>cycles*2 && evidence.reentries>=cycles*4,"actual production generation callbacks and suppression/reentry exercised");
        Need(evidence.generatedPixels>cycles && evidence.changingGenerated>=cycles,"actual generated image readbacks change");
        Need(evidence.memoryMax-evidence.memoryMin<64ull*1024*1024,"retired GPU memory growth bounded across both modes");
    }catch(const std::exception& error){evidence.failure=error.what();std::fprintf(stderr,"FAIL: %s\n",error.what());}
    WriteReport(report,evidence,collector,frames,cycles);Collector::active=nullptr;
    std::printf("sources=%u callbacks=%u retired=%u generatedPixelReadbacks=%u SDKMessages=%u\n",evidence.sources,evidence.callbacks,evidence.retired,evidence.generatedPixels,collector.count.load());
    return evidence.failure.empty()?0:1;
}
