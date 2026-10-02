#include "FrameGen/OrdinaryPresentation.h"
#include "FrameGen/PresentationTargets.h"
#include "Upscaling/FSRHostResources.h"
#include "Upscaling/FSRFrameAdapter.h"
#include "InteropTestRig.h"
#include <DirectXMath.h>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <cctype>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
using namespace InteropFixture;
template<class T>T Value(Result<T> value){if(!value){std::fprintf(stderr,"FAIL: %s\n",value.error().message.c_str());std::exit(value.error().kind==ErrorKind::UnsupportedDevice?77:1);}return std::move(*value);}
void Accepted(Result<void> value){if(!value){std::fprintf(stderr,"FAIL: %s\n",value.error().message.c_str());std::exit(value.error().kind==ErrorKind::UnsupportedDevice?77:1);}}
std::string Json(std::string value){std::string result="\"";for(char c:value){if(c=='\\'||c=='\"')result+='\\';if(static_cast<unsigned char>(c)<32)result+=' ';else result+=c;}return result+'\"';}
std::vector<std::string> Modules() {
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,GetCurrentProcessId());Require(snapshot!=INVALID_HANDLE_VALUE,"module snapshot");
    MODULEENTRY32W entry{};entry.dwSize=sizeof(entry);std::vector<std::string> names;Require(Module32FirstW(snapshot,&entry)!=FALSE,"module enumeration");
    do {auto name=std::filesystem::path(entry.szModule).string();std::transform(name.begin(),name.end(),name.begin(),[](unsigned char c){return char(std::tolower(c));});
        Require(!name.starts_with("sl.") && !name.starts_with("nvngx") && !name.starts_with("_nvngx") && !name.starts_with("nvapi"),"no NVIDIA runtime module loaded");names.push_back(name);
    }while(Module32NextW(snapshot,&entry));CloseHandle(snapshot);return names;
}
int main(int argc,char** argv) {
    std::filesystem::path plugin,runtime,report;
    for(int i=1;i+1<argc;i+=2){std::string key=argv[i];if(key=="--plugin")plugin=argv[i+1];else if(key=="--runtime")runtime=argv[i+1];else if(key=="--output")report=argv[i+1];else Require(false,"known argument");}
    Require(plugin.is_absolute() && runtime.is_absolute() && report.is_absolute(),"absolute plugin/runtime/report paths");
    Modules();
    HMODULE pluginModule=LoadLibraryExW(plugin.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!pluginModule)std::fprintf(stderr,"Plugin load Win32 error=%lu\n",GetLastError());Require(pluginModule!=nullptr,"built plugin imports load in fresh process");Modules();
    Require(!GetModuleHandleW(L"amd_fidelityfx_loader_dx12.dll") && !GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll"),"plugin load does not eagerly load AMD runtime");
    std::filesystem::create_directories(report.parent_path());
    auto staging=report.parent_path()/("startup-"+std::to_string(GetCurrentProcessId()));std::filesystem::create_directories(staging);
    // Real runtimes in the working directory must not satisfy a missing approved subdirectory.
    auto oldWorking=std::filesystem::current_path();std::filesystem::current_path(runtime);
    FsrRuntime missing;auto rejected=missing.Load(staging/"missing");std::filesystem::current_path(oldWorking);
    Require(!rejected && !GetModuleHandleW(L"amd_fidelityfx_loader_dx12.dll"),"missing optional runtime fails without working-directory fallback");
    std::filesystem::create_directories(staging/"FSR");
    for(auto name:{"amd_fidelityfx_loader_dx12.dll","amd_fidelityfx_upscaler_dx12.dll"})std::filesystem::copy_file(runtime/name,staging/"FSR"/name,std::filesystem::copy_options::overwrite_existing);
    Rig rig(true);
    ComPtr<ID3D11DeviceContext1> context1;Check(rig.context11.As(&context1),"native UI rectangle context");
    HWND window=CreateWindowExW(0,L"STATIC",L"FSR clean-process helper",WS_OVERLAPPEDWINDOW,0,0,800,600,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);Require(window!=nullptr,"hidden window");
    DXGI_SWAP_CHAIN_DESC desc{};desc.BufferDesc.Width=321;desc.BufferDesc.Height=181;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=1;desc.OutputWindow=window;desc.Windowed=TRUE;
    OrdinaryPresentation ordinary;ComPtr<IDXGISwapChain> chain;Check(ordinary.CreateSwapChain(rig.factory.Get(),rig.device11.Get(),desc,&chain,&IDXGIFactory::CreateSwapChain),"production ordinary presenter");
    PresentationTargets presentation;
    Check(presentation.CacheAfterRetirement(chain.Get(),PresentationBufferAccess::D3D11Current),"production host caches ordinary D3D11 buffers");
    Require(presentation.Buffers().size()==1,"D3D11 caches only the current buffer");
    Require(presentation.BufferIndex(0)==0 && presentation.BufferIndex(1)==0,"physical flip indices resolve to D3D11 buffer zero");
    PresentationTargets indexedPresentation;
    Require(indexedPresentation.BufferIndex(0)==0 && indexedPresentation.BufferIndex(1)==1,"NVIDIA indexed selection remains unchanged");
    FsrHostResources fsr(staging);BackendConfiguration configuration;configuration.backend=BackendKind::Fsr;configuration.generationEnabled=false;configuration.generationBackend=0;
    auto render=Value(fsr.PrepareSizing(rig.device11.Get(),configuration,{321,181},DXGI_FORMAT_R8G8B8A8_UNORM,ColorEncoding::Gamma22));Require(!fsr.FeatureReady(),"sizing before temporal context");
    Check(chain->Present(0,0),"ordinary early startup Present without feature");Accepted(fsr.CompleteStartup());Require(fsr.FeatureReady(),"deferred production FSR startup");auto provider=fsr.Provider();
    auto sourceDesc=rig.Description();sourceDesc.Width=render.width;sourceDesc.Height=render.height;ComPtr<ID3D11Texture2D> source;Check(rig.device11->CreateTexture2D(&sourceDesc,nullptr,&source),"source texture");
    ComPtr<ID3D11RenderTargetView> sourceRTV,depthRTV,motionRTV;Check(rig.device11->CreateRenderTargetView(source.Get(),nullptr,&sourceRTV),"source RTV");Check(rig.device11->CreateRenderTargetView(fsr.Depth11(),nullptr,&depthRTV),"depth RTV");Check(rig.device11->CreateRenderTargetView(fsr.Motion11(),nullptr,&motionRTV),"motion RTV");
    ComPtr<ID3D11Texture2D> readback;auto nativeDesc=sourceDesc;nativeDesc.Width=321;nativeDesc.Height=181;nativeDesc.Usage=D3D11_USAGE_STAGING;nativeDesc.BindFlags=0;nativeDesc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;Check(rig.device11->CreateTexture2D(&nativeDesc,nullptr,&readback),"native readback");
    std::uint64_t previousHash{},changing{};std::vector<std::string> during;
    {
        FsrFrameAdapter adapter(*fsr.Upscaler(),fsr.Bridge(),fsr.Resources(),fsr.Color11(),fsr.Depth11(),fsr.Motion11(),fsr.Output11(),ColorEncoding::Gamma22);
        for(unsigned index=0;index<12;++index) {
            ComPtr<IDXGISwapChain3> chain3;Check(chain.As(&chain3),"ordinary chain3");
            const auto slot=presentation.BufferIndex(chain3->GetCurrentBackBufferIndex());
            Check(presentation.Select(rig.device11.Get(),slot),"production UI selects current D3D11 target");
            ComPtr<ID3D11Texture2D> native=presentation.Buffers()[slot];
            ComPtr<ID3D11Texture2D> current;Check(chain->GetBuffer(0,IID_PPV_ARGS(&current)),"native D3D11 buffer zero");
            Require(native.Get()==current.Get(),"cached buffer remains the D3D11 current identity across Present");
            const float color[]{0.15f+float(index)*0.03f,.35f,.6f,1},depth[]{1,0,0,0},motion[]{0,0,0,0};
            rig.context11->ClearRenderTargetView(sourceRTV.Get(),color);rig.context11->ClearRenderTargetView(depthRTV.Get(),depth);rig.context11->ClearRenderTargetView(motionRTV.Get(),motion);
            UpscaleFrame frame;frame.backend=BackendKind::Fsr;frame.color=frame.input=source.Get();frame.depth=fsr.Depth11();frame.motion=fsr.Motion11();frame.output=native.Get();frame.render=frame.subrect=render;frame.display={321,181};
            frame.colorFormat=DXGI_FORMAT_R8G8B8A8_UNORM;frame.depthFormat=DXGI_FORMAT_R32_FLOAT;frame.motionFormat=DXGI_FORMAT_R16G16_FLOAT;frame.motionConvention={float(render.width),float(render.height),true,false};frame.deltaMilliseconds=16;frame.sourceId=index+1;
            frame.camera.identity=1;frame.camera.nearDistance=.1f;frame.camera.farDistance=100;frame.camera.verticalFovRadians=1;
            DirectX::XMFLOAT4X4 view,projection;DirectX::XMStoreFloat4x4(&view,DirectX::XMMatrixIdentity());DirectX::XMStoreFloat4x4(&projection,DirectX::XMMatrixPerspectiveFovLH(1,float(render.width)/render.height,.1f,100));std::memcpy(frame.camera.view.data(),&view,sizeof(view));std::memcpy(frame.camera.projection.data(),&projection,sizeof(projection));
            auto jitter=Value(fsr.Upscaler()->QueryJitter(frame.sourceId));frame.jitterX=jitter[0];frame.jitterY=jitter[1];Require(Value(adapter.Evaluate(frame))==UpscaleOutcome::Temporal,"official FSR adapter delivers temporal native output");
            const float uiColor[]{1,0,1,1};const D3D11_RECT uiRect{0,0,4,4};
            context1->ClearView(presentation.RTV(),uiColor,&uiRect,1);
            // Readback proves consumed native pixels independently of frame/Present counters.
            rig.context11->CopyResource(readback.Get(),native.Get());Check(ordinary.Retire(),"retire native readers");Check(fsr.Bridge()->Drain(),"FSR transfer retirement");D3D11_MAPPED_SUBRESOURCE mapped{};Check(rig.context11->Map(readback.Get(),0,D3D11_MAP_READ,0,&mapped),"read delivered native pixels");std::uint64_t hash=1469598103934665603ull,energy{};
            for(unsigned y=0;y<181;++y){auto* row=static_cast<unsigned char*>(mapped.pData)+y*mapped.RowPitch;for(unsigned x=0;x<321*4;++x){hash=(hash^row[x])*1099511628211ull;energy+=row[x];}}
            auto* uiPixel=static_cast<unsigned char*>(mapped.pData);
            Require(uiPixel[0]==255 && uiPixel[1]==0 && uiPixel[2]==255 && uiPixel[3]==255,"native UI sentinel reaches the reconstructed presentation target");
            rig.context11->Unmap(readback.Get(),0);Require(energy>321*181*100,"native output is nonempty");if(index && hash!=previousHash)++changing;previousHash=hash;Check(chain->Present(0,0),"ordinary Present after reconstruction");
        }
        during=Modules();
    }
    Require(changing>=10,"native outputs change across real dispatches");Accepted(fsr.Retire());Check(ordinary.BeforeResize(),"ordinary resize retirement");
    rig.context11->ClearState();presentation.ResetAfterRetirement();
    Check(chain->ResizeBuffers(2,323,183,DXGI_FORMAT_R8G8B8A8_UNORM,0),"ordinary resize after releasing cached UI and backbuffer references");
    Check(ordinary.AfterResize(S_OK),"ordinary resized presentation admission");
    Check(presentation.CacheAfterRetirement(chain.Get(),PresentationBufferAccess::D3D11Current),"production host recaches resized D3D11 buffer");
    Check(presentation.Select(rig.device11.Get(),presentation.BufferIndex(1)),"resized native UI selection");
    D3D11_TEXTURE2D_DESC resized{};presentation.Texture()->GetDesc(&resized);Require(resized.Width==323 && resized.Height==183,"resized cache uses new native extent");
    Check(ordinary.Retire(),"ordinary final retirement");rig.context11->ClearState();presentation.ResetAfterRetirement();rig.ValidateDebug();chain.Reset();ordinary.ResetAfterRetirement();DestroyWindow(window);
    Modules();Require(!GetModuleHandleW(L"amd_fidelityfx_loader_dx12.dll") && !GetModuleHandleW(L"amd_fidelityfx_upscaler_dx12.dll"),"AMD modules unload after proven retirement");
    std::ofstream output(report);output<<"{\n\"result\":\"PASS\",\"pluginImportLoadSucceeded\":true,\"ordinaryHelperStartupSucceeded\":true,\"temporalFrames\":12,\"changingNativeReadbacks\":"<<changing<<",\"missingRuntimeRejectedWithoutCwdFallback\":true,\"loadedNvidiaRuntimeModules\":0,\"providerId\":"<<provider.id<<",\"providerName\":"<<Json(provider.name)<<",\"skyrimStartupTested\":false,\"crossVendorTested\":false,\"frameGenerationTested\":false,\n\"loadedModulesDuringDispatch\":[";
    for(std::size_t i=0;i<during.size();++i){if(i)output<<',';output<<Json(during[i]);}output<<"]\n}\n";Require(bool(output),"startup report saved");FreeLibrary(pluginModule);
    std::puts("PASS: fresh-process plugin load; separate ordinary/FSR helper startup and native pixels; zero NVIDIA runtime modules. Skyrim startup NOT RUN.");return 0;
}
