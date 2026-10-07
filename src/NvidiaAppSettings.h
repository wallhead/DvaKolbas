#pragma once
#include <Windows.h>
#include <filesystem>
#include <cstdint>
#include "NvidiaDriverSettings.h"

namespace TheosRenderPipeline::NvidiaAppSettings {
inline constexpr const char* PreparationFailureNotice =
    "RaZkolbaS could not establish application-controlled NVIDIA settings. "
    "See RaZkolbaS.log for the module path and failure. Use a compatible NVIDIA driver, "
    "or select FSR with FSR FG in RaZkolbaS.ini and restart Skyrim.";
using Log = void(*)(const char*);
void SetLog(Log callback) noexcept;
// Read-only startup snapshot. Call for an actual NVIDIA renderer, including FSR.
// Configuration is not proof of active interpolation; loaded interposers are logged separately.
void ReportDriverSettings();
DriverConflict SmoothMotionStatus() noexcept;
// Notice uses the current loaded-module observation, including late driver
// loads. A module plus an unreadable setting means possible conflict only.
const char* CurrentSmoothMotionNotice() noexcept;
// Hook only owned NVIDIA/NGX imports. All references live until process exit.
// No NVAPI SetSetting/SaveSettings or registry writes are used.
bool ProtectModule(HMODULE module);
bool PrepareCore(); // Call after creation of the NVIDIA rendering device, before NGX init.
enum class StreamlineResolverOwner { Host, Compatibility };
bool PrepareStreamline(const std::filesystem::path& directory, StreamlineResolverOwner owner=StreamlineResolverOwner::Host);
// Compose into the checked Ampere/Turing NVAPI wrapper instead of replacing its IAT.
void* FilterNvapiFunction(std::uint32_t id,void* function);
}
