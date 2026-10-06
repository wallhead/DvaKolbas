#pragma once
#include "Error.h"
#include <Windows.h>
#include <filesystem>

namespace TheosRenderPipeline::NeuralRendering {
// Caller retains a write/delete-denied handle throughout verification and load.
// Never loads or executes the candidate module.
Result<void> VerifyDriverCoreTrust(const std::filesystem::path&, HANDLE heldFile);
}
