// Compile the actual host control flow with CPU facades; no vendor or GPU work.
#include <dxgi1_4.h>
#include <d3d11.h>
#include "Upscaling/UpscalerBackend.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <format>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <optional>
struct Indexed { UINT GetCurrentBackBufferIndex(){return 0;} };
namespace Microsoft::WRL { template<class T> struct ComPtr { Indexed* value{};Indexed** operator&(){return &value;}Indexed* operator->(){return value;} }; }
#undef IID_PPV_ARGS
#define IID_PPV_ARGS(value) value
struct Chain {Indexed index;HRESULT QueryInterface(Indexed** out){*out=&index;return S_OK;}HRESULT GetDesc(DXGI_SWAP_CHAIN_DESC*){return S_OK;} };
struct Texture {void GetDesc(D3D11_TEXTURE2D_DESC* d){d->Width=d->Height=64;}};
struct Buffer {void* Get(){return this;}};
struct Context {explicit operator bool()const{return true;}Context* Get(){return this;}Context* operator->(){return this;}void CopyResource(void*,void*){}void ClearState(){}void Flush(){}};
struct RenderPipeline {
    struct Image {Texture* mImage{};} mMotionVectors,mDepthBuffer;
    unsigned mPendingHistoryResets{};
    static RenderPipeline* GetSingleton(){static RenderPipeline p;return &p;}
};
namespace logger {template<class... T> void info(T&&...){}template<class... T> void error(T&&...){} }
struct ScopedD3D11PerformanceStage {template<class... T> ScopedD3D11PerformanceStage(T&&...){} };
namespace TheosRenderPipeline {
struct PerformanceTuning {struct Settings {struct Diagnostics {bool frameDetails{};} diagnostics;} settings;enum class D3D11Stage{kPresentationCopy};static PerformanceTuning* GetSingleton(){static PerformanceTuning p;return &p;}};
struct GameSwapChain {};
struct FsrHostResize {Upscaling::Extent render;HRESULT result{S_OK};};
inline HRESULT ValidateFsrResize(UINT,const UINT*,IUnknown* const*){return S_OK;}
inline HRESULT ValidateFsrResizeFlags(UINT,UINT){return S_OK;}
struct FsrPresentation {static Upscaling::Result<std::optional<DXGI_SWAP_CHAIN_DESC>> TranslateResizeDescriptor(const DXGI_SWAP_CHAIN_DESC& d){return std::optional{d};}};
namespace SourceDLSSG {inline bool querySucceeds{};inline unsigned queries{};inline bool QueryRenderSize(UINT,UINT,int,int* w,int* h){++queries;*w=64;*h=64;return querySucceeds;}}
namespace NeuralRendering {template<class A,class B> HRESULT RetireBeforeSourceResize(A a,B b){auto hr=a();return FAILED(hr)?hr:b();}}
}
using namespace TheosRenderPipeline;
struct NvidiaHost {
    bool resetNextEvaluation_{},proxyActive_{true},splitSourceDLSSActive_{true},upscalerReady_{true},evaluationFailureLogged_{},splitSourceRuntimeFailureLogged_{},sourceRecoveryActive_{};
    bool fsrSourcePending_{},fsrUiComplete_{},frameGenerationEnabled_{},sourceUpscalerInitializationPending_{};
    int renderWidth_{64},renderHeight_{64},outputWidth_{64},outputHeight_{64},warmupPresentsRemaining_{};
    unsigned long long evaluationCount_{},upscaleEvaluationCount_{},presentCount_{};
    Upscaling::UpscaleOutcome fsrGenerationOutcome_{Upscaling::UpscaleOutcome::SkippedInvalidInput};
    bool recovery{},mixed{true},configured{};unsigned retired{};std::vector<bool> resets;
    std::string status_;GameSwapChain outer;void* outerSwapChain_{&outer};Chain chain;Chain* innerSwapChain_{&chain};
    Context context_;
    struct Targets {bool live{true};void* GameFacing(){return live?this:nullptr;}void* UpscaleInput(){return GameFacing();}void* UpscaleOutput(){return GameFacing();}void ResetGameFacingAfterRetirement(){live=false;}} gameTargets_;
    struct Presentation {std::vector<Buffer> buffers{1};auto& Buffers(){return buffers;}UINT BufferIndex(UINT i){return i;}void ResetAfterRetirement(){buffers.clear();}} presentation_;
    struct Foreground {void Reset(){}} fsrForeground_;
    struct NativePass {void ResetEvaluation(){}} nativeUIPass_;
    struct Settings {struct Creation {int AllocationQuality(){return 2;}} creation;Creation& Startup(){return creation;}} sourceUpscalerSettings_;
    struct Presenter {NvidiaHost* host;HRESULT WaitBeforeProducer(){return S_OK;}Upscaling::Result<void> Suspend(){return {};}Upscaling::Result<void> BeforeResize(){++host->retired;return {};}Upscaling::Result<FsrHostResize> Resize(const DXGI_SWAP_CHAIN_DESC&){return FsrHostResize{{64,64}};}Upscaling::Result<FsrHostResize> ResizeExternal(const DXGI_SWAP_CHAIN_DESC&,Upscaling::Extent e){return FsrHostResize{e};}} presenter{this};Presenter* fsrPresentation_{&presenter};
    DXGI_SWAP_CHAIN_DESC fsrDescriptor_{};
    bool FsrActive()const{return false;}bool FsrFgActive()const{return mixed;}HRESULT FailureResult()const{return S_OK;}HRESULT UpdateFsrSuspension(){return S_OK;}bool FsrPresentSuspended(){return false;}
    bool xess{};bool XessActive()const{return xess;}
#if defined(TRP_ENABLE_XESS)
    struct XessResources {
        bool ownerThread{true},canRetire{true};unsigned retirements{};
        XessResources* Upscaler(){return this;}bool OnOwnerThread(){return ownerThread;}
        Upscaling::Result<void> Retire(){++retirements;if(!canRetire)return std::unexpected(Upscaling::RuntimeError{Upscaling::ErrorKind::RetirementFailure,E_FAIL,"XeSS reader blocked"});return {};}
    } xessOwner;
    XessResources* xessResources_{&xessOwner};
#endif
    bool EvaluateXessFrame(IDXGISwapChain*,bool){throw std::runtime_error("unexpected XeSS route in DLSS/FSR fixture");}
    bool PresentationBackendReadyForEvaluation(){return true;}bool EvaluateFsrFrame(IDXGISwapChain*,bool){return false;}
    bool EvaluateSourceNvidiaFrame(bool,bool reset){resets.push_back(reset);if(recovery){resetNextEvaluation_=sourceRecoveryActive_=true;status_="source recovery remains visible";}return true;}
    bool StartupConfigured(){return configured;}void SetRuntimeEnabled(bool){}HRESULT FailLifecycle(HRESULT h,const char*){return h;}
    bool RetireCommunityNeural(){++retired;return true;}void EndNativeUIPass(){}void ReleaseSourceUpscaler(bool){}bool CreateGameFacingResources(Chain*){gameTargets_.live=true;return true;}bool CompleteStartupAfterDeviceCreation(){return true;}
    bool EvaluateFrame(IDXGISwapChain*,bool);HRESULT ResizeFsrSwapChain(GameSwapChain&,UINT,UINT,UINT,DXGI_FORMAT,UINT,const UINT*,IUnknown* const*);
};
#include "MixedEvaluate.inc"
#include "MixedResize.inc"
static void Require(bool b,const char* why){if(!b){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
int main(){
    NvidiaHost h;RenderPipeline::GetSingleton()->mPendingHistoryResets=3;
    for(int i=0;i<8;++i)Require(h.EvaluateFrame(reinterpret_cast<IDXGISwapChain*>(h.outerSwapChain_),true),"mixed source evaluation");
    Require(h.resets==std::vector<bool>({true,true,true,false,false,false,false,false}),"FG skips must not repeatedly reset DLSS or retain pending resets");
    h.configured=true;h.recovery=true;Require(h.EvaluateFrame(reinterpret_cast<IDXGISwapChain*>(h.outerSwapChain_),true),"spatial recovery");
    Require(h.status_=="source recovery remains visible","normal backend banner must not erase active source recovery error");h.recovery=false;
    Require(h.EvaluateFrame(reinterpret_cast<IDXGISwapChain*>(h.outerSwapChain_),true)&&h.resets.back(),"real source failure must still reset the following DLSS source");
    NvidiaHost resize;SourceDLSSG::querySucceeds=false;
    Require(resize.ResizeFsrSwapChain(resize.outer,2,128,128,DXGI_FORMAT_R8G8B8A8_UNORM,0,nullptr,nullptr)==E_INVALIDARG,"failed render-size query rejected");
    Require(resize.retired==0&&resize.gameTargets_.live&&!resize.presentation_.buffers.empty(),"failed resize query must retain the running source and presenter");
#if defined(TRP_ENABLE_XESS)
    NvidiaHost xess;xess.xess=true;xess.xessOwner.ownerThread=false;
    Require(xess.ResizeFsrSwapChain(xess.outer,2,64,64,DXGI_FORMAT_R8G8B8A8_UNORM,0,nullptr,nullptr)==DXGI_ERROR_WAS_STILL_DRAWING && !xess.retired,
        "off-owner XeSS FSR resize retains SDK and presenter");
    xess.xessOwner.ownerThread=true;const auto queries=SourceDLSSG::queries;
    Require(xess.ResizeFsrSwapChain(xess.outer,2,128,128,DXGI_FORMAT_R8G8B8A8_UNORM,0,nullptr,nullptr)==E_INVALIDARG && !xess.retired && xess.gameTargets_.live && SourceDLSSG::queries==queries,
        "unqualified new XeSS output size rejected before retirement without DLSS sizing");
    Require(xess.ResizeFsrSwapChain(xess.outer,2,64,64,DXGI_FORMAT_R8G8B8A8_UNORM,0,nullptr,nullptr)==S_OK && xess.retired==2 && SourceDLSSG::queries==queries,
        "same-size owner-thread XeSS restore retains SDK dimensions and rebuilds after reader retirement");
    NvidiaHost blocked;blocked.xess=true;blocked.xessOwner.canRetire=false;
    Require(blocked.ResizeFsrSwapChain(blocked.outer,2,64,64,DXGI_FORMAT_R8G8B8A8_UNORM,0,nullptr,nullptr)==E_FAIL && blocked.gameTargets_.live &&
        !blocked.presentation_.buffers.empty() && blocked.xessOwner.retirements==1,
        "blocked XeSS reader during FSR resize retains source before any buffer release or reconstruction");
#endif
    std::puts("PASS: actual mixed-host FG skips, source reset consumption and non-destructive resize query failure");
}
