#pragma once
#include "IniLayout.h"
#include "SourceDLSSGSettings.h"
#include "RuntimePathSettings.h"
#include "NeuralRendering/StartupSettings.h"
#include <atomic>
#include <dxgi.h>
#include <string>

// Source startup policy and user requests, independent of any presenter SDK.
class SourceFrameGeneration
{
  public:
    static SourceFrameGeneration* GetSingleton()
    {
        static SourceFrameGeneration value;
        return &value;
    }
    struct Settings
    {
        bool enabled{true}; // Next-launch preference; the live presenter request can differ while a restart is staged.
        long generationBackend{1};
        bool sourceDLSSGMFGUnlock{true}; // Matches the packaged default; explicit false is preserved.
        bool sourceDLSSGMFGUnlockPresent{};
        std::string sourceDLSSGStreamlineDirectory;
        TheosRenderPipeline::SourceDLSSG::Preferences sourceDLSSG;
        int nativeUICompositionMode{}; // 0 = dedicated UI; 1 = HUD-less detection.
        std::string neuralRenderingRuntimePath;
        TheosRenderPipeline::RuntimePathSettings configuredRuntimePaths;
        TheosRenderPipeline::NeuralRendering::StartupSettings neuralStartup,configuredNeuralStartup;
    };
    Settings settings;

    void LoadINI(std::uint32_t adapterVendorId = 0);
    // Called once before installing device hooks. Live changes do not reload
    // this startup snapshot or rebuild the presentation host.
    template<class Ini> void LoadStartupPreferences(const Ini& source)
    {
        const TheosRenderPipeline::IniLayout::ReadView ini(source);
        settings.neuralStartup = settings.configuredNeuralStartup = TheosRenderPipeline::NeuralRendering::LoadStartupSettings(ini);
        settings.generationBackend = ini.GetLongValue("Experimental", "FrameGenerationBackend", 1);
        settings.enabled = ini.GetBoolValue("FrameGeneration", "Enabled", settings.generationBackend != 0);
        RequestRuntimeInterpolation(settings.enabled);
        settings.sourceDLSSGMFGUnlockPresent = ini.GetValue("Experimental", "SourceDLSSGMFGUnlock", nullptr) != nullptr;
        settings.sourceDLSSGMFGUnlock = ini.GetBoolValue("Experimental", "SourceDLSSGMFGUnlock", true);
        settings.configuredRuntimePaths.Load(ini);
        settings.sourceDLSSGStreamlineDirectory = settings.configuredRuntimePaths.streamline;
        settings.sourceDLSSG = TheosRenderPipeline::SourceDLSSG::LoadPreferences(ini);
        settings.nativeUICompositionMode = std::clamp(static_cast<int>(ini.GetLongValue(
            "Experimental", "NativeUICompositionMode", ini.GetLongValue("Experimental", "PureDarkHUDFixMethod", 0))), 0, 1);
        settings.neuralRenderingRuntimePath = settings.configuredRuntimePaths.neural;
    }
    void ResolveRuntimePaths(const std::filesystem::path& pluginDirectory)
    {
        settings.neuralStartup = settings.configuredNeuralStartup.Resolve(pluginDirectory / "TheosRenderPipeline");
        const auto resolved = settings.configuredRuntimePaths.Resolve(pluginDirectory);
        settings.sourceDLSSGStreamlineDirectory = resolved.streamline;
        settings.neuralRenderingRuntimePath = resolved.neural;
    }
    template<class Ini> void StoreRuntimePaths(Ini& ini) const
    {
        settings.configuredRuntimePaths.Store(ini);
    }
    template<class Ini> void StoreCompatibilityPreference(Ini& ini) const
    {
        // Seed older INIs when saving; never overwrite an explicit opt-out or
        // a startup preference edited on disk since this session began.
        if (!ini.GetValue("Experimental", "SourceDLSSGMFGUnlock", nullptr)) {
            ini.SetBoolValue("Experimental", "SourceDLSSGMFGUnlock", settings.sourceDLSSGMFGUnlock);
        }
    }
    template<class Ini> void StoreUIComposition(Ini& ini) const
    {
        if (!ini.GetValue("Experimental", "NativeUICompositionMode", nullptr)) {
            ini.SetLongValue("Experimental", "NativeUICompositionMode", std::clamp(static_cast<int>(
                ini.GetLongValue("Experimental", "PureDarkHUDFixMethod", settings.nativeUICompositionMode)), 0, 1));
        }
        ini.Delete("Experimental", "PureDarkHUDFixMethod");
    }
    static double GetRefreshRate(HWND window);
    void RequestRuntimeInterpolation(bool enabled) { settings.enabled=enabled; requested_.store(enabled, std::memory_order_release); }
    template<class Ini> void StoreInterpolationPreference(Ini& ini) const
    { ini.SetBoolValue("FrameGeneration", "Enabled", settings.enabled); }
    bool RuntimeInterpolationRequested() const { return requested_.load(std::memory_order_acquire); }
    double refreshRate{};

  private:
    SourceFrameGeneration() = default;
    std::atomic_bool requested_{true};
};
