#pragma once
#include <Windows.h>
#include <TlHelp32.h>

namespace NrRuntimeResearch {
// Standalone research only. Failed enumeration is not permission to run.
inline bool GameRunningOrUnknown() {
    const HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(snapshot==INVALID_HANDLE_VALUE)return true;
    PROCESSENTRY32W entry{};entry.dwSize=sizeof(entry);
    if(!Process32FirstW(snapshot,&entry)){CloseHandle(snapshot);return true;}
    bool blocked=false;
    for(;;){
        if(!_wcsicmp(entry.szExeFile,L"SkyrimSE.exe")){blocked=true;break;}
        if(!Process32NextW(snapshot,&entry)){
            blocked=GetLastError()!=ERROR_NO_MORE_FILES;
            break;
        }
    }
    CloseHandle(snapshot);return blocked;
}
}
