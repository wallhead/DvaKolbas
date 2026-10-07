#include "Upscaling/FSRRuntime.h"
#include "Upscaling/FSRProviderPolicy.h"
#include "Upscaling/FSRCreationFallback.h"
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
    // Vendor creation is the boundary double. Exercise cleanup ordering and
    // the already-published game-buffer constraint without spoofing an adapter.
    for(unsigned scenario=0;scenario<9;++scenario) {
        std::string events;
        const ProviderInfo ml{23,"4.1.1"}, analytical{22,"3.1.5"};
        auto result=CreateFsrWithStartupFallback(ml,
            scenario==1?ProviderPolicy::MachineLearning:scenario==8?ProviderPolicy::Analytical:ProviderPolicy::Compatible,Extent{960,540},
            [&](const ProviderInfo& provider)->Result<void> {
                events+=provider.id==23?'M':'A';
                if(scenario==7)return {};
                if(provider.id==22 && scenario!=6)return {};
                return std::unexpected(RuntimeError{scenario==2?ErrorKind::DeviceLost:ErrorKind::ContextFailure,1,"creation failed"});
            },
            [&]()->Result<void> {
                events+='R';
                if(scenario==3)return std::unexpected(RuntimeError{ErrorKind::RetirementFailure,2,"still owned"});
                return {};
            },
            [&]()->Result<std::pair<ProviderInfo,Extent>> {
                events+='Q';
                if(scenario==4)return std::unexpected(RuntimeError{ErrorKind::NoProvider,0,"missing"});
                return std::pair{analytical,scenario==5?Extent{961,540}:Extent{960,540}};
            });
        const char* expected[]{"MRQA","M","M","MR","MRQ","MRQ","MRQA","M","M"};
        Require(events==expected[scenario],"Auto fallback respects cleanup and admission order");
        Require(bool(result)==(scenario==0 || scenario==7),"only first success or safely created same-size analytical fallback succeeds");
        if(result)Require(result->id==(scenario==7?23:22),"creation publishes the actually created provider");
        if(scenario==3)Require(result.error().kind==ErrorKind::RetirementFailure,"failed retirement is preserved instead of retrying");
    }
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
        Require(SelectProvider(otherVersions,ProviderPolicy::Compatible)->id == 44,"Auto selects discovered FSR4 instead of unrelated or malformed versions");
        std::vector<ProviderInfo> ordered{{21,"2.3.4"},{22,"3.1.5"},{23,"4.1.1"}};
        Require(SelectProvider(ordered,ProviderPolicy::Compatible)->id==23,"Auto prefers FSR4 regardless of enumeration order");
        Require(SelectProvider(ordered,ProviderPolicy::Compatible,0x10de)->id==22,"official Auto uses analytical FSR on NVIDIA even if catalog contains ML");
        Require(!SelectProvider(ordered,ProviderPolicy::MachineLearning,0x10de),"official explicit ML does not force AMD-only runtime on NVIDIA");
        Require(SelectProvider(ordered,ProviderPolicy::MachineLearning)->id==23,"explicit ML selects discovered FSR4");
        ordered.pop_back();
        Require(SelectProvider(ordered,ProviderPolicy::Compatible)->id==22,"Auto falls back to FSR3 when ML is unavailable on this device");
        Require(!SelectProvider(ordered,ProviderPolicy::MachineLearning),"explicit ML cannot silently become FSR3");
        Require(!SelectProvider(ordered,static_cast<ProviderPolicy>(99)),"invalid policy cannot select a provider");
        mode(15);
        ffxContext nameMismatch{};
        Require(runtime.Functions().CreateContext(&nameMismatch,&create.header,nullptr)==FFX_API_RETURN_OK,"name-mismatch context seam");
        Require(!runtime.VerifyActualProvider(nameMismatch,*selected),"same ID with a different actual provider name rejects the context");
        Require(runtime.Functions().DestroyContext(&nameMismatch,nullptr)==FFX_API_RETURN_OK,"name-mismatch context retired");
    }
    Require(!Loaded(base / "good"),"runtime releases both fixture modules");
    std::puts("PASS: runtime loading, unwinding, provider identity, typed queries and lifetime");
}
