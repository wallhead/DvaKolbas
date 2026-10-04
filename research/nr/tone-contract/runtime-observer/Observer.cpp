// Research observer only. No injection, remote calls, code writes, GPU work or
// target termination. A hardware execution breakpoint reads a pinned NR frame.
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <bcrypt.h>
#include <array>
#include <atomic>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs=std::filesystem;
static std::atomic<bool> cancelled{};
BOOL WINAPI ConsoleControl(DWORD) { cancelled=true; return TRUE; }
void Need(bool ok,const char* what) {
    if (!ok) throw std::runtime_error(std::string(what)+" Win32="+std::to_string(GetLastError()));
}
struct Handle {
    HANDLE value{};
    explicit Handle(HANDLE v=nullptr):value(v){}
    ~Handle(){if(value&&value!=INVALID_HANDLE_VALUE)CloseHandle(value);}
    Handle(const Handle&)=delete;
    Handle& operator=(const Handle&)=delete;
};
template<class T> T Read(HANDLE process,uint64_t address) {
    T result{};SIZE_T count{};
    Need(ReadProcessMemory(process,reinterpret_cast<void*>(address),&result,sizeof(result),&count)
         && count==sizeof(result),"ReadProcessMemory");
    return result;
}
std::string Hex(uint64_t v) {std::ostringstream out;out<<"0x"<<std::hex<<v;return out.str();}
std::string Utf8(const std::wstring& text) {
    if(text.empty())return {};
    int size=WideCharToMultiByte(CP_UTF8,0,text.data(),static_cast<int>(text.size()),nullptr,0,nullptr,nullptr);
    Need(size>0,"UTF8 conversion");std::string result(size,'\0');
    Need(WideCharToMultiByte(CP_UTF8,0,text.data(),static_cast<int>(text.size()),result.data(),size,nullptr,nullptr)==size,"UTF8 conversion");
    return result;
}
std::string Json(const std::string& text) {
    std::ostringstream out;out<<'"';
    for(unsigned char c:text) {
        if(c=='"'||c=='\\')out<<'\\'<<c;
        else if(c<32)out<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<unsigned(c)<<std::dec;
        else out<<c;
    }
    out<<'"';return out.str();
}
std::string Sha256(const fs::path& path) {
    BCRYPT_ALG_HANDLE algorithm{};BCRYPT_HASH_HANDLE hash{};
    Need(BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0,"SHA256 provider");
    try {
        DWORD objectSize{},count{};
        Need(BCryptGetProperty(algorithm,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&objectSize),sizeof(objectSize),&count,0)>=0,"SHA256 object size");
        std::vector<unsigned char> object(objectSize);
        Need(BCryptCreateHash(algorithm,&hash,object.data(),objectSize,nullptr,0,0)>=0,"SHA256 hash");
        std::ifstream file(path,std::ios::binary);Need(bool(file),"open pinned image");
        std::array<unsigned char,65536> buffer{};
        while(file) {
            file.read(reinterpret_cast<char*>(buffer.data()),buffer.size());
            if(auto n=file.gcount())Need(BCryptHashData(hash,buffer.data(),static_cast<ULONG>(n),0)>=0,"SHA256 data");
        }
        Need(file.eof(),"read pinned image");
        std::array<unsigned char,32> digest{};
        Need(BCryptFinishHash(hash,digest.data(),static_cast<ULONG>(digest.size()),0)>=0,"SHA256 finish");
        BCryptDestroyHash(hash);hash=nullptr;BCryptCloseAlgorithmProvider(algorithm,0);algorithm=nullptr;
        std::ostringstream out;for(auto v:digest)out<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(v);
        return out.str();
    } catch(...) {if(hash)BCryptDestroyHash(hash);if(algorithm)BCryptCloseAlgorithmProvider(algorithm,0);throw;}
}
fs::path ProcessPath(HANDLE process) {
    std::array<wchar_t,32768> path{};DWORD size=static_cast<DWORD>(path.size());
    Need(QueryFullProcessImageNameW(process,0,path.data(),&size)!=0,"target executable path");
    return fs::path(std::wstring(path.data(),size));
}
fs::path MappedPath(HANDLE process,HMODULE module) {
    std::array<wchar_t,32768> path{};
    DWORD n=GetMappedFileNameW(process,module,path.data(),static_cast<DWORD>(path.size()));
    Need(n>0&&n<path.size(),"mapped image path");
    std::wstring devicePath(path.data(),n);
    for(wchar_t drive=L'A';drive<=L'Z';++drive) {
        wchar_t name[]{drive,L':',0};std::array<wchar_t,4096> device{};
        if(QueryDosDeviceW(name,device.data(),static_cast<DWORD>(device.size()))) {
            std::wstring prefix=device.data();
            if(devicePath.starts_with(prefix+L"\\"))return fs::path(std::wstring(name)+devicePath.substr(prefix.size()));
        }
    }
    throw std::runtime_error("Mapped image is not on a resolved local drive");
}
struct Image {
    uint64_t base{},point{},callback{},attachBreakpoint{},attachThreadStart{};
    fs::path path;
    std::string sha;
    std::array<unsigned char,16> prefix{};
};
uint32_t FixtureEntry(HANDLE process,uint64_t base) {
    auto dos=Read<IMAGE_DOS_HEADER>(process,base);
    Need(dos.e_magic==IMAGE_DOS_SIGNATURE&&dos.e_lfanew>0&&dos.e_lfanew<0x100000,"fixture PE header");
    auto nt=Read<IMAGE_NT_HEADERS64>(process,base+dos.e_lfanew);
    Need(nt.Signature==IMAGE_NT_SIGNATURE,"fixture NT header");
    auto entry=nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    Need(entry.VirtualAddress!=0,"fixture export directory");
    auto exports=Read<IMAGE_EXPORT_DIRECTORY>(process,base+entry.VirtualAddress);
    Need(exports.NumberOfNames<256&&exports.NumberOfFunctions<256,"fixture export count");
    for(uint32_t i=0;i<exports.NumberOfNames;++i) {
        auto nameRva=Read<uint32_t>(process,base+exports.AddressOfNames+i*4);
        auto name=Read<std::array<char,64>>(process,base+nameRva);
        if(std::strncmp(name.data(),"TraceNetworkEntry",name.size())==0) {
            auto index=Read<uint16_t>(process,base+exports.AddressOfNameOrdinals+i*2);
            Need(index<exports.NumberOfFunctions,"fixture export index");
            return Read<uint32_t>(process,base+exports.AddressOfFunctions+index*4);
        }
    }
    throw std::runtime_error("Missing fixture observation entry");
}
Image FindImage(HANDLE process,bool fixture) {
    std::array<HMODULE,4096> modules{};DWORD bytes{};
    Need(EnumProcessModulesEx(process,modules.data(),sizeof(modules),&bytes,LIST_MODULES_64BIT)!=0
         && bytes<=sizeof(modules),"enumerate target modules");
    Image image{};unsigned found{};
    for(size_t i=0;i<bytes/sizeof(HMODULE);++i) {
        std::array<wchar_t,32768> name{};
        Need(GetModuleFileNameExW(process,modules[i],name.data(),static_cast<DWORD>(name.size()))!=0,"module filename");
        auto filename=fs::path(name.data()).filename().wstring();
        if(_wcsicmp(filename.c_str(),L"ntdll.dll")==0) {
            auto local=GetModuleHandleW(L"ntdll.dll");Need(local!=nullptr,"local ntdll");
            for(const auto& [exportName,destination]:std::array<std::pair<const char*,uint64_t*>,2>{{
                {"DbgBreakPoint",&image.attachBreakpoint},{"DbgUiRemoteBreakin",&image.attachThreadStart}}}) {
                auto address=GetProcAddress(local,exportName);Need(address!=nullptr,"system attach export");
                auto rva=reinterpret_cast<uint64_t>(address)-reinterpret_cast<uint64_t>(local);
                Need(rva<0x10000000,"system export belongs to ntdll");
                *destination=reinterpret_cast<uint64_t>(modules[i])+rva;
                auto remote=Read<std::array<unsigned char,8>>(process,*destination);
                std::array<unsigned char,8> own{};std::memcpy(own.data(),address,own.size());
                if(remote!=own)throw std::runtime_error("Target system attach instruction differs from local ntdll");
            }
        }
        bool matches=fixture?_wcsicmp(filename.c_str(),L"NrObserverFixture.exe")==0
                            :_wcsicmp(filename.c_str(),L"nvngx_dlssnr.dll")==0;
        if(!matches)continue;
        ++found;image.base=reinterpret_cast<uint64_t>(modules[i]);image.path=MappedPath(process,modules[i]);
    }
    Need(found==1,"exactly one loaded NR image required; enable NR and load the save first");
    Need(image.attachBreakpoint!=0&&image.attachThreadStart!=0,"system attach breakpoint resolved");
    image.sha=Sha256(image.path);
    if(fixture) {
        std::array<wchar_t,32768> self{};Need(GetModuleFileNameW(nullptr,self.data(),static_cast<DWORD>(self.size()))!=0,"observer executable path");
        if(image.sha!=Sha256(fs::path(self.data()).parent_path()/L"NrObserverFixture.exe"))
            throw std::runtime_error("Unknown fixture build");
        image.point=image.base+FixtureEntry(process,image.base);
    } else {
        if(image.sha!="8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206"
           && image.sha!="e67dee209320cdafe0e93e45675d7aa34323a53acc57a72b2e40a181581c989a")
            throw std::runtime_error("Unknown NR image hash; observer refused");
        image.point=image.base+0x21bb0;image.callback=image.base+0x11527b0;
    }
    image.prefix=Read<std::array<unsigned char,16>>(process,image.point);
    if(!fixture && image.prefix!=std::array<unsigned char,16>{0x48,0x8b,0xc4,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x81})
        throw std::runtime_error("Pinned NR instruction prefix mismatch");
    return image;
}
struct Options {
    DWORD pid{};unsigned seconds=15,samples=32,stride=15;
    bool fixture{};fs::path output,cancelFile;
};
Options Parse(int argc,wchar_t** argv) {
    Options options;
    for(int i=1;i<argc;++i) {
        std::wstring key=argv[i];
        if(key==L"--fixture") {options.fixture=true;continue;}
        if(i+1==argc)throw std::runtime_error("Missing observer argument");
        std::wstring value=argv[++i];
        if(key==L"--output")options.output=value;
        else if(key==L"--cancel-file")options.cancelFile=value;
        else {
            size_t consumed{};auto number=std::stoul(value,&consumed);
            if(consumed!=value.size()||number>MAXDWORD)throw std::runtime_error("Invalid numeric observer argument");
            if(key==L"--pid")options.pid=static_cast<DWORD>(number);
            else if(key==L"--seconds")options.seconds=static_cast<unsigned>(number);
            else if(key==L"--samples")options.samples=static_cast<unsigned>(number);
            else if(key==L"--stride")options.stride=static_cast<unsigned>(number);
            else throw std::runtime_error("Unknown observer argument");
        }
    }
    if(!options.pid||options.pid==GetCurrentProcessId()||options.output.empty()
       ||options.seconds<1||options.seconds>30||options.samples<1||options.samples>64
       ||options.stride<1||options.stride>120)throw std::runtime_error("Observer bounds: explicit PID/output, seconds 1..30, samples 1..64, stride 1..120");
    if(fs::exists(options.output))throw std::runtime_error("Refused existing capture output");
    return options;
}
struct Registers {DWORD64 dr0{},dr1{},dr2{},dr3{},dr6{},dr7{};};
Registers Saved(const CONTEXT& c) {return {c.Dr0,c.Dr1,c.Dr2,c.Dr3,c.Dr6,c.Dr7};}
void Restore(CONTEXT& c,const Registers& saved) {
    c.Dr0=saved.dr0;c.Dr1=saved.dr1;c.Dr2=saved.dr2;c.Dr3=saved.dr3;c.Dr6=saved.dr6;c.Dr7=saved.dr7;
}
struct Thread {HANDLE handle{};Registers saved;};
struct Sample {uint64_t elapsedMs{},ordinal{},frame{},callback{};DWORD thread{};std::array<unsigned char,0x15c> data{};};
struct Session {
    HANDLE process;DWORD pid;Image image;bool attached{},pending{},exited{},restored=true,detached{};
    DEBUG_EVENT event{};DWORD continuation=DBG_CONTINUE;
    std::map<DWORD,Thread> threads;
    std::vector<Sample> samples;
    uint64_t hits{},armedAt{};
    DWORD attachThread{};
    bool armed{};
    Session(HANDLE p,DWORD id,Image i):process(p),pid(id),image(std::move(i)){}
    ~Session(){for(auto& item:threads)CloseHandle(item.second.handle);}

    void Arm(DWORD tid) {
        if(threads.contains(tid))return;
        Handle thread(OpenThread(THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_SUSPEND_RESUME|THREAD_QUERY_INFORMATION,FALSE,tid));
        Need(thread.value!=nullptr,"open observation thread");
        CONTEXT context{};context.ContextFlags=CONTEXT_DEBUG_REGISTERS;
        Need(GetThreadContext(thread.value,&context)!=0,"read debug registers");
        if(context.Dr7&0xff)throw std::runtime_error("Thread already has a hardware breakpoint; refused");
        // Save before mutating, so every failed SetThreadContext has a cleanup owner.
        threads.emplace(tid,Thread{thread.value,Saved(context)});thread.value=nullptr;
        context.Dr0=image.point;context.Dr6&=~1ull;context.Dr7=(context.Dr7&~0xf0003ull)|1ull;
        Need(SetThreadContext(threads.at(tid).handle,&context)!=0,"arm hardware observation");
    }
    void Continue() {
        if(pending) {Need(ContinueDebugEvent(event.dwProcessId,event.dwThreadId,continuation)!=0,"continue debug event");pending=false;}
    }
    void Finish() noexcept {
        if(!attached)return;
        if(exited) {restored=true;detached=true;attached=false;return;}
        // Freeze each tracked live thread until detach completes. Undo exactly
        // our one SuspendThread increment afterwards, preserving prior suspends.
        std::vector<HANDLE> suspended;
        for(auto& [id,thread]:threads) {
            (void)id;DWORD exitCode{};
            if(GetExitCodeThread(thread.handle,&exitCode)&&exitCode!=STILL_ACTIVE)continue;
            if(SuspendThread(thread.handle)==DWORD(-1)) {restored=false;continue;}
            suspended.push_back(thread.handle);
            CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;
            if(!GetThreadContext(thread.handle,&c)) {restored=false;continue;}
            Restore(c,thread.saved);
            if(!SetThreadContext(thread.handle,&c))restored=false;
            CONTEXT verify{};verify.ContextFlags=CONTEXT_DEBUG_REGISTERS;
            if(!GetThreadContext(thread.handle,&verify)||verify.Dr0!=thread.saved.dr0
               ||verify.Dr1!=thread.saved.dr1||verify.Dr2!=thread.saved.dr2||verify.Dr3!=thread.saved.dr3
               ||verify.Dr7!=thread.saved.dr7)restored=false;
        }
        // The known breakpoint is handled even when reading its data failed.
        if(pending) {
            if(!ContinueDebugEvent(event.dwProcessId,event.dwThreadId,continuation))restored=false;
            pending=false;
        }
        // A queued exception on an explicitly suspended worker may not become
        // a debug event until that worker resumes. Resume after removing DR0,
        // while still attached, then acknowledge any already-raised faults.
        for(auto handle:suspended)if(ResumeThread(handle)==DWORD(-1))restored=false;
        DEBUG_EVENT queued{};
        const auto drainDeadline=GetTickCount64()+1000;
        while(GetTickCount64()<drainDeadline&&WaitForDebugEvent(&queued,50)) {
            DWORD status=DBG_CONTINUE;
            if(queued.dwDebugEventCode==EXCEPTION_DEBUG_EVENT) {
                const auto& exception=queued.u.Exception.ExceptionRecord;
                bool ours=exception.ExceptionCode==EXCEPTION_SINGLE_STEP
                    && reinterpret_cast<uint64_t>(exception.ExceptionAddress)==image.point;
                if(!ours)status=DBG_EXCEPTION_NOT_HANDLED;
            }
            if(queued.dwDebugEventCode==CREATE_PROCESS_DEBUG_EVENT&&queued.u.CreateProcessInfo.hFile)
                CloseHandle(queued.u.CreateProcessInfo.hFile);
            if(queued.dwDebugEventCode==LOAD_DLL_DEBUG_EVENT&&queued.u.LoadDll.hFile)
                CloseHandle(queued.u.LoadDll.hFile);
            if(!ContinueDebugEvent(queued.dwProcessId,queued.dwThreadId,status)) {restored=false;break;}
        }
        detached=DebugActiveProcessStop(pid)!=0;
        attached=false;
    }
    void Observe(const Options& o) {
        Need(DebugActiveProcess(pid)!=0,"attach observer");attached=true;
        // Microsoft requires this after establishing a debugging connection.
        Need(DebugSetProcessKillOnExit(FALSE)!=0,"disable debugger kill-on-exit");
        const auto attachDeadline=GetTickCount64()+10000;
        for(;;) {
            if(cancelled||(!o.cancelFile.empty()&&fs::exists(o.cancelFile)))return;
            if(armed&&GetTickCount64()-armedAt>=o.seconds*1000ull)return;
            if(!armed&&GetTickCount64()>=attachDeadline)throw std::runtime_error("Attach initialization timed out");
            if(!WaitForDebugEvent(&event,50)) {
                if(GetLastError()==ERROR_SEM_TIMEOUT)continue;
                Need(false,"wait debug event");
            }
            pending=true;continuation=DBG_CONTINUE;
            switch(event.dwDebugEventCode) {
            case CREATE_PROCESS_DEBUG_EVENT:
                if(event.u.CreateProcessInfo.hFile)CloseHandle(event.u.CreateProcessInfo.hFile);
                Arm(event.dwThreadId);break;
            case CREATE_THREAD_DEBUG_EVENT:
                if(reinterpret_cast<uint64_t>(event.u.CreateThread.lpStartAddress)==image.attachThreadStart)
                    attachThread=event.dwThreadId;
                Arm(event.dwThreadId);break;
            case EXIT_THREAD_DEBUG_EVENT:
                if(auto it=threads.find(event.dwThreadId);it!=threads.end()) {CloseHandle(it->second.handle);threads.erase(it);}break;
            case LOAD_DLL_DEBUG_EVENT:
                if(event.u.LoadDll.hFile)CloseHandle(event.u.LoadDll.hFile);break;
            case UNLOAD_DLL_DEBUG_EVENT:
                if(reinterpret_cast<uint64_t>(event.u.UnloadDll.lpBaseOfDll)==image.base)
                    throw std::runtime_error("Observed NR image unloaded");
                break;
            case EXIT_PROCESS_DEBUG_EVENT: exited=true;Continue();return;
            case EXCEPTION_DEBUG_EVENT: {
                const auto& e=event.u.Exception;
                if(e.ExceptionRecord.ExceptionCode==EXCEPTION_BREAKPOINT&&!armed&&e.dwFirstChance
                   &&event.dwThreadId==attachThread
                   &&reinterpret_cast<uint64_t>(e.ExceptionRecord.ExceptionAddress)==image.attachBreakpoint) {
                    auto current=FindImage(process,o.fixture);
                    if(current.base!=image.base||current.sha!=image.sha||current.prefix!=image.prefix)
                        throw std::runtime_error("Observed image changed during attach");
                    armed=true;armedAt=GetTickCount64();
                    std::cout<<"ARMED "<<image.sha<<" maximumSeconds="<<o.seconds<<std::endl;
                } else if(e.ExceptionRecord.ExceptionCode==EXCEPTION_SINGLE_STEP) {
                    auto it=threads.find(event.dwThreadId);
                    if(it==threads.end()) {continuation=DBG_EXCEPTION_NOT_HANDLED;break;}
                    CONTEXT c{};c.ContextFlags=CONTEXT_INTEGER|CONTEXT_CONTROL|CONTEXT_DEBUG_REGISTERS;
                    Need(GetThreadContext(it->second.handle,&c)!=0,"read observation context");
                    if(!(c.Dr6&1)||c.Rip!=image.point) {continuation=DBG_EXCEPTION_NOT_HANDLED;break;}
                    // RF suppresses re-trapping on this unchanged instruction.
                    // No software breakpoint or trap-flag single stepping.
                    c.EFlags|=0x10000;c.Dr6&=~1ull;
                    c.ContextFlags=CONTEXT_CONTROL|CONTEXT_DEBUG_REGISTERS;
                    Need(SetThreadContext(it->second.handle,&c)!=0,"resume hardware instruction");
                    ++hits;
                    if((hits-1)%o.stride==0) {
                        Sample row;row.elapsedMs=GetTickCount64()-armedAt;row.ordinal=hits;
                        row.thread=event.dwThreadId;row.frame=c.R8;
                        row.data=Read<decltype(row.data)>(process,row.frame);
                        if(image.callback)row.callback=Read<uint64_t>(process,image.callback);
                        samples.push_back(row);
                        if(samples.size()>=o.samples)return;
                    }
                } else continuation=DBG_EXCEPTION_NOT_HANDLED;
                break;
            }
            default: break;
            }
            Continue();
        }
    }
};
template<class T> T Field(const Sample& row,size_t offset) {
    T value{};std::memcpy(&value,row.data.data()+offset,sizeof(value));return value;
}
void Number(std::ostream& out,float value) {if(std::isfinite(value))out<<value;else out<<"null";}
void WriteReport(std::ostream& out,const Options& o,const Session& session,const std::string& reason,const std::string& error) {
    out<<std::setprecision(9)<<"{\"schema\":1,\"pid\":"<<o.pid<<",\"fixture\":"<<(o.fixture?"true":"false")
       <<",\"module\":"<<Json(Utf8(session.image.path.wstring()))<<",\"moduleSha256\":"<<Json(session.image.sha)
       <<",\"observationRva\":"<<Json(Hex(session.image.point-session.image.base))
       <<",\"finishReason\":"<<Json(reason)<<",\"error\":"<<Json(error)
       <<",\"detached\":"<<(session.detached?"true":"false")<<",\"registersRestored\":"<<(session.restored?"true":"false")
       <<",\"hits\":"<<session.hits<<",\"stride\":"<<o.stride
       <<",\"limitations\":[\"Debugger observations are not timings\",\"No resource format, view or pixel capture\",\"Subrects are dispatch values, not proof of active viewport\",\"Sampled resets are not complete history\"],\"samples\":[";
    const std::array<const char*,9> resources{"color","mvec","depth","output","controlMask","ui","uiAlpha","backbuffer","bidirectionalDistortion"};
    for(size_t n=0;n<session.samples.size();++n) {
        if(n)out<<',';const auto& row=session.samples[n];
        out<<"\n{\"elapsedMs\":"<<row.elapsedMs<<",\"ordinal\":"<<row.ordinal<<",\"threadId\":"<<row.thread
           <<",\"framePointer\":"<<Json(Hex(row.frame))<<",\"callbackPointer\":"<<Json(Hex(row.callback));
        for(size_t r=0;r<resources.size();++r) {
            auto offset=r*0x18;
            out<<','<<Json(resources[r])<<":{\"pointer\":"<<Json(Hex(Field<uint64_t>(row,offset)))
               <<",\"x\":"<<Field<uint32_t>(row,offset+8)<<",\"y\":"<<Field<uint32_t>(row,offset+12)
               <<",\"width\":"<<Field<uint32_t>(row,offset+16)<<",\"height\":"<<Field<uint32_t>(row,offset+20)<<'}';
        }
        const std::array<std::pair<const char*,size_t>,7> floats{{{"mvScaleX",0xd8},{"mvScaleY",0xdc},
            {"intensity",0xe0},{"tone",0xe4},{"structure",0xe8},{"requestedSkinStructure",0xf4},{"effectiveSkinStructure",0xf8}}};
        for(auto [name,offset]:floats) {out<<','<<Json(name)<<':';Number(out,Field<float>(row,offset));}
        const std::array<std::pair<const char*,size_t>,8> integers{{{"style",0xec},{"autoMask",0xf0},{"reset",0x100},
            {"depthInverted",0x104},{"enabled",0x108},{"uiCorrection",0x10c},{"invertX",0x110},{"invertY",0x114}}};
        for(auto [name,offset]:integers)out<<','<<Json(name)<<':'<<Field<uint32_t>(row,offset);
        out<<",\"coefficients\":[";for(size_t i=0;i<14;++i){if(i)out<<',';Number(out,Field<float>(row,0x124+i*4));}out<<"]}";
    }
    out<<"]}\n";
}
int wmain(int argc,wchar_t** argv) {
    try {
        auto options=Parse(argc,argv);
        Handle process(OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|SYNCHRONIZE,FALSE,options.pid));
        Need(process.value!=nullptr,"open target for observation");
        auto target=ProcessPath(process.value).filename().wstring();
        bool allowed=options.fixture?_wcsicmp(target.c_str(),L"NrObserverFixture.exe")==0
            :(_wcsicmp(target.c_str(),L"SkyrimSE.exe")==0||_wcsicmp(target.c_str(),L"NrToneModelProbe.exe")==0);
        if(!allowed)throw std::runtime_error("Refused target executable");
        Session session(process.value,options.pid,FindImage(process.value,options.fixture));
        std::ofstream output(options.output,std::ios::binary);Need(bool(output),"create capture output before attach");
        SetConsoleCtrlHandler(ConsoleControl,TRUE);
        std::string error,reason;
        try {session.Observe(options);}catch(const std::exception& e){error=e.what();}
        session.Finish();
        if(!error.empty())reason="error";
        else if(session.exited)reason="target-exited";
        else if(cancelled||(!options.cancelFile.empty()&&fs::exists(options.cancelFile)))reason="cancelled";
        else if(session.samples.size()>=options.samples)reason="sample-limit";
        else reason="timeout";
        WriteReport(output,options,session,reason,error);output.flush();Need(bool(output),"write capture after detach");
        std::cout<<"FINISHED "<<reason<<" samples="<<session.samples.size()<<" restored="<<session.restored<<" detached="<<session.detached<<std::endl;
        if(!error.empty())std::cerr<<error<<'\n';
        if(!error.empty()||!session.restored||!session.detached)return 1;
        return session.samples.empty()?3:0;
    }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
