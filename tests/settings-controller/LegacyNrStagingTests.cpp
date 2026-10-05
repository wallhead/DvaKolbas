#include "PCH.h"
#include "RendererSettingsController.h"
#include "RendererSettingsEdits.h"
#include "FrameGen/NvidiaHost.h"
#include "FrameGen/SourceFrameGeneration.h"
#include "FrameGen/SourceDLSSGBackend.h"
#include <stdexcept>
using namespace TheosRenderPipeline;
void Require(bool ok, const char* reason) { if (!ok) throw std::runtime_error(reason); }
int main() { try {
    auto controller = RendererSettingsController::Current();
    auto& host = *NvidiaHost::GetSingleton();
    auto& generation = *SourceFrameGeneration::GetSingleton();
    auto& backend = SourceDLSSG::Backend::Get();
    auto draft = controller.Capture(true, false);
    draft.sourceDLSSG.neuralEnabled = true;
    Require(controller.Apply(draft, false).applied && backend.NeuralConfiguration().enabled,
        "legacy NR is active before staging FSR");
    draft = controller.Capture(true, false);
    StageRendererUpscaleProvider(draft, true);
    draft.fsr.sourceColorEncoding = Upscaling::ColorEncoding::Gamma22;
    for (int save = 0; save < 2; ++save) {
        auto result = controller.Apply(PrepareRendererStartupDraft(draft, false), true);
        Require(result.applied && !result.error, "pending FSR defaults can be saved");
        Require(host.configuration.Requested().mode == FSR && !host.FsrActive(), "FSR remains pending for restart");
        Require(backend.NeuralConfiguration().enabled, "saving pending FSR must preserve active legacy NR");
        Require(!generation.settings.sourceDLSSG.neuralEnabled, "next-launch FSR NR remains disabled");
        draft = controller.Capture(true, false);
        Require(draft.sourceDLSSG.neuralEnabled, "pending FSR menu displays the actual active legacy NR toggle");
    }
    const auto before = draft;
    draft.enableGPUTimings = !draft.enableGPUTimings;
    Require(controller.ApplyLiveEdits(before, draft).applied && backend.NeuralConfiguration().enabled,
        "unrelated live edits keep active legacy NR while FSR is pending");
    Require(!generation.settings.sourceDLSSG.neuralEnabled, "live edit cannot enable NR in saved FSR defaults");
    Require(!controller.SetNeuralRenderingEnabled(false).error && !backend.NeuralConfiguration().enabled,
        "live NR off still addresses current NVIDIA owner");
    Require(!controller.SetNeuralRenderingEnabled(true).error && backend.NeuralConfiguration().enabled,
        "live NR on uses actual owner instead of pending FSR mode");
    Require(!generation.settings.sourceDLSSG.neuralEnabled, "live toggle preserves incompatible next-launch NR off");
    draft = controller.Capture(true, false);
    SetRendererUpscaleProvider(draft, false);
    draft.sourceDLSSG.neuralEnabled = false;
    Require(controller.Apply(draft, false).applied && !backend.NeuralConfiguration().enabled,
        "an explicit NR edit for NVIDIA still disables the live pass");
    std::puts("PASS legacy NR Save/live-edit/toggle isolation from pending FSR");
    return 0;
} catch (const std::exception& error) { std::fprintf(stderr, "FAIL %s\n", error.what()); return 1; } }
