#include "FrameGen/NeuralRenderingModulePathHook.h"
#include "HookSafety.h"
#include <array>
#include <cstdio>
#include <string_view>
using namespace TheosRenderPipeline;
using namespace TheosRenderPipeline::NeuralRendering;
namespace {
int failed{};
void Check(bool ok, const char* name) { std::printf("%s %s\n", ok ? "PASS" : "FAIL", name); failed += !ok; }
DWORD WINAPI Foreign(HMODULE, LPWSTR, DWORD) { return 0; }
using Protect = BOOL(WINAPI*)(LPVOID, SIZE_T, DWORD, PDWORD);
Protect originalProtect{};
void* deniedSlot{};
BOOL WINAPI DenyOneProtection(LPVOID address, SIZE_T bytes, DWORD protection, PDWORD previous) {
    if (address == deniedSlot) { SetLastError(ERROR_ACCESS_DENIED); return FALSE; }
    return originalProtect(address, bytes, protection, previous);
}
bool Replace(void** slot, void* value) {
    DWORD old{}, ignored{};
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &old)) return false;
    InterlockedExchangePointer(slot, value);
    return VirtualProtect(slot, sizeof(void*), old, &ignored) != FALSE;
}
}
int wmain(int argc, wchar_t** argv) {
    if (argc != 2) return 1;
    const auto feature = LoadLibraryExW(argv[1], nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!feature) return 1;
    using QueryW = DWORD(WINAPI*)(HMODULE, LPWSTR, DWORD);
    using QueryA = DWORD(WINAPI*)(HMODULE, LPSTR, DWORD);
    const auto queryW = reinterpret_cast<QueryW>(GetProcAddress(feature, "QueryModuleW"));
    const auto queryA = reinterpret_cast<QueryA>(GetProcAddress(feature, "QueryModuleA"));
    auto* slotW = reinterpret_cast<void**>(HookSafety::ImportSlot(reinterpret_cast<uintptr_t>(feature), "KERNEL32.dll", "GetModuleFileNameW"));
    auto* slotA = reinterpret_cast<void**>(HookSafety::ImportSlot(reinterpret_cast<uintptr_t>(feature), "KERNEL32.dll", "GetModuleFileNameA"));
    if (!queryW || !queryA || !slotW || !slotA) return 1;
    const auto originalW = *slotW, originalA = *slotA;
    const auto caller = GetModuleHandleW(nullptr), unrelated = GetModuleHandleW(L"kernel32.dll");
    const std::filesystem::path loader = L"C:\\NR fixture\\nvngx.dll";
    {
        ModulePathHook hook, second;
        Check(hook.Install(feature, loader), "InstallCallerScopedHook");
        std::array<wchar_t, 32768> out{}, expected{};
        std::array<char, 32768> narrow{}, expectedA{};
        queryW(caller, out.data(), DWORD(out.size()));
        Check(std::wstring_view(out.data()) == loader.native(), "OnlyRetainedCallerGetsLoaderPath");
        queryA(caller, narrow.data(), DWORD(narrow.size()));
        Check(std::string_view(narrow.data()) == "C:\\NR fixture\\nvngx.dll", "CallerAnsiPathExact");
        for (const auto module : {feature, unrelated, HMODULE(nullptr)}) {
            GetModuleFileNameW(module, expected.data(), DWORD(expected.size()));
            queryW(module, out.data(), DWORD(out.size()));
            Check(std::wstring_view(out.data()) == expected.data(), "OtherWideModulePathUnchanged");
            GetModuleFileNameA(module, expectedA.data(), DWORD(expectedA.size()));
            queryA(module, narrow.data(), DWORD(narrow.size()));
            Check(std::string_view(narrow.data()) == expectedA.data(), "OtherAnsiModulePathUnchanged");
        }
        Check(second.Install(feature, loader) && second.Restore(), "SharedMatchingOwnerCanReleaseWithoutRemovingHook");
        const auto proxyW = *slotW;
        Check(Replace(slotW, reinterpret_cast<void*>(&Foreign)), "InjectRestoreOwnershipConflict");
        Check(!hook.Restore() && *slotW == reinterpret_cast<void*>(&Foreign), "FailedRestoreNeverOverwritesLaterOwner");
        // A proxy captured by a caller can still run after partial restoration.
        reinterpret_cast<QueryA>(*slotA)(caller, narrow.data(), DWORD(narrow.size()));
        // Query the saved proxy explicitly; the IAT may already be restored.
        const auto proxyQueryW = reinterpret_cast<QueryW>(proxyW);
        proxyQueryW(caller, out.data(), DWORD(out.size()));
        Check(std::wstring_view(out.data()) == loader.native(), "FailedRestoreRetainsUsableCallbackState");
        proxyQueryW(unrelated, out.data(), DWORD(out.size()));
        GetModuleFileNameW(unrelated, expected.data(), DWORD(expected.size()));
        Check(std::wstring_view(out.data()) == expected.data(), "FailedRestoreStillForwardsOtherModules");
        Check(Replace(slotW, proxyW) && hook.Restore(), "OwnedRestoreCanBeRetriedSafely");
        Check(*slotW == originalW && *slotA == originalA, "OriginalImportsRestored");
    }
    {
        ModulePathHook hook;
        Check(hook.Install(feature, loader), "InstallForDeniedProtectionRestore");
        auto* protectSlot = reinterpret_cast<void**>(HookSafety::ImportSlot(reinterpret_cast<uintptr_t>(caller), "KERNEL32.dll", "VirtualProtect"));
        if (!protectSlot) return 1;
        originalProtect = reinterpret_cast<Protect>(*protectSlot);
        deniedSlot = slotA;
        Check(Replace(protectSlot, reinterpret_cast<void*>(&DenyOneProtection)), "InjectDeniedProtectionForOneImport");
        Check(!hook.Restore(), "DeniedVirtualProtectRestoreReportsFailure");
        std::array<char, 32768> out{};
        queryA(caller, out.data(), DWORD(out.size()));
        Check(std::string_view(out.data()) == "C:\\NR fixture\\nvngx.dll", "DeniedProtectionKeepsRemainingProxyUsable");
        ModulePathHook newcomer;
        Check(!newcomer.Install(feature, loader), "PendingRestorationRejectsNewOwners");
        deniedSlot = nullptr;
        Check(Replace(protectSlot, reinterpret_cast<void*>(originalProtect)), "RestoreFixtureVirtualProtectImport");
        Check(hook.Restore() && *slotW == originalW && *slotA == originalA, "ProtectionRecoveryRestoresRemainingOwnedImport");
    }
    {
        const std::filesystem::path unicode = L"C:\\NR-\U0001F680\\nvngx.dll";
        BOOL usedDefault{};
        const auto codePage = GetACP();
        const auto flags = codePage == CP_UTF8 ? WC_ERR_INVALID_CHARS : WC_NO_BEST_FIT_CHARS;
        const auto convertible = WideCharToMultiByte(codePage, flags, unicode.c_str(), -1, nullptr, 0, nullptr,
            codePage == CP_UTF8 ? nullptr : &usedDefault);
        ModulePathHook hook;
        const auto installed = hook.Install(feature, unicode);
        Check(installed == (convertible > 0 && !usedDefault), "LossyAnsiLoaderPathRejected");
        if (installed) Check(hook.Restore(), "ExactUnicodePathRestores");
        Check(*slotW == originalW && *slotA == originalA, "RejectedUnicodeInstallLeavesImportsUnchanged");
    }
    FreeLibrary(feature);
    return failed ? 1 : 0;
}
