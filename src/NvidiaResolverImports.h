#pragma once
#include "HookSafety.h"

namespace TheosRenderPipeline::NvidiaAppSettings::Detail {
inline std::uintptr_t* ResolverSlot(HMODULE module) {
    std::uintptr_t* found{};
    for(const auto* name:{"kernel32.dll","kernelbase.dll",
        "api-ms-win-core-libraryloader-l1-1-0.dll",
        "api-ms-win-core-libraryloader-l1-2-0.dll","api-ms-win-core-libraryloader-l1-2-1.dll"}) {
        auto* candidate=HookSafety::ImportSlot(reinterpret_cast<std::uintptr_t>(module),name,"GetProcAddress");
        // Different imports may have different hook predecessors. Keep the
        // existing single-import contract rather than patching an arbitrary one.
        if(candidate) {if(found)return nullptr;found=candidate;}
    }
    return found;
}
}
