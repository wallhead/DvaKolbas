#pragma once
#include <Windows.h>
#include <filesystem>
#include <cstdint>

namespace TheosRenderPipeline::NvidiaAppSettings {
using Log = void(*)(const char*);
void SetLog(Log callback) noexcept;
// Hook only owned NVIDIA/NGX imports. All references live until process exit.
// No NVAPI SetSetting/SaveSettings or registry writes are used.
bool ProtectModule(HMODULE module);
bool PrepareCore(); // Call after creation of the NVIDIA rendering device, before NGX init.
enum class StreamlineResolverOwner { Host, Compatibility };
bool PrepareStreamline(const std::filesystem::path& directory, StreamlineResolverOwner owner=StreamlineResolverOwner::Host);
// Compose into the checked Ampere/Turing NVAPI wrapper instead of replacing its IAT.
void* FilterNvapiFunction(std::uint32_t id,void* function);
}
