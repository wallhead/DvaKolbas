#include <Windows.h>
#include <cstdint>
// Deliberately lacks the public runtime/event exports needed for native capture.
extern "C" __declspec(dllexport) bool ReShadeRegisterAddon(HMODULE,uint32_t){return true;}
