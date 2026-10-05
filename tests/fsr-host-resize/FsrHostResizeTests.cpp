#include "FrameGen/SourceHostLifecycle.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

namespace TheosRenderPipeline::SourceDLSSG {
struct Backend { static Backend& Get(){ static Backend value;return value;} bool Quiesce(){return true;} };
}
namespace TheosRenderPipeline::NativeInput { void Publish(unsigned,unsigned){} }
struct RetirementResult {
    bool succeeded{true};
    explicit operator bool() const { return succeeded; }
    struct Error { std::string message{"reader retirement blocked"}; };
    Error error() const { return {}; }
};
struct FsrResources {
    std::shared_ptr<int> runtime{std::make_shared<int>(1)},device{std::make_shared<int>(2)},bridge{std::make_shared<int>(3)};
    bool sized{true},canRetire{true};
    std::vector<int>* events{};
    RetirementResult ReleaseSizedAfterRetirement(){events->push_back(2);if(!canRetire)return {false};sized=false;return {};}
    RetirementResult Retire(){auto result=ReleaseSizedAfterRetirement();if(result){runtime.reset();device.reset();bridge.reset();}return result;}
};
struct NvidiaHost {
    struct LifecycleOperations;
    struct Ordinary { std::vector<int>* events{};HRESULT Retire(){events->push_back(1);return S_OK;}HRESULT BeforeResize(){return S_OK;} } ordinaryPresentation_;
    struct Context { std::vector<int>* events{};void ClearState(){events->push_back(4);}void Flush(){} } contextStorage;
    Context* context_{&contextStorage};
    struct Targets { bool gameFacing{true};void ResetGameFacingAfterRetirement(){gameFacing=false;} } gameTargets_;
    struct Presentation { void ResetAfterRetirement(){} } presentation_;
    std::shared_ptr<FsrResources> fsrResources_{std::make_shared<FsrResources>()};
    std::vector<int> events;
    std::string status_;
    bool resetNextEvaluation_{},proxyActive_{true},sourceUpscalerInitializationPending_{},fsrSizingRetainedForResize_{};
    IDXGISwapChain* outerSwapChain_{},*innerSwapChain_{};
    NvidiaHost(){ordinaryPresentation_.events=&events;contextStorage.events=&events;fsrResources_->events=&events;}
    bool FsrActive() const {return true;}
    bool FsrFgActive() const {return false;}
    bool RetireCommunityNeural(){events.push_back(0);return true;}
    bool CreateGameFacingResources(IDXGISwapChain*){return true;}
    bool CompleteStartupAfterDeviceCreation(){return true;}
    HRESULT FailLifecycle(HRESULT result,const char*){return result;}
    HRESULT FailureResult() const {return S_OK;}
    HRESULT BeforeResizeBuffers(IDXGISwapChain*);
    void SetRuntimeEnabled(bool){}
    void EndNativeUIPass(){events.push_back(3);}
    void ReleaseSourceUpscaler(bool retainFsrDevice=false){if(!retainFsrDevice)fsrResources_->Retire();}
    void ResetSessionAfterRetirement(){fsrSizingRetainedForResize_=false;}
};
#include "LifecycleAdapter.inc"
#include "BeforeResize.inc"
static void Require(bool condition,const char* name){if(!condition){std::fprintf(stderr,"FAIL: %s\n",name);std::exit(1);}}
int main(){
    NvidiaHost resize;auto runtime=resize.fsrResources_->runtime,device=resize.fsrResources_->device,bridge=resize.fsrResources_->bridge;
    Require(resize.BeforeResizeBuffers(nullptr)==S_OK,"ordinary resize retirement succeeds");
    Require(resize.fsrResources_->runtime==runtime && resize.fsrResources_->device==device && resize.fsrResources_->bridge==bridge,
        "OrdinaryResizePreservesRuntimeDeviceAndBridge");
    Require(!resize.fsrResources_->sized && !resize.gameTargets_.gameFacing && resize.fsrSizingRetainedForResize_,"OnlySizedAllocationsReleasedAfterResizeProof");
    Require(resize.events==std::vector<int>({0,1,2,3,4}),"NrAndPresentationReadersRetireBeforeSizedRelease");
    NvidiaHost blocked;blocked.fsrResources_->canRetire=false;auto owned=blocked.fsrResources_->runtime;
    Require(blocked.BeforeResizeBuffers(nullptr)==DXGI_ERROR_WAS_STILL_DRAWING,"FailedRetirementRejectsResize");
    Require(blocked.fsrResources_->runtime==owned && blocked.fsrResources_->sized && blocked.gameTargets_.gameFacing && !blocked.fsrSizingRetainedForResize_,
        "FailedRetirementKeepsOwnershipAndDoesNotAuthorizeReuse");
    NvidiaHost destroy;destroy.fsrSizingRetainedForResize_=true;NvidiaHost::LifecycleOperations destroyOps{destroy};
    Require(TheosRenderPipeline::SourceHostLifecycle::Destroy(destroyOps),"destruction retirement succeeds");
    Require(!destroy.fsrResources_->runtime && !destroy.fsrResources_->device && !destroy.fsrResources_->bridge && !destroy.fsrSizingRetainedForResize_,
        "FullDestructionReleasesRetainedDeviceAndRuntime");
    std::puts("PASS: ordinary FSR resize ownership and failed-retirement preservation");
}
