#include <Windows.h>
extern "C" __declspec(dllexport) DWORD WINAPI QueryModuleW(HMODULE module, LPWSTR out, DWORD size)
{
    return GetModuleFileNameW(module, out, size);
}
extern "C" __declspec(dllexport) DWORD WINAPI QueryModuleA(HMODULE module, LPSTR out, DWORD size)
{
    return GetModuleFileNameA(module, out, size);
}
