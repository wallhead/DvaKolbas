#include "FrameGen/XessGenerationRuntime.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::Upscaling;
namespace fs = std::filesystem;
static void Require(bool value, const char* reason)
{ if (!value) { std::fprintf(stderr,"FAIL: %s\n",reason); std::exit(1); } }
static fs::path Dll(const fs::path& root, const wchar_t* name)
{ return root / L"RaZkolbaS/XeSS" / name; }
static void CopyPair(const fs::path& from, const fs::path& to)
{
    fs::create_directories(Dll(to,L"libxell.dll").parent_path());
    for (const auto name : {L"libxess_fg.dll",L"libxell.dll"})
        fs::copy_file(Dll(from,name),Dll(to,name),fs::copy_options::overwrite_existing);
}
static void RemoveExportName(const fs::path& file, const char* name)
{
    std::ifstream input(file,std::ios::binary);
    std::string bytes{std::istreambuf_iterator<char>(input),{}};
    input.close();
    const auto at=bytes.find(name);
    Require(at!=std::string::npos,"export name exists in pinned fixture");
    bytes[at]='!';
    std::ofstream output(file,std::ios::binary|std::ios::trunc);
    output.write(bytes.data(),bytes.size());
}
int wmain(int argc,wchar_t** argv)
{
    Require(argc==2,"fixture root required");
    const auto root=fs::absolute(argv[1]);
    auto missing=XessGenerationRuntime::Load(root/L"absent");
    Require(!missing && missing.error().kind==ErrorKind::MissingRuntime,"missing FG reports MissingRuntime");
    Require(!XessGenerationRuntime::Load(L"relative"),"relative directory rejected");
    const auto good=root/L"good";
    const auto noLatency=root/L"no-latency";
    CopyPair(good,noLatency);fs::remove(Dll(noLatency,L"libxell.dll"));
    missing=XessGenerationRuntime::Load(noLatency);
    Require(!missing && missing.error().kind==ErrorKind::MissingRuntime,"missing XeLL reports MissingRuntime");
    Require(!GetModuleHandleW(Dll(noLatency,L"libxess_fg.dll").c_str()),"missing XeLL unwinds FG module");
    for (const auto name : {L"libxess_fg.dll",L"libxell.dll"}) {
        const auto invalid=root/(name==std::wstring_view(L"libxell.dll")?L"wrong-latency":L"wrong-generation");
        CopyPair(good,invalid);
        { std::fstream file(Dll(invalid,name),std::ios::in|std::ios::out|std::ios::binary);
          uint32_t offset{};file.seekg(0x3c);file.read(reinterpret_cast<char*>(&offset),4);
          const uint16_t architecture=IMAGE_FILE_MACHINE_I386;
          file.seekp(offset+4);file.write(reinterpret_cast<const char*>(&architecture),2); }
        const auto result=XessGenerationRuntime::Load(invalid);
        Require(!result && result.error().kind==ErrorKind::WrongArchitecture,"wrong architecture rejected before DLL execution");
    }
    for (const bool latency : {false,true}) {
        const auto invalid=root/(latency?L"missing-latency-export":L"missing-generation-export");
        CopyPair(good,invalid);
        RemoveExportName(Dll(invalid,latency?L"libxell.dll":L"libxess_fg.dll"),latency?"xellSleep":"xefgSwapChainDestroy");
        const auto result=XessGenerationRuntime::Load(invalid);
        Require(!result && result.error().kind==ErrorKind::MissingExport,"required public export missing is rejected");
        Require(!GetModuleHandleW(Dll(invalid,L"libxess_fg.dll").c_str()) && !GetModuleHandleW(Dll(invalid,L"libxell.dll").c_str()),"partial load unwinds both modules");
    }
    const auto unicode=root/L"\u041f\u043b\u0430\u0433\u0438\u043d\u044b-\u65e5\u672c";
    CopyPair(good,unicode);
    auto result=XessGenerationRuntime::Load(unicode);
    if (!result) std::fprintf(stderr,"loader kind=%d native=%lld %s\n",static_cast<int>(result.error().kind),result.error().nativeResult,result.error().message.c_str());
    Require(bool(result),"Unicode absolute directory loads");
    auto owner=std::move(*result);
    xefg_swapchain_version_t version{};xell_version_t latencyVersion{};
    Require(owner->Generation().GetVersion(&version)==XEFG_SWAPCHAIN_RESULT_SUCCESS && version.major>0,"typed FG export is callable");
    Require(owner->Latency().GetVersion(&latencyVersion)==XELL_RESULT_SUCCESS && latencyVersion.major>0,"typed XeLL export is callable");
    const auto collision=XessGenerationRuntime::Load(good);
    Require(!collision && collision.error().kind==ErrorKind::IncompatibleAbi,"different module directory collision rejected");
    auto contextOwner=owner;owner.reset();
    for (const auto name : {L"libxess_fg.dll",L"libxell.dll"}) {
        Require(GetModuleHandleW(Dll(unicode,name).c_str())!=nullptr,"context owner retains both modules");
        const auto write=CreateFileW(Dll(unicode,name).c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
        Require(write==INVALID_HANDLE_VALUE && GetLastError()==ERROR_SHARING_VIOLATION,"retained runtime denies concurrent writes");
    }
    contextOwner.reset();
    Require(!GetModuleHandleW(Dll(unicode,L"libxess_fg.dll").c_str()) && !GetModuleHandleW(Dll(unicode,L"libxell.dll").c_str()),"last owner releases FG and XeLL");
    std::puts("PASS: Intel FG/XeLL missing, architecture, exports, Unicode, collision, lease and retention");
}
