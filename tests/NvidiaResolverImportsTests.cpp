// Exercise the production import selector against small mapped PE fixtures.
#include "NvidiaResolverImports.h"
#include <cstdio>
#include <stdexcept>

namespace {
void Need(bool value,const char* name){if(!value)throw std::runtime_error(name);}
struct Image {
    alignas(8) std::array<unsigned char,4096> bytes{};
    Image() {
        auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(bytes.data());
        dos->e_magic=IMAGE_DOS_SIGNATURE;dos->e_lfanew=128;
        auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(bytes.data()+128);
        nt->Signature=IMAGE_NT_SIGNATURE;nt->OptionalHeader.Magic=IMAGE_NT_OPTIONAL_HDR64_MAGIC;
        nt->OptionalHeader.SizeOfImage=bytes.size();
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]={512,3*sizeof(IMAGE_IMPORT_DESCRIPTOR)};
    }
    void Import(unsigned index,const char* module,const char* symbol="GetProcAddress") {
        auto* descriptor=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(bytes.data()+512)+index;
        const DWORD start=1024+index*512;
        descriptor->Name=start;descriptor->OriginalFirstThunk=start+128;descriptor->FirstThunk=start+160;
        std::strcpy(reinterpret_cast<char*>(bytes.data()+start),module);
        auto* names=reinterpret_cast<IMAGE_THUNK_DATA64*>(bytes.data()+start+128);
        names->u1.AddressOfData=start+192;
        std::strcpy(reinterpret_cast<char*>(bytes.data()+start+192+offsetof(IMAGE_IMPORT_BY_NAME,Name)),symbol);
        *Slot(index)=reinterpret_cast<std::uintptr_t>(&GetProcAddress);
    }
    std::uintptr_t* Slot(unsigned index){return reinterpret_cast<std::uintptr_t*>(bytes.data()+1024+index*512+160);}
    HMODULE Module(){return reinterpret_cast<HMODULE>(bytes.data());}
};
}
int main(){try {
    using TheosRenderPipeline::NvidiaAppSettings::Detail::ResolverSlot;
    Image ordinary;ordinary.Import(0,"KERNEL32.dll");
    Need(ResolverSlot(ordinary.Module())==ordinary.Slot(0),"kernel32 resolver remains supported");
    Image base;base.Import(0,"KernelBase.dll");
    Need(ResolverSlot(base.Module())==base.Slot(0),"kernelbase resolver must retain application override filtering");
    Image api;api.Import(0,"api-ms-win-core-libraryloader-l1-1-0.dll");
    Need(ResolverSlot(api.Module())==api.Slot(0),"older libraryloader contract must retain filtering");
    Image duplicate;duplicate.Import(0,"kernel32.dll");duplicate.Import(1,"kernelbase.dll");
    Need(!ResolverSlot(duplicate.Module()),"ambiguous resolver imports must not select an arbitrary predecessor");
    Image unrelated;unrelated.Import(0,"foreign.dll");
    Need(!ResolverSlot(unrelated.Module()),"foreign resolver library must not be patched");
    Image wrong;wrong.Import(0,"kernelbase.dll","GetModuleHandleW");
    Need(!ResolverSlot(wrong.Module()),"a different library-loader symbol must not be patched");
    Need(!ResolverSlot(reinterpret_cast<HMODULE>(1)),"unreadable image fails safely");
    std::puts("PASS NvidiaResolverImports");return 0;
}catch(const std::exception& error){std::fprintf(stderr,"FAIL: %s\n",error.what());return 1;}}
