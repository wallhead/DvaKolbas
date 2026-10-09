#include "Upscaling/XessRuntime.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
using namespace TheosRenderPipeline::Upscaling;
namespace fs = std::filesystem;
static void Require(bool value, const char* why)
{ if (!value) { std::fprintf(stderr,"FAIL: %s\n",why); std::exit(1); } }
static fs::path Dll(const fs::path& root) { return root / L"RaZkolbaS/XeSS/libxess.dll"; }
int wmain(int argc, wchar_t** argv)
{
    Require(argc==2,"fixture directory required");
    const auto root=fs::absolute(argv[1]);
    Require(!ValidateXessAdapter(LUID{12,4},LUID{13,4}),"mismatched rendering/compute adapter rejected");
    Require(bool(ValidateXessAdapter(LUID{12,4},LUID{12,4})),"same rendering/compute adapter admitted");
    XessRuntime absent;
    const auto missing=absent.Load(root/L"absent");
    Require(!missing && missing.error().kind==ErrorKind::MissingRuntime,"absent DLL reports MissingRuntime");
    Require(!absent.Load(L"relative"),"relative DLL root rejected");
    const auto badRoot=root/L"wrong";
    fs::create_directories(Dll(badRoot).parent_path());
    fs::copy_file(Dll(root/L"good"),Dll(badRoot),fs::copy_options::overwrite_existing);
    { std::fstream file(Dll(badRoot),std::ios::in|std::ios::out|std::ios::binary);
      uint32_t offset{}; file.seekg(0x3c);file.read(reinterpret_cast<char*>(&offset),4);
      const uint16_t machine=IMAGE_FILE_MACHINE_I386;
      file.seekp(offset+4);file.write(reinterpret_cast<const char*>(&machine),2); }
    XessRuntime wrong;const auto architecture=wrong.Load(badRoot);
    Require(!architecture && architecture.error().kind==ErrorKind::WrongArchitecture,"x86 DLL rejected before execution");
    XessRuntime incomplete;const auto exports=incomplete.Load(root/L"missing");
    Require(!exports && exports.error().kind==ErrorKind::MissingExport,"incomplete API is rejected");
    Require(!incomplete.Functions().GetVersion && !GetModuleHandleW(Dll(root/L"missing").c_str()),"partial load unwinds module and table");
    const auto unicode=root/L"\u041f\u043b\u0430\u0433\u0438\u043d\u044b-\u65e5\u672c";
    fs::create_directories(Dll(unicode).parent_path());
    fs::copy_file(Dll(root/L"good"),Dll(unicode),fs::copy_options::overwrite_existing);
    auto runtime=std::make_shared<XessRuntime>();
    Require(bool(runtime->Load(unicode)),"Unicode absolute path loads");
    xess_version_t version{};
    Require(runtime->Functions().GetVersion(&version)==XESS_RESULT_SUCCESS && version.major==2,"typed export remains callable");
    XessRuntime collision;
    const auto foreign=collision.Load(root/L"good");
    Require(!foreign && foreign.error().kind==ErrorKind::IncompatibleAbi,"same basename from another directory rejected");
    auto contextOwner=runtime;
    runtime.reset();
    Require(GetModuleHandleW(Dll(unicode).c_str())!=nullptr,"context owner retains DLL after outer owner resets");
    contextOwner.reset();
    Require(!GetModuleHandleW(Dll(unicode).c_str()),"last owner releases DLL");
    std::puts("PASS: XeSS missing/export/architecture/Unicode/collision/retention checks");
}
