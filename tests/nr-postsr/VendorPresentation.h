#pragma once
#include "FrameGen/SourceDLSSGSession.h"
#include "NeuralRendering/RuntimeFileLease.h"
#include "PluginPaths.h"
#include <dxgi1_6.h>
#include <cstdio>
#include <thread>
#include <atomic>
#include <vector>

// Test-only public Streamline bootstrap. It mirrors the production manual
// factory-proxy route; the production Session/Interop remain unchanged.
// Libraries and their verified leases stay process-owned, including on errors.
struct VendorPresentation {
    struct ForegroundUnavailable : std::runtime_error {ForegroundUnavailable():std::runtime_error("Visible vendor FG probe must stay in the foreground"){} };
    TheosRenderPipeline::SourceDLSSG::SessionAPI api;
    Microsoft::WRL::ComPtr<ID3D12Device> upgradedDevice;
    Microsoft::WRL::ComPtr<IDXGIFactory> upgradedFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> chain;
    HMODULE interposer{};
    HWND window{};
    PFun_slUpgradeInterface* upgrade{};
    unsigned presents{},onSamples{},offSamples{},doubleSamples{},readerSamples{},recoveries{};
    unsigned phaseDoubles[4]{};
    bool forceOff{};
    bool injectForegroundLoss{};
    static inline std::atomic<unsigned> apiErrors{},lastApiError{};
    static void ApiError(const sl::APIError& error){lastApiError.store(unsigned(error.hres));++apiErrors;}
    void RequireForeground(bool wait){
        const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(wait?30:0);
        while(GetForegroundWindow()!=window&&std::chrono::steady_clock::now()<deadline){
            MSG message{};while(PeekMessageW(&message,nullptr,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageW(&message);}
            if(!IsWindow(window))break;std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        const auto foreground=GetForegroundWindow();
        if(foreground!=window){
            DWORD foregroundPid{};
            const auto foregroundThread=GetWindowThreadProcessId(foreground,&foregroundPid);
            char foregroundClass[128]{};
            if(foreground)GetClassNameA(foreground,foregroundClass,sizeof(foregroundClass));
            std::printf("VENDOR_FOCUS_LOST present=%u expected=%p foreground=%p foregroundPid=%lu foregroundThread=%lu class=%s iconic=%u active=%u\n",
                presents,static_cast<void*>(window),static_cast<void*>(foreground),foregroundPid,foregroundThread,
                foregroundClass,IsIconic(window)!=FALSE,GetActiveWindow()==window);
            throw ForegroundUnavailable();
        }
        if(injectForegroundLoss&&presents==631){
            std::puts("VENDOR_INTERRUPT_INJECTED present=631");
            throw ForegroundUnavailable();
        }
    }

    static void Hr(HRESULT hr){if(FAILED(hr))throw hr;}
    static void Result(sl::Result result,const char* operation){
        if(result!=sl::Result::eOk)throw std::runtime_error(std::string(operation)+" Streamline="+std::to_string(unsigned(result)));
    }
    template<class T> void Export(T& function,const char* name){
        function=reinterpret_cast<T>(GetProcAddress(interposer,name));
        if(!function)throw std::runtime_error(std::string("Missing public export ")+name);
    }
    void Load(const std::filesystem::path& root,ID3D12Device* device){
        namespace NR=TheosRenderPipeline::NeuralRendering;
        namespace Paths=TheosRenderPipeline::PluginPaths;
        const NR::RuntimeProfile pins[]{
            {"fg","nvngx_dlssg.dll","5d5cbf14d2727d47f93fd10bf77bd91708ae122482a6f86fd564971641ebd47b",7453808},
            {"common","sl.common.dll","a4b2b5acbe49fbc6d44dd432cac19cd53218f698b2539dc7ed0fb268c72cfc8d",830592},
            {"dlss-g","sl.dlss_g.dll","b8b5effd7debdb750abd216de43385fb653261712bc315d85eba68811fb3ee02",625792},
            {"interposer","sl.interposer.dll","27b2190057994c0b287c2c5716953bf1586f6499ac12fbbb2092b9aaf8396570",651392},
            {"pcl","sl.pcl.dll","12aa4e76c28a27c735e4ecb3072f44d09428acb107b70ac38e4bd48ddb05f88d",360064},
            {"reflex","sl.reflex.dll","ecf12973cdcec2ffced2ea77b1c7e45f4d387e7c864ddb5531b66a6f947effb3",382080}};
        if(!root.is_absolute())throw std::runtime_error("Vendor probe needs absolute isolated runtime root");
        // Leases intentionally live with the loaded modules until process exit.
        auto* leases=new std::vector<NR::RuntimeFileLease>;
        for(const auto& pin:pins){
            const auto path=root/pin.relativePath;
            if(Paths::HasConflictingLoadedModule(path,false))throw std::runtime_error("Conflicting vendor module owner");
            auto held=NR::RuntimeFileLease::Open(path,pin);
            if(!held)throw std::runtime_error(held.error().message);
            leases->push_back(std::move(*held));
        }
        interposer=LoadLibraryExW((root/L"sl.interposer.dll").c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if(!interposer)throw HRESULT_FROM_WIN32(GetLastError());
        PFun_slInit* init{};PFun_slSetFeatureLoaded* loadFeature{};PFun_slSetD3DDevice* setDevice{};
        PFun_slIsFeatureSupported* supported{};PFun_slGetFeatureFunction* featureFunction{};
        Export(init,"slInit");Export(loadFeature,"slSetFeatureLoaded");Export(setDevice,"slSetD3DDevice");
        Export(supported,"slIsFeatureSupported");Export(featureFunction,"slGetFeatureFunction");Export(upgrade,"slUpgradeInterface");
        Export(api.newFrameToken,"slGetNewFrameToken");Export(api.setConstants,"slSetConstants");Export(api.setTag,"slSetTag");
        const wchar_t* paths[]{root.c_str()};const sl::Feature features[]{sl::kFeatureReflex,sl::kFeatureDLSS_G};
        sl::Preferences preferences{};preferences.pathsToPlugins=paths;preferences.numPathsToPlugins=1;
        preferences.featuresToLoad=features;preferences.numFeaturesToLoad=2;preferences.renderAPI=sl::RenderAPI::eD3D12;
        preferences.engine=sl::EngineType::eCustom;preferences.engineVersion="TheosRenderPipeline 0.1";
        preferences.projectId="f1b2e5d8-9c4a-4e7b-8a36-5d2e90c47a11";
        preferences.flags=sl::PreferenceFlags::eDisableCLStateTracking|sl::PreferenceFlags::eUseManualHooking|sl::PreferenceFlags::eUseDXGIFactoryProxy;
        preferences.logLevel=sl::LogLevel::eDefault;
        preferences.logMessageCallback=[](sl::LogType type,const char* message){std::printf("VENDOR_LOG %u %s\n",unsigned(type),message?message:"");};
        Result(init(preferences,sl::kSDKVersion),"slInit");
        for(auto feature:features)Result(loadFeature(feature,true),"slSetFeatureLoaded");
        void* proxy=device;device->AddRef();auto result=upgrade(&proxy);upgradedDevice.Attach(static_cast<ID3D12Device*>(proxy));
        Result(result,"upgrade device");Result(setDevice(device),"slSetD3DDevice");
        auto luid=device->GetAdapterLuid();sl::AdapterInfo info{};info.deviceLUID=reinterpret_cast<uint8_t*>(&luid);info.deviceLUIDSizeInBytes=sizeof(luid);
        for(auto feature:{sl::kFeatureReflex,sl::kFeaturePCL,sl::kFeatureDLSS_G})Result(supported(feature,info),"slIsFeatureSupported");
        for(size_t i=0;i<std::size(pins);++i){
            auto module=Paths::RetainLoadedModule(root/pins[i].relativePath);
            if(!module||!(*leases)[i].Matches(Paths::ModulePath(module)))throw std::runtime_error("Loaded vendor path differs from pinned lease");
        }
        auto resolve=[&](sl::Feature feature,const char* name,auto& function){void* address{};Result(featureFunction(feature,name,address),name);
            if(!address)throw std::runtime_error(name);function=reinterpret_cast<std::remove_reference_t<decltype(function)>>(address);};
        resolve(sl::kFeatureReflex,"slReflexSetOptions",api.setReflexOptions);resolve(sl::kFeatureReflex,"slReflexSleep",api.reflexSleep);
        resolve(sl::kFeatureReflex,"slReflexGetState",api.getReflexState);resolve(sl::kFeaturePCL,"slPCLSetMarker",api.marker);
        resolve(sl::kFeatureDLSS_G,"slDLSSGGetState",api.getState);resolve(sl::kFeatureDLSS_G,"slDLSSGSetOptions",api.setOptions);
        std::puts("VENDOR_BOOTSTRAP six held modules; SDK 2.11.1; manual proxy; OTA off; no MFG unlock");
    }
    void Create(IDXGIFactory* factory,ID3D12CommandQueue* queue,UINT width,UINT height){
        Microsoft::WRL::ComPtr<IDXGIFactory5> capabilities;Hr(factory->QueryInterface(IID_PPV_ARGS(&capabilities)));
        BOOL tearing{};Hr(capabilities->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,&tearing,sizeof(tearing)));
        if(!tearing)throw std::runtime_error("Immediate Presents require tearing support");
        void* proxy=factory;factory->AddRef();auto result=upgrade(&proxy);upgradedFactory.Attach(static_cast<IDXGIFactory*>(proxy));Result(result,"upgrade factory");
        WNDCLASSW wc{};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"TRPNrVendorFgProbe";
        if(!RegisterClassW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)throw HRESULT_FROM_WIN32(GetLastError());
        window=CreateWindowExW(0,wc.lpszClassName,L"DvaKolbas isolated NVIDIA FG check",WS_POPUP|WS_VISIBLE,60,60,width,height,nullptr,nullptr,wc.hInstance,nullptr);
        if(!window)throw HRESULT_FROM_WIN32(GetLastError());
        const bool activated=SetForegroundWindow(window)!=FALSE;
        std::printf("VENDOR_ACTIVATION requested=1 granted=%u foreground=%u\n",activated,GetForegroundWindow()==window);
        DXGI_SWAP_CHAIN_DESC desc{};desc.BufferDesc.Width=width;desc.BufferDesc.Height=height;desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=2;desc.OutputWindow=window;desc.Windowed=TRUE;
        desc.SwapEffect=DXGI_SWAP_EFFECT_FLIP_DISCARD;desc.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        Microsoft::WRL::ComPtr<IDXGISwapChain> native;Hr(upgradedFactory->CreateSwapChain(queue,&desc,&native));Hr(native.As(&chain));
        Hr(upgradedFactory->MakeWindowAssociation(window,DXGI_MWA_NO_ALT_ENTER));
    }
    void Resize(UINT width,UINT height){
        Hr(chain->ResizeBuffers(2,width,height,DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING));
        if(!SetWindowPos(window,nullptr,60,60,width,height,SWP_NOZORDER|SWP_NOACTIVATE))throw HRESULT_FROM_WIN32(GetLastError());
    }
    HRESULT Present(){
        MSG message{};while(PeekMessageW(&message,nullptr,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageW(&message);}
        if(!IsWindow(window)||IsIconic(window))return E_ABORT;
        RequireForeground(false);
        if(presents%100==0)std::printf("VENDOR_WINDOW present=%u foreground=%u active=%u apiErrors=%u lastError=0x%08x\n",presents,GetForegroundWindow()==window,GetActiveWindow()==window,apiErrors.load(),lastApiError.load());
        const auto hr=chain->Present(0,DXGI_PRESENT_ALLOW_TEARING);
        if(hr!=S_OK)return FAILED(hr)?hr:E_FAIL; // Occlusion cannot qualify output.
        ++presents;return hr;
    }
    void Sample(const TheosRenderPipeline::SourceDLSSG::SessionSnapshot& snapshot,unsigned phase){
        const bool on=TheosRenderPipeline::SourceDLSSG::GenerationEnabled(snapshot.options.mode);
        if(on){++onSamples;if(snapshot.state.numFramesActuallyPresented==2){++doubleSamples;++phaseDoubles[phase];}}
        else ++offSamples;
        if(snapshot.state.inputsProcessingCompletionFence&&snapshot.state.lastPresentInputsProcessingCompletionFenceValue)++readerSamples;
    }
    void Close(){chain.Reset();upgradedFactory.Reset();upgradedDevice.Reset();if(window){DestroyWindow(window);window=nullptr;}}
};
