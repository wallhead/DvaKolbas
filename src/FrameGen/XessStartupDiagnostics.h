#pragma once
#include <Windows.h>
#include <array>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <filesystem>

namespace TheosRenderPipeline::XessStartupDiagnostics
{
    // Narrow first-chance evidence capture. Never handle or suppress an exception.
    inline HANDLE file=INVALID_HANDLE_VALUE;
    inline LONG captured{};
    inline void* handler{};

    inline void Write(const char* text)
    {
        DWORD written{};
        if(file!=INVALID_HANDLE_VALUE)WriteFile(file,text,static_cast<DWORD>(std::strlen(text)),&written,nullptr);
    }
    inline void Bytes(const char* label,uintptr_t address,SIZE_T count)
    {
        std::array<unsigned char,96> bytes{};SIZE_T read{};
        const BOOL success=ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),bytes.data(),
            (std::min)(count,bytes.size()),&read);
        char line[512]{};
        int length=std::snprintf(line,sizeof(line),"%s address=0x%llX read=%llu ok=%d bytes=",label,
            static_cast<unsigned long long>(address),static_cast<unsigned long long>(read),success!=FALSE);
        for(SIZE_T i=0;i<read && length+3<sizeof(line);++i)
            length+=std::snprintf(line+length,sizeof(line)-length,"%02X",bytes[i]);
        std::snprintf(line+length,sizeof(line)-length,"\r\n");Write(line);
    }
    inline LONG CALLBACK Observe(EXCEPTION_POINTERS* exception)
    {
        if(!exception || !exception->ExceptionRecord || !exception->ContextRecord ||
            exception->ExceptionRecord->ExceptionCode!=EXCEPTION_BREAKPOINT ||
            InterlockedCompareExchange(&captured,1,0)!=0)return EXCEPTION_CONTINUE_SEARCH;
        const auto& context=*exception->ContextRecord;
        char line[512]{};
        std::snprintf(line,sizeof(line),"XeSS startup breakpoint evidence; thread=%lu exeBase=0x%llX exception=0x%llX rip=0x%llX rsp=0x%llX\r\n",
            GetCurrentThreadId(),reinterpret_cast<unsigned long long>(GetModuleHandleW(nullptr)),
            reinterpret_cast<unsigned long long>(exception->ExceptionRecord->ExceptionAddress),
            context.Rip,context.Rsp);Write(line);
        const auto address=reinterpret_cast<uintptr_t>(exception->ExceptionRecord->ExceptionAddress);
        Bytes("exception-code",address>=32?address-32:address,96);
        std::array<uintptr_t,16> stack{};SIZE_T read{};
        ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(context.Rsp),stack.data(),sizeof(stack),&read);
        for(SIZE_T i=0;i<read/sizeof(uintptr_t);++i) {
            std::snprintf(line,sizeof(line),"stack[%llu]=0x%llX\r\n",static_cast<unsigned long long>(i),static_cast<unsigned long long>(stack[i]));Write(line);
            MEMORY_BASIC_INFORMATION region{};
            if(VirtualQuery(reinterpret_cast<void*>(stack[i]),&region,sizeof(region)) && region.State==MEM_COMMIT &&
                (region.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)))
                Bytes("return-code",stack[i]>=32?stack[i]-32:stack[i],64);
        }
        FlushFileBuffers(file);
        return EXCEPTION_CONTINUE_SEARCH;
    }
    inline bool Install(const std::filesystem::path& path)
    {
        if(handler)return true;
        file=CreateFileW(path.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file==INVALID_HANDLE_VALUE)return false;
        handler=AddVectoredExceptionHandler(1,Observe);
        if(!handler){CloseHandle(file);file=INVALID_HANDLE_VALUE;return false;}
        Write("XeSS startup diagnostics armed; exception handling remains unchanged.\r\n");
        FlushFileBuffers(file);return true;
    }
}
