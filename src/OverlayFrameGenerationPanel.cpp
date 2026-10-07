#include "OverlayUI.h"
#include "OverlayUIStyle.h"
#include "OverlayFrameView.h"
#include "OverlayFsrGenerationControls.h"
#include "CommunityShaderIntegration.h"
#include "NvidiaAppSettings.h"

#include "FrameGen/NvidiaHost.h"
#include "FrameGen/SourceDLSSGBackend.h"
#include "FrameGen/SourceFrameGeneration.h"
#include <PCH.h>
#include "RenderPipeline.h"

using namespace TheosRenderPipeline::Overlay;

void OverlayUI::DrawFrameGenerationPanel(float tabCardHeight, const FrameView& view)
{
    auto* frameGen = SourceFrameGeneration::GetSingleton();
    auto* nvidiaHost = NvidiaHost::GetSingleton();
    const auto& sourceDLSSGActive = view.sourceDLSSGActive;
    const auto& frameGenerationRuntimeActive = view.frameGenerationRuntimeActive;
    const auto& activeDisplayMultiplier = view.activeDisplayMultiplier;

    if (ImGui::BeginTabItem("Frame generation", nullptr,
                            requestedPage == SettingsPage::FrameGeneration ? ImGuiTabItemFlags_SetSelected
                                                                           : ImGuiTabItemFlags_None))
    {
        const auto drawSmoothMotionNotice=[] {
            if(const auto* notice=TheosRenderPipeline::NvidiaAppSettings::CurrentSmoothMotionNotice()) {
                ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1.0f,0.75f,0.25f,1.0f));
                ImGui::TextWrapped("%s",notice);
                ImGui::PopStyleColor();
                ImGui::Separator();
            }
        };
        if(!BeginSettingsColumns("generation",tabCardHeight,view)){ImGui::EndTabItem();return;}
        drawSmoothMotionNotice();
        const bool nvidiaSupported=settingsDraft.upscaleType!=FSR &&
            (RenderPipeline::GetSingleton()->mAdapterVendorId==0 || RenderPipeline::GetSingleton()->mAdapterVendorId==0x10de);
        if(DrawGenerationBackendChoice(settingsDraft.generationBackendPreference,nvidiaSupported)) {
            const auto kind=settingsDraft.upscaleType==FSR?TheosRenderPipeline::Upscaling::BackendKind::Fsr:
                settingsDraft.upscaleType==DLAA?TheosRenderPipeline::Upscaling::BackendKind::Dlaa:TheosRenderPipeline::Upscaling::BackendKind::Dlss;
            settingsDraft.generationBackend=TheosRenderPipeline::ResolveGenerationBackend(settingsDraft.generationBackendPreference,kind);
        }
        const long actualBackend=nvidiaHost->FsrFgActive()?2:nvidiaHost->FsrActive()?0:1;
        ImGui::Text("Running backend: %s",actualBackend==2?"FSR FG":actualBackend==1?"NVIDIA FG":"Ordinary (diagnostic)");
        DrawSettingsHelp("Backend selection requires Save as default and restart. Auto selects NVIDIA FG for DLSS, and FSR FG for FSR upscaling.");
        if(settingsDraft.generationBackend!=actualBackend)ImGui::TextWrapped("Selected backend is pending restart. The on/off switch controls the running backend.");
        bool requested=frameGen->RuntimeInterpolationRequested();
        ImGui::BeginDisabled(actualBackend==0);
        if(ImGui::Checkbox("Frame generation##runtime",&requested))
            TheosRenderPipeline::SetLiveGenerationRequest(settingsDraft,*frameGen,requested,actualBackend);
        ImGui::EndDisabled();
        if(nvidiaHost->FsrFgActive() || settingsDraft.generationBackend==2 || view.fsrActive) {
            int fgPolicy=static_cast<int>(settingsDraft.fsr.generationProviderPolicy);
            const char* fgProviders[]{"FSR3 FG (3.1.6)","Auto (FSR4 / FSR3 FG)","FSR4 FG (ML, experimental)"};
            const auto availability=nvidiaHost->FsrMlChoices();
            if(DrawFsrProviderChoice("Provider##fsr-fg",fgPolicy,fgProviders,availability.generation.value_or(true),availability.generationReason.c_str()))
                settingsDraft.fsr.generationProviderPolicy=static_cast<TheosRenderPipeline::Upscaling::ProviderPolicy>(fgPolicy);
            DrawSettingsHelp("Provider changes require Save as default and restart. Official ML FG requires Windows 11, Radeon RX 9000 or later and DirectX 12 Agility SDK 1.4.9+. Auto uses FSR3 FG when ML FG is unavailable. Upscaling and FG providers are independent.");
            auto status=nvidiaHost->FsrFgStatus();
            ImGui::TextWrapped("%s",status.text.c_str());
            if(!nvidiaHost->FsrFgActive())ImGui::TextWrapped("FSR provider is staged for the next launch; the current backend stays active.");
            if(!view.fsrActive) {
                int encoding=static_cast<int>(settingsDraft.fsr.sourceColorEncoding);
                const char* encodings[]{"Unknown", "Linear", "Gamma 2.2 SDR", "sRGB"};
                if(ImGui::Combo("Source encoding##fsr-fg",&encoding,encodings,4))
                    settingsDraft.fsr.sourceColorEncoding=static_cast<TheosRenderPipeline::Upscaling::ColorEncoding>(encoding);
                DrawSettingsHelp("Explicit SDR producer encoding used by FSR FG presentation. Save and restart; independent of FSR upscaling quality/model.");
            }
            if(showDeveloperControls)ImGui::TextWrapped("%s",nvidiaHost->Status().c_str());
            DrawFrameGenerationAdvanced(view);
            EndSettingsColumns();
            ImGui::EndTabItem();return;
        }
        auto& sourceBackend = TheosRenderPipeline::SourceDLSSG::Backend::Get();
        const auto& sourceState = sourceBackend.Snapshot();
        const auto& unlock = sourceBackend.MFGState();
        const auto usableMaximum = TheosRenderPipeline::SourceDLSSG::MFGContract::Maximum(
            unlock.UsesCompatibilityUnlock(), unlock.Ready(), sourceState.state.numFramesToGenerateMax);
        const bool supportsDynamic = TheosRenderPipeline::SourceDLSSG::MFGContract::Dynamic(
                                         unlock.UsesCompatibilityUnlock(), unlock.Ready(),
                                         sourceState.state.bIsDynamicMFGSupported == sl::eTrue) &&
                                     sourceState.state.numFramesToGenerateMax > 1;
        if (ImGui::CollapsingHeader("Status and measurements"))
        {
        DrawStatusLabel(frameGenerationRuntimeActive ? "DLSS-G active" : "Frame generation inactive",
                        frameGenerationRuntimeActive ? UIHealth::kHealthy : UIHealth::kIdle);
        DrawSettingsValue("Multiplier", std::format("x{}", activeDisplayMultiplier).c_str());
        DrawSettingsValue("Display", std::format("{:.0f} Hz", frameGen->refreshRate).c_str());
        DrawSettingsValue("Reflex", TheosRenderPipeline::SourceDLSSG::ReflexModeName(sourceState.reflexSubmitted));
        DrawSettingsValue("Output cap",
                          sourceState.frameLimitSubmittedUs
                              ? std::format("{:.1f} FPS", 1000000.0 / sourceState.frameLimitSubmittedUs).c_str()
                              : "Off");
        DrawSettingsValue("Path", unlock.UsesTuringUnlock() ? "Turing MFG (experimental)"
                                  : unlock.UsesAmpereUnlock() ? "Ampere MFG (experimental)"
                                  : unlock.UsesAdaUnlock()  ? "Ada MFG"
                                                            : "Native NVIDIA");
        if (usableMaximum > 1)
            ImGui::Text("Available: x2 to x%u", usableMaximum + 1);
        else
            ImGui::TextUnformatted(usableMaximum == 1 ? "Available: x2" : "Availability: waiting");
        if (unlock.UsesCompatibilityUnlock() && !unlock.Ready())
            ImGui::TextWrapped("%s", unlock.status);
        if (sourceState.stateQueryResult == sl::Result::eWarnOutOfVRAM)
            ImGui::TextWrapped("NVIDIA VRAM budget warning");
        if (nvidiaHost->WarmupPresentsRemaining() > 0)
            ImGui::Text("Warmup: %d frames", nvidiaHost->WarmupPresentsRemaining());
        if (showDeveloperControls && ImGui::CollapsingHeader("Runtime details"))
        {
            ImGui::Text("Evaluations: %llu", static_cast<unsigned long long>(nvidiaHost->EvaluationCount()));
            ImGui::Text("Host Presents: %llu | failures %llu | last 0x%08X",
                        static_cast<unsigned long long>(nvidiaHost->PresentCount()),
                        static_cast<unsigned long long>(nvidiaHost->FailedPresentCount()),
                        static_cast<unsigned int>(nvidiaHost->LastPresentResult()));
            if (nvidiaHost->RuntimeStateObservationCount() > 0)
            {
                ImGui::Text("Last query: %u outputs | max generated %u | min dimension %u",
                            nvidiaHost->RuntimeFramesActuallyPresented(), nvidiaHost->RuntimeMaxGeneratedFrames(),
                            nvidiaHost->RuntimeMinWidthOrHeight());
                ImGui::Text("DLSS-G status: %u | observations: %llu", nvidiaHost->RuntimeDLSSGStatus(),
                            static_cast<unsigned long long>(nvidiaHost->RuntimeStateObservationCount()));
            }
            else
            {
                ImGui::TextDisabled("DLSS-G state: waiting");
            }
            ImGui::TextWrapped("%s", nvidiaHost->Status().c_str());
            if (view.sourceDLSSGActive)
            {
                ImGui::Text("Configured multiplier: x%u", sourceBackend.Snapshot().options.numFramesToGenerate + 1);
                ImGui::Text("Output limit submitted interval: %u us", sourceBackend.Snapshot().frameLimitSubmittedUs);
                ImGui::TextWrapped("MFG: %s", sourceBackend.MFGState().status);
            }

            ImGui::Text("Dynamic multiplier: %s", supportsDynamic ? "supported" : "unavailable");
        }
        DrawStageMeasurements(SettingsPage::FrameGeneration);
        }
        NextSettingsColumn(tabCardHeight);
        const bool runtimeInterpolationRequested=frameGen->RuntimeInterpolationRequested();
        if (runtimeInterpolationRequested != nvidiaHost->FrameGenerationEnabled())
        {
            ImGui::TextDisabled("Waiting for the current GPU frame to retire...");
        }

        if (!sourceDLSSGActive)
        {
            ImGui::TextWrapped("NVIDIA host is unavailable. Check runtime status and restart Skyrim.");
        }
        if (sourceDLSSGActive)
        {
            ImGui::TextUnformatted("Multiplier");
            auto& request = settingsDraft.sourceDLSSG.generation;
            const char* multipliers[]{"x2", "x3", "x4", "x5", "x6"};
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::BeginCombo("##sourceMultiplier", multipliers[std::clamp(request.generatedFrames, 1u, 5u) - 1]))
            {
                for (unsigned count = 1; count <= 5; ++count)
                {
                    ImGui::BeginDisabled(count > usableMaximum);
                    if (ImGui::Selectable(multipliers[count - 1], request.generatedFrames == count))
                    {
                        request.generatedFrames = count;
                    }
                    ImGui::EndDisabled();
                }
                ImGui::EndCombo();
            }
            if (supportsDynamic)
            {
                ImGui::Checkbox("Dynamic multiplier##sourceMFG", &request.dynamic);
            }
            else if (request.dynamic)
            {
                ImGui::TextWrapped("A saved dynamic request is unsupported by this NVIDIA runtime.");
                if (ImGui::Button("Use fixed multiplier##sourceMFG"))
                {
                    request.dynamic = false;
                }
            }
            if (request.dynamic)
            {
                int target = static_cast<int>(request.dynamicTargetFPS);
                ImGui::TextUnformatted("Target output FPS");
                if (TheosRenderPipeline::Overlay::FPSInput("##sourceMFGTarget", target))
                {
                    // Keep intermediate digits while typing 120, rather than
                    // replacing 1 and 12 with zero. Invalid edits leave active settings unchanged.
                    request.dynamicTargetFPS = static_cast<unsigned>(std::clamp(target, 0, 1000));
                }
                ImGui::TextDisabled("0 = display refresh rate; explicit targets must exceed 60 FPS.");
            }
            if (request.generatedFrames > sourceState.state.numFramesToGenerateMax || sourceState.generationLimited)
            {
                ImGui::TextWrapped("NVIDIA runtime maximum: x%u", sourceState.state.numFramesToGenerateMax + 1);
            }
            if (sourceState.generationLimited)
            {
                ImGui::TextWrapped("Saved/requested MFG is unsupported here. The runtime uses the supported count; "
                                   "your requested preference is retained.");
            }
            ImGui::Checkbox("UI recomposition##sourceDLSSG", &settingsDraft.sourceDLSSG.uiRecomposition);
            DrawSettingsHelp("Generates the scene and HUD separately to reduce HUD ghosting in motion. "
                             "Small GPU and VRAM cost. Changes apply automatically.");
            if (frameGenerationRuntimeActive && sourceState.uiRecompositionRequested)
            {
                if (sourceState.options.enableUserInterfaceRecomposition == sl::eTrue)
                    ImGui::TextDisabled("Submitted to DLSS-G");
                else
                    ImGui::TextWrapped("Waiting for HUD-less and UI layers");
            }
            ImGui::Separator();
            ImGui::TextUnformatted("NVIDIA Reflex");
            const auto requestedReflex = static_cast<sl::ReflexMode>(settingsDraft.sourceDLSSG.reflexMode);
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::BeginCombo("##sourceReflex", TheosRenderPipeline::SourceDLSSG::ReflexModeName(requestedReflex)))
            {
                for (const auto mode :
                     {sl::ReflexMode::eOff, sl::ReflexMode::eLowLatency, sl::ReflexMode::eLowLatencyWithBoost})
                {
                    if (ImGui::Selectable(TheosRenderPipeline::SourceDLSSG::ReflexModeName(mode),
                                          mode == requestedReflex))
                    {
                        settingsDraft.sourceDLSSG.reflexMode = static_cast<int>(mode);
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::TextUnformatted("Output FPS limit");
            TheosRenderPipeline::Overlay::FPSInput("##sourceReflexFPS", settingsDraft.sourceDLSSG.outputFPSLimit);
            settingsDraft.sourceDLSSG.outputFPSLimit = std::clamp(settingsDraft.sourceDLSSG.outputFPSLimit, 0, 1000);
            ImGui::TextDisabled("0 = no explicit cap");
            DrawSettingsHelp("The cap includes generated frames. Avoid stacking it with another limiter.");
        }

        DrawFrameGenerationAdvanced(view);
        EndSettingsColumns();
        ImGui::EndTabItem();
    }
}

void OverlayUI::DrawFrameGenerationAdvanced(const FrameView&)
{
    if (ImGui::CollapsingHeader("Advanced settings"))
    {
        DrawUIStatusPanel();
        DrawStageMeasurements(SettingsPage::Advanced);
        ImGui::Checkbox("Lab mode", &showDeveloperControls);
        DrawSettingsHelp("Show runtime details and experimental controls. This changes menu visibility only.");
        if (showDeveloperControls && !TheosRenderPipeline::CommunityShaders::Active() &&
            ImGui::CollapsingHeader("UI integration (Lab)"))
        {
            ImGui::TextDisabled("Save and restart");
            ImGui::Checkbox("Native-resolution Skyrim UI", &settingsDraft.nativeUI);
            ImGui::Checkbox("Startup overlays at native resolution", &settingsDraft.lateOverlayBridge);
        }
    }
}
