#include "RendererUpgrade.h"
#include "NvidiaBaselinePolicy.h"
#include <fstream>
#include <cstdio>
#include <stdexcept>

namespace {
void Need(bool condition,const char* name) { if(!condition)throw std::runtime_error(name); }
std::string Read(const std::filesystem::path& path) {
    std::ifstream input(path,std::ios::binary);
    return {std::istreambuf_iterator<char>(input),std::istreambuf_iterator<char>()};
}
void Write(const std::filesystem::path& path,const std::string& value) {
    std::ofstream output(path,std::ios::binary);output<<value;
    Need(bool(output),"write fixture");
}
struct Fixture {
    std::filesystem::path root=std::filesystem::temp_directory_path()/
        (L"RazUpgrade-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64()));
    Fixture(){Need(std::filesystem::create_directory(root),"unique fixture directory");}
    ~Fixture(){std::error_code ignored;for(const auto* name:{L"TheosRenderPipeline.ini",L"RaZkolbaS.ini",L"TheosRenderPipeline.dll"})
        std::filesystem::remove(root/name,ignored);
        for(const auto* name:{L"TheosRenderPipeline",L"RaZkolbaS"}) {
            std::filesystem::remove(root/name/L"runtime.dll",ignored);std::filesystem::remove(root/name,ignored);
        }
        std::filesystem::remove(root,ignored);}
};
}
int main(){try {
    using namespace TheosRenderPipeline::RendererUpgrade;
    Fixture f;const auto old=f.root/L"TheosRenderPipeline.ini",current=f.root/L"RaZkolbaS.ini";
    Need(Prepare(f.root,false).status==Status::Ready,"no legacy installation needs no migration");
    const std::string content="; user settings\r\n[NR PASS 1]\r\nStyle=2\r\nTone=0.65\r\nCustom=\xD0\xA2\xD0\xB5\xD1\x81\xD1\x82\r\n";
    Write(old,content);
    Need(Prepare(f.root,true).status==Status::Conflict,"loaded old renderer must block before migration or hooks");
    Need(!std::filesystem::exists(current),"duplicate renderer does not migrate files");
    Write(f.root/L"TheosRenderPipeline.dll","legacy fixture");
    Need(Prepare(f.root,false).status==Status::Conflict,"enabled old DLL must block even before its SKSE load callback");
    std::filesystem::remove(f.root/L"TheosRenderPipeline.dll");
    const auto migration=Prepare(f.root,false);
    Need(migration.status==Status::Migrated,"missing renamed INI must recover existing legacy settings");
    Need(Read(current)==content&&Read(old)==content,"migration preserves exact legacy bytes and leaves original available");
    const auto renamed=f.root/L"RaZkolbaS/runtime.dll",oldResource=f.root/L"TheosRenderPipeline/runtime.dll";
    std::filesystem::create_directory(renamed.parent_path());Write(renamed,"new resources");
    Need(TheosRenderPipeline::ResolveRuntimePath("TheosRenderPipeline/runtime.dll",f.root)==renamed,
        "migrated relative paths resolve existing renamed resources");
    std::filesystem::create_directory(oldResource.parent_path());Write(oldResource,"separate user runtime");
    Need(TheosRenderPipeline::ResolveRuntimePath("TheosRenderPipeline/runtime.dll",f.root)==oldResource,
        "existing separate legacy runtime still takes priority");
    Need(TheosRenderPipeline::ResolveRuntimePath(oldResource,f.root)==oldResource,
        "explicit absolute resource overrides remain unchanged");
    Need(TheosRenderPipeline::ResolveRuntimePath("TheosRenderPipeline/missing.dll",f.root)==oldResource.parent_path()/L"missing.dll",
        "missing renamed resources do not silently select another file");
    Write(current,"current user settings");
    Need(Prepare(f.root,false).status==Status::Ready&&Read(current)=="current user settings","current settings always take priority");
    std::filesystem::remove(current);
    const auto held=CreateFileW(old.c_str(),GENERIC_READ,0,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    Need(held!=INVALID_HANDLE_VALUE,"hold unreadable legacy INI fixture");
    const auto denied=Prepare(f.root,false);CloseHandle(held);
    Need(denied.status==Status::Failed&&denied.native!=0,"unreadable legacy settings report migration failure");
    Need(Read(old)==content,"migration failure never changes old settings");
    Need(!std::filesystem::exists(current),"failed migration never publishes partial settings");
    std::puts("PASS RendererUpgrade");return 0;
}catch(const std::exception& e){std::printf("FAIL: %s\n",e.what());return 1;}}
