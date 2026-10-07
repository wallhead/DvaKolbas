#include "Upscaling/FSRFrameGeneration.h"
#include "fsr-fg/GenerationTestRig.h"
#include <dx12/ffx_api_dx12.h>
#include <cstring>
#include <future>
#include <thread>
using namespace GenerationFixture;
static void CheckDestructorSerialization()
{
    auto session = std::make_shared<FsrSdkSession>();
    auto owner = std::make_unique<FsrFrameGeneration>(session);
    std::promise<void> entered, finished; auto start = entered.get_future(); auto done = finished.get_future();
    std::thread worker;
    {
        auto lock = session->Lock(); Require(lock.Owns(*session), "idle test session lock owned");
        worker = std::thread([&] { entered.set_value(); owner.reset(); finished.set_value(); });
        start.wait();
        Require(done.wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout, "destructor owner inspection serialized with SDK operations");
    }
    Require(done.wait_for(std::chrono::seconds(2)) == std::future_status::ready, "destructor progresses when SDK lock is released");
    worker.join();
}
int main(int argc, char** argv)
{
    Require(argc == 2, "vendor fixture root supplied"); Rig rig;
    auto runtime = std::make_shared<FsrRuntime>(); Require(bool(runtime->Load(std::filesystem::absolute(argv[1]))), "SR runtime loads");
    Require(bool(runtime->LoadFrameGeneration(std::filesystem::absolute(argv[1]))), "FG runtime loads");
    auto dll = GetModuleHandleW(L"amd_fidelityfx_loader_dx12.dll");
    auto mode = reinterpret_cast<void (*)(unsigned)>(GetProcAddress(dll, "FixtureMode")); Require(mode != nullptr, "fixture mode");
    auto stat = reinterpret_cast<unsigned (*)(unsigned)>(GetProcAddress(dll,"FixturePresentationStat"));Require(stat!=nullptr,"callback dispatch observation");
    for(unsigned testMode:{0u,36u}) {
    mode(testMode);
    auto session = std::make_shared<FsrSdkSession>(); FsrFrameGeneration generation(session);
    FsrEffectProvider selected{FsrEffect::FrameGeneration, testMode==36?ProviderInfo{42,"4.0.1"}:ProviderInfo{17726168133342859270ull,"3.1.6"}};
    auto foreignSession = std::make_shared<FsrSdkSession>();
    { auto foreign = foreignSession->Lock(); Require(!generation.Create(foreign, runtime, rig.device12.Get(), selected, rig.limits), "foreign-session lock token rejected"); }
    {
    auto lock = session->Lock();
    Require(bool(generation.Create(lock, runtime, rig.device12.Get(), selected, rig.limits)), "VersionDescriptorsLinked: actual requested context identity");
    Require(!generation.Prepare(lock, rig.list.Get(), rig.frame, rig.resources, true), "ConfigurePrecedesPrepare");
    // Normal DXGI chain supplies a real COM object; the vendor double installs
    // the callback without running an SDK interpolation presenter.
    WNDCLASSW windowClass{}; windowClass.lpfnWndProc = DefWindowProcW; windowClass.lpszClassName = L"TRP.FsrContextFixture";
    windowClass.hInstance = GetModuleHandleW(nullptr); RegisterClassW(&windowClass);
    auto window = CreateWindowW(windowClass.lpszClassName, L"FG contract fixture", WS_OVERLAPPEDWINDOW, 0,0,128,128,nullptr,nullptr,windowClass.hInstance,nullptr);
    Require(window != nullptr, "hidden native window");
    DXGI_SWAP_CHAIN_DESC1 desc{}; desc.Width = desc.Height = 128; desc.Format = rig.limits.format;
    desc.SampleDesc.Count = 1; desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; desc.BufferCount = 2; desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    ComPtr<IDXGISwapChain1> base; ComPtr<IDXGISwapChain4> chain;
    Check(rig.factory->CreateSwapChainForHwnd(rig.queue.Get(), window, &desc, nullptr, nullptr, &base), "normal fixture swapchain"); Check(base.As(&chain), "swapchain 4");
    Require(bool(generation.Configure(lock, chain.Get(), 41, true)), "source 41 configured before Prepare");
    Require(!generation.Configure(lock, chain.Get(), 41, true), "ConfigureExactlyOncePerSourceFrame");
    Require(bool(generation.Prepare(lock, rig.list.Get(), rig.frame, rig.resources, true)), "source 41 prepared");
    ffxDispatchDescFrameGeneration descriptor{}; descriptor.header.type = FFX_API_DISPATCH_DESC_TYPE_FRAMEGENERATION;
    descriptor.commandList = rig.list.Get(); descriptor.presentColor = ffxApiGetResourceDX12(rig.resources.scene);
    descriptor.outputs[0] = ffxApiGetResourceDX12(rig.resources.ui, FFX_API_RESOURCE_STATE_UNORDERED_ACCESS);
    descriptor.numGeneratedFrames = 1; descriptor.backbufferTransferFunction = FFX_API_BACKBUFFER_TRANSFER_FUNCTION_SRGB; descriptor.frameID = 41;
    descriptor.generationRect = {0,0,128,128}; descriptor.minMaxLuminance[1] = 100;
    const auto before = descriptor;
    auto invoke = reinterpret_cast<ffxReturnCode_t (*)(ffxDispatchDescFrameGeneration*)>(GetProcAddress(dll, "FixtureInvokeGeneration"));
    Require(invoke != nullptr, "vendor callback captured");
    Require(bool(session->BeginPresent(lock)), "Present owns the same SDK lock");
    Require(invoke(&descriptor) == FFX_API_RETURN_OK, "generation callback dispatch succeeds without recursive lock");
    Require(std::memcmp(&before, &descriptor, sizeof(descriptor)) == 0, "CallbackDescriptorUnchanged");
    const auto dispatches=stat(4);
    Require(invoke(&descriptor) == FFX_API_RETURN_ERROR_PARAMETER, "SecondCallbackInSamePresentRejected");
    Require(stat(4)==dispatches,"duplicate invocation never reaches generation Dispatch");
    Require(generation.LastCallback(lock).invocations == 2 && generation.LastCallback(lock).result == FFX_API_RETURN_ERROR_PARAMETER,
        "duplicate callback retains transaction failure");
    session->EndPresent(lock);
    Require(bool(session->BeginPresent(lock)), "separate malformed descriptor transaction");
    descriptor.frameID = 42;
    Require(invoke(&descriptor) != FFX_API_RETURN_OK, "CallbackDescriptorFrameIdMatchesConfiguredId");
    session->EndPresent(lock);
    Require(bool(session->BeginPresent(lock)), "separate dispatch failure transaction");
    descriptor.frameID = 41; mode(18);
    Require(invoke(&descriptor) == FFX_API_RETURN_ERROR, "injected generation failure returned");
    Require(generation.LastCallback(lock).result == FFX_API_RETURN_ERROR, "GenerationFailureRetainsDiagnostic"); mode(0);
    session->EndPresent(lock);
    Require(invoke(&descriptor) != FFX_API_RETURN_OK, "callback outside Present rejected");
    Require(bool(generation.Configure(lock, chain.Get(), 42, false)), "SuppressedFrameConfiguresDisabled");
    auto next = rig.frame; next.sourceId = 42;
    Require(!generation.Prepare(lock, rig.list.Get(), next, rig.resources, false), "suppressed source skips Prepare");
    Require(!generation.DisableAndDetach(lock), "lifecycle detach requires stopped admissions");
    Require(bool(session->StopAdmissions(lock)), "stop new SDK transactions before teardown");
    Require(!session->ResumeAfterFeatureRetirement(lock),"cannot reopen admissions around a live FG feature");
    Require(bool(generation.DisableAndDetach(lock)), "detach callbacks for lifecycle");
    mode(24);
    Require(bool(generation.DestroyAfterRetirement(lock)), "no GPU work recorded by vendor double; safe context destruction");
    Require(bool(session->ResumeAfterFeatureRetirement(lock)),"SuccessfulDestroyClearsCallerContextEvenWhenSdkDoesNot");mode(0);
    chain.Reset(); base.Reset(); DestroyWindow(window); rig.ValidateDebug();
    }
    }
    CheckDestructorSerialization();
    std::puts("PASS: FG context, source configuration, inherited callback lock and diagnostics");
}
