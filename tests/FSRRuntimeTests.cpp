#include "Upscaling/FSRRuntime.h"
#include "Upscaling/FSRProviderPolicy.h"
#include "RendererBackendPolicy.h"
#include <ffx_upscale.h>
#include <dx12/ffx_api_dx12.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
using namespace TheosRenderPipeline::Upscaling;
namespace fs = std::filesystem;
static void Require(bool value, const char* why) { if (!value) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); } }
static bool Loaded(const fs::path& p) { return GetModuleHandleW((p / "FSR/amd_fidelityfx_loader_dx12.dll").c_str()) || GetModuleHandleW((p / "FSR/amd_fidelityfx_upscaler_dx12.dll").c_str()); }
int main(int argc, char** argv)
{
    Require(argc == 2, "fixture directory supplied"); const auto base = fs::absolute(argv[1]);
    FsrRuntime absent; auto missing = absent.Load(base / "absent");
    Require(!missing && missing.error().kind == ErrorKind::MissingRuntime, "missing runtime is explicit");
    Require(!absent.Load("relative"), "relative module root rejected");
    TheosRenderPipeline::Upscaling::BackendConfiguration dlss;
    Require(TheosRenderPipeline::ResolveBackend(dlss, false).valid, "missing FSR leaves DLSS usable");
    auto wrong = base / "wrong/FSR"; fs::create_directories(wrong);
    // A PE machine mismatch is diagnosed before Windows attempts execution.
    fs::copy_file(base / "good/FSR/amd_fidelityfx_upscaler_dx12.dll", wrong / "amd_fidelityfx_upscaler_dx12.dll", fs::copy_options::overwrite_existing);
    std::fstream f(wrong / "amd_fidelityfx_upscaler_dx12.dll", std::ios::binary|std::ios::in|std::ios::out);
    uint32_t offset{}; f.seekg(0x3c); f.read(reinterpret_cast<char*>(&offset),4); uint16_t machine = IMAGE_FILE_MACHINE_I386;
    f.seekp(offset+4); f.write(reinterpret_cast<char*>(&machine),2); f.close();
    FsrRuntime bad; auto architecture = bad.Load(base / "wrong");
    Require(!architecture && architecture.error().kind == ErrorKind::WrongArchitecture, "wrong machine has a distinct error");
    Require(!Loaded(base / "wrong"), "wrong-machine load leaves no module");
    for (int i = 1; i <= 5; ++i) {
        auto path = base / ("missing" + std::to_string(i));
        { FsrRuntime runtime; const auto result = runtime.Load(path);
          Require(!result && result.error().kind == ErrorKind::MissingExport, "each missing C export is rejected");
          Require(!runtime.Functions().Query, "failed load clears partially resolved table"); }
        Require(!Loaded(path), "partial load unwinds both modules");
    }
    {
        FsrRuntime runtime; Require(bool(runtime.Load(base / "good")), "complete fixture loads");
        FsrRuntime collision; auto second = collision.Load(base / "missing1");
        Require(!second && second.error().kind == ErrorKind::IncompatibleAbi, "different existing basename is rejected");
        auto dll = GetModuleHandleW((base / "good/FSR/amd_fidelityfx_loader_dx12.dll").c_str());
        auto mode = reinterpret_cast<void(*)(unsigned)>(GetProcAddress(dll,"FixtureMode"));
        auto queryId = reinterpret_cast<uint64_t(*)()>(GetProcAddress(dll,"FixtureQueryId"));
        auto createId = reinterpret_cast<uint64_t(*)()>(GetProcAddress(dll,"FixtureCreateId"));
        auto* device = reinterpret_cast<ID3D12Device*>(1); // Fixture boundary only; never dereferenced.
        mode(1); auto zero = runtime.Enumerate(device); Require(!zero && zero.error().kind == ErrorKind::NoProvider,"zero providers is explicit");
        mode(2); auto incompatible = runtime.Enumerate(device); Require(!incompatible && incompatible.error().kind == ErrorKind::IncompatibleAbi,"incompatible descriptor ABI is distinct");
        for (unsigned value : {5u, 6u, 7u}) {
            mode(value); auto malformed = runtime.Enumerate(device);
            Require(!malformed && malformed.error().kind == ErrorKind::IncompatibleAbi,"unstable/oversized count and null provider name rejected");
        }
        mode(3); auto providers = runtime.Enumerate(device); Require(providers && providers->size() == 2,"enumeration count growth retries safely");
        auto selected = SelectProvider(*providers, ProviderPolicy::Analytical);
        Require(selected && selected->id == 17 && selected->name == "fixture analytical FSR 3.1.5","analytical selection retains discovered identity");
        auto extent = runtime.QueryRenderExtent(device,*selected,Quality::Performance,{1921,1081});
        Require(extent && *extent == Extent{960,540},"render extent comes from selected provider");
        Require((*providers)[0].name == "fixture analytical FSR 3.1.5","provider names survive later SDK queries");
        ffxCreateContextDescUpscale create{}; create.header.type = FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE;
        ffxCreateBackendDX12Desc backend{{FFX_API_CREATE_CONTEXT_DESC_TYPE_BACKEND_DX12,nullptr},device};
        ffxCreateContextDescUpscaleVersion version{{FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE_VERSION,nullptr},FFX_UPSCALER_VERSION};
        ffxOverrideVersion override{{FFX_API_DESC_TYPE_OVERRIDE_VERSION,nullptr},selected->id};
        create.header.pNext = &backend.header; backend.header.pNext = &version.header; version.header.pNext = &override.header;
        ffxContext context{};
        Require(runtime.Functions().CreateContext(&context,&create.header,nullptr) == FFX_API_RETURN_OK,"create descriptor uses typed version/backend/override chain");
        Require(queryId() == createId() && createId() == selected->id,"sizing/create use same discovered override");
        Require(bool(runtime.VerifyActualProvider(context,*selected)),"actual created provider matches request");
        mode(4); auto mismatch = runtime.VerifyActualProvider(context,*selected);
        Require(!mismatch && mismatch.error().kind == ErrorKind::ContextFailure,"actual provider mismatch rejects context");
        Require(runtime.Functions().DestroyContext(&context,nullptr) == FFX_API_RETURN_OK,"context destroyed before unload");
        mode(0); Require(!SelectProvider({},ProviderPolicy::Compatible),"no invented provider fallback");
        Require(!runtime.Enumerate(nullptr),"null device rejected");
        Require(!runtime.QueryRenderExtent(device,*selected,Quality::Quality,{0,1080}),"zero display rejected");
        Require(!runtime.QueryRenderExtent(device,*selected,static_cast<Quality>(99),{1920,1080}),"unknown quality rejected");
        std::vector<ProviderInfo> otherVersions{{41,"13.1.5"},{42,"3.1.50"},{43,"3.1.5.1"},{44,"4.1.1"}};
        Require(!SelectProvider(otherVersions,ProviderPolicy::Analytical),"analytical policy cannot silently choose a different version");
        Require(SelectProvider(otherVersions,ProviderPolicy::Compatible)->id == 41,"compatible policy retains discovered provider order");
    }
    Require(!Loaded(base / "good"),"runtime releases both fixture modules");
    std::puts("PASS: runtime loading, unwinding, provider identity, typed queries and lifetime");
}
