#include "Upscaling/FSRRuntime.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <ffx_framegeneration.h>
#include <dx12/ffx_api_dx12.h>
using namespace TheosRenderPipeline::Upscaling;
namespace fs = std::filesystem;
static void Require(bool value, const char* why)
{ if (!value) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); } }
static HMODULE Module(const fs::path& root, const wchar_t* name)
{ return GetModuleHandleW((root / "FSR" / name).c_str()); }
template<class Runtime> static void CheckLoadCases(const fs::path& base)
{
    if constexpr (!requires(Runtime& runtime) { runtime.LoadFrameGeneration(base); }) {
        Require(false, "Optional FG loading is not implemented");
    } else {
        const auto good = base / "good";
        const wchar_t* fg = L"amd_fidelityfx_framegeneration_dx12.dll";
        fs::create_directories(good / "FSR");
        fs::copy_file(good / "FSR/amd_fidelityfx_loader_dx12.dll", good / "FSR" / fg, fs::copy_options::overwrite_existing);
        Runtime runtime;
        Require(bool(runtime.Load(good)), "SR runtime loads independently");
        Require(!Module(good, fg), "ordinary SR does not load FG");
        auto* device = reinterpret_cast<ID3D12Device*>(1); // Vendor double never dereferences it.
        const auto query = runtime.Functions().Query;
        auto missing = runtime.LoadFrameGeneration(base / "absent");
        Require(!missing && missing.error().kind == ErrorKind::MissingRuntime, "MissingFgLeavesSrUsable: explicit missing module");
        Require(runtime.Functions().Query == query && bool(runtime.Enumerate(device)), "MissingFgLeavesSrUsable: SR still enumerates");
        Require(!runtime.LoadFrameGeneration("relative"), "relative FG root rejected");
        auto partial = base / "partial/FSR"; fs::create_directories(partial);
        fs::copy_file(base / "missing4/FSR/amd_fidelityfx_loader_dx12.dll", partial / fg, fs::copy_options::overwrite_existing);
        auto incomplete = runtime.LoadFrameGeneration(base / "partial");
        Require(!incomplete && incomplete.error().kind == ErrorKind::MissingExport, "PartialFgLoadUnwindsOnlyFg: missing export rejected");
        Require(!Module(base / "partial", fg) && runtime.Functions().Query == query && bool(runtime.Enumerate(device)), "PartialFgLoadUnwindsOnlyFg: unload FG, preserve SR");
        auto foreign = base / "foreign/FSR"; fs::create_directories(foreign);
        fs::copy_file(good / "FSR" / fg, foreign / fg, fs::copy_options::overwrite_existing);
        auto handle = LoadLibraryExW((foreign / fg).c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
        Require(handle != nullptr, "foreign FG fixture loaded");
        auto collision = runtime.LoadFrameGeneration(good);
        Require(!collision && collision.error().kind == ErrorKind::IncompatibleAbi, "ForeignFgModuleRejected");
        FreeLibrary(handle);
        Require(bool(runtime.LoadFrameGeneration(good)), "verified plugin-relative FG loads");
        Require(Module(good, fg) != nullptr && bool(runtime.Enumerate(device)), "FG loading preserves SR");
        Require(!runtime.LoadFrameGeneration(base / "foreign"), "loaded FG root cannot change");
    }
}
template<class Catalog> static void CheckSelection(Catalog providers)
{
    if constexpr (!requires { SelectFsrEffectProvider(providers, FsrEffect::FrameGeneration); }) {
        Require(false, "deterministic effect-tagged selection is not implemented");
    } else {
        auto selected = SelectFsrEffectProvider(providers, FsrEffect::FrameGeneration);
        Require(selected && selected->identity.id == 17726168133342859270ull && selected->identity.name == "3.1.6", "ProviderOrderDoesNotChangeSelection: choose pinned analytical FG");
        std::reverse(providers.begin(), providers.end());
        selected = SelectFsrEffectProvider(providers, FsrEffect::FrameGeneration);
        Require(selected && selected->identity.id == 17726168133342859270ull, "ProviderOrderDoesNotChangeSelection: reversed catalog");
        auto swapchain = SelectFsrEffectProvider(providers, FsrEffect::FrameGenerationSwapChain);
        Require(swapchain && swapchain->identity.id == 17752306900579389447ull, "swapchain selected by its own effect");
        const Catalog wrong{{FsrEffect::FrameGenerationSwapChain, {17726168133342859270ull, "3.1.6"}}};
        Require(!SelectFsrEffectProvider(wrong, FsrEffect::FrameGeneration), "WrongEffectProviderRejected");
        const Catalog mismatch{{FsrEffect::FrameGeneration, {17726168133342859270ull, "4.0.1"}}};
        Require(!SelectFsrEffectProvider(mismatch, FsrEffect::FrameGeneration), "matching opaque ID with wrong name rejected");
        Require(!SelectFsrEffectProvider(Catalog{}, FsrEffect::FrameGeneration), "no implicit fallback on empty catalog");
        const Catalog unknown{{FsrEffect::FrameGeneration, {123, "3.1.6"}}};
        Require(!SelectFsrEffectProvider(unknown, FsrEffect::FrameGeneration), "unknown opaque ID rejected");
    }
}
template<class Runtime> static void CheckActual(Runtime& runtime, ffxContext& context, const FsrEffectProvider& expected, void (*mode)(unsigned))
{
    if constexpr (!requires { runtime.VerifyActualProvider(context, expected); }) {
        Require(false, "effect-tagged created-provider verification is not implemented");
    } else {
        Require(bool(runtime.VerifyActualProvider(context, expected)), "CreatedProviderMatches: exact ID and name");
        mode(4); auto wrongId = runtime.VerifyActualProvider(context, expected);
        Require(!wrongId && wrongId.error().kind == ErrorKind::ContextFailure, "CreatedProviderMatches: reject different actual ID");
        mode(15); auto wrongName = runtime.VerifyActualProvider(context, expected);
        Require(!wrongName && wrongName.error().kind == ErrorKind::ContextFailure, "CreatedProviderMatches: reject different actual name");
        mode(0);
        auto wrongEffect = expected; wrongEffect.effect = FsrEffect::FrameGenerationSwapChain;
        Require(!runtime.VerifyActualProvider(context, wrongEffect), "WrongEffectProviderRejected: mismatched creation identity");
    }
}
static void CheckQueriesAndContext(const fs::path& base)
{
    FsrRuntime runtime; Require(bool(runtime.Load(base / "good")), "SR query fixture loads");
    auto* device = reinterpret_cast<ID3D12Device*>(1);
    Require(!runtime.EnumerateForEffect(device, FsrEffect::FrameGeneration), "no implicit FG query/load during SR");
    Require(bool(runtime.LoadFrameGeneration(base / "good")), "FG query fixture loads");
    const auto dll = Module(base / "good", L"amd_fidelityfx_loader_dx12.dll");
    auto mode = reinterpret_cast<void (*)(unsigned)>(GetProcAddress(dll, "FixtureMode")); Require(mode != nullptr, "vendor fixture mode available");
    auto sr = runtime.EnumerateForEffect(device, FsrEffect::Upscale);
    auto fg = runtime.EnumerateForEffect(device, FsrEffect::FrameGeneration);
    auto chain = runtime.EnumerateForEffect(device, FsrEffect::FrameGenerationSwapChain);
    Require(sr && sr->size() == 1 && sr->front().identity.id == 17, "DistinctEffectQueries: SR descriptor");
    Require(fg && fg->size() == 1 && fg->front().effect == FsrEffect::FrameGeneration && fg->front().identity.id == 17726168133342859270ull, "DistinctEffectQueries: FG descriptor");
    Require(chain && chain->size() == 1 && chain->front().effect == FsrEffect::FrameGenerationSwapChain && chain->front().identity.id == 17752306900579389447ull, "DistinctEffectQueries: NewDX12 descriptor");
    Require(!runtime.EnumerateForEffect(device, static_cast<FsrEffect>(99)), "unknown effect rejected");
    for (auto effect : {FsrEffect::FrameGeneration, FsrEffect::FrameGenerationSwapChain}) {
        for (auto malformed : {5u, 6u, 7u}) {
            mode(malformed); auto providers = runtime.EnumerateForEffect(device, effect);
            Require(!providers && providers.error().kind == ErrorKind::IncompatibleAbi, "ProviderCountBounded: unstable/oversized/null name");
        }
        mode(1); auto absent = runtime.EnumerateForEffect(device, effect);
        Require(!absent && absent.error().kind == ErrorKind::NoProvider, "empty effect catalog does not fall back");
    }
    mode(0);
    ffxCreateContextDescFrameGeneration create{}; create.header.type = FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATION;
    ffxCreateBackendDX12Desc backend{{FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12, nullptr}, device};
    ffxCreateContextDescFrameGenerationVersion version{{FFX_API_CREATE_CONTEXT_DESC_TYPE_FRAMEGENERATION_VERSION, nullptr}, FFX_FRAMEGENERATION_VERSION};
    ffxOverrideVersion override{{FFX_API_DESC_TYPE_OVERRIDE_VERSION, nullptr}, 17726168133342859270ull};
    create.header.pNext = &backend.header; backend.header.pNext = &version.header; version.header.pNext = &override.header;
    ffxContext context{};
    Require(runtime.Functions().CreateContext(&context, &create.header, nullptr) == FFX_API_RETURN_OK, "FG vendor context created with separate ABI and override");
    CheckActual(runtime, context, fg->front(), mode);
    Require(runtime.Functions().DestroyContext(&context, nullptr) == FFX_API_RETURN_OK, "destroy before runtime unload");
    mode(36);
    const auto mlCatalog=runtime.EnumerateForEffect(device,FsrEffect::FrameGeneration);
    Require(bool(mlCatalog),"ML catalog fixture enumerates separately from SR");
    const auto ml=SelectFsrEffectProvider(*mlCatalog,FsrEffect::FrameGeneration,ProviderPolicy::MachineLearning);
    Require(ml && ml->identity.id==42,"use discovered ML override, not a fabricated version ID");
    override.versionId=ml->identity.id;
    Require(runtime.Functions().CreateContext(&context,&create.header,nullptr)==FFX_API_RETURN_OK,"ML FG override and current ABI reach creation");
    Require(bool(runtime.VerifyActualProvider(context,*ml)),"ML actual identity is verified after creation");
    mode(4);
    Require(!runtime.VerifyActualProvider(context,*ml),"ML actual provider substitution rejected");
    Require(runtime.Functions().DestroyContext(&context,nullptr)==FFX_API_RETURN_OK,"ML fixture retired before unload");mode(0);
}
int main(int argc, char** argv)
{
    Require(argc == 2, "fixture directory supplied");
    const auto base = fs::absolute(argv[1]);
    const auto searchLength = GetDllDirectoryW(0, nullptr);
    std::wstring search(searchLength + 1, L'\0');
    const auto filledLength = GetDllDirectoryW(static_cast<DWORD>(search.size()), search.data());
    CheckLoadCases<FsrRuntime>(base);
    CheckSelection(std::vector<FsrEffectProvider>{
        {FsrEffect::FrameGeneration, {42, "4.0.1"}},
        {FsrEffect::FrameGeneration, {17726168133342859270ull, "3.1.6"}},
        {FsrEffect::FrameGenerationSwapChain, {17752306900579389447ull, "3.1.7"}}});
    CheckQueriesAndContext(base);
    const std::vector<FsrEffectProvider> mlCatalog{
        {FsrEffect::FrameGeneration, {42, "4.0.1"}},
        {FsrEffect::FrameGeneration, {17726168133342859270ull, "3.1.6"}}};
    const auto ml = SelectFsrEffectProvider(mlCatalog, FsrEffect::FrameGeneration, ProviderPolicy::MachineLearning);
    Require(ml && ml->identity.id == 42, "ML FG retains the opaque catalog ID");
    const auto automatic = SelectFsrEffectProvider(mlCatalog, FsrEffect::FrameGeneration, ProviderPolicy::Compatible);
    Require(automatic && automatic->identity.id == 42, "Auto prefers supported ML FG");
    const std::vector<FsrEffectProvider> analyticalOnly{mlCatalog.back()};
    Require(!SelectFsrEffectProvider(analyticalOnly, FsrEffect::FrameGeneration, ProviderPolicy::MachineLearning),
        "Explicit ML FG cannot silently become analytical");
    const auto fallback = SelectFsrEffectProvider(analyticalOnly, FsrEffect::FrameGeneration, ProviderPolicy::Compatible);
    Require(fallback && fallback->identity.name == "3.1.6", "Auto keeps analytical FG on unsupported devices");
    Require(!SelectFsrEffectProvider({{FsrEffect::Upscale, {42, "4.0.1"}}}, FsrEffect::FrameGeneration, ProviderPolicy::MachineLearning),
        "SR4 cannot establish FG4 availability");
    Require(!SelectFsrEffectProvider({{FsrEffect::FrameGeneration, {42, "4.0.2b"}}}, FsrEffect::FrameGeneration, ProviderPolicy::MachineLearning),
        "Unqualified ML FG revisions are not admitted");
    Require(!SelectFsrEffectProvider({{FsrEffect::FrameGeneration, {17726168133342859270ull, "4.0.1"}}}, FsrEffect::FrameGeneration, ProviderPolicy::MachineLearning),
        "Analytical identity cannot be relabelled as ML");
    std::wstring after(searchLength + 1, L'\0');
    Require(GetDllDirectoryW(static_cast<DWORD>(after.size()), after.data()) == filledLength && search == after, "no global DLL search-path changes");
    Require(!GetModuleHandleW(L"amd_fidelityfx_framegeneration_dx12.dll"), "runtime destruction unloads FG");
    std::puts("PASS: optional FG module isolation");
}
