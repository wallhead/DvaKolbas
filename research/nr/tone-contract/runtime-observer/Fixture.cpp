// Owned CPU target for real Win32 attach, hardware observation and detach tests.
#include <windows.h>
#include <array>
#include <atomic>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

static std::atomic<unsigned> foreignHandled{};
extern "C" __declspec(dllexport) __declspec(noinline)
void TraceForeignBreakpoint() { __debugbreak(); }
static void* foreignAddress{};
LONG CALLBACK HandleForeign(EXCEPTION_POINTERS* pointers) {
    const auto& exception=*pointers->ExceptionRecord;
    if(exception.ExceptionCode==EXCEPTION_BREAKPOINT
       &&exception.ExceptionAddress==foreignAddress) {
        ++foreignHandled;
        pointers->ContextRecord->Rip=reinterpret_cast<DWORD64>(exception.ExceptionAddress)+1;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

extern "C" __declspec(dllexport) __declspec(noinline)
unsigned TraceNetworkEntry(void*, void*, const unsigned char* frame) {
    // Keep a real non-inlined call with the third argument in R8. Null remains
    // harmless to the target, but must fail the observer's memory read.
    return frame ? frame[0xec] : 99;
}

int main() {
    std::atomic<bool> quit{};
    std::atomic<unsigned> mode{};
    std::atomic<bool> foreignOnAttach{};
    std::atomic<unsigned long long> calls{};
    std::vector<std::thread> workers;
    bool occupied{};
    auto trap=reinterpret_cast<unsigned char*>(&TraceForeignBreakpoint);
    // MSVC may insert a hotpatch NOP before the intrinsic INT3.
    for(size_t i=0;i<16;++i)if(trap[i]==0xcc){foreignAddress=trap+i;break;}
    if(!foreignAddress)return 2;
    AddVectoredExceptionHandler(1,HandleForeign);
    auto spawn = [&](unsigned count,bool newOnly=false) {
        for (unsigned i=0;i<count;++i) {
            const auto style=static_cast<unsigned>(workers.size()%2);
            workers.emplace_back([&, style, newOnly] {
                std::array<unsigned char,0x15c> frame{};
                auto put=[&](size_t offset,auto value){std::memcpy(frame.data()+offset,&value,sizeof(value));};
                put(0,size_t{0x12345678}); put(0x10,2560u); put(0x14,1440u);
                put(0x18,size_t{0x22345678}); put(0x28,1280u); put(0x2c,720u);
                put(0x30,size_t{0x32345678}); put(0x38,4u); put(0x40,2560u); put(0x44,1440u);
                put(0x48,size_t{0x42345678}); put(0x58,2560u); put(0x5c,1440u);
                put(0xd8,1.f); put(0xdc,1.f); put(0xe0,0.5f); put(0xe4,1.f);
                put(0xe8,0.75f); put(0xec,style); put(0xf4,0.25f);
                put(0x104,1u); put(0x108,1u); put(0x128,1.f); put(0x12c,-0.125f);
                while (!quit.load()) {
                    if(foreignOnAttach.load()) {
                        if(IsDebuggerPresent()&&foreignOnAttach.exchange(false)) {
                            TraceForeignBreakpoint();
                        } else {SwitchToThread();continue;}
                    }
                    const auto active=mode.load();
                    if (active && (active!=2 || newOnly)) {
                        auto result=TraceNetworkEntry(nullptr,nullptr,active==3?nullptr:frame.data());
                        calls.fetch_add(result==99?1:1+result);
                    }
                    Sleep(2);
                }
            });
        }
    };
    spawn(2);
    std::cout<<"{\"ready\":true,\"pid\":"<<GetCurrentProcessId()<<"}"<<std::endl;
    bool allRestored=true;
    for (std::string command;std::getline(std::cin,command);) {
        if (command=="quit") break;
        if (command=="run") mode=1;
        if (command=="newthreads") { spawn(4,true); mode=2; }
        if (command=="badframe") mode=3;
        if (command=="foreign-on-attach") {
            foreignOnAttach=true;
            std::cout<<"{\"foreignConfigured\":true}"<<std::endl;
        }
        if (command=="occupied") {
            HANDLE thread=workers.front().native_handle();
            if(SuspendThread(thread)==DWORD(-1))return 2;
            CONTEXT context{};context.ContextFlags=CONTEXT_DEBUG_REGISTERS;
            if(!GetThreadContext(thread,&context))return 2;
            context.Dr3=reinterpret_cast<DWORD64>(&TraceNetworkEntry);context.Dr7|=0x40;
            if(!SetThreadContext(thread,&context))return 2;
            if(ResumeThread(thread)==DWORD(-1))return 2;
            occupied=true;
            std::cout<<"{\"occupied\":true}"<<std::endl;
        }
        if (command=="check") {
            bool restored=true;
            size_t index{};
            for (auto& worker:workers) {
                HANDLE thread=worker.native_handle();
                if (SuspendThread(thread)==DWORD(-1)) {restored=false;continue;}
                CONTEXT context{};context.ContextFlags=CONTEXT_DEBUG_REGISTERS;
                const auto expected=occupied&&index==0?0x40u:0u;
                const auto expectedAddress=occupied&&index==0?reinterpret_cast<DWORD64>(&TraceNetworkEntry):0ull;
                if (!GetThreadContext(thread,&context) || (context.Dr7&0xff)!=expected || context.Dr0!=0
                    || context.Dr3!=expectedAddress)
                    restored=false;
                if (ResumeThread(thread)==DWORD(-1)) restored=false;
                ++index;
            }
            allRestored &= restored;
            std::cout<<"{\"registersRestored\":"<<(restored?"true":"false")
                     <<",\"alive\":true,\"foreignHandled\":"<<foreignHandled.load()
                     <<",\"calls\":"<<calls.load()<<"}"<<std::endl;
        }
    }
    quit=true;
    for (auto& worker:workers) worker.join();
    return allRestored?0:1;
}
